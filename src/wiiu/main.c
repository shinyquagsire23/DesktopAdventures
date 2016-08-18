#include <coreinit/core.h>
#include <coreinit/debug.h>
#include <coreinit/thread.h>
#include <coreinit/foreground.h>
#include <coreinit/systeminfo.h>
#include <coreinit/screen.h>
#include <proc_ui/procui.h>
#include <sysapp/launch.h>
#include <vpad/input.h>

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

void clearScreen(u8 r, u8 g, u8 b, u8 a)
{
    for (int ii = 0; ii < 2; ii++)
    {
        fillScreen(r,g,b,a);
        flipBuffers();
    }
}

void screenInit()
{
    //Grab the buffer size for each screen (TV and gamepad)
    int buf0_size = OSScreenGetBufferSizeEx(0);
    int buf1_size = OSScreenGetBufferSizeEx(1);
    OSReport("Screen sizes %x, %x\n", buf0_size, buf1_size);

    //Set the buffer area.
    screenBufferTop = MEM1_alloc(buf0_size, 0x100);
    screenBufferBottom = MEM1_alloc(buf1_size, 0x100);
    OSReport("Allocated screen buffers at %x, %x\n", screenBufferTop, screenBufferBottom);

    OSScreenSetBufferEx(0, screenBufferTop);
    OSScreenSetBufferEx(1, screenBufferBottom);
    OSReport("Set screen buffers\n");

    OSScreenEnableEx(0, 1);
    OSScreenEnableEx(1, 1);

    clearScreen(0,0,0,0);
}

void screenDeinit()
{
    for(int ii = 0; ii < 2; ii++)
    {
        fillScreen(0,0,0,0);
        flipBuffers();
    }

    MEM1_free(screenBufferTop);
    MEM1_free(screenBufferBottom);
}

void SaveCallback()
{
    OSSavesDone_ReadyToRelease(); // Required
}

bool AppRunning()
{
    if(!OSIsMainCore())
    {
        ProcUISubProcessMessages(true);
    }
    else
    {
        ProcUIStatus status = ProcUIProcessMessages(true);

        if(status == PROCUI_STATUS_EXITING)
        {
            // Being closed, deinit things and prepare to exit
            isAppRunning = false;

            if(initialized)
            {
                initialized = false;
                screenDeinit();
                memoryRelease();
                FSShutdown();
            }

            ProcUIShutdown();
        }
        else if(status == PROCUI_STATUS_RELEASE_FOREGROUND)
        {
            // Free up MEM1 to next foreground app, etc.
            initialized = false;

            screenDeinit();
            memoryRelease();
            ProcUIDrawDoneRelease();
        }
        else if(status == PROCUI_STATUS_IN_FOREGROUND)
        {
            // Reallocate MEM1, reinit screen, etc.
            if(!initialized)
            {
                initialized = true;

                memoryInitialize();
                screenInit();
            }
        }
    }

    return isAppRunning;
}

int
CoreEntryPoint(int argc, const char **argv)
{
   OSReport("Hello world from %s", argv[0]);
   return argc;
}

int
main(int argc, char **argv)
{
    OSScreenInit();
    ProcUIInit(&SaveCallback);
    AppRunning();

    setActiveScreen(SCREEN_BOTTOM);
    fillScreen(0,0,0,0);
    flipBuffers();
    setActiveScreen(SCREEN_TOP);
    fillScreen(0,0,0,0);
    flipBuffers();

    OSSystemInfo *info = OSGetSystemInfo();
    u32 clockSpeed = *(u32*)(info + sizeof(u32));

    chdir("fs:/vol/content/");
    load_resources();

    OSTime last_time = OSGetSystemTime();
    while(AppRunning())
    {
        if(!initialized) continue;

        OSTime time = OSGetSystemTime();
        double delta = (double)(((double)(time - last_time) * 1000.0f) / (OSGetSystemInfo()->clockSpeed / 4));
        //OSReport("delta %f\n", delta);
        last_time = time;

        reset_input_state();
        update_input();

        update_world(delta);
    }
    fsDevInit();
    return 0;
}

void update_input()
{
    int error;
    VPADStatus vpad_data;
    VPADRead(0, &vpad_data, 1, &error);

    if(vpad_data.trigger & VPAD_BUTTON_X)
    {
        if(current_map < NUM_MAPS)
        {
            unload_map();
            current_map++;
            load_map(current_map);
        }
    }
    else if(vpad_data.trigger & VPAD_BUTTON_Y)
    {
        if(current_map > 0)
        {
            unload_map();
            current_map--;
            load_map(current_map);
        }
    }

    if(vpad_data.trigger & VPAD_BUTTON_A)
    {
        button_fire();
    }

    if(vpad_data.hold & VPAD_BUTTON_UP)
        button_move_up();
    else if(vpad_data.hold & VPAD_BUTTON_RIGHT)
        button_move_right();
    else if(vpad_data.hold & VPAD_BUTTON_LEFT)
        button_move_left();
    else if(vpad_data.hold & VPAD_BUTTON_DOWN)
        button_move_down();
}

void render_flip_buffers()
{
    setActiveScreen(SCREEN_TOP);
    flipBuffers();
    setActiveScreen(SCREEN_BOTTOM);
    flipBuffers();
}

void buffer_clear_screen(u8 r, u8 g, u8 b, u8 a)
{
    setActiveScreen(SCREEN_TOP);
    fillScreen(r,g,b,a);
    setActiveScreen(SCREEN_BOTTOM);
    fillScreen(r,g,b,a);
}

void buffer_plot_pixel(int x, int y, u8 r, u8 g, u8 b, u8 a)
{
    int scale = 2;

    setActiveScreen(SCREEN_TOP);
    int x_shift = (getScreenWidth() - (288*scale)) / 2;
    int y_shift = (getScreenHeight() - (288*scale)) / 2;
    drawFillRect((x*scale)+x_shift,(y*scale)+y_shift,(x*scale)+x_shift+1,(y*scale)+y_shift+1,r,g,b,a);

    setActiveScreen(SCREEN_BOTTOM);
    scale = 1;
    x_shift = (getScreenWidth() - (288*scale)) / 2;
    y_shift = (getScreenHeight() - (288*scale)) / 2;
    drawFillRect((x*scale)+x_shift,(y*scale)+y_shift,(x*scale)+x_shift+1,(y*scale)+y_shift+1,r,g,b,a);

    setActiveScreen(SCREEN_TOP);
}
