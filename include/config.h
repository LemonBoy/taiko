#ifndef _CONFIGURATION_MODULE
#define _CONFIGURATION_MODULE

#include <gccore.h>

typedef struct _taikoConfig
{
	u64 titleId;
	int videoFlag;
	int mainContent;
} taikoConf;

#define taikoConfigPath ("/shared2/taiko.conf")

void loadTaikoConf();
void saveProfiles();

void addProfile(u64 titleid, int videoFlag, int mainContent);

#endif
