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

#include "screen.h"

#include <unistd.h>

#include "main.h"
#include "useful.h"
#include "player.h"
#include "map.h"

void render(int x, int y);
void render_pre();
void render_post();
void render_flip_buffers();

unsigned short tiles_low[0x100 * 0x100];
unsigned short tiles_middle[0x100 * 0x100];
unsigned short tiles_high[0x100 * 0x100];
unsigned short tiles_overlay[0x100 * 0x100];

char *active_text;
int active_text_x;
int active_text_y;

u32 SCREEN_WIDTH = 288;
u32 SCREEN_HEIGHT = 288;

int SCREEN_SHIFT_X = 0;
int SCREEN_SHIFT_Y = 0;
u8 SCREEN_FADE_LEVEL = 0;

//Default game uses 10fps for "Normal" speed, +- 5fps for Slow/Fast
u16 TARGET_TICK_FPS = 10;

void init_screen()
{
    for(int i = 0; i < 0x100 * 0x100; i++)
    {
        tiles_low[i] = 0xFFFF;
        tiles_middle[i] = 0xFFFF;
        tiles_high[i] = 0xFFFF;
    }
}

int draw_screen()
{
    SCREEN_FADE_LEVEL = MIN(SCREEN_FADE_LEVEL, (SCREEN_TILE_WIDTH/2)+1);

    render_pre();
    render(SCREEN_SHIFT_X, SCREEN_SHIFT_Y);
    render_post();
    render_flip_buffers();
    return 1;
}

void screen_transition_out()
{
    for(int i = (SCREEN_TILE_WIDTH > map_get_width() ? ((SCREEN_TILE_WIDTH-map_get_width())/2)+1 : 0); i <= (SCREEN_TILE_WIDTH/2)+1; i++)
    {
        player_update();

        SCREEN_FADE_LEVEL = i;
        draw_screen();
        usleep(1000*(1000/TARGET_TICK_FPS));
    }
}

void screen_transition_in()
{
    for(int i = (SCREEN_TILE_WIDTH/2)+1; i > (SCREEN_TILE_WIDTH > map_get_width() ? ((SCREEN_TILE_WIDTH-map_get_width())/2)+1 : 0); i--)
    {
        player_update();

        SCREEN_FADE_LEVEL = i;
        draw_screen();
        usleep(1000*(1000/TARGET_TICK_FPS));
    }
    SCREEN_FADE_LEVEL = 0;
    draw_screen();
}
