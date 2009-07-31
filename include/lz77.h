#ifndef _LZ77_MODULE
#define _LZ77_MODULE

#define LZ77_0x10_FLAG 	0x10
#define LZ77_0x11_FLAG 	0x11

int isLZ77compressed(u8 *buffer);
void decompressLZ77content(u8 *buffer, u32 lenght, u8 **output, u32 outputLen);

#endif
