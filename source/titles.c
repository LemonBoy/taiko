/*
 * 		taiko 0.1 - A nandloader replacement for Nintendo Wii
 * 
 *      titles.c
 * 
 * 		Class for dealing with titles and their contents.
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
#include <malloc.h>
#include <string.h>
#include <gccore.h>

#include "titles.h"
#include "tools.h"

void __identifyAsTitle()
{
	s32 fd;
	char tikPath[ISFS_MAXPATH];
	u32 tmdSize = 0, key = 0;
	u8 *certsBuffer;
	u8 *tikBuffer;
	u8 *tmdBuffer;
	
	certsBuffer = memalign(32, 0xA00);
	fd = ISFS_Open("/sys/cert.sys", ISFS_OPEN_READ);
	debugPrint("\t[*] %s -> ISFS_Open (certs) -> %i\n", __FUNCTION__, fd);
	debugPrint("\t[*] %s -> IOS_Read (certs) -> %i\n", __FUNCTION__, ISFS_Read(fd, certsBuffer, 0xA00));
	ISFS_Close(fd);

	sprintf(tikPath, "/ticket/%08x/%08x.tik", (u32)(titleId >> 32), (u32)(titleId));
	tikBuffer = memalign(32, 0x2A4);
	fd = ISFS_Open(tikPath, ISFS_OPEN_READ);
	debugPrint("Tik path %s, file decscriptor %i\n", tikPath, fd);
	debugPrint("Tik read %i\n", ISFS_Read(fd, tikBuffer, 0x2A4));
	ISFS_Close(fd);

	debugPrint("\t[*] %s -> ES_GetStoredTMDSize -> %i\n", __FUNCTION__, ES_GetStoredTMDSize(titleId, &tmdSize));
	ES_GetStoredTMDSize(titleId, &tmdSize);
	tmdBuffer = memalign(32, tmdSize);
	debugPrint("\t[*] %s -> ES_GetStoredTMD -> %i\n", __FUNCTION__, ES_GetStoredTMD(titleId, (signed_blob*)tmdBuffer, tmdSize));

	debugPrint("\t[*] ES_Identify->%i\n", ES_Identify((signed_blob*)certsBuffer, 0xA00, (signed_blob*)tmdBuffer, tmdSize, (signed_blob*)tikBuffer, 0x2A4, &key));
}

int __findMainContent()
{
	int dolHeading[3] = { 0x00, 0x00, 0x01 };
	int lz77Heading[1] = { 0x11 };
	
	static u8 tmdBuffer[MAX_SIGNED_TMD_SIZE] ATTRIBUTE_ALIGN(32);
	u8 contentBuffer[0x3] ATTRIBUTE_ALIGN(32);
	
	char contentPath[ISFS_MAXPATH];
	int x, titleContents, titleBootContent;
	u32 tmdSize;
	
	debugPrint("\t%s -> %i\n", "ES_GetStoredTMDSize", ES_GetStoredTMDSize(titleId, &tmdSize));
	debugPrint("\t%s -> %i\n", "ES_GetStoredTMD", ES_GetStoredTMD(titleId, (signed_blob*)tmdBuffer, tmdSize));
	
	titleContents = *(u16 *)(tmdBuffer + 0x1DE);
	titleBootContent = *(u16 *)(tmdBuffer + 0x1E0);
	
	printf("\t[*] The title has %i contents, we are content %i\n", titleContents, titleBootContent);
	
	for (x = 1; x <= titleContents; x++)
	{
		if (x == titleBootContent)
		{
			printf("\t[*] Boot content found. %08x.app\n", x);
			continue;
		}
		
		sprintf(contentPath, "/title/%08x/%08x/content/%08x.app", (u32)(titleId >> 32), (u32)(titleId), x);
		
		s32 nandFd = ISFS_Open(contentPath, ISFS_OPEN_READ);
		ISFS_Read(nandFd, contentBuffer, 0x3);
		ISFS_Close(nandFd);
		
		if (!(memcmp(contentBuffer, dolHeading, 0x3)))
		{
			printf("\t[*] %08x.app seem a valid dol.\n", x);
		}
		else if (!(memcmp(contentBuffer, lz77Heading, 0x1)))
		{
			printf("\t[*] %08x.app seem a valid dol but lz77 compressed.\n", x);
		}
		else
		{
			printf("\t[*] %08x.app isnt a valid dol.\n", x);
		}
	}
	
	return 1;
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
	
	return dol->entry;
}

u32 __load(u16 contentIndex)
{
	char contentPath[ISFS_MAXPATH];
	u8 *data;
	u8 *decData;
	fstats *contentStat = (fstats *)memalign( 32, sizeof(fstats) );

	sprintf(contentPath, "/title/%08x/%08x/content/%08x.app", (u32)(titleId >> 32), (u32)(titleId), contentIndex);

	s32 nandFd = ISFS_Open(contentPath, ISFS_OPEN_READ);
	printf("\t[*] File descriptor : %i\n", nandFd);
	ISFS_GetFileStats(nandFd, contentStat);

	printf("\t[*] Content %i at %s\n", contentIndex, contentPath);
	printf("\t[*] Content size : %i\n", contentStat->file_length);
		
	data = memalign(32, contentStat->file_length);
	ISFS_Read(nandFd, data, contentStat->file_length);
	ISFS_Close(nandFd);
	
	__hexdump(data, 100);

	if (data[0] == LZ77_0x11_FLAG)
	{
		printf("\t[*] LZ77 compressed content...unpacking may take a while...\n");
		__decompressLZ77_1(data, contentStat->file_length, &decData);
		return __relocate(decData);
	}

	return __relocate(data);
	
}
