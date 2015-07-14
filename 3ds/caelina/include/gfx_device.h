
#ifndef GFX_DEVICE
#define GFX_DEVICE


#ifdef __cplusplus
extern "C" {
#endif

void* gfxCreateDevice(int width, int height);
void  gfxDestroyDevice(void* device);
void  gfxMakeCurrent(void* device);
void  gfxFlush(unsigned char* fb);

#ifdef __cplusplus
}
#endif


#endif
