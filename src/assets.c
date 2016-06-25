/*  DesktopAdventures, A reimplementation of the Desktop Adventures game engine
 *
 *  DesktopAventures is the legal property of its developers, whose names
 *  can be found in the AUTHORS file distributed with this source
 *  distribution.
 *
 *  DesktopAdventures is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see <http://www.gnu.org/licenses/>
 */

#include "assets.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/gl.h>
#include "map.h"
#include "main.h"
#include "tile.h"
#include "sound.h"
#include "tname.h"
#include "puzzle.h"
#include "screen.h"
#include "useful.h"
#include "palette.h"
#include "character.h"

u8* yodesk;
long yodesk_size = 0;

u8 ASSETS_LOADING = 1;
u16 NUM_MAPS = 0;

u32 version = 4;
u32* tile;

GLuint texture[0x2001];
u32 tile_metadata[0x2000];
izon_data **zone_data;
//TNAME **tile_names;

u32 yodesk_seek = 0;

u8 load_demo = 0;
u8 is_yoda = 1;
u16 ipuznum = 0;

void load_resources()
{
    char *file_to_load = is_yoda ? (load_demo ? "YodaDemo.dta" : "YODESK.DTA") : "DESKTOP.DAW";
    yodesk = readFileToBytes(file_to_load, &yodesk_size);
    printf("%s loaded, %lx bytes large\n", file_to_load, yodesk_size);

    u16 izon_count = 0;
    while(1)
    {
        u32 tag_seek = get_location();
        char *tag = get_strn(4);
        if(!strncmp(tag, "VERS", 4)) //VERSion
        {
            printf("Found VERS at %x, version number %x\n", tag_seek, read_long());
        }
        else if(!strncmp(tag, "STUP", 4)) //STartUP Graphic, uses yodesk_palette
        {
            printf("Found STUP at %x\n", tag_seek);
            load_texture(288, current_file_pointer()+sizeof(u32), 0x2000); //Load Startup Texture past the last tile

            u32 len = read_long();
            seek(len+sizeof(u64)+tag_seek);
            draw_STUP();
        }
        else if(!strncmp(tag, "ZONE", 4)) //ZONEs (maps)
        {
            printf("Found ZONE at %x\n", tag_seek);

            u32 ZONE_LENGTH;

            if(is_yoda)
            {
                NUM_MAPS = read_short();
                u16 unknown = read_short();
                ZONE_LENGTH = read_long();
            }
            else
            {
                ZONE_LENGTH = read_long();
                NUM_MAPS = read_short();
            }
            zone_data = malloc(NUM_MAPS * sizeof(char*));
            for(int j = 0; j < NUM_MAPS; j++)
                zone_data[j] = (izon_data*)calloc(sizeof(izon_data), sizeof(u8));

            printf("%i maps in DAT\n", NUM_MAPS);

            seek(tag_seek+sizeof(u32)+sizeof(u32)+sizeof(u32)+sizeof(u16));
        }
        else if(!strncmp(tag, "IZON", 4)) //Internal ZONe? (map)
        {
            izon_count++;
            printf("Found IZON %i at %x\n", izon_count-1, tag_seek);
            zone_data[izon_count-1]->izon_offset = tag_seek;

            u32 len = read_long();
            seek(tag_seek+len);
        }
        else if(!strncmp(tag, "IZAX", 4)) //IZAX
        {
            printf("Found IZAX at %x\n", tag_seek);
            zone_data[izon_count-1]->izax_offset = tag_seek;

            u32 len = read_long();
            seek(tag_seek+len);
        }
        else if(!strncmp(tag, "IZX2", 4)) //IZX2
        {
            printf("Found IZX2 at %x\n", tag_seek);
            zone_data[izon_count-1]->izx2_offset = tag_seek;

            u32 len = read_long();
            seek(tag_seek+len);
        }
        else if(!strncmp(tag, "IZX3", 4)) //IZX3
        {
            printf("Found IZX3 at %x\n", tag_seek);
            zone_data[izon_count-1]->izx3_offset = tag_seek;

            u32 len = read_long();
            seek(tag_seek+len);
        }
        else if(!strncmp(tag, "IZX4", 4)) //IZX4
        {
            printf("Found IZX4 at %x\n", tag_seek);
            zone_data[izon_count-1]->izx4_offset = tag_seek;

            u32 len = read_long();
            seek(tag_seek+len);
        }
        else if(!strncmp(tag, "IACT", 4)) //IACT
        {
            if(zone_data[izon_count-1]->iact_offset == 0)
            {
                zone_data[izon_count-1]->num_iacts = read_prefix();
                zone_data[izon_count-1]->iact_offset = tag_seek;
                read_iact_stats((u16)(izon_count-1), zone_data[izon_count-1]->iact_offset, zone_data[izon_count-1]->num_iacts);
                printf("Found %u IACT%s at %x\n", zone_data[izon_count-1]->num_iacts, (zone_data[izon_count-1]->num_iacts > 1 && zone_data[izon_count-1]->num_iacts != 0 ? "s" : ""), tag_seek);
            }

            seek(tag_seek+sizeof(u32));
            u32 len = read_long();
            seek(tag_seek+len+0x8);
        }
        else if(!strncmp(tag, "SNDS", 4)) //SouNDS
        {
            printf("Found SNDS at %x\n", tag_seek);

            u32 length = read_long();
            u16 unk1 = read_short();
            sound_files = malloc(256 * sizeof(char*));

            for(int j = 0; (get_location() - tag_seek) < (length - 2); j++)
            {
                u32 str_length = read_short();
                sound_files[j] = get_strn(str_length);
                printf("%x: %x %s\n", j, str_length, sound_files[j]);
            }
            seek(tag_seek+length+0x8);
        }
        else if(!strncmp(tag, "TILE", 4)) //TILEs (graphics)
        {
            printf("Found TILE at %x\n", tag_seek);
            int section_length = read_long();
            printf("0x%x tiles in TILES\n", section_length / ((32*32)+4));
            for(u32 j = 0; j < section_length / ((32*32)+4); j++)
            {
                if(j % 0x100 == 0 || j == (section_length / ((32*32)+4))-1)
                    printf("%x of %x...\n", j, section_length / ((32*32)+4));

                u32 tile_stuff = read_long();
                tile_metadata[j] = tile_stuff;
                load_texture(32, current_file_pointer(), j);
                seek_add(32*32*sizeof(u8));
            }
            seek(tag_seek+section_length+0x8);
        }
        else if(!strncmp(tag, "PUZ2", 4)) //Puzzle configurations maybe?
        {
            printf("Found PUZ2 at %x\n", tag_seek);
            u32 length = read_long();
            ipuz_data = malloc(512 * sizeof(char*));

            seek_add(sizeof(u16));
        }
        else if(!strncmp(tag, "IPUZ", 4)) //Puzzle string
        {
            printf("Found IPUZ at %x\n", tag_seek);
            u16 id = read_prefix();

            ipuz_element *e = malloc(sizeof(ipuz_element));
            e->size = read_long();
            e->unk1 = read_long();
            e->unk2 = read_long();
            if(is_yoda)
            {
                e->unk3 = read_long();
            }
            e->unk4 = read_short();

            e->string1_len = read_short();
            for(u16 j = 0; j < e->string1_len; j++)
                e->string1[j] = read_byte();

            e->string2_len = read_short();
            for(u16 j = 0; j < e->string2_len; j++)
                e->string2[j] = read_byte();

            e->string3_len = read_short();
            for(u16 j = 0; j < e->string3_len; j++)
                e->string3[j] = read_byte();

            e->string4_len = read_short();
            for(u16 j = 0; j < e->string4_len; j++)
                e->string4[j] = read_byte();

            e->unused_len = read_short();
            for(u16 j = 0; j < e->unused_len; j++)
                e->unused[j] = read_byte();

            e->item_a = read_short();

            if(is_yoda)
            {
                e->item_b = read_short();
            }

            ipuz_data[id] = e;
            ipuznum++;

            seek_add(e->size+0xA);
        }
        else if(!strncmp(tag, "CHAR", 4))
        {
            u32 size = read_long();
            printf("Found CHAR at %x, size %x\n", tag_seek, size);

            char_data = malloc((size / 0x54) * sizeof(char*));

            for(int j = 0; j < (size / 0x54); j++)
            {
                u16 id = read_short();
                char_data[id] = (ichr_data*)(current_file_pointer());
                printf("%x - %-16s %x %x %x %x\n", id, char_data[id]->name, char_data[id]->unk_1, char_data[id]->flags, char_data[id]->unk_4, char_data[id]->unk_5);
                seek_add((u32)(is_yoda ? 0x54 : 0x4E) - 2);
            }
            seek(tag_seek+size+8);
        }
        else if(!strncmp(tag, "CHWP", 4))
        {
            printf("Found CHWP at %x\n", tag_seek);

            u32 len = read_long();

            chwp_data = malloc(((len/sizeof(chwp_entry))+1) * sizeof(chwp_entry*));

            u16 entry_index = 0;
            while(1)
            {
                chwp_data[entry_index++] = (chwp_entry*)(current_file_pointer());

                u16 id_1 = read_short();
                u16 id_2 = read_short();
                u16 health = read_short();

                if(id_1 == 0xFFFF)
                    break;

                if(char_data[id_1]->flags & ICHR_IS_WEAPON)
                    printf("%-16s is a weapon with sound %-14s, health %x?\n", char_data[id_1]->name, sound_files[id_2], health);
                else
                    printf("%-16s gets weapon %-25s, health %x\n", char_data[id_1]->name, id_2 == 0xFFFF ? "none" : char_data[id_2]->name, health);
            }
            seek(tag_seek+len+8);
        }
        else if(!strncmp(tag, "CAUX", 4))
        {
            printf("Found CAUX at %x\n", tag_seek);

            u32 len = read_long();

            caux_data = malloc(((len/sizeof(caux_entry))+1) * sizeof(caux_entry*));

            u16 entry_index = 0;
            while(1)
            {
                caux_data[entry_index++] = (caux_entry*)(current_file_pointer());

                u16 id_1 = read_short();
                u16 damage = read_short();

                if(id_1 == 0xFFFF)
                    break;

                if(char_data[id_1]->flags & ICHR_IS_WEAPON)
                    printf("%-16s is a weapon,          damage: %x\n", char_data[id_1]->name, damage);
                else
                    printf("%-16s not a weapon, ambient damage: %x\n", char_data[id_1]->name, damage);
            }
            seek(tag_seek+len+8);
        }
        else if (!strncmp(tag, "ANAM", 4)) //Tile names
        {
            printf("Found ANAM at %x\n", tag_seek);
        }
        else if (!strncmp(tag, "PNAM", 4)) //Tile names
        {
            printf("Found PNAM at %x\n", tag_seek);
        }
        else if(!strncmp(tag, "TNAM", 4)) //Tile names
        {
            printf("Found TNAM at %x\n", tag_seek);

            u32 len = read_long();

            char *nop = "NO NAME";

            for(int j = 0; j < 0x10000; j++)
                tile_names[j] = nop;

            for(int j = 0; j < len / (is_yoda ? 26 : 18); j++)
            {
                u16 id = read_short();
                char *name;
                if (id == 0xFFFF)
                    name = nop;
                else
                    name = current_file_pointer();

                tile_names[id] = name;

                //printf("%x, %s\n", id, tile_names[id]);

                if (is_yoda)
                    seek_add(24);
                else
                    seek_add(16);
            }
            seek(tag_seek+len+8);
        }
        else if(!strncmp(tag, "ENDF", 4))
        {
            //print_iact_stats();
            for(int j = 0; j < ipuznum; j++)
            {
                ipuz_element *e = ipuz_data[j];
                if(e == 0)
                    continue;

                //if(is_yoda)
                //	printf("%x: %x %x %x %x \"%s\" \"%s\" \"%s\" \"%s\", %s (%x), %s (%x)\n", j, e->unk1, e->unk2, e->unk3, e->unk4, e->string1, e->string2, e->string3, e->string4, tile_names[e->item_a], e->item_a, tile_names[e->item_b], e->item_b);
                //else
                //	printf("%x: %x %x %x \"%s\" \"%s\" \"%s\" \"%s\", %s (%x)\n", j, e->unk1, e->unk2, e->unk4, e->string1, e->string2, e->string3, e->string4, tile_names[e->item_a], e->item_a);
            }
            printf("Found ENDF at %x\n", tag_seek);
            break;
        }
        else
        {
            seek_sub(sizeof(u32)-sizeof(u8));
        }
    }

    load_map(0);
    init_screen();
    render_map();
    ASSETS_LOADING = 0;
}

