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

#ifndef TILE_H
#define TILE_H

#include "useful.h"

#define TILE_NONE                           (0xFFFF)

#define TILE_GAME_OBJECT 					BIT(0)
#define TILE_UNDER_PLAYER_NONCOLLIDING 		BIT(1)
#define TILE_MIDDLE_LAYER_COLLIDING 		BIT(2)
#define TILE_PUSH_PULL_BLOCK 				BIT(3)
#define TILE_ABOVE_PLAYER_NONCOLLIDING		BIT(4)

#define TILE_MINI_MAP_TILE					BIT(5)
#define TILE_WEAPON							BIT(6)
#define TILE_ITEM							BIT(7)
#define TILE_CHARACTER						BIT(8)

//WEAPON
#define TILE_LIGHT_BLASTER					BIT(16)
#define TILE_HEAVY_BLASTER					BIT(17)
#define TILE_LIGHTSABER						BIT(18)
#define TILE_THE_FORCE						BIT(19)

//ITEMS
#define TILE_KEYCARD						BIT(16)
#define TILE_PUZZLE_ITEM_1					BIT(17)
#define TILE_PUZZLE_ITEM_2					BIT(18)
#define TILE_PUZZLE_ITEM_SEED_END			BIT(19)
#define TILE_LOCATOR						BIT(20)
#define TILE_HEALTH_PACK					BIT(22)

//CHARACTER
#define TILE_PLAYER							BIT(16)
#define TILE_ENEMY							BIT(17)
#define TILE_FRIENDLY						BIT(18)

//OTHER
#define TILE_IS_DOOR						BIT(16)

//MINI-MAP
#define TILE_HOME							BIT(17)
#define TILE_PUZZLE_UNSOLVED				BIT(18)
#define TILE_PUZZLE_SOLVED					BIT(19)
#define TILE_GATEWAY_UNSOLVED				BIT(20)
#define TILE_GATEWAY_SOLVED					BIT(21)
#define TILE_WALL_UP_LOCKED					BIT(22)
#define TILE_WALL_DOWN_LOCKED				BIT(23)
#define TILE_WALL_LEFT_LOCKED				BIT(24)
#define TILE_WALL_RIGHT_LOCKED				BIT(25)
#define TILE_WALL_UP_UNLOCKED				BIT(26)
#define TILE_WALL_DOWN_UNLOCKED				BIT(27)
#define TILE_WALL_LEFT_UNLOCKED				BIT(28)
#define TILE_WALL_RIGHT_UNLOCKED			BIT(29)
#define TILE_OBJECTIVE						BIT(30)

//TNAME **tile_names;
u32 tile_metadata[0x2000];

#endif
