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

void load_map(u16 map_id)
{
	init_screen();
	u32 location = map_file_locs[map_id];
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

	unsigned int test = 0xD5828281 + 0xD5828281;
	printf("Loaded map %i, map type %x, width %i, height %i, %x\n", map_id, planet, width, height, test);
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
