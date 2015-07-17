#include "useful.h"
#include <GL/gl.h>

void load_resources();
void seek(u32 location);
void seek_add(u32 amount);
u32 get_location();
u32 read_long();
u16 read_short();
u8 read_byte();

typedef struct izon_data
{
	u32 izon_offset;
	u32 izax_offset;
	u32 izx2_offset;
	u32 izx3_offset;
	u32 izx4_offset;
	u16 num_iacts;
	u32 iact_offset;
} izon_data;

//static const u8* yodesk_palette;
GLuint texture[0x1001];
u32 tile_metadata[0x1000];
u8 ASSETS_LOADING;
izon_data **zone_data;
u16 NUM_MAPS;
