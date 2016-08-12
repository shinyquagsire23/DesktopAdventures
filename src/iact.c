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
#include <unistd.h>
#include <stdio.h>
#include "map.h"
#include "main.h"
#include "tile.h"
#include "sound.h"
#include "assets.h"
#include "player.h"
#include "screen.h"

void print_iact(u32 loc);

char triggers[0x24][30] = { "FirstEnter", "Enter", "BumpTile", "DragItem", "Walk", "TempVarEq", "RandVarEq", "RandVarGt", "RandVarLs", "EnterVehicle", "CheckMapTile", "EnemyDead", "AllEnemiesDead", "HasItem", "HasEndItem", "Unk0f", "Unk10", "GameInProgress?", "GameCompleted?", "HealthLs", "HealthGt", "Unk15", "Unk16", "DragWrongItem", "PlayerAtPos", "GlobalVarEq", "GlobalVarLs", "GlobalVarGt", "ExperienceEq", "Unk1d", "Unk1e", "TempVarNe", "RandVarNe", "GlobalVarNe", "CheckMapTileVar", "ExperienceGt"};
char commands[0x26][30] = { "SetMapTile", "ClearTile", "MoveMapTile", "DrawOverlayTile", "SayText", "ShowText", "RedrawTile", "RedrawTiles", "RenderChanges", "WaitTicks", "PlaySound", "Unk0b", "Random", "SetTempVar", "AddTempVar", "SetMapTileVar", "ReleaseCamera", "LockCamera", "SetPlayerPos", "MoveCamera", "FlagOnce", "ShowObject", "HideObject", "EnemySpawn", "NPCSpawn", "RemoveDraggedItem", "RemoveDraggedItemSimilar?", "SpawnItem", "AddItemToInv", "DropItem", "Open?Show?", "Unk1f", "Unk20", "WarpToMap", "SetGlobalVar", "AddGlobalVar", "SetRandVar", "AddHealth"};

u16 active_triggers[0x24][8];
u16 IACT_RANDVAR = 0;
u16 IACT_TEMPVAR = 0;

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
        u16 args[6];
        u16 command = read_short();
        for(int j = 0; j < 6; j++)
            args[j] = read_short();

        *(u16*)(pos_str) = args[3];
        *(u16*)(pos_str+2) = args[4];
        *(u16*)(pos_str+4) = args[5];

        printf("        %s, %04x, %04x, %04x, %04x, %04x, %04x, %s\n", triggers[command], args[0], args[1], args[2], args[3], args[4], args[5], pos_str);
    }
    u16 iactItemCount2 = read_short();
    printf("    Script: commands %d\n", iactItemCount2);
    for(u16 k = 0; k < iactItemCount2; k++)
    {
        char pos_str[7];
        u16 args[5];
        u16 command = read_short();
        for(int j = 0; j < 5; j++)
            args[j] = read_short();
        u16 strlen = read_short();

        *(u16*)(pos_str) = args[2];
        *(u16*)(pos_str+2) = args[3];
        *(u16*)(pos_str+4) = args[4];

        printf("        %s, %04x, %04x, %04x, %04x, %04x, %04x, %s\n", commands[command], args[0], args[1], args[2], args[3], args[4], strlen, pos_str);
        if (strlen)
        {
            char* str = malloc(strlen+1);
            for (u16 l = 0; l < strlen; l++)
            {
                str[l] = read_byte();
            }
            str[strlen] = 0;
            printf("            \"%s\"\n", str);
            free(str);
        }
    }
}

