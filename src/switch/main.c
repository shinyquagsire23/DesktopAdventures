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

#include "player.h"
#include "palette.h"
#include "draw.h"
#include "useful.h"
#include "assets.h"
#include "screen.h"
#include "input.h"
#include "font.h"
#include "tname.h"
#include "map.h"

bool initialized = false;
bool isAppRunning = true;

u16 current_map = 0;
bool exiting = false;
bool rendering_world = false;
bool rendering_in_window = false;

int inventory_scroll = 0;
int draw_scale = 2;

int WINDOW_X = 25;
int WINDOW_Y = 20;
int WINDOW_CONTENTS_X, WINDOW_CONTENTS_Y;
int ABS_MOUSE_X = 0, ABS_MOUSE_Y = 0;

bool WINDOW_MOVING = false;
int WINDOW_START_X, WINDOW_START_Y;
int WINDOW_X_MOVE_START, WINDOW_Y_MOVE_START;

int
main(int argc, char **argv)
{
    gfxInitDefault();
    
    SCREEN_SHIFT_X = 8 + (WINDOW_X + 2);
    SCREEN_SHIFT_Y = 8 + (WINDOW_Y + 2 + 12);
    WINDOW_CONTENTS_X = WINDOW_X + 2;
    WINDOW_CONTENTS_Y = WINDOW_Y + 2 + 12;
    
    load_resources();

    //TODO: Hack, we're just measuring how many ticks 1ms is for timing
    // because clock() doesn't have ms resolution...
    u64 tick_pre = svcGetSystemTick();
    svcSleepThread(1000*1000);
    u64 tick_post = svcGetSystemTick();
    
    double ticks_per_ms = (double)(tick_post - tick_pre);
    
    u64 last_tick = svcGetSystemTick();
    while(appletMainLoop())
    {
        u64 tick = svcGetSystemTick();
        double delta = (double)((double)(tick - last_tick)/(ticks_per_ms));
        last_tick = tick;

        reset_input_state();
        update_input();
        
        SCREEN_SHIFT_X = 8 + (WINDOW_X + 2);
        SCREEN_SHIFT_Y = 8 + (WINDOW_Y + 2 + 12);
        WINDOW_CONTENTS_X = WINDOW_X + 2;
        WINDOW_CONTENTS_Y = WINDOW_Y + 2 + 12;
        
        if (exiting) break;

        update_world(delta);
        gfxWaitForVsync();
    }
    
    gfxExit();
    return 0;
}

