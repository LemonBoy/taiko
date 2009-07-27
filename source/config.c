#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <gccore.h>

#include "tools.h"
#include "config.h"

s32 tConf = 0;
int foundProfiles = 0;

void loadTaikoConf()
{
	int savedProfiles = 0, currentProfile = 0;
	fstats *configStat = (fstats *)memalign( 32, sizeof(fstats) );
	u8 *itemBuf = memalign(32, sizeof(taikoConf));
	
	if (!tConf)
		tConf = ISFS_Open(taikoConfigPath, ISFS_OPEN_RW);
		
	debugPrint("\t [*] tConf %i\n", tConf);	
		
	ISFS_GetFileStats(tConf, configStat);
	
	savedProfiles = configStat->file_length / sizeof(taikoConf);
	taikoConfItems = calloc(savedProfiles, sizeof(taikoConf));
	
	debugPrint("\t [*] Found %i profiles\n", savedProfiles);
	
	for (currentProfile = 0; currentProfile < savedProfiles; currentProfile++)
	{
		ISFS_Read(tConf, itemBuf, sizeof(taikoConf));
		memcpy(taikoConfItems + currentProfile * sizeof(taikoConf), itemBuf, sizeof(taikoConf));
		
		foundProfiles++;
	}
}

taikoConf searchProfile(u32 titleHigh, u32 titleLow)
{
	int currentProfile;
	
	for (currentProfile = 0; currentProfile < foundProfiles; currentProfile++)
	{
		if ((taikoConfItems[currentProfile].titleHigh = titleHigh) &&
			(taikoConfItems[currentProfile].titleHigh = titleLow))
				return taikoConfItems[currentProfile];
	}
	
	return *(taikoConf*)NULL;
}
