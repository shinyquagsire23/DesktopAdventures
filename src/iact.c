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
#include "input.h"
#include "tile.h"
#include "sound.h"
#include "assets.h"
#include "player.h"
#include "screen.h"

#ifdef PC_BUILD
#define log(f_, ...) printf((f_), __VA_ARGS__)
#elif WIIU
    #include <coreinit/debug.h>
    #define log(f_, ...) OSReport((f_), __VA_ARGS__)
#endif

void print_iact(u32 loc);

char triggers[0x24][30] = { "FirstEnter", "Enter", "BumpTile", "DragItem", "Walk", "TempVarEq", "RandVarEq", "RandVarGt", "RandVarLs", "EnterVehicle", "CheckMapTile", "EnemyDead", "AllEnemiesDead", "HasItem", "CheckEndItem", "CheckStartItem", "Unk10", "GameInProgress?", "GameCompleted?", "HealthLs", "HealthGt", "Unk15", "Unk16", "DragWrongItem", "PlayerAtPos", "GlobalVarEq", "GlobalVarLs", "GlobalVarGt", "ExperienceEq", "Unk1d", "Unk1e", "TempVarNe", "RandVarNe", "GlobalVarNe", "CheckMapTileVar", "ExperienceGt"};
char commands[0x26][30] = { "SetMapTile", "ClearTile", "MoveMapTile", "DrawOverlayTile", "SayText", "ShowText", "RedrawTile", "RedrawTiles", "RenderChanges", "WaitTicks", "PlaySound", "Unk0b", "Random", "SetTempVar", "AddTempVar", "SetMapTileVar", "ReleaseCamera", "LockCamera", "SetPlayerPos", "MoveCamera", "FlagOnce", "ShowObject", "HideObject", "ShowEntity", "HideEntity", "ShowAllEntities", "HideAllEntities", "SpawnItem", "AddItemToInv", "RemoveItemFromInv", "Open?Show?", "Unk1f", "Unk20", "WarpToMap", "SetGlobalVar", "AddGlobalVar", "SetRandVar", "AddHealth"};

u16 iact_trig_clear_exempt[0x24];
u16 active_triggers[0x24][8];

void item_select_prompt(u16 x, u16 y, u16 item)
{
    bool show = false;
    int ticks = 0;

    int center_shift_x = map_get_width() < SCREEN_TILE_WIDTH ? ((SCREEN_TILE_WIDTH - map_get_width()) / 2)*32 : 0;
    int center_shift_y = map_get_height() < SCREEN_TILE_HEIGHT ? ((SCREEN_TILE_HEIGHT - map_get_height()) / 2)*32 : 0;

    int item_screen_x = ((x - map_camera_x)*32) + center_shift_x;
    int item_screen_y = ((y - map_camera_y)*32) + center_shift_y;

    while(1)
    {
        if(!(BUTTON_LCLICK_STATE && MOUSE_X > item_screen_x && MOUSE_Y > item_screen_y && MOUSE_X < item_screen_x+32 && MOUSE_Y < item_screen_y+32))
            break;

        map_set_tile(LAYER_HIGH, x, y, show?item:TILE_NONE);
        render_map();

        if(ticks > (TARGET_TICK_FPS/2))
        {
            show = !show;
            ticks = 0;
        }

        draw_screen();
        reset_input_state();
        update_input();
        usleep(1000*(1000/60));
        ticks++;
    }

    while(1)
    {
        if(BUTTON_FIRE_STATE || (BUTTON_LCLICK_STATE && MOUSE_X > item_screen_x && MOUSE_Y > item_screen_y && MOUSE_X < item_screen_x+32 && MOUSE_Y < item_screen_y+32))
        {
            map_set_tile(LAYER_HIGH, x, y, TILE_NONE);
            render_map();

            player_add_item_to_inv(item);
            break;
        }

        map_set_tile(LAYER_HIGH, x, y, show?item:TILE_NONE);
        render_map();

        if(ticks > (TARGET_TICK_FPS/2))
        {
            show = !show;
            ticks = 0;
        }

        draw_screen();
        reset_input_state();
        update_input();
        usleep(1000*(1000/60));
        ticks++;
    }

    usleep(1000*(1000/TARGET_TICK_FPS));
}

