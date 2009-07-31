/*
 * 	    taiko 0.1 - A nandloader replacement for Nintendo Wii
 * 
 *      config.c
 * 
 * 	    Module to handle the profile database.
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
		__errorCheck(ISFS_CreateFile(taikoConfigPath, 0, ISFS_OPEN_RW, ISFS_OPEN_RW, ISFS_OPEN_RW), 0);
		tConf = ISFS_Open(taikoConfigPath, ISFS_OPEN_RW);
	}
	
	if (__errorCheck(tConf, 0) < 0)
	{
		printf("\t[*] Cannot open configuration file\n");
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

void searchProfile(u32 titleHigh, u32 titleLow, taikoConf *foundProfile)
{
	int currentProfile;
	
	if ((!configLoaded) || (!foundProfiles))
		foundProfile = NULL;
	
	for (currentProfile = 0; currentProfile < foundProfiles; currentProfile++)
	{
		if ((taikoConfItems[currentProfile].titleHigh = titleHigh) &&
			(taikoConfItems[currentProfile].titleHigh = titleLow))
		{
			foundProfile = &taikoConfItems[currentProfile];
			__dprintf("Found a valid profile\n");
		}
	}	
	
	foundProfile = NULL;
}
