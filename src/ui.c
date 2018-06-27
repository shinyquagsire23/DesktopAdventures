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
#include "ui.h"

#include <stdio.h>
#include "font.h"
#include "palette.h"
#include "player.h"
#include "screen.h"
#include "tname.h"
#include "assets.h"

int inventory_scroll = 0;
int draw_scale = 1;

bool WINDOW_ONLY = false;
int WINDOW_X = 0;
int WINDOW_Y = 0;
int ABS_MOUSE_X = 0, ABS_MOUSE_Y = 0;

bool WINDOW_MOVING = false;
int WINDOW_START_X, WINDOW_START_Y;
int WINDOW_X_MOVE_START, WINDOW_Y_MOVE_START;

bool MOUSE_HELD_LEFT = false;
bool MOUSE_DOWN_LEFT = false;
bool TOUCH_HELD = false;
bool TOUCH_DOWN = false;
bool TOUCH_UP = false;

bool rendering_world = false;

ui_render_target main_target;
ui_render_target window_target;
ui_render_target window_title_target;
ui_render_target window_content_target;
ui_render_target game_target;

void ui_get_target_bounds(ui_render_target* target, int* x1, int* y1, int* x2, int* y2)
{
    ui_render_target* iter = target;
    
    // Check cached values
    u32 c = (target->x | target->y << 32) ^ (target->w | target->h << 32);
    if (target->parent)
        c ^= target->parent->c;

    if (c == target->c)
    {
        *x1 = target->cx1;
        *y1 = target->cy1;
        if (x2)
            *x2 = target->cx2;
        if (y2)
            *y2 = target->cy2;
        return;
    }
    
    *x1 = 0;
    *y1 = 0;
    
    int minX = 0;
    int minY = 0;

    while(iter)
    {
        *x1 += iter->x;
        *y1 += iter->y;
        
        if (iter->parent)
        {
            int x, y, xMax, yMax;
            ui_get_target_bounds(iter->parent, &x, &y, &xMax, &yMax);
            minX = minX ? MIN(minX, xMax) : xMax;
            minY = minY ? MIN(minY, yMax) : yMax;
        }
        
        iter = iter->parent;
    }

    minX = minX ? MIN(*x1 + target->w, minX) : *x1 + target->w;
    minY = minY ? MIN(*y1 + target->h, minY) : *y1 + target->h;

    if (x2)
        *x2 = minX;
    
    if (y2)
        *y2 = minY;
    
    // Store cached values
    target->c = c;
    target->cx1 = *x1;
    target->cy1 = *y1;
    target->cx2 = minX;
    target->cy2 = minY;
}

void ui_render_target_clear(ui_render_target* target, u8 r, u8 g, u8 b, u8 a)
{
    u32 x1, x2, y1, y2;
    ui_get_target_bounds(target, &x1, &y1, &x2, &y2);
    
    drawFillRect(x1 * draw_scale, y1 * draw_scale, x2 * draw_scale, y2 * draw_scale, r, g, b, a);
}

void buffer_clear_screen(u8 r, u8 g, u8 b, u8 a)
{
    fillScreen(0, 0x82, 0x82, 0xff);

    ui_render_target_clear(&window_target, 200, 200, 200, 255);
    ui_render_target_clear(&game_target, r, g, b, a);
}

void buffer_plot_pixel(ui_render_target* target, int x, int y, u8 r, u8 g, u8 b, u8 a)
{    
    int x_shift = 0;
    int y_shift = 0;
    
    int x1, x2, y1, y2;
    ui_get_target_bounds(target, &x1, &y1, &x2, &y2);
    
    if (!CONTAINED_2D(x+x1+1,y+y1+1, x1, y1, x2+2, y2+2))
        return;
    
    x_shift = x1 * draw_scale;
    y_shift = y1 * draw_scale;
    
    drawFillRect((x*draw_scale)+x_shift,(y*draw_scale)+y_shift,(x*draw_scale)+x_shift+(draw_scale-1),(y*draw_scale)+y_shift+(draw_scale-1),r,g,b,a);
}


