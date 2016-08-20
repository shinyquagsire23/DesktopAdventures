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

#define TRUE  1
#define FALSE 0

int initGL();
int resizeWindow(int width, int height);

SDL_Window* displayWindow;
SDL_Renderer* displayRenderer;
SDL_RendererInfo displayRendererInfo;
SDL_Event event;

// Our opengl context handle
SDL_GLContext mainContext;

u16 current_map = 0;
int done = FALSE;

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    SDL_CreateWindowAndRenderer(SCREEN_WIDTH+232, SCREEN_HEIGHT+16, SDL_WINDOW_OPENGL, &displayWindow, &displayRenderer);
    SDL_GetRendererInfo(displayRenderer, &displayRendererInfo);
    SDL_SetRenderDrawBlendMode(displayRenderer, SDL_BLENDMODE_BLEND);

    if ((displayRendererInfo.flags & SDL_RENDERER_ACCELERATED) == 0 ||
        (displayRendererInfo.flags & SDL_RENDERER_TARGETTEXTURE) == 0)
    {
        fprintf( stderr, "Video init failed: %s\n", SDL_GetError());
        Quit(1);
    }

    SCREEN_SHIFT_X = 8;
    SCREEN_SHIFT_Y = 8;

    srand(time(NULL));
    initGL();
    sound_init();
    resizeWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
    load_resources();

    clock_t last_time = clock();
    while (!done)
    {
        clock_t time = clock();
        double delta = (double)(time - last_time)/(CLOCKS_PER_SEC/1000.0);
        last_time = time;

        reset_input_state();
        update_input();

        update_world(delta);
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

    if (modstate & (KMOD_LSHIFT | KMOD_RSHIFT))
    {
        button_push();
    }

    if (keystate[SDL_SCANCODE_SPACE])
    {
        button_fire();
    }
}

bool rendering_world = false;

void buffer_clear_screen(u8 r, u8 g, u8 b, u8 a)
{
    SDL_SetRenderDrawColor(displayRenderer, 200, 200, 200, 255);
    SDL_Rect rect = {0, 0, SCREEN_WIDTH+236, SCREEN_HEIGHT+16};
    SDL_RenderFillRect(displayRenderer, &rect);

    SDL_SetRenderDrawColor(displayRenderer, r, g, b, a);
    SDL_Rect rectGame = {8, 8, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(displayRenderer, &rectGame);
}

void buffer_plot_pixel(int x, int y, u8 r, u8 g, u8 b, u8 a)
{
    if(rendering_world)
    {
        if(x > SCREEN_WIDTH+SCREEN_SHIFT_X || x < SCREEN_SHIFT_X || y < SCREEN_SHIFT_Y || y > SCREEN_HEIGHT+SCREEN_SHIFT_Y)
            return;
    }
    SDL_SetRenderDrawColor(displayRenderer, r, g, b, a);
    SDL_RenderDrawPoint(displayRenderer, x, y);
}

void render_flip_buffers()
{
    SDL_GL_SwapWindow(displayWindow);
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
            case SDL_QUIT:
                done = TRUE;
                break;
            default:
                break;
        }
    }
    handleKeyDown();
}