void run_iact(u32 loc, int iact_id)
{
    seek(loc);
    read_long(); //IACT
    read_long(); //len
    u16 iactItemCount1 = read_short();
    seek_add(iactItemCount1*7*sizeof(u16));

    u16 iactItemCount2 = read_short();
    for(u16 k = 0; k < iactItemCount2; k++)
    {
        u16 args[5];
        u16 command = read_short();
        for(int j = 0; j < 5; j++)
            args[j] = read_short();
        u16 strlen = read_short();
        char *string = get_strn(strlen);

        switch(command)
        {
            case IACT_CMD_SetMapTileVar:
            case IACT_CMD_SetMapTile:
                map_set_tile(args[2], args[0], args[1], args[3]);
                break;
            case IACT_CMD_ClearTile:
                map_set_tile(args[2], args[0], args[1], TILE_NONE);
                break;
            case IACT_CMD_MoveMapTile:
                map_set_tile(args[2], args[3], args[4], map_get_tile(args[2], args[0], args[1]));
                map_set_tile(args[2], args[0], args[1], TILE_NONE);
                break;
            case IACT_CMD_SayText: //TODO
                printf("Luke says: %s\n", string);
                break;
            case IACT_CMD_ShowText: //TODO
                printf("Someone says: %s\n", string);
                break;
            case IACT_CMD_RedrawTile:
            case IACT_CMD_RedrawTiles:
                //TODO? This might be able to stay nopped as long as they don't abuse this command somehow.
                break;
            case IACT_CMD_RenderChanges:
                render_map();
                redraw_swap_buffers();
                break;
            case IACT_CMD_WaitTicks:
                usleep(1000*(1000/TARGET_TICK_FPS)*args[0]);
                break;
            case IACT_CMD_PlaySound:
                sound_play(args[0]);
                break;
            case IACT_CMD_TransitionIn:
                map_update_camera(true);
                screen_transition_in();
                PLAYER_MAP_CHANGE_REASON = MAP_CHANGE_NONE;
                break;
            case IACT_CMD_Random:
                //IACT_RANDVAR = (random() % args[0]) + 1;
                break;
            case IACT_CMD_SetTempVar:
                IACT_TEMPVAR = args[0];
                break;
            case IACT_CMD_AddTempVar:
                IACT_TEMPVAR += args[0];
                break;
            case IACT_CMD_ReleaseCamera:
                map_camera_locked = false;
                break;
            case IACT_CMD_LockCamera:
                map_camera_locked = true;
                break;
            case IACT_CMD_SetPlayerPos:
                player_entity.x = args[0];
                player_entity.y = args[1];
                break;
            case IACT_CMD_MoveCamera:
                //TODO: Actual movement using first two args
                map_camera_x = args[2];
                map_camera_y = args[3];
                break;
            case IACT_CMD_FlagOnce:
                map_set_iact_flagonce(iact_id, true);
                break;
            case IACT_CMD_ShowObject:
                map_get_object_by_id(args[0])->visible = true;
                break;
            case IACT_CMD_HideObject:
                map_get_object_by_id(args[0])->visible = false;
                break;
            case IACT_CMD_WarpToMap:
                player_entity.x = args[1];
                player_entity.y = args[2];
                PLAYER_MAP_CHANGE_REASON = MAP_CHANGE_SCRIPT;

                unload_map();
                load_map(args[0]);
                break;
            case IACT_CMD_SetGlobalVar:
                map_set_global_var(args[0]);
                break;
            case IACT_CMD_AddGlobalVar:
                map_set_global_var(map_get_global_var() + args[0]);
                break;
            case IACT_CMD_SetRandVar:
                IACT_RANDVAR = args[0];
                break;
            case IACT_CMD_AddHealth:
                player_entity.health += args[0];
                break;
            default:
                printf("Unhandled script command %s, args: %x %x %x %x %x, strlen %x\n", commands[command], args[0], args[1], args[2], args[3], args[4], strlen);
                break;
        }
    }
}

void iact_set_trigger(u8 trigger, u8 count, ...)
{
    va_list args;
    va_start (args, count);

    active_triggers[trigger][0] = true;
    active_triggers[trigger][1] = count;
    for(int i = 0; i < count; i++)
        active_triggers[trigger][i+2] = (u16)va_arg(args, int);

    va_end(args);
}

void iact_update()
{
    for(int i = 0; i < zone_data[map_get_id()]->num_iacts; i++)
    {
        seek(zone_data[map_get_id()]->iact_offsets[i]);
        read_long(); //IACT
        read_long(); //len
        u16 iactItemCount1 = read_short();
        bool conditions_met = true;
        for (u16 k = 0; k < iactItemCount1; k++)
        {
            u16 args[6];
            u16 command = read_short();
            for(int j = 0; j < 6; j++)
                args[j] = read_short();

            if(!active_triggers[command][0])
            {
                conditions_met = false;
                switch(command)
                {
                    case IACT_TRIG_RandVarGt:
                        if(IACT_RANDVAR > args[0])
                            conditions_met = true;
                        break;
                    case IACT_TRIG_RandVarLs:
                        if(IACT_RANDVAR < args[0])
                            conditions_met = true;
                        break;
                    case IACT_TRIG_RandVarEq:
                        if(IACT_RANDVAR == args[0])
                            conditions_met = true;
                        break;
                    case IACT_TRIG_RandVarNe:
                        if(IACT_RANDVAR != args[0])
                            conditions_met = true;
                        break;
                    case IACT_TRIG_TempVarEq:
                        if(IACT_TEMPVAR == args[0])
                            conditions_met = true;
                        break;
                    case IACT_TRIG_TempVarNe:
                        if(IACT_TEMPVAR != args[0])
                            conditions_met = true;
                        break;
                    case IACT_TRIG_HealthGt:
                        if(player_entity.health > args[0])
                            conditions_met = true;
                        break;
                    case IACT_TRIG_HealthLs:
                        if(player_entity.health < args[0])
                            conditions_met = true;
                        break;
                    case IACT_TRIG_GlobalVarGt:
                        if(map_get_global_var() > args[0])
                            conditions_met = true;
                        break;
                    case IACT_TRIG_GlobalVarLs:
                        if(map_get_global_var() < args[0])
                            conditions_met = true;
                        break;
                    case IACT_TRIG_GlobalVarNe:
                        if(map_get_global_var() != args[0])
                            conditions_met = true;
                        break;
                    case IACT_TRIG_CheckMapTile:
                    case IACT_TRIG_CheckMapTileVar:
                        if(map_get_tile(args[3], args[1], args[2]) == args[0])
                            conditions_met = true;
                        break;
                }

                if(!conditions_met)
                    break;
            }
            else
            {
                for(int j = 0; j < active_triggers[command][1]; j++)
                {
                    if(args[j] != active_triggers[command][j+2])
                    {
                        conditions_met = false;
                        break;
                    }
                }

                if(!conditions_met)
                    break;
            }
        }

        if(conditions_met && !map_get_iact_flagonce(i))
        {
            //print_iact(zone_data[map_get_id()]->iact_offsets[i]);
            run_iact(zone_data[map_get_id()]->iact_offsets[i], i);;
        }
    }

    for(int i = 0; i < 0x24; i++)
    {
        for(int j = 0; j < 8; j++)
            active_triggers[i][j] = 0;
    }
}
