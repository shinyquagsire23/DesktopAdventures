#include "useful.h"
#include <GL/gl.h>

void load_resources();
void seek(u32 location);
void seek_add(u32 amount);
u32 get_location();
u32 read_long();
u16 read_short();
u8 read_byte();

//static const u8* yodesk_palette;
GLuint texture[0x1001];
u32 tile_metadata[0x1000];
u8 ASSETS_LOADING;
u32 *map_file_locs;
u16 NUM_MAPS;