int buffer_render_char(ui_render_target* target, int x, int y, char c)
{
    if(c == ' ')
        return deskAdvInvFontInfo.space_width+1;

    int offset = deskAdvInvFontDescriptors[c-deskAdvInvFontInfo.start_char].offset;
    for(int i = 0; i < deskAdvInvFontDescriptors[c-deskAdvInvFontInfo.start_char].width; i++)
    {
        for(int j = 0; j < DESKADV_INV_FONT_HEIGHT; j++)
        {
            if(deskAdvInvFontBitmaps[offset+j] & (1<<(7-i)))
                buffer_plot_pixel(target, x+i,y+j,0,0,0,255);
        }
    }
    return deskAdvInvFontDescriptors[c-deskAdvInvFontInfo.start_char].width+1;
}

void buffer_render_text(ui_render_target* target, int x, int y, char *text)
{
    for(int i = 0; i < strlen(text); i++)
    {
        x += buffer_render_char(target, x,y,text[i]);
    }
}

void buffer_draw_line(ui_render_target* target, int x1, int y1, int x2, int y2, char r, char g, char b, char a)
{

	int x, y;
	if (x1 == x2){
		if (y1 < y2) for (y = y1; y <= y2; y++) buffer_plot_pixel(target, x1, y, r, g, b, a);
		else for (y = y2; y <= y1; y++) buffer_plot_pixel(target, x1, y, r, g, b, a);
	}
	else {
		if (x1 < x2) for (x = x1; x <= x2; x++) buffer_plot_pixel(target, x, y1, r, g, b, a);
		else for (x = x2; x <= x1; x++) buffer_plot_pixel(target, x, y1, r, g, b, a);
	}
}

void buffer_render_outdent(ui_render_target* target, int x, int y, int width, int height, u32 highlight, u32 shadow)
{
    u8 r,g,b,a;
    
    r = (u8)((shadow & 0xFF0000) >> 16);
    g = (u8)((shadow & 0xFF00) >> 8);
    b = (u8)((shadow & 0xFF));
    a = 255;
    
    buffer_draw_line(target, x+width-1,y+height-1,x, y+height-1, r,g,b,a);
    buffer_draw_line(target, x+width-1,y+height-1,x+width-1, y, r,g,b,a);

    r = (u8)((highlight & 0xFF0000) >> 16);
    g = (u8)((highlight & 0xFF00) >> 8);
    b = (u8)((highlight & 0xFF));
    a = 255;

    buffer_draw_line(target, x,y,x+width-1,y, r,g,b,a);
    buffer_draw_line(target, x,y,x,y+height-2, r,g,b,a);
}

void buffer_render_tile(ui_render_target* target, int x, int y, u8 alpha, u32 tile)
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

        buffer_plot_pixel(target, x+(i%width),y+(i/height),r,g,b,alpha);
    }
}

void ui_init(int x, int y, int w, int h, bool windowOnly)
{
    main_target.x = x;
    main_target.y = y;
    main_target.w = w;
    main_target.h = h;
    main_target.parent = NULL;
    
    window_target.parent = &main_target;
    window_target.x = WINDOW_X;
    window_target.y = WINDOW_Y;
    window_target.w = (SCREEN_WIDTH+236+3);
    window_target.h = (SCREEN_HEIGHT+16+3+12);
    
    window_title_target.parent = &window_target;
    window_title_target.x = 3;
    window_title_target.y = 3;
    window_title_target.w = SCREEN_WIDTH+236-2;
    window_title_target.h = 3+12-2;

    window_content_target.parent = &window_target;
    window_content_target.x = 2;
    window_content_target.y = 2 + 12;
    window_content_target.w = (SCREEN_WIDTH+236+1);
    window_content_target.h = (SCREEN_HEIGHT+16+1);
    
    if (windowOnly)
    {
        main_target.x = -2;
        main_target.y = -18;
        main_target.w = (SCREEN_WIDTH+236+1);
        main_target.h = (SCREEN_HEIGHT+16+1)+12;
        WINDOW_ONLY = true;
    }

    game_target.parent = &window_content_target;
    
    game_target.x = 8;
    game_target.y = 8;
    game_target.w = SCREEN_WIDTH;
    game_target.h = SCREEN_HEIGHT;
    render_set_target(&game_target);
}

