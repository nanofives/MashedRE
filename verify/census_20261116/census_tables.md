Distinct functions executed (stage `anchor`): **2199** of 6445 counted; 1026 ran before the exe's stop at `0x005aa3d0`.

### Phase x RE status (distinct RVAs; an RVA counts in every phase it ran in)

| phase | C4:match | C4:libid | C4:frida-replay | C4 | C3 | C2 | C1 | none | total |
|---|---|---|---|---|---|---|---|---|---|
| boot | 940 | 297 | 7 | 6 | 25 | 268 | 43 | 22 | 1608 |
| audio-init | 233 | 22 | 7 | 0 | 21 | 63 | 12 | 10 | 368 |
| menu | 184 | 44 | 5 | 6 | 15 | 21 | 4 | 2 | 281 |
| race-load | 713 | 133 | 11 | 14 | 39 | 220 | 35 | 14 | 1179 |
| race-tick | not measured | | | | | | | | |

### Phase x top-down binding

| phase | matched | replay | admitted | provisional | stand-in | under-stand-in | hand-port | exe-body | shim | rw-lib | crt-lib | missing | stop | unbound | total |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| boot | 422 | 4 | 1 | 8 | 41 | 7 | 18 | 60 | 19 | 53 | 17 | 5 | 7 | 946 | 1608 |
| audio-init | 133 | 3 | 8 | 11 | 6 | 0 | 2 | 11 | 2 | 8 | 8 | 1 | 12 | 163 | 368 |
| menu | 116 | 4 | 1 | 2 | 5 | 6 | 9 | 19 | 0 | 6 | 3 | 0 | 3 | 107 | 281 |
| race-load | 215 | 4 | 1 | 4 | 9 | 1 | 11 | 32 | 9 | 33 | 11 | 1 | 9 | 839 | 1179 |
| race-tick | not measured | | | | | | | | | | | | | | |

### First phase x binding (each RVA once, in the first phase it ran in)

| phase | matched | replay | admitted | provisional | stand-in | under-stand-in | hand-port | exe-body | shim | rw-lib | crt-lib | missing | stop | unbound | total |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| boot | 422 | 4 | 1 | 8 | 41 | 7 | 18 | 60 | 19 | 53 | 17 | 5 | 7 | 946 | 1608 |
| audio-init | 69 | 2 | 8 | 11 | 0 | 0 | 0 | 1 | 0 | 0 | 0 | 0 | 10 | 96 | 197 |
| menu | 17 | 0 | 0 | 0 | 0 | 0 | 0 | 7 | 0 | 0 | 0 | 0 | 0 | 6 | 30 |
| race-load | 18 | 0 | 0 | 0 | 1 | 1 | 1 | 3 | 1 | 1 | 0 | 0 | 1 | 337 | 364 |
| race-tick | not measured | | | | | | | | | | | | | | |

### race-load: functions not on a matched / replay / admitted / CRT binding, ranked by race-load calls

Counts by binding: unbound 839, rw-lib 33, exe-body 32, hand-port 11, stop 9, stand-in 9, shim 9, provisional 4, missing 1, under-stand-in 1

