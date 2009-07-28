#ifndef _TITLES_MODULE
#define _TITLES_MODULE

#include <gccore.h>
#include "lz77.h"

#define maxTextSections 7
#define maxDataSections 11

typedef struct _dolheader 
{
	u32 textoff	[maxTextSections];
	u32 dataoff	[maxDataSections];
	u32 textmem	[maxTextSections];
	u32 datamem	[maxDataSections];
	u32 textsize[maxTextSections];
	u32 datasize[maxDataSections];
	u32 bssmem;
	u32 bsssize;
	u32 entry;
} dolheader;

typedef void (*dolEntry)(void);

static u64 titleId;

int __findMainContent();
u32 __load(u16 contentIndex);

#endif
