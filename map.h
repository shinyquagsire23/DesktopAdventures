#include "useful.h"

#define DESERT	0x1
#define SNOW	0x2
#define FOREST	0x3
#define SWAMP	0x5

u32 camera_x;
u32 camera_y;
u16 width;
u16 height;

void load_map(u16 map_id);
void render_map();
