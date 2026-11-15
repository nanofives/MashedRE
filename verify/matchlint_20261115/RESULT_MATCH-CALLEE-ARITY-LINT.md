# RESULT MATCH-CALLEE-ARITY-LINT

Verdict: GREEN (report-only lint, text row, no build, no gates, no edits under re/match).

Tool: `py -3.12 re/tools/match/callee_arity_lint.py --out <md> --tsv <tsv>` (new file `re/tools/match/callee_arity_lint.py`, text-only).
It scans every `re/match/**/*.c` (4273 sources, 4763 distinct callees `FUN_xxxxxxxx`), reads each prototype/definition head as written, and groups by callee RVA.
A callee is ARITY when two sources declare different parameter counts (`()` unspecified is not counted, `(void)` is 0, `...` is not counted), PROTO when counts agree but return or parameter types differ.
Reference = the definition in the callee own source `re/match/<dir>/<rva>.c`; tagged `c4=match` when `hooks.csv` notes for that RVA carry `c4=match`.

## Totals

- ARITY callees: 401. With an own-source reference: 379. With a `c4=match` reference: 288.
- PROTO-only callees (same count, different types/return): 1971 (mostly `int` vs `void*` vs `uint` spellings of one parameter, listed in `lint.tsv`, not triaged).
- Table below: ARITY rows with a reference, where the other source declares a different count than the reference: 374 callees (285 with `c4=match` reference).
- Full per-callee listing (every declaring source, line, declared prototype): regenerate with the command above (`lint_full.md` is 900 KB and not committed). Summary per callee: `verify/matchlint_20261115/lint.tsv`.

## Known case cross-check

Pre-fix `re/match/s28/005c47e0.c` (commit 4d1137460^, line 38) declares `int FUN_004c1040(void);` (n=0, call at line 120 with no arguments).
Reference `re/match/rwl01/004c1040.c:42` defines `int FUN_004c1040(int,int)` (n=2). The lint scan of the pre-fix file returns n=0 for `004c1040`, so the analogue class (`(void)` declared where the callee own source takes N>0) is flagged: ARITY entries such as 0x004058c0 (ref n=2, `s01/raw/004058e0.c:37` declares `void()`), 0x0040acd0, 0x0040dbd0 below.
The original side (pushes at 0x005c49f5 / 0x005c49fd, call at 0x005c4a00) is taken from the row text, not re-read here: [UNCERTAIN] until disassembled. The current `s28/005c47e0.c` already declares (int,int) (PROTO-only vs rwl01 types `int,int`, no ARITY now).

## Mismatch table (reference callee, declared arity in the offending source)