void ui_update()
{
    window_target.x = WINDOW_X;
    window_target.y = WINDOW_Y;
    
    // Window movement
    int windowTitleX = (WINDOW_X+3);
    int windowTitleY = (WINDOW_Y+3);
    int windowTitleX2 = (WINDOW_X+SCREEN_WIDTH+236);
    int windowTitleY2 = (WINDOW_Y+3+12);
    if ((MOUSE_HELD_LEFT | TOUCH_HELD) && CONTAINED_2D(ABS_MOUSE_X, ABS_MOUSE_Y, windowTitleX, windowTitleY, windowTitleX2, windowTitleY2) && !WINDOW_MOVING && !WINDOW_ONLY)
    {
        WINDOW_MOVING = true;
        WINDOW_X_MOVE_START = ABS_MOUSE_X;
        WINDOW_Y_MOVE_START = ABS_MOUSE_Y;
        WINDOW_START_X = WINDOW_X;
        WINDOW_START_Y = WINDOW_Y;
    }
    
    if (!(MOUSE_HELD_LEFT | TOUCH_HELD) && WINDOW_MOVING)
        WINDOW_MOVING = false;

    if (WINDOW_MOVING)
    {
        WINDOW_X = WINDOW_START_X + ABS_MOUSE_X - WINDOW_X_MOVE_START;
        WINDOW_Y = WINDOW_START_Y + ABS_MOUSE_Y - WINDOW_Y_MOVE_START;
    }
    
    // Window world movement
    static int last_x, last_y;
    bool mouseMoved = false;
    
    if (last_x != ABS_MOUSE_X || last_y != ABS_MOUSE_Y)
        mouseMoved = true;
    last_x = ABS_MOUSE_X;
    last_y = ABS_MOUSE_Y;
    
    u32 game_x, game_y;
    ui_get_target_bounds(&game_target, &game_x, &game_y, NULL, NULL);
    if (CURRENT_ITEM_DRAGGED != -1)
        mouse_move(-1,-1);
    else if (CONTAINED_2D(ABS_MOUSE_X - game_x, ABS_MOUSE_Y - game_y, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT) && mouseMoved)
        mouse_move(ABS_MOUSE_X-game_x,ABS_MOUSE_Y-game_y);
    else if (!CONTAINED_2D(ABS_MOUSE_X - game_x, ABS_MOUSE_Y - game_y, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT) && mouseMoved)
        mouse_move(-1,-1);
    
    // Item drag and drop
    if(MOUSE_DOWN_LEFT || TOUCH_DOWN || TOUCH_UP)
    {
        if (MOUSE_DOWN_LEFT)
            MOUSE_DOWN_LEFT = false;
        if (TOUCH_DOWN)
            TOUCH_DOWN = false;
        if (TOUCH_UP)
            TOUCH_UP = false;

        u32 window_contents_x, window_contents_y;
        ui_get_target_bounds(&window_content_target, &window_contents_x, &window_contents_y, NULL, NULL);

        if(CURRENT_ITEM_DRAGGED != -1)
        {
            if (CONTAINED_2D(ABS_MOUSE_X - game_x, ABS_MOUSE_Y - game_y, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT))
                drop_item(ABS_MOUSE_X - game_x, ABS_MOUSE_Y - game_y);
            else if(CONTAINED_2D(ABS_MOUSE_X, ABS_MOUSE_Y, 
                    window_contents_x + SCREEN_WIDTH + 16 + 17 + 79, window_contents_y + 237 + 19, 
                    window_contents_x + SCREEN_WIDTH + 16 + 17 + 79 + 32, window_contents_y + 237 + 19 + 32))
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
                                window_contents_x + item_render_x, window_contents_y + item_render_y,
                                window_contents_x + item_render_x + 32, window_contents_y + item_render_y + 32))
                {
                    CURRENT_ITEM_DRAGGED = i+inventory_scroll;
                }
            }
        }
    }
}

