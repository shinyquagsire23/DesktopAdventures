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

#ifndef DESKADV_UI_H
#define DESKADV_UI_H

#include "useful.h"

typedef struct ui_render_target
{
    int x;
    int y;
    int w;
    int h;
    
    // Caching
    u64 c;
    int cx1,cy1,cx2,cy2;
    struct ui_render_target* parent;
} ui_render_target;


void ui_get_target_bounds(ui_render_target* target, int* x1, int* y1, int* x2, int* y2);
void ui_render_target_clear(ui_render_target* target, u8 r, u8 g, u8 b, u8 a);

void ui_init(int x, int y, int w, int h, bool windowOnly);
void ui_update();
void ui_render();

void ui_set_mouse_abs(int x, int y);
void ui_add_mouse_abs(int x, int y);
void ui_mouse_left_down();
void ui_set_mouse_left(bool val);
void ui_touch_down();
void ui_touch_up();
void ui_set_draw_scale(int scale);

#endif // DESKADV_UI_H
