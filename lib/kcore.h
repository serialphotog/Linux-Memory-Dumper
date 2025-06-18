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

#include <elf.h>

#include "lmat.h"

int dump_kcore(int kcore_fd, 
               int out_fd, 
               struct section* sections, 
               int num_ranges);

int scan_kcore(const int kcore_fd, 
               const char* pattern, 
               const int context_bytes, 
               struct section* sections, 
               const int num_ranges);

int match_physical_addresses_to_phdrs(const Elf64_Phdr* prog_hdr,
                                      const unsigned int num_hdrs,
                                      const struct addr_range* ranges,
                                      const unsigned int num_physical_ranges,
                                      struct section* sections);