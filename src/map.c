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
#include "tname.h"
#include "screen.h"
#include "assets.h"
#include "useful.h"
#include "character.h"
#include "objectinfo.h"

u16 *map_tiles_low;
u16 *map_tiles_middle;
u16 *map_tiles_high;
u16 *map_overlay;
u32 camera_x = 0;
u32 camera_y = 0;

entity player_entity;
entity *entities[512];
obj_info **object_info;
u16 object_info_qty = 0;
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
char triggers[0x24][30] = { "FirstEnter", "Enter", "BumpTile", "DragItem", "Walk", "TempVarEq", "RandVarEq", "RandVarGt", "RandVarLs", "EnterVehicle", "CheckMapTile", "EnemyDead", "AllEnemiesDead", "HasItem", "HasEndItem", "Unk0f", "Unk10", "GameInProgress?", "GameCompleted?", "HealthLs", "HealthGt", "Unk15", "Unk16", "DragWrongItem", "PlayerAtPos", "GlobalVarEq", "GlobalVarLs", "GlobalVarGt", "ExperienceEq", "Unk1d", "Unk1e", "TempVarNe", "RandVarNe", "GlobalVarNe", "CheckMapTileVar", "ExperienceGt"};
char commands[0x26][30] = { "SetMapTile", "ClearTile", "MoveMapTile", "DrawOverlayTile", "SayText", "ShowText", "RedrawTile", "RedrawTiles", "RenderChanges", "WaitSecs", "PlaySound", "Unk0b", "Random", "SetTempVar", "AddTempVar", "SetMapTileVar", "ReleaseCamera", "LockCamera", "SetPlayerPos", "MoveCamera", "Redraw", "OpenDoor?", "CloseDoor?", "EnemySpawn", "NPCSpawn", "RemoveDraggedItem", "RemoveDraggedItemSimilar?", "SpawnItem", "AddItemToInv", "DropItem", "Open?Show?", "Unk1f", "Unk20", "WarpToMap", "SetGlobalVar", "AddGlobalVar", "SetRandVar", "AddHealth"};

//TODO: Try to make this a struct or something, less allocating of data that's already in our RAM buffer of the .DAT
void load_map(u16 map_id)
{
    id = map_id;

    init_screen();
    u32 location = zone_data[map_id]->izon_offset;
    location += 4; //IZON

    camera_x = 0;
    camera_y = 0;

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

    map_tiles_low = malloc(width*height*2);
    map_tiles_middle = malloc(width*height*2);
    map_tiles_high = malloc(width*height*2);
    map_overlay = malloc(width*height*2);
    for(int i = 0; i < width*height; i++)
    {
        map_tiles_low[i] = read_short();
        map_tiles_middle[i] = read_short();
        map_tiles_high[i] = read_short();
        map_overlay[i] = 0xFFFF;
    }

    printf("Loading map %i, %s, %s, %s, width %i, height %i\n", map_id, (unknown == 0x7AC ? "DYNAMIC" : "STATIC"), map_flags[flags], area_types[area_type], width, height);

    //Process Object Info
    if(!is_yoda)
        seek(zone_data[map_id]->htsp_offset);

    object_info_qty = zone_data[map_id]->htsp_offset == 0 && !is_yoda ? 0 : read_short();
    object_info = malloc(object_info_qty * sizeof(obj_info*));
    for(int i = 0; i < object_info_qty; i++)
    {
        object_info[i] = malloc(sizeof(obj_info));

        object_info[i]->type = read_long();
        object_info[i]->x = read_short();
        object_info[i]->y = read_short();
        object_info[i]->unk1 = read_short();
        object_info[i]->arg = read_short();

        printf("  obj_info: %s, %u, %u, %u, %x (%s?)\n", obj_types[object_info[i]->type], object_info[i]->x, object_info[i]->y, object_info[i]->unk1, object_info[i]->arg, tile_names[object_info[i]->arg]);

        //Display items and NPCs for debug purposes
        switch(object_info[i]->type)
        {
            case ITEM:
                map_overlay[object_info[i]->x+(object_info[i]->y*width)] = object_info[i]->arg;
                break;
            case WEAPON:
                map_overlay[object_info[i]->x+(object_info[i]->y*width)] = object_info[i]->arg;
                break;
            case PUZZLE_NPC:
                map_overlay[object_info[i]->x+(object_info[i]->y*width)] = object_info[i]->arg;
                break;
        }
    }

    load_izax(); //TODO: Indy IZAX is funky.
#ifndef _3DS
    //read_iact(zone_data[map_id]->iact_offset, zone_data[map_id]->num_iacts); //Prints out a bunch of stuff... This kills the 3DS.
#endif
}

