#ifndef _CONFIGURATION_MODULE
#define _CONFIGURATION_MODULE

#include <gccore.h>

typedef struct _taikoConfig
{
	u32 titleHigh;
	u32 titleLow;
	int videoFlag;
	int mainContent;
} taikoConf;

static taikoConf * taikoConfItems;

#define taikoConfigPath ("/title/00000001/00000002/data/taiko.conf")

void loadTaikoConf();

#endif
