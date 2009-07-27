/*
 * 		taiko 0.1 - A nandloader replacement for Nintendo Wii
 * 
 *      main.c
 * 
 * 		Main module.
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
#include <stdlib.h>
#include <unistd.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

//#include "config.h"
#include "titles.h"
#include "video.h"
#include "tools.h"
#include "version.h"

static videoPatch viPatches[] = 
{
	{"Force PAL60", FORCE_PAL_60},
	{"Force PAL50", FORCE_PAL_50},
	{"Force NTSC", FORCE_NTSC},
	{"Force Composite", FORCE_COMPOSITE}
};

int main(int argc, char **argv) 
{	
	int videoFlags = NO_FORCING;
	dolEntry entryPoint;
		
	VIDEO_Init();
	
	__initializeVideo();
	ISFS_Initialize();
	
	printf("\n\n");
	printf("\ttaiko %i.%i\n", VERSION_MAJOR, VERSION_MINOR);
	
	if (ES_GetTitleID(&titleId) < 0)
		__rebootWii();
		
	printf("\t[*] Booting : %08x-%08x\n", (u32)(titleId >> 32), (u32)(titleId));
	printf("\t[*] Running under IOS%i (rev %i)\n", IOS_GetVersion(), IOS_GetRevision());
	
	//loadTaikoConf();
	//__identifyAsTitle();
	
	if (SYS_ResetButtonDown())
	{	
		int selectedPatch = 0;
		
		WPAD_Init();
		
		while (1)
		{
			printf("\x1b[6;0H");
			WPAD_ScanPads();
			
			printf("\t[*] Press LEFT and RIGHT to choose between patches.\n\t[*] Press A to apply patches and boot, B to boot directly.\n\n");
			printf("\t[*] Selected patch << %s >>\n", viPatches[selectedPatch].name);
			
			if (WPAD_ButtonsDown(0) & WPAD_BUTTON_LEFT) 	{ if (selectedPatch > 0) selectedPatch--; else selectedPatch = viPatchesCount; }
			if (WPAD_ButtonsDown(0) & WPAD_BUTTON_RIGHT) 	{ if (selectedPatch < viPatchesCount) selectedPatch++; else selectedPatch = 0; }
			if (WPAD_ButtonsDown(0) & WPAD_BUTTON_2) 		{ __findMainContent(); }
			if (WPAD_ButtonsDown(0) & WPAD_BUTTON_A) 		{ printf("\t[*] Applying patches...\n"); videoFlags |= viPatches[selectedPatch].patchFlag; break; }
			if (WPAD_ButtonsDown(0) & WPAD_BUTTON_B) 		{ break; }
		}
		
		WPAD_Flush(0);
		WPAD_Shutdown();
	}
	
	entryPoint = (dolEntry)__load(1);
	
	__setupRam();
	__setVideoMode(titleId & 0xFF, videoFlags);
	
	printf("\t[*] Video mode is ok, set as %c.\n", (char)(titleId & 0xFF));
	
	ISFS_Deinitialize();
	__shutdownIos();

	entryPoint();

	return 0;
}
