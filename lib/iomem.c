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

#include "colors.h"

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
    FILE* iomem_fd;
    size_t n = LINE_SIZE;
    int count = 0;

    char* lineptr = malloc(LINE_SIZE);
    if (NULL == lineptr)
    {
        fprintf(stderr, "%sFailed to allocate memory for line buffer%s\n", 
            COLOR_RED, COLOR_CLEAR);
        return -1;
    }

    if (NULL == (iomem_fd = fopen(IOMEM_FILENAME, "r")))
    {
        fprintf(stderr, "%sCould not open %s%s\n", COLOR_RED, IOMEM_FILENAME, 
            COLOR_CLEAR);
        return -1;
    }

    printf("Scanning %s for physical memory regions\n", IOMEM_FILENAME);

    int index = 0;
    while (getline(&lineptr, &n, iomem_fd) != -1)
    {
        if (strstr(lineptr, SYSTEM_RAM_LABEL))
        {
            uint64_t start, end;
            sscanf(lineptr, "%lx-%lx", &start, &end);

            addrs[count].index = index;
            addrs[count].start = start;
            addrs[count].end = end;

            if (++count >= MAX_PHYSICAL_RANGES)
            {
                fprintf(stderr, "%sToo many physical memory ranges encountered%s\n", 
                    COLOR_RED, COLOR_CLEAR);
                return -1;
            }
        }

        if (lineptr[0] != ' ')
        {
            index++;
        }
    }

    fclose(iomem_fd);
    free(lineptr);

    return count;
}