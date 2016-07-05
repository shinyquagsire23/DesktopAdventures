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

#include "iact.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "map.h"
#include "assets.h"

char triggers[0x24][30] = { "FirstEnter", "Enter", "BumpTile", "DragItem", "Walk", "TempVarEq", "RandVarEq", "RandVarGt", "RandVarLs", "EnterVehicle", "CheckMapTile", "EnemyDead", "AllEnemiesDead", "HasItem", "HasEndItem", "Unk0f", "Unk10", "GameInProgress?", "GameCompleted?", "HealthLs", "HealthGt", "Unk15", "Unk16", "DragWrongItem", "PlayerAtPos", "GlobalVarEq", "GlobalVarLs", "GlobalVarGt", "ExperienceEq", "Unk1d", "Unk1e", "TempVarNe", "RandVarNe", "GlobalVarNe", "CheckMapTileVar", "ExperienceGt"};
char commands[0x26][30] = { "SetMapTile", "ClearTile", "MoveMapTile", "DrawOverlayTile", "SayText", "ShowText", "RedrawTile", "RedrawTiles", "RenderChanges", "WaitSecs", "PlaySound", "Unk0b", "Random", "SetTempVar", "AddTempVar", "SetMapTileVar", "ReleaseCamera", "LockCamera", "SetPlayerPos", "MoveCamera", "Redraw", "OpenDoor?", "CloseDoor?", "EnemySpawn", "NPCSpawn", "RemoveDraggedItem", "RemoveDraggedItemSimilar?", "SpawnItem", "AddItemToInv", "DropItem", "Open?Show?", "Unk1f", "Unk20", "WarpToMap", "SetGlobalVar", "AddGlobalVar", "SetRandVar", "AddHealth"};

u16 active_triggers[0x24][8];

void read_iact()
{
    printf("Reading IACT data, %u IACTs\n", zone_data[map_get_id()]->num_iacts);
    for(int i = 0; i < zone_data[map_get_id()]->num_iacts; i++)
    {
        //print_iact(zone_data[map_get_id()]->iact_offsets[i]);
    }
}

void print_iact(u32 loc)
{
    seek(loc);
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

void iact_set_trigger(u8 trigger, u8 count, ...)
{
    va_list args;
    va_start (args, count);

    active_triggers[trigger][0] = true;
    active_triggers[trigger][1] = count;
    for(int i = 0; i < count; i++)
        active_triggers[trigger][i+2] = va_arg(args, u16);

    va_end(args);
}

void iact_update()
{
    for(int i = 0; i < zone_data[map_get_id()]->num_iacts; i++)
    {
        seek(zone_data[map_get_id()]->iact_offsets[i]);
        read_long(); //IACT
        u32 length = read_long();
        u16 iactItemCount1 = read_short();
        bool conditions_met = true;
        for (u16 k = 0; k < iactItemCount1; k++)
        {
            u16 args[6];
            u16 command = read_short();
            for(int j = 0; j < 6; j++)
                args[j] = read_short();

            //TODO: Comparison triggers don't work well with this model
            if(!active_triggers[command][0])
            {
                conditions_met = false;
                break;
            }

            for(int j = 0; j < active_triggers[command][1]; j++)
            {
                if(args[j] != active_triggers[command][j+2])
                {
                    conditions_met = false;
                    break;
                }
            }
        }

        if(conditions_met)
        {
            //TODO: Run the scripts.
            print_iact(zone_data[map_get_id()]->iact_offsets[i]);
        }
    }

    for(int i = 0; i < 0x24; i++)
    {
        for(int j = 0; j < 8; j++)
            active_triggers[i][j] = 0;
    }
}