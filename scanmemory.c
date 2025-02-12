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

#include "lib/lmat.h"

#include "lib/colors.h"
#include "lib/iomem.h"
#include "lib/kcore.h"

#include <elf.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    int ret = 0;
    int kcore_fd;

    // The program expects two arguments: The pattern to search for and the 
    // number of bytes of context to include around matches
    if (argc < 3)
    {
        printf("Usage: %s <pattern> <bytes_of_context>\n", argv[0]);
        ret = -1;
        goto cleanup;
    }
    const char* pattern = argv[1];
    int context_bytes = atoi(argv[2]);

    // We will require root privileges to scan kcore
    if (0 != getuid())
    {
        printf("%sThis program requires root privileges to function!%s\n", 
            COLOR_RED, COLOR_CLEAR);
            ret = -1;
            goto cleanup;
    }

    // Obtain a handle to /proc/kcore
    if (-1 == (kcore_fd = open64(KCORE_FILENAME, O_RDWR | O_LARGEFILE)))
    {
        fprintf(stderr, "%sCould not open %s%s\n", COLOR_RED, KCORE_FILENAME, 
            COLOR_CLEAR);
            ret = -1;
            goto cleanup;
    }

    // Get the physical memory ranges from iomem
    struct addr_range ranges[MAX_PHYSICAL_RANGES];
    int num_physical_ranges = get_system_ram_address_ranges(ranges);
    if (-1 == num_physical_ranges)
    {
        fprintf(stderr, "%sFailed to get physical memory ranges%s\n", 
            COLOR_RED, COLOR_CLEAR);
            ret = -1;
            goto cleanup;
    }

    // Get the ELF headers from kcore
    Elf64_Ehdr elf_hdr;
    read(kcore_fd, (void*)&elf_hdr, sizeof(elf_hdr));

    // Get the program headers from kcore
    lseek(kcore_fd, elf_hdr.e_phoff, SEEK_SET);
    size_t phdrs_size = elf_hdr.e_phnum * elf_hdr.e_phentsize;
    Elf64_Phdr* prog_hdr = (Elf64_Phdr*) malloc(phdrs_size);
    if (NULL == prog_hdr)
    {
        fprintf(stderr, "%sFailed to get program headers from kcore%s\n", 
            COLOR_RED, COLOR_CLEAR);
            ret = -1;
            goto cleanup;
    }
    read(kcore_fd, (void*)prog_hdr, phdrs_size);

    // Map the physical address ranges from iomem to the headers from kcore
    struct section sections[MAX_PHYSICAL_RANGES];
    match_physical_addresses_to_phdrs(prog_hdr, elf_hdr.e_phnum, ranges, 
        num_physical_ranges, sections);

    // Scan kcore for pattern
    int matches = scan_kcore(kcore_fd, pattern, context_bytes, sections, 
        num_physical_ranges);
    if (-1 == matches)
    {
        fprintf(stderr, "%sFailed to scan kcore for %s%s\n", COLOR_RED, 
            pattern, COLOR_CLEAR);
        ret = -1;
        goto cleanup;
    }

    if (0 == matches)
    {
        printf("%sNo matches found for %s%s\n", COLOR_GREEN, pattern, 
            COLOR_CLEAR);
    }
    else
    {
        printf("%sFound %d total matches for %s%s\n", COLOR_YELLOW, matches, 
            pattern, COLOR_CLEAR);
    }

    // Cleanup
cleanup:
    if (kcore_fd >= 0)
    {
        close(kcore_fd);
        kcore_fd = -1;
    }

    return ret;
}