void unload_map()
{
    free(map_tiles_low);
    free(map_tiles_middle);
    free(map_tiles_high);

    for(int i = 0; i < num_entities; i++)
        free(entities[i]);

    for(int i = 0; i < object_info_qty; i++)
        free(object_info[i]);

    free(object_info);

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
    void *zero_buf = calloc(0x10, sizeof(u8));

    seek(zone_data[id]->izax_offset);
    izax_data_1 *first_section = (izax_data_1*)(zone_data[id]->izax_offset != 0 ? current_file_pointer() : zero_buf);

    /* Possible items to be found and replaced in scripts
     * (some scripts have filler spots for these items)
     * Probably used for generating the maps.
     */
    seek(zone_data[id]->izx2_offset);
    izax_data_2 *second_section = (izax_data_2*)(zone_data[id]->izx2_offset != 0 ? current_file_pointer() : zero_buf);

    /* Ending or transition to possible ending item(s). Usually takes one of a select
     * amount of items and turns it into a single ending item for a map.
     * Probably used to properly shape plot by end of the map generation.
     */
    seek(zone_data[id]->izx3_offset);
    izax_data_3 *third_section = (izax_data_3*)(zone_data[id]->izx3_offset != 0 ? current_file_pointer() : zero_buf);

    /*
     * If the value in IZX4 is zero, this map is a static map used for
     * entering or exiting a randomized set of zones using set items,
     * ie Dagobah in Yoda Stories is set zero to start a plot, and
     * the map where the last item is used is also set zero and requires
     * a certain item.
     */
    seek(zone_data[id]->izx4_offset);
    izax_data_4 *fourth_section = (izax_data_4*)(zone_data[id]->izx4_offset != 0 ? current_file_pointer() : zero_buf);

    printf("Reading IZAX data, %u entries in first section, %u in the second and %u in the third. %s %s\n", first_section->num_entries, second_section->num_entries, third_section->num_entries, !fourth_section->is_intermediate ? "This map is either a seed item map or an end item consuming map!" : "", first_section->mission_specific ? "This map is specific to a particular plot!" : "");

    for(int i = 0; i < first_section->num_entries; i++)
    {
        printf("  entity: id=%x, x=%x, y=%x, item=%s, qty=%x, %x\n", first_section->entries[i].entity_id, first_section->entries[i].x, first_section->entries[i].y, tile_names[first_section->entries[i].item], first_section->entries[i].num_items, first_section->entries[i].unk3);
        add_new_entity(first_section->entries[i].entity_id, first_section->entries[i].x, first_section->entries[i].y, DOWN, first_section->entries[i].item, first_section->entries[i].num_items);
    }

    for(int i = 0; i < second_section->num_entries; i++)
    {
        printf("  item: %s\n", tile_names[second_section->entries[i].item]);
    }

    for(int i = 0; i < third_section->num_entries; i++)
    {
        printf("   end item: %s\n", tile_names[third_section->entries[i].item]);
    }
}

void read_iact(u32 location, u16 num_iacts)
{
    printf("Reading IACT data, %u IACTs\n", num_iacts);
    seek(location);
    for(int i = 0; i < num_iacts; i++)
    {
        read_long(); //IACT
        u32 length = read_long();
        u16 iactItemCount1 = read_short();
        printf("\n    Action: size %08x, actions %d\n", length, iactItemCount1);
        for (u16 k = 0; k < iactItemCount1; k++)
        {
            char pos_str[7];
            u16 command = read_short();
            u16 arg1 = read_short();
            u16 arg2 = read_short();
            u16 arg3 = read_short();
            u16 arg4 = read_short();
            u16 arg5 = read_short();
            u16 arg6 = read_short();

            *(u16*)(pos_str) = arg4;
            *(u16*)(pos_str+2) = arg5;
            *(u16*)(pos_str+4) = arg6;

            printf("        %s, %04x, %04x, %04x, %04x, %04x, %04x, %s\n", triggers[command], arg1, arg2, arg3, arg4, arg5, arg6, pos_str);
        }
        u16 iactItemCount2 = read_short();
        printf("    Script: commands %d\n", iactItemCount2);
        for(u16 k = 0; k < iactItemCount2; k++)
        {
            char pos_str[7];
            u16 command = read_short();
            u16 arg1 = read_short();
            u16 arg2 = read_short();
            u16 arg3 = read_short();
            u16 arg4 = read_short();
            u16 arg5 = read_short();
            u16 strlen = read_short();

            *(u16*)(pos_str) = arg3;
            *(u16*)(pos_str+2) = arg4;
            *(u16*)(pos_str+4) = arg5;

            printf("        %s, %04x, %04x, %04x, %04x, %04x, %04x, %s\n", commands[command], arg1, arg2, arg3, arg4, arg5, strlen, pos_str);
            if (strlen)
            {
                char* str = malloc(strlen+1);
                for (u16 l = 0; l < strlen; l++)
                {
                    str[l] = read_byte();
                }
                printf("            \"%s\"\n", str);
                free(str);
            }
        }
    }
}

u32 stats_action[255];
u32 stats_action_maps[255][255];
u32 stats_action_map_num[255];
u32 stats_command[255];
u32 stats_command_maps[255][255];
u32 stats_command_map_num[255];

