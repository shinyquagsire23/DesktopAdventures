/****************************************
 *       NDS NeHe Lesson 04            *
 *       Author: Machinamentum         *
 ****************************************/

#ifdef _3DS

#include <3ds.h>
#include <GL/gl.h>
#include <gfx_device.h>

#include "../useful.h"
#include "../assets.h"
#include "../screen.h"
#include "../input.h"
#include "../map.h"

u16 current_map = 0;

int main()
{
   gfxInitDefault();
   consoleInit(GFX_BOTTOM, NULL);
   hidInit(NULL);

   void* device = gfxCreateDevice(240, 400);
   gfxMakeCurrent(device);

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

   while (aptMainLoop())
   {
      hidScanInput();

      if (keysDown() & KEY_START)
         break;

      if(keysDown() & KEY_X)
      {
    	  if(current_map < NUM_MAPS)
    	  {
    	  	current_map++;
    	  	load_map(current_map);
    	  }
      }
      else if(keysDown() & KEY_Y)
      {
    	  if(current_map > 0)
    	  {
    	  	current_map--;
    	  	load_map(current_map);
    	  }
      }

      if(keysDown() & KEY_UP)
      {
    	  if(camera_y > 0)
    	  	camera_y--;
      }
      else if(keysDown() & KEY_DOWN)
      {
    	  if(camera_y < height - 9)
    	  	camera_y++;
      }
      else if(keysDown() & KEY_LEFT)
      {
    	  if(camera_x > 0)
    	  	camera_x--;
      }
      else if(keysDown() & KEY_RIGHT)
      {
    	  if(camera_x < width - 9)
    	  	camera_x++;
      }

	  render_map();
	  draw_screen();

      gfxFlush(gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL));
      gfxFlushBuffers();
      gfxSwapBuffersGpu();
      gspWaitForVBlank();
   }

   // Exit services
   gfxExit();
   hidExit();
   return 0;
}

#endif
