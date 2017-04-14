/*****************************************************************************
 * pce                                                                       *
 *****************************************************************************/

/*****************************************************************************
 * File name:   src/drivers/sound/sound-n3ds.c                               *
 * Created:     2017-04-11 by Adrian Siekierka <kontakt@asie.pl>             *
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
#include <stdio.h>
#include <string.h>

#include <3ds.h>

#include <drivers/sound/sound.h>
#include <drivers/sound/sound-n3ds.h>


static inline
void snd_n3ds_cleanup (sound_drv_t *sdrv)
{
	int i;
	sound_n3ds_t *drv;

	drv = sdrv->ext;

	for (i = 0; i < N3DS_AUDIO_BUF_COUNT; i++)
	{
		if (drv->buf[i].data_vaddr != NULL && (drv->buf[i].status == NDSP_WBUF_DONE))
		{
			linearFree(drv->buf[i].data_vaddr);
			drv->buf[i].data_vaddr = NULL;
			drv->buf[i].status = NDSP_WBUF_FREE;
		}
	}
}

static
void snd_n3ds_close (sound_drv_t *sdrv)
{
	int i;
	sound_n3ds_t *drv;

	drv = sdrv->ext;

	ndspChnWaveBufClear(0);
	ndspExit();

	for (i = 0; i < N3DS_AUDIO_BUF_COUNT; i++)
	{
		if (drv->buf[i].data_vaddr != NULL)
			linearFree(drv->buf[i].data_vaddr);
	}

	free (drv);
}

static
int snd_n3ds_write (sound_drv_t *sdrv, const uint16_t *buf, unsigned cnt)
{
	int i;
	uint16_t *buf2;
	sound_n3ds_t *drv;

	drv = sdrv->ext;

	snd_n3ds_cleanup(sdrv);
	for (i = 0; i < N3DS_AUDIO_BUF_COUNT; i++)
	{
		if (drv->buf[i].status == NDSP_WBUF_FREE)
		{
			drv->buf[i].nsamples = cnt;
			drv->buf[i].data_vaddr = linearAlloc(2 * sdrv->channels * cnt);
			if (!drv->sign)
			{
				buf2 = drv->buf[i].data_vaddr;
				for (int i = 0; i < sdrv->channels * cnt; i++)
					buf2[i] = buf[i] ^ 0x8000;
			}
			else
			{
				memcpy(drv->buf[i].data_vaddr, buf, 2 * sdrv->channels * cnt);
			}
			DSP_FlushDataCache(drv->buf[i].data_vaddr, 2 * sdrv->channels * cnt);
			ndspChnWaveBufAdd(0, &(drv->buf[i]));
			return (0);
		}
	}

	fprintf(stderr, "sound-n3ds: buffer overrun\n");
	return (1);
}

static
int snd_n3ds_set_params (sound_drv_t *sdrv, unsigned chn, unsigned long srate, int sign)
{
	sound_n3ds_t *drv;

	drv = sdrv->ext;

	drv->sign = sign;
	ndspChnReset(0);
	ndspChnSetInterp(0, NDSP_INTERP_LINEAR);
	ndspChnSetRate(0, srate);
	ndspChnSetFormat(0, NDSP_CHANNELS(chn) | NDSP_ENCODING(NDSP_ENCODING_PCM16));
	ndspChnSetMix(0, drv->mix);

	return (0);
}

static
int snd_n3ds_init (sound_n3ds_t *drv, const char *name)
{
	snd_init (&drv->sdrv, drv);

	drv->sdrv.close = snd_n3ds_close;
	drv->sdrv.write = snd_n3ds_write;
	drv->sdrv.set_params = snd_n3ds_set_params;

	memset(&drv->buf, 0, sizeof(ndspWaveBuf) * N3DS_AUDIO_BUF_COUNT);
	memset(&drv->mix, 0, sizeof(float) * 12);
	drv->mix[0] = 1.0f;
	drv->mix[1] = 1.0f;

	return (0);
}

sound_drv_t *snd_n3ds_open (const char *name)
{
	sound_n3ds_t *drv;

	drv = malloc (sizeof (sound_n3ds_t));

	if (drv == NULL) {
		return (NULL);
	}

	if (snd_n3ds_init (drv, name)) {
		free (drv);
		return (NULL);
	}

	ndspInit();
	ndspSetOutputMode(NDSP_OUTPUT_STEREO);
	ndspSetOutputCount(1);
	ndspSetMasterVol(1.0f);

	return (&drv->sdrv);
}
