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

#include "screen.h"

#include <GL/gl.h>

#include "useful.h"
#include "assets.h"

unsigned short tiles_low[0x100 * 0x100];
unsigned short tiles_middle[0x100 * 0x100];
unsigned short tiles_high[0x100 * 0x100];
unsigned short tiles_overlay[0x100 * 0x100];

void init_screen()
{
	for(int i = 0; i < 0x100 * 0x100; i++)
	{
		tiles_low[i] = 0xFFFF;
		tiles_middle[i] = 0xFFFF;
		tiles_high[i] = 0xFFFF;
	}
}

int draw_screen()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();

#ifdef _3DS
	glTranslatef((400 - SCREEN_WIDTH) / 2, (240 - SCREEN_WIDTH) / 2, 0.0f); //Center screen
#endif

	for(int y = 0; y < SCREEN_TILE_WIDTH; y++)
	{
		for (int x = 0; x < SCREEN_TILE_WIDTH; x++)
		{
			if (tiles_low[(y * SCREEN_TILE_WIDTH) + x] != 0xFFFF)
			{
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
				glBindTexture(GL_TEXTURE_2D, texture[tiles_low[(y * 9) + x]]);
				glBegin(GL_QUADS);
				{
					glTexCoord2f(1.0f, 1.0f);
					glVertex2f(32 * x, 32 * y);

					glTexCoord2f(0.0f, 1.0f);
					glVertex2f((32 * x) + 32, 32 * y);

					glTexCoord2f(0.0f, 0.0f);
					glVertex2f((32 * x) + 32, (32 * y) + 32);

					glTexCoord2f(1.0f, 0.0f);
					glVertex2f(32 * x, (32 * y) + 32);
				}
				glEnd();
			}

			if (tiles_middle[(y * SCREEN_TILE_WIDTH) + x] != 0xFFFF)
			{
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
				glBindTexture(GL_TEXTURE_2D, texture[tiles_middle[(y * 9) + x]]);
				glBegin(GL_QUADS);
				{
					glTexCoord2f(1.0f, 1.0f);
					glVertex2f(32 * x, 32 * y);

					glTexCoord2f(0.0f, 1.0f);
					glVertex2f((32 * x) + 32, 32 * y);

					glTexCoord2f(0.0f, 0.0f);
					glVertex2f((32 * x) + 32, (32 * y) + 32);

					glTexCoord2f(1.0f, 0.0f);
					glVertex2f(32 * x, (32 * y) + 32);
				}
				glEnd();
			}

			if (tiles_high[(y * SCREEN_TILE_WIDTH) + x] != 0xFFFF)
			{
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
				glBindTexture(GL_TEXTURE_2D, texture[tiles_high[(y * 9) + x]]);
				glBegin(GL_QUADS);
				{
					glTexCoord2f(1.0f, 1.0f);
					glVertex2f(32 * x, 32 * y);

					glTexCoord2f(0.0f, 1.0f);
					glVertex2f((32 * x) + 32, 32 * y);

					glTexCoord2f(0.0f, 0.0f);
					glVertex2f((32 * x) + 32, (32 * y) + 32);

					glTexCoord2f(1.0f, 0.0f);
					glVertex2f(32 * x, (32 * y) + 32);
				}
				glEnd();
			}

			if (tiles_overlay[(y * SCREEN_TILE_WIDTH) + x] != 0xFFFF)
			{
				glColor4f(1.0f, 1.0f, 1.0f, 0.6f);
				glBindTexture(GL_TEXTURE_2D, texture[tiles_overlay[(y * 9) + x]]);
				float scale = 1.0f;
				glBegin(GL_QUADS);
				{
					glTexCoord2f(1.0f, 1.0f);
					glVertex2f((32 * x), (32 * y));

					glTexCoord2f(0.0f, 1.0f);
					glVertex2f((32 * x) + (32 * scale), (32 * y));

					glTexCoord2f(0.0f, 0.0f);
					glVertex2f((32 * x) + (32 * scale), (32 * y) + (32 * scale));

					glTexCoord2f(1.0f, 0.0f);
					glVertex2f((32 * x), (32 * y) + (32 * scale));
				}
				glEnd();
			}
		}
	}

	if (ASSETS_LOADING)
	{
		glBindTexture(GL_TEXTURE_2D, texture[0x2000]);
		glBegin(GL_QUADS);
		{
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(0, 0);

			glTexCoord2f(-1.0f, 1.0f);
			glVertex2f(288, 0);

			glTexCoord2f(-1.0f, 0.0f);
			glVertex2f(288, 288);

			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(0, 288);
		}
		glEnd();
	}

#ifdef PC_BUILD
	SDL_GL_SwapBuffers();
#elif _3DS
#endif
	return 1;
}
