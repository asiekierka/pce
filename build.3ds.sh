#!/bin/sh
make -j4
FLAGS3DSX=--romfs=romfs/

smdhtool --create "PCE - Atari ST" "PCE emulator" "PCE" pce-3ds-icon.png pce-atarist.smdh
smdhtool --create "PCE - IBM PC" "PCE emulator" "PCE" pce-3ds-icon.png pce-ibmpc.smdh
smdhtool --create "PCE - Macintosh" "PCE emulator" "PCE" pce-3ds-icon.png pce-macplus.smdh
smdhtool --create "PCE - RC759" "PCE emulator" "PCE" pce-3ds-icon.png pce-rc759.smdh

3dsxtool src/arch/atarist/pce-atarist pce-atarist.3dsx $FLAGS3DSX --smdh=pce-atarist.smdh
3dsxtool src/arch/ibmpc/pce-ibmpc pce-ibmpc.3dsx $FLAGS3DSX --smdh=pce-ibmpc.smdh
3dsxtool src/arch/macplus/pce-macplus pce-macplus.3dsx $FLAGS3DSX --smdh=pce-macplus.smdh
3dsxtool src/arch/rc759/pce-rc759 pce-rc759.3dsx $FLAGS3DSX --smdh=pce-rc759.smdh
