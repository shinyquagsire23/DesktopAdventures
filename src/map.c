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

#include "map.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include "iact.h"
#include "input.h"
#include "tile.h"
#include "tname.h"
#include "screen.h"
#include "assets.h"
#include "player.h"
#include "useful.h"
#include "character.h"
#include "objectinfo.h"

u32 tile_metadata[0x2000];
double world_timer = 0.0;

u16 **map_tiles_low;
u16 **map_tiles_middle;
u16 **map_tiles_high;
u16 *map_global_vars; //TODO: TempVar and RandVar are per-map as well.
bool **map_iact_flagonce;
u16 *map_overlay;
u32 map_camera_x = 0;
u32 map_camera_y = 0;
bool map_camera_locked = true;

entity *entities[512];
obj_info ***object_info;
u16 *object_info_qty = NULL;
u16 num_entities = 0;

u32 unknown;
u16 width;
u16 height;
u8 flags;
u8 area_type;
u8 same;
u16 id;

char area_types[0x6][10] = { "UNUSED", "DESERT", "SNOW", "FOREST", "UNUSED", "SWAMP" };
char map_flags[0x13][32] = {"NOP", "ENEMY_TERRITORY", "FINAL_DESTINATION", "ITEM_FOR_ITEM", "FIND_SOMETHING_USEFUL_NPC", "ITEM_TO_PASS", "FROM_ANOTHER_MAP", "TO_ANOTHER_MAP", "INDOORS", "INTRO_SCREEN", "FINAL_ITEM", "MAP_START_AREA", "UNUSED_C", "VICTORY_SCREEN", "LOSS_SCREEN", "MAP_TO_ITEM_FOR_LOCK", "FIND_SOMETHING_USEFUL_DROP", "FIND_SOMETHING_USEFUL_BUILDING", "FIND_THE_FORCE"};
char obj_types[0x10][32] = {"QUEST_ITEM_SPOT", "SPAWN", "THE_FORCE", "VEHICLE_TO", "VEHICLE_FROM", "LOCATOR", "ITEM", "PUZZLE_NPC", "WEAPON", "DOOR_IN", "DOOR_OUT", "UNKNOWN", "LOCK", "TELEPORTER", "XWING_FROM", "XWING_TO"};

void map_init(u16 num_maps)
{
    map_tiles_low = calloc(num_maps*sizeof(u16*), 1);
    map_tiles_middle = calloc(num_maps*sizeof(u16*), 1);
    map_tiles_high = calloc(num_maps*sizeof(u16*), 1);

    map_global_vars = calloc(num_maps*sizeof(u16), 1);
    map_iact_flagonce = calloc(num_maps*sizeof(bool*), 1);

    object_info = calloc(num_maps*sizeof(void*), 1);
    object_info_qty = calloc(num_maps*sizeof(u16*), 1);
}

