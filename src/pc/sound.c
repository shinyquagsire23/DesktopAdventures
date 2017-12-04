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

#include "sound.h"

#ifndef __EMSCRIPTEN__
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

const int NUM_CONCURRENT_SNDS = 0x100;

static Mix_Chunk **wave = NULL;

void sound_init()
{
    wave = malloc(NUM_CONCURRENT_SNDS*sizeof(Mix_Chunk*));
    for(int i = 0; i < NUM_CONCURRENT_SNDS; i++)
        wave[i] = NULL;

    if (Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1)
    {
        printf("Error opening audio: %s\n", SDL_GetError());
        return;
    }
}

void sound_play(u16 id)
{
    char *path = malloc(0x100);
    strcpy(path, "sfx/");
    strcat(path, sound_files[id]);

    if(wave[id] == NULL)
    {
        wave[id] = Mix_LoadWAV(path);
        if (wave[id] == NULL)
        {
            printf("LoadWAV failed to load %s\n", path);
            free(path);
            return;
        }
    }
    free(path);

    Mix_PlayChannel(-1, wave[id], 0);
}

void sound_exit()
{
    for(int i = 0; i < NUM_CONCURRENT_SNDS; i++)
    {
        if(wave[i] != NULL)
            Mix_FreeChunk(wave[i]);
    }

    free(wave);
    Mix_CloseAudio();
}

#else
    void sound_init(){}
    void sound_play(u16 id){}
    void sound_exit(){}
#endif