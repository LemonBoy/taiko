#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <gccore.h>
#include <unistd.h>

#include "tools.h"
#include "config.h"

static int configLoaded = 0;
static taikoConf * taikoConfItems;
static int foundProfiles = 0;

void loadTaikoConf()
{
	s32 tConf;
	fstats *configStat = (fstats *)memalign( 32, sizeof(fstats) );
	
	int savedProfiles = 0, currentProfile = 0;
	u8 *itemBuf = memalign(32, sizeof(taikoConf));	

	tConf = ISFS_Open(taikoConfigPath, ISFS_OPEN_RW);
		
	if ((tConf < 0) && (tConf == -106))
	{
		__errorCheck(ISFS_CreateFile(taikoConfigPath, 0, ISFS_OPEN_RW, ISFS_OPEN_RW, ISFS_OPEN_READ), 0);
		tConf = ISFS_Open(taikoConfigPath, ISFS_OPEN_RW);
	}
	
	if (__errorCheck(tConf, 0) < 0)
	{
		printf("\t[*] Cannot open configuration file\n"); sleep(10);
		return;
	}
	
	__errorCheck(ISFS_GetFileStats(tConf, configStat), 1);

	savedProfiles = configStat->file_length / sizeof(taikoConf);
	taikoConfItems = calloc(savedProfiles, sizeof(taikoConf));
	
	printf("\t[*] Found %i profiles\n", savedProfiles); sleep(10);
	
	for (currentProfile = 0; currentProfile < savedProfiles; currentProfile++)
	{
		__errorCheck(ISFS_Read(tConf, itemBuf, sizeof(taikoConf)), 1);
		memcpy(taikoConfItems + currentProfile * sizeof(taikoConf), itemBuf, sizeof(taikoConf));
		
		foundProfiles++;
	}
	
	configLoaded = 1;
	
	__errorCheck(ISFS_Close(tConf), 1);
}

taikoConf searchProfile(u32 titleHigh, u32 titleLow)
{
	int currentProfile;
	
	if ((!configLoaded) || (!foundProfiles))
		return *(taikoConf*)NULL;
	
	for (currentProfile = 0; currentProfile < foundProfiles; currentProfile++)
	{
		if ((taikoConfItems[currentProfile].titleHigh = titleHigh) &&
			(taikoConfItems[currentProfile].titleHigh = titleLow))
				return taikoConfItems[currentProfile];
	}
	
	return *(taikoConf*)NULL;
}
