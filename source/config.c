/*
 * 	    taiko 0.1 - A nandloader replacement for Nintendo Wii
 * 
 *      config.c
 * 
 * 	    Module to handle the profile database stored in /shared2/taiko.conf.
 *      
 *      Copyright 2009 The Lemon Man <giuseppe@FullMetal>
 *      
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <gccore.h>
#include <unistd.h>

#include "tools.h"
#include "config.h"

#define DEBUG_CONFIG

#ifdef DEBUG_CONFIG
#define __dprintf(fmt, args...) \
	fprintf(stderr, "\t[+] %s:%d->" fmt, __FUNCTION__, __LINE__, ##args)
#else
#define __dprintf(fmt, args...)
#endif

static bool configLoaded = false;
static s32 configFd = 0;

void loadTaikoConf()
{
	configFd = ISFS_Open(taikoConfigPath, ISFS_OPEN_RW);
		
	if ((configFd < 0) && (configFd == -106))
	{
		__errorCheck(ISFS_CreateFile(taikoConfigPath, 0, ISFS_OPEN_RW, ISFS_OPEN_RW, ISFS_OPEN_RW), 0);
		configFd = ISFS_Open(taikoConfigPath, ISFS_OPEN_RW);
	}
	
	if (__errorCheck(configFd, 0) < 0)
	{
		printf("\t[*] Cannot open configuration file\n");
		return;
	}
	
	configLoaded = true;
}
	
taikoConf *getProfile(u64 titleId)
{
	int currentProfile;
	int savedProfiles = 0;
	u8 *itemBuf = memalign( 32, sizeof(taikoConf) );
	fstats *configStat = (fstats *)memalign( 32, sizeof(fstats) );
	
	if (!configLoaded)
	{
		return NULL;
	}
	
	__errorCheck(ISFS_GetFileStats(configFd, configStat), 1);

	savedProfiles = configStat->file_length / sizeof(taikoConf);
	
	free(configStat);
	
	printf("\t[*] Found %i profiles\n", savedProfiles);
	
	for (currentProfile = 0; currentProfile < savedProfiles; currentProfile++)
	{
		__errorCheck(ISFS_Read(configFd, itemBuf, sizeof(taikoConf) - sizeof(int)), 1);
		taikoConf *c = (taikoConf *)itemBuf;
		if (c->titleId == titleId)
		{
			printf ("\t[*] Profile found\n");
			return (taikoConf*)(itemBuf);
		}
	}
	
	free(itemBuf);
	
	return NULL;
}

void addProfile(u64 titleid, int videoFlag, int mainContent)
{
	int index;
	
	index = -1;
	
	if (!configLoaded)
	{
		return;
	}
	
	searchProfile(titleid, index);
	
	if (index < 0)
	{
		__dprintf("Profile for %16llx not found\n", titleid);
		
		foundProfiles++;

		taikoConfItems = (taikoConf *)realloc(taikoConfItems, sizeof(taikoConf) * foundProfiles);
		memset(&(taikoConfItems[foundProfiles]), 0, sizeof(taikoConf));
		
		taikoConfItems[foundProfiles].titleId = titleid;
		taikoConfItems[foundProfiles].videoFlag = videoFlag;
		taikoConfItems[foundProfiles].mainContent = mainContent;
	} else {
		__dprintf("Profile for %16llx found\n", titleid);
		
		taikoConfItems[index].videoFlag = videoFlag;
		taikoConfItems[index].mainContent = mainContent;
	}	
	
	__dprintf("There are %i profiles now\n", foundProfiles);

}

void saveProfiles()
{
	s32 tConf;
	u8* profileBuf;
	
	if ((!configLoaded) || (!foundProfiles))
	{
		return;
	}
	
	__dprintf("Writing back %i profiles\n", foundProfiles);
	
	tConf = ISFS_Open(taikoConfigPath, ISFS_OPEN_RW);
	
	__errorCheck(tConf, 1);

	profileBuf = memalign(32, sizeof(taikoConf));
	
	while (foundProfiles > 0)
	{
		memcpy(profileBuf, &taikoConfItems[foundProfiles], sizeof(taikoConf));
		__errorCheck(ISFS_Write(tConf, profileBuf, sizeof(taikoConf)), 1);
		
		foundProfiles--;
	}
	
	__errorCheck(ISFS_Close(tConf), 1);
	
	configLoaded = 0;
	
	free(profileBuf);
	free(taikoConfItems);
}
