/*
 * main.h
 *
 *  Created on: Jan 18, 2015
 *      Author: maxamillion
 */
#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "SDL.h"

#ifndef MAIN_H_
#define MAIN_H_

void handleKeyDown();
void Quit(int returnCode);
void handleKeyPress( SDL_keysym *keysym );

#endif /* MAIN_H_ */
