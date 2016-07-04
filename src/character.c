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

#include "character.h"

const u8 CHAR_WALK_UP_ANIM[4] = {FRAME_WALK_UP_1, FRAME_UP, FRAME_WALK_UP_2, FRAME_UP};
const u8 CHAR_WALK_DOWN_ANIM[4] = {FRAME_WALK_DOWN_1, FRAME_DOWN, FRAME_WALK_DOWN_2, FRAME_DOWN};
const u8 CHAR_WALK_LEFT_ANIM[4] = {FRAME_WALK_LEFT_1, FRAME_LEFT, FRAME_WALK_LEFT_2, FRAME_LEFT};
const u8 CHAR_WALK_RIGHT_ANIM[4] = {FRAME_WALK_RIGHT_1, FRAME_RIGHT, FRAME_WALK_RIGHT_2, FRAME_RIGHT};
const u8 CHAR_WALK_UP_LEFT_ANIM[4] = {FRAME_WALK_UP_LEFT_1, FRAME_UP_LEFT, FRAME_WALK_UP_LEFT_2, FRAME_UP_LEFT};
const u8 CHAR_WALK_DOWN_LEFT_ANIM[4] = {FRAME_WALK_DOWN_LEFT_1, FRAME_DOWN_LEFT, FRAME_WALK_DOWN_LEFT_2, FRAME_DOWN_LEFT};
const u8 CHAR_WALK_UP_RIGHT_ANIM[4] = {FRAME_WALK_UP_RIGHT_1, FRAME_UP_RIGHT, FRAME_WALK_UP_RIGHT_2, FRAME_UP_RIGHT};
const u8 CHAR_WALK_DOWN_RIGHT_ANIM[4] = {FRAME_WALK_DOWN_RIGHT_1, FRAME_DOWN_RIGHT, FRAME_WALK_DOWN_RIGHT_2, FRAME_DOWN_RIGHT};
ichr_data **char_data;