| callee RVA | reference source:line (n) | ref tag | offending source:line declared prototype (n) |
|---|---|---|---|
| 0x004058c0 | re/match/s01/004058c0.c:7 (2) | c4=match | re/match/s01/raw/004058e0.c:37 `void()` (0) |
| 0x0040acd0 | re/match/s02/0040acd0.c:39 (0) | c4=match | re/match/s08/0043dfd0.c:187 `void(float)` (1) |
| 0x0040ae30 | re/match/s02/0040ae30.c:39 (1) | c4=match | re/match/s02/0040b090.c:43 `void(int,float)` (2) |
| 0x0040dbd0 | re/match/s02/0040dbd0.c:42 (0) | c4=match | re/match/t01g/004111c0.c:88 `void(uint)` (1) |
| 0x00418560 | re/match/racepath/00418560.c:68 (1) | c4=match | re/match/s03/h3_00418860.c:11 `void(int,int)` (2) |
| 0x0041c380 | re/match/s04/0041c380.c:48 (1) | c4=match | re/match/s04/0041cb80.c:34 `void()` (0) |
| 0x0041e8f0 | re/match/s04/0041e8f0.c:32 (1) | c4=match | re/match/s05/00426030.c:31 `void()` (0) |
| 0x0041ebb0 | re/match/s04/0041ebb0.c:34 (0) | c4=match | re/match/s02/00410b30.c:48 `void(undefined4)` (1) |
| 0x00422570 | re/match/s05/00422570.c:84 (4) | c4=match | re/match/s05/0041f8f0.c:73 `void(unsignedint,char*,int)` (3) |
| 0x00426070 | re/match/s06/00426070.c:30 (0) | c4=match | re/match/s05/0041f8f0.c:72 `int(float)` (1) |
| 0x00426080 | re/match/s06/00426080.c:29 (0) | c4=match | re/match/s02/0040da50.c:43 `undefined4(int)` (1) |
| 0x00426bb0 | re/match/s06/00426bb0.c:29 (0) | c4=match | re/match/s08/00446520.c:119 `int(int)` (1) |
| 0x00426c50 | re/match/s06/00426c50.c:32 (1) | c4=match | re/match/s02/0040ddb0.c:32 `void()` (0) |
| 0x00428a30 | re/match/s06/00428a30.c:56 (0) | c4=match | re/match/s06/00429290.c:50 `void(int,int)` (2) |
| 0x00428bf0 | re/match/s06/00428bf0.c:48 (0) | c4=match | re/match/s06/00429290.c:51 `void(int,int)` (2) |
| 0x00428d30 | re/match/s06/00428d30.c:62 (0) | c4=match | re/match/s06/00429290.c:52 `void(int,int)` (2) |
| 0x00429290 | re/match/s06/00429290.c:75 (2) | c4=match | re/match/s01/00402750.c:43 `int()` (0)<br>re/match/s10/004669b0.c:50 `void(char*,char*,int)` (3)<br>re/match/topm01/004669b0.c:54 `void(char*,char*,int)` (3) |
| 0x0042a5d0 | re/match/s06/0042a5d0.c:39 (1) | c4=match | re/match/pilot/a3_00456760.c:18 `void*(char*,int,int)` (3)<br>re/match/pilot/a6_00419090.c:23 `int(char*,int,int)` (3)<br>re/match/s03/h1_00418a30.c:82 `int(char*,int,int)` (3)<br>re/match/s03/h1_0041a1e0.c:22 `int(char*,int,int)` (3)<br>re/match/s03/h1_0041a4a0.c:61 `int(char*,int,int)` (3)<br>re/match/s03/h1_0041ad60.c:18 `int(char*,int,int)` (3)<br>+24 more (lint.tsv) |
| 0x0042a640 | re/match/s06/0042a640.c:39 (1) | c4=match | re/match/s12/00479330.c:45 `int(char*,int,int)` (3) |
| 0x0042a6b0 | re/match/s06/0042a6b0.c:41 (1) | c4=match | re/match/s02/0040bbb0.c:42 `int(char*,int,int)` (3)<br>re/match/s03/h1_00418a30.c:83 `int(char*,int,int)` (3)<br>re/match/s03/h1_grp_0041ad60.c:54 `int(char*,int,int)` (3)<br>re/match/s04/0041c320.c:41 `int(char*,int,int)` (3)<br>re/match/s05/00420420.c:99 `int(char*,int,int)` (3)<br>re/match/s06/00427ca0.c:45 `int(char*,int,int)` (3)<br>+2 more (lint.tsv) |
| 0x0042a7f0 | re/match/s06/0042a7f0.c:39 (1) | c4=match | re/match/s12/00479330.c:48 `int(char*,int,int)` (3) |
| 0x0042a860 | re/match/s06/0042a860.c:39 (1) | c4=match | re/match/s12/00479330.c:49 `int(char*,int,int)` (3) |
| 0x00431b80 | re/match/s07/00431b80.c:41 (3) | c4=match | re/match/s08/0043dfd0.c:226 `void(int)` (1) |
| 0x00432450 | re/match/s08/00432450.c:38 (0) | c4=match | re/match/s08/004324a0.c:39 `void(int,int,int,int,int,int)` (6) |
| 0x00451060 | re/match/s08/00451060.c:30 (0) | c4=match | re/match/s08/004492b0.c:33 `void(int)` (1) |
| 0x00451820 | re/match/s09/00451820.c:36 (1) | c4=match | re/match/s09/00451d80.c:53 `void(S_00451d80*,uint)` (2) |
| 0x00451ad0 | re/match/s09/00451ad0.c:47 (1) | c4=match | re/match/s09/00451d80.c:54 `void(S_00451d80*,uint)` (2) |
| 0x004522d0 | re/match/s09/004522d0.c:28 (1) | c4=match | re/match/g2seq/005516e0.c:38 `void()` (0)<br>re/match/g2sh/00533840.c:38 `void()` (0)<br>re/match/s26/005ae4b0.c:25 `void()` (0)<br>re/match/s26/005afcf0.c:40 `void()` (0)<br>re/match/s27/005b3550.c:28 `void(,...)` (0)<br>re/match/s27/005b3550.c:30 `void()` (0)<br>+4 more (lint.tsv) |
| 0x00455fe0 | re/match/s09/00455fe0.c:41 (1) | c4=match | re/match/s09/00456c70.c:29 `void()` (0) |
| 0x00459400 | re/match/s09/00459400.c:39 (1) | c4=match | re/match/racepath/00412620.c:56 `int(int,int)` (2)<br>re/match/s09/0045a190.c:86 `int(int,int)` (2) |
| 0x0045b350 | re/match/s30/0045b350.c:34 (0) | c4=match | re/match/s11/00473c20.c:54 `void(int)` (1)<br>re/match/t01g/004111c0.c:135 `void(int)` (1) |
| 0x0045bfe0 | re/match/s10/0045bfe0.c:29 (0) | c4=match | re/match/s01/00405920.c:56 `int(RwLine*,void*)` (2)<br>re/match/s01/00406160.c:55 `int(void*,void*)` (2)<br>re/match/s09/00454350.c:56 `int(Seg18*,Hit3c*)` (2)<br>re/match/s09/00459620.c:88 `int(Line_459620*,Hit_459620*)` (2)<br>re/match/s12/00475210.c:33 `int(Vec3*,Info40*)` (2) |
| 0x0045cbe0 | re/match/s10/0045cbe0.c:52 (2) | c4=match | re/match/s10/0045cc50.c:28 `void(undefined4,undefined4,undefined4)` (3)<br>re/match/s10/0045cfe0.c:34 `void(int,int,int)` (3)<br>re/match/s10/0045d080.c:30 `void(int,int,int)` (3)<br>re/match/s10/0045d1e0.c:31 `void(int,int,int)` (3) |
| 0x00463150 | re/match/s10/00463150.c:38 (0) | c4=match | re/match/s10/004656e0.c:33 `void(int)` (1) |
| 0x00466b50 | re/match/s10/00466b50.c:110 (0) | c4=match | re/match/s06/00429290.c:48 `void(int)` (1)<br>re/match/s15/004926c0.c:33 `void(int)` (1) |
| 0x0046d400 | re/match/s11/0046d400.c:31 (4) | c4=match | re/match/s03/h4_00412f30.c:77 `float(V3,char*)` (2)<br>re/match/s08/00448220.c:68 `float(V3_00448220,char*)` (2)<br>re/match/s11/004704c0.c:40 `float(V3_004704c0,int)` (2) |
| 0x00472330 | re/match/g2sh/00472330.c:40 (1) | c4=match | re/match/s11/00472380.c:28 `void*(void*,int,int)` (3) |
| 0x00472b10 | re/match/s11/00472b10.c:51 (3) | c4=match | re/match/s07/0042c010.c:52 `void(void*,V2d,V2d,int)` (4) |
| 0x00472c60 | re/match/s11/00472c60.c:55 (2) | c4=match | re/match/s06/00428d30.c:50 `void(S06_V2d,S06_V2d,S06_RGBA)` (3)<br>re/match/s06/00429bd0.c:50 `void(S06_V2d,S06_V2d,S06_RGBA)` (3)<br>re/match/s06/00429e10.c:58 `void(S06_V2d,S06_V2d,S06_RGBA)` (3)<br>re/match/s07/0042bde0.c:27 `void(int,int,int,int,RGBA4)` (5)<br>re/match/s07/0042e3a0.c:40 `void(RwV2d,RwV2d,RwRGBA)` (3)<br>re/match/s07/0042e8b0.c:41 `void(RwV2d,RwV2d,RwRGBA)` (3)<br>+10 more (lint.tsv) |
| 0x00472f40 | re/match/s11/00472f40.c:53 (2) | c4=match | re/match/s06/00428d30.c:51 `void(S06_V2d,S06_V2d,S06_RGBA)` (3)<br>re/match/s07/0042e3a0.c:41 `void(RwV2d,RwV2d,RwRGBA)` (3) |
| 0x004730b0 | re/match/s11/004730b0.c:53 (2) | c4=match | re/match/s06/00428d30.c:52 `void(S06_V2d,S06_V2d,S06_RGBA)` (3)<br>re/match/s07/0042e3a0.c:42 `void(RwV2d,RwV2d,RwRGBA)` (3) |
| 0x00473870 | re/match/s11/00473870.c:50 (5) | c4=match | re/match/s07/00431240.c:59 `void(void*,float,float,float,float,MRgba,int)` (7)<br>re/match/s07/004314b0.c:53 `void(void*,float,float,float,float,MRgba,int)` (7)<br>re/match/s07/00431710.c:60 `void(void*,float,float,float,float,MRgba,int)` (7)<br>re/match/s08/0043af10.c:111 `void(void*,float,float,float,float,uint,int)` (7) |
| 0x004739f0 | re/match/s11/004739f0.c:50 (10) | c4=match | re/match/s07/00431240.c:60 `void(void*,float,float,float,float,MRgba,float,int,int,float,int,int)` (12)<br>re/match/s07/004314b0.c:54 `void(void*,float,float,float,float,MRgba,float,int,int,float,int,int)` (12)<br>re/match/s07/00431710.c:61 `void(void*,float,float,float,float,MRgba,float,int,int,float,int,int)` (12)<br>re/match/s08/004368e0.c:131 `undefined4(undefined4,float,float,float,float,uint,undefined4,undefined4,undefined4,undefined4,int,int)` (12)<br>re/match/s08/0043af10.c:112 `void(void*,float,float,float,float,uint,float,float,float,float,int,int)` (12) |
| 0x00474d60 | re/match/s11/00474d60.c:42 (2) | c4=match | re/match/s09/004595c0.c:35 `void(int)` (1) |
| 0x00476710 | re/match/s12/00476710.c:88 (5) | c4=match | re/match/s12/00476880.c:27 `void()` (0) |
| 0x004767c0 | re/match/s12/004767c0.c:88 (5) | c4=match | re/match/s12/004768a0.c:27 `void()` (0) |
| 0x00477490 | re/match/s12/00477490.c:66 (1) | c4=match | re/match/s12/004777d0.c:30 `void()` (0) |
| 0x00479030 | re/match/s12/00479030.c:35 (1) | c4=match | re/match/s12/004790e0.c:28 `int(int,int)` (2) |
| 0x004790e0 | re/match/s12/004790e0.c:31 (1) | c4=match | re/match/s12/00479330.c:54 `void()` (0) |
| 0x0047b9e0 | re/match/s13/0047b9e0.c:33 (1) | c4=match | re/match/s02/00410b30.c:70 `void(undefined4,int,int)` (3) |
| 0x0047ba00 | re/match/topm01/0047ba00.c:43 (2) | c4=match | re/match/s13/0047ba00.c:32 `void(int)` (1) |
| 0x0047cd50 | re/match/s13/0047cd50.c:30 (2) | c4=match | re/match/s13/00481a30.c:78 `void()` (0) |
| 0x0047d100 | re/match/s13/0047d100.c:28 (1) | c4=match | re/match/s08/0044ab20.c:40 `void(int,int)` (2)<br>re/match/s09/004527f0.c:97 `void(int,int)` (2)<br>re/match/s09/00452af0.c:28 `void(int,int)` (2)<br>re/match/s11/00471ac0.c:81 `void(int,int)` (2)<br>re/match/s11/00471cf0.c:54 `void(int,int)` (2)<br>re/match/s13/00480100.c:46 `void(int,int)` (2)<br>+1 more (lint.tsv) |
| 0x00481750 | re/match/s13/00481750.c:31 (1) | c4=match | re/match/s01/004039f0.c:35 `void(int,RwV3d*,RwV3d*)` (3)<br>re/match/s08/0044e0a0.c:34 `void(int,V3_0044e0a0*,V3_0044e0a0*)` (3)<br>re/match/s09/00451ad0.c:45 `void(int,V3_00451ad0*,V3_00451ad0*)` (3)<br>re/match/s09/0045b390.c:66 `void(int,V3_45b390*,V3_45b390*)` (3) |
| 0x00484310 | re/match/s14fu/00484310.c:69 (2) | c4=match | re/match/racepath/004844a0.c:61 `void(int)` (1) |
| 0x00487e20 | re/match/s14/00487e20.c:28 (3) | c4=match | re/match/s14/0048a130.c:87 `int(void*,void*,int,int)` (4)<br>re/match/s15/00491f00.c:64 `int(int,void*,int,int)` (4) |
| 0x0048a830 | re/match/s14/0048a830.c:33 (0) | c4=match | re/match/s02/0040bbb0.c:53 `void(int)` (1) |
| 0x0048e820 | re/match/s15/0048e820.c:80 (0) | c4=match | re/match/s02/0040bbb0.c:56 `void(int)` (1) |
| 0x00491490 | re/match/s15/00491490.c:30 (0) | c4=match | re/match/s06/00426ab0.c:42 `void(undefined4)` (1) |
| 0x004935a0 | re/match/s15/004935a0.c:31 (1) | c4=match | re/match/s15/00493600.c:32 `void()` (0) |
| 0x004935d0 | re/match/s15/004935d0.c:31 (1) | c4=match | re/match/s15/00493600.c:33 `void()` (0) |
| 0x00493710 | re/match/s15/00493710.c:75 (0) | c4=match | re/match/s15/00492270.c:29 `undefined4(int)` (1) |
| 0x00493c00 | re/match/s15/00493c00.c:35 (4) | c4=match | re/match/s16/004944c0.c:83 `int*__fastcall(void*,float,ArgP4)` (3) |
| 0x00494460 | re/match/s15/00494460.c:32 (0) | c4=match | re/match/s16/00495350.c:35 `void(int)` (1) |
| 0x00494480 | re/match/s15/00494480.c:33 (0) | c4=match | re/match/s16/00495350.c:36 `int(int)` (1) |
| 0x00494ac0 | re/match/s16/00494ac0.c:39 (4) | c4=match | re/match/s16/00494c80.c:82 `int*__fastcall(void*,float,ArgP4)` (3) |
| 0x00494c80 | re/match/s16/00494c80.c:84 (0) | c4=match | re/match/s01/00402750.c:84 `char*(int)` (1) |
| 0x00499710 | re/match/s16/00499710.c:29 (0) | c4=match | re/match/s16/00495350.c:38 `int(int)` (1) |
| 0x00499ba0 | re/match/s16/00499ba0.c:69 (3) | c4=match | re/match/s15/00492370.c:38 `int(int,int,int,int)` (4) |
| 0x0049a0a0 | re/match/s16/0049a0a0.c:31 (0) | c4=match | re/match/s08/00448940.c:52 `void(int)` (1) |
| 0x0049a2e0 | re/match/s16/0049a2e0.c:29 (1) | c4=match | re/match/s16/0049a0a0.c:28 `void()` (0) |
| 0x0049a740 | re/match/s16/0049a740.c:29 (1) | c4=match | re/match/s16/0049a0a0.c:29 `void()` (0) |
| 0x004b3bf0 | re/match/s18/004b3bf0.c:34 (3) | c4=match | re/match/s06/0042a5d0.c:36 `int(char*)` (1) |
| 0x004b3c60 | re/match/s18/004b3c60.c:33 (3) | c4=match | re/match/s06/0042a640.c:36 `int(char*)` (1) |
| 0x004b3cc0 | re/match/s18/004b3cc0.c:34 (3) | c4=match | re/match/s06/0042a7f0.c:36 `int(char*)` (1) |
| 0x004b3d20 | re/match/s18/004b3d20.c:33 (3) | c4=match | re/match/s06/0042a6b0.c:37 `int(char*)` (1) |
| 0x004b3d80 | re/match/s18/004b3d80.c:33 (3) | c4=match | re/match/s06/0042a6b0.c:38 `int(char*)` (1) |
| 0x004b3de0 | re/match/s18/004b3de0.c:33 (3) | c4=match | re/match/s06/0042a860.c:36 `int(char*)` (1) |
| 0x004b4b60 | re/match/s18/004b4b60.c:31 (3) | c4=match | re/match/s12/00475210.c:34 `int(int)` (1) |
| 0x004b4cd0 | re/match/s18/004b4cd0.c:31 (3) | c4=match | re/match/s01/00405920.c:57 `int(int)` (1)<br>re/match/s09/00454350.c:62 `int(int)` (1)<br>re/match/s09/00459620.c:94 `int(int)` (1) |
| 0x004b4d10 | re/match/s18/004b4d10.c:31 (3) | c4=match | re/match/s01/00406160.c:56 `int(int)` (1) |
| 0x004b5240 | re/match/s18/004b5240.c:28 (2) | c4=match | re/match/s13/0047d100.c:26 `void(int)` (1) |
| 0x004c07b0 | re/match/s19/004c07b0.c:39 (3) | c4=match | re/match/s19/004c32b0.c:46 `void()` (0) |
| 0x004c0830 | re/match/s19/004c0830.c:33 (3) | c4=match | re/match/s19/004c32b0.c:47 `void()` (0) |
| 0x004c1940 | re/match/rwl01/004c1940.c:33 (1) | c4=match | re/match/s19/004c32b0.c:48 `void()` (0) |
| 0x004c1980 | re/match/rwl01/004c1980.c:39 (2) | c4=match | re/match/s19/004c32b0.c:49 `void()` (0) |
| 0x004c1be0 | re/match/s19/004c1be0.c:28 (3) | c4=match | re/match/s16/00495350.c:42 `void(int,int)` (2) |
| 0x004c2b80 | re/match/rwta/004c2b80.c:42 (1) | c4=match | re/match/rwl01/004c1d30.c:33 `void()` (0) |
| 0x004c2bb0 | re/match/rwta/004c2bb0.c:44 (1) | c4=match | re/match/rwl01/004c1d30.c:34 `void()` (0) |
| 0x004c4430 | re/match/s19/004c4430.c:35 (3) | c4=match | re/match/s19/004c32b0.c:55 `void()` (0) |
| 0x004c4470 | re/match/s19/004c4470.c:43 (3) | c4=match | re/match/s19/004c32b0.c:56 `void()` (0) |
| 0x004c5a60 | re/match/rwl01/004c5a60.c:38 (1) | c4=match | re/match/rwl01/004c5e00.c:40 `undefined4(int*,int)` (2)<br>re/match/s19/004c5930.c:39 `RwTexture*(RwTexture*,void*)` (2) |
| 0x004c5f60 | re/match/rwl01/004c5f60.c:59 (2) | c4=match | re/match/s19/004c32b0.c:58 `void()` (0) |
| 0x004c6140 | re/match/s19/004c6140.c:29 (3) | c4=match | re/match/rwl01/004c5f60.c:53 `void()` (0) |
| 0x004c78a0 | re/match/rwl01/004c78a0.c:33 (1) | c4=match | re/match/s19/004c32b0.c:59 `void()` (0) |
| 0x004c78e0 | re/match/rwl01/004c78e0.c:39 (2) | c4=match | re/match/s19/004c32b0.c:60 `void()` (0) |
| 0x004cbca0 | re/match/rwl01/004cbca0.c:38 (2) | c4=match | re/match/s19/004c32b0.c:61 `void()` (0) |
| 0x004cbd00 | re/match/rwl01/004cbd00.c:33 (1) | c4=match | re/match/s19/004c32b0.c:62 `void()` (0) |
| 0x004ccba0 | re/match/rwl01/004ccba0.c:38 (2) | c4=match | re/match/s19/004c32b0.c:52 `void()` (0) |
| 0x004cd810 | re/match/rwl01/004cd810.c:34 (1) | c4=match | re/match/s19/004c32b0.c:64 `void()` (0) |
| 0x004cd850 | re/match/rwl01/004cd850.c:38 (2) | c4=match | re/match/s19/004c32b0.c:65 `void()` (0) |
| 0x004cd900 | re/match/rwl01/004cd900.c:48 (2) | c4=match | re/match/s19/004c32b0.c:66 `void()` (0) |
| 0x004cdb60 | re/match/rwl01/004cdb60.c:37 (1) | c4=match | re/match/s19/004c32b0.c:67 `void()` (0) |
| 0x004cf9e0 | re/match/s20/004cf9e0.c:29 (1) | c4=match | re/match/s20/004cf7d0.c:55 `void*(void*,void*)` (2) |
| 0x004d0640 | re/match/g2sh/004d0640.c:39 (2) | c4=match | re/match/s20/004c8e50.c:46 `int(void*,void*,int)` (3) |
| 0x004d0fc0 | re/match/g2sh/004d0fc0.c:39 (2) | c4=match | re/match/s20/004c8e50.c:48 `int(void*,void*,int)` (3) |
| 0x004d0fe0 | re/match/g2sh/004d0fe0.c:39 (2) | c4=match | re/match/s20/004c8e50.c:47 `int(void*,void*,int)` (3) |
| 0x004d1630 | re/match/g2sh/004d1630.c:39 (2) | c4=match | re/match/s20/004c8e50.c:49 `int(void*,void*,int)` (3) |
| 0x004d3450 | re/match/rwtb/004d3450.c:40 (2) | c4=match | re/match/s20/004c8e50.c:54 `int(void*,void*,int)` (3) |
| 0x004d34d0 | re/match/g2sh/004d34d0.c:38 (2) | c4=match | re/match/s20/004c8e50.c:39 `int(void*,void*,int)` (3) |
| 0x004d34e0 | re/match/g2sh/004d34e0.c:38 (2) | c4=match | re/match/s20/004c8e50.c:50 `int(void*,void*,int)` (3) |
| 0x004d7ff0 | re/match/s21/004d7ff0.c:28 (1) | c4=match | re/match/rwl01/004c3e90.c:44 `undefined4(undefined4,undefined4)` (2)<br>re/match/rwl01/004cdd60.c:41 `undefined4(undefined4,undefined4)` (2)<br>re/match/rwl01/004ce5d0.c:33 `undefined4(undefined4,int)` (2)<br>re/match/rwl01/004d4170.c:33 `undefined4(undefined4,undefined4)` (2)<br>re/match/rwl01/004d4910.c:32 `undefined4(undefined4,undefined4)` (2)<br>re/match/rwl01/004d4dd0.c:43 `int(int,uint)` (2)<br>+27 more (lint.tsv) |
| 0x004d8430 | re/match/s21/004d8430.c:32 (2) | c4=match | re/match/s19/004c32b0.c:36 `void()` (0) |
| 0x004d8470 | re/match/s21/004d8470.c:30 (1) | c4=match | re/match/s19/004c32b0.c:37 `void()` (0) |
| 0x004d8530 | re/match/s21/004d8530.c:32 (2) | c4=match | re/match/s19/004c32b0.c:38 `void()` (0) |
| 0x004d8550 | re/match/s21/004d8550.c:30 (1) | c4=match | re/match/s19/004c32b0.c:39 `void()` (0) |
| 0x004d8560 | re/match/s21/004d8560.c:28 (0) | c4=match | re/match/rwl03/0053fbb0.c:37 `undefined4(int,int)` (2)<br>re/match/rwl03/005401d0.c:31 `undefined4(int*,int)` (2)<br>re/match/rwl03/00540260.c:30 `undefined4(int,int)` (2)<br>re/match/s10/00466b50.c:105 `int(int)` (1)<br>re/match/s21/004d8b70.c:30 `int(int)` (1) |
| 0x004d8a80 | re/match/rwl01/004d8a80.c:44 (2) | c4=match | re/match/s19/004c32b0.c:40 `void()` (0) |
| 0x004d8b70 | re/match/s21/004d8b70.c:33 (1) | c4=match | re/match/s19/004c32b0.c:41 `void()` (0) |
| 0x004d8fa0 | re/match/s21/004d8fa0.c:38 (2) | c4=match | re/match/s19/004c32b0.c:42 `void()` (0) |
| 0x004d9000 | re/match/rwl01/004d9000.c:33 (1) | c4=match | re/match/s19/004c32b0.c:43 `void()` (0) |
| 0x004d9030 | re/match/rwl01/004d9030.c:30 (1) | c4=match | re/match/s19/004c32b0.c:44 `void()` (0) |
| 0x004d9040 | re/match/rwl01/004d9040.c:30 (1) | c4=match | re/match/s19/004c32b0.c:45 `void()` (0) |
| 0x004dd150 | re/match/rwl01/004dd150.c:40 (2) | c4=match | re/match/s20/004c8e50.c:51 `int(void*,void*,int)` (3) |
| 0x004e1290 | re/match/rwl01/004e1290.c:29 (0) | c4=match | re/match/rwl01/004e18f0.c:36 `undefined4(int)` (1) |
| 0x004e12a0 | re/match/rwl01/004e12a0.c:28 (0) | c4=match | re/match/rwl01/004e19f0.c:35 `int(int)` (1) |
| 0x004e36a0 | re/match/g2sh/004e36a0.c:49 (2) | c4=match | re/match/rwl02/004e3470.c:32 `void()` (0) |
| 0x004e3b80 | re/match/g2sh/004e3b80.c:40 (1) | c4=match | re/match/rwl02/004e3470.c:36 `void()` (0) |
| 0x004e3bd0 | re/match/g2sh/004e3bd0.c:41 (1) | c4=match | re/match/rwl02/004e3470.c:37 `void()` (0) |
| 0x004e3e70 | re/match/g2sh/004e3e70.c:40 (2) | c4=match | re/match/rwl02/004e3470.c:40 `void()` (0) |
| 0x004e3ea0 | re/match/g2sh/004e3ea0.c:40 (1) | c4=match | re/match/rwl02/004e3470.c:41 `void()` (0) |
| 0x004e4130 | re/match/rwl02/004e4130.c:38 (1) | c4=match | re/match/rwl02/004e3470.c:43 `void()` (0) |
| 0x004e4190 | re/match/g2sh/004e4190.c:39 (3) | c4=match | re/match/rwl02/004e3470.c:44 `void()` (0) |
| 0x004e41b0 | re/match/rwl02/004e41b0.c:28 (3) | c4=match | re/match/rwl02/004e3470.c:45 `void()` (0) |
| 0x004e41e0 | re/match/g2sh/004e41e0.c:39 (1) | c4=match | re/match/rwl02/004e3470.c:46 `void()` (0) |
| 0x004e4200 | re/match/g2sh/004e4200.c:39 (3) | c4=match | re/match/rwl02/004e3470.c:47 `void()` (0) |
| 0x004e4250 | re/match/g2sh/004e4250.c:39 (3) | c4=match | re/match/rwl02/004e3470.c:50 `void()` (0) |
| 0x004e42a0 | re/match/rwl02/004e42a0.c:30 (3) | c4=match | re/match/rwl02/004e3470.c:53 `void()` (0) |
| 0x004e42d0 | re/match/rwl02/004e42d0.c:30 (3) | c4=match | re/match/rwl02/004e3470.c:54 `void()` (0) |
| 0x004e4300 | re/match/rwl02/004e4300.c:30 (1) | c4=match | re/match/rwl02/004e3470.c:55 `void()` (0) |
| 0x004e4f00 | re/match/g2sh/004e4f00.c:46 (2) | c4=match | re/match/rwl02/004e5d30.c:31 `void()` (0) |
| 0x004e5190 | re/match/rwl02/004e5190.c:29 (1) | c4=match | re/match/rwl02/004e5820.c:29 `void()` (0) |
| 0x004e5e70 | re/match/g2sh/004e5e70.c:44 (2) | c4=match | re/match/rwl02/004e5d30.c:33 `void()` (0) |
| 0x004e5ef0 | re/match/rwl02/004e5ef0.c:31 (2) | c4=match | re/match/rwl02/004e3470.c:56 `void()` (0) |
| 0x004e5f30 | re/match/rwl02/004e5f30.c:27 (3) | c4=match | re/match/rwl02/004e3470.c:57 `void()` (0) |
| 0x004e5f70 | re/match/g2sh/004e5f70.c:38 (1) | c4=match | re/match/rwl02/004e3470.c:58 `void()` (0) |
| 0x004e5f90 | re/match/rwl02/004e5f90.c:33 (1) | c4=match | re/match/rwl02/004e67b0.c:30 `void()` (0)<br>re/match/rwl05/00557ec0.c:36 `void()` (0)<br>re/match/rwl05/00558030.c:36 `void()` (0) |
| 0x004e64f0 | re/match/g2sh/004e64f0.c:51 (2) | c4=match | re/match/rwl02/004e5d30.c:35 `void()` (0) |
| 0x004e6880 | re/match/rwl02/004e6880.c:31 (1) | c4=match | re/match/rwl02/004e67b0.c:31 `void()` (0) |
| 0x004e6d00 | re/match/rwl02/004e6d00.c:30 (1) | c4=match | re/match/rwl02/004e6ab0.c:48 `int(int,int)` (2)<br>re/match/rwl02/004e7420.c:64 `void()` (0) |
| 0x004e6d40 | re/match/g2sh/004e6d40.c:40 (1) | c4=match | re/match/rwl02/004e6ab0.c:32 `void()` (0)<br>re/match/rwl02/004e7420.c:65 `void()` (0) |
| 0x004e7cc0 | re/match/rwl02/004e7cc0.c:37 (1) | c4=match | re/match/rwl02/004e7420.c:66 `void()` (0) |
| 0x004e7e50 | re/match/rwl02/004e7e50.c:31 (2) | c4=match | re/match/rwl02/004e3470.c:59 `void()` (0) |
| 0x004e7e90 | re/match/rwl02/004e7e90.c:28 (3) | c4=match | re/match/rwl02/004e3470.c:60 `void()` (0) |
| 0x004e7ed0 | re/match/g2sh/004e7ed0.c:38 (1) | c4=match | re/match/rwl02/004e3470.c:61 `void()` (0) |
| 0x004e7ef0 | re/match/g2sh/004e7ef0.c:46 (2) | c4=match | re/match/rwl02/004e5d30.c:36 `void()` (0) |
| 0x004e8540 | re/match/g2sh/004e8540.c:39 (2) | c4=match | re/match/rwl02/004e5d30.c:38 `void()` (0) |
| 0x004e8560 | re/match/g2sh/004e8560.c:39 (1) | c4=match | re/match/rwl02/004e5d30.c:39 `void()` (0) |
| 0x004e98d0 | re/match/rwl02/004e98d0.c:30 (2) | c4=match | re/match/rwl02/004e3470.c:62 `void()` (0) |
| 0x004e9910 | re/match/rwl02/004e9910.c:31 (2) | c4=match | re/match/rwl02/004e3470.c:63 `void()` (0) |
| 0x004e9950 | re/match/rwl02/004e9950.c:27 (3) | c4=match | re/match/rwl02/004e3470.c:64 `void()` (0) |
| 0x004e9990 | re/match/g2sh/004e9990.c:38 (1) | c4=match | re/match/rwl02/004e3470.c:65 `void()` (0) |
| 0x004ea4c0 | re/match/g2sh/004ea4c0.c:39 (1) | c4=match | re/match/rwl02/004e5d30.c:40 `void()` (0) |
| 0x004ea4d0 | re/match/g2sh/004ea4d0.c:39 (1) | c4=match | re/match/rwl02/004e5d30.c:41 `void()` (0) |
| 0x004eb3c0 | re/match/rwl02/004eb3c0.c:30 (0) | c4=match | re/match/rwl03/004f43f0.c:36 `undefined**(int)` (1) |
| 0x004eb9d0 | re/match/rwl02/004eb9d0.c:27 (0) | c4=match | re/match/rwl03/004f4340.c:36 `undefined**(int)` (1) |
| 0x004f08e0 | re/match/g2sh/004f08e0.c:39 (1) | c4=match | re/match/rwl02/004e5d30.c:42 `void()` (0) |
| 0x004f08f0 | re/match/g2sh/004f08f0.c:39 (1) | c4=match | re/match/rwl02/004e5d30.c:43 `void()` (0) |
| 0x004f0900 | re/match/s22/004f0900.c:29 (0) | c4=match | re/match/rwl02/004e5680.c:26 `int(int)` (1) |
| 0x004f0990 | re/match/g2sh/004f0990.c:41 (1) | c4=match | re/match/rwl02/004e5d30.c:44 `void()` (0) |
| 0x004f09c0 | re/match/g2sh/004f09c0.c:43 (2) | c4=match | re/match/rwl02/004e5d30.c:45 `void()` (0) |
| 0x004f4540 | re/match/g2sh/004f4540.c:39 (1) | c4=match | re/match/s22/004f4510.c:30 `void*(void*,int,int)` (3) |
| 0x004f8620 | re/match/g2sh/004f8620.c:39 (1) | c4=match | re/match/s22/004f8580.c:35 `void*(void*,int,int)` (3) |
| 0x004f8630 | re/match/g2sh/004f8630.c:39 (1) | c4=match | re/match/s22/004f8580.c:36 `void*(void*,int,int)` (3) |
| 0x0052d950 | re/match/g2sh/0052d950.c:41 (1) | c4=match | re/match/rwl03/0052d8e0.c:29 `void()` (0) |
| 0x0052e3d0 | re/match/rwta/0052e3d0.c:47 (1) | c4=match | re/match/rwl03/0052e310.c:31 `void()` (0) |
| 0x0052e490 | re/match/g2sh/0052e490.c:44 (1) | c4=match | re/match/rwl03/0052e310.c:32 `void()` (0) |
| 0x0052e4e0 | re/match/g2sh/0052e4e0.c:39 (1) | c4=match | re/match/rwl03/0052e310.c:33 `void()` (0) |
| 0x0052e500 | re/match/g2sh/0052e500.c:43 (1) | c4=match | re/match/rwl03/0052e310.c:35 `void()` (0) |
| 0x0052e650 | re/match/rwl03/0052e650.c:31 (4) | c4=match | re/match/rwl03/0052e310.c:34 `void()` (0) |
| 0x0052e710 | re/match/g2sh/0052e710.c:44 (3) | c4=match | re/match/rwl03/0052e310.c:42 `void()` (0) |
| 0x0052fd90 | re/match/g2sh/0052fd90.c:38 (2) | c4=match | re/match/rwl03/0052fdb0.c:29 `void()` (0) |
| 0x00530000 | re/match/rwl03/00530000.c:29 (1) | c4=match | re/match/rwl03/0052e650.c:29 `void(int,int)` (2) |
| 0x00533740 | re/match/rwtb/00533740.c:53 (1) | c4=match | re/match/rwl03/005336d0.c:29 `void()` (0) |
| 0x005337f0 | re/match/g2sh/005337f0.c:40 (1) | c4=match | re/match/rwl03/005336d0.c:30 `void()` (0) |
| 0x00533840 | re/match/g2sh/00533840.c:42 (1) | c4=match | re/match/rwl03/005336d0.c:32 `void()` (0) |
| 0x00534ad0 | re/match/g2sh/00534ad0.c:39 (1) | c4=match | re/match/rwl03/00534a80.c:36 `int()` (0) |
| 0x00534af0 | re/match/g2sh/00534af0.c:41 (1) | c4=match | re/match/rwl03/00534a80.c:37 `int()` (0) |
| 0x00537b20 | re/match/rwl03/00537b20.c:31 (3) | c4=match | re/match/rwl03/00536780.c:46 `externvoid()` (0) |
| 0x00537d20 | re/match/rwl03/00537d20.c:30 (3) | c4=match | re/match/rwl03/00536780.c:49 `externvoid()` (0) |
| 0x00537d70 | re/match/rwl03/00537d70.c:31 (3) | c4=match | re/match/rwl03/00536780.c:42 `externvoid()` (0) |
| 0x00537e60 | re/match/rwl03/00537e60.c:30 (3) | c4=match | re/match/rwl03/00536780.c:44 `externvoid()` (0) |
| 0x005386b0 | re/match/rwta/005386b0.c:52 (1) | c4=match | re/match/rwl03/00538600.c:36 `int()` (0) |
| 0x00538830 | re/match/g2sh/00538830.c:40 (1) | c4=match | re/match/rwl03/00538600.c:39 `int()` (0) |
| 0x00538870 | re/match/g2sh/00538870.c:39 (1) | c4=match | re/match/rwl03/00538600.c:40 `int()` (0) |
| 0x00538980 | re/match/g2sh/00538980.c:41 (3) | c4=match | re/match/rwl03/00538600.c:44 `int()` (0) |
| 0x00538c30 | re/match/rwtb/00538c30.c:41 (2) | c4=match | re/match/rwl03/00538bd0.c:29 `int(int,int,int)` (3) |
| 0x0053d1f0 | re/match/g2sh/0053d1f0.c:39 (1) | c4=match | re/match/rwl03/0053d0b0.c:39 `void()` (0) |
| 0x0053d200 | re/match/rwl03/0053d200.c:29 (2) | c4=match | re/match/rwl03/0053d0b0.c:37 `void()` (0) |
| 0x0053d230 | re/match/g2sh/0053d230.c:39 (2) | c4=match | re/match/rwl03/0053d0b0.c:32 `void()` (0) |
| 0x0053d3e0 | re/match/g2sh/0053d3e0.c:39 (1) | c4=match | re/match/rwl03/0053d0b0.c:35 `void()` (0) |
| 0x0053ebb0 | re/match/g2sh/0053ebb0.c:42 (1) | c4=match | re/match/rwl03/0053eaa0.c:33 `int()` (0) |
| 0x0053ebf0 | re/match/g2sh/0053ebf0.c:45 (1) | c4=match | re/match/rwl03/0053eaa0.c:34 `int()` (0) |
| 0x0053ec40 | re/match/g2sh/0053ec40.c:39 (1) | c4=match | re/match/rwl03/0053eaa0.c:35 `int()` (0) |
| 0x0053ec60 | re/match/g2sh/0053ec60.c:42 (1) | c4=match | re/match/rwl03/0053eaa0.c:36 `int()` (0) |
| 0x0053f9b0 | re/match/g2sh/0053f9b0.c:39 (1) | c4=match | re/match/rwl03/0053eaa0.c:41 `int()` (0) |
| 0x0053fa20 | re/match/rwl03/0053fa20.c:31 (3) | c4=match | re/match/rwl03/0053eaa0.c:44 `int()` (0) |
| 0x0053fa90 | re/match/g2sh/0053fa90.c:39 (1) | c4=match | re/match/rwl03/0053eaa0.c:46 `int()` (0) |
| 0x0053fb00 | re/match/rwl03/0053fb00.c:31 (3) | c4=match | re/match/rwl03/0053eaa0.c:49 `int()` (0) |
| 0x00543500 | re/match/rwtb/00543500.c:57 (4) | c4=match | re/match/rwl04/00543710.c:54 `void()` (0) |
| 0x00543950 | re/match/rwl04/00543950.c:37 (3) | c4=match | re/match/rwl04/00543710.c:55 `void()` (0) |
| 0x005439d0 | re/match/rwl04/005439d0.c:34 (3) | c4=match | re/match/rwl04/00543710.c:56 `void()` (0) |
| 0x005440c0 | re/match/g2sh/005440c0.c:39 (1) | c4=match | re/match/rwl04/00543e50.c:41 `int()` (0) |
| 0x005440d0 | re/match/g2sh/005440d0.c:39 (1) | c4=match | re/match/rwl04/00543e50.c:42 `int()` (0) |
| 0x005440e0 | re/match/g2sh/005440e0.c:38 (2) | c4=match | re/match/rwl04/00543e50.c:43 `int()` (0) |
| 0x005444a0 | re/match/rwtb/005444a0.c:90 (4) | c4=match | re/match/rwl04/00543e50.c:46 `int()` (0) |
| 0x00544dc0 | re/match/g2sh/00544dc0.c:39 (1) | c4=match | re/match/rwl04/00544d20.c:29 `void()` (0) |
| 0x00544dd0 | re/match/g2sh/00544dd0.c:39 (1) | c4=match | re/match/rwl04/00544d20.c:30 `void()` (0) |
| 0x00544e00 | re/match/rwtb/00544e00.c:50 (2) | c4=match | re/match/rwl04/00544d20.c:32 `void()` (0) |
| 0x00544f10 | re/match/g2sh/00544f10.c:40 (1) | c4=match | re/match/rwl04/00544d20.c:33 `void()` (0) |
| 0x005451d0 | re/match/g2sh/005451d0.c:39 (1) | c4=match | re/match/rwl04/00544d20.c:37 `void()` (0) |
| 0x005492d0 | re/match/g2sh/005492d0.c:40 (1) | c4=match | re/match/rwl04/00549640.c:42 `void()` (0) |
| 0x00549310 | re/match/g2sh/00549310.c:40 (2) | c4=match | re/match/rwl04/00549640.c:43 `void()` (0) |
| 0x00549710 | re/match/rwta/00549710.c:50 (1) | c4=match | re/match/rwl04/00549640.c:30 `void()` (0) |
| 0x005497f0 | re/match/g2sh/005497f0.c:43 (1) | c4=match | re/match/rwl04/00549640.c:31 `void()` (0) |
| 0x0054a000 | re/match/g2sh/0054a000.c:40 (1) | c4=match | re/match/rwl04/00549640.c:33 `void()` (0) |
| 0x0054a030 | re/match/g2sh/0054a030.c:41 (2) | c4=match | re/match/rwl04/00549640.c:34 `void()` (0) |
| 0x0054a090 | re/match/g2sh/0054a090.c:41 (3) | c4=match | re/match/rwl04/00549640.c:36 `void()` (0) |
| 0x0054a0f0 | re/match/g2sh/0054a0f0.c:40 (1) | c4=match | re/match/rwl04/00549640.c:38 `void()` (0) |
| 0x00550c30 | re/match/s24/00550c30.c:28 (2) | c4=match | re/match/rwl04/00551190.c:34 `void()` (0) |
| 0x00550c50 | re/match/s24/00550c50.c:41 (4) | c4=match | re/match/rwl04/00551190.c:35 `void()` (0) |
| 0x00550d80 | re/match/s24/00550d80.c:28 (1) | c4=match | re/match/rwl04/00551190.c:36 `void()` (0) |
| 0x00550da0 | re/match/s24/00550da0.c:46 (3) | c4=match | re/match/rwl04/00551190.c:37 `void()` (0) |
| 0x00551050 | re/match/s24/00551050.c:33 (3) | c4=match | re/match/rwl04/00551190.c:40 `void()` (0) |
| 0x00551090 | re/match/s24/00551090.c:63 (2) | c4=match | re/match/rwl04/00551190.c:44 `void()` (0) |
| 0x00551180 | re/match/s24/00551180.c:28 (1) | c4=match | re/match/rwl04/00551190.c:41 `void()` (0) |
| 0x00551290 | re/match/s24/00551290.c:30 (1) | c4=match | re/match/rwl04/00551190.c:42 `void()` (0) |
| 0x005512b0 | re/match/s24/005512b0.c:28 (1) | c4=match | re/match/rwl04/00551190.c:39 `void()` (0) |
| 0x005512d0 | re/match/s24/005512d0.c:29 (2) | c4=match | re/match/rwl04/00551190.c:43 `void()` (0) |
| 0x005514e0 | re/match/s24/005514e0.c:28 (1) | c4=match | re/match/rwl04/00551190.c:45 `void()` (0) |
| 0x00551610 | re/match/g2sh/00551610.c:48 (1) | c4=match | re/match/rwl04/005515a0.c:29 `int()` (0) |
| 0x00551680 | re/match/g2sh/00551680.c:43 (1) | c4=match | re/match/rwl04/005515a0.c:30 `int()` (0) |
| 0x005518f0 | re/match/rwtb/005518f0.c:57 (3) | c4=match | re/match/rwl04/005515a0.c:34 `int()` (0) |
| 0x00551ce0 | re/match/rwl04/00551ce0.c:28 (0) | c4=match | re/match/rwl04/005524a0.c:27 `int(undefined4)` (1) |
| 0x00552b60 | re/match/s24/00552b60.c:37 (0) | c4=match | re/match/s06/00427ca0.c:44 `void(int)` (1) |
| 0x00557bf0 | re/match/g2sh/00557bf0.c:41 (4) | c4=match | re/match/rwl05/005578a0.c:29 `int()` (0) |
| 0x00557c60 | re/match/g2sh/00557c60.c:39 (2) | c4=match | re/match/rwl05/005578a0.c:30 `int()` (0) |
| 0x00557d90 | re/match/rwta/00557d90.c:45 (2) | c4=match | re/match/rwl05/005578a0.c:32 `int()` (0) |
| 0x00557e50 | re/match/rwtb/00557e50.c:43 (1) | c4=match | re/match/rwl05/005578a0.c:33 `int()` (0) |
| 0x00557e90 | re/match/g2sh/00557e90.c:40 (1) | c4=match | re/match/rwl05/005578a0.c:34 `int()` (0) |
| 0x0055bd70 | re/match/rwl05/0055bd70.c:27 (1) | c4=match | re/match/rwl06/00575fe0.c:43 `void(void*,void*,int,int)` (4)<br>re/match/rwl06/0057a660.c:32 `void(int,int,int,float*)` (4) |
| 0x00561ff0 | re/match/g2sh/00561ff0.c:38 (2) | c4=match | re/match/rwl05/00561ee0.c:29 `void()` (0) |
| 0x00562500 | re/match/rwl05/00562500.c:29 (1) | c4=match | re/match/rwl05/00562460.c:27 `int(int,int)` (2) |
| 0x0056aae0 | re/match/rwl06/0056aae0.c:30 (20) | c4=match | re/match/rwl06/0056adb0.c:35 `void(int*,int*,int,undefined4,undefined4,int,undefined4,undefined4,int,undefined4,undefined4,int,int,int)` (14) |
| 0x0057c2b0 | re/match/rwl06/0057c2b0.c:30 (1) | c4=match | re/match/rwl06/0057c270.c:25 `void()` (0) |
| 0x0057c2e0 | re/match/g2sh/0057c2e0.c:39 (1) | c4=match | re/match/rwl06/0057c270.c:26 `void()` (0) |
| 0x005a5fd0 | re/match/s30/005a5fd0.c:39 (1) | c4=match | re/match/s25/005a5f00.c:35 `void()` (0) |
| 0x005a6010 | re/match/s30/005a6010.c:37 (1) | c4=match | re/match/s25/005a5f00.c:36 `void()` (0)<br>re/match/s25/005a6030.c:29 `void()` (0) |
| 0x005a6670 | re/match/s25/005a6670.c:28 (1) | c4=match | re/match/s25/005a6340.c:33 `void()` (0) |
| 0x005a6df0 | re/match/s25/005a6df0.c:28 (1) | c4=match | re/match/s25/005a6710.c:28 `void(int,int,int,undefined4)` (4)<br>re/match/s25/005a6f30.c:47 `uint(int,undefined4,undefined4,undefined4)` (4)<br>re/match/s25/005a71f0.c:41 `void(int,int,int,VP2*)` (4) |
| 0x005a7a60 | re/match/s25/005a7a60.c:28 (1) | c4=match | re/match/s25/005ac880.c:33 `int(int,int)` (2) |
| 0x005a7b00 | re/match/s25/005a7b00.c:29 (2) | c4=match | re/match/s25/005aba20.c:33 `void(undefined4)` (1) |
| 0x005a7b50 | re/match/s25/005a7b50.c:29 (0) | c4=match | re/match/s25/005aba20.c:34 `undefined4(int)` (1) |
| 0x005a8390 | re/match/s25/005a8390.c:37 (3) | c4=match | re/match/s25/005a8060.c:50 `void()` (0) |
| 0x005a8560 | re/match/s25/005a8560.c:41 (1) | c4=match | re/match/s25/005a8060.c:51 `void()` (0) |
| 0x005aa1e0 | re/match/s25/005aa1e0.c:28 (3) | c4=match | re/match/s25/005aa060.c:26 `undefined4(undefined4)` (1) |
| 0x005aad40 | re/match/s25/005aad40.c:29 (0) | c4=match | re/match/s25/005a60b0.c:31 `undefined4(int)` (1)<br>re/match/s25/005a60e0.c:30 `undefined4(int)` (1) |
| 0x005ac930 | re/match/tlr/005ac930.c:32 (3) | c4=match | re/match/s25/005ac900.c:26 `undefined4(undefined4)` (1) |
| 0x005aea00 | re/match/s26/005aea00.c:28 (2) | c4=match | re/match/s25/005ac210.c:51 `undefined4()` (0)<br>re/match/s26/005afcf0.c:49 `undefined4()` (0)<br>re/match/s27/005b51d0.c:40 `undefined4(,...)` (0)<br>re/match/s27/005b51d0.c:42 `undefined4()` (0)<br>re/match/s27/005b53b0.c:39 `undefined4(,...)` (0)<br>re/match/s27/005b53b0.c:41 `undefined4()` (0) |
| 0x005aea50 | re/match/s26/005aea50.c:29 (0) | c4=match | re/match/s25/005aadc0.c:27 `int(int,undefined4,int)` (3)<br>re/match/s27/005b3b80.c:37 `undefined4*(uint,int,int)` (3) |
| 0x005aeaf0 | re/match/s26/005aeaf0.c:30 (2) | c4=match | re/match/s26/005aea60.c:32 `void()` (0) |
| 0x005af470 | re/match/s26/005af470.c:28 (1) | c4=match | re/match/s26/005af2d0.c:29 `int(int,undefined4)` (2) |
| 0x005af700 | re/match/s26/005af700.c:26 (3) | c4=match | re/match/s26/005af690.c:26 `int*(int,int,uint,int,uint*)` (5) |
| 0x005b0a90 | re/match/s26/005b0a90.c:30 (7) | c4=match | re/match/s26/005afcf0.c:54 `int*(int*,undefined4)` (2) |
| 0x005b10d0 | re/match/s26/005b10d0.c:28 (4) | c4=match | re/match/s26/005af690.c:28 `void(int)` (1) |
| 0x005b1140 | re/match/s26/005b1140.c:27 (3) | c4=match | re/match/s26/005af7a0.c:27 `void(undefined4,undefined4,undefined4,undefined4)` (4)<br>re/match/s26/005af7d0.c:27 `void(int,int,int,int)` (4) |
| 0x005b11d0 | re/match/s27/005b11d0.c:28 (1) | c4=match | re/match/s27/005b7b30.c:31 `void(,...)` (0) |
| 0x005bd9e0 | re/match/s28/005bd9e0.c:32 (2) | c4=match | re/match/s27/005bce80.c:31 `void()` (0) |
| 0x005c31d0 | re/match/s28/005c31d0.c:30 (2) | c4=match | re/match/s28/005c1f97.c:31 `unsignedint*(undefined1*,unsignedchar*,unsignedint,int,int)` (5)<br>re/match/s28/005c27e6.c:28 `int(unsignedint*,char*,int,int,int)` (5) |
| 0x005c7070 | re/match/s28/005c7070.c:31 (1) | c4=match | re/match/s28/005c6cc0.c:33 `void()` (0) |
| 0x005c7fb0 | re/match/s28/005c7fb0.c:35 (1) | c4=match | re/match/s28/005c7a90.c:29 `void()` (0) |
| 0x005c7fe0 | re/match/s28/005c7fe0.c:30 (1) | c4=match | re/match/s28/005c7a90.c:30 `void()` (0) |
| 0x005c8980 | re/match/s28/005c8980.c:30 (1) | c4=match | re/match/s28/005c8120.c:31 `void()` (0) |
| 0x005c8eb0 | re/match/s28/005c8eb0.c:31 (2) | c4=match | re/match/s28/005c8120.c:33 `void()` (0) |
| 0x005c9d00 | re/match/s28/005c9d00.c:28 (0) | c4=match | re/match/s15/00493710.c:45 `int(void*)` (1)<br>re/match/s20/004c8e50.c:27 `int(void*,void*,int)` (3)<br>re/match/s27/005b66c0.c:26 `int(int,int,int)` (3) |

