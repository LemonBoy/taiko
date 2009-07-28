#ifndef _VIDEO_MODULE
#define _VIDEO_MODULE

#define NO_FORCING 		0x0
#define FORCE_PAL_60 	0x1
#define FORCE_PAL_50 	0x2
#define FORCE_NTSC 		0x3
#define FORCE_COMPOSITE 0x4

#define viPatchesCount (sizeof(viPatches) / sizeof(videoPatch) - 1)

typedef struct _videoPatch
{
	char *name;
	int patchFlag;
} videoPatch;

void __initializeVideo();
void __setVideoMode(char region, int forcedMode);

#endif
