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


#pragma once

#include <stdint.h>

#define LIME_HEADER_MAGIC 0x4C694D45
#define LIME_HEADER_VERSION 1

// Taken from https://github.com/504ensicsLabs/LiME/blob/master/doc/README.md
typedef struct 
{
    unsigned int magic;     // Always 0x4C694D45 (LiME)
    unsigned int version;   // Header version number
    uint64_t s_addr;        // Starting address of physical RAM range
    uint64_t e_addr;        // Ending address of physical RAM range
    unsigned char reserved[8]; // Reserved space
} __attribute__ ((__packed__)) lime_memory_range_header;