/*
This file is part of Linux Memory Analysis Tools.

Foobar is free software: you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation, either version 3 of the License, or (at your option) any later 
version.

Foobar is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with 
Linux Memory Analysis Tools. If not, see <https://www.gnu.org/licenses/>.
*/


#define _FILE_OFFSET_BITS 64
#define _LARGEFILE64_SOURCE

#include "kcore.h"

#include "colors.h"
#include "hex.h"
#include "lime.h"

#include <elf.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define CHUNK_SIZE 0x100000 // 1M

/**
 * Scans an individual memory region looking for a pattern.
 * 
 * @param kcore_fd      The file descriptor for /proc/kcore
 * @param pattern       The pattern to search for
 * @param context_bytes The number of bytes of context to include around a match
 * @param offset        The offset into kcore to scan 
 * @param size          The size of the memory region to scan
 * 
 * @return the number of matches found, or -1 if there's an error
 */
int scan_memory_region(int kcore_fd, 
                       const char* pattern, 
                       int context_bytes, 
                       uint64_t offset, 
                       size_t size)
{
    int matches = 0;
    int pattern_len = strlen(pattern);
    size_t buffer_size = pattern_len + (2 * context_bytes);
    unsigned char* buffer = malloc(buffer_size);
    if (NULL == buffer)
    {
        fprintf(stderr, "%sFailed to allocated buffer for scan!%s\n", COLOR_RED, 
            COLOR_CLEAR);
        return -1;
    }

    uint64_t file_pos = offset;
    size_t section_bounds = offset + size;
    while (file_pos <= section_bounds - pattern_len)
    {
        lseek(kcore_fd, context_bytes ? file_pos - context_bytes : 0, SEEK_SET);
        size_t read_size = read(kcore_fd, buffer, buffer_size);
        if (read_size < pattern_len)
        {
            break;
        }

        // Check for a pattern match
        if (memcmp(buffer + 
            (file_pos > context_bytes 
                ? context_bytes 
                : file_pos), pattern, pattern_len) 
            == 0)
        {
            size_t prefix = (file_pos > context_bytes ? context_bytes : file_pos);
            size_t suffix = (file_pos + pattern_len + context_bytes <= section_bounds)
                ? context_bytes : section_bounds - (file_pos + pattern_len);
            size_t total_size = prefix + pattern_len + suffix;

            // Display the match
            print_hex_and_ascii(buffer, total_size, 
                file_pos > context_bytes ? file_pos - context_bytes : 0, 
                pattern, pattern_len, 
                file_pos > context_bytes ? context_bytes : file_pos);

            matches++;
        }

        file_pos++;
    }

    return matches;
}

/**
 * Writes a memory region to an output file.asm
 * 
 * @param out_fd   The file descriptor of the output file
 * @param kcore_fd The file descriptor of the /proc/kcore file
 * @param len      The length of the memory region to write
 * 
 * @return 0 for success, else -1 if there's an error
 */
int write_memory_region(int out_fd, int kcore_fd, size_t len)
{
    size_t remaining = len;
    size_t next_chunk;
    int have_read, written;
    char* buffer = malloc(CHUNK_SIZE);
    if (NULL == buffer)
    {
        // Shouldn't happen...
        fprintf(stderr, "%sFailed to malloc buffer%s\n", COLOR_RED, COLOR_CLEAR);
        return -1;
    }

    while (remaining)
    {
        if (remaining > CHUNK_SIZE)
        {
            next_chunk = CHUNK_SIZE;
        }
        else
        {
            next_chunk = remaining;
        }

        have_read = read(kcore_fd, buffer, next_chunk);
        if (-1 == have_read)
        {
            fprintf(stderr, "%sKcore read failed!%s\n", COLOR_RED, COLOR_CLEAR);
            free(buffer);
            return -1;
        }

        written = write(out_fd, buffer, have_read);
        if (-1 == written)
        {
            fprintf(stderr, "%sFailed to write memory region!%s\n", COLOR_RED, 
                COLOR_CLEAR);
            free(buffer);
            return -1;
        }

        remaining -= written;
    }

    free(buffer);
    return 0;
}

/**
 * Writes the LiME headers (and associated memory regions) to an output file.
 * 
 * @param kcore_fd   The file descriptor of the /proc/kcore file
 * @param out_fd     The file descriptor for the output file
 * @param sections   The array of memory sections to write
 * @param num_ranges The number of memory ranges to write
 * 
 * @return 0 for success, else -1 if there's an error
 */
