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

#ifndef INPUT_H
#define INPUT_H

#include "useful.h"

void button_move_down();
void button_move_up();
void button_move_left();
void button_move_right();
void button_push();
void button_fire();
void drop_item(int x, int y);
void mouse_move(int x, int y);
void mouse_left();
void mouse_right();
void item_dragging(u16 item);
void reset_input_state();
void update_input();

int CURRENT_ITEM_DRAGGED;

u8 BUTTON_DOWN_STATE;
u8 BUTTON_UP_STATE;
u8 BUTTON_LEFT_STATE;
u8 BUTTON_RIGHT_STATE;
u8 BUTTON_PUSH_STATE;
u8 BUTTON_FIRE_STATE;
u8 BUTTON_LCLICK_STATE;
u8 BUTTON_RCLICK_STATE;

bool MOUSE_MOVED;
int MOUSE_X;
int MOUSE_Y;

#endif
