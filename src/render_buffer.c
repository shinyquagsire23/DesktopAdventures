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

#ifdef RENDER_BUFFER

#include "assets.h"
#include "screen.h"
#include "palette.h"

void buffer_clear_screen(u8 r, u8 g, u8 b, u8 a);
void buffer_plot_pixel(int x, int y, u8 r, u8 g, u8 b, u8 a);

void render_texture(int x, int y, int width, int height, u8 alpha, void *buffer)
{
    if(!buffer)
        return;

    for(int i = 0; i < width*height; i++)
    {
        u8 index = *(u8*)(buffer+i);
        u8 r,g,b;

        if(!index)
            continue;

        if(is_yoda)
        {
            b = (u8) (yodesk_palette[(index * 4)]);
            g = (u8) (yodesk_palette[(index * 4) + 1]);
            r = (u8) (yodesk_palette[(index * 4) + 2]);
        }
        else
        {
            b = (u8) (indy_palette[(index * 4)]);
            g = (u8) (indy_palette[(index * 4) + 1]);
            r = (u8) (indy_palette[(index * 4) + 2]);
        }

        buffer_plot_pixel(x+(i%width),y+(i/height),r,g,b,alpha);
    }
}

void render(int x, int y)
{
    buffer_clear_screen(0,0,0,255);

    if (ASSETS_LOADING)
    {
        render_texture(x,y,288,288,255,texture_buffers[0x2000]);

        int bar_x = 8;
        int bar_y = 264;
        for(int i = 0; i < (int)(278.0f*ASSETS_PERCENT); i++)
        {
            for(int j = 0; j < 16; j++)
            {
                buffer_plot_pixel(bar_x+i,bar_y+j,0,0,128,255);
            }
        }
    }
    else
    {
        for (int y = SCREEN_FADE_LEVEL; y < SCREEN_TILE_WIDTH-SCREEN_FADE_LEVEL; y++) {
            for (int x = SCREEN_FADE_LEVEL; x < SCREEN_TILE_WIDTH-SCREEN_FADE_LEVEL; x++) {
                if (tiles_low[(y * SCREEN_TILE_WIDTH) + x] != 0xFFFF) {
                    render_texture(32 * x, 32 * y, 32, 32, 255, texture_buffers[tiles_low[(y * 9) + x]]);
                }

                if (tiles_middle[(y * SCREEN_TILE_WIDTH) + x] != 0xFFFF) {
                    render_texture(32 * x, 32 * y, 32, 32, 255, texture_buffers[tiles_middle[(y * 9) + x]]);
                }

                if (tiles_high[(y * SCREEN_TILE_WIDTH) + x] != 0xFFFF) {
                    render_texture(32 * x, 32 * y, 32, 32, 255, texture_buffers[tiles_high[(y * 9) + x]]);
                }

                if (tiles_overlay[(y * SCREEN_TILE_WIDTH) + x] != 0xFFFF) {
                    render_texture(32 * x, 32 * y, 32, 32, 153, texture_buffers[tiles_overlay[(y * 9) + x]]);
                }
            }
        }
    }
}

#endif
