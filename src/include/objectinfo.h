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

#ifndef OBJECTINFO_H
#define OBJECTINFO_H

enum OBJ_TYPE
{
    OBJ_QUEST_ITEM_SPOT,
    OBJ_SPAWN,
    OBJ_THE_FORCE,
    OBJ_VEHICLE_TO,
    OBJ_VEHICLE_FROM,
    OBJ_LOCATOR,
    OBJ_ITEM,
    OBJ_PUZZLE_NPC,
    OBJ_WEAPON,
    OBJ_DOOR_IN,
    OBJ_DOOR_OUT,
    OBJ_UNKNOWN,
    OBJ_LOCK,
    OBJ_TELEPORTER,
    OBJ_XWING_FROM,
    OBJ_XWING_TO,
};

typedef struct obj_info
{
    u32 type;
    u16 x;
    u16 y;
    u16 unk1;
    u16 arg;
} obj_info;

#endif
