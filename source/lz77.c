/*
 *      taiko 0.1 - A nandloader replacement for Nintendo Wii
 * 
 *      lz77.c
 * 
 *      Class for lz77 decompression routines.
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
#include <gccore.h>

#include "lz77.h"
#include "tools.h"

u32 packBytes(int a, int b, int c, int d)
{
	return (d << 24) | (c << 16) | (b << 8) | (a);
}

void __decompressLZ77_11(u8 *in, u32 inputLen, u8 **output, u32 outputLen)
{
	int x, y;
	
	u8 *out;
	
	u32 compressedPos 	= 0x4;
	u32 decompressedPos	= 0x0;
	u32 decompressedSize = 0;
	
	decompressedSize = packBytes(in[0], in[1], in[2], in[3]) >> 8;

	if (!decompressedSize)
	{
		decompressedSize = packBytes(in[4], in[5], in[6], in[7]);
		compressedPos += 0x4;
	}
	
	printf("\t[*] Decompressed size : %i\n", decompressedSize);
	
	out = memalign(32, decompressedSize);
	
	while (compressedPos < inputLen && decompressedPos < decompressedSize)
	{
		u8 byteFlag = in[compressedPos];
		compressedPos++;
		
		for (x = 7; x >= 0; x--)
		{
			if ((byteFlag & (1 << x)) > 0)
			{
				u8 first = in[compressedPos];
				u8 second = in[compressedPos + 1];
				
				u32 pos, copyLen;
				
				if (first < 0x20)
				{
					u8 third = in[compressedPos + 2];
					
					if (first >= 0x10)
					{
						u32 fourth = in[compressedPos + 3];
						
						pos			= (u32)(((third & 0xF) << 8) | fourth) + 1;
						copyLen		= (u32)((second << 4) | ((first & 0xF) << 12) | (third >> 4)) + 273;
						
						compressedPos += 4;
					} else {
						pos          = (u32)(((second & 0xF) << 8) | third) + 1;
						copyLen		 = (u32)(((first & 0xF) << 4) | (second >> 4)) + 17;
						
						compressedPos += 3;
					}
				} else {
					pos 		= (u32)(((first & 0xF) << 8) | second) + 1;
					copyLen		= (u32)(first >> 4) + 1;

					compressedPos += 2;
				}				

				for (y = 0; y < copyLen; y++)
				{
					out[decompressedPos + y] = out[decompressedPos - pos + y];
				}
				
				decompressedPos += copyLen;
			} else {
				out[decompressedPos] = in[compressedPos];
				
				decompressedPos++;
				compressedPos++;
			}
			
			if (compressedPos >= inputLen || decompressedPos >= decompressedSize)
				break;

		}
	}
	
	*output = out;
	outputLen = decompressedSize;
}

void __decompressLZ77_10(u8 *in, u32 inputLen, u8 **output, u32 outputLen)
{	
	int x, y;
	
	u8 *out;
	
	u32 compressedPos = 0;
	u32 decompressedSize = 0x4;
	u32 decompressedPos = 0;
	
	decompressedSize = packBytes(in[0], in[1], in[2], in[3]) >> 8;

	int compressionType = (packBytes(in[0], in[1], in[2], in[3]) >> 4) & 0xF;
	
	if (compressionType != 1)
	{
		__errorCheck(-1337, 1);
	}
	
	printf("\t[*] Decompressed size : %i\n", decompressedSize);
	
	out = memalign(32, decompressedSize);

	compressedPos += 0x4;
	
	while (decompressedPos < decompressedSize)
	{
		u8 flag = *(u8*)(in + compressedPos);
		compressedPos += 1;
		
		for (x = 0; x < 8; x++)
		{
			if (flag & 0x80)
			{
				u8 first = in[compressedPos];
				u8 second = in[compressedPos + 1];
				
				u16 pos = (u16)((((first << 8) + second) & 0xFFF) + 1);
				u8 copyLen = (u8)(3 + ((first >> 4) & 0xF));

				for (y = 0; y < copyLen; y++)
				{
					out[decompressedPos + y] = out[decompressedPos - pos + (y % pos)];
				}
				
				compressedPos += 2;
				decompressedPos += copyLen;				
			}
			else
			{
				out[decompressedPos] = in[compressedPos];
				compressedPos += 1;
				decompressedPos += 1;
			}
			
			flag <<= 1;
			
			if (decompressedPos >= decompressedSize)
				break;
		}
	}

    *output = out;
	outputLen = decompressedSize;
}

int isLZ77compressed(u8 *buffer)
{
	if ((buffer[0] == LZ77_0x10_FLAG) || (buffer[0] == LZ77_0x11_FLAG))
	{
		return 1;
	}
	
	return 0;
}

void decompressLZ77content(u8 *buffer, u32 lenght, u8 **output, u32 outputLen)
{
	switch (buffer[0])
	{
		case LZ77_0x10_FLAG:
			printf("\t[*] LZ77 variant 0x10 compressed content...unpacking may take a while...\n");
			__decompressLZ77_10(buffer, lenght, output, outputLen); break;
		case LZ77_0x11_FLAG:
			printf("\t[*] LZ77 variant 0x11 compressed content...unpacking may take a while...\n");
			__decompressLZ77_11(buffer, lenght, output, outputLen); break;
		default:
			__errorCheck(-1337, 1);
	}
}