void load_texture(u16 width, u8 *data, u32 texture_num)
{
    u32 *data_buffer = malloc((size_t)(width * width * 4));
    int index = 0;
    for(int i = 0; i < width * width; i++)
    {
        int color_index = data[index];
        u32 color;
        if(is_yoda)
            color = ((u8)(yodesk_palette[(color_index * 4)]) << 16) + ((u8)(yodesk_palette[(color_index * 4) + 1]) << 8) + ((u8)(yodesk_palette[(color_index * 4) + 2]) << 0);
        else
            color = ((u8)(indy_palette[(color_index * 4)]) << 16) + ((u8)(indy_palette[(color_index * 4) + 1]) << 8) + ((u8)(indy_palette[(color_index * 4) + 2]) << 0);

        if(color_index != 0)
            color |= 0xFF000000; //Make sure it's not transparent

        data_buffer[(width * width) - i - 1] = color;
        index++;
    }
    glGenTextures(0x1, &texture[texture_num]);
    glBindTexture( GL_TEXTURE_2D, texture[texture_num]);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_buffer);

#ifdef PC_BUILD
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#endif
}

void seek(u32 location)
{
    yodesk_seek = location;
}

void seek_add(u32 amount)
{
    yodesk_seek += amount;
}

void seek_sub(u32 amount)
{
    yodesk_seek -= amount;
}

u32 get_location()
{
    return yodesk_seek;
}

char *get_str()
{
    u32 len = strlen((char*)(yodesk + yodesk_seek));
    yodesk_seek += len;
    return (char*)(yodesk + yodesk_seek - len);
}

char *get_strn(size_t len)
{
    yodesk_seek += len;
    return (char*)(yodesk + yodesk_seek - len);
}

u32 read_long()
{
    u32 value = *(u32*)(yodesk + yodesk_seek);
    yodesk_seek += 4;
    return value;
}

u16 read_short()
{
    u16 value = *(u16*)(yodesk + yodesk_seek);
    yodesk_seek += 2;
    return value;
}

u16 read_prefix()
{
    seek_sub(sizeof(u32)+sizeof(u16));
    u16 prefix = read_short();
    seek_add(sizeof(u32));

    return prefix;
}

u8 read_byte()
{
    u8 value = *(u8*)(yodesk + yodesk_seek);
    yodesk_seek += 1;
    return value;
}

void* current_file_pointer()
{
    return (void*)(yodesk + yodesk_seek);
}
