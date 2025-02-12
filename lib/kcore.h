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

#include <elf.h>

#include "lmat.h"

int dump_kcore(int kcore_fd, 
               int out_fd, 
               struct section* sections, 
               int num_ranges);

int scan_kcore(int kcore_fd, 
               const char* pattern, 
               int context_bytes, 
               struct section* sections, 
               int num_ranges);

int match_physical_addresses_to_phdrs(Elf64_Phdr* prog_hdr,
                                      unsigned int num_hdrs,
                                      struct addr_range* ranges,
                                      unsigned int num_physical_ranges,
                                      struct section* sections);