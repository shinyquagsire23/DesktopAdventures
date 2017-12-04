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

#ifdef RENDER_GL

#ifdef PC_BUILD
    #include <SDL_opengl.h>
#else
    #include <GL/gl.h>
#endif
#include "assets.h"
#include "screen.h"

void render(int x, int y)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();

    glTranslatef((float)x, (float)y, 0.0f);

    if (ASSETS_LOADING)
    {
        glBindTexture(GL_TEXTURE_2D, texture[0x2000]);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
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

#ifdef _3DS
        glTranslatef(0, -16.0f, 0.0f);
#endif

        glDisable(GL_TEXTURE_2D);
        glColor4f(0.0f, 0.0f, 0.5f, 1.0f);
        glBegin(GL_QUADS);
        {
            glTexCoord2f(0.0f, 1.0f);
            glVertex2f(8, 264);

            glTexCoord2f(-1.0f, 1.0f);
            glVertex2f(280*ASSETS_PERCENT, 264);

            glTexCoord2f(-1.0f, 0.0f);
            glVertex2f(280*ASSETS_PERCENT, 280);

            glTexCoord2f(0.0f, 0.0f);
            glVertex2f(8, 280);
        }
        glEnd();
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        for (int y = SCREEN_FADE_LEVEL; y < SCREEN_TILE_HEIGHT-SCREEN_FADE_LEVEL; y++) {
            for (int x = SCREEN_FADE_LEVEL; x < SCREEN_TILE_WIDTH-SCREEN_FADE_LEVEL; x++) {
                if (tiles_low[(y * SCREEN_TILE_WIDTH) + x] != 0xFFFF) {
                    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                    glBindTexture(GL_TEXTURE_2D, texture[tiles_low[(y * SCREEN_TILE_WIDTH) + x]]);
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

                if (tiles_middle[(y * SCREEN_TILE_WIDTH) + x] != 0xFFFF) {
                    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                    glBindTexture(GL_TEXTURE_2D, texture[tiles_middle[(y * SCREEN_TILE_WIDTH) + x]]);
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

                if (tiles_high[(y * SCREEN_TILE_WIDTH) + x] != 0xFFFF) {
                    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                    glBindTexture(GL_TEXTURE_2D, texture[tiles_high[(y * SCREEN_TILE_WIDTH) + x]]);
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

                if (tiles_overlay[(y * SCREEN_TILE_WIDTH) + x] != 0xFFFF) {
                    glColor4f(1.0f, 1.0f, 1.0f, 0.6f);
                    glBindTexture(GL_TEXTURE_2D, texture[tiles_overlay[(y * SCREEN_TILE_WIDTH) + x]]);
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
    }
}

#endif