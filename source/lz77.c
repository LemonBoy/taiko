/*
 * 		taiko 0.1 - A nandloader replacement for Nintendo Wii
 * 
 *      lz77.c
 * 
 * 		Class for lz77 decompression routines.
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

u32 packBytes(int a, int b, int c, int d)
{
	return ((int)a << 24) | ((int)b << 16) | ((int)c << 8) | ((int)d);
}

void __decompressLZ77_1(u8 *in, u32 inputLen, u8 **output)
{
	int x, y;
	
	u8 *out;
	
	u32 compressedPos 	= 0x4;
	u32 decompressedPos	= 0x0;
	u32 decompressedSize = 0;
	
	decompressedSize = packBytes(in[0], in[1], in[2], in[3]) & 0x00FFFFFF;

	if (!decompressedSize)
	{
		decompressedSize = packBytes(in[4], in[5], in[6], in[7]);
		compressedPos += 0x4;
	}
	
	printf("[*] Decompressed size : %i\n", decompressedSize);
	
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
}
