/*  DesktopAdventures, A reimplementation of the Desktop Adventures game engine
 *
 *  DesktopAdventures is the legal property of its developers, whose names
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

#include "player.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "map.h"
#include "iact.h"
#include "tile.h"
#include "input.h"
#include "assets.h"
#include "sound.h"
#include "objectinfo.h"

int last_dir = -1;
int anim_count = 0;
bool moving = false;
bool player_position_updated = false;

u16 *player_inventory;
u16 player_inventory_count;
u16 PLAYER_EQUIPPED_ITEM = -1;

u16 player_experience = 0;

u8 PLAYER_MAP_CHANGE_REASON = 0;
u16 PLAYER_MAP_CHANGE_TO = 0;
u16 DOOR_IN_x = 0;
u16 DOOR_IN_y = 0;
u16 DOOR_IN_map = 0;

bool player_collides(int dir, int x, int y)
{
    switch(dir)
    {
        case LEFT:
            if(((!(map_get_meta(LAYER_MIDDLE, player_entity.x-1,player_entity.y) & (TILE_MIDDLE_LAYER_COLLIDING | TILE_GAME_OBJECT))
                || map_get_tile(LAYER_MIDDLE, player_entity.x-1, player_entity.y) == TILE_NONE))
               && player_entity.x != 0)
                return false;
            break;
        case RIGHT:
            if(((!(map_get_meta(LAYER_MIDDLE,player_entity.x+1, player_entity.y) & (TILE_MIDDLE_LAYER_COLLIDING | TILE_GAME_OBJECT))
                || map_get_tile(LAYER_MIDDLE,player_entity.x+1, player_entity.y) == TILE_NONE))
               && player_entity.x != map_get_width()-1)
                return false;
            break;
        case UP:
            if(((!(map_get_meta(LAYER_MIDDLE,player_entity.x, player_entity.y-1) & (TILE_MIDDLE_LAYER_COLLIDING | TILE_GAME_OBJECT))
                || map_get_tile(LAYER_MIDDLE,player_entity.x, player_entity.y-1) == TILE_NONE))
               )
                return false;
            break;
        case DOWN:
            if(((!(map_get_meta(LAYER_MIDDLE,player_entity.x, player_entity.y+1) & (TILE_MIDDLE_LAYER_COLLIDING | TILE_GAME_OBJECT))
                || map_get_tile(LAYER_MIDDLE,player_entity.x, player_entity.y+1) == TILE_NONE))
               && player_entity.y != map_get_height()-1)
                return false;
            break;
        case UP_LEFT:
            if(((!(map_get_meta(LAYER_MIDDLE, player_entity.x-1,player_entity.y-1) & (TILE_MIDDLE_LAYER_COLLIDING | TILE_GAME_OBJECT))
                || map_get_tile(LAYER_MIDDLE, player_entity.x-1, player_entity.y-1) == TILE_NONE))
               && (player_entity.x != 0 && player_entity.y != 0))
                return false;
            break;
        case UP_RIGHT:
            if(((!(map_get_meta(LAYER_MIDDLE,player_entity.x+1, player_entity.y-1) & (TILE_MIDDLE_LAYER_COLLIDING | TILE_GAME_OBJECT))
                || map_get_tile(LAYER_MIDDLE,player_entity.x+1, player_entity.y-1) == TILE_NONE))
               && (player_entity.x != map_get_width()-1 && player_entity.y != 0))
                return false;
            break;
        case DOWN_LEFT:
            if(((!(map_get_meta(LAYER_MIDDLE, player_entity.x-1,player_entity.y+1) & (TILE_MIDDLE_LAYER_COLLIDING | TILE_GAME_OBJECT))
                || map_get_tile(LAYER_MIDDLE, player_entity.x-1, player_entity.y+1) == TILE_NONE))
               && (player_entity.x != 0&& player_entity.y != map_get_height()-1))
                return false;
            break;
        case DOWN_RIGHT:
            if(((!(map_get_meta(LAYER_MIDDLE,player_entity.x+1, player_entity.y+1) & (TILE_MIDDLE_LAYER_COLLIDING | TILE_GAME_OBJECT))
                || map_get_tile(LAYER_MIDDLE,player_entity.x+1, player_entity.y+1) == TILE_NONE))
               && (player_entity.x != map_get_width()-1 && player_entity.y != map_get_height()-1))
                return false;
            break;
    }
    return true;
}

bool player_collides_event(int dir, int x, int y)
{
    switch(dir)
    {
        case LEFT:
            if(map_get_meta(LAYER_MIDDLE, player_entity.x-1,player_entity.y) & TILE_GAME_OBJECT)
                return true;
            break;
        case RIGHT:
            if(map_get_meta(LAYER_MIDDLE,player_entity.x+1, player_entity.y) & TILE_GAME_OBJECT)
                return true;
            break;
        case UP:
            if(map_get_meta(LAYER_MIDDLE,player_entity.x, player_entity.y-1) & TILE_GAME_OBJECT)
                return true;
            break;
        case DOWN:
            if(map_get_meta(LAYER_MIDDLE,player_entity.x, player_entity.y+1) & TILE_GAME_OBJECT)
                return true;
            break;
        case UP_LEFT:
            if(map_get_meta(LAYER_MIDDLE, player_entity.x-1,player_entity.y-1) & TILE_GAME_OBJECT)
                return true;
            break;
        case UP_RIGHT:
            if(map_get_meta(LAYER_MIDDLE,player_entity.x+1, player_entity.y-1) & TILE_GAME_OBJECT)
                return true;
            break;
        case DOWN_LEFT:
            if(map_get_meta(LAYER_MIDDLE, player_entity.x-1,player_entity.y+1) & TILE_GAME_OBJECT)
                return true;
            break;
        case DOWN_RIGHT:
            if(map_get_meta(LAYER_MIDDLE,player_entity.x+1, player_entity.y+1) & TILE_GAME_OBJECT)
                return true;
            break;
    }
    return false;
}

bool player_do_push(int dir)
{
    if(BUTTON_PUSH_STATE)
    {
        switch(dir)
        {
            case LEFT:
                if((map_get_meta(LAYER_MIDDLE, player_entity.x-1,player_entity.y) & (TILE_PUSH_PULL_BLOCK)) && player_entity.x > 1 && map_get_tile(LAYER_MIDDLE, player_entity.x-1,player_entity.y) != TILE_NONE)
                {
                    if(map_get_tile(LAYER_MIDDLE, player_entity.x-2,player_entity.y) == TILE_NONE)
                    {
                        map_set_tile(LAYER_MIDDLE, player_entity.x-2, player_entity.y, map_get_tile(LAYER_MIDDLE, player_entity.x-1,player_entity.y));

                        int object_index = 0;
                        while(1)
                        {
                            obj_info *object = map_get_object(object_index++, player_entity.x-1, player_entity.y);
                            if (object == NULL)
                                break;

                            if(!object->visible) continue;

                            if(object->type == OBJ_ITEM || object->type == OBJ_WEAPON)
                            {
                                object->visible = false;
                                map_set_tile(LAYER_MIDDLE, player_entity.x-1, player_entity.y, object->arg);
                                return true;
                            }
                        }

                        map_set_tile(LAYER_MIDDLE, player_entity.x-1, player_entity.y, TILE_NONE);
                        return true;
                    }
                }
                break;
            case RIGHT:
                if((map_get_meta(LAYER_MIDDLE, player_entity.x+1,player_entity.y) & (TILE_PUSH_PULL_BLOCK)) && player_entity.x != map_get_width()-2 && map_get_tile(LAYER_MIDDLE, player_entity.x+1,player_entity.y) != TILE_NONE)
                {
                    if(map_get_tile(LAYER_MIDDLE, player_entity.x+2,player_entity.y) == TILE_NONE)
                    {
                        map_set_tile(LAYER_MIDDLE, player_entity.x+2, player_entity.y, map_get_tile(LAYER_MIDDLE, player_entity.x+1,player_entity.y));

                        int object_index = 0;
                        while(1)
                        {
                            obj_info *object = map_get_object(object_index++, player_entity.x+1, player_entity.y);
                            if (object == NULL)
                                break;

                            if(!object->visible) continue;

                            if(object->type == OBJ_ITEM || object->type == OBJ_WEAPON)
                            {
                                object->visible = false;
                                map_set_tile(LAYER_MIDDLE, player_entity.x+1, player_entity.y, object->arg);
                                return true;
                            }
                        }

                        map_set_tile(LAYER_MIDDLE, player_entity.x+1, player_entity.y, TILE_NONE);
                        return true;
                    }
                }
                break;
            case UP:
                if((map_get_meta(LAYER_MIDDLE,player_entity.x, player_entity.y-1) & (TILE_PUSH_PULL_BLOCK)) && player_entity.y > 1 && map_get_tile(LAYER_MIDDLE, player_entity.x,player_entity.y-1) != TILE_NONE)
                {
                    if(map_get_tile(LAYER_MIDDLE, player_entity.x,player_entity.y-2) == TILE_NONE)
                    {
                        map_set_tile(LAYER_MIDDLE, player_entity.x, player_entity.y-2, map_get_tile(LAYER_MIDDLE, player_entity.x,player_entity.y-1));

                        int object_index = 0;
                        while(1)
                        {
                            obj_info *object = map_get_object(object_index++, player_entity.x, player_entity.y-1);
                            if (object == NULL)
                                break;

                            if(!object->visible) continue;

                            if(object->type == OBJ_ITEM || object->type == OBJ_WEAPON)
                            {
                                object->visible = false;
                                map_set_tile(LAYER_MIDDLE, player_entity.x, player_entity.y-1, object->arg);
                                return true;
                            }
                        }

                        map_set_tile(LAYER_MIDDLE, player_entity.x, player_entity.y-1, TILE_NONE);
                        return true;
                    }
                }
                break;
            case DOWN:
                if((map_get_meta(LAYER_MIDDLE,player_entity.x, player_entity.y+1) & (TILE_PUSH_PULL_BLOCK)) && player_entity.y != map_get_height()-2 && map_get_tile(LAYER_MIDDLE, player_entity.x,player_entity.y+1) != TILE_NONE)
                {
                    if(map_get_tile(LAYER_MIDDLE, player_entity.x,player_entity.y+2) == TILE_NONE)
                    {
                        map_set_tile(LAYER_MIDDLE, player_entity.x, player_entity.y+2, map_get_tile(LAYER_MIDDLE, player_entity.x,player_entity.y+1));

                        int object_index = 0;
                        while(1)
                        {
                            obj_info *object = map_get_object(object_index++, player_entity.x, player_entity.y+1);
                            if (object == NULL)
                                break;

                            if(!object->visible) continue;

                            if(object->type == OBJ_ITEM || object->type == OBJ_WEAPON)
                            {
                                object->visible = false;
                                map_set_tile(LAYER_MIDDLE, player_entity.x, player_entity.y+1, object->arg);
                                return true;
                            }
                        }

                        map_set_tile(LAYER_MIDDLE, player_entity.x, player_entity.y+1, TILE_NONE);
                        return true;
                    }
                }
                break;
        }
    }
    return false;
}

bool player_do_pull(int dir)
{
    if(BUTTON_PUSH_STATE)
    {
        switch(dir)
        {
            case LEFT:
                if((map_get_meta(LAYER_MIDDLE, player_entity.x+1,player_entity.y) & (TILE_PUSH_PULL_BLOCK)) && player_entity.x != 0 && map_get_tile(LAYER_MIDDLE, player_entity.x+1,player_entity.y) != TILE_NONE)
                {
                    map_set_tile(LAYER_MIDDLE, player_entity.x, player_entity.y, map_get_tile(LAYER_MIDDLE, player_entity.x+1,player_entity.y));

                    int object_index = 0;
                    while(1)
                    {
                        obj_info *object = map_get_object(object_index++, player_entity.x+1, player_entity.y);
                        if (object == NULL)
                            break;

                        if(!object->visible) continue;

                        if(object->type == OBJ_ITEM || object->type == OBJ_WEAPON)
                        {
                            object->visible = false;
                            map_set_tile(LAYER_MIDDLE, player_entity.x+1, player_entity.y, object->arg);
                            return true;
                        }
                    }

                    map_set_tile(LAYER_MIDDLE, player_entity.x+1, player_entity.y, TILE_NONE);
                    return true;
                }
                break;
            case RIGHT:
                if((map_get_meta(LAYER_MIDDLE, player_entity.x-1,player_entity.y) & (TILE_PUSH_PULL_BLOCK)) && player_entity.x != map_get_width()-1 && map_get_tile(LAYER_MIDDLE, player_entity.x-1,player_entity.y) != TILE_NONE)
                {
                    map_set_tile(LAYER_MIDDLE, player_entity.x, player_entity.y, map_get_tile(LAYER_MIDDLE, player_entity.x-1,player_entity.y));

                    int object_index = 0;
                    while(1)
                    {
                        obj_info *object = map_get_object(object_index++, player_entity.x-1, player_entity.y);
                        if (object == NULL)
                            break;

                        if(!object->visible) continue;

                        if(object->type == OBJ_ITEM || object->type == OBJ_WEAPON)
                        {
                            object->visible = false;
                            map_set_tile(LAYER_MIDDLE, player_entity.x-1, player_entity.y, object->arg);
                            return true;
                        }
                    }

                    map_set_tile(LAYER_MIDDLE, player_entity.x-1, player_entity.y, TILE_NONE);
                    return true;
                }
                break;
            case UP:
                if((map_get_meta(LAYER_MIDDLE,player_entity.x, player_entity.y+1) & (TILE_PUSH_PULL_BLOCK)) && player_entity.y != 0 && map_get_tile(LAYER_MIDDLE, player_entity.x,player_entity.y+1) != TILE_NONE)
                {
                    map_set_tile(LAYER_MIDDLE, player_entity.x, player_entity.y, map_get_tile(LAYER_MIDDLE, player_entity.x,player_entity.y+1));

                    int object_index = 0;
                    while(1)
                    {
                        obj_info *object = map_get_object(object_index++, player_entity.x, player_entity.y+1);
                        if (object == NULL)
                            break;

                        if(!object->visible) continue;

                        if(object->type == OBJ_ITEM || object->type == OBJ_WEAPON)
                        {
                            object->visible = false;
                            map_set_tile(LAYER_MIDDLE, player_entity.x, player_entity.y+1, object->arg);
                            return true;
                        }
                    }

                    map_set_tile(LAYER_MIDDLE, player_entity.x, player_entity.y+1, TILE_NONE);
                    return true;
                }
                break;
            case DOWN:
                if((map_get_meta(LAYER_MIDDLE,player_entity.x, player_entity.y-1) & (TILE_PUSH_PULL_BLOCK)) && player_entity.y != map_get_height()-1 && map_get_tile(LAYER_MIDDLE, player_entity.x,player_entity.y-1) != TILE_NONE)
                {
                    map_set_tile(LAYER_MIDDLE, player_entity.x, player_entity.y, map_get_tile(LAYER_MIDDLE, player_entity.x,player_entity.y-1));

                    int object_index = 0;
                    while(1)
                    {
                        obj_info *object = map_get_object(object_index++, player_entity.x, player_entity.y-1);
                        if (object == NULL)
                            break;

                        if(!object->visible) continue;

                        if(object->type == OBJ_ITEM || object->type == OBJ_WEAPON)
                        {
                            object->visible = false;
                            map_set_tile(LAYER_MIDDLE, player_entity.x, player_entity.y-1, object->arg);
                            return true;
                        }
                    }

                    map_set_tile(LAYER_MIDDLE, player_entity.x, player_entity.y-1, TILE_NONE);
                    return true;
                }
                break;
        }
    }
    return false;
}

void player_goto_door_in()
{
    player_entity.x = DOOR_IN_x;
    player_entity.y = DOOR_IN_y;
}

void player_bump(int dir, int x, int y)
{
    int bump_x = player_entity.x;
    int bump_y = player_entity.y;

    switch(dir)
    {
        case LEFT:
            bump_x--;
            break;
        case RIGHT:
            bump_x++;
            break;
        case UP:
            bump_y--;
            break;
        case DOWN:
            bump_y++;
            break;
        case UP_LEFT:
            bump_x--;
            bump_y--;
            break;
        case UP_RIGHT:
            bump_x++;
            bump_y--;
            break;
        case DOWN_LEFT:
            bump_x--;
            bump_y++;
            break;
        case DOWN_RIGHT:
            bump_x++;
            bump_y++;
            break;
        default:
            break;
    }

    u32 meta = map_get_meta(LAYER_MIDDLE, bump_x, bump_y);
    if(meta & (TILE_ITEM | TILE_WEAPON))
    {
        u16 item = map_get_tile(LAYER_MIDDLE, bump_x, bump_y);
        map_set_tile(LAYER_MIDDLE, bump_x, bump_y, TILE_NONE);

        item_select_prompt(bump_x, bump_y, item);
    }
    else
    {
        iact_set_trigger(IACT_TRIG_BumpTile, 3, bump_x, bump_y, map_get_tile(LAYER_MIDDLE, bump_x, bump_y));
    }
}

void player_stand(int x, int y)
{
    int object_index = 0;
    while(1)
    {
        obj_info *object = map_get_object(object_index++, x, y);
        if(object == NULL)
            break;

        if(!object->visible) continue;

        u32 obj_type = object->type;

        if (obj_type == OBJ_DOOR_IN)
        {
            DOOR_IN_x = x;
            DOOR_IN_y = y;
            DOOR_IN_map = map_get_id();

            PLAYER_MAP_CHANGE_REASON = MAP_CHANGE_DOOR_IN;

            unload_map();
            load_map(object->arg);
        }
        else if (obj_type == OBJ_DOOR_OUT)
        {
            PLAYER_MAP_CHANGE_REASON = MAP_CHANGE_DOOR_OUT;
            PLAYER_MAP_CHANGE_TO = DOOR_IN_map;
        }
        else if(obj_type == OBJ_XWING_FROM)
        {
            PLAYER_MAP_CHANGE_REASON = MAP_CHANGE_XWING_FROM;
            PLAYER_MAP_CHANGE_TO = object->arg;
        }
        else if(obj_type == OBJ_XWING_TO)
        {
            PLAYER_MAP_CHANGE_REASON = MAP_CHANGE_XWING_TO;
            PLAYER_MAP_CHANGE_TO = object->arg;
        }
    }

    iact_set_trigger(IACT_TRIG_Walk, 2, player_entity.x, player_entity.y);
}

void player_face(int dir)
{
    last_dir = dir;
}

void player_move(int dir)
{
    if(player_entity.attacking) return;

    if(dir != last_dir)
        anim_count = 0;

    moving = true;

    bool moved = true;

    bool pull = false;
    bool push = player_do_push(dir);

    switch(dir)
    {
        case LEFT:
            if(!player_collides(LEFT, player_entity.x,player_entity.y))
            {
                if(!push)
                    pull = player_do_pull(LEFT);
                player_entity.x--;
            }
            else if(player_collides_event(LEFT, player_entity.x, player_entity.y))
            {
                moved = false;
            }
            else if(!player_collides(UP_LEFT, player_entity.x,player_entity.y) && player_collides(DOWN_LEFT, player_entity.x,player_entity.y))
            {
                player_entity.y--;
                player_entity.x--;
                dir = UP_LEFT;
            }
            else if(!player_collides(DOWN_LEFT, player_entity.x,player_entity.y) && player_collides(UP_LEFT, player_entity.x,player_entity.y))
            {
                player_entity.y++;
                player_entity.x--;
                dir = DOWN_LEFT;
            }
            else
                moved = false;
            break;
        case RIGHT:
            if(!player_collides(RIGHT, player_entity.x,player_entity.y))
            {
                if(!push)
                    pull =player_do_pull(RIGHT);
                player_entity.x++;
            }
            else if(player_collides_event(RIGHT, player_entity.x, player_entity.y))
            {
                moved = false;
            }
            else if(!player_collides(UP_RIGHT, player_entity.x,player_entity.y) && player_collides(DOWN_RIGHT, player_entity.x,player_entity.y))
            {
                player_entity.y--;
                player_entity.x++;
                dir = UP_RIGHT;
            }
            else if(!player_collides(DOWN_RIGHT, player_entity.x,player_entity.y) && player_collides(UP_RIGHT, player_entity.x,player_entity.y))
            {
                player_entity.y++;
                player_entity.x++;
                dir = DOWN_RIGHT;
            }
            else
                moved = false;
            break;
        case UP:
            if(!player_collides(UP, player_entity.x,player_entity.y))
            {
                if(!push)
                    pull =player_do_pull(UP);
                player_entity.y--;
            }
            else if(player_collides_event(UP, player_entity.x, player_entity.y))
            {
                moved = false;
            }
            else if(!player_collides(UP_LEFT, player_entity.x,player_entity.y) && player_collides(UP_RIGHT, player_entity.x,player_entity.y))
            {
                player_entity.y--;
                player_entity.x--;
                dir = UP_LEFT;
            }
            else if(!player_collides(UP_RIGHT, player_entity.x,player_entity.y) && player_collides(UP_LEFT, player_entity.x,player_entity.y))
            {
                player_entity.y--;
                player_entity.x++;
                dir = UP_RIGHT;
            }
            else
                moved = false;
            break;
        case DOWN:
            if(!player_collides(DOWN, player_entity.x,player_entity.y))
            {
                if(!push)
                    pull =player_do_pull(DOWN);
                player_entity.y++;
            }
            else if(player_collides_event(DOWN, player_entity.x, player_entity.y))
            {
                moved = false;
            }
            else if(!player_collides(DOWN_LEFT, player_entity.x,player_entity.y) && player_collides(DOWN_RIGHT, player_entity.x,player_entity.y))
            {
                player_entity.y++;
                player_entity.x--;
                dir = DOWN_LEFT;
            }
            else if(!player_collides(DOWN_RIGHT, player_entity.x,player_entity.y) && player_collides(DOWN_LEFT, player_entity.x,player_entity.y))
            {
                player_entity.y++;
                player_entity.x++;
                dir = DOWN_RIGHT;
            }
            else
                moved = false;
            break;
        case UP_LEFT:
            if(!player_collides(UP_LEFT, player_entity.x,player_entity.y))
            {
                player_entity.y--;
                player_entity.x--;
            }
            else if(!player_collides(UP, player_entity.x,player_entity.y))
            {
                player_entity.y--;
            }
            else if(!player_collides(LEFT, player_entity.x,player_entity.y))
            {
                player_entity.x--;
            }
            else
                moved = false;
            break;
        case UP_RIGHT:
            if(!player_collides(UP_RIGHT, player_entity.x,player_entity.y))
            {
                player_entity.y--;
                player_entity.x++;
            }
            else if(!player_collides(UP, player_entity.x,player_entity.y))
            {
                player_entity.y--;
            }
            else if(!player_collides(RIGHT, player_entity.x,player_entity.y))
            {
                player_entity.x++;
            }
            else
                moved = false;
            break;
        case DOWN_LEFT:
            if(!player_collides(DOWN_LEFT, player_entity.x,player_entity.y))
            {
                player_entity.y++;
                player_entity.x--;
            }
            else if(!player_collides(DOWN, player_entity.x,player_entity.y))
            {
                player_entity.y++;
            }
            else if(!player_collides(LEFT, player_entity.x,player_entity.y))
            {
                player_entity.x--;
            }
            else
                moved = false;
            break;
        case DOWN_RIGHT:
            if(!player_collides(DOWN_RIGHT, player_entity.x,player_entity.y))
            {
                player_entity.y++;
                player_entity.x++;
            }
            else if(!player_collides(DOWN, player_entity.x,player_entity.y))
            {
                player_entity.y++;
            }
            else if(!player_collides(RIGHT, player_entity.x,player_entity.y))
            {
                player_entity.x++;
            }
            else
                moved = false;
            break;
    }

    if(push || pull)
        sound_play(1);

    last_dir = dir;
    if(!moved)
    {
        player_bump(dir, player_entity.x, player_entity.y);
    }
    else
    {
        player_stand(player_entity.x, player_entity.y);
    }
}

void player_handle_walk_animation()
{
    player_entity.char_id = 0;

    if(moving)
        anim_count = (anim_count+1)%4;
    else
        anim_count = 1; //Resting frame

    switch(last_dir)
    {
        case LEFT:
            player_entity.current_frame = CHAR_WALK_LEFT_ANIM[anim_count];
            break;
        case RIGHT:
            player_entity.current_frame = CHAR_WALK_RIGHT_ANIM[anim_count];
            break;
        case UP:
            player_entity.current_frame = CHAR_WALK_UP_ANIM[anim_count];
            break;
        case DOWN:
            player_entity.current_frame = CHAR_WALK_DOWN_ANIM[anim_count];
            break;
        case UP_LEFT:
            player_entity.current_frame = CHAR_WALK_UP_LEFT_ANIM[anim_count];
            break;
        case UP_RIGHT:
            player_entity.current_frame = CHAR_WALK_UP_RIGHT_ANIM[anim_count];
            break;
        case DOWN_LEFT:
            player_entity.current_frame = CHAR_WALK_DOWN_LEFT_ANIM[anim_count];
            break;
        case DOWN_RIGHT:
            player_entity.current_frame = CHAR_WALK_DOWN_RIGHT_ANIM[anim_count];
            break;
        default:
            break;
    }
    moving = 0;
}

void player_handle_attack_animation()
{
    player_entity.extend_dir = last_dir;
    bool is_gun = false;

    if(tile_metadata[player_inventory[PLAYER_EQUIPPED_ITEM]] & TILE_LIGHTSABER)
        player_entity.extend_offset = 1;
    else if(tile_metadata[player_inventory[PLAYER_EQUIPPED_ITEM]] & TILE_LIGHT_BLASTER || tile_metadata[player_inventory[PLAYER_EQUIPPED_ITEM]] & TILE_HEAVY_BLASTER || tile_metadata[player_inventory[PLAYER_EQUIPPED_ITEM]] & TILE_THE_FORCE)
    {
        player_entity.extend_offset++;
        is_gun = true;
    }

    switch(last_dir)
    {
        case LEFT:
            player_entity.current_frame = CHAR_ATTACK_LEFT_ANIM[anim_count];
            player_entity.extend_frame = is_gun ? FRAME_WEAPON_PROJECTILE_LEFT : CHAR_ATTACK_EXTEND_LEFT_ANIM[anim_count];
            break;
        case RIGHT:
            player_entity.current_frame = CHAR_ATTACK_RIGHT_ANIM[anim_count];
            player_entity.extend_frame = is_gun ? FRAME_WEAPON_PROJECTILE_RIGHT : CHAR_ATTACK_EXTEND_RIGHT_ANIM[anim_count];
            break;
        case UP:
        case UP_LEFT:
        case UP_RIGHT:
            player_entity.current_frame = CHAR_ATTACK_UP_ANIM[anim_count];
            player_entity.extend_frame = is_gun ? FRAME_WEAPON_PROJECTILE_UP : CHAR_ATTACK_EXTEND_UP_ANIM[anim_count];
            break;
        case DOWN:
        case DOWN_LEFT:
        case DOWN_RIGHT:
            player_entity.current_frame = CHAR_ATTACK_DOWN_ANIM[anim_count];
            player_entity.extend_frame = is_gun ? FRAME_WEAPON_PROJECTILE_DOWN : CHAR_ATTACK_EXTEND_DOWN_ANIM[anim_count];
            break;
        default:
            break;
    }

    anim_count++;
    if(anim_count >= 4)
        player_entity.attacking = false;
}

void player_init()
{
    player_entity.health = 300;
    player_inventory = calloc(256, sizeof(u16));
    player_inventory_count = 0;

    if(is_yoda)
        player_entity.is_active_visible = false;
}

void player_equip_item(u16 inv_slot)
{
    PLAYER_EQUIPPED_ITEM = inv_slot;

    //TODO: Do these transfer to Indy?
    if(tile_metadata[player_inventory[PLAYER_EQUIPPED_ITEM]] & TILE_LIGHTSABER)
        sound_play(0x1F);
    else if(tile_metadata[player_inventory[PLAYER_EQUIPPED_ITEM]] & TILE_LIGHT_BLASTER || tile_metadata[player_inventory[PLAYER_EQUIPPED_ITEM]] & TILE_HEAVY_BLASTER)
        sound_play(0x20);
    else if(tile_metadata[player_inventory[PLAYER_EQUIPPED_ITEM]] & TILE_THE_FORCE)
        sound_play(0x34);
}

void player_add_item_to_inv(u16 item)
{
    player_inventory[player_inventory_count++] = item;
}

void player_remove_item_from_inv(u16 item)
{
    for(int i = 0; i < player_inventory_count; i++)
    {
        if(player_inventory[i] == item)
        {
            player_inventory[i] = NULL;
            for(int j = i; j < player_inventory_count; j++)
            {
                player_inventory[j] = player_inventory[j+1];
            }
            player_inventory_count--;
            break;
        }
    }
}

bool player_has_item(u16 item)
{
    for(int i = 0; i < player_inventory_count; i++)
    {
        if(player_inventory[i] == item) return true;
    }
    return false;
}

void player_update()
{
    if(player_entity.attacking)
        player_handle_attack_animation();
    else
        player_handle_walk_animation();

    iact_set_trigger(IACT_TRIG_PlayerAtPos, 2, player_entity.x, player_entity.y);

    if(BUTTON_FIRE_STATE && PLAYER_EQUIPPED_ITEM != 0xFFFF && !player_entity.attacking)
    {
        for(int i = 0; i < 0x100; i++)
        {
            if(char_data[i]->frames[FRAME_WEAPON_ICON] == player_inventory[PLAYER_EQUIPPED_ITEM])
            {
                bool continue_attack = true;
                switch(last_dir)
                {
                    case LEFT:
                        if(player_entity.x == 0)
                        {
                            continue_attack = false;
                        }
                        break;
                    case RIGHT:
                        if(player_entity.x == map_get_width()-1)
                        {
                            continue_attack = false;
                        }
                        break;
                    case UP:
                    case UP_LEFT:
                    case UP_RIGHT:
                        if(player_entity.y == 0)
                        {
                            continue_attack = false;
                        }
                        break;
                    case DOWN:
                    case DOWN_LEFT:
                    case DOWN_RIGHT:
                        if(player_entity.y == map_get_height()-1)
                        {
                            continue_attack = false;
                        }
                        break;
                    default:
                        break;
                }

                if(!continue_attack) break;

                anim_count = 0;
                player_entity.attacking = true;
                player_entity.char_id = i;
                player_entity.extend_offset = 0;

                player_handle_attack_animation();
                sound_play(chwp_data[i]->id_2);
                break;
            }
        }
    }

    if(player_position_updated)
    {
        player_stand(player_entity.x, player_entity.y);
        player_position_updated = false;
    }
}
