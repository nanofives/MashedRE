# RESULT PORT-TOP-04C: AppDestroy 0x00402a40 runs its matched body (L-96 top-down lane)

Branch `orch/PORT-TOP-04C` from `orch/PORT-TOP-04B` @ `70485ace5` (04B not merged yet). Code commits `15d4cf413` + `0c80bce8d`.
Exe `98bb95915222d9b7` (sha256/16), asi `df5c94f92deb6ae8`, built from `0c80bce8d`.

Verdict: GREEN. AppDestroy 0x00402a40 runs its matched body on the knob path. Knob-ON boot exits 0 with the matched teardown in the
trace. 12 / 12 nav-demo captures are byte-identical knob ON vs OFF. Default gates are unchanged. 9 of AppDestroy's 43 callees reach
a named stand-in (listed below).

## What runs now

Under `MASHED_ORIG_TOP=1`, WinMain 0x00492370 calls `PortTop_Bound_00402a40` (Match/PortTop_00492370.cpp), which runs
**AppDestroy 0x00402a40, the matched source** `re/match/s01/raw/00402a40.c` (293 B, byte-identical) compiled in
`Match/PortTop_00402a40.cpp`, then `HandTop_AppDestroy` for the exe audio, DirectInput and the small.mpg graph. Those
three exe subsystems keep their state outside the original globals, so the matched body cannot reach them (see Risks).
Before this step the whole teardown was the stand-in `HandTop_AppDestroy`.

Every callee binds by name, in the brief's order (matched exe body, existing port body, RW binding, named stand-in). No
raw `.text` call.

### AppDestroy's 43 calls (in body order)

| rva | binding | kind |
|---|---|---|
| 0x00403660 | Orig_00403660 (Match/PortTop_00403660.cpp) | matched, new |
| 0x00494ef0 (thunk_FUN_00493f70) | ThunkVideoStateGet (Boot/Teardown.cpp) | port, exe body already (reads DAT_00771a04) |
| 0x00494f20 (thunk_FUN_00494460) | Orig_00494f20 -> IntroVideoClose 0x00494460 (Match/MatchS15_00494460.cpp, now in both targets) -> Orig_00494320 | matched (thunk + 2 bodies) |
| 0x00494bc0 | Orig_00494bc0 | matched, new |
| 0x004c5930 | MashedRw_TexDictionaryDestroy_004c5930 | RW binding (TOP-03) |
| 0x00496ce0 | Orig_00496ce0 | matched, new |
| 0x0040cf80 | Orig_0040cf80 | matched, new |
| 0x00426ba0 / 0x00426c00 | HudDrawEnabled / GameStateFlagGet | matched, exe bodies already |
| 0x0040cfd0 | Orig_0040cfd0 | matched, new |
| 0x004114c0 | PortTop_StandIn04C_004114c0 | **stand-in** (RW free slot, below). Its exe body VtableTeardown_114c0 stays for other callers |
| 0x0041a3d0 0x0041de70 0x0041c2c0 0x0041da80 0x0041d890 0x0041ccf0 0x0041c0e0 | Orig_<rva> | matched, new (the last three hold statics 0x0041d6d0 / 0x0041cb00 / 0x0041beb0) |
| 0x0041e0d0 | PortTop_StandIn04C_0041e0d0 | **stand-in** (null deref, below) |
| 0x0041b660 | PortTop_StandIn04C_0041b660 | **stand-in** (group source, below) |
| 0x00489250 | PortTop_StandIn04C_00489250 | **stand-in** (null deref) |
| 0x0045b930 | Orig_0045b930 | matched, new |
| 0x0041ffb0 | PortTop_StandIn04C_0041ffb0 | **stand-in** (null deref) |
| 0x004189e0 (thunk_FUN_004196f0) | Orig_004189e0 -> PortTop_StandIn04C_004196f0 | thunk matched, target **stand-in** (null deref) |
| 0x00421590 (thunk_FUN_004210f0) | Orig_00421590 -> Orig_004210f0 | matched (thunk + body) |
| 0x004014b0 | PortTop_StandIn04C_004014b0 | **stand-in** (null deref) |
| 0x0040bd00 | Orig_0040bd00 | matched, new |
| 0x00484130 | PortTop_StandIn04C_00484130 | **stand-in** (RW free slot, below) |
| 0x0045b350 | RwInitNullStub | matched, exe body already |
| 0x005581f0 | Orig_005581f0 | matched, new |
| 0x004671a0 then 0x00467020 | BackBuffRasterOpen, Orig_00467020 | matched (TOP-04B): **camera 0x006905b0 destroyed** |
| 0x00467070 | Orig_00467070 | matched, new: **camera clone 0x006905b4 + frame 0x006905b8 destroyed** |
| 0x004b4880 x2 | Orig_004b4880 | matched, new: **cameras 0x0067f18c / 0x0067f190 destroyed** |
| 0x0042c2a0 0x00427620 0x00425ed0 0x004015a0 0x00467010 0x00428400 | Orig_<rva> | matched, new |
| 0x0047ba10 (thunk_FUN_00496970) | Orig_0047ba10 -> Orig_00496970 | matched (thunk + body) |
| 0x004b6550 (thunk_FUN_004b6700) | Orig_004b6550 -> PortTop_StandIn04C_004b6700 | thunk matched, target **stand-in** |
| 0x004d8560 | UtilReturnOne4d8560 | port, exe body already |

