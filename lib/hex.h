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

#include <stddef.h>

// Character to use to represent an unprintable ASCII character
#define UNPRINTABLE_ASCII '.'

// Various padding values for the output
#define HEADER_DELIM_NUM 83
#define HEX_HEADER_PADDING 37

void print_hex_and_ascii(const unsigned char* data, 
                         size_t size, 
                         long offset, 
                         const char* pattern, 
                         size_t pattern_len, 
                         size_t match_pos);