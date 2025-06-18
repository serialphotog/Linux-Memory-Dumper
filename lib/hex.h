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

#include <stddef.h>

// Character to use to represent an unprintable ASCII character
#define UNPRINTABLE_ASCII '.'

// Various padding values for the output
#define HEADER_DELIM_NUM 83
#define HEX_HEADER_PADDING 37

void print_hex_and_ascii(const unsigned char* data, 
                         const size_t size, 
                         const long offset, 
                         const char* pattern, 
                         const size_t pattern_len, 
                         const size_t match_pos);