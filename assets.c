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

#include <GL/gl.h>
#include "useful.h"
#include "palette.h"
#include "map.h"
#include "tile.h"
#include "tname.h"
#include "character.h"
#include "puzzle.h"
#include "sound.h"

u8* yodesk;
u32* gw;
u32 yodesk_size = 0;

u8 ASSETS_LOADING = 1;
u16 NUM_MAPS = 0;

u32 version = 4;
u32* tile;

GLuint texture[0x1001];
u32 tile_metadata[0x1000];
izon_data **zone_data;
//TNAME **tile_names;

u32 yodesk_seek = 0;

u8 load_demo = 0;
u8 is_yoda = 1;
u16 ipuznum = 0;

void load_resources()
{
	yodesk = readFileToBytes(is_yoda ? (load_demo ? "YodaDemo.dta" : "YODESK.DTA") : "WhateverIndianaJonesUses.dta", &yodesk_size);
	printf("YODESK.DTA loaded, %x bytes large\n", yodesk_size);

	u16 izon_count = -1;
	for(int i = 0; i < yodesk_size; i++)
	{
		seek(i);
		if(!strncmp((yodesk + i), "VERS", 4)) //VERSion
		{
			printf("Found VERS at %x, version number %x\n", i, *(u32*)(yodesk + i + 4));
		}
		else if(!strncmp((yodesk + i), "STUP", 4)) //STartUP Graphic, uses yodesk_palette
		{
			printf("Found STUP at %x\n", i);
			load_texture(288, yodesk+i+8, 0x1000); //Load Startup Texture past the last tile
		}
		else if(!strncmp((yodesk + i), "ZONE", 4)) //ZONEs (maps)
		{
			printf("Found ZONE at %x\n", i);
			i += 4; //"ZONE"

			NUM_MAPS = *(u16*)(yodesk + i); i += 2;
			u16 unknown = *(u16*)(yodesk + i); i += 2;
			u32 ZONE_LENGTH = *(u32*)(yodesk + i);
			zone_data = malloc(NUM_MAPS * sizeof(char*));
			for(int j = 0; j < NUM_MAPS; j++)
				zone_data[j] = (izon_data*)malloc(sizeof(izon_data));

			printf("%i maps in DAT\n", NUM_MAPS);
		}
		else if(!strncmp((yodesk + i), "IZON", 4)) //Internal ZONe? (map)
		{
			printf("Found IZON %i at %x\n", izon_count, i);
			izon_count++;
			zone_data[izon_count]->izon_offset = i;
		}
		else if(!strncmp((yodesk + i), "IZAX", 4)) //IZAX
		{
			//printf("Found IZAX at %x\n", i);
			zone_data[izon_count]->izax_offset = i;
		}
		else if(!strncmp((yodesk + i), "IZX2", 4)) //IZX2
		{
			//printf("Found IZX2 at %x\n", i);
			zone_data[izon_count]->izx2_offset = i;
		}
		else if(!strncmp((yodesk + i), "IZX3", 4)) //IZX3
		{
			//printf("Found IZX3 at %x\n", i);
			zone_data[izon_count]->izx3_offset = i;
		}
		else if(!strncmp((yodesk + i), "IZX4", 4)) //IZX4
		{
			//printf("Found IZX4 at %x\n", i);
			zone_data[izon_count]->izx4_offset = i;
		}
		else if(!strncmp((yodesk + i), "IACT", 4)) //IACT
		{
			if(zone_data[izon_count]->iact_offset == 0)
			{
				zone_data[izon_count]->num_iacts = *(u16*)(yodesk + i - 2);
				zone_data[izon_count]->iact_offset = i;
				read_iact_stats(izon_count, zone_data[izon_count]->iact_offset, zone_data[izon_count]->num_iacts);
				printf("Found %u IACT%s at %x\n", zone_data[izon_count]->num_iacts, (zone_data[izon_count]->num_iacts > 1 && zone_data[izon_count]->num_iacts != 0 ? "s" : ""), i);
			}
		}
		else if(!strncmp((yodesk + i), "SNDS", 4)) //SouNDS
		{
			printf("Found SNDS at %x\n", i);
			read_long(); //SNDS

			u32 length = read_long();
			u16 unk1 = read_short();
			sound_files = malloc(256 * sizeof(char*));
			i += 4+4+2;

			u32 seed = i;

			for(int j = 0; (i - seed) < (length - 2); j++)
			{
				u32 str_length = read_short(); i += 2;
				sound_files[j] = yodesk + i;
				printf("%x %s\n", str_length, sound_files[j]);
				seek_add(str_length); i += str_length;
			}
			i -= 4;
		}
		else if(!strncmp((yodesk + i), "TILE", 4)) //TILEs (graphics)
		{
			printf("Found TILE at %x\n", i);
			i+=4; //Section length + "TILE"
			int section_length = *(u32*)(yodesk + i); i+=4;
			printf("0x%x tiles in TILES\n", section_length / ((32*32)+4));
			for(int j = 0; j < section_length / ((32*32)+4); j++)
			{
				int tile_stuff = *(u32*)(yodesk + i);
				tile_metadata[j] = tile_stuff;
				load_texture(32, yodesk+i+4+(j * ((32*32) + 4)), j);
			}
		}
		else if(!strncmp((yodesk + i), "PUZ2", 4)) //Puzzle configurations maybe?
		{
			printf("Found PUZ2 at %x\n", i);
			read_long(); //PUZ2
			u32 length = read_long();
			ipuz_data = malloc(512 * sizeof(char*));
		}
		else if(!strncmp((yodesk + i), "IPUZ", 4)) //Puzzle string
		{
			printf("Found IPUZ at %x\n", i);
			u16 id = *(u16*)(yodesk + i - 2);
			read_long(); //IPUZ

			ipuz_element *e = malloc(sizeof(ipuz_element));
			e->size = read_long();
			e->unk1 = read_long();
			e->unk2 = read_long();
			e->unk3 = read_long();
			e->unk4 = read_short();

			e->string1_len = read_short();
			for(int i = 0; i < e->string1_len; i++)
				e->string1[i] = read_byte();

			e->string2_len = read_short();
			for(int i = 0; i < e->string2_len; i++)
				e->string2[i] = read_byte();

			e->string3_len = read_short();
			for(int i = 0; i < e->string3_len; i++)
				e->string3[i] = read_byte();

			e->string4_len = read_short();
			for(int i = 0; i < e->string4_len; i++)
				e->string4[i] = read_byte();

			e->unused_len = read_short();
			for(int i = 0; i < e->unused_len; i++)
				e->unused[i] = read_byte();

			e->item_a = read_short();
			e->item_b = read_short();

			ipuz_data[id] = e;
			ipuznum++;
		}
		else if(!strncmp((yodesk + i), "CHAR", 4) && strncmp((yodesk + i), "CHARGE", 6))
		{
			read_long(); //"CHAR"
			u16 size = read_short();
			read_short();
			printf("%x\n", size);

			char_data = malloc((size / 0x54) * sizeof(char*));

			for(int j = 0; j < (size / 0x54); j++)
			{
				u16 id = read_short();
				char_data[id] = (ichr_data*)(current_file_pointer());
				printf("%x - %-16s %x %x %x\n", id, char_data[id]->name, char_data[id]->unk_1, char_data[id]->unk_3, char_data[id]->unk_4, char_data[id]->unk_5);
				seek_add(0x54 - 2);
			}
			i += size + 6;
		}
		else if(!strncmp((yodesk + i), "CHWP", 4))
		{
			printf("Found CHWP at %x\n", i);
		}
		else if (!strncmp((yodesk + i), "ANAM", 4)) //Tile names
		{
			printf("Found ANAM at %x\n", i);
		}
		else if (!strncmp((yodesk + i), "PNAM", 4)) //Tile names
		{
			printf("Found PNAM at %x\n", i);
		}
		else if(!strncmp((yodesk + i), "TNAM", 4)) //Tile names
		{
			printf("Found TNAM at %x\n", i);

			read_long(); //"TNAM"
			u32 size = read_long();
			i += size - 1;

			char *nop = "NO NAME";

			for(int j = 0; j < 0x10000; j++)
				tile_names[j] = nop;

			for(int j = 0; j < size / (is_yoda ? 26 : 18); j++)
			{
				u16 id = read_short();
				char *name;
				if (id == 0xFFFF)
					name = nop;
				else
					name = current_file_pointer();

				tile_names[id] = name;

				printf("%x, %s\n", id, tile_names[id]);

				if (is_yoda)
					seek_add(24);
				else
					seek_add(16);
			}
		}
		else if(!strncmp((yodesk + i), "ENDF", 4))
		{
			//print_iact_stats();
			for(int j = 0; j < ipuznum; j++)
			{
				ipuz_element *e = ipuz_data[j];
				printf("%x %x %x %x \"%s\" \"%s\" \"%s\" \"%s\", %s (%x), %s (%x)\n", e->unk1, e->unk2, e->unk3, e->unk4, e->string1, e->string2, e->string3, e->string4, tile_names[e->item_a], e->item_a, tile_names[e->item_b], e->item_b);
			}
			printf("Found ENDF at %x\n", i);
		}
	}

	load_map(0);
	init_screen();
	render_map();
	ASSETS_LOADING = 0;
}

void load_texture(u16 width, u8 *data, u32 texture_num)
{
	u32 *data_buffer = malloc(width * width * 4);
	int index = 0;
	for(int i = 0; i < width * width; i++)
	{
		int color_index = data[index];
		u32 color = ((u8)(yodesk_palette[(color_index * 4)]) << 16) + ((u8)(yodesk_palette[(color_index * 4) + 1]) << 8) + ((u8)(yodesk_palette[(color_index * 4) + 2]) << 0);

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

u32 get_location()
{
	return yodesk_seek;
}

u32 read_long()
{
	u32 value = *(u32*)(yodesk + yodesk_seek);
	yodesk_seek += 4;
	return value;
}

u16 read_short()
{
	u32 value = *(u16*)(yodesk + yodesk_seek);
	yodesk_seek += 2;
	return value;
}

u8 read_byte()
{
	u32 value = *(u8*)(yodesk + yodesk_seek);
	yodesk_seek += 1;
	return value;
}

void* current_file_pointer()
{
	return (void*)(yodesk + yodesk_seek);
}