//TODO: Try to make this a struct or something, less allocating of data that's already in our RAM buffer of the .DAT
void load_map(u16 map_id)
{
    id = map_id;

    init_screen();
    u32 location = zone_data[map_id]->izon_offset;
    location += 4; //IZON

    seek(location);

    /*
     * The unknown value can be either 0x1FA or 0x7AC. 0x7AC denotes
     * that certain 0xFFFF values need replacement in the scripts or
     * object info sections, specifically for quest items which can
     * change within the map (ie 'Find Something Useful...').
     */
    unknown = read_long();

    width = read_short();
    height = read_short();

    /*
     * Used for determining how to piece the maps together. During
     * map generation, there must be a specific amount of certain maps,
     * with other maps placed in between as fillers. The item chain in
     * the map must also check out properly. This value assists the
     * generator by allowing specific selection of certain maps at
     * generation time.
     */
    flags = read_byte();

    seek_add(5);

    area_type = read_byte();
    same = read_byte();

    map_overlay = malloc(width * height * 2);
    for (int i = 0; i < width * height; i++)
    {
        map_overlay[i] = 0xFFFF;
    }

    if(map_tiles_low[id] == NULL)
    {
        map_tiles_low[id] = malloc(width * height * 2);
        map_tiles_middle[id] = malloc(width * height * 2);
        map_tiles_high[id] = malloc(width * height * 2);
        map_iact_flagonce[id] = calloc(zone_data[map_id]->num_iacts*sizeof(bool), 1);
        for (int i = 0; i < width * height; i++)
        {
            map_tiles_low[id][i] = read_short();
            map_tiles_middle[id][i] = read_short();
            map_tiles_high[id][i] = read_short();

            map_overlay[i] = 0xFFFF;
        }

        //Process Object Info
        if(!is_yoda)
            seek(zone_data[map_id]->htsp_offset);

        object_info_qty[id] = zone_data[map_id]->htsp_offset == 0 && !is_yoda ? 0 : read_short();
        object_info[id] = malloc(object_info_qty[id] * sizeof(obj_info *));
        for (int i = 0; i < object_info_qty[id]; i++)
        {
            object_info[id][i] = malloc(sizeof(obj_info));

            object_info[id][i]->type = read_long();
            object_info[id][i]->x = read_short();
            object_info[id][i]->y = read_short();
            object_info[id][i]->visible = read_short();
            object_info[id][i]->arg = read_short();
        }

        iact_set_trigger(IACT_TRIG_FirstEnter, 0);
    }
    else
    {
        seek_add(width * height * sizeof(u16) * 3);
    }

    for (int i = 0; i < object_info_qty[id]; i++)
    {
        printf("  obj_info: %s, %u, %u, %u, %x (%s?)\n", obj_types[object_info[id][i]->type], object_info[id][i]->x, object_info[id][i]->y, object_info[id][i]->visible, object_info[id][i]->arg, tile_names[object_info[id][i]->arg]);

        //Display items and NPCs for debug purposes
        switch (object_info[id][i]->type)
        {
            case OBJ_ITEM:
            case OBJ_WEAPON:
            case OBJ_PUZZLE_NPC:
                map_overlay[object_info[id][i]->x + (object_info[id][i]->y * width)] = object_info[id][i]->visible ? object_info[id][i]->arg : TILE_NONE;
                break;
            case OBJ_DOOR_OUT:
                player_entity.x = object_info[id][i]->x;
                player_entity.y = object_info[id][i]->y;
                break;
        }
    }

    switch(PLAYER_MAP_CHANGE_REASON)
    {
        case MAP_CHANGE_DOOR_OUT:
            player_goto_door_in();
            break;
    }

    printf("Loading map %i, %s, %s, %s, width %i, height %i\n", map_id, (unknown == 0x7AC ? "DYNAMIC" : "STATIC"), map_flags[flags], area_types[area_type], width, height);
    iact_set_trigger(IACT_TRIG_Enter, 0);

    load_izax(); //TODO: Indy IZAX is funky.
#ifndef _3DS
    read_iact(); //Prints out a bunch of stuff... This kills the 3DS.
#endif
}

void unload_map()
{
    //Give the world one last render before unloading
    render_map();
    draw_screen();

    if(PLAYER_MAP_CHANGE_REASON != MAP_CHANGE_NONE)
        screen_transition_out();

    //These maps are not worth keeping in memory
    if(flags == MAP_FLAG_INTRO_SCREEN)
    {
        free(map_tiles_low[id]);
        free(map_tiles_middle[id]);
        free(map_tiles_high[id]);

        free(map_iact_flagonce[id]);

        map_tiles_low[id] = NULL;

        for(int i = 0; i < object_info_qty[id]; i++)
        {
            free(object_info[id][i]);
            object_info[id][i] = NULL;
        }

        free(object_info[id]);
        object_info[id] = NULL;
    }
    free(map_overlay);

    for(int i = 0; i < num_entities; i++)
        free(entities[i]);

    num_entities = 0;


}

void add_existing_entity(entity e)
{
    entities[num_entities++] = &e;
}

void add_new_entity(u16 id, u16 x, u16 y, u16 frame, u16 item, u16 num_items)
{
    entity *e = malloc(sizeof(entity));

    e->char_id = id;
    e->x = x;
    e->y = y;
    e->current_frame = frame;
    e->item = item;
    e->num_items = num_items;

    entities[num_entities++] = e;
}