void update_input()
{
    hidScanInput();


    // Update mouse
    MousePosition mouse_pos = {0};
    JoystickPosition joy_pos = {0};
    hidJoystickRead(&joy_pos, CONTROLLER_P1_AUTO, JOYSTICK_RIGHT);
    hidMouseRead(&mouse_pos);
    
    bool mouseMoved = false;
    if (hidTouchCount())
    {
        touchPosition pos;
        hidTouchRead(&pos, 0);

        ABS_MOUSE_X = pos.px / draw_scale;
        ABS_MOUSE_Y = pos.py / draw_scale;
        mouseMoved = true;
    }
    else if (mouse_pos.velocityX || mouse_pos.velocityY)
    {
        ABS_MOUSE_X = mouse_pos.x / draw_scale;
        ABS_MOUSE_Y = mouse_pos.y / draw_scale;
        mouseMoved = true;
    }
    else
    {
        ABS_MOUSE_X += (int)((float)joy_pos.dx / (float)JOYSTICK_MAX * 16.0f);
        ABS_MOUSE_Y -= (int)((float)joy_pos.dy / (float)JOYSTICK_MAX * 16.0f);
        
        if (joy_pos.dx || joy_pos.dy)
            mouseMoved = true;
    }
    
    ABS_MOUSE_X = CLAMP(ABS_MOUSE_X, 0, getScreenWidth() / draw_scale);
    ABS_MOUSE_Y = CLAMP(ABS_MOUSE_Y, 0, getScreenHeight() / draw_scale);
    
    bool mouse_held_left = hidMouseButtonsHeld() & MOUSE_LEFT 
                            || hidKeysHeld(CONTROLLER_P1_AUTO) & (KEY_ZR | KEY_R | KEY_SR | KEY_TOUCH);
    bool mouse_held_right = hidMouseButtonsHeld() & MOUSE_RIGHT
                             || hidKeysHeld(CONTROLLER_P1_AUTO) & (KEY_ZL | KEY_L | KEY_SL);

    bool mouse_down_left = hidMouseButtonsDown() & MOUSE_LEFT 
                            || hidKeysDown(CONTROLLER_P1_AUTO) & (KEY_ZR | KEY_R | KEY_SR | KEY_TOUCH); 
    bool mouse_down_right = hidMouseButtonsDown() & MOUSE_RIGHT
                             || hidKeysDown(CONTROLLER_P1_AUTO) & (KEY_ZL | KEY_L | KEY_SL);
    bool touch_up = hidKeysUp(CONTROLLER_P1_AUTO) & KEY_TOUCH;

    // Window movement
    int windowTitleX = (WINDOW_X+3);
    int windowTitleY = (WINDOW_Y+3);
    int windowTitleX2 = (WINDOW_X+SCREEN_WIDTH+236);
    int windowTitleY2 = (WINDOW_Y+3+12);
    if (mouse_held_left && CONTAINED_2D(ABS_MOUSE_X, ABS_MOUSE_Y, windowTitleX, windowTitleY, windowTitleX2, windowTitleY2) && !WINDOW_MOVING)
    {
        WINDOW_MOVING = true;
        WINDOW_X_MOVE_START = ABS_MOUSE_X;
        WINDOW_Y_MOVE_START = ABS_MOUSE_Y;
        WINDOW_START_X = WINDOW_X;
        WINDOW_START_Y = WINDOW_Y;
    }
    
    if (!mouse_held_left && WINDOW_MOVING)
        WINDOW_MOVING = false;

    if (WINDOW_MOVING)
    {
        WINDOW_X = WINDOW_START_X + ABS_MOUSE_X - WINDOW_X_MOVE_START;
        WINDOW_Y = WINDOW_START_Y + ABS_MOUSE_Y - WINDOW_Y_MOVE_START;
        SCREEN_SHIFT_X = 8 + (WINDOW_X + 2);
        SCREEN_SHIFT_Y = 8 + (WINDOW_Y + 2 + 12);
        WINDOW_CONTENTS_X = WINDOW_X + 2;
        WINDOW_CONTENTS_Y = WINDOW_Y + 2 + 12;
    }

    // Window world movement
    if (CURRENT_ITEM_DRAGGED != -1)
        mouse_move(-1,-1);
    else if (CONTAINED_2D(ABS_MOUSE_X - SCREEN_SHIFT_X, ABS_MOUSE_Y - SCREEN_SHIFT_Y, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT) && mouseMoved)
        mouse_move(ABS_MOUSE_X-SCREEN_SHIFT_X,ABS_MOUSE_Y-SCREEN_SHIFT_Y);
    else if (!CONTAINED_2D(ABS_MOUSE_X - SCREEN_SHIFT_X, ABS_MOUSE_Y - SCREEN_SHIFT_Y, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT) && mouseMoved)
        mouse_move(-1,-1);

    if(mouse_held_right)
        mouse_right();

    if(mouse_held_left)
        mouse_left();

    // Item drag and drop
    if(mouse_down_left || touch_up)
    {
        if(CURRENT_ITEM_DRAGGED != -1)
        {
            if (CONTAINED_2D(ABS_MOUSE_X - SCREEN_SHIFT_X, ABS_MOUSE_Y - SCREEN_SHIFT_Y, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT))
                drop_item(ABS_MOUSE_X - SCREEN_SHIFT_X, ABS_MOUSE_Y - SCREEN_SHIFT_Y);
            else if(CONTAINED_2D(ABS_MOUSE_X, ABS_MOUSE_Y, 
                    WINDOW_CONTENTS_X + SCREEN_WIDTH + 16 + 17 + 79, WINDOW_CONTENTS_Y + 237 + 19, 
                    WINDOW_CONTENTS_X + SCREEN_WIDTH + 16 + 17 + 79 + 32, WINDOW_CONTENTS_Y + 237 + 19 + 32))
            {
                
                player_equip_item(CURRENT_ITEM_DRAGGED);
                CURRENT_ITEM_DRAGGED = -1;
            }
            else
            {
                CURRENT_ITEM_DRAGGED = -1;
            }
        }
        else
        {
            for(int i = 0; i < 7; i++)
            {
                if(!player_inventory) break;
                if(player_inventory[i+inventory_scroll] == NULL) break;

                int item_render_x = SCREEN_WIDTH + 19;
                int item_render_y = 8 + (i * 32);

                if(CONTAINED_2D(ABS_MOUSE_X, ABS_MOUSE_Y, 
                                WINDOW_CONTENTS_X + item_render_x, WINDOW_CONTENTS_Y + item_render_y,
                                WINDOW_CONTENTS_X + item_render_x + 32, WINDOW_CONTENTS_Y + item_render_y + 32))
                {
                    CURRENT_ITEM_DRAGGED = i+inventory_scroll;
                }
            }
        }
    }
    
    // Only keyboard/controller from here on
    if(CURRENT_ITEM_DRAGGED != -1) return;
    
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

    if(hidKeysHeld(CONTROLLER_P1_AUTO) & KEY_A || hidKeyboardHeld(KBD_SPACE))
    {
        button_fire();
    }
    
    if (hidKeysHeld(CONTROLLER_P1_AUTO) & KEY_B || hidKeyboardModifierHeld(KBD_MOD_LSHIFT) || hidKeyboardModifierHeld(KBD_MOD_RSHIFT))
    {
        button_push();
    }

    bool didButtonMovement = true;
    if(hidKeysHeld(CONTROLLER_P1_AUTO) & (KEY_DUP | KEY_LSTICK_UP) || hidKeyboardHeld(KBD_UP))
        button_move_up();
    else if(hidKeysHeld(CONTROLLER_P1_AUTO) & (KEY_DRIGHT | KEY_LSTICK_RIGHT) || hidKeyboardHeld(KBD_RIGHT))
        button_move_right();
    else if(hidKeysHeld(CONTROLLER_P1_AUTO) & (KEY_DLEFT | KEY_LSTICK_LEFT) || hidKeyboardHeld(KBD_LEFT))
        button_move_left();
    else if(hidKeysHeld(CONTROLLER_P1_AUTO) & (KEY_DDOWN | KEY_LSTICK_DOWN) || hidKeyboardHeld(KBD_DOWN))
        button_move_down();
    else
        didButtonMovement = false;
    
    // Disable mouse look if we're using buttons
    if (didButtonMovement) mouse_move(-1,-1);
    
    if(hidKeysHeld(CONTROLLER_P1_AUTO) & KEY_PLUS || hidKeyboardDown(KBD_ESC))
        exiting = true;
}

