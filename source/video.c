/*
 * 		taiko 0.1 - A nandloader replacement for Nintendo Wii
 * 
 *      video.c
 * 
 * 		Video mode setting & patching.
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
#include <gccore.h>

#include "video.h"

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

void __initializeVideo()
{
    VIDEO_Init();
	
	rmode = VIDEO_GetPreferredMode(NULL);
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(xfb);

	VIDEO_SetBlack(FALSE);

	VIDEO_Flush();
	
	if (rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();
	
	CON_InitEx(rmode, 20, 20, rmode->fbWidth ,rmode->xfbHeight);

	VIDEO_ClearFrameBuffer(rmode, xfb, COLOR_BLACK);
}	

void __setVideoMode(char region, int forcedMode)
{
	u8 videoMode;
	GXRModeObj *vMode = NULL;
	
	vMode = VIDEO_GetPreferredMode(NULL);

    switch (region) 
    {
		case 'P':
			videoMode = (CONF_GetEuRGB60() > 0) ? VI_EURGB60 : VI_PAL;
		
			if (CONF_GetVideo() != CONF_VIDEO_PAL)
			{
				videoMode = VI_EURGB60;
				vMode = (CONF_GetProgressiveScan() > 0 && VIDEO_HaveComponentCable()) ? &TVNtsc480Prog : &TVEurgb60Hz480IntDf;
			}
			
			break;
        case 'E':
        case 'J':
        default:
		
		videoMode = VI_NTSC;
                              
		if (CONF_GetVideo() != CONF_VIDEO_NTSC)
        {
			vMode = (CONF_GetProgressiveScan() > 0 && VIDEO_HaveComponentCable()) ? &TVNtsc480Prog : &TVNtsc480IntDf;
        }
	}
	
	if (forcedMode > 0)
	{
		if (forcedMode & FORCE_PAL_60)
		{
			/* Force PAL60 */
			vMode = &TVEurgb60Hz480IntDf;
			videoMode = VI_EURGB60;
		} 
		if (forcedMode & FORCE_PAL_50)
		{
			/* Force PAL50 */
			vMode = &TVPal528IntDf;
			videoMode = VI_PAL;
		}
		if (forcedMode & FORCE_NTSC)
		{
			/* Force NTSC */
			vMode = &TVNtsc480IntDf;
			videoMode = VI_NTSC;
		}
		
		if (forcedMode & FORCE_COMPOSITE)
		{
			/* Force composite */
			vMode = &TVNtsc480Prog;
			videoMode = VI_NTSC;
		}
	}
	      		
	*(u32 *)0x800000CC = videoMode;
	DCFlushRange((void*)0x800000CC, sizeof(u32));
	
	VIDEO_Configure(vMode);
	VIDEO_SetNextFramebuffer(xfb);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	
	if (rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();

}