### Own-source reference not c4=match (lower priority)

| callee RVA | reference source:line (n) | ref tag | offending source:line declared prototype (n) |
|---|---|---|---|
| 0x00407db0 | re/match/s01/00407db0.c:5 (2) | own source, not c4=match | re/match/s01/00407e20.c:22 `int(int,int,int)` (3) |
| 0x00408610 | re/match/s01/00408610.c:74 (3) | own source, not c4=match | re/match/s02/0040da50.c:44 `void(void*,undefined4)` (2) |
| 0x00416250 | re/match/t01g/00416250.c:105 (3) | own source, not c4=match | re/match/racepath/00418560.c:60 `void(Bank*,int,unsignedchar*,float)` (4) |
| 0x00420420 | re/match/s05/00420420.c:179 (1) | own source, not c4=match | re/match/topm01/0040d110.c:57 `void(,...)` (0) |
| 0x00422ba0 | re/match/s05/00422ba0.c:81 (0) | own source, not c4=match | re/match/s05/00425a40.c:34 `undefined4(int)` (1) |
| 0x00424eb0 | re/match/s05/00424eb0.c:90 (1) | own source, not c4=match | re/match/s05/00425a40.c:36 `undefined4(int,void*)` (2) |
| 0x00426c10 | re/match/s06/00426c10.c:35 (0) | own source, not c4=match | re/match/t01g/004111c0.c:400 `void(int)` (1) |
| 0x0042aad0 | re/match/s06/0042aad0.c:35 (1) | own source, not c4=match | re/match/s08/0043af10.c:102 `void()` (0) |
| 0x0042aae0 | re/match/s06/0042aae0.c:55 (0) | own source, not c4=match | re/match/s08/00432ad0.c:30 `void(int)` (1)<br>re/match/s08/00433f40.c:53 `void(int)` (1)<br>re/match/s08/0043c5b0.c:65 `void(int)` (1) |
| 0x0042ae10 | re/match/s06/0042ae10.c:41 (0) | own source, not c4=match | re/match/s08/0043d7c0.c:88 `int(int)` (1)<br>re/match/s08/0043dfd0.c:202 `undefined4(int)` (1) |
| 0x0042e5b0 | re/match/s07/0042e5b0.c:48 (0) | own source, not c4=match | re/match/s08/0043c5b0.c:68 `void(int)` (1) |
| 0x00442440 | re/match/s08/00442440.c:56 (1) | own source, not c4=match | re/match/s08/004464c0.c:30 `void(int*,int)` (2) |
| 0x00450b10 | re/match/s08/00450b10.c:47 (7) | own source, not c4=match | re/match/s06/004270f0.c:59 `void(int,V2,V2,RGBA4,float*)` (5) |
| 0x0046f6c0 | re/match/s11/0046f6c0.c:107 (3) | own source, not c4=match | re/match/s11/00470c70.c:114 `void(int)` (1) |
| 0x00470670 | re/match/s11/00470670.c:121 (5) | own source, not c4=match | re/match/s11/00470c70.c:109 `void(int,float,Tab4c*,float*)` (4) |
| 0x00473ee0 | re/match/s11/00473ee0.c:56 (3) | own source, not c4=match | re/match/s07/0042e5b0.c:45 `void(RwV2d,RwV2d,int,int,int,int,int,int)` (8) |
| 0x004744a0 | re/match/s11/004744a0.c:52 (0) | own source, not c4=match | re/match/s06/00428d30.c:54 `void(S06_V2d,S06_V2d,int,double,double,int,int)` (7) |
| 0x00474890 | re/match/s11/00474890.c:50 (9) | own source, not c4=match | re/match/s07/0042e5b0.c:46 `void(int,RwV2d,RwV2d,RwRGBA,int,int,int)` (7)<br>re/match/s07/0042e8b0.c:42 `void(void*,RwV2d,RwV2d,RwRGBA,int,int,int)` (7) |
| 0x0047d3c0 | re/match/s13/0047d3c0.c:75 (1) | own source, not c4=match | re/match/s13/0047eb30.c:78 `void(int,int)` (2) |
| 0x0047d640 | re/match/s13/0047d640.c:112 (1) | own source, not c4=match | re/match/s13/0047e9c0.c:27 `void(int,undefined4,undefined4)` (3) |
| 0x0047def0 | re/match/s13/0047def0.c:125 (1) | own source, not c4=match | re/match/s13/0047e9c0.c:28 `void(int,undefined4,undefined4)` (3) |
| 0x0047f6d0 | re/match/s13/0047f6d0.c:36 (5) | own source, not c4=match | re/match/tlr/0047f7d0.c:26 `int(int,int)` (2) |
| 0x0047f7d0 | re/match/tlr/0047f7d0.c:29 (5) | own source, not c4=match | re/match/s13/0047f840.c:32 `int(int,int)` (2) |
| 0x004938e0 | re/match/s15/004938e0.c:31 (2) | own source, not c4=match | re/match/s15/00494480.c:29 `void(undefined4)` (1) |
| 0x00493f70 | re/match/s15/00493f70.c:29 (0) | own source, not c4=match | re/match/s10/0045dbe0.c:29 `int(int)` (1)<br>re/match/s16/00495350.c:31 `int(int)` (1) |
| 0x00493fc0 | re/match/s15/00493fc0.c:29 (0) | own source, not c4=match | re/match/s15/00494480.c:30 `undefined4(undefined4)` (1)<br>re/match/s16/00495350.c:33 `int(float,float)` (2) |
| 0x00493fd0 | re/match/s15/00493fd0.c:53 (7) | own source, not c4=match | re/match/s16/00495350.c:34 `void(int,V2*,V2*,int,int)` (5) |
| 0x004c32b0 | re/match/s19/004c32b0.c:85 (3) | own source, not c4=match | re/match/s15/00493710.c:47 `int(int,int)` (2) |
| 0x004c3730 | re/match/s19/004c3730.c:31 (3) | own source, not c4=match | re/match/rwl01/004c3e90.c:41 `void()` (0)<br>re/match/s19/004c35f0.c:28 `int()` (0) |
| 0x004c3880 | re/match/s19/004c3880.c:32 (3) | own source, not c4=match | re/match/rwl01/004c3e90.c:43 `void()` (0)<br>re/match/s19/004c35f0.c:30 `int()` (0) |
| 0x004c3e20 | re/match/rwl01/004c3e20.c:38 (1) | own source, not c4=match | re/match/s19/004c32b0.c:53 `void()` (0) |
| 0x004c3e90 | re/match/rwl01/004c3e90.c:47 (2) | own source, not c4=match | re/match/s19/004c32b0.c:54 `void()` (0) |
| 0x004c5e00 | re/match/rwl01/004c5e00.c:45 (1) | own source, not c4=match | re/match/s19/004c32b0.c:57 `void()` (0) |
| 0x004c8c70 | re/match/s20/004c8c70.c:75 (0) | own source, not c4=match | re/match/rwd01/004c7a70.c:169 `int(void*,void*,int)` (3) |
| 0x004ca160 | re/match/rwtb/004ca160.c:125 (2) | own source, not c4=match | re/match/s20/004c8e50.c:28 `int(void*,void*,int)` (3) |
| 0x004d3090 | re/match/rwl01/004d3090.c:49 (2) | own source, not c4=match | re/match/s20/004c8e50.c:36 `int(void*,void*,int)` (3)<br>re/match/s20/004d3b00.c:69 `int(int,RwRaster*,int)` (3) |
| 0x004d4f90 | re/match/rwl01/004d4f90.c:72 (3) | own source, not c4=match | re/match/rwl03/005327d0.c:40 `int(int,int,void*,int)` (4)<br>re/match/rwl04/00543710.c:64 `int(int,int,int,int)` (4)<br>re/match/rwl04/00549a50.c:31 `int(int,int,int*,int)` (4)<br>re/match/s16/00496d60.c:34 `int(int,int,int,int)` (4)<br>re/match/s16/00496db0.c:34 `int(int,int,int,int)` (4) |
| 0x004dc280 | re/match/s21/004dc280.c:34 (3) | own source, not c4=match | re/match/rwd01/004c7a70.c:177 `void()` (0) |
| 0x004dc4a0 | re/match/rwl01/004dc4a0.c:29 (3) | own source, not c4=match | re/match/rwd01/004c7a70.c:176 `void()` (0) |
| 0x004de1b0 | re/match/rwl01/004de1b0.c:49 (2) | own source, not c4=match | re/match/s20/004c8e50.c:35 `int(void*,void*,int)` (3)<br>re/match/s20/004d3b00.c:72 `RwImage*(RwImage*,RwRaster*,int)` (3) |
| 0x004df120 | re/match/rwl01/004df120.c:63 (2) | own source, not c4=match | re/match/s20/004c8e50.c:34 `int(void*,void*,int)` (3)<br>re/match/s20/004d3b00.c:73 `RwRaster*(RwRaster*,RwImage*,int)` (3) |
| 0x004e3b30 | re/match/g2seq/004e3b30.c:40 (2) | own source, not c4=match | re/match/rwl02/004e3470.c:35 `void()` (0) |
| 0x004e3e40 | re/match/g2seq/004e3e40.c:41 (1) | own source, not c4=match | re/match/rwl02/004e3470.c:39 `void()` (0) |
| 0x004e6610 | re/match/g2seq/004e6610.c:39 (1) | own source, not c4=match | re/match/rwl02/004e65c0.c:28 `void()` (0) |
| 0x004e6630 | re/match/g2seq/004e6630.c:39 (1) | own source, not c4=match | re/match/rwl02/004e65c0.c:29 `void()` (0) |
| 0x004f36c0 | re/match/rwl03/004f36c0.c:47 (1) | own source, not c4=match | re/match/rwl03/004f3b00.c:31 `int(int,int)` (2) |
| 0x004f79f0 | re/match/s22/004f79f0.c:79 (6) | own source, not c4=match | re/match/rwl02/004ea9a0.c:54 `undefined4()` (0)<br>re/match/rwl03/00530650.c:40 `undefined4()` (0)<br>re/match/rwtb/004ea9a0.c:50 `undefined4()` (0) |
| 0x0052e560 | re/match/g2seq/0052e560.c:39 (1) | own source, not c4=match | re/match/rwl03/0052e310.c:36 `void()` (0) |
| 0x0052e580 | re/match/g2seq/0052e580.c:39 (1) | own source, not c4=match | re/match/rwl03/0052e310.c:37 `void()` (0) |
| 0x0052e5a0 | re/match/g2seq/0052e5a0.c:39 (2) | own source, not c4=match | re/match/rwl03/0052e310.c:38 `void()` (0) |
| 0x0052e5c0 | re/match/g2seq/0052e5c0.c:42 (1) | own source, not c4=match | re/match/rwl03/0052e310.c:39 `void()` (0) |
| 0x00532b80 | re/match/rwl03/00532b80.c:80 (4) | own source, not c4=match | re/match/s18/004b47e0.c:32 `void(Key_4b47e0*,Key_4b47e0*,Key_4b47e0*,float,int)` (5) |
| 0x00533820 | re/match/g2seq/00533820.c:39 (1) | own source, not c4=match | re/match/rwl03/005336d0.c:31 `void()` (0) |
| 0x00533900 | re/match/g2seq/00533900.c:40 (2) | own source, not c4=match | re/match/rwl03/005336d0.c:33 `void()` (0) |
| 0x00534990 | re/match/g2seq/00534990.c:41 (1) | own source, not c4=match | re/match/s23/00534920.c:33 `void*(void*,int,int)` (3) |
| 0x00535700 | re/match/rwl03/00535700.c:29 (4) | own source, not c4=match | re/match/s14/00487e20.c:25 `int(int,int,int)` (3) |
| 0x00536e90 | re/match/rwl03/00536e90.c:33 (5) | own source, not c4=match | re/match/rwl03/00536780.c:39 `externvoid()` (0) |
| 0x00538800 | re/match/g2seq/00538800.c:39 (1) | own source, not c4=match | re/match/rwl03/00538600.c:38 `int()` (0) |
| 0x0053d1c0 | re/match/g2seq/0053d1c0.c:41 (1) | own source, not c4=match | re/match/rwl03/0053d0b0.c:38 `void()` (0) |
| 0x0053f9d0 | re/match/g2seq/0053f9d0.c:39 (2) | own source, not c4=match | re/match/rwl03/0053eaa0.c:42 `int()` (0) |
| 0x00544100 | re/match/g2seq/00544100.c:39 (2) | own source, not c4=match | re/match/rwl04/00543e50.c:44 `int()` (0) |
| 0x005441f0 | re/match/rwtb/005441f0.c:47 (3) | own source, not c4=match | re/match/rwl04/00543e50.c:45 `int()` (0) |
| 0x00544de0 | re/match/g2seq/00544de0.c:39 (1) | own source, not c4=match | re/match/rwl04/00544d20.c:31 `void()` (0) |
| 0x00549350 | re/match/g2seq/00549350.c:42 (1) | own source, not c4=match | re/match/rwl04/00549640.c:44 `void()` (0) |
| 0x00549390 | re/match/g2seq/00549390.c:42 (4) | own source, not c4=match | re/match/rwl04/00549640.c:29 `void()` (0) |
| 0x00549fe0 | re/match/g2seq/00549fe0.c:39 (1) | own source, not c4=match | re/match/rwl04/00549640.c:32 `void()` (0) |
| 0x00550f20 | re/match/s24/00550f20.c:53 (3) | own source, not c4=match | re/match/rwl04/00551190.c:38 `void()` (0) |
| 0x005516c0 | re/match/g2seq/005516c0.c:39 (1) | own source, not c4=match | re/match/rwl04/005515a0.c:31 `int()` (0) |
| 0x005516e0 | re/match/g2seq/005516e0.c:44 (1) | own source, not c4=match | re/match/rwl04/005515a0.c:32 `int()` (0) |
| 0x00551840 | re/match/rwl04/00551840.c:30 (3) | own source, not c4=match | re/match/rwl04/005515a0.c:36 `int()` (0) |
| 0x00554940 | re/match/s24/00554940.c:72 (5) | own source, not c4=match | re/match/rwl05/00554390.c:89 `void()` (0)<br>re/match/rwl05/005551d0.c:28 `int()` (0) |
| 0x00556ca0 | re/match/s24/00556ca0.c:31 (3) | own source, not c4=match | re/match/s06/00427e00.c:46 `void(int,short*,float,float*,int)` (5)<br>re/match/s06/00427f00.c:40 `void(int,short*,float,float*,int)` (5)<br>re/match/s06/00427ff0.c:43 `void(int,short*,float,float*,int)` (5)<br>re/match/s06/00428140.c:44 `void(int,short*,float,float*,int)` (5) |
| 0x0055b650 | re/match/rwl05/0055b650.c:28 (3) | own source, not c4=match | re/match/s13/00481750.c:28 `void(int)` (1)<br>re/match/s13/00481d90.c:32 `void(int)` (1) |
| 0x00562ac0 | re/match/tlr/00562ac0.c:50 (5) | own source, not c4=match | re/match/rwl05/00562600.c:26 `int()` (0) |
| 0x00569e90 | re/match/rwl06/00569e90.c:28 (39) | own source, not c4=match | re/match/rwl06/00569690.c:26 `void(int,undefined4,undefined4)` (3) |
| 0x005a65d0 | re/match/s25/005a65d0.c:31 (3) | own source, not c4=match | re/match/s25/005a6340.c:32 `void()` (0) |
| 0x005a6d60 | re/match/s25/005a6d60.c:31 (4) | own source, not c4=match | re/match/s25/005a6df0.c:26 `uint(int)` (1) |
| 0x005aab00 | re/match/s25/005aab00.c:27 (2) | own source, not c4=match | re/match/s25/005a60b0.c:30 `int*(undefined4)` (1) |
| 0x005aab70 | re/match/s25/005aab70.c:31 (2) | own source, not c4=match | re/match/s25/005a60e0.c:29 `int*(undefined4)` (1) |
| 0x005ab380 | re/match/s25/005ab380.c:29 (2) | own source, not c4=match | re/match/s25/005a7b60.c:38 `int(undefined4,int*,uint)` (3)<br>re/match/s25/005ab410.c:32 `int(undefined4,HDR20*,int)` (3)<br>re/match/s25/005abfa0.c:41 `undefined4(undefined4,SH*,uint)` (3) |
| 0x005ab410 | re/match/s25/005ab410.c:34 (4) | own source, not c4=match | re/match/s25/005a7b60.c:39 `int(undefined4,int,undefined4,undefined4,uint)` (5) |
| 0x005ab710 | re/match/s25/005ab710.c:43 (5) | own source, not c4=match | re/match/s25/005ab090.c:27 `void()` (0) |
| 0x005b0f40 | re/match/s26/005b0f40.c:28 (1) | own source, not c4=match | re/match/s27/005b7010.c:33 `void(,...)` (0)<br>re/match/s27/005b7880.c:29 `void(,...)` (0) |
| 0x005b1180 | re/match/s27/005b1180.c:29 (1) | own source, not c4=match | re/match/s28/005bdad0.c:36 `undefined4(int,int,int)` (3) |
| 0x005bd6f0 | re/match/s27/005bd6f0.c:59 (1) | own source, not c4=match | re/match/s27/005bce80.c:32 `void()` (0) |
| 0x005bde70 | re/match/s28/005bde70.c:30 (4) | own source, not c4=match | re/match/s27/005bd6f0.c:47 `undefined4(KR)` (1)<br>re/match/s28/005bdad0.c:38 `void(int,undefined4)` (2) |
| 0x005bf350 | re/match/s28/005bf350.c:29 (1) | own source, not c4=match | re/match/s28/005bdad0.c:41 `undefined4(int,int,int)` (3) |
| 0x005c2f8d | re/match/s28/005c2f8d.c:36 (2) | own source, not c4=match | re/match/s28/005c2f70.c:26 `void()` (0) |
| 0x005cb2a0 | re/match/s28/005cb2a0.c:30 (3) | own source, not c4=match | re/match/rwd01/004c7a70.c:174 `void()` (0) |

