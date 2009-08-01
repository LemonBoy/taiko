/*
 * 	    taiko 0.1 - A nandloader replacement for Nintendo Wii
 * 
 *      patch.c
 * 
 *      Class for memory patching.
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
#include <unistd.h>
#include <string.h>
#include <gccore.h>

#include "tools.h"

#define _CHECK_PATCH

#define DEBUG_PATCH

#ifdef DEBUG_PATCH
#define __dprintf(fmt, args...) \
	fprintf(stderr, "\t[+] %s:%d->" fmt, __FUNCTION__, __LINE__, ##args)
#else
#define __dprintf(fmt, args...)
#endif

void fix002error()
{
	write32(0x80003188, read32(0x80003140));
}

u32 searchPattern(u32 address, u32 len, u32 *pattern, u32 patternLen)
{
	void *memoryBase;
	
	u32 currentAddress;
	u32 scanEndAddress;
	
	if (address >= 0x80000000 && address + len < 0x90000000)
		memoryBase = (void *)address;
	else
		return 0;

	scanEndAddress = address + len;
	
	for (currentAddress = 0; currentAddress < scanEndAddress; currentAddress++)
	{
		if (!memcmp(memoryBase + currentAddress, pattern, patternLen))
		{
			__dprintf("Found pattern at %#x\n", (u32)(memoryBase + currentAddress));
			return (u32)(memoryBase + currentAddress);
		}
	}

	__dprintf("Pattern not found\n");
	
	return 0;
}

void patchAddress(u32 address, u32 *patch, u32 patchLen)
{
	void *memoryBase = (void *)address;
	
	memcpy(memoryBase, patch, patchLen);
#ifdef _CHECK_PATCH
	if (!memcmp(memoryBase, patch, patchLen))
		__dprintf("Patch applied at %#x!\n", address);
	else
		__dprintf("Failed to patch %#x!\n", address);
#endif
	
}
