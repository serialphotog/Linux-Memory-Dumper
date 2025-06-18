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


#include "hex.h"

#include "colors.h"

#include <ctype.h>
#include <stdio.h>

/**
 * Gets the ASCII representation of a character, or a printable representation
 * if the character is not a printable character.
 * 
 * @param c The character to get the representation of. 
 * 
 * @return The character or a representation if it's non-printable.
 */
char get_ascii_representation(const char c)
{
    return isprint(c) ? c : UNPRINTABLE_ASCII;
}

/**
 * Simple helper that prints the header for a match. 
 * 
 * @param offset The offset in kcore where the match was found
 * @param pos    The position of the match
 */
void print_match_header(const long offset, const size_t pos)
{
    printf("\nMatch at offset 0x%lx\n", offset + pos);
    printf("     Offset    | Hexadecimal");
    for (int i = 0; i < HEX_HEADER_PADDING; i++)
    {
        printf(" ");
    }
    printf("| ASCII\n");
    for (int i = 0; i < HEADER_DELIM_NUM; i++)
    {
        printf("-");
    }
    printf("\n");
}

/**
 * Prints both the hex and ASCII representations of a match, including the
 * specified number of bytes of context. 
 * 
 * @param data        The data containing the match and the context
 * @param size        The size of the match region
 * @param offset      The offset where the match was found 
 * @param pattern     The pattern that was matched 
 * @param pattern_len The length of the pattern string
 * @param match_os    The position of the match
 */
void print_hex_and_ascii(const unsigned char* data, 
                         const size_t size, 
                         const long offset, 
                         const char* pattern, 
                         const size_t pattern_len, 
                         const size_t match_pos)
{
    print_match_header(offset, match_pos);

    // Print the hex output
    for (size_t i = 0; i < size; i += 16)
    {
        // Offset
        printf("%s0x%08lx%s | ", COLOR_CYAN, offset + i, COLOR_CLEAR);

        // Hex value
        for (size_t j = i; j < i + 16; j++) 
        {
            if (j < size)
            {
                if (j >= match_pos && j < match_pos + pattern_len)
                {
                    // This byte is a part of the pattern
                    printf("%s%02x%s ", COLOR_RED, data[j], COLOR_CLEAR);
                }
                else 
                {
                    // Not a part of the pattern, just a context byte
                    printf("%02x ", data[j]);
                }
            }
            else 
            {
                printf("   ");
            }
        }

        printf("| ");

        // Print the ASCII value
        for (size_t j = i; j < i + 16 && j < size; j++)
        {
            if (j >= match_pos && j < match_pos + pattern_len)
            {
                // This is part of the match
                printf("%s%c%s", COLOR_RED, get_ascii_representation(data[j]), 
                    COLOR_CLEAR);
            }
            else 
            {
                // Not a part of the pattern, just context
                printf("%c", get_ascii_representation(data[j]));
            }
        }

        printf("\n");
    }

    printf("\n");
}