void render_flip_buffers()
{
    gfxFlushBuffers();
    gfxSwapBuffers();
}

void buffer_clear_screen(u8 r, u8 g, u8 b, u8 a)
{
    fillScreen(0, 0x82, 0x82, 0xff);
    
    drawFillRect(WINDOW_X * draw_scale, WINDOW_Y * draw_scale, (WINDOW_X+SCREEN_WIDTH+236+3) * draw_scale, (WINDOW_Y+SCREEN_HEIGHT+16+3+12) * draw_scale, 200, 200, 200, 255);
    drawFillRect((SCREEN_SHIFT_X) * draw_scale, (SCREEN_SHIFT_Y) * draw_scale, (SCREEN_SHIFT_X+SCREEN_WIDTH) * draw_scale, (SCREEN_SHIFT_Y+SCREEN_HEIGHT) * draw_scale, r, g, b, a);
}

void buffer_plot_pixel(int x, int y, u8 r, u8 g, u8 b, u8 a)
{
    //TODO: I'd like the notion of a 'render target' which can clamp rendering to specified bounds
    // and have the x and y be relative to those bounds.
    if (rendering_in_window)
    {
        x += WINDOW_CONTENTS_X;
        y += WINDOW_CONTENTS_Y;
    }
    
    int x_shift = 0;
    int y_shift = 0;
    
    if(rendering_world)
    {
        if (!CONTAINED_2D(x,y, SCREEN_SHIFT_X-1, SCREEN_SHIFT_Y-1, SCREEN_SHIFT_X+SCREEN_WIDTH, SCREEN_SHIFT_Y+SCREEN_HEIGHT))
            return;
    }
    
    drawFillRect((x*draw_scale)+x_shift,(y*draw_scale)+y_shift,(x*draw_scale)+x_shift+1,(y*draw_scale)+y_shift+1,r,g,b,a);
}


