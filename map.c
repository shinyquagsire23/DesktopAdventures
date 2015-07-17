#include "map.h"

#include "screen.h"
#include "assets.h"

u16 *map_tiles_low;
u16 *map_tiles_middle;
u16 *map_tiles_high;
u32 camera_x = 0;
u32 camera_y = 0;

u32 unknown;
u16 width;
u16 height;
u8 flags;
u8 planet;
u8 same;

char triggers[0x24][30] = { "FirstEnter", "Enter", "BumpTile", "DragItem", "Walk", "TempVarEq", "RandVarEq", "RandVarGt", "RandVarLs", "EnterVehicle", "CheckMapTile", "EnemyDead", "AllEnemiesDead", "HasItem", "Unk0e", "Unk0f", "Unk10", "GameInProgress?", "GameCompleted?", "HealthLs", "HealthGt", "Unk15", "Unk16", "DragWrongItem", "PlayerAtPos", "GlobalVarEq", "GlobalVarLs", "GlobalVarGt", "ExperienceEq", "Unk1d", "Unk1e", "TempVarNe", "RandVarNe", "GlobalVarNe", "CheckMapTileVar", "ExperienceGt"};
char commands[0x26][30] = { "SetMapTile", "ClearTile", "MoveMapTile", "DrawOverlayTile", "SayText", "ShowText", "RedrawTile", "RedrawTiles", "RenderChanges", "WaitSecs", "PlaySound", "Unk0b", "Random", "SetTempVar", "AddTempVar", "SetMapTileVar", "ReleaseCamera", "LockCamera", "SetPlayerPos", "MoveCamera", "Redraw", "OpenDoor?", "CloseDoor?", "EnemySpawn", "NPCSpawn", "RemoveDraggedItem", "RemoveDraggedItemSimilar?", "SpawnItem", "AddItemToInv", "DropItem", "Open?Show?", "Unk1f", "Unk20", "WarpToMap", "SetGlobalVar", "AddGlobalVar", "SetRandVar", "AddHealth"};

void load_map(u16 map_id)
{
	init_screen();
	u32 location = zone_data[map_id]->izon_offset;
	location += 4; //IZON

	camera_x = 0;
	camera_y = 0;

	seek(location);
	unknown = read_long();
	width = read_short();
	height = read_short();
	flags = read_byte();

	seek_add(5);

	planet = read_byte();
	same = read_byte();

	map_tiles_low = malloc(width*height*2);
	map_tiles_middle = malloc(width*height*2);
	map_tiles_high = malloc(width*height*2);
	for(int i = 0; i < width*height; i++)
	{
		map_tiles_low[i] = read_short();
		map_tiles_middle[i] = read_short();
		map_tiles_high[i] = read_short();
	}

	printf("Loaded map %i, map type %x, width %i, height %i\n", map_id, planet, width, height);
#ifndef _3DS
	read_iact(zone_data[map_id]->iact_offset, zone_data[map_id]->num_iacts); //Prints out a bunch of stuff... This kills the 3DS.
#endif
}

void read_iact(u32 location, u16 num_iacts)
{
	printf("Reading IACT data, %u IACTs\n", num_iacts);
	seek(location);
	for(int i = 0; i < num_iacts; i++)
	{
		read_long(); //IACT
		u32 ignored6 = read_long();
		u16 iactItemCount1 = read_short();
		printf("\n    Action: unknown %08x, actions %d\n", ignored6, iactItemCount1);
		for (u16 k = 0; k < iactItemCount1; k++)
		{
			char pos_str[7];
			u16 command = read_short();
			u16 arg1 = read_short();
			u16 arg2 = read_short();
			u16 arg3 = read_short();
			u16 arg4 = read_short();
			u16 arg5 = read_short();
			u16 arg6 = read_short();

			*(u16*)(pos_str) = arg4;
			*(u16*)(pos_str+2) = arg5;
			*(u16*)(pos_str+4) = arg6;

			printf("        %s, %04x, %04x, %04x, %04x, %04x, %04x, %s\n", triggers[command], arg1, arg2, arg3, arg4, arg5, arg6, pos_str);
		}
		u16 iactItemCount2 = read_short();
		printf("    Script: commands %d\n", iactItemCount2);
		for(u16 k = 0; k < iactItemCount2; k++)
		{
			char pos_str[7];
			u16 command = read_short();
			u16 arg1 = read_short();
			u16 arg2 = read_short();
			u16 arg3 = read_short();
			u16 arg4 = read_short();
			u16 arg5 = read_short();
			u16 strlen = read_short();

			*(u16*)(pos_str) = arg3;
			*(u16*)(pos_str+2) = arg4;
			*(u16*)(pos_str+4) = arg5;

			printf("        %s, %04x, %04x, %04x, %04x, %04x, %04x, %s\n", commands[command], arg1, arg2, arg3, arg4, arg5, strlen, pos_str);
			if (strlen)
			{
				char* str = malloc(strlen+1);
				for (u16 l = 0; l < strlen; l++)
				{
					str[l] = read_byte();
				}
				printf("            \"%s\"\n", str);
			}
		}
	}
}

void render_map()
{
	for(int i = 0; i < 9; i++)
	{
		for(int j = 0; j < 9; j++)
		{
			tiles_low[(j*9)+i] = map_tiles_low[((camera_y+j)*width)+i+camera_x];
			tiles_middle[(j*9)+i] = map_tiles_middle[((camera_y+j)*width)+i+camera_x];
			tiles_high[(j*9)+i] = map_tiles_high[((camera_y+j)*width)+i+camera_x];
		}
	}
}
