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

typedef struct ipuz_element
{
	u32 size;
	u32 unk1;
	u32 unk2;
	u32 unk3;
	u16 unk4;
	u16 string1_len;
	char string1[0x800];
	u16 string2_len;
	char string2[0x800];
	u16 string3_len;
	char string3[0x800];
	u16 string4_len;
	char string4[0x800];
	u16 unused_len;
	char unused[0x800];
	u32 item_a;
	u32 item_b;
} ipuz_element;

ipuz_element **ipuz_data;
