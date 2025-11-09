/*
This file is part of Linux Memory Dumper.

Linux Memory Dumper is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

Linux Memory Dumper is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
Linux Memory Dumper. If not, see <https://www.gnu.org/licenses/>.
*/


#define _FILE_OFFSET_BITS 64
#define _LARGEFILE64_SOURCE

#include "kcore.h"

#include "color-print.h"
#include "lime.h"

#include <elf.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define CHUNK_SIZE 0x100000 // 1M

/**
 * Writes a memory region to an output file.asm
 * 
 * @param out_fd   The file descriptor of the output file
 * @param kcore_fd The file descriptor of the /proc/kcore file
 * @param len      The length of the memory region to write
 * 
 * @return 0 for success, else -1 if there's an error
 */
static int write_memory_region(const int out_fd, 
                               const int kcore_fd, 
                               const size_t len)
{
    size_t remaining = len;
    size_t next_chunk;
    int have_read, written;
    char* buffer = malloc(CHUNK_SIZE);
    if (NULL == buffer)
    {
        // Shouldn't happen...
        fprint_red(stderr, "[-] Failed to malloc buffer\n");
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
            fprint_red(stderr, "[-] Kcore read failed!\n");
            free(buffer);
            return -1;
        }

        written = write(out_fd, buffer, have_read);
        if (-1 == written)
        {
            fprint_red(stderr, "[-] Failed to write memory regions!\n");
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
static int write_lime(const int kcore_fd,
                      const int out_fd,
                      const struct section* sections,
                      const int num_ranges)
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
            fprint_red(stderr, "[-] Error writing file header (errno %d)\n", errno);
            return -1;
        }

        print_cyan("\t[*] Copying section %d (0x%lx - 0x%lx)\n", 
            i, lime_header.s_addr, lime_header.e_addr);

        // Copy over the actual memory content
        off64_t pos = lseek64(kcore_fd, sections[i].file_offset, SEEK_SET);
        if (-1 == pos)
        {
            fprint_red(stderr, "[-] Error setting position in kcore (errno %d)\n", 
                errno);
            return -1;
        }

        if (write_memory_region(out_fd, kcore_fd, sections[i].size) != 0)
        {
            fprint_red(stderr, "[-] Error writing data (errno %d)\n", errno);
            return -1;
        }
    }

    return 0;
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
int match_physical_addresses_to_phdrs(const Elf64_Phdr* prog_hdr,
                                      const unsigned int num_hdrs,
                                      const struct addr_range* ranges,
                                      const unsigned int num_physical_ranges,
                                      struct section* sections)
{
    int filled_sections = 0;

    print_green("[*] Attempting to associate memory ranges from %s with headers from %s\n", 
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