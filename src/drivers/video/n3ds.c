/*****************************************************************************
 * pce                                                                       *
 *****************************************************************************/

/*****************************************************************************
 * File name:   src/drivers/video/n3ds.c                                     *
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


#include <stdlib.h>
#include <string.h>

#include <drivers/video/terminal.h>
#include <drivers/video/n3ds.h>

#include <drivers/video/lodepng/lodepng.h>

static const char n3ds_shader_shbin[289] = {
	0x44, 0x56, 0x4c, 0x42, 0x01, 0x00, 0x00, 0x00, 0xa0, 0x00, 0x00, 
	0x00, 0x44, 0x56, 0x4c, 0x50, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 
	0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x54, 0x00, 0x00, 0x00, 0x08, 
	0x00, 0x00, 0x00, 0x94, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x4e, 0x01, 0xf0, 0x07, 0x4e, 0x02, 0x10, 0x20, 0x4e, 0x01, 0xf0, 
	0x27, 0x4e, 0x03, 0x08, 0x02, 0x08, 0x04, 0x18, 0x02, 0x08, 0x05, 
	0x28, 0x02, 0x08, 0x06, 0x38, 0x02, 0x08, 0x07, 0x10, 0x20, 0x4c, 
	0x07, 0x10, 0x41, 0x4c, 0x00, 0x00, 0x00, 0x88, 0x6e, 0x03, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0xa1, 0x0a, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x4e, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 
	0xc3, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0xc3, 0x06, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x62, 0xc3, 0x06, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x61, 0xc3, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6f, 0x03, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x56, 0x4c, 0x45, 0x02, 
	0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 
	0x00, 0x01, 0x00, 0x00, 0x00, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x54, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x6c, 
	0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x74, 0x00, 0x00, 0x00, 
	0x0b, 0x00, 0x00, 0x00, 0x02, 0x00, 0x5f, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 
	0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x03, 
	0x00, 0x01, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x02, 0x00, 0x02, 0x00, 
	0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x13, 
	0x00, 0x70, 0x72, 0x6f, 0x6a, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 
	0x00, 0x00, 0x00
};

static const int n3ds_shader_shbin_length = 289;

static touch_area_t touch_areas[] = {
  { 2, 97, 22, 16, PCE_KEY_ESC, 0 },
  { 23, 97, 22, 16, PCE_KEY_F1, 0 },
  { 44, 97, 22, 16, PCE_KEY_F2, 0 },
  { 65, 97, 22, 16, PCE_KEY_F3, 0 },
  { 86, 97, 22, 16, PCE_KEY_F4, 0 },
  { 107, 97, 22, 16, PCE_KEY_F5, 0 },
  { 128, 97, 22, 16, PCE_KEY_F6, 0 },
  { 149, 97, 22, 16, PCE_KEY_F7, 0 },
  { 170, 97, 22, 16, PCE_KEY_F8, 0 },
  { 191, 97, 22, 16, PCE_KEY_F9, 0 },
  { 212, 97, 22, 16, PCE_KEY_F10, 0 },
  { 233, 97, 22, 16, PCE_KEY_F11, 0 },
  { 254, 97, 22, 16, PCE_KEY_F12, 0 },
  { 275, 97, 22, 16, PCE_KEY_INS, 0 },
  { 296, 97, 22, 16, PCE_KEY_DEL, 0 },

  { 2, 112, 22, 22, PCE_KEY_BACKQUOTE, 0 },
  { 23, 112, 22, 22, PCE_KEY_1, 0 },
  { 44, 112, 22, 22, PCE_KEY_2, 0 },
  { 65, 112, 22, 22, PCE_KEY_3, 0 },
  { 86, 112, 22, 22, PCE_KEY_4, 0 },
  { 107, 112, 22, 22, PCE_KEY_5, 0 },
  { 128, 112, 22, 22, PCE_KEY_6, 0 },
  { 149, 112, 22, 22, PCE_KEY_7, 0 },
  { 170, 112, 22, 22, PCE_KEY_8, 0 },
  { 191, 112, 22, 22, PCE_KEY_9, 0 },
  { 212, 112, 22, 22, PCE_KEY_0, 0 },
  { 233, 112, 22, 22, PCE_KEY_MINUS, 0 },
  { 254, 112, 22, 22, PCE_KEY_EQUAL, 0 },
  { 275, 112, 43, 22, PCE_KEY_BACKSPACE, 0 },

  { 2, 133, 34, 22, PCE_KEY_TAB, 0 },
  { 35, 133, 22, 22, PCE_KEY_Q, 0 },
  { 56, 133, 22, 22, PCE_KEY_W, 0 },
  { 77, 133, 22, 22, PCE_KEY_E, 0 },
  { 98, 133, 22, 22, PCE_KEY_R, 0 },
  { 119, 133, 22, 22, PCE_KEY_T, 0 },
  { 140, 133, 22, 22, PCE_KEY_Y, 0 },
  { 161, 133, 22, 22, PCE_KEY_U, 0 },
  { 182, 133, 22, 22, PCE_KEY_I, 0 },
  { 203, 133, 22, 22, PCE_KEY_O, 0 },
  { 224, 133, 22, 22, PCE_KEY_P, 0 },
  { 245, 133, 22, 22, PCE_KEY_LBRACKET, 0 },
  { 266, 133, 22, 22, PCE_KEY_RBRACKET, 0 },
  { 287, 133, 31, 22, PCE_KEY_BACKSLASH, 0 },

  { 2, 154, 41, 22, PCE_KEY_LCTRL, TOUCH_AREA_MODIFIER },
  { 42, 154, 22, 22, PCE_KEY_A, 0 },
  { 63, 154, 22, 22, PCE_KEY_S, 0 },
  { 84, 154, 22, 22, PCE_KEY_D, 0 },
  { 105, 154, 22, 22, PCE_KEY_F, 0 },
  { 126, 154, 22, 22, PCE_KEY_G, 0 },
  { 147, 154, 22, 22, PCE_KEY_H, 0 },
  { 168, 154, 22, 22, PCE_KEY_J, 0 },
  { 189, 154, 22, 22, PCE_KEY_K, 0 },
  { 210, 154, 22, 22, PCE_KEY_L, 0 },
  { 231, 154, 22, 22, PCE_KEY_SEMICOLON, 0 },
  { 252, 154, 22, 22, PCE_KEY_QUOTE, 0 },
  { 273, 154, 45, 22, PCE_KEY_RETURN, 0 },

  { 2, 175, 54, 22, PCE_KEY_LSHIFT, TOUCH_AREA_MODIFIER },
  { 55, 175, 22, 22, PCE_KEY_Z, 0 },
  { 76, 175, 22, 22, PCE_KEY_X, 0 },
  { 97, 175, 22, 22, PCE_KEY_C, 0 },
  { 118, 175, 22, 22, PCE_KEY_V, 0 },
  { 139, 175, 22, 22, PCE_KEY_B, 0 },
  { 160, 175, 22, 22, PCE_KEY_N, 0 },
  { 181, 175, 22, 22, PCE_KEY_M, 0 },
  { 202, 175, 22, 22, PCE_KEY_COMMA, 0 },
  { 223, 175, 22, 22, PCE_KEY_PERIOD, 0 },
  { 244, 175, 22, 22, PCE_KEY_SLASH, 0 },
  { 265, 175, 53, 22, PCE_KEY_RSHIFT, TOUCH_AREA_MODIFIER },

  { 3, 196, 28, 22, PCE_KEY_LMETA, TOUCH_AREA_MODIFIER },
  { 29, 196, 37, 22, PCE_KEY_LALT, TOUCH_AREA_MODIFIER },
  { 65, 196, 142, 22, PCE_KEY_SPACE, 0 },
  { 206, 196, 28, 22, PCE_KEY_RALT, TOUCH_AREA_MODIFIER },
  { 233, 196, 22, 22, PCE_KEY_HOME, 0 },
  { 254, 196, 22, 22, PCE_KEY_UP, 0 },
  { 275, 196, 22, 22, PCE_KEY_END, 0 },
  { 296, 196, 22, 22, PCE_KEY_PAGEUP, 0 },

  { 233, 217, 22, 22, PCE_KEY_LEFT, 0 },
  { 254, 217, 22, 22, PCE_KEY_DOWN, 0 },
  { 275, 217, 22, 22, PCE_KEY_RIGHT, 0 },
  { 296, 217, 22, 22, PCE_KEY_PAGEDN, 0 }
};
#define touch_areas_len (sizeof(touch_areas) / sizeof(touch_area_t))

#define N3DS_CIRCLE_DEAD_ZONE 10

static
unsigned next_power_of_two(unsigned v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	return v + 1;
}

static
void n3ds_draw_texture(C3D_Tex* tex, int x, int y, int tx, int ty, int width, int height, float cmul)
{
	float xmin, ymin, txmin, tymin, xmax, ymax, txmax, tymax;
	xmin = x;
	ymin = y;
	xmax = x+width;
	ymax = y+height;
	txmin = (float) tx / tex->width;
	txmax = (float) (tx + width) / tex->width;
	tymin = (float) ty / tex->height;
	tymax = (float) (ty + height) / tex->height;

	C3D_TexBind(0, tex);
	C3D_ImmDrawBegin(GPU_TRIANGLE_STRIP);
                C3D_ImmSendAttrib(xmin, ymin, 0.0f, 0.0f);
                C3D_ImmSendAttrib(txmin, tymin, cmul, 0.0f);

                C3D_ImmSendAttrib(xmax, ymin, 0.0f, 0.0f);
                C3D_ImmSendAttrib(txmax, tymin, cmul, 0.0f);

                C3D_ImmSendAttrib(xmin, ymax, 0.0f, 0.0f);
                C3D_ImmSendAttrib(txmin, tymax, cmul, 0.0f);

                C3D_ImmSendAttrib(xmax, ymax, 0.0f, 0.0f);
                C3D_ImmSendAttrib(txmax, tymax, cmul, 0.0f);
	C3D_ImmDrawEnd();
}

static
void n3ds_free (n3ds_t *nt)
{
}

static
void n3ds_del (n3ds_t *nt)
{
	if (nt != NULL) {
		n3ds_free (nt);
		free (nt);
	}
}

static
void n3ds_resize (n3ds_t *nt, unsigned w, unsigned h)
{
	linearFree(nt->display_buf);
	C3D_TexDelete(&(nt->tex_display));

	nt->display_buf_w = next_power_of_two(w);
	nt->display_buf_h = next_power_of_two(h);
	nt->display_buf = linearAlloc(nt->display_buf_w * nt->display_buf_h * 3);

	C3D_TexInit(&(nt->tex_display), next_power_of_two(w), next_power_of_two(h), GPU_RGB8);
}

static
int n3ds_open_keyboard (n3ds_t *nt)
{
	unsigned *image, *imageLinear;
	unsigned width, height, i, sdtFlags;

	lodepng_decode32_file(&image, &width, &height, "romfs:/kbd_display.png");
	
	C3D_TexInit(&(nt->tex_keyboard), width, height, GPU_RGBA8);
	imageLinear = linearAlloc(width * height * 4);

	for (i = 0; i < width*height; i++)
		imageLinear[i] = __builtin_bswap32(image[i]);

	GSPGPU_FlushDataCache(imageLinear, width * height * 4);
	sdtFlags = (GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(1) | GX_TRANSFER_RAW_COPY(0) |
                GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGBA8) |
                GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO));

	GX_DisplayTransfer(imageLinear, GX_BUFFER_DIM(width, height), nt->tex_keyboard.data, GX_BUFFER_DIM(width, height), sdtFlags);
	gspWaitForPPF();

	linearFree(imageLinear);
	free(image);
}

static
int n3ds_open (n3ds_t *nt, unsigned w, unsigned h)
{
	C3D_TexEnv* texEnv;

	romfsInit();
	gfxInitDefault();
	gfxSet3D(false);
//	consoleInit(GFX_BOTTOM, NULL);

	osSetSpeedupEnable(1);
	APT_SetAppCpuTimeLimit(80);

	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);

	C3D_RenderBufInit(&(nt->scr_top), 240, 400, GPU_RB_RGB8, 0);
	C3D_RenderBufInit(&(nt->scr_bottom), 240, 320, GPU_RB_RGB8, 0);

	Mtx_OrthoTilt(&(nt->mtx_top), 0.0, 400.0, 0.0, 240.0, -1.0, 1.0, true);
	Mtx_OrthoTilt(&(nt->mtx_bottom), 0.0, 320.0, 0.0, 240.0, -1.0, 1.0, true);

	nt->display_buf_w = next_power_of_two(w);
	nt->display_buf_h = next_power_of_two(h);
	nt->display_buf = linearAlloc(nt->display_buf_w * nt->display_buf_h * 3);

	C3D_TexInit(&(nt->tex_display), next_power_of_two(w), next_power_of_two(h), GPU_RGB8);
	C3D_TexSetFilter(&(nt->tex_display), GPU_LINEAR, GPU_LINEAR);
	n3ds_open_keyboard(nt);

 	texEnv = C3D_GetTexEnv(0);
	C3D_TexEnvSrc(texEnv, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, 0);
	C3D_TexEnvOp(texEnv, C3D_Both, 0, 0, 0);
	C3D_TexEnvFunc(texEnv, C3D_Both, GPU_MODULATE);

	C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);

	nt->shader_dvlb = DVLB_ParseFile((u32 *) n3ds_shader_shbin, n3ds_shader_shbin_length);
	shaderProgramInit(&(nt->shader_program));
	shaderProgramSetVsh(&(nt->shader_program), &(nt->shader_dvlb->DVLE[0]));
	nt->shader_proj_mtx_loc = shaderInstanceGetUniformLocation(nt->shader_program.vertexShader, "projection");
	AttrInfo_Init(&(nt->shader_attr));

	AttrInfo_AddLoader(&(nt->shader_attr), 0, GPU_FLOAT, 3); // v0 = position
	AttrInfo_AddLoader(&(nt->shader_attr), 1, GPU_FLOAT, 3); // v1 = texcoord

	C3D_BindProgram(&(nt->shader_program));
	C3D_SetAttrInfo(&(nt->shader_attr));

	return (0);
}

static
int n3ds_close (n3ds_t *nt)
{
	linearFree(nt->display_buf);

	C3D_TexDelete(&(nt->tex_display));
	C3D_TexDelete(&(nt->tex_keyboard));

	C3D_RenderBufDelete(&(nt->scr_top));
	C3D_RenderBufDelete(&(nt->scr_bottom));

	C3D_Fini();

	gfxExit();
	romfsExit();
	
	return (0);
}

static
int n3ds_set_msg_trm (n3ds_t *nt, const char *msg, const char *val)
{
	if (strcmp (msg, "term.grab") == 0) {
		return (0);
	}
	else if (strcmp (msg, "term.release") == 0) {
		return (0);
	}
	else if (strcmp (msg, "term.title") == 0) {
		return (0);
	}
	else if (strcmp (msg, "term.fullscreen.toggle") == 0) {
		return (0);
	}
	else if (strcmp (msg, "term.fullscreen") == 0) {
		return (0);
	}

	return (-1);
}

static
void n3ds_update (n3ds_t *vt)
{
	float xmin, ymin, xmax, ymax, txmin, tymin, txmax, tymax;
	int i;
	unsigned char *buf1, *buf2;
	unsigned sdtFlags;

	if (next_power_of_two(vt->trm.w) != vt->display_buf_w
		|| next_power_of_two(vt->trm.h) != vt->display_buf_h)
	{
		n3ds_resize(vt, vt->trm.w, vt->trm.h);
	}

	xmin = 0; ymin = 0; xmax = 400; ymax = 240;
	txmin = 0;
	txmax = ((float) vt->trm.w / vt->display_buf_w);
	tymin = 1.0f - ((float) vt->trm.h / vt->display_buf_h);
	tymax = 1;

	buf1 = (unsigned char*) vt->trm.buf;
	buf2 = (unsigned char*) vt->display_buf;
	for (i = 0; i < vt->trm.h; i++) {
		memcpy(buf2, buf1, vt->trm.w * 3);
		buf1 += vt->trm.w * 3;
		buf2 += vt->display_buf_w * 3;
	}

	GSPGPU_FlushDataCache(vt->display_buf, vt->display_buf_w * vt->display_buf_h * 3);
	sdtFlags = (GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(1) | GX_TRANSFER_RAW_COPY(0) |
                GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGB8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) |
                GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO));

	GX_DisplayTransfer(vt->display_buf, GX_BUFFER_DIM(vt->display_buf_w, vt->display_buf_h), 
		vt->tex_display.data, GX_BUFFER_DIM(vt->tex_display.width,
		vt->tex_display.height), sdtFlags);

	gspWaitForPPF();

	C3D_RenderBufClear(&(vt->scr_top));
	C3D_RenderBufBind(&(vt->scr_top));
	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, vt->shader_proj_mtx_loc, &(vt->mtx_top));

	C3D_TexBind(0, &(vt->tex_display));
	C3D_ImmDrawBegin(GPU_TRIANGLE_STRIP);
                C3D_ImmSendAttrib(xmin, ymin, 0.0f, 0.0f);
                C3D_ImmSendAttrib(txmin, tymin, 1.0f, 0.0f);

                C3D_ImmSendAttrib(xmax, ymin, 0.0f, 0.0f);
                C3D_ImmSendAttrib(txmax, tymin, 1.0f, 0.0f);

                C3D_ImmSendAttrib(xmin, ymax, 0.0f, 0.0f);
                C3D_ImmSendAttrib(txmin, tymax, 1.0f, 0.0f);

                C3D_ImmSendAttrib(xmax, ymax, 0.0f, 0.0f);
                C3D_ImmSendAttrib(txmax, tymax, 1.0f, 0.0f);
	C3D_ImmDrawEnd();

	C3D_RenderBufClear(&(vt->scr_bottom));
	C3D_RenderBufBind(&(vt->scr_bottom));
	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, vt->shader_proj_mtx_loc, &(vt->mtx_bottom));

	n3ds_draw_texture(&(vt->tex_keyboard), 0, 0, 0, 0, 320, 240, 1.0f);
	for (i = 0; i < vt->keys_down_count; i++)
	{
		n3ds_draw_texture(&(vt->tex_keyboard), vt->keys_down[i]->x, 240 - (vt->keys_down[i]->y + vt->keys_down[i]->h),
			vt->keys_down[i]->x, 240 - (vt->keys_down[i]->y + vt->keys_down[i]->h - 1),
			vt->keys_down[i]->w, vt->keys_down[i]->h - 1, 0.5f);
	}

	C3D_Flush();

        C3D_RenderBufTransfer(&(vt->scr_top), (u32*) gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL),
                GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGB8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8));
        C3D_RenderBufTransfer(&(vt->scr_top), (u32*) gfxGetFramebuffer(GFX_TOP, GFX_RIGHT, NULL, NULL),
                GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGB8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8));
	C3D_RenderBufTransfer(&(vt->scr_bottom), (u32*) gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL),
		GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGB8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8));
        gfxSwapBuffersGpu();

//        gspWaitForEvent(GSPGPU_EVENT_VBlank0, false);
}

static
void n3ds_event_mouse_button_check (n3ds_t *vt, unsigned shift, unsigned mask, unsigned key_down, unsigned key_up)
{
	if (key_down & mask)
		vt->button |= 1U << shift;
	else if (key_up & mask)
		vt->button &= ~(1U << shift);
}

static inline
bool n3ds_key_touched(touchPosition* pos, touch_area_t* area)
{
  return pos->px >= area->x && pos->py >= area->y
    && pos->px < (area->x + area->w)
    && pos->py < (area->y + area->h);
}

static
void n3ds_check (n3ds_t *vt)
{
	unsigned key_down, key_up, old_button, changed;
	u64 button_check;
	float circle_mul;
	touchPosition touch;
	touch_area_t* area;
	circlePosition circle;
	int i;

	changed = 0;
	old_button = vt->button;
	button_check = osGetTime();

	hidScanInput();
	hidCircleRead(&circle);
	key_down = hidKeysDown();
	key_up = hidKeysUp();

	if (!aptMainLoop() || (key_down & KEY_START) != 0)
		trm_set_msg_emu (&vt->trm, "emu.exit", "1");

	if (vt->trm.set_mouse != NULL)
	{
		circle_mul = (float) (button_check - vt->button_last_check) / 400.0f;
		n3ds_event_mouse_button_check(vt, 0, KEY_A, key_down, key_up);
		n3ds_event_mouse_button_check(vt, 1, KEY_B, key_down, key_up);
		n3ds_event_mouse_button_check(vt, 2, KEY_X, key_down, key_up);
		n3ds_event_mouse_button_check(vt, 2, KEY_Y, key_down, key_up);

		if (circle.dx <= N3DS_CIRCLE_DEAD_ZONE && circle.dx >= -N3DS_CIRCLE_DEAD_ZONE)
			circle.dx = 0;
		if (circle.dy <= N3DS_CIRCLE_DEAD_ZONE && circle.dy >= -N3DS_CIRCLE_DEAD_ZONE)
			circle.dy = 0;
		if (circle.dx != 0 || circle.dy != 0 || vt->button != old_button)
		{
			trm_set_mouse(&vt->trm, round(circle.dx * circle_mul),
				round(-circle.dy * circle_mul), vt->button);
		}
	}

	if ((key_down | key_up) & KEY_TOUCH)
	{
		hidTouchRead(&touch);

		if (key_down & KEY_TOUCH)
			for(i = 0; i < touch_areas_len; i++)
			{
				area = &touch_areas[i];
				if (n3ds_key_touched(&touch, area))
				{
					trm_set_key(&vt->trm, PCE_KEY_EVENT_DOWN, area->keycode);
					vt->keys_down[vt->keys_down_count++] = area;
					changed = 1;
					break;
				}
			}
		else if (key_up & KEY_TOUCH && vt->keys_down_count > 0)
			if ((vt->keys_down[vt->keys_down_count - 1]->flags & TOUCH_AREA_MODIFIER) == 0)
			{
				for (; vt->keys_down_count > 0; vt->keys_down_count--)
					trm_set_key(&vt->trm, PCE_KEY_EVENT_UP,
						vt->keys_down[vt->keys_down_count - 1]->keycode);
				changed = 1;
			}

		if (changed != 0)
			n3ds_update(vt);
	}

	vt->button_last_check = button_check;
}

static
void n3ds_init (n3ds_t *nt, ini_sct_t *ini)
{
	int i;

	trm_init (&nt->trm, nt);

	nt->trm.del = (void *) n3ds_del;
	nt->trm.open = (void *) n3ds_open;
	nt->trm.close = (void *) n3ds_close;
	nt->trm.set_msg_trm = (void *) n3ds_set_msg_trm;
	nt->trm.update = (void *) n3ds_update;
	nt->trm.check = (void *) n3ds_check;

	nt->button = 0;
	nt->button_last_check = osGetTime();

	for (i = 0; i < MAX_KEYS_DOWN; i++)
		nt->keys_down[i] = PCE_KEY_NONE;
	nt->keys_down_count = 0;
}

terminal_t *n3ds_new (ini_sct_t *ini)
{
	n3ds_t *nt;

	nt = malloc (sizeof (n3ds_t));
	if (nt == NULL) {
		return (NULL);
	}

	n3ds_init (nt, ini);

	return (&nt->trm);
}
