#include "screen.h"

#include <GL/gl.h>

#include "useful.h"
#include "assets.h"

unsigned short tiles_low[0x100 * 0x100];
unsigned short tiles_middle[0x100 * 0x100];
unsigned short tiles_high[0x100 * 0x100];

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

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

#ifdef _3DS
	glTranslatef((400 - SCREEN_WIDTH) / 2, (240 - SCREEN_WIDTH) / 2, 0.0f); //Center screen
#endif

	for(int y = 0; y < SCREEN_TILE_WIDTH; y++)
	{
		for (int x = 0; x < SCREEN_TILE_WIDTH; x++)
		{
			if (tiles_low[(y * SCREEN_TILE_WIDTH) + x] != 0xFFFF)
			{
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
		}
	}

	if (ASSETS_LOADING)
	{
		glBindTexture(GL_TEXTURE_2D, texture[0x1000]);
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
