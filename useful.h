#ifdef PC_BUILD
#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long
#elif defined _3DS
#include <3ds.h>
#endif

/*typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef volatile u8 vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;

typedef volatile s8 vs8;
typedef volatile s16 vs16;
typedef volatile s32 vs32;
typedef volatile s64 vs64;*/

#define SCREEN_WIDTH  288
#define SCREEN_TILE_WIDTH (SCREEN_WIDTH / 32)
#define SCREEN_BPP     32

char* readFileToBytes(char* file, long *size);