## Rows ready for roadmap.ps1 add (not added)

Each ARITY mismatch is one row by the parent instruction; with 288 c4=match references, batching by offending source is the practical form. One row per offending source file, title `fix <callee list> declaration in <source>` (declaration-only edit, then match.py score check, no body change). Suggested row template:
`roadmap.ps1 add MATCH-ARITY-<offending-source-rva> -Section MATCH -Title "Declare FUN_<callee> with its own-source prototype (n=<ref n>) in <offending source>; run match.py before/after, report score delta"`.
Offending-source list (source -> callees to fix) is derivable from `lint.tsv` plus the table above; sources with the most offending callees first:

- `re/match/s19/004c32b0.c`: 29 callees (0x004c07b0, 0x004c0830, 0x004c1940, 0x004c1980, 0x004c3e20, 0x004c3e90, 0x004c4430, 0x004c4470 ...)
- `re/match/rwl02/004e3470.c`: 27 callees (0x004d7ff0, 0x004e36a0, 0x004e3b30, 0x004e3b80, 0x004e3bd0, 0x004e3e40, 0x004e3e70, 0x004e3ea0 ...)
- `re/match/s20/004c8e50.c`: 13 callees (0x004ca160, 0x004d0640, 0x004d0fc0, 0x004d0fe0, 0x004d1630, 0x004d3090, 0x004d3450, 0x004d34d0 ...)
- `re/match/rwl04/00549640.c`: 12 callees (0x004d7ff0, 0x005492d0, 0x00549310, 0x00549350, 0x00549390, 0x00549710, 0x005497f0, 0x00549fe0 ...)
- `re/match/rwl02/004e5d30.c`: 12 callees (0x004e4f00, 0x004e5e70, 0x004e64f0, 0x004e7ef0, 0x004e8540, 0x004e8560, 0x004ea4c0, 0x004ea4d0 ...)
- `re/match/rwl04/00551190.c`: 12 callees (0x00550c30, 0x00550c50, 0x00550d80, 0x00550da0, 0x00550f20, 0x00551050, 0x00551090, 0x00551180 ...)
- `re/match/rwl03/0052e310.c`: 11 callees (0x004d7ff0, 0x0052e3d0, 0x0052e490, 0x0052e4e0, 0x0052e500, 0x0052e560, 0x0052e580, 0x0052e5a0 ...)
- `re/match/rwl03/0053eaa0.c`: 9 callees (0x0053ebb0, 0x0053ebf0, 0x0053ec40, 0x0053ec60, 0x0053f9b0, 0x0053f9d0, 0x0053fa20, 0x0053fa90 ...)
- `re/match/s16/00495350.c`: 7 callees (0x00493f70, 0x00493fc0, 0x00493fd0, 0x00494460, 0x00494480, 0x00499710, 0x004c1be0)
- `re/match/s12/00479330.c`: 6 callees (0x0042a5d0, 0x0042a640, 0x0042a6b0, 0x0042a7f0, 0x0042a860, 0x004790e0)
- `re/match/rwl04/00543e50.c`: 6 callees (0x005440c0, 0x005440d0, 0x005440e0, 0x00544100, 0x005441f0, 0x005444a0)
- `re/match/rwl04/00544d20.c`: 6 callees (0x00544dc0, 0x00544dd0, 0x00544de0, 0x00544e00, 0x00544f10, 0x005451d0)
- `re/match/rwl04/005515a0.c`: 6 callees (0x00551610, 0x00551680, 0x005516c0, 0x005516e0, 0x00551840, 0x005518f0)
- `re/match/rwl03/005336d0.c`: 5 callees (0x00533740, 0x005337f0, 0x00533820, 0x00533840, 0x00533900)
- `re/match/rwl03/00536780.c`: 5 callees (0x00536e90, 0x00537b20, 0x00537d20, 0x00537d70, 0x00537e60)
- `re/match/rwl03/00538600.c`: 5 callees (0x005386b0, 0x00538800, 0x00538830, 0x00538870, 0x00538980)
- `re/match/rwl03/0053d0b0.c`: 5 callees (0x0053d1c0, 0x0053d1f0, 0x0053d200, 0x0053d230, 0x0053d3e0)
- `re/match/rwl05/005578a0.c`: 5 callees (0x00557bf0, 0x00557c60, 0x00557d90, 0x00557e50, 0x00557e90)
- `re/match/s06/00429290.c`: 4 callees (0x00428a30, 0x00428bf0, 0x00428d30, 0x00466b50)
- `re/match/s06/00428d30.c`: 4 callees (0x00472c60, 0x00472f40, 0x004730b0, 0x004744a0)
- `re/match/rwd01/004c7a70.c`: 4 callees (0x004c8c70, 0x004dc280, 0x004dc4a0, 0x005cb2a0)
- `re/match/rwl04/00543710.c`: 4 callees (0x004d4f90, 0x00543500, 0x00543950, 0x005439d0)
- `re/match/s08/0043dfd0.c`: 3 callees (0x0040acd0, 0x0042ae10, 0x00431b80)
- `re/match/t01g/004111c0.c`: 3 callees (0x0040dbd0, 0x00426c10, 0x0045b350)
- `re/match/s02/0040bbb0.c`: 3 callees (0x0042a6b0, 0x0048a830, 0x0048e820)
- `re/match/s08/0043af10.c`: 3 callees (0x0042aad0, 0x00473870, 0x004739f0)
- `re/match/s08/0043c5b0.c`: 3 callees (0x0042aae0, 0x0042e5b0, 0x00472c60)
- `re/match/s26/005afcf0.c`: 3 callees (0x004522d0, 0x005aea00, 0x005b0a90)
- `re/match/s07/0042e3a0.c`: 3 callees (0x00472c60, 0x00472f40, 0x004730b0)
- `re/match/s07/00431710.c`: 3 callees (0x00472c60, 0x00473870, 0x004739f0)
- `re/match/rwl01/004c3e90.c`: 3 callees (0x004c3730, 0x004c3880, 0x004d7ff0)
- `re/match/s20/004d3b00.c`: 3 callees (0x004d3090, 0x004de1b0, 0x004df120)
- `re/match/rwl02/004e65c0.c`: 3 callees (0x004d7ff0, 0x004e6610, 0x004e6630)
- `re/match/rwl06/0057c270.c`: 3 callees (0x004d7ff0, 0x0057c2b0, 0x0057c2e0)
- `re/match/rwl02/004e7420.c`: 3 callees (0x004e6d00, 0x004e6d40, 0x004e7cc0)
- `re/match/s28/005bdad0.c`: 3 callees (0x005b1180, 0x005bde70, 0x005bf350)
- `re/match/s02/0040da50.c`: 2 callees (0x00408610, 0x00426080)
- `re/match/s02/00410b30.c`: 2 callees (0x0041ebb0, 0x0047b9e0)
- `re/match/s05/0041f8f0.c`: 2 callees (0x00422570, 0x00426070)
- `re/match/s05/00425a40.c`: 2 callees (0x00422ba0, 0x00424eb0)

Total offending sources: 295.

## Limits

- Arity is the declaration as written: a K&R `()` declaration is unspecified and skipped, and a prototype whose parameter list is split over lines is not parsed. Both are misses, not claims.
- A mismatch where the reference source itself is wrong (not c4=match rows) is not decided here: the reference is the callee own source and only `c4=match` rows are byte-checked against the original.
- Whether any flagged call site passes the wrong number of arguments at the instruction level is [UNCERTAIN]: test = disassemble the call site of each offending source and count pushes, as its own row.