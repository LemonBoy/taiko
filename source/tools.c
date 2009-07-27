/*
 * 		taiko 0.1 - A nandloader replacement for Nintendo Wii
 * 
 *      tools.c
 * 
 * 		Module holding all the tool functions.
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

void debugPrint(const char *text, ...)
{
	#ifdef DEBUG
	char buffer[1024];
	va_list args;
	va_start(args, text);
	vsprintf(buffer, text, args);
	va_end(args);
	  
	printf(buffer);
	
	sleep(10);
	#endif
}

void __setupRam()
{
	printf("\t[*] Setting bus speed\n");
	*(u32*)0x800000F8 = 0x0E7BE2C0;
	printf("\t[*] Setting cpu speed\n");
	*(u32*)0x800000FC = 0x2B73A840;
	
	DCFlushRange((void*)0x800000F8, 0xFF);
}