void load_izax()
{
    seek(zone_data[id]->izax_offset);
    read_long(); //IZAX
    u32 izax_data_1_size = zone_data[id]->izax_offset != 0 ? read_long() : 0;
    izax_data_1 *first_section = (izax_data_1*)(zone_data[id]->izax_offset != 0 ? malloc(izax_data_1_size) : calloc(0x10, sizeof(u8)));
    seek_sub(sizeof(u32)*2);

    first_section->magic = read_long();
    first_section->size = read_long();
    first_section->mission_specific = read_short();
    first_section->num_entries = read_short();
    for(int i = 0; i < first_section->num_entries; i++)
    {
        first_section->entries[i].entity_id = read_short();
        first_section->entries[i].x = read_short();
        first_section->entries[i].y = read_short();
        first_section->entries[i].item = read_short();
        first_section->entries[i].num_items = read_short();
        first_section->entries[i].unk3 = read_short();
        read_bytes(first_section->entries[i].unk4, 0x10 * sizeof(u16));
    }

    /* Possible items to be found and replaced in scripts
     * (some scripts have filler spots for these items)
     * Probably used for generating the maps.
     */
    seek(zone_data[id]->izx2_offset);
    read_long(); //IZX2
    u32 izax_data_2_size = zone_data[id]->izx2_offset != 0 ? read_long() : 0;
    izax_data_2 *second_section = (izax_data_2*)(zone_data[id]->izx2_offset != 0 ? malloc(izax_data_2_size) : calloc(0x10, sizeof(u8)));
    seek_sub(sizeof(u32)*2);

    second_section->magic = read_long();
    second_section->size = read_long();
    second_section->num_entries = read_short();
    for(int i = 0; i < second_section->num_entries; i++)
    {
        second_section->entries[i].item = read_short();
    }

    /* Ending or transition to possible ending item(s). Usually takes one of a select
     * amount of items and turns it into a single ending item for a map.
     * Probably used to properly shape plot by end of the map generation.
     */
    seek(zone_data[id]->izx3_offset);
    read_long(); //IZX3
    u32 izax_data_3_size = zone_data[id]->izx3_offset != 0 ? read_long() : 0;
    izax_data_3 *third_section = (izax_data_3*)(zone_data[id]->izx3_offset != 0 ? malloc(izax_data_3_size) : calloc(0x10, sizeof(u8)));
    seek_sub(sizeof(u32)*2);

    third_section->magic = read_long();
    third_section->size = read_long();
    third_section->num_entries = read_short();
    for(int i = 0; i < third_section->num_entries; i++)
    {
        third_section->entries[i].item = read_short();
    }

    /*
     * If the value in IZX4 is zero, this map is a static map used for
     * entering or exiting a randomized set of zones using set items,
     * ie Dagobah in Yoda Stories is set zero to start a plot, and
     * the map where the last item is used is also set zero and requires
     * a certain item.
     */
    seek(zone_data[id]->izx4_offset);
    read_long(); //IZX4
    u32 izax_data_4_size = read_long();
    izax_data_4 *fourth_section = (izax_data_4*)(zone_data[id]->izx4_offset != 0 ? malloc(izax_data_4_size) : calloc(0x10, sizeof(u8)));
    seek_sub(sizeof(u32)*2);

    fourth_section->magic = read_long();
    fourth_section->size = read_long();
    fourth_section->is_intermediate = read_short();

    printf("Reading IZAX data, %u entries in first section, %u in the second and %u in the third. %s %s\n", first_section->num_entries, second_section->num_entries, third_section->num_entries, !fourth_section->is_intermediate ? "This map is either a seed item map or an end item consuming map!" : "", first_section->mission_specific ? "This map is specific to a particular plot!" : "");

    for(int i = 0; i < first_section->num_entries; i++)
    {
        printf("  entity: id=%x, x=%x, y=%x, item=%s, qty=%x, %x\n", first_section->entries[i].entity_id, first_section->entries[i].x, first_section->entries[i].y, tile_names[first_section->entries[i].item], first_section->entries[i].num_items, first_section->entries[i].unk3);
        add_new_entity(first_section->entries[i].entity_id, first_section->entries[i].x, first_section->entries[i].y, FRAME_DOWN, first_section->entries[i].item, first_section->entries[i].num_items);
    }

    for(int i = 0; i < second_section->num_entries; i++)
    {
        printf("  item: %s\n", tile_names[second_section->entries[i].item]);
    }

    for(int i = 0; i < third_section->num_entries; i++)
    {
        printf("   end item: %s\n", tile_names[third_section->entries[i].item]);
    }

    free(first_section);
    free(second_section);
    free(third_section);
    free(fourth_section);
}

void render_map()
{
    for (int i = 0; i < object_info_qty[id]; i++)
    {
        //Display items and NPCs for debug purposes
        switch (object_info[id][i]->type)
        {
            case OBJ_ITEM:
            case OBJ_WEAPON:
            case OBJ_PUZZLE_NPC:
                map_overlay[object_info[id][i]->x + (object_info[id][i]->y * width)] = object_info[id][i]->visible ? object_info[id][i]->arg : TILE_NONE;
                break;
        }
    }


    for(int i = 0; i < 9; i++)
    {
        for(int j = 0; j < 9; j++)
        {
            tiles_low[(j*9)+i] = map_tiles_low[id][((map_camera_y+j)*width)+i+map_camera_x];
            tiles_middle[(j*9)+i] = map_tiles_middle[id][((map_camera_y+j)*width)+i+map_camera_x];
            tiles_high[(j*9)+i] = map_tiles_high[id][((map_camera_y+j)*width)+i+map_camera_x];
            tiles_overlay[(j*9)+i] = map_overlay[((map_camera_y+j)*width)+i+map_camera_x];
        }
    }

    if(player_entity.y >= map_camera_y &&
       player_entity.y < (map_camera_y + 9) &&
       player_entity.x >= map_camera_x &&
       player_entity.x < (map_camera_x + 9))
    {
        tiles_middle[((player_entity.y - map_camera_y)*9) + (player_entity.x - map_camera_x)] = char_data[player_entity.char_id]->frames[player_entity.current_frame];
    }

    for(int i = 0; i < num_entities; i++)
    {
        entity *e = entities[i];
        if(e->current_frame < 2)
            continue; //These frames are transparent/unused in Yoda Stories, not sure for Indy

        if(e->y >= map_camera_y &&
            e->y < map_camera_y + 9 &&
            e->x >= map_camera_x &&
            e->x < map_camera_x + 9)
        {
            tiles_high[((e->y - map_camera_y)*9) + (e->x - map_camera_x)] = char_data[e->char_id]->frames[e->current_frame];

            if(e->num_items > 0)
                tiles_overlay[((e->y - map_camera_y)*9) + (e->x - map_camera_x)] = e->item;
        }
    }

    //Test for rendering characters
    for(int i = 0; i < 26; i++)
    {
        //tiles_high[i] = char_data[id]->frames[i];
    }
}

