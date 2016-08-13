#include "draw.h"

#include <coreinit/cache.h>
#include <coreinit/screen.h>
#include <gx2/display.h>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void *screenBufferTop;
void *screenBufferBottom;
int activeScreen = 0;

u32 getScreenWidth()
{
    if(activeScreen == 0)
        return 1280;
    else
        return 854;
}

u32 getScreenHeight()
{
    if(activeScreen == 0)
        return 720;
    else
        return 480;
}

void setActiveScreen(int screen)
{
    activeScreen = screen;
}

void flipBuffers()
{
	//Grab the buffer size for each screen (TV and gamepad)
	int buf0_size = OSScreenGetBufferSizeEx(0);
	int buf1_size = OSScreenGetBufferSizeEx(1);
	
	//Flush the cache
	if(activeScreen == SCREEN_BOTTOM)
	    DCFlushRange(screenBufferBottom, buf1_size);
	else
	    DCFlushRange(screenBufferTop, buf0_size);
	
	//Flip the buffer
	OSScreenFlipBuffersEx(activeScreen);
}

void drawOSString(int x, int y, char * string)
{
	OSScreenPutFontEx(activeScreen, x, y, string);
}

void drawOSStringf(int x, int y, const char *format, ...)
{
    char buffer[0x500];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, 0x500, format, args);
    va_end(args);
    
	OSScreenPutFontEx(activeScreen, x, y, buffer);
}

void fillScreen(char r,char g,char b,char a)
{
	uint32_t num = (r << 24) | (g << 16) | (b << 8) | a;
	OSScreenClearBufferEx(activeScreen, num);
}

//Rendering in 
void drawPixel(int x, int y, char r, char g, char b, char a)
{
	uint32_t num = (r << 24) | (g << 16) | (b << 8) | a;
	OSScreenPutPixelEx(activeScreen,x,y,num);
}

void drawLine(int x1, int y1, int x2, int y2, char r, char g, char b, char a)
{

	int x, y;
	if (x1 == x2){
		if (y1 < y2) for (y = y1; y <= y2; y++) drawPixel(x1, y, r, g, b, a);
		else for (y = y2; y <= y1; y++) drawPixel(x1, y, r, g, b, a);
	}
	else {
		if (x1 < x2) for (x = x1; x <= x2; x++) drawPixel(x, y1, r, g, b, a);
		else for (x = x2; x <= x1; x++) drawPixel(x, y1, r, g, b, a);
	}
}

void drawBorder(int thickness, char r, char g, char b, char a)
{
    drawRectThickness(0,0,getScreenWidth()-1,getScreenHeight()-1,thickness,r,g,b,a);
}

void drawRectThickness(int x1, int y1, int x2, int y2, int thickness, char r, char g, char b, char a)
{
	for(int i = 0; i < thickness; i++)
    {
        drawRect(x1+i,y1+i,x2-i,y2-i, r, g, b, a);
    }
}

void drawRect(int x1, int y1, int x2, int y2, char r, char g, char b, char a)
{
	drawLine(x1, y1, x2, y1, r, g, b, a);
	drawLine(x2, y1, x2, y2, r, g, b, a);
	drawLine(x1, y2, x2, y2, r, g, b, a);
	drawLine(x1, y1, x1, y2, r, g, b, a);
}

void drawFillRect(int x1, int y1, int x2, int y2, char r, char g, char b, char a)
{
	int X1, X2, Y1, Y2, i, j;

	if (x1 < x2){
		X1 = x1;
		X2 = x2;
	}
	else {
		X1 = x2;
		X2 = x1;
	}

	if (y1 < y2){
		Y1 = y1;
		Y2 = y2;
	}
	else {
		Y1 = y2;
		Y2 = y1;
	}
	for (i = X1; i <= X2; i++){
		for (j = Y1; j <= Y2; j++){
			drawPixel(i, j, r, g, b, a);
		}
	}
}

void drawCircle(int xCen, int yCen, int radius, char r, char g, char b, char a)
{
	int x = 0;
	int y = radius;
	int p = (5 - radius * 4) / 4;
	drawCircleCircum(xCen, yCen, x, y, r, g, b, a);
	while (x < y){
		x++;
		if (p < 0){
			p += 2 * x + 1;
		}
		else{
			y--;
			p += 2 * (x - y) + 1;
		}
		drawCircleCircum(xCen, yCen, x, y, r, g, b, a);
	}
}

void drawFillCircle(int xCen, int yCen, int radius, char r, char g, char b, char a)
{
	drawCircle(xCen, yCen, radius, r, g, b, a);
	int x, y;
	for (y = -radius; y <= radius; y++){
		for (x = -radius; x <= radius; x++)
			if (x*x + y*y <= radius*radius + radius * .8f)
				drawPixel(xCen + x, yCen + y, r, g, b, a);
	}
}