void ui_render()
{
    //Game Surrounding box
    buffer_render_outdent(&window_content_target, 5, 5, SCREEN_WIDTH+6,SCREEN_HEIGHT+6, 0x808080, 0xFFFFFF);
    buffer_render_outdent(&window_content_target, 6, 6, SCREEN_WIDTH+4,SCREEN_HEIGHT+4, 0x808080, 0xFFFFFF);
    buffer_render_outdent(&window_content_target, 7, 7, SCREEN_WIDTH+2,SCREEN_HEIGHT+2, 0x808080, 0xFFFFFF);

    //Surrounding box
    buffer_render_outdent(&window_content_target, SCREEN_WIDTH + 17, 6, 187,228, 0x808080, 0xFFFFFF);
    buffer_render_outdent(&window_content_target, SCREEN_WIDTH + 18, 7, 185,226, 0x808080, 0xFFFFFF);

    //Force box
    buffer_render_outdent(&window_content_target, SCREEN_WIDTH + 17 + 77, 236 + 18, 13,36, 0x808080, 0xFFFFFF);
    buffer_render_outdent(&window_content_target, SCREEN_WIDTH + 17 + 78, 237 + 18, 11,34, 0x808080, 0xFFFFFF);
    buffer_render_outdent(&window_content_target, SCREEN_WIDTH + 17 + 79, 237 + 19, 9,32, 0xFFFFFF, 0x808080);

    //Item box
    buffer_render_outdent(&window_content_target, SCREEN_WIDTH + 16 + 17 + 77, 236 + 18, 36,36, 0x808080, 0xFFFFFF);
    buffer_render_outdent(&window_content_target, SCREEN_WIDTH + 16 + 17 + 78, 237 + 18, 34,34, 0x808080, 0xFFFFFF);
    buffer_render_outdent(&window_content_target, SCREEN_WIDTH + 16 + 17 + 79, 237 + 19, 32,32, 0xFFFFFF, 0x808080);
    if(PLAYER_EQUIPPED_ITEM != 0xFFFF)
    {
        buffer_render_tile(&window_content_target, SCREEN_WIDTH + 16 + 17 + 79, 237 + 19, 255, player_inventory[PLAYER_EQUIPPED_ITEM]);
    }

    //Scroll box
    buffer_render_outdent(&window_content_target, SCREEN_WIDTH + 17 + 190, 6, 20,228, 0x808080, 0xFFFFFF);
    buffer_render_outdent(&window_content_target, SCREEN_WIDTH + 18 + 190, 7, 18,226, 0x808080, 0xFFFFFF);

    for(int i = 0; i < 7; i++)
    {
        buffer_render_outdent(&window_content_target, SCREEN_WIDTH + 19, 8 + (i * 32),32,32, 0xFFFFFF, 0x808080);
        buffer_render_outdent(&window_content_target, SCREEN_WIDTH + 19 + 33, 8 + (i * 32),150,32, 0xFFFFFF, 0x808080);
    }

    for(int i = 0; i < 7; i++)
    {
        if(!player_inventory) break;
        if(player_inventory[i+inventory_scroll] == NULL) break;

        int item_render_x = SCREEN_WIDTH + 19;
        int item_render_y = 8 + (i * 32);
        if(CURRENT_ITEM_DRAGGED != i+inventory_scroll)
            buffer_render_tile(&window_content_target, item_render_x, item_render_y, 255, player_inventory[i+inventory_scroll]);

        buffer_render_text(&window_content_target, SCREEN_WIDTH + 19 + 32 + 10, 8 + (i * 32) + ((32/2) - deskAdvInvFontInfo.height/2), tile_names[player_inventory[i+inventory_scroll]]);
    }
    
    buffer_render_outdent(&window_target, 0, 0, (SCREEN_WIDTH+236)+4, (SCREEN_HEIGHT+16+12)+4, 0xc3c3c3, 0x000000);
    buffer_render_outdent(&window_target, 1, 1, (SCREEN_WIDTH+236)+2, (SCREEN_HEIGHT+16+12)+2, 0xffffff, 0x828282);
    ui_render_target_clear(&window_title_target, 0, 0, 0x82, 255);

    if(CURRENT_ITEM_DRAGGED != -1)
    {
        buffer_render_tile(&main_target, ABS_MOUSE_X - 16, ABS_MOUSE_Y - 16, 255, player_inventory[CURRENT_ITEM_DRAGGED]);
    }
    else
    {
        //TODO
        /*for (int x = 0; x < 12; x++)
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
        }*/
    }
}

void ui_set_mouse_abs(int x, int y)
{
    ABS_MOUSE_X = x / draw_scale;
    ABS_MOUSE_Y = y / draw_scale;
}

void ui_add_mouse_abs(int x, int y)
{
    ABS_MOUSE_X += x;
    ABS_MOUSE_Y += y;
}

void ui_mouse_left_down()
{
    MOUSE_DOWN_LEFT = true;
}

void ui_set_mouse_left(bool val)
{
    MOUSE_HELD_LEFT = val;
}

void ui_touch_down()
{
    TOUCH_DOWN = true;
    TOUCH_UP = false;
    TOUCH_HELD = true;
}

void ui_touch_up()
{
    TOUCH_DOWN = false;
    TOUCH_UP = true; 
    TOUCH_HELD = false;
}

void ui_set_draw_scale(int scale)
{
    draw_scale = scale;
}
