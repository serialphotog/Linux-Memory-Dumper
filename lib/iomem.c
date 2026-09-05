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


#include "iomem.h"

#include "color-print.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_SIZE 512

/**
 * Parses the /proc/iomem file to determine the address ranges of the physical
 * ram. 
 * 
 * @param addrs The array of address ranges that was found 
 * 
 * @return The number of RAM regions found, or -1 if there was an error
 */
int get_system_ram_address_ranges(struct addr_range* addrs)
{
    FILE* iomem_fd = NULL;
    char* lineptr = NULL;
    size_t n = LINE_SIZE;
    int count = 0;
    int ret = -1;

    lineptr = malloc(LINE_SIZE);
    if (NULL == lineptr)
    {
        fprint_red(stderr, "[-] Failed to allocate memory for line buffer\n");
        goto cleanup;
    }

    if (NULL == (iomem_fd = fopen(IOMEM_FILENAME, "r")))
    {
        fprint_red(stderr, "[-] Could not open %s\n", IOMEM_FILENAME);
        goto cleanup;
    }

    print_green("[*] Scanning %s for physical memory regions\n", IOMEM_FILENAME);

    int index = 0;
    while (getline(&lineptr, &n, iomem_fd) != -1)
    {
        if (strstr(lineptr, SYSTEM_RAM_LABEL))
        {
            uint64_t start, end;
            if (sscanf(lineptr, "%lx-%lx", &start, &end) != 2)
            {
                fprint_red(stderr, "[-] Could not parse iomem line: %s\n", lineptr);
                goto cleanup;
            }

            if (count >= MAX_PHYSICAL_RANGES)
            {
                fprint_red(stderr, "[-] Too many physical memory regions encountered: [%d]\n", count);
                goto cleanup;
            }

            addrs[count].index = index;
            addrs[count].start = start;
            addrs[count].end = end;
            count++;
        }

        if (lineptr[0] != ' ')
        {
            index++;
        }
    }

    ret = count;

cleanup:
    if (NULL != iomem_fd)
    {
        fclose(iomem_fd);
    }
    free(lineptr);
    return ret;
}