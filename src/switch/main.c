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
#include "tname.h"
#include "map.h"
#include "ui.h"

bool initialized = false;
bool isAppRunning = true;

u16 current_map = 0;
bool exiting = false;

int
main(int argc, char **argv)
{
    gfxInitDefault();
    
    ui_init(0,0,1280,720, false);
    ui_set_draw_scale(2);
    ui_update();
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
        ui_update();
        
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

        ui_set_mouse_abs(pos.px, pos.py);
        mouseMoved = true;
    }
    else if (mouse_pos.velocityX || mouse_pos.velocityY)
    {
        ui_set_mouse_abs(mouse_pos.x, mouse_pos.y);
        mouseMoved = true;
    }
    else
    {
        ui_set_mouse_abs((int)((float)joy_pos.dx / (float)JOYSTICK_MAX * 16.0f), -(int)((float)joy_pos.dy / (float)JOYSTICK_MAX * 16.0f));
        
        if (joy_pos.dx || joy_pos.dy)
            mouseMoved = true;
    }
    
    bool mouse_held_left = hidMouseButtonsHeld() & MOUSE_LEFT 
                            || hidKeysHeld(CONTROLLER_P1_AUTO) & (KEY_ZR | KEY_R | KEY_SR | KEY_TOUCH);
    bool mouse_held_right = hidMouseButtonsHeld() & MOUSE_RIGHT
                             || hidKeysHeld(CONTROLLER_P1_AUTO) & (KEY_ZL | KEY_L | KEY_SL);

    bool mouse_down_left = hidMouseButtonsDown() & MOUSE_LEFT 
                            || hidKeysDown(CONTROLLER_P1_AUTO) & (KEY_ZR | KEY_R | KEY_SR | KEY_TOUCH); 
    bool mouse_down_right = hidMouseButtonsDown() & MOUSE_RIGHT
                             || hidKeysDown(CONTROLLER_P1_AUTO) & (KEY_ZL | KEY_L | KEY_SL);
    bool touch_up = hidKeysUp(CONTROLLER_P1_AUTO) & KEY_TOUCH;

    if(mouse_held_right)
        mouse_right();

    if(mouse_held_left)
        mouse_left();
    
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

void render_pre()
{
}

void render_post()
{
    ui_render();
}
