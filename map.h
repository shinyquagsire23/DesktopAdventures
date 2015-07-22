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

#include "useful.h"

#define DESERT	0x1
#define SNOW	0x2
#define FOREST	0x3
#define SWAMP	0x5

typedef struct izax_entry
{
	u16 entity_id;
	u16 x;
	u16 y;
	u16 item;
	u16 num_items;
	u16 unk3;
	u16 unk4[0x10];
} izax_entry;

typedef struct izax_entry_2
{
	u16 item;
} izax_entry_2;

typedef struct izax_entry_3
{
	u16 item;
} izax_entry_3;

typedef struct izax_data_1
{
	u32 magic;
	u32 size;
	u16 pad;
	u16 num_entries;
	izax_entry entries[];
} izax_data_1;

typedef struct izax_data_2
{
	u16 num_entries;
	izax_entry_2 entries[];
} izax_data_2;

typedef struct izax_data_3
{
	u16 num_entries;
	izax_entry_3 entries[];
} izax_data_3;

u32 camera_x;
u32 camera_y;
u16 width;
u16 height;

void load_map(u16 map_id);
void unload_map();
void render_map();

void read_iact_stats(u16 map_num, u32 location, u16 num_iacts);
void print_iact_stats();
