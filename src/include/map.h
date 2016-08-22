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

#ifndef MAP_H
#define MAP_H

#include "useful.h"

#include "objectinfo.h"

enum area_types
{
 DESERT = 0x1,
 SNOW,
 FOREST,
 SWAMP = 0x5
};

enum map_flags
{
    MAP_FLAG_ENEMY_TERRITORY = 0x1,
    MAP_FLAG_FINAL_DESTINATION,
    MAP_FLAG_ITEM_FOR_ITEM,
    MAP_FLAG_FIND_SOMETHING_USEFUL_NPC,
    MAP_FLAG_ITEM_TO_PASS, //Seen in map 99
    MAP_FLAG_FROM_ANOTHER_MAP,
    MAP_FLAG_TO_ANOTHER_MAP,
    MAP_FLAG_INDOORS,
    MAP_FLAG_INTRO_SCREEN,
    MAP_FLAG_FINAL_ITEM,
    MAP_FLAG_MAP_START_AREA,
    MAP_FLAG_UNUSED_C,
    MAP_FLAG_VICTORY_SCREEN,
    MAP_FLAG_LOSS_SCREEN,
    MAP_FLAG_MAP_TO_ITEM_FOR_LOCK,
    MAP_FLAG_FIND_SOMETHING_USEFUL_DROP, //Or building?
    MAP_FLAG_FIND_SOMETHING_USEFUL_BUILDING,
    MAP_FLAG_FIND_THE_FORCE,
};

enum MAP_LAYER
{
    LAYER_LOW,
    LAYER_MIDDLE,
    LAYER_HIGH,
    LAYER_OVERLAY
} MAP_LAYER;

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

void map_init(u16 num_maps);
u32 map_get_width();
u32 map_get_height();
u16 map_get_id();
bool map_is_loaded(u16 test_id);
u16 map_get_num_objects();
obj_info *map_get_object_by_id(int index);
obj_info *map_get_object(int index, int x, int y);
u16 map_get_tile(u8 layer, int x, int y);
void map_set_tile(u8 layer, int x, int y, u16 tile);
u32 map_get_meta(u8 layer, int x, int y);
void map_update_camera(bool redraw);
u16 map_get_global_var();
void map_set_global_var(u16 val);
u16 map_get_temp_var();
void map_set_temp_var(u16 val);
u16 map_get_rand_var();
void map_set_rand_var(u16 val);
bool map_get_iact_flagonce(int iact_id);
void map_set_iact_flagonce(int iact_id, bool val);

void map_show_entity(u16 index);
void map_hide_entity(u16 index);
void map_show_all_entities();
void map_hide_all_entities();
bool map_is_entity_active_visible(u16 index);
bool map_all_entities_active_visible();

void read_iact_stats(u16 map_num, u32 location, u16 num_iacts);
void print_iact_stats();

u32 map_camera_x;
u32 map_camera_y;
bool map_camera_locked;

#endif