void buffer_render_tile(int x, int y, u8 alpha, u32 tile)
{
    int width = 32, height = 32;
    void *buffer = texture_buffers[tile];

    if(!buffer || buffer == -1)
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

int buffer_render_char(int x, int y, char c)
{
    if(c == ' ')
        return deskAdvInvFontInfo.space_width+1;

    int offset = deskAdvInvFontDescriptors[c-deskAdvInvFontInfo.start_char].offset;
    for(int i = 0; i < deskAdvInvFontDescriptors[c-deskAdvInvFontInfo.start_char].width; i++)
    {
        for(int j = 0; j < deskAdvInvFontInfo.height; j++)
        {
            if(deskAdvInvFontBitmaps[offset+j] & (1<<(7-i)))
                buffer_plot_pixel(x+i,y+j,0,0,0,255);
        }
    }
    return deskAdvInvFontDescriptors[c-deskAdvInvFontInfo.start_char].width+1;
}

void buffer_render_text(int x, int y, char *text)
{
    for(int i = 0; i < strlen(text); i++)
    {
        x += buffer_render_char(x,y,text[i]);
    }
}

void buffer_render_outdent(int x, int y, int width, int height, u32 highlight, u32 shadow)
{
    SDL_SetRenderDrawColor(displayRenderer, (u8)((shadow & 0xFF0000) >> 16), (u8)((shadow & 0xFF00) >> 8), (u8)((shadow & 0xFF)), 255);
    SDL_RenderDrawLine(displayRenderer,x+width-1,y+height-1,x, y+height-1);
    SDL_RenderDrawLine(displayRenderer,x+width-1,y+height-1,x+width-1, y);

    SDL_SetRenderDrawColor(displayRenderer, (u8)((highlight & 0xFF0000) >> 16), (u8)((highlight & 0xFF00) >> 8), (u8)((highlight & 0xFF)), 255);
    SDL_RenderDrawLine(displayRenderer,x,y,x+width-1,y);
    SDL_RenderDrawLine(displayRenderer,x,y,x,y+height-2);
}

void render_pre()
{
    rendering_world = true;
}

int inventory_scroll = 0;

void render_post()
{
    rendering_world = false;

    //Game Surrounding box
    buffer_render_outdent(5, 5, SCREEN_WIDTH+6,SCREEN_HEIGHT+6, 0x808080, 0xFFFFFF);
    buffer_render_outdent(6, 6, SCREEN_WIDTH+4,SCREEN_HEIGHT+4, 0x808080, 0xFFFFFF);
    buffer_render_outdent(7, 7, SCREEN_WIDTH+2,SCREEN_HEIGHT+2, 0x808080, 0xFFFFFF);

    //Surrounding box
    buffer_render_outdent(SCREEN_WIDTH + 17, 6, 187,228, 0x808080, 0xFFFFFF);
    buffer_render_outdent(SCREEN_WIDTH + 18, 7, 185,226, 0x808080, 0xFFFFFF);

    //Force box
    buffer_render_outdent(SCREEN_WIDTH + 17 + 77, 236 + 18, 13,36, 0x808080, 0xFFFFFF);
    buffer_render_outdent(SCREEN_WIDTH + 17 + 78, 237 + 18, 11,34, 0x808080, 0xFFFFFF);
    buffer_render_outdent(SCREEN_WIDTH + 17 + 79, 237 + 19, 9,32, 0xFFFFFF, 0x808080);

    //Item box
    buffer_render_outdent(SCREEN_WIDTH + 16 + 17 + 77, 236 + 18, 36,36, 0x808080, 0xFFFFFF);
    buffer_render_outdent(SCREEN_WIDTH + 16 + 17 + 78, 237 + 18, 34,34, 0x808080, 0xFFFFFF);
    buffer_render_outdent(SCREEN_WIDTH + 16 + 17 + 79, 237 + 19, 32,32, 0xFFFFFF, 0x808080);

    //Scroll box
    buffer_render_outdent(SCREEN_WIDTH + 17 + 190, 6, 20,228, 0x808080, 0xFFFFFF);
    buffer_render_outdent(SCREEN_WIDTH + 18 + 190, 7, 18,226, 0x808080, 0xFFFFFF);

    for(int i = 0; i < 7; i++)
    {
        buffer_render_outdent(SCREEN_WIDTH + 19, 8 + (i * 32),32,32, 0xFFFFFF, 0x808080);
        buffer_render_outdent(SCREEN_WIDTH + 19 + 33, 8 + (i * 32),150,32, 0xFFFFFF, 0x808080);
    }

    if(!player_inventory) return;

    for(int i = 0; i < 7; i++)
    {
        if(player_inventory[i+inventory_scroll] == NULL) break;

        buffer_render_tile(SCREEN_WIDTH + 19, 8 + (i * 32), 255, player_inventory[i+inventory_scroll]);
        buffer_render_text(SCREEN_WIDTH + 19 + 32 + 10, 8 + (i * 32) + ((32/2) - deskAdvInvFontInfo.height/2), tile_names[player_inventory[i+inventory_scroll]]);
    }
}
#endif
