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

enum area_types
{
 DESERT = 0x1,
 SNOW,
 FOREST,
 SWAMP = 0x5
};

enum map_flags
{
    ENEMY_TERRITORY = 0x1,
    FINAL_DESTINATION,
    ITEM_FOR_ITEM,
    FIND_SOMETHING_USEFUL_NPC,
    ITEM_TO_PASS, //Seen in map 99
    FROM_ANOTHER_MAP,
    TO_ANOTHER_MAP,
    INDOORS,
    INTRO_SCREEN,
    FINAL_ITEM,
    MAP_START_AREA,
    UNUSED_C,
    VICTORY_SCREEN,
    LOSS_SCREEN,
    MAP_TO_ITEM_FOR_LOCK,
    FIND_SOMETHING_USEFUL_DROP, //Or building?
    FIND_SOMETHING_USEFUL_BUILDING,
    FIND_THE_FORCE,
};

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
    u16 mission_specific;
    u16 num_entries;
    izax_entry entries[];
} izax_data_1;

typedef struct izax_data_2
{
    u32 magic;
    u32 size;
    u16 num_entries;
    izax_entry_2 entries[];
} izax_data_2;

typedef struct izax_data_3
{
    u32 magic;
    u32 size;
    u16 num_entries;
    izax_entry_3 entries[];
} izax_data_3;

typedef struct izax_data_4
{
    u32 magic;
    u32 size;
    u16 is_intermediate;
} izax_data_4;

void load_map(u16 map_id);
void load_izax();
void unload_map();
void render_map();
void update_world(double delta);

void read_iact_stats(u16 map_num, u32 location, u16 num_iacts);
void print_iact_stats();
