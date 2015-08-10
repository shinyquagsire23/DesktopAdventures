#include "useful.h"
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

#include <GL/gl.h>

void load_resources();
void seek(u32 location);
void seek_add(u32 amount);
u32 get_location();
u32 read_long();
u16 read_short();
u8 read_byte();
void* current_file_pointer();

typedef struct izon_data
{
	u32 izon_offset;
	u32 izax_offset;
	u32 izx2_offset;
	u32 izx3_offset;
	u32 izx4_offset;
	u16 num_iacts;
	u32 iact_offset;
} izon_data;

//static const u8* yodesk_palette;
GLuint texture[0x2001];
u32 tile_metadata[0x2000];
u8 ASSETS_LOADING;
izon_data **zone_data;
u16 NUM_MAPS;
u8 load_demo;
u8 is_yoda;
