/*****************************************************************************
 * pce                                                                       *
 *****************************************************************************/

/*****************************************************************************
 * File name:   src/drivers/sound/sound-n3ds.h                               *
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


#ifndef PCE_DRIVERS_SOUND_N3DS_H
#define PCE_DRIVERS_SOUND_N3DS_H 1


#include <drivers/sound/sound.h>


#define N3DS_AUDIO_BUF_COUNT 32

typedef struct sound_n3ds_t {
	sound_drv_t sdrv;

	ndspWaveBuf buf[N3DS_AUDIO_BUF_COUNT];
	float mix[12];
	int sign;
} sound_n3ds_t;


#endif
