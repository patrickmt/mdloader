/*
 *  Copyright (C) 2018  Massdrop Inc.
 *
 *  This file is part of Massdrop Loader.
 *
 *  Massdrop Loader is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Massdrop Loader is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Massdrop Loader.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _MDLOADER_PARSER_H
#define _MDLOADER_PARSER_H

#define EXT_HEX ".hex"
#define EXT_BIN ".bin"

#define FTYPE_NONE  0   //No file type
#define FTYPE_HEX   1   //HEX file type detected
#define FTYPE_BIN   2   //BIN file type detected

typedef struct data_s {
    uint32_t addr;
    uint32_t size;
    char *data;
} data_t;

void free_data(data_t *data);
data_t *create_data(uint32_t data_length);
data_t *load_file(char *fname);

#pragma pack(push, 1)
typedef struct hex_record_s {
    char start_code;        //:
    char byte_count[2];     //XX
    union {
        uint32_t i;
        uint8_t c[4];       //XXXX
    } address;
    char record_type[2];    //XX
    //<- variable length ASCII data matching byte_count*2 ->
    //XX checksum
} hex_record_t;
#pragma pack(pop)

#endif //_MDLOADER_PARSER_H