int write_lime(int kcore_fd,
               int out_fd,
               struct section* sections,
               int num_ranges)
{
    // Setup the basic header structure
    lime_memory_range_header lime_header;
    lime_header.magic = LIME_HEADER_MAGIC;
    lime_header.version = LIME_HEADER_VERSION;
    memset(&lime_header.reserved, 0x00, 8);

    // Write out each memory region
    for (int i = 0; i < num_ranges; i++)
    {
        lime_header.s_addr = sections[i].physical_base;
        lime_header.e_addr = sections[i].physical_base + sections[i].size - 1;

        // Write the LiME memory range header
        if (sizeof(lime_memory_range_header) != 
            write(out_fd, &lime_header, sizeof(lime_memory_range_header)))
        {
            fprintf(stderr, "%sError writing file header (errno %d)%s\n", 
                COLOR_RED, errno, COLOR_CLEAR);
            return -1;
        }

        printf("%sCopying section %d (0x%lx - 0x%lx)%s\n", COLOR_CYAN, i, 
            lime_header.s_addr, lime_header.e_addr, COLOR_CLEAR);

        // Copy over the actual memory content
        off64_t pos = lseek64(kcore_fd, sections[i].file_offset, SEEK_SET);
        if (-1 == pos)
        {
            fprintf(stderr, "%sError setting position in kcore (errno %d)%s\n", 
                COLOR_RED, errno, COLOR_CLEAR);
            return -1;
        }

        if (write_memory_region(out_fd, kcore_fd, sections[i].size) != 0)
        {
            fprintf(stderr, "%sError writing data (errno %d)%s\n", COLOR_RED, 
                errno, COLOR_CLEAR);
            return -1;
        }
    }

    return 0;
}

/**
 * Scans the /proc/kcore file looking for a pattern.asm
 * 
 * @param kcore_fd      The file descriptor of the /proc/kcore file
 * @param pattern       The pattern to search for
 * @param context_bytes The number of bytes of context to include around a match
 * @param sections      The memory sections to scan
 * @param num_ranges    The number of memory ranges to scan
 * 
 * @return The total number of matches, else -1 if there's an error
 */
int scan_kcore(int kcore_fd,
               const char* pattern,
               int context_bytes,
               struct section* sections,
               int num_ranges)
{
    int total_matches = 0;

    // Scan each memory region
    for (int i = 0; i < num_ranges; i++)
    {
        uint64_t s_addr = sections[i].physical_base;
        uint64_t e_addr = sections[i].physical_base + sections[i].size - 1;
        printf("%sScanning section %d (0x%lx - 0x%lx) for %s%s\n", COLOR_CYAN, 
            i, s_addr, e_addr, pattern, COLOR_CLEAR);

        // Perform the scan
        int section_matches = scan_memory_region(kcore_fd, pattern, 
            context_bytes, sections[i].file_offset, sections[i].size);
        if (-1 == section_matches)
        {
            fprintf(stderr, "%sFailed to scan memory region %d%s\n", 
                COLOR_RED, i, COLOR_CLEAR);
            return -1;
        }

        total_matches += section_matches;
    }

    return total_matches;
}

/**
 * Dumps the system's RAM from the /proc/kcore file to disk.asm
 * 
 * @param kcore_fd   The file descriptor for /proc/kcore
 * @param out_fd     The file descriptor for the output file
 * @param sections   The array of memory sections to dump to disk
 * @param num_ranges The number of memory ranges to dump
 * 
 * @return 0 for success, else -1 if there's an error
 */
int dump_kcore(int kcore_fd, 
               int out_fd, 
               struct section* sections, 
               int num_ranges)
{
    return write_lime(kcore_fd, out_fd, sections, num_ranges);
}

/**
 * Attempts to associate the memory ranges from /proc/iomem to the ranges
 * present in the headers of /proc/kcore. 
 * 
 * @param prog_hdr            The program headers from /proc/kcore
 * @param num_hdrs            The number of headers from kcore
 * @param ranges              The memory ranges fround from iomem
 * @param num_physical_ranges The number of ranges from iomem
 * @param sections            The memory sections found (output)
 * 
 * @return The number of associated sections
 */
int match_physical_addresses_to_phdrs(Elf64_Phdr* prog_hdr,
                                      unsigned int num_hdrs,
                                      struct addr_range* ranges,
                                      unsigned int num_physical_ranges,
                                      struct section* sections)
{
    int filled_sections = 0;

    printf("Attempting to associate memory ranges from %s with headers from %s\n", 
        IOMEM_FILENAME, KCORE_FILENAME);

    for (int i = 0; i < num_hdrs; i++)
    {
        for (int j = 0; j < num_physical_ranges; j++)
        {
            if (prog_hdr[i].p_paddr == ranges[j].start)
            {
                sections[filled_sections].physical_base = ranges[j].start;
                sections[filled_sections].file_offset = prog_hdr[i].p_offset;
                sections[filled_sections].size = prog_hdr[i].p_memsz;

                filled_sections++;
            }
        }
    }

    return filled_sections;
}