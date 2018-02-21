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
#ifndef DRAW_H
#define DRAW_H
#include <switch.h>

typedef struct tga_hdr tga_hdr;
struct __attribute__((__packed__)) tga_hdr
{
    u8 idlength;
    u8 colormaptype;
    u8 datatype;
    u16 colormaporigin;
    u16 colormaplength;
    u8 colormapdepth;
    u16 x_origin;
    u16 y_origin;
    u16 width;
    u16 height;
    u8 bpp;
    u8 imagedescriptor;
};

void *screenBufferTop;
void *screenBufferBottom;

#define SCREEN_TOP 0
#define SCREEN_BOTTOM 1

//Function declarations for my graphics library
void flipBuffers();
void fillScreen(char r, char g, char b, char a);
void drawString(int x, int y, char * string);
void drawPixel(int x, int y, char r, char g, char b, char a);
void drawLine(int x1, int y1, int x2, int y2, char r, char g, char b, char a);
void drawBorder(int thickness, char r, char g, char b, char a);
void drawRect(int x1, int y1, int x2, int y2, char r, char g, char b, char a);
void drawRectThickness(int x1, int y1, int x2, int y2, int thickness, char r, char g, char b, char a);
void drawFillRect(int x1, int y1, int x2, int y2, char r, char g, char b, char a);
void drawCircle(int xCen, int yCen, int radius, char r, char g, char b, char a);
void drawFillCircle(int xCen, int yCen, int radius, char r, char g, char b, char a);
void drawCircleCircum(int cx, int cy, int x, int y, char r, char g, char b, char a);
void drawTGA(int x, int y, void *tga_mem);
#endif /* DRAW_H */
