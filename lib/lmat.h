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


#pragma once

#include <unistd.h>
#include <stdint.h>

// The path to kcore file on disk
#define KCORE_FILENAME "/proc/kcore"

// The path to the iomem file on disk
#define IOMEM_FILENAME "/proc/iomem"

// The maximum number of physical address ranges we support
#define MAX_PHYSICAL_RANGES 32

// The label that's associated with system RAM in iomem
#define SYSTEM_RAM_LABEL "System RAM"

// Represents a memory address range
struct addr_range
{
    int         index;
    uint64_t    start;
    uint64_t    end;
};

// Represents a section of memory
struct section 
{
    uint64_t    physical_base;
    uint64_t    file_offset;
    size_t      size;
};