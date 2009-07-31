/*
 * 	    taiko 0.1 - A nandloader replacement for Nintendo Wii
 * 
 *      tools.c
 * 
 * 	    Module holding all the tool functions.
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
#include <gccore.h>

s32  __IOS_LoadStartupIOS()	{ return 0; }
void __clearConsole() 		{ printf("\x1b[2J"); }
void __rebootWii() 			{ SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0); }
void __shutdownIos()		{ SYS_ResetSystem(SYS_SHUTDOWN, 0, 0); }

char ascii(char s)
{
	if(s < 0x20) return '.';
	if(s > 0x7E) return '.';
	return s;
}

void __hexdump(void *d, int len) 
{
	u8 *data;
	int i, off;
	data = (u8*)d;
	for (off=0; off<len; off += 16) 
	{
		printf("\t%08x ", off);
		for(i=0; i<16; i++)
			if((i+off)>=len) printf("\t   ");
			else printf("\t%02x ",data[off+i]);
		
		printf("\t ");
		for(i=0; i<16; i++)
			if((i+off)>=len) printf("\t ");
			else printf("\t%c",ascii(data[off+i]));
	printf("\t\n");
	}
}

void __setupRam()
{
	printf("\t[*] Setting bus speed\n");
	*(u32*)0x800000F8 = 0x0E7BE2C0;
	printf("\t[*] Setting cpu speed\n");
	*(u32*)0x800000FC = 0x2B73A840;
	
	DCFlushRange((void*)0x800000F8, 0xFF);
}

int __errorCheck(int ret, int fatal)
{
	if (ret >= 0) return 0;
	
	switch (ret)
	{
		case -1: case -102: case -1026:
			printf("\t[*] Forbidden resource"); break;
		case -4:
			printf("\t[*] Invalid argument"); break;
		case -106:
			printf("\t[*] File not found"); break;
		case -1017:
			printf("\t[*] No such IOCTL"); break;
		case -1234:
			printf("\t[*] Cannot retrieve title id"); break;	
		case -2016:
			printf("\t[*] Unaligned data"); break;
		case -1337:
			printf("\t[*] Decompression error"); break;
		case -2000:
			printf("\t[*] Wrong application entry point"); break;
		default:
			printf("\t[*] Error %i", ret); break;
	}
	
	printf("\n");
	
	if (fatal)
	{
		printf("\t[*] Rebooting in 10 seconds...");
		
		sleep(10);
		__rebootWii();
	}
	
	return -1;
}
