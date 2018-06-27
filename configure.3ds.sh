#!/bin/sh
ARCH="-march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft"
INCLUDES="-I$DEVKITPRO/libctru/include"
LIBS="-L$DEVKITPRO/libctru/lib -lcitro3d -lctru -lm"

./configure --host=arm-none-eabi --without-sdl --without-x --disable-utils --with-n3ds \
  --disable-cpm80 --disable-sim405 --disable-sim6502 --disable-sims32 --disable-simarm \
  CFLAGS="-g -Wall -O2 -mword-relocations -fomit-frame-pointer -ffunction-sections -D_GNU_SOURCE -DARM11 -D_3DS $ARCH $INCLUDES" \
  LDFLAGS="-specs=$DEVKITARM/arm-none-eabi/lib/3dsx.specs -g $ARCH" \
  LIBS="$LIBS"