u32 map_get_width()
{
    return width;
}

u32 map_get_height()
{
    return height;
}

u16 map_get_id()
{
    return id;
}

obj_info *map_get_object_by_id(int index)
{
    return object_info[id][index];
}

obj_info *map_get_object(int index, int x, int y)
{
    int seek_index = index;
    for(int i = 0; i < object_info_qty[id]; i++)
    {
        if(object_info[id][i]->x == (u16)x && object_info[id][i]->y == (u16)y)
        {
            if(seek_index == 0)
                return object_info[id][i];
            seek_index--;
        }
    }
    return NULL;
}

u16 map_get_tile(u8 layer, int x, int y)
{
    switch(layer)
    {
        case LAYER_LOW:
            return map_tiles_low[id][(y*width)+x];
        case LAYER_MIDDLE:
            return map_tiles_middle[id][(y*width)+x];
        case LAYER_HIGH:
            return map_tiles_high[id][(y*width)+x];
        default:
        case LAYER_OVERLAY:
            return map_overlay[(y*width)+x];
    }
}

void map_set_tile(u8 layer, int x, int y, u16 tile)
{
    switch(layer)
    {
        case LAYER_LOW:
            map_tiles_low[id][(y*width)+x] = tile;
            break;
        case LAYER_MIDDLE:
            map_tiles_middle[id][(y*width)+x] = tile;
            break;
        case LAYER_HIGH:
            map_tiles_high[id][(y*width)+x] = tile;
            break;
        default:
        case LAYER_OVERLAY:
            map_overlay[(y*width)+x] = tile;
            break;
    }
}

u32 map_get_meta(u8 layer, int x, int y)
{
    switch(layer)
    {
        case LAYER_LOW:
            return tile_metadata[map_tiles_low[id][(y*width)+x]];
        case LAYER_MIDDLE:
            return tile_metadata[map_tiles_middle[id][(y*width)+x]];
        case LAYER_HIGH:
            return tile_metadata[map_tiles_high[id][(y*width)+x]];
        default:
        case LAYER_OVERLAY:
            return tile_metadata[map_overlay[(y*width)+x]];
    }
}

void map_update_camera(bool redraw)
{
    map_camera_x = MIN(MAX(0, player_entity.x - 4), width - 9);
    map_camera_y = MIN(MAX(0, player_entity.y - 4), height - 9);
    if(redraw)
        render_map();
}

u16 map_get_global_var()
{
    return map_global_vars[id];
}

void map_set_global_var(u16 val)
{
    map_global_vars[id] = val;
}

bool map_get_iact_flagonce(int iact_id)
{
    return map_iact_flagonce[id][iact_id];
}

void map_set_iact_flagonce(int iact_id, bool val)
{
    map_iact_flagonce[id][iact_id] = val;
}

void update_world(double delta)
{
    if(PLAYER_MAP_CHANGE_REASON != MAP_CHANGE_NONE && PLAYER_MAP_CHANGE_REASON != MAP_CHANGE_SCRIPT)
    {
        map_update_camera(true);

        screen_transition_in();
        PLAYER_MAP_CHANGE_REASON = MAP_CHANGE_NONE;
    }

    //Limit our FPS so that each frame corresponds to a game tick for "Game Speed"
    world_timer += delta;
    if(world_timer > (1000/TARGET_TICK_FPS))
    {
        if (BUTTON_LEFT_STATE)
            player_move(LEFT);
        if (BUTTON_RIGHT_STATE)
            player_move(RIGHT);
        if (BUTTON_UP_STATE)
            player_move(UP);
        if (BUTTON_DOWN_STATE)
            player_move(DOWN);

        if(map_camera_locked)
            map_update_camera(false);

        player_update();
        iact_update();

        render_map();
        draw_screen();
        world_timer = 0.0;
    }
}
