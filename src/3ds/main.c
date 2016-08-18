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

#ifdef _3DS

#include <3ds.h>
#include <GL/gl.h>
#include <gfx_device.h>
#include <time.h>

#include "useful.h"
#include "assets.h"
#include "screen.h"
#include "input.h"
#include "map.h"

u16 current_map = 0;

int main()
{
   gfxInitDefault();
   consoleInit(GFX_BOTTOM, NULL);
   hidInit();

   void* device_top = gfxCreateDevice(480, 400, 0);
   gfxMakeCurrent(device_top);

   glEnable(GL_TEXTURE_2D);
   glEnable (GL_BLEND);
   glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   //glShadeModel( GL_SMOOTH);
   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
   
   glViewport(0,0,240,400);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
   glOrtho(0,400,240,0,-1,1);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glBindTexture(GL_TEXTURE_2D, 0);
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   load_resources();

   clock_t last_time = clock();
   while (aptMainLoop())
   {
      clock_t time = clock();
      double delta = (double)(time - last_time)/(CLOCKS_PER_SEC/1000.0);
      last_time = time;

      reset_input_state();
      update_input();

      update_world(delta);
   }

   // Exit services
   gfxExit();
   hidExit();
   return 0;
}

void render_flip_buffers()
{
    gfxFlush(gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL), 240, 400, GX_TRANSFER_FMT_RGB8);

    gfxSwapBuffersGpu();
    gspWaitForVBlank();
}

void update_input()
{
    hidScanInput();

    if (keysDown() & KEY_START)
        break;

    if(keysDown() & KEY_X)
    {
        if(current_map < NUM_MAPS)
        {
            unload_map();
            current_map++;
            load_map(current_map);
        }
    }
    else if(keysDown() & KEY_Y)
    {
        if(current_map > 0)
        {
            unload_map();
            current_map--;
            load_map(current_map);
        }
    }

    if(keysDown() & KEY_UP)
        button_move_up();
    else if(keysDown() & KEY_RIGHT)
        button_move_right();
    else if(keysDown() & KEY_LEFT)
        button_move_left();
    else if(keysDown() & KEY_DOWN)
        button_move_down();

    if(keysDown() & KEY_A)
        button_fire();
}

#endif