void read_iact()
{
    log("Reading IACT data, %u IACTs\n", zone_data[map_get_id()]->num_iacts);
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
    log("\n    Action: size %08x, actions %d\n", length, iactItemCount1);
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

        log("        %s, %04x, %04x, %04x, %04x, %04x, %04x, %s\n", triggers[command], args[0], args[1], args[2], args[3], args[4], args[5], pos_str);
    }
    u16 iactItemCount2 = read_short();
    log("    Script: commands %d\n", iactItemCount2);
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

        log("        %s, %04x, %04x, %04x, %04x, %04x, %04x, %s\n", commands[command], args[0], args[1], args[2], args[3], args[4], strlen, pos_str);
        if (strlen)
        {
            char* str = malloc(strlen+1);
            for (u16 l = 0; l < strlen; l++)
            {
                str[l] = read_byte();
            }
            str[strlen] = 0;
            log("            \"%s\"\n", str);
            free(str);
        }
    }
}

void show_textbox(int x, int y, char *text)
{
    active_text = text;
    active_text_x = (x - map_camera_x) * 32;
    active_text_y = (y - map_camera_y) * 32;

    draw_screen();

    while(1)
    {
        if(BUTTON_FIRE_STATE)
        {
            break; //TODO: Scroll text
        }

        draw_screen();
        reset_input_state();
        update_input();
        usleep(1000*(1000/60));
    }

    while(1)
    {
        if(!BUTTON_FIRE_STATE)
        {
            break;
        }

        draw_screen();
        reset_input_state();
        update_input();
        usleep(1000*(1000/60));
    }

    draw_screen();

    active_text = NULL;
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
            case IACT_CMD_DrawOverlayTile:
                if(args[1] >= map_camera_y && args[0] >= map_camera_x && args[0]-map_camera_x < SCREEN_TILE_WIDTH && args[1]-map_camera_y < SCREEN_TILE_HEIGHT)
                    tiles_high[(((args[1]-map_camera_y)+(map_get_height() < SCREEN_TILE_HEIGHT ? (SCREEN_TILE_HEIGHT - map_get_height()) / 2 : 0))*SCREEN_TILE_WIDTH)+(args[0]-map_camera_x)+(map_get_width() < SCREEN_TILE_WIDTH ? (SCREEN_TILE_WIDTH - map_get_width()) / 2 : 0)] = args[2];
                break;
            case IACT_CMD_SayText: //TODO
                log("Luke says: %s\n", string);
                show_textbox(player_entity.x,player_entity.y,string);
                break;
            case IACT_CMD_ShowText: //TODO
                log("Someone says: %s\n", string);
                show_textbox(args[0],args[1],string);
                break;
            case IACT_CMD_RedrawTile:
            case IACT_CMD_RedrawTiles:
                render_map();
                break;
            case IACT_CMD_RenderChanges:
                draw_screen();
                break;
            case IACT_CMD_WaitTicks:
                for(int i = 0; i < args[0]; i++)
                {
                    if(SCREEN_FADE_LEVEL > 0)
                    {
                        SCREEN_FADE_LEVEL--;

                        map_update_camera(false);
                        render_map();
                        draw_screen();
                    }

                    usleep(1000*(1000/TARGET_TICK_FPS));
                }

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
                map_set_rand_var((random() % args[0]) + 1);
                break;
            case IACT_CMD_SetTempVar:
                map_set_temp_var(args[0]);
                break;
            case IACT_CMD_AddTempVar:
                map_set_temp_var(map_get_temp_var() + args[0]);
                break;
            case IACT_CMD_ReleaseCamera:
                map_camera_locked = false;
                player_entity.is_active_visible = false;
                break;
            case IACT_CMD_LockCamera:
                map_camera_locked = true;
                player_entity.is_active_visible = true;
                break;
            case IACT_CMD_SetPlayerPos:
                player_entity.x = args[0];
                player_entity.y = args[1];

                if(map_camera_locked)
                    map_update_camera(false);

                player_position_updated = true;
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
                if(args[0] < map_get_num_objects())
                    map_get_object_by_id(args[0])->visible = true;
                break;
            case IACT_CMD_HideObject:
                if(args[0] < map_get_num_objects())
                    map_get_object_by_id(args[0])->visible = false;
                break;
            case IACT_CMD_ShowEntity:
                map_show_entity(args[0]);
                break;
            case IACT_CMD_HideEntity:
                map_hide_entity(args[0]);
                break;
            case IACT_CMD_ShowAllEntities:
                map_show_all_entities();
                break;
            case IACT_CMD_HideAllEntities:
                map_hide_all_entities();
                break;
            case IACT_CMD_SpawnItem:
                item_select_prompt(args[1], args[2], args[0]);
                break;
            case IACT_CMD_AddItemToInv:
                player_add_item_to_inv(args[0]);
                break;
            case IACT_CMD_RemoveItemFromInv:
                player_remove_item_from_inv(args[0]);
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
                map_set_rand_var(args[0]);
                break;
            case IACT_CMD_AddHealth:
                player_entity.health += args[0];
                break;
            default:
                log("Unhandled script command %s, args: %x %x %x %x %x, strlen %x\n", commands[command], args[0], args[1], args[2], args[3], args[4], strlen);
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
                        if(map_get_rand_var() > args[0])
                            conditions_met = true;
                        break;
                    case IACT_TRIG_RandVarLs:
                        if(map_get_rand_var() < args[0])
                            conditions_met = true;
                        break;
                    case IACT_TRIG_RandVarEq:
                        if(map_get_rand_var() == args[0])
                            conditions_met = true;
                        break;
                    case IACT_TRIG_RandVarNe:
                        if(map_get_rand_var() != args[0])
                            conditions_met = true;
                        break;
                    case IACT_TRIG_TempVarEq:
                        if(map_get_temp_var() == args[0])
                            conditions_met = true;
                        break;
                    case IACT_TRIG_TempVarNe:
                        if(map_get_temp_var() != args[0])
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
                    case IACT_TRIG_GlobalVarEq:
                        if(map_get_global_var() == args[0])
                            conditions_met = true;
                        break;
                    case IACT_TRIG_GlobalVarNe:
                        if(map_get_global_var() != args[0])
                            conditions_met = true;
                        break;
                    case IACT_TRIG_ExperienceEq:
                        if(player_experience == args[0])
                            conditions_met = true;
                        break;
                    case IACT_TRIG_ExperienceGt:
                        if(player_experience > args[0])
                            conditions_met = true;
                        break;
                    case IACT_TRIG_CheckMapTile:
                    case IACT_TRIG_CheckMapTileVar:
                        if(map_get_tile(args[3], args[1], args[2]) == args[0])
                            conditions_met = true;
                        break;
                    case IACT_TRIG_EnemyDead:
                        if(!map_is_entity_active_visible(args[0]))
                            conditions_met = true;
                        break;
                    case IACT_TRIG_AllEnemiesDead:
                        conditions_met = !map_all_entities_active_visible();
                        break;
                    case IACT_TRIG_HasItem:
                        conditions_met = player_has_item(args[0]);
                        break;
                    case IACT_TRIG_DragWrongItem:
                        if(active_triggers[IACT_TRIG_DragItem][0])
                        {
                            if(args[0] == active_triggers[IACT_TRIG_DragItem][0+2] && args[1] == active_triggers[IACT_TRIG_DragItem][1+2] && args[2] == active_triggers[IACT_TRIG_DragItem][2+2] && args[3] == active_triggers[IACT_TRIG_DragItem][3+2] && args[4] != active_triggers[IACT_TRIG_DragItem][4+2])
                                conditions_met = true;
                        }
                        break;
                    case IACT_TRIG_GameInProgress_MAYBE:
                        conditions_met = true; //TODO
                        break;
                    case IACT_TRIG_GameCompleted_MAYBE:
                        conditions_met = false; //TODO
                        break;
                    case IACT_TRIG_CheckStartItem:
                        //TODO: This is a hack! Dagobah relies on this command to determine which planet is linked to each mission.
                        //      This should be implemented along with random world generation.

                        if(!load_demo)
                        {
                            //Tatooine
                            /*if(args[0] == 0x063c)
                                conditions_met = true;*/

                            //Ice Planet
                            if (args[0] == 0x058b)
                                conditions_met = true;

                            //Endor
                            /*if(args[0] == 0x0651)
                                conditions_met = true;*/
                        }
                        else
                        {
                            //Ice Planet
                            if(args[0] == 0x058b)
                                conditions_met = true;
                        }
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
            player_update();
            render_map();
            run_iact(zone_data[map_get_id()]->iact_offsets[i], i);
        }
    }

    for(int i = 0; i < 0x24; i++)
    {
        if(iact_trig_clear_exempt[i]) continue;

        for(int j = 0; j < 8; j++)
            active_triggers[i][j] = 0;
    }

    for(int i = 0; i < 0x24; i++)
    {
        iact_trig_clear_exempt[i] = false;
    }
}
