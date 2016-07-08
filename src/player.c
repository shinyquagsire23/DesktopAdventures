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

#include "player.h"

#include <stdio.h>
#include <stddef.h>
#include "map.h"
#include "iact.h"
#include "tile.h"
#include "objectinfo.h"

int last_dir = -1;
int anim_count = 0;
bool moving = false;

u8 PLAYER_MAP_CHANGE_REASON = 0;
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

    iact_set_trigger(IACT_TRIG_BumpTile, 3, bump_x, bump_y, map_get_tile(LAYER_MIDDLE, bump_x, bump_y));
}

void player_stand(int x, int y)
{
    int object_index = 0;
    while(1)
    {
        obj_info *object = map_get_object(object_index++, x, y);
        if(object == NULL)
            break;

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

            unload_map();
            load_map(DOOR_IN_map);
        }
    }

    iact_set_trigger(IACT_TRIG_Walk, 2, player_entity.x, player_entity.y);
}

void player_move(int dir)
{
    if(dir != last_dir)
        anim_count = 0;

    moving = true;

    bool moved = true;

    switch(dir) {
        case LEFT:
            if(!player_collides(LEFT, player_entity.x,player_entity.y))
                player_entity.x--;
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
                player_entity.x++;
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
                player_entity.y--;
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
                player_entity.y++;
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
            else
                moved = false;
            break;
        case UP_RIGHT:
            if(!player_collides(UP_RIGHT, player_entity.x,player_entity.y))
            {
                player_entity.y--;
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
            else
                moved = false;
            break;
        case DOWN_RIGHT:
            if(!player_collides(DOWN_RIGHT, player_entity.x,player_entity.y))
            {
                player_entity.y++;
                player_entity.x++;
            }
            else
                moved = false;
            break;
    }

    if(!moved)
    {
        player_bump(dir, player_entity.x, player_entity.y);
    }
    else
    {
        player_stand(player_entity.x, player_entity.y);
    }
    last_dir = dir;
}

void player_handle_walk_animation()
{
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

void player_init()
{
    player_entity.health = 300;
}

void player_update()
{
    player_handle_walk_animation();
    iact_set_trigger(IACT_TRIG_PlayerAtPos, 2, player_entity.x, player_entity.y);
}
