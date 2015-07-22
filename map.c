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

#include "map.h"

#include "screen.h"
#include "assets.h"
#include "character.h"

u16 *map_tiles_low;
u16 *map_tiles_middle;
u16 *map_tiles_high;
u16 *map_overlay;
u32 camera_x = 0;
u32 camera_y = 0;

entity player_entity;
entity *entities[512];
u16 num_entities = 0;

u32 unknown;
u16 width;
u16 height;
u8 flags;
u8 planet;
u8 same;
u16 id;

char triggers[0x24][30] = { "FirstEnter", "Enter", "BumpTile", "DragItem", "Walk", "TempVarEq", "RandVarEq", "RandVarGt", "RandVarLs", "EnterVehicle", "CheckMapTile", "EnemyDead", "AllEnemiesDead", "HasItem", "Unk0e", "Unk0f", "Unk10", "GameInProgress?", "GameCompleted?", "HealthLs", "HealthGt", "Unk15", "Unk16", "DragWrongItem", "PlayerAtPos", "GlobalVarEq", "GlobalVarLs", "GlobalVarGt", "ExperienceEq", "Unk1d", "Unk1e", "TempVarNe", "RandVarNe", "GlobalVarNe", "CheckMapTileVar", "ExperienceGt"};
char commands[0x26][30] = { "SetMapTile", "ClearTile", "MoveMapTile", "DrawOverlayTile", "SayText", "ShowText", "RedrawTile", "RedrawTiles", "RenderChanges", "WaitSecs", "PlaySound", "Unk0b", "Random", "SetTempVar", "AddTempVar", "SetMapTileVar", "ReleaseCamera", "LockCamera", "SetPlayerPos", "MoveCamera", "Redraw", "OpenDoor?", "CloseDoor?", "EnemySpawn", "NPCSpawn", "RemoveDraggedItem", "RemoveDraggedItemSimilar?", "SpawnItem", "AddItemToInv", "DropItem", "Open?Show?", "Unk1f", "Unk20", "WarpToMap", "SetGlobalVar", "AddGlobalVar", "SetRandVar", "AddHealth"};

//TODO: Try to make this a struct or something, less allocating of data that's already in our RAM buffer of the .DAT
void load_map(u16 map_id)
{
	id = map_id;

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
	map_overlay = malloc(width*height*2);
	for(int i = 0; i < width*height; i++)
	{
		map_tiles_low[i] = read_short();
		map_tiles_middle[i] = read_short();
		map_tiles_high[i] = read_short();
		map_overlay[i] = 0xFFFF;
	}

	printf("Loaded map %i, map type %x, width %i, height %i\n", map_id, planet, width, height);
	load_izax(zone_data[map_id]->izax_offset);
#ifndef _3DS
	//read_iact(zone_data[map_id]->iact_offset, zone_data[map_id]->num_iacts); //Prints out a bunch of stuff... This kills the 3DS.
#endif
}

void unload_map()
{
	free(map_tiles_low);
	free(map_tiles_middle);
	free(map_tiles_high);

	for(int i = 0; i < num_entities; i++)
		free(entities[i]);

	num_entities = 0;
}

void add_existing_entity(entity e)
{
	entities[num_entities++] = &e;
}

void add_new_entity(u16 id, u16 x, u16 y, u16 frame, u16 item, u16 num_items)
{
	entity *e = malloc(sizeof(entity));

	e->char_id = id;
	e->x = x;
	e->y = y;
	e->current_frame = frame;
	e->item = item;
	e->num_items = num_items;

	entities[num_entities++] = e;
}

void load_izax(u32 location)
{
	seek(location);
	izax_data_1 *first_section = (izax_data_1*)(current_file_pointer());
	seek_add(0xC + (first_section->num_entries * sizeof(izax_entry)));
	izax_data_2 *second_section = (izax_data_2*)(current_file_pointer());
	seek_add(0x2 + (second_section->num_entries * sizeof(izax_entry_2)));
	izax_data_3 *third_section = (izax_data_3*)(current_file_pointer());
	printf("Reading IZAX data, %u entries in first section, %u in the second and %u in the third\n", first_section->num_entries, second_section->num_entries, third_section->num_entries);

	for(int i = 0; i < first_section->num_entries; i++)
	{
		printf("  entity: id=%x, x=%x, y=%x, item=%x, qty=%x, %x\n", first_section->entries[i].entity_id, first_section->entries[i].x, first_section->entries[i].y, first_section->entries[i].item, first_section->entries[i].num_items, first_section->entries[i].unk3);
		add_new_entity(first_section->entries[i].entity_id, first_section->entries[i].x, first_section->entries[i].y, DOWN, first_section->entries[i].item, first_section->entries[i].num_items);
	}

	for(int i = 0; i < second_section->num_entries; i++)
	{
		printf("  second: %x\n", second_section->entries[i].unk);
	}

	for(int i = 0; i < third_section->num_entries; i++)
	{
		printf("   third: %x\n", third_section->entries[i].unk);
	}
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
				free(str);
			}
		}
	}
}