Count: AppDestroy has 43 distinct callees. 34 now reach a matched or existing exe body: 23 new matched TUs, 3 matched
thunks whose targets are matched, 7 existing exe bodies, 1 RW binding. 9 reach a named stand-in: 7 directly, 2 behind
a matched thunk. In all, 32 new exe-only TUs (Match/PortTop_<rva>.cpp, 35 bodies with the 3 statics) and 2 shared TUs
(MatchS15_00494460.cpp, MatchS04_0041f320.cpp) joined the exe list.

### The cameras (the 04B hand-on)

The three camera records DisplayInit 0x004921d0 builds (LibRw/RwApiBind.cpp `MashedRwCamera`, RW 3.x layout with the
librw camera behind it) are destroyed by the matched teardown through the bound RW entry points:
`0x00467020(0x004671a0(-1))` for DAT_006905b0, `0x00467070` for the clone DAT_006905b4 and its frame DAT_006905b8,
`0x004b4880(DAT_0067f18c)` and `0x004b4880(DAT_0067f190)`. Each goes RwFrameDestroy / RwRasterDestroy /
RwCameraDestroy on librw. The knob log shows the four records nonzero at AppDestroy entry
(`0x006905b0=08FB5080 0x006905b4=08FB51C0 0x0067f18c=08FB4860 0x0067f190=08FB4E00`). The frame, raster and camera destroy
bindings do not log. So the trace shows these calls only as the unconditional calls in the matched body, the return
line and the clean exit, not one line per camera. WinMain's own RwTexDictionaryDestroy of the DisplayInit TXD
(`0x08EAC780`) follows.

## Stand-ins (named, listed in Match/PortTop_StandIns04C.cpp and BINDINGS.tsv)

A stand-in logs its first call (`[PORT-TOP] rva 0x...: stand-in, not run (PORT-TOP-04C)`) and does nothing.

**Matched callees of AppDestroy that do not run.** AppInitialiseOnBootup 0x00402750, which creates the state AppDestroy
tears down, is still the stand-in `HandTop_InitRest` on the knob path, so the original globals it fills are zero in the
exe. These bodies read through those pointers with no null check and would fault:

