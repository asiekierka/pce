/*****************************************************************************
 * pce                                                                       *
 *****************************************************************************/

/*****************************************************************************
 * File name:   src/drivers/video/n3ds.h                                     *
 * Created:     2017-04-10 by Adrian Siekierka <kontakt@asie.pl>             *
 * Copyright:   (C) 2017 Adrian Siekierka <kontakt@asie.pl>                  *
 *****************************************************************************/

/*****************************************************************************
 * This program is free software. You can redistribute it and / or modify it *
 * under the terms of the GNU General Public License version 2 as  published *
 * by the Free Software Foundation.                                          *
 *                                                                           *
 * This program is distributed in the hope  that  it  will  be  useful,  but *
 * WITHOUT  ANY   WARRANTY,   without   even   the   implied   warranty   of *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU  General *
 * Public License for more details.                                          *
 *****************************************************************************/


#ifndef PCE_VIDEO_N3DS_H
#define PCE_VIDEO_N3DS_H 1


#include <stdio.h>
#include <3ds.h>
#include <citro3d.h>

#include <libini/libini.h>
#include <drivers/video/terminal.h>

#define TOUCH_AREA_MODIFIER 1
#define MAX_KEYS_DOWN 8
#define AUTOREPEAT_TIME 125

typedef struct
{
	u16 x, y, w, h;
	pce_key_t keycode;
	u8 flags;
} touch_area_t;

typedef struct {
	terminal_t     trm;
	
	C3D_Tex        tex_display, tex_keyboard;
	C3D_Mtx        mtx_top, mtx_bottom;
	C3D_RenderBuf  scr_top, scr_bottom;

	u8*            display_buf;
	unsigned       display_buf_w, display_buf_h;

	DVLB_s* shader_dvlb;
	shaderProgram_s shader_program;
	int shader_proj_mtx_loc;
	C3D_AttrInfo shader_attr;

	unsigned       button;
	u64            button_last_check;

	touch_area_t*  keys_down[MAX_KEYS_DOWN];
	unsigned       keys_down_count;
} n3ds_t;


/*!***************************************************************************
 * @short Create a new n3ds terminal
 *****************************************************************************/
terminal_t *n3ds_new (ini_sct_t *ini);


#endif