u32 stats_action[255];
u32 stats_action_maps[255][255];
u32 stats_action_map_num[255];
u32 stats_command[255];
u32 stats_command_maps[255][255];
u32 stats_command_map_num[255];

void read_iact_stats(u16 map_num, u32 location, u16 num_iacts)
{
	//printf("Reading IACT data, %u IACTs\n", num_iacts);
	seek(location);
	for(int i = 0; i < num_iacts; i++)
	{
		read_long(); //IACT
		u32 ignored6 = read_long();
		u16 iactItemCount1 = read_short();
		//printf("\n    Action: unknown %08x, actions %d\n", ignored6, iactItemCount1);
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

			stats_action[command]++;
			if(stats_action_map_num[command] > 0)
			{
				if(stats_action_maps[command][stats_action_map_num[command] - 1] != map_num)
					stats_action_maps[command][stats_action_map_num[command]++] = map_num;
			}
			else
			{
				stats_action_maps[command][stats_action_map_num[command]++] = map_num;
			}
			//printf("        %s, %04x, %04x, %04x, %04x, %04x, %04x, %s\n", triggers[command], arg1, arg2, arg3, arg4, arg5, arg6, pos_str);
		}
		u16 iactItemCount2 = read_short();
		//printf("    Script: commands %d\n", iactItemCount2);
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

			stats_command[command]++;
			if(stats_command_map_num[command] > 0)
			{
				if(stats_command_maps[command][stats_command_map_num[command] - 1] != map_num)
					stats_command_maps[command][stats_command_map_num[command]++] = map_num;
			}
			else
			{
				stats_command_maps[command][stats_command_map_num[command]++] = map_num;
			}

			//printf("        %s, %04x, %04x, %04x, %04x, %04x, %04x, %s\n", commands[command], arg1, arg2, arg3, arg4, arg5, strlen, pos_str);
			if (strlen)
			{
				char* str = malloc(strlen+1);
				for (u16 l = 0; l < strlen; l++)
				{
					str[l] = read_byte();
				}
				//printf("            \"%s\"\n", str);
				free(str);
			}
		}
	}
}

void print_iact_stats()
{
	for(int i = 0; i < 0x24; i++)
	{
		printf("%x: %u,    ", i, stats_action[i]);
		for(int j = 0; j < stats_action_map_num[i]; j++)
		{
			if(stats_action[i] < 50)
				printf("%u, ", stats_action_maps[i][j]);
		}
		printf("\n");
	}

	printf("\n");

	for(int i = 0; i < 0x26; i++)
	{
		printf("%x: %u,   ", i, stats_command[i]);
		for(int j = 0; j < stats_command_map_num[i]; j++)
		{
			if(stats_command[i] < 50)
				printf("%u, ", stats_command_maps[i][j]);
		}
		printf("\n");
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
			tiles_overlay[(j*9)+i] = map_overlay[((camera_y+j)*width)+i+camera_x];
		}
	}

	if(player_entity.current_frame >= 2)
	{
		if(player_entity.y >= camera_y &&
		    player_entity.y < (camera_y + 9) &&
		    player_entity.x >= camera_x &&
		    player_entity.x < (camera_x + 9))
		{
			tiles_high[((player_entity.y - camera_y)*9) + (player_entity.x - camera_x)] = char_data[player_entity.char_id]->frames[player_entity.current_frame];
		}
	}

	for(int i = 0; i < num_entities; i++)
	{
		entity *e = entities[i];
		if(e->current_frame < 2)
			continue; //These frames are transparent/unused in Yoda Stories, not sure for Indy

		if(e->y >= camera_y &&
			e->y < camera_y + 9 &&
			e->x >= camera_x &&
			e->x < camera_x + 9)
		{
			tiles_high[((e->y - camera_y)*9) + (e->x - camera_x)] = char_data[e->char_id]->frames[e->current_frame];
			tiles_overlay[((e->y - camera_y)*9) + (e->x - camera_x)] = e->item;
		}
	}

	//Test for rendering characters
	/*for(int i = 0; i < 26; i++)
	{
		tiles_high[i] = char_data[id]->frames[i];
	}*/
}