| rva | reason |
|---|---|
| 0x004014b0 | reads DAT_00636570 +4 / DAT_0063656c +4 |
| 0x0041e0d0 | reads +4 of each pointer of the 0x0063d610 table |
| 0x00489250 | reads DAT_007030a8 +4 / DAT_007030ac +4 |
| 0x0041ffb0 | 0x004b4880(DAT_0063e490) reads +4 of a zero pointer when DAT_00636acc == 0 |
| 0x004196f0 (behind thunk 0x004189e0) | reads +4 of each pointer of the 0x0063bde0 table |
| 0x00412010 (level 2, under 0x0045b930) | reads DAT_0063bb38[i] +0x3c |

**RW free slot.** 0x004114c0 (exe body VtableTeardown_114c0, Match/MatchS02_004114c0.cpp) and 0x00484130 call RwGlobals
+0x10c (the RW free function) through `*(0x007d3ff8)`. In the exe that pointer is the B15 Im2D fake device
(D3d9Render/RwIm2DBridge.cpp, 0x34 bytes), so +0x10c lies past it. The first knob boot (batch 1, exe `08e7567e`) faulted
there: `0xC0000005` after `0x004d41e0` and before the first RpClumpDestroy. Both are stand-ins on the AppDestroy path.
0x004114c0's exe body stays for its other callers. Running them needs an RW memory-function binding of `*(0x007d3ff8)`
+0x108 / +0x10c.

**One-body rule.**
- 0x0041b660: its only matched source is the group file `h1_grp_0041ad60.c`, whose members 0x0041ad60, 0x0041ae20,
  0x0041b520 and 0x0041b630 already have exe bodies.
- 0x004b6700 (behind thunk 0x004b6550): its port body Zero4b6700 is in the .asi-only multi-function
  `Util/PromoLoop_sessionB.cpp`.

**Level 2 (callees of the newly compiled bodies), not compiled this step:** 0x00413bb0, 0x00475d90, 0x00475ea0,
0x00477870, 0x00485e10, 0x00486350, 0x00487140, 0x0048af70, 0x0048bc10, 0x0048cf70, 0x0048fdd0, 0x0048ffd0 (under
0x0040bd00); 0x0041a980, 0x00422140, 0x0041ec00, 0x0040cf40 (under 0x0040cf80); 0x00490000, 0x0045bed0, 0x0045bf30,
0x004725f0, 0x00426b40, 0x00405400, 0x00471df0, 0x004114e0 (under 0x0040cfd0); 0x00458880, 0x004593b0, 0x00404820
(under 0x0045b930); 0x0045e040, 0x00466a50 (under 0x00467010); 0x00555830, 0x00556e40, 0x00552b90 (under 0x00427620);
0x004768c0 (under 0x004210f0). Each row's reason (matched not compiled, .asi-only multi-function TU, .asi-only Match TU
with raw callees, static owned by an .asi body) is in BINDINGS.tsv. The full tree below AppDestroy is 371 functions to
depth 6 (`survey_tree.py`), so this step stops at level 2.

**RW-range callees with no librw binding yet:** RpWorldDestroy 0x004e5700 (under 0x0042c2a0), 0x004ce2d0 (RwImageSetPath
by rw catalog match), 0x004c5a60 (under 0x00428400), 0x004d41e0 (under 0x00496ce0), 0x004d7ff0 / 0x004d8480 (port bodies
only in the .asi-only Render/HighAB3Helpers_p6.cpp; the RwError path of 0x005581f0).

RW binding added: **RpClumpDestroy 0x004e6e00** -> `rw::Clump::destroy` (`MashedRw_ClumpDestroy_004e6e00`, null-safe,
logs each call). 0x004e6920 (C2, no name) is not bound: no evidence of its RW identity, and its three callers are all
stand-ins this step.

## Matched sources edited (object code unchanged)

Six matched sources could not compile as C++. Each change is preamble or prototype only, and each file is still
byte-identical under `re/tools/match/match.py --bytes` (XP DDK 9176) with `relocheck.py` OK:

