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

#include "main.h"

#ifdef PC_BUILD

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <tname.h>

#include "player.h"
#include "palette.h"
#include "useful.h"
#include "assets.h"
#include "screen.h"
#include "sound.h"
#include "input.h"
#include "font.h"
#include "map.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define TRUE  1
#define FALSE 0

int initGL();
int resizeWindow(int width, int height);
void null_loop_iter();
void loop_iter();

void render(int x, int y);
void render_pre();
void render_post();
void render_flip_buffers();

SDL_Window* displayWindow;
SDL_Renderer* displayRenderer;
SDL_RendererInfo displayRendererInfo;
SDL_Event event;
clock_t last_time;

u16 current_map = 79;
int done = FALSE;
int sdl_mouse_x = 0;
int sdl_mouse_y = 0;
int sdl_left_state = 0;
int sdl_right_state = 0;

int SDL_WIDTH, SDL_HEIGHT;
bool win95_sim = true;

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    SDL_WIDTH = (SCREEN_WIDTH+236+1)-2;
    SDL_HEIGHT = (SCREEN_HEIGHT+16+1)-6;

    if (win95_sim)
    {
        SDL_WIDTH = 1280;
        SDL_HEIGHT = 720;
    }

    SDL_CreateWindowAndRenderer(SDL_WIDTH, SDL_HEIGHT, 0, &displayWindow, &displayRenderer);
    SDL_GetRendererInfo(displayRenderer, &displayRendererInfo);
    SDL_SetRenderDrawBlendMode(displayRenderer, SDL_BLENDMODE_BLEND);

    ui_init(0, 0, SDL_WIDTH, SDL_HEIGHT, !win95_sim);
    ui_set_draw_scale(1);

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(null_loop_iter, 60, 1);
#endif

    srand(time(NULL));
    sound_init();
    resizeWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!load_resources())
    {
        sound_exit();
        Quit(0);
        return -1;
    }

    last_time = clock();

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(loop_iter, 60, 1);
#else
    while (!done)
    {
        loop_iter();
    }
#endif

    sound_exit();
    Quit(0);
    return (0);
}

void null_loop_iter()
{
    render_pre();
    render(0,0);
    render_post();
    render_flip_buffers();
}

void loop_iter()
{
    clock_t time = clock();
    double delta = (double)(time - last_time)/(CLOCKS_PER_SEC/1000.0);
    last_time = time;

    reset_input_state();
    update_input();
    ui_update();

    update_world(delta);

#ifdef __EMSCRIPTEN__
    if (done) {
 	        emscripten_cancel_main_loop();
 	    }
#endif
}

int resizeWindow(int width, int height)
{
    if (height == 0)
        height = 1;

    return ( TRUE);
}

/* function to handle key press events */
void handleKeyPress(SDL_Keysym *keysym)
{
    if(CURRENT_ITEM_DRAGGED != -1) return;

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
    if(CURRENT_ITEM_DRAGGED != -1) return;

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

    if (modstate & (KMOD_LSHIFT | KMOD_RSHIFT))
    {
        button_push();
    }

    if (keystate[SDL_SCANCODE_SPACE])
    {
        button_fire();
    }
}

void handleTouchEvent(SDL_TouchFingerEvent* event)
{
    if (event->type == SDL_FINGERUP)
        ui_touch_up();
    else if (event->type == SDL_FINGERDOWN)
        ui_touch_down();
    else
    {
        ui_set_mouse_abs((int)(event->x*SDL_WIDTH), (int)(event->y*SDL_HEIGHT));
    }
}

void handleMouseMove(SDL_MouseMotionEvent *event)
{
    int x = event->x;
    int y = event->y;

    sdl_mouse_x = x;
    sdl_mouse_y = y;
    ui_set_mouse_abs(x, y);
}

void handleMousePress()
{
    int left_state = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT);
    int right_state = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT);

    if(!sdl_left_state && left_state)
    {
        ui_mouse_left_down();
    }

    sdl_left_state = left_state;
    sdl_right_state = right_state;
    
    ui_set_mouse_left(left_state);
}

void fillScreen(char r, char g, char b, char a)
{
    SDL_SetRenderDrawColor(displayRenderer, r, g, b, a);
    SDL_Rect rectGame = {0, 0, SDL_WIDTH, SDL_HEIGHT};
    SDL_RenderFillRect(displayRenderer, &rectGame);
    
}

void drawPixel(int x, int y, char r, char g, char b, char a)
{
    SDL_SetRenderDrawColor(displayRenderer, r, g, b, a);
    SDL_RenderDrawPoint(displayRenderer, x, y);
}

void drawFillRect(int x1, int y1, int x2, int y2, char r, char g, char b, char a)
{
    SDL_SetRenderDrawColor(displayRenderer, r, g, b, a);
    SDL_Rect rectGame = {x1, y1, x2-x1+1, y2-y1+1};
    SDL_RenderFillRect(displayRenderer, &rectGame);
}

void render_flip_buffers()
{
    SDL_UpdateWindowSurface(displayWindow);
    SDL_RenderPresent(displayRenderer);
}

void Quit(int returnCode)
{
    SDL_Quit();
    exit(returnCode);
}

void update_input()
{
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_KEYDOWN:
                handleKeyPress(&event.key.keysym);
                break;
            case SDL_MOUSEMOTION:
                handleMouseMove(&event.motion);
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                handleMousePress();
                break;
            case SDL_FINGERMOTION:
            case SDL_FINGERDOWN:
            case SDL_FINGERUP:
                handleTouchEvent(&event);
                break;
            case SDL_QUIT:
                done = TRUE;
                break;
            default:
                break;
        }
    }
    handleKeyDown();

    if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))
        mouse_left();

    if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT))
        mouse_right();
}

void render_pre()
{
}

void render_post()
{
    if(CURRENT_ITEM_DRAGGED != -1)
        SDL_ShowCursor(SDL_DISABLE);
    else
        SDL_ShowCursor(SDL_ENABLE);

    ui_render();
}
#endif
