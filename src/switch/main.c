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

#include <switch.h>
#include <time.h>

#include "draw.h"
#include "useful.h"
#include "assets.h"
#include "screen.h"
#include "input.h"
#include "map.h"

bool initialized = false;
bool isAppRunning = true;

u16 current_map = 0;
bool exiting = false;

int
main(int argc, char **argv)
{
    gfxInitDefault();
    load_resources();

    while(appletMainLoop())
    {
        double delta = 32.0;

        reset_input_state();
        update_input();
        
        if (exiting) break;

        update_world(delta);
        gfxWaitForVsync();
    }
    return 0;
}

void update_input()
{
    hidScanInput();

    if(hidKeysDown(CONTROLLER_P1_AUTO) & KEY_X)
    {
        if(current_map < NUM_MAPS)
        {
            unload_map();
            current_map++;
            load_map(current_map);
        }
    }
    else if(hidKeysDown(CONTROLLER_P1_AUTO) & KEY_Y)
    {
        if(current_map > 0)
        {
            unload_map();
            current_map--;
            load_map(current_map);
        }
    }

    if(hidKeysDown(CONTROLLER_P1_AUTO) & KEY_A)
    {
        button_fire();
    }

    if(hidKeysHeld(CONTROLLER_P1_AUTO) & KEY_UP)
        button_move_up();
    else if(hidKeysHeld(CONTROLLER_P1_AUTO) & KEY_RIGHT)
        button_move_right();
    else if(hidKeysHeld(CONTROLLER_P1_AUTO) & KEY_LEFT)
        button_move_left();
    else if(hidKeysHeld(CONTROLLER_P1_AUTO) & KEY_DOWN)
        button_move_down();
    
    if(hidKeysHeld(CONTROLLER_P1_AUTO) & KEY_PLUS)
        exiting = true;
}

void render_flip_buffers()
{
    gfxFlushBuffers();
    gfxSwapBuffers();
}

void buffer_clear_screen(u8 r, u8 g, u8 b, u8 a)
{
    fillScreen(r,g,b,a);
}

void buffer_plot_pixel(int x, int y, u8 r, u8 g, u8 b, u8 a)
{
    int scale = 2;

    int x_shift = (getScreenWidth() - (288*scale)) / 2;
    int y_shift = (getScreenHeight() - (288*scale)) / 2;
    drawFillRect((x*scale)+x_shift,(y*scale)+y_shift,(x*scale)+x_shift+1,(y*scale)+y_shift+1,r,g,b,a);
}

void render_pre()
{

}

void render_post()
{

}
