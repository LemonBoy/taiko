/*
 * 	    taiko 0.1 - A nandloader replacement for Nintendo Wii
 * 
 *      titles.c
 * 
 *      Class for dealing with titles and their contents.
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
#include <malloc.h>
#include <string.h>
#include <gccore.h>

#include "titles.h"
#include "patch.h"
#include "tools.h"

#define DEBUG_TITLES

#ifdef DEBUG_TITLES
#define __dprintf(fmt, args...) \
	fprintf(stderr, "\t[+] %s:%d->" fmt, __FUNCTION__, __LINE__, ##args)
#else
#define __dprintf(fmt, args...)
#endif

int __findMainContent()
{
	int dolHeading[3]       = { 0x00, 0x00, 0x01 };
	
	static u8 tmdBuffer[MAX_SIGNED_TMD_SIZE] ATTRIBUTE_ALIGN(32);
	u8 contentBuffer[0x3] ATTRIBUTE_ALIGN(32);
	
	int x, titleContents, titleBootContent, mainContent;
	u32 tmdSize;

	if (ES_GetTitleID(&titleId) < 0)
		__rebootWii();	
	
	mainContent = 1;
	tmdSize = 0;
	
	__errorCheck(ES_GetStoredTMDSize(titleId, &tmdSize), 1);
	__errorCheck(ES_GetStoredTMD(titleId, (signed_blob*)tmdBuffer, tmdSize), 1);
	
	titleContents    = MAX_NUM_TMD_CONTENTS;//*(u16 *)(tmdBuffer + 0x1DE);
	titleBootContent = 7;                   //*(u16 *)(tmdBuffer + 0x1E0);
	
	printf("\t[*] The title has %i contents, we are content %i\n", titleContents, titleBootContent);
	
	for (x = 1; x <= titleContents; x++)
	{
		if (x == titleBootContent)
		{
			printf("\t[*] Boot content found. %08x.app\n", x);
			continue;
		}

		s32 nandFd = ES_OpenContent(x);
		
		__errorCheck(nandFd, 1);
		__errorCheck(ES_ReadContent(nandFd, contentBuffer, 0x3), 1);
		__errorCheck(ES_CloseContent(nandFd), 1);
		
		if (!(memcmp(contentBuffer, dolHeading, 0x3)))
		{
			printf("\t[*] %08x.app seem a valid dol.\n", x);
			mainContent = x;
		}
		else if (isLZ77compressed(contentBuffer))
		{
			printf("\t[*] %08x.app seem a valid dol but lz77 compressed.\n", x);
			mainContent = x;
		}
		else
		{
			printf("\t[*] %08x.app isnt a valid dol.\n", x);
		}
	}
	
	return mainContent;
}
			
u32 __relocate(u8 *dolBuffer)
{
	u32 x;
	static dolheader *dol __attribute__((aligned(32)));
	dol = (dolheader *)dolBuffer; 
	
	printf("\t[*] Clear BSS\n");
	
	memset((void*)dol->bssmem, 0, dol->bsssize);
	DCFlushRange((void*)dol->bssmem, dol->bsssize);
	
	for (x = 0; x < 7; x++)
	{
		if (dol->textoff[x] < sizeof(dolheader)) continue;
		memcpy((void *)dol->textmem[x], dolBuffer + dol->textoff[x], dol->textsize[x]);
		printf("\t[*] Read text section\n");
	}

	for (x = 0; x < 11; x++)
	{
		if (dol->dataoff[x] < sizeof(dolheader)) continue;
		memcpy((void *)dol->datamem[x], dolBuffer + dol->dataoff[x], dol->datasize[x]);
		printf("\t[*] Read data section\n");
	}
	
	printf("\t[*] Entry point at : 0x%x\n", dol->entry);
	
	free(dolBuffer);
	
	return dol->entry;
}

u32 __load(u16 contentIndex)
{
	u8 *data;
	u8 *decData;
	
	u32 dataSize;
	u32 decDataSize;
	
	if (ES_GetTitleID(&titleId) < 0)
		__errorCheck(-1234, 1);
		
	__dprintf("contentIndex->%i\n", contentIndex);
	
	dataSize = 0;
	decDataSize = 0;

	s32 nandFd = ES_OpenContent(contentIndex);
	__errorCheck(nandFd, 1);
	dataSize = ES_SeekContent(nandFd, 0, SEEK_END);
	__errorCheck(dataSize, 1);
	
	ES_SeekContent(nandFd, 0, SEEK_SET);

	printf("\t[*] Content %i\n", contentIndex);
	printf("\t[*] Content size : %i\n", dataSize);
		
	data = memalign(32, dataSize);
	
	__errorCheck(ES_ReadContent(nandFd, data, dataSize), 1);
	__errorCheck(ES_CloseContent(nandFd), 1);
	
	int compressed = isLZ77compressed(data);

	if (compressed)
	{
		decompressLZ77content(data, dataSize, &decData, decDataSize);	
		free(data);
	}
	
	__dprintf("Now testing the patch system, pattern size %i\n", sizeof(pattern_002_error));
	
	u32 error002patch = searchPattern(0x80000000, dataSize, pattern_002_error, sizeof(pattern_002_error));
 
	if (error002patch > 0)
		patchAddress(error002patch, patch_002_error, sizeof(patch_002_error));
	
	fix002error();
	
	if (compressed)
		return __relocate(decData);
	else
		return __relocate(data);
	
}
