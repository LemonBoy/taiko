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

#define taikoConfigPath ("/shared2/taiko.conf")

void loadTaikoConf();

#endif