int buffer_render_char(int x, int y, char c)
{
    if(c == ' ')
        return deskAdvInvFontInfo.space_width+1;

    int offset = deskAdvInvFontDescriptors[c-deskAdvInvFontInfo.start_char].offset;
    for(int i = 0; i < deskAdvInvFontDescriptors[c-deskAdvInvFontInfo.start_char].width; i++)
    {
        for(int j = 0; j < DESKADV_INV_FONT_HEIGHT; j++)
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

void buffer_draw_line(int x1, int y1, int x2, int y2, char r, char g, char b, char a)
{

	int x, y;
	if (x1 == x2){
		if (y1 < y2) for (y = y1; y <= y2; y++) buffer_plot_pixel(x1, y, r, g, b, a);
		else for (y = y2; y <= y1; y++) buffer_plot_pixel(x1, y, r, g, b, a);
	}
	else {
		if (x1 < x2) for (x = x1; x <= x2; x++) buffer_plot_pixel(x, y1, r, g, b, a);
		else for (x = x2; x <= x1; x++) buffer_plot_pixel(x, y1, r, g, b, a);
	}
}

void buffer_render_outdent(int x, int y, int width, int height, u32 highlight, u32 shadow)
{
    u8 r,g,b,a;
    
    r = (u8)((shadow & 0xFF0000) >> 16);
    g = (u8)((shadow & 0xFF00) >> 8);
    b = (u8)((shadow & 0xFF));
    a = 255;
    
    buffer_draw_line(x+width-1,y+height-1,x, y+height-1, r,g,b,a);
    buffer_draw_line(x+width-1,y+height-1,x+width-1, y, r,g,b,a);

    r = (u8)((highlight & 0xFF0000) >> 16);
    g = (u8)((highlight & 0xFF00) >> 8);
    b = (u8)((highlight & 0xFF));
    a = 255;

    buffer_draw_line(x,y,x+width-1,y, r,g,b,a);
    buffer_draw_line(x,y,x,y+height-2, r,g,b,a);
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

void render_pre()
{
    rendering_world = true;
    rendering_in_window = false;
}

void render_post()
{
    rendering_world = false;
    rendering_in_window = true;
    
    MousePosition mouse_pos = {0};
    hidMouseRead(&mouse_pos);
    mouse_pos.x /= draw_scale;
    mouse_pos.y /= draw_scale;
    
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
    if(PLAYER_EQUIPPED_ITEM != 0xFFFF)
    {
        buffer_render_tile(SCREEN_WIDTH + 16 + 17 + 79, 237 + 19, 255, player_inventory[PLAYER_EQUIPPED_ITEM]);
    }

    //Scroll box
    buffer_render_outdent(SCREEN_WIDTH + 17 + 190, 6, 20,228, 0x808080, 0xFFFFFF);
    buffer_render_outdent(SCREEN_WIDTH + 18 + 190, 7, 18,226, 0x808080, 0xFFFFFF);

    for(int i = 0; i < 7; i++)
    {
        buffer_render_outdent(SCREEN_WIDTH + 19, 8 + (i * 32),32,32, 0xFFFFFF, 0x808080);
        buffer_render_outdent(SCREEN_WIDTH + 19 + 33, 8 + (i * 32),150,32, 0xFFFFFF, 0x808080);
    }

    for(int i = 0; i < 7; i++)
    {
        if(!player_inventory) break;
        if(player_inventory[i+inventory_scroll] == NULL) break;

        int item_render_x = SCREEN_WIDTH + 19;
        int item_render_y = 8 + (i * 32);
        if(CURRENT_ITEM_DRAGGED != i+inventory_scroll)
            buffer_render_tile(item_render_x, item_render_y, 255, player_inventory[i+inventory_scroll]);

        buffer_render_text(SCREEN_WIDTH + 19 + 32 + 10, 8 + (i * 32) + ((32/2) - deskAdvInvFontInfo.height/2), tile_names[player_inventory[i+inventory_scroll]]);
    }
    
    // Render mouse and border
    rendering_in_window = false;
    
    buffer_render_outdent(WINDOW_X, WINDOW_Y, (SCREEN_WIDTH+236)+4, (SCREEN_HEIGHT+16+12)+4, 0xc3c3c3, 0x000000);
    buffer_render_outdent(WINDOW_X+1, WINDOW_Y+1, (SCREEN_WIDTH+236)+2, (SCREEN_HEIGHT+16+12)+2, 0xffffff, 0x828282);
    drawFillRect((WINDOW_X+3) * draw_scale, (WINDOW_Y+3) * draw_scale, (WINDOW_X+SCREEN_WIDTH+236) * draw_scale, (WINDOW_Y+3+12) * draw_scale, 0, 0, 0x82, 255);

    if(CURRENT_ITEM_DRAGGED != -1)
    {
        buffer_render_tile(ABS_MOUSE_X - 16, ABS_MOUSE_Y - 16, 255, player_inventory[CURRENT_ITEM_DRAGGED]);
    }
    else
    {
        for (int x = 0; x < 12; x++)
        {
            for (int y = 0; y < 21; y++)
            {
                extern u8 *win95_cursor_bin;
                u32 pixel = ((u32*)(&win95_cursor_bin))[(y * 12) + x];
                
                u8 r,g,b,a;
                r = (u8)((pixel & 0xFF));
                g = (u8)((pixel & 0xFF00) >> 8);
                b = (u8)((pixel & 0xFF0000) >> 16);
                a = (u8)((pixel & 0xFF000000) >> 24);
                
                if (!a) continue;
                buffer_plot_pixel(ABS_MOUSE_X + x, ABS_MOUSE_Y + y, r, g, b, a);
            }
        }
    }
}
