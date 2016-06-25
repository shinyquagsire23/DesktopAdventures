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

#include "main.h"

#ifdef PC_BUILD

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "useful.h"
#include "assets.h"
#include "screen.h"
#include "input.h"
#include "map.h"

#define TRUE  1
#define FALSE 0

int initGL();
int resizeWindow(int width, int height);

SDL_Window* displayWindow;
SDL_Renderer* displayRenderer;
SDL_RendererInfo displayRendererInfo;

u16 current_map = 0;

int main(int argc, char **argv)
{
    int done = FALSE;
    SDL_Event event;
    SDL_Init(SDL_INIT_VIDEO);

    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_WIDTH, SDL_WINDOW_OPENGL, &displayWindow, &displayRenderer);
    SDL_GetRendererInfo(displayRenderer, &displayRendererInfo);

    if ((displayRendererInfo.flags & SDL_RENDERER_ACCELERATED) == 0 ||
        (displayRendererInfo.flags & SDL_RENDERER_TARGETTEXTURE) == 0)
    {
        fprintf( stderr, "Video init failed: %s\n", SDL_GetError());
        Quit(1);
    }

    initGL();
    resizeWindow(SCREEN_WIDTH, SCREEN_WIDTH);
    load_resources();

    clock_t last_time = clock();
    double test = 0.0;
    while (!done)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_KEYDOWN:
                    handleKeyPress(&event.key.keysym);
                break;
                case SDL_QUIT:
                    done = TRUE;
                break;
                default:
                break;
            }
        }

        handleKeyDown();
        render_map();
        draw_screen();
        SDL_GL_SwapWindow(displayWindow);
    }

    Quit(0);
    return (0);
}

int initGL()
{
    glEnable( GL_TEXTURE_2D);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glEnable (GL_BLEND);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    return 1;
}

int resizeWindow(int width, int height)
{
    if (height == 0)
        height = 1;

    glViewport(0, 0, (GLsizei) width, (GLsizei) height);
    glMatrixMode( GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 288, 288, 0);
    glMatrixMode( GL_MODELVIEW);
    glLoadIdentity();

    return ( TRUE);
}

/* function to handle key press events */
void handleKeyPress(SDL_Keysym *keysym)
{
    switch (keysym->sym)
    {
        case SDLK_ESCAPE:
            /* ESC key was pressed */
            Quit(0);
        break;
        case SDLK_F1:
            /* F1 key was pressed
             * this toggles fullscreen mode
             */
            //SDL_WM_ToggleFullScreen(surface);
        break;
        case SDLK_p:
            if(current_map < NUM_MAPS)
            {
                unload_map();
                current_map++;
                load_map(current_map);
            }
        break;
        case SDLK_o:
            if(current_map > 0)
            {
                unload_map();
                current_map--;
                load_map(current_map);
            }
        break;
        case SDLK_UP:
            if(camera_y > 0)
                camera_y--;
        break;
        case SDLK_DOWN:
            if(camera_y < height - 9)
                camera_y++;
        break;
        case SDLK_LEFT:
            if(camera_x > 0)
                camera_x--;
        break;
        case SDLK_RIGHT:
            if(camera_x < width - 9)
                camera_x++;
        break;
        default:
        break;
    }
    return;
}

void handleKeyDown()
{
    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    //continuous-response keys
    if (keystate[SDL_SCANCODE_LEFT])
    {

    }
    if (keystate[SDL_SCANCODE_RIGHT])
    {

    }
    if (keystate[SDL_SCANCODE_UP])
    {

    }
    if (keystate[SDL_SCANCODE_DOWN])
    {

    }
}

void Quit(int returnCode)
{
    SDL_Quit();
    exit(returnCode);
}

void draw_STUP()
{
    draw_screen();
    SDL_GL_SwapWindow(displayWindow);
}

#endif
