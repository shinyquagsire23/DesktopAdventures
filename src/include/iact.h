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

#ifndef DESKTOPADVENTURES_IACT_H
#define DESKTOPADVENTURES_IACT_H

#include "useful.h"

enum IACT_CMDS
{
    IACT_CMD_SetMapTile,
    IACT_CMD_ClearTile,
    IACT_CMD_MoveMapTile,
    IACT_CMD_DrawOverlayTile,
    IACT_CMD_SayText,
    IACT_CMD_ShowText,
    IACT_CMD_RedrawTile,
    IACT_CMD_RedrawTiles,
    IACT_CMD_RenderChanges,
    IACT_CMD_WaitTicks,
    IACT_CMD_PlaySound,
    IACT_CMD_TransitionIn,
    IACT_CMD_Random,
    IACT_CMD_SetTempVar,
    IACT_CMD_AddTempVar,
    IACT_CMD_SetMapTileVar,
    IACT_CMD_ReleaseCamera,
    IACT_CMD_LockCamera,
    IACT_CMD_SetPlayerPos,
    IACT_CMD_MoveCamera,
    IACT_CMD_FlagOnce,
    IACT_CMD_ShowObject,
    IACT_CMD_HideObject,
    IACT_CMD_EnemySpawn,
    IACT_CMD_EnemyDespawn,
    IACT_CMD_SpawnAllEnemies,
    IACT_CMD_DespawnAllEnemies,
    IACT_CMD_SpawnItem,
    IACT_CMD_AddItemToInv,
    IACT_CMD_DropItem,
    IACT_CMD_OpenShow_MAYBE,
    IACT_CMD_Unk1f,
    IACT_CMD_Unk20,
    IACT_CMD_WarpToMap,
    IACT_CMD_SetGlobalVar,
    IACT_CMD_AddGlobalVar,
    IACT_CMD_SetRandVar,
    IACT_CMD_AddHealth,
} IACT_CMD;

enum IACT_TRIGGERS
{
    IACT_TRIG_FirstEnter,
    IACT_TRIG_Enter,
    IACT_TRIG_BumpTile,
    IACT_TRIG_DragItem,
    IACT_TRIG_Walk,
    IACT_TRIG_TempVarEq,
    IACT_TRIG_RandVarEq,
    IACT_TRIG_RandVarGt,
    IACT_TRIG_RandVarLs,
    IACT_TRIG_EnterVehicle,
    IACT_TRIG_CheckMapTile,
    IACT_TRIG_EnemyDead,
    IACT_TRIG_AllEnemiesDead,
    IACT_TRIG_HasItem,
    IACT_TRIG_HasEndItem,
    IACT_TRIG_Unk0f,
    IACT_TRIG_Unk10,
    IACT_TRIG_GameInProgress_MAYBE,
    IACT_TRIG_GameCompleted_MAYBE,
    IACT_TRIG_HealthLs,
    IACT_TRIG_HealthGt,
    IACT_TRIG_Unk15,
    IACT_TRIG_Unk16,
    IACT_TRIG_DragWrongItem,
    IACT_TRIG_PlayerAtPos,
    IACT_TRIG_GlobalVarEq,
    IACT_TRIG_GlobalVarLs,
    IACT_TRIG_GlobalVarGt,
    IACT_TRIG_ExperienceEq,
    IACT_TRIG_Unk1d,
    IACT_TRIG_Unk1e,
    IACT_TRIG_TempVarNe,
    IACT_TRIG_RandVarNe,
    IACT_TRIG_GlobalVarNe,
    IACT_TRIG_CheckMapTileVar,
    IACT_TRIG_ExperienceGt,
} IACT_TRIGGER;

void read_iact();
void read_iact_stats(u16 map_num, u32 location, u16 num_iacts);
void print_iact_stats();

void iact_set_trigger(u8 trigger, u8 count, ...);
void iact_update();

u16 IACT_RANDVAR;
u16 IACT_TEMPVAR;

#endif //DESKTOPADVENTURES_IACT_H