| source | change | match.py | relocheck |
|---|---|---|---|
| s01/raw/00402a40.c | g2s raw preamble -> MATCH-S02 preamble, 5 externs -> MGLOBAL, `thunk_FUN_00493f70` / `thunk_FUN_00494460` prototypes `(int p0)` (the body passes 0) | 293 B BYTE-IDENTICAL | OK 50/50 |
| s01/004015a0.c | MATCH-S02 preamble, 2 externs -> MGLOBAL(A) | 129 B BYTE-IDENTICAL | OK |
| s01/00403660.c | MATCH-S02 preamble, 2 externs -> MGLOBAL | 54 B BYTE-IDENTICAL | OK |
| s03/raw_0041a3d0.c | raw preamble -> MATCH-S02, 1 extern -> MGLOBAL | 13 B BYTE-IDENTICAL | OK |
| s05/004210f0.c | `FUN_004768c0(void *p0)` (the body passes &DAT_0063e548) | 12 B BYTE-IDENTICAL | OK |
| s14/00484130.c | `typedef int code(int)` under `__cplusplus` only | 54 B BYTE-IDENTICAL | OK |

Converter: `verify/porttop_20261108/mglobal_convert.py` (keeps the files' CRLF).

## Lint and build

`cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"`: both targets OK. rva-lint `OK -- no NEW duplicate bodies (1
known)`, asm-lint OK. Two iterations fixed C++ compile errors in the sources (table above) and the `__fastcall` prototype
of the 0x0048af70 stand-in. Build 4 added the two RW-free-slot stand-ins (commit `0c80bce8d`).

## Knob-ON boot (`knob_on_1`, `MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000`, exe `98bb9591`)

Exit 0 after 3000 frames, 27.0 s. Teardown part of the trace (`knob_on_1.log`, repeated lines collapsed):

```
[ORIG_TOP] cfglog: Calling AppDestroy
[ORIG_TOP] 0x00402a40 AppDestroy (matched) enter: cameras 0x006905b0=08FB5080 0x006905b4=08FB51C0 0x0067f18c=08FB4860
           0x0067f190=08FB4E00, DAT_005f9bd8=0, DAT_00771a04=0, DAT_007f0a30=00000000, DAT_008a94a8=00000000
[PORT-TOP] rva 0x004d41e0: stand-in, not run      (0x00496ce0)
[PORT-TOP] rva 0x004114c0: stand-in, not run
[RW_BIND] RpClumpDestroy(0x00000000) x4           (0x0041a3d0 / 0x0041de70 / 0x0041c2c0 / 0x0041da80)
[PORT-TOP] rva 0x0041e0d0: stand-in, not run
[RW_BIND] RpClumpDestroy(0x00000000) x6, RwTexDictionaryDestroy(0x00000000), RpClumpDestroy x2   (0x0041d890 / 0x0041ccf0 / 0x0041c0e0)
[PORT-TOP] 0x0041b660, 0x00489250, 0x00412010, 0x00458880, 0x004593b0, 0x00404820
[RW_BIND] RwTexDictionaryDestroy(0x00000000)      (0x0045b930)
[PORT-TOP] 0x0041ffb0, 0x004196f0, 0x004768c0, 0x004014b0, then the 12 callees of 0x0040bd00
[RW_BIND] RwTexDictionaryDestroy(0x00000000) x4   (0x0040bd00)
[PORT-TOP] 0x00484130, 0x004d7ff0, 0x004d8480 (0x005581f0's RwError path), 0x00555830, 0x00556e40, 0x00552b90
[RW_BIND] RwTexDictionaryDestroy(0x00000000)      (0x00425ed0)
[RW_BIND] RpClumpDestroy(0x00000000) x130         (0x004015a0: 13 slots x 10)
[PORT-TOP] 0x004b6700, 0x0045e040, 0x00466a50, 0x004ce2d0, 0x004c5a60
[ORIG_TOP] 0x00402a40 AppDestroy (matched) returned; then HandTop_AppDestroy for the exe audio / DirectInput / small.mpg
[RW_BIND] RwTexDictionaryDestroy(0x08EAC780) -> rw::TexDictionary::destroy
[ORIG_TOP] 0x00493550 -> 0x004938c0 SoftwareTidyUpBeforeExiting ... RwEngineStop / Close / Term
[ORIG_TOP] 0x00493560 -> 0x004954f0 HardwareExitApplication (matched) returned 0
[ORIG_TOP] WinMain 0x00492370 returned 0 after 3000 frames
```

A stand-in logs only its first call. Every RW object pointer the matched teardown passes is 0, apart from the cameras:
the knob path never creates the clumps, worlds and dictionaries that 0x00402750 creates in the original. The values the
wrapper logs show three things. 0x0045b930's function-table loop (count DAT_005f9bd8) runs 0 times. The 0x00494460
video close is skipped (DAT_00771a04 = 0). 0x00484130 and 0x004114c0 would have received 0.

## Captures

`MASHED_NAV_DEMO=1 MASHED_DETERMINISTIC=1`, four runs knob ON, OFF, OFF, ON in root `D:/MashedWT/PORT-TOP-04C_rootK`,
exe `98bb9591`. `mashed_re_settings.bin` was restored from the baseline (`efd5da58...`) before each run. All four exit 0, each
with `NAV_DEMO phase=28 done ok=1`. Both knob logs contain the `AppDestroy (matched) returned` line, the hand logs do not.
**12 / 12 captures byte-identical across the four runs**, and identical to TOP-04B's table (`walk_02_mainmenu.bmp` =
`24249b64...`):

