#ifndef _PATCH_MODULE
#define _PATCH_MODULE

static u32 pattern_002_error[2] = {0x40820214, 0x3C608000};
static u32 patch_002_error[2] = {0x48000214, 0x3C608000};

u32 searchPattern(u32 address, u32 len, u32 *pattern, u32 patternLen);
void patchAddress(u32 address, u32 *patch, u32 patchLen);
void fix002error();

#endif
