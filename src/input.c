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

int CURRENT_ITEM_DRAGGED = 0;

u8 BUTTON_DOWN_STATE;
u8 BUTTON_UP_STATE;
u8 BUTTON_LEFT_STATE;
u8 BUTTON_RIGHT_STATE;
u8 BUTTON_PUSH_STATE;
u8 BUTTON_FIRE_STATE;
u8 BUTTON_LCLICK_STATE;
u8 BUTTON_RCLICK_STATE;

int MOUSE_X = -1;
int MOUSE_Y = -1;

void button_move_down()
{
    BUTTON_DOWN_STATE = 1;
}

void button_move_up()
{
    BUTTON_UP_STATE = 1;
}

void button_move_left()
{
    BUTTON_LEFT_STATE = 1;
}

void button_move_right()
{
    BUTTON_RIGHT_STATE = 1;
}

void button_push()
{
    BUTTON_PUSH_STATE = 1;
}

void button_fire()
{
    BUTTON_FIRE_STATE = 1;
}

void mouse_move(int x, int y)
{
    MOUSE_X = x;
    MOUSE_Y = y;
}

void mouse_left()
{
    BUTTON_LCLICK_STATE = 1;
    if(CURRENT_ITEM_DRAGGED >= 0)
    {
        CURRENT_ITEM_DRAGGED = -1;
        //TODO
    }
}

void mouse_right()
{
    BUTTON_RCLICK_STATE = 1;
    //TODO: Use FORCE, not sure if Indy binds anything to RClick
}

void item_dragging(u16 item)
{
    CURRENT_ITEM_DRAGGED = item;
}

void reset_input_state()
{
    BUTTON_DOWN_STATE = 0;
    BUTTON_UP_STATE = 0;
    BUTTON_LEFT_STATE = 0;
    BUTTON_RIGHT_STATE = 0;
    BUTTON_PUSH_STATE = 0;
    BUTTON_FIRE_STATE = 0;
    BUTTON_LCLICK_STATE = 0;
    BUTTON_RCLICK_STATE = 0;
}