void drawCircleCircum(int cx, int cy, int x, int y, char r, char g, char b, char a)
{

	if (x == 0){
		drawPixel(cx, cy + y, r, g, b, a);
		drawPixel(cx, cy - y, r, g, b, a);
		drawPixel(cx + y, cy, r, g, b, a);
		drawPixel(cx - y, cy, r, g, b, a);
	}
	if (x == y){
		drawPixel(cx + x, cy + y, r, g, b, a);
		drawPixel(cx - x, cy + y, r, g, b, a);
		drawPixel(cx + x, cy - y, r, g, b, a);
		drawPixel(cx - x, cy - y, r, g, b, a);
	}
	if (x < y){
		drawPixel(cx + x, cy + y, r, g, b, a);
		drawPixel(cx - x, cy + y, r, g, b, a);
		drawPixel(cx + x, cy - y, r, g, b, a);
		drawPixel(cx - x, cy - y, r, g, b, a);
		drawPixel(cx + y, cy + x, r, g, b, a);
		drawPixel(cx - y, cy + x, r, g, b, a);
		drawPixel(cx + y, cy - x, r, g, b, a);
		drawPixel(cx - y, cy - x, r, g, b, a);
	}
}

extern const u8 msx_font[];
int multiplier = 2;

void drawCharacter(char c, int x, int y, int r, int g, int b, int a)
{
    u8 *font = (u8*)(msx_font + c * 8);
    int i, j;
    for (i = 7; i >= 0; --i)
    {
        for (j = 0; j < 8; ++j)
        {
            if ((*font & (128 >> j)))
            {
                drawFillRect(x+(j*multiplier), y+((8-i)*multiplier), x+(j*multiplier)+(multiplier-1), y+((8-i)*multiplier)+(multiplier-1), r, g, b, a);
            }
        }
        ++font;
    }
}

void centerStringfColor(int y, int r, int g, int b, int a, char *format, ...)
{
    char *buffer = malloc(0x500);
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, 0x500, format, args);
    va_end(args);

    centerStringColor(y,buffer,r,g,b,a);
    free(buffer);
}

void centerStringf(int y, char *format, ...)
{
    char *buffer = malloc(0x500);
    char *buffer_format = malloc(0x500);
    strcpy(buffer_format, format);
    va_list args;
    va_start(args, buffer_format);
    vsnprintf(buffer, 0x500, buffer_format, args);
    va_end(args);

    centerStringColor(y,buffer,255,255,255,0);
    free(buffer);
}

void centerString(int y, char *str)
{
    centerStringColor(y,str,255,255,255,0);
}

void centerStringColor(int y, char *str, int r, int g, int b, int a)
{
    drawStringColor(getScreenWidth()/2 - (strlen(str)*8*multiplier/2),y,str,r,g,b,a);
}

void drawString(int x, int y, char* str)
{
    drawStringColor(x,y,str,255,255,255,0);
}

void drawStringf(int x, int y, const char *format, ...)
{
    char *buffer = malloc(0x500);
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, 0x500, format, args);
    va_end(args);
    
	drawString(x,y,buffer);
	free(buffer);
}

void drawStringfColor(int x, int y, int r, int g, int b, int a, const char *format, ...)
{
    char *buffer = malloc(0x500);
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, 0x500, format, args);
    va_end(args);
    
	drawStringColor(x,y,buffer,r,g,b,a);
	free(buffer);
}

void drawStringColor(int x, int y, char* str, int r, int g, int b, int a)
{
    if(!str)return;
    int k;
    int dx=0, dy=0;
    for(k=0;k<strlen(str);k++)
    {
        if(str[k] == 0x80)
        {
            r = str[k+1];
            g = str[k+2];
            b = str[k+3];
            k += 3;
            continue;
        }
        
        if(str[k]>=32)
        {
            drawCharacter(str[k],x+(dx*multiplier),y+(dy*multiplier),r,g,b,a);
            dx+=8;
        }
        if(str[k]=='\n'){dx=0;dy+=8;}
    }
}

void drawTGA(int x_pos, int y_pos, void *tga_mem)
{
    tga_hdr *tga = (tga_hdr*)tga_mem;
    int height = ((tga->height & 0xFF) << 8) + ((tga->height & 0xFF00) >> 8);
    int width = ((tga->width & 0xFF) << 8) + ((tga->width & 0xFF00) >> 8);
    for(int x = 0; x < 128; x++)
    {
        for(int y = 0; y < 128; y++)
        {
            u8 *icon_pixel = (tga_mem+0x12)+((height-1-y)*width*4)+(x*4);
            drawPixel(x+x_pos,y+y_pos,icon_pixel[0],icon_pixel[1],icon_pixel[2],icon_pixel[3]);
        }
    }
}
