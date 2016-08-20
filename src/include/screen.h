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

unsigned short tiles_low[0x100 * 0x100];
unsigned short tiles_middle[0x100 * 0x100];
unsigned short tiles_high[0x100 * 0x100];
unsigned short tiles_overlay[0x100 * 0x100];

char *active_text;
int active_text_x;
int active_text_y;

void init_screen();
int draw_screen();
void screen_transition_in();
void screen_transition_out();

int SCREEN_SHIFT_X;
int SCREEN_SHIFT_Y;
u8 SCREEN_FADE_LEVEL;
u16 TARGET_TICK_FPS;