- **provisional** (4): `0x005a6f30` FUN_005a6f30 104, `0x005a6ea0` FUN_005a6ea0 104, `0x005aa0c0` AudioTreeWalkPredicateSearch 72, `0x005ae800` AudioPoolBlockAlloc 36
- **stand-in** (9): `0x0055deb0` RwpWorldSolverHandle_RegAbi 650, `0x00556e90` FUN_00556e90 208, `0x004967e0` FUN_004967e0 105, `0x00429290` FUN_00429290 104, `0x004d8480` RwErrSlotWrite 78, `0x004963e0` ConfigLogError 7, `0x004b4080` FUN_004b4080 5, `0x004950b0` QpcTimeScaledTo3Mhz 3, `0x005c4ad0` FUN_005c4ad0 1
- **under-stand-in** (1): `0x0042f510` Vehicle0HandleGet 3
- **missing** (1): `0x00428450` FUN_00428450 104
- **stop** (9): `0x004ccef0` FUN_004ccef0 5647, `0x004a34b0` _strncpy 349, `0x0045dd60` FUN_0045dd60 104, `0x0045d7a0` FUN_0045d7a0 104, `0x005a75b0` FUN_005a75b0 104, `0x0057c210` RwpBodyTableLookup 72, `0x005aead0`  4, `0x005a7b60` FUN_005a7b60 1, `0x00551460` FUN_00551460 1
- **hand-port** (11): `0x004b302f` __stricmp 7943, `0x004a2be9` __security_check_cookie 1090, `0x0042b930` MenuAlphaGet 837, `0x004c3ac0` Vec3Magnitude 154, `0x0042f6a0` GetRaceSubMode 131, `0x00494ef0` ThunkVideoStateGet 104, `0x0042b8d0` StatePhaseIsIdle 104, `0x0040e340` GetLiveCarCount 104, `0x0047b9b0` FUN_0047b9b0 2, `0x004935d0` FUN_004935d0 2, `0x004927c0` FUN_004927c0 1
- **shim** (9): `0x00550b00` VfsFileExists 383, `0x004b5350` thunk_FUN_00550b00 383, `0x004e6920` FUN_004e6920 260, `0x004cc230` FUN_004cc230 49, `0x004cc160` FUN_004cc160 49, `0x004b3f90` FUN_004b3f90 20, `0x0054f8d0` FUN_0054f8d0 2, `0x00534b60` FUN_00534b60 2, `0x004e4dd0` RpLightCreate 2
- **rw-lib** (33): `0x004cbd30` RwStreamRead 22561, `0x004cc5e0` FUN_004cc5e0 7761, `0x004d8060` RwPluginListDestructDispatch 964, `0x004c0ed0` FUN_004c0ed0 848, `0x004e4440` FUN_004e4440 690, `0x004b5190` FUN_004b5190 647, `0x004c0b30` FUN_004c0b30 529, `0x004c5cb0` FUN_004c5cb0 477, `0x004c1040` FUN_004c1040 461, `0x004e69a0` FUN_004e69a0 322, `0x0053fbb0` FUN_0053fbb0 196, `0x004c39b0` RwV3dNormalize 144, `0x004c2b80` FUN_004c2b80 104, `0x004c7730` FUN_004c7730 104, `0x004c2bb0` FUN_004c2bb0 104 ...
- **exe-body** (32): `0x004cca80` FUN_004cca80 3152, `0x00561f40` FUN_00561f40 1644, `0x005cb2a0` RwDeviceMatrixMultiply 1456, `0x004d8810` RwNativeRasterRead 1082, `0x004c51a0` RwMatrixTranslate 832, `0x004c5010` RwMatrixScale 728, `0x004e8120` FUN_004e8120 499, `0x004c4220` FUN_004c4220 491, `0x004cf5a0` FUN_004cf5a0 475, `0x00450b10` HudIm2DQuad 416, `0x004f3e90` FUN_004f3e90 389, `0x004e8c70` FUN_004e8c70 388, `0x004e8a10` FUN_004e8a10 388, `0x004e9330` FUN_004e9330 386, `0x004e7060` RpAtomicStreamRead 386 ...
- **unbound** (839; by status C4:match 448, C2 187, C4:libid 122, C1 29, C3 26, none 13, C4 11, C4:frida-replay 3), top 20: `0x0053e6d0` C1 third-party-library[renderware] 53004, `0x004df8a0` C4:match third-party-library[renderware] 28080, `0x004d54f0` C4:match render 26832, `0x004d7480` C4:match render 26832, `0x004b5380` C4:match render 22563, `0x004b6cd0` none  22480, `0x005a6670` C4:match audio 20800, `0x004d5480` C4:match render 13208, `0x004a5984` C2 boot 9476, `0x004a59bf` C4:libid third-party-library[msvc-crt-main] 9476, `0x004a8828` C4:libid third-party-library[msvc-crt-main] 8461, `0x004b2eb0` C2 third-party-library[msvc-crt-main] 7943, `0x004b6b10` C2 third-party-library[msvc-crt] 7943, `0x004d6ce0` C4:match render 6344, `0x004d53b0` C4:match render 6344, `0x004d6e60` C4:match render 6240, `0x004d7100` C4:match render 6240, `0x004cb130` C4:match render 6240, `0x004dcbb0` C4:match third-party-library[renderware] 6240, `0x004a45cf` C4:libid third-party-library[msvc-crt-main] 6167

Unbound by subsystem (race-load): third-party-library[renderware] 163, render 142, third-party-library[lua-4.0] 130, third-party-library[RenderWare-Physics-3.7] 76, util 49, third-party-library[msvc-crt-main] 45, track 41, audio 40, third-party-library[directshow-baseclasses] 22, frontend 20, third-party-library[libpng-1.0.12] 19, gameplay 16

### Audio init window

Stops remaining before `0x0045d460`: **65**; window binding unbound 57, matched 25, stop 8, replay 2, provisional 2