| capture | sha256 (16) |
|---|---|
| walk_01_title | 83c8cbb913899494 |
| walk_02_mainmenu | 24249b644e3e8057 |
| walk_03_singleplayer | feb2c803554cd028 |
| walk_04_colourselect | 2871487f1fcef51c |
| walk_05_abilityselect | 9f02cb91d09e0500 |
| walk_06_challengeselect | b1f095c07072b28d |
| walk_07_back_mainmenu | 3465159db8526481 |
| walk_08_options_highlighted | 843e3fc49d166754 |
| walk_09_options | 5db0164fb2c6df8c |
| walk_10_sound | ca2f64dc5e7a9407 |
| walk_11_sound_adjusted | 536f710d21e50619 |
| walk_12_back_mainmenu2 | 6d41bc3ac8ad8d8a |

## Default gates (knob OFF)

Exe `98bb9591` (commit `0c80bce8d`), `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, `A4_ROOT=D:/MashedWT/PORT-TOP-04C_root`.
All four arms exit 0 and self-exited (H_DEF 127 s, each B arm 13 s).

| gate | this branch | merge_train.json ref |
|---|---|---|
| G-CTRL | `8284be46/67931834/31fe043e` | same |
| B | `1d5bf4f9/1d5bf4f9/1d5bf4f9` | same |

Batch 1 (exe `08e7567e`, before the RW-free-slot fix) gave the same gate hashes and 12 / 12 identical captures. Its knob
boot and both knob nav runs exited `0xC0000005` in AppDestroy. Its data is in `D:/MashedWT/PORT-TOP-04C_data/batch1`.

## Machine rules

Each batch (knob boot, 4 gate arms, 4 nav demos) ran as one `D:/MashedWT/PORT-TOP-04C_data/run_all_04c.py` inside
`mashed_lock.py run --label PORT-TOP-04C`, inside `test-slot`. Batch 1 waited 2 h 43 min for a slot, batch 2 4 min.
A first knob-only queue entry was stopped while it was still waiting, before any launch. I killed only that entry's own
waiter PIDs. Builds: `mashed_lock.py status` showed `free` right before builds 1, 2 and 4. I did not re-check it before
build 3, which ran a few minutes after build 2.

## Port column (scripts/backfill_exe_file.py, then scripts/backfill_port.py)

41 rows changed: **37 none -> called, 4 body -> called** (0x00426ba0, 0x00426c00, 0x00494ef0, 0x00556cd0). none -> called:
0x00402a40, 0x004015a0, 0x00403660, 0x0040bd00, 0x0040cf80, 0x0040cfd0, 0x0041a3d0, 0x0041beb0, 0x0041c0e0, 0x0041c2c0,
0x0041cb00, 0x0041ccf0, 0x0041d6d0, 0x0041d890, 0x0041da80, 0x0041de70, 0x0041f320, 0x004189e0, 0x004210f0, 0x00421590,
0x00425ed0, 0x00427620, 0x00428400, 0x0042c2a0, 0x0045b930, 0x00467010, 0x00467070, 0x0047ba10, 0x00494320, 0x00494460,
0x00494bc0, 0x00494f20, 0x00496970, 0x00496ce0, 0x004b4880, 0x004b6550, 0x005581f0. The column is static: 0x00494460 and
0x00494320 count as called but were not reached in these runs (DAT_00771a04 = 0). Both scripts warn about 3 malformed
records they leave untouched (0x004464c0, 0x0041f360, 0x00420870).

BINDINGS.tsv: `update_bindings_top04c.py`, 373 rows (matched 103, stand-in 62, port 41, bound 30, missing 12, rw 28,
shim 11, crt 2, under-stand-in 84). No `under-stand-in` row is left under 0x00402a40.

## Risks

1. The exe audio, DirectInput and small.mpg shutdowns still come from the hand stand-in `HandTop_AppDestroy`, after the
   matched body. Their state is not in the original globals the matched callees read.
2. On this path the matched teardown frees only the cameras, and every other pointer is 0, because 0x00402750 is a
   stand-in. Once 0x00402750 runs matched, the six null-deref stand-ins above must be compiled with it.
3. 0x00494460 and 0x00494320 are compiled into the exe but were not run (DAT_00771a04 = 0 on the knob path). 0x00494320
   calls through DAT_00771a10 and DAT_00771a14 with no null check.
4. `RpClumpDestroy` logs every call: about 150 lines per knob exit.

## Merge state

Not merged with `orch/integration`: 04A and 04B are not landed. When they land: `git -c merge.autoStash=false merge
orch/integration`, re-run both backfill scripts and the BINDINGS updater, rebuild, then re-run the batch.

## Raw data (D:/MashedWT/PORT-TOP-04C_data)

| file | sha256 |
|---|---|
| knob_on_1.log | 3e2e98a94cfcd2b17c4534916bcf5bdbbc43ecaa2514eb7db8b9f82bad30499e |
| navdemo_knob1.log / navdemo_knob2.log | 92fc43422351eae5ac1046f7f7894fd7f8ec19087260cae089397b99ecc2b04b / 2b72b00ed288eaf8d2448fc470510b48fd7c700665586979ef6a84553aa3139a |
| navdemo_hand1.log / navdemo_hand2.log | a65aa53e3afe724495162c91f3c62cd423407146723b2da09565c6cef8757175 / 64b08bc963df806126a979d214c57c0710494d9b7e80a95396f4d32c4c93637b |
| navdemo_*/walk_02_mainmenu.bmp (all four) | 24249b644e3e8057271f3f1c252514c7820abec3f3da15d316f39dbf156a9066 |
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| run_all.log | e0907bbc52f224ea05d1ff38fcf2138397578472438f47f529015665ec9ce0dd |
| batch1/knob_on_1.log (the 0xC0000005 run) | a840785960cfb39298afed526dc6d556b4cb2d2f4c98bda274dfbbec211e8bb2 |

## Next

- Run AppInitialiseOnBootup 0x00402750 matched, then compile the six null-deref stand-ins with it.
- Bind the RW memory functions (`*(0x007d3ff8)` +0x108 / +0x10c) so 0x004114c0 and 0x00484130 can run.
- Level 2 under the compiled bodies (list above), and librw bindings for RpWorldDestroy 0x004e5700 and 0x004ce2d0.
