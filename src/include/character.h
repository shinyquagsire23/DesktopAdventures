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

#ifndef CHARACTER_H
#define CHARACTER_H

#include "useful.h"

typedef struct entity
{
    u16 char_id;
    u16 x;
    u16 y;
    u16 current_frame;
    u16 extend_frame;
    u8  extend_dir;
    u8  extend_offset;
    u16 item;
    u16 num_items;
    u16 health;
    bool attacking;
    bool is_active_visible;
} entity;

enum ICHR_FLAGS
{
    ICHR_FRIENDLY  = 0x1,
    ICHR_ENEMY     = 0x2,
    ICHR_IS_WEAPON = 0x4,

    ICHR_BEHAVIOR_HARD    = 0x10000,
    ICHR_BEHAVIOR_MEDIUM  = 0x20000,
    ICHR_BEHAVIOR_RANDOM  = 0x30000,
    ICHR_BEHAVIOR_STATIONARY = 0x40000,
    ICHR_BEHAVIOR_ANIMATED  = 0x80000,
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
    u16 health;
} chwp_entry;

typedef struct caux_entry
{
    u16 id_1;
    u16 damage;
} caux_entry;

enum CHAR_FRAME
{
    FRAME_DOWN = 1,
    FRAME_UP = 2,
    FRAME_UP_LEFT = 2,
    FRAME_LEFT,
    FRAME_DOWN_LEFT,
    FRAME_UP_RIGHT,
    FRAME_RIGHT,
    FRAME_DOWN_RIGHT,

    //Sprites for hero, walking
    FRAME_WALK_UP_1 = 8,
    FRAME_WALK_DOWN_1,
    FRAME_WALK_UP_LEFT_1,
    FRAME_WALK_LEFT_1,
    FRAME_WALK_DOWN_LEFT_1,
    FRAME_WALK_UP_RIGHT_1,
    FRAME_WALK_RIGHT_1,
    FRAME_WALK_DOWN_RIGHT_1,
    FRAME_WALK_UP_2,
    FRAME_WALK_DOWN_2,
    FRAME_WALK_UP_LEFT_2,
    FRAME_WALK_LEFT_2,
    FRAME_WALK_DOWN_LEFT_2,
    FRAME_WALK_UP_RIGHT_2,
    FRAME_WALK_RIGHT_2,
    FRAME_WALK_DOWN_RIGHT_2,

    //Sprites for weapon, attacking.
    //Extend is used for whips (Indianna Jones) and Sabers (Luke)
    FRAME_WEAPON_PROJECTILE_UP = 0,
    FRAME_WEAPON_PROJECTILE_DOWN,
    FRAME_WEAPON_PROJECTILE_UP_LEFT,
    FRAME_WEAPON_PROJECTILE_LEFT,
    FRAME_WEAPON_PROJECTILE_DOWN_LEFT,
    FRAME_WEAPON_PROJECTILE_UP_RIGHT,
    FRAME_WEAPON_PROJECTILE_RIGHT,

    FRAME_WEAPON_ICON = 7,
    FRAME_ATTACK_UP_1,
    FRAME_ATTACK_DOWN_1,
    FRAME_ATTACK_EXTEND_UP_2,
    FRAME_ATTACK_LEFT_1,
    FRAME_ATTACK_EXTEND_DOWN_1,
    FRAME_ATTACK_EXTEND_LEFT_1,
    FRAME_ATTACK_RIGHT_1,
    FRAME_ATTACK_EXTEND_RIGHT_1,
    FRAME_ATTACK_UP_2,
    FRAME_ATTACK_DOWN_2,
    FRAME_ATTACK_EXTEND_UP_1,
    FRAME_ATTACK_LEFT_2,
    FRAME_ATTACK_EXTEND_DOWN_2,
    FRAME_ATTACK_EXTEND_LEFT_2,
    FRAME_ATTACK_RIGHT_2,
    FRAME_ATTACK_EXTEND_RIGHT_2,
} CHAR_FRAME;

const u8 CHAR_WALK_UP_ANIM[4];
const u8 CHAR_WALK_DOWN_ANIM[4];
const u8 CHAR_WALK_LEFT_ANIM[4];
const u8 CHAR_WALK_RIGHT_ANIM[4];
const u8 CHAR_WALK_UP_LEFT_ANIM[4];
const u8 CHAR_WALK_DOWN_LEFT_ANIM[4];
const u8 CHAR_WALK_UP_RIGHT_ANIM[4];
const u8 CHAR_WALK_DOWN_RIGHT_ANIM[4];

const u8 CHAR_ATTACK_UP_ANIM[4];
const u8 CHAR_ATTACK_DOWN_ANIM[4];
const u8 CHAR_ATTACK_LEFT_ANIM[4];
const u8 CHAR_ATTACK_RIGHT_ANIM[4];
const u8 CHAR_ATTACK_EXTEND_UP_ANIM[4];
const u8 CHAR_ATTACK_EXTEND_DOWN_ANIM[4];
const u8 CHAR_ATTACK_EXTEND_LEFT_ANIM[4];
const u8 CHAR_ATTACK_EXTEND_RIGHT_ANIM[4];

enum CHAR_DIRECTION
{
    UP,
    DOWN,
    UP_LEFT,
    LEFT,
    DOWN_LEFT,
    UP_RIGHT,
    RIGHT,
    DOWN_RIGHT,
} CHAR_DIRECTION;

ichr_data **char_data;
chwp_entry **chwp_data;
caux_entry **caux_data;

#endif