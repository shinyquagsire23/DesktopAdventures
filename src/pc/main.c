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
#include <SDL_opengl.h>

#include "useful.h"
#include "assets.h"
#include "screen.h"
#include "sound.h"
#include "input.h"
#include "map.h"

#define TRUE  1
#define FALSE 0

int initGL();
int resizeWindow(int width, int height);

SDL_Window* displayWindow;
SDL_Renderer* displayRenderer;
SDL_RendererInfo displayRendererInfo;

// Our opengl context handle
SDL_GLContext mainContext;

u16 current_map = 0;

int main(int argc, char **argv)
{
    int done = FALSE;
    SDL_Event event;
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_WIDTH, SDL_WINDOW_OPENGL, &displayWindow, &displayRenderer);
    SDL_GetRendererInfo(displayRenderer, &displayRendererInfo);
    SDL_SetRenderDrawBlendMode(displayRenderer, SDL_BLENDMODE_BLEND);

    if ((displayRendererInfo.flags & SDL_RENDERER_ACCELERATED) == 0 ||
        (displayRendererInfo.flags & SDL_RENDERER_TARGETTEXTURE) == 0)
    {
        fprintf( stderr, "Video init failed: %s\n", SDL_GetError());
        Quit(1);
    }

    srand(time(NULL));
    initGL();
    sound_init();
    resizeWindow(SCREEN_WIDTH, SCREEN_WIDTH);
    load_resources();

    clock_t last_time = clock();
    while (!done)
    {
        clock_t time = clock();
        double delta = (double)(time - last_time)/(CLOCKS_PER_SEC/1000.0);
        last_time = time;

        reset_input_state();
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

        update_world(delta);

        SDL_GL_SwapWindow(displayWindow);
    }

    sound_exit();
    Quit(0);
    return (0);
}

int initGL()
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    mainContext = SDL_GL_CreateContext(displayWindow);

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
    glOrtho(0, 288, 288, 0, -1, 1);
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
        default:
        break;
    }
    return;
}

void handleKeyDown()
{
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    const Uint32 modstate = SDL_GetModState();

    //continuous-response keys
    if (keystate[SDL_SCANCODE_UP])
    {
        button_move_up();
    }
    else if (keystate[SDL_SCANCODE_RIGHT])
    {
        button_move_right();
    }
    else if (keystate[SDL_SCANCODE_LEFT])
    {
        button_move_left();
    }
    else if (keystate[SDL_SCANCODE_DOWN])
    {
        button_move_down();
    }
    else if (modstate & (KMOD_LSHIFT | KMOD_RSHIFT))
    {
        button_push();
    }
}

void buffer_clear_screen(u8 r, u8 g, u8 b, u8 a)
{
    SDL_SetRenderDrawColor(displayRenderer, r, g, b, a);
    SDL_RenderClear(displayRenderer);
}

void buffer_plot_pixel(int x, int y, u8 r, u8 g, u8 b, u8 a)
{
    SDL_SetRenderDrawColor(displayRenderer, r, g, b, a);
    SDL_RenderDrawPoint(displayRenderer, x, y);
}

void Quit(int returnCode)
{
    SDL_Quit();
    exit(returnCode);
}

void redraw_swap_buffers()
{
    draw_screen();
    SDL_GL_SwapWindow(displayWindow);
}

#endif
