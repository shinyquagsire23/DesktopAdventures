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

#include "useful.h"

typedef struct entity
{
    u16 char_id;
    u16 x;
    u16 y;
    u16 current_frame;
    u16 item;
    u16 num_items;
} entity;

enum ICHR_FLAGS
{
    ICHR_FRIENDLY  = 0x1,
    ICHR_ENEMY     = 0x2,
    ICHR_IS_WEAPON = 0x4,

    ICHR_BEHAVIOR_HARD    = 0x10000,
    ICHR_BEHAVIOR_MEDIUM  = 0x20000,
    ICHR_BEHAVIOR_RANDOM  = 0x30000,
    ICHR_BEHAVIOR_SITTING = 0x40000,
    ICHR_BEHAVIOR_PATROL  = 0x80000,
};

typedef struct ichr_data
{
    u32 magic;
    u32 unk_1;
    u8 name[0x10];
    u32 flags;
    u16 unk_4;
    u32 unk_5;
    u16 frames[26];
} ichr_data;

typedef struct chwp_entry
{
    u16 id_1;
    u16 id_2;
    u16 unk;
} chwp_entry;

enum CHAR_FRAME
{
    BLNK1 = 0,
    BLNK2,

    UP,
    DOWN,
    UP_LEFT,
    LEFT,
    DOWN_LEFT,
    UP_RIGHT,
    RIGHT,

    //Sprites for hero, walking
    DOWN_RIGHT = 9,
    WALK_UP_1,
    WALK_DOWN_1,
    WALK_UP_LEFT_1,
    WALK_LEFT_1,
    WALK_DOWN_LEFT_1,
    WALK_UP_RIGHT_1,
    WALK_RIGHT_1,
    WALK_DOWN_RIGHT_1,
    WALK_UP_2,
    WALK_DOWN_2,
    WALK_UP_LEFT_2,
    WALK_LEFT_2,
    WALP_DOWN_LEFT_2,
    WALK_UP_RIGHT_2,
    WALK_RIGHT_2,
    WALK_DOWN_RIGHT_2,

    //Sprites for weapon, attacking.
    //Extend is used for whips (Indianna Jones) and Sabers (Luke)
    WEAPON_ICON = 9,
    ATTACK_UP_1,
    ATTACK_DOWN_1,
    ATTACK_EXTEND_UP_2,
    ATTACK_LEFT_1,
    ATTACK_EXTEND_DOWN_1,
    ATTACK_EXTEND_LEFT_1,
    ATTACK_RIGHT_1,
    ATTACK_EXTEND_RIGHT_1,
    ATTACK_UP_2,
    ATTACK_DOWN_2,
    ATTACK_EXTEND_UP_1,
    ATTACK_LEFT_2,
    ATTACK_EXTEND_DOWN_2,
    ATTACK_EXTEND_LEFT_2,
    ATTACK_RIGHT_2,
    ATTACK_EXTEND_RIGHT_2,
};

ichr_data **char_data;
chwp_entry **chwp_data;
