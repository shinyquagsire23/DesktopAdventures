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

int last_dir = -1;
int anim_count = 0;
bool moving = false;

void player_move(int dir)
{
    if(dir != last_dir)
        anim_count = 0;

    moving = true;

    switch(dir) {
        case LEFT:
            player_entity.x--;
            break;
        case RIGHT:
            player_entity.x++;
            break;
        case UP:
            player_entity.y--;
            break;
        case DOWN:
            player_entity.y++;
            break;
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
        default:
            break;
    }
    moving = 0;
}

void player_update()
{
    player_handle_walk_animation();
}