void read_iact_stats(u16 map_num, u32 location, u16 num_iacts)
{
    //printf("Reading IACT data, %u IACTs\n", num_iacts);
    seek(location);
    for(int i = 0; i < num_iacts; i++)
    {
        read_long(); //IACT
        u32 ignored6 = read_long();
        u16 iactItemCount1 = read_short();
        //printf("\n    Action: unknown %08x, actions %d\n", ignored6, iactItemCount1);
        for (u16 k = 0; k < iactItemCount1; k++)
        {
            char pos_str[7];
            u16 command = read_short();
            u16 arg1 = read_short();
            u16 arg2 = read_short();
            u16 arg3 = read_short();
            u16 arg4 = read_short();
            u16 arg5 = read_short();
            u16 arg6 = read_short();

            *(u16*)(pos_str) = arg4;
            *(u16*)(pos_str+2) = arg5;
            *(u16*)(pos_str+4) = arg6;

            stats_action[command]++;
            if(stats_action_map_num[command] > 0)
            {
                if(stats_action_maps[command][stats_action_map_num[command] - 1] != map_num)
                    stats_action_maps[command][stats_action_map_num[command]++] = map_num;
            }
            else
            {
                stats_action_maps[command][stats_action_map_num[command]++] = map_num;
            }
            //printf("        %s, %04x, %04x, %04x, %04x, %04x, %04x, %s\n", triggers[command], arg1, arg2, arg3, arg4, arg5, arg6, pos_str);
        }
        u16 iactItemCount2 = read_short();
        //printf("    Script: commands %d\n", iactItemCount2);
        for(u16 k = 0; k < iactItemCount2; k++)
        {
            char pos_str[7];
            u16 command = read_short();
            u16 arg1 = read_short();
            u16 arg2 = read_short();
            u16 arg3 = read_short();
            u16 arg4 = read_short();
            u16 arg5 = read_short();
            u16 strlen = read_short();

            *(u16*)(pos_str) = arg3;
            *(u16*)(pos_str+2) = arg4;
            *(u16*)(pos_str+4) = arg5;

            stats_command[command]++;
            if(stats_command_map_num[command] > 0)
            {
                if(stats_command_maps[command][stats_command_map_num[command] - 1] != map_num)
                    stats_command_maps[command][stats_command_map_num[command]++] = map_num;
            }
            else
            {
                stats_command_maps[command][stats_command_map_num[command]++] = map_num;
            }

            //printf("        %s, %04x, %04x, %04x, %04x, %04x, %04x, %s\n", commands[command], arg1, arg2, arg3, arg4, arg5, strlen, pos_str);
            if (strlen)
            {
                char* str = malloc(strlen+1);
                for (u16 l = 0; l < strlen; l++)
                {
                    str[l] = read_byte();
                }
                //printf("            \"%s\"\n", str);
                free(str);
            }
        }
    }
}

void print_iact_stats()
{
    for(int i = 0; i < 0x24; i++)
    {
        printf("%x: %u,    ", i, stats_action[i]);
        for(int j = 0; j < stats_action_map_num[i]; j++)
        {
            if(stats_action[i] < 50)
                printf("%u, ", stats_action_maps[i][j]);
        }
        printf("\n");
    }

    printf("\n");

    for(int i = 0; i < 0x26; i++)
    {
        printf("%x: %u,   ", i, stats_command[i]);
        for(int j = 0; j < stats_command_map_num[i]; j++)
        {
            if(stats_command[i] < 50)
                printf("%u, ", stats_command_maps[i][j]);
        }
        printf("\n");
    }
}

void render_map()
{
    for(int i = 0; i < 9; i++)
    {
        for(int j = 0; j < 9; j++)
        {
            tiles_low[(j*9)+i] = map_tiles_low[((camera_y+j)*width)+i+camera_x];
            tiles_middle[(j*9)+i] = map_tiles_middle[((camera_y+j)*width)+i+camera_x];
            tiles_high[(j*9)+i] = map_tiles_high[((camera_y+j)*width)+i+camera_x];
            tiles_overlay[(j*9)+i] = map_overlay[((camera_y+j)*width)+i+camera_x];
        }
    }

    if(player_entity.current_frame >= 2)
    {
        if(player_entity.y >= camera_y &&
            player_entity.y < (camera_y + 9) &&
            player_entity.x >= camera_x &&
            player_entity.x < (camera_x + 9))
        {
            tiles_high[((player_entity.y - camera_y)*9) + (player_entity.x - camera_x)] = char_data[player_entity.char_id]->frames[player_entity.current_frame];
        }
    }

    for(int i = 0; i < num_entities; i++)
    {
        entity *e = entities[i];
        if(e->current_frame < 2)
            continue; //These frames are transparent/unused in Yoda Stories, not sure for Indy

        if(e->y >= camera_y &&
            e->y < camera_y + 9 &&
            e->x >= camera_x &&
            e->x < camera_x + 9)
        {
            tiles_high[((e->y - camera_y)*9) + (e->x - camera_x)] = char_data[e->char_id]->frames[e->current_frame];

            if(e->num_items > 0)
                tiles_overlay[((e->y - camera_y)*9) + (e->x - camera_x)] = e->item;
        }
    }

    //Test for rendering characters
    /*for(int i = 0; i < 26; i++)
    {
        tiles_high[i] = char_data[id]->frames[i];
    }*/
}

void update_world(double delta)
{
    //TODO
}
