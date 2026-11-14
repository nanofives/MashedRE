# RESULT RWC-FRAME-CLUMP (runs 43 to 45; RED: the draw compare runs and is RED on one named cause)

Runs 43, 44 and 45, 2026-11-13. Branch `orch/RWC-FRAME-CLUMP`, based on `orch/integration` 69cd03777. Top-down row.

**Run 45 summary (section 11).** (1) A core-side Im2D draw dump exists (`MASHED_RWC_DRAWDUMP`, default OFF) in the
schema of the original-side capture, and the draw compare ran: **RED, 0 of 82 original draws matched in each of 4 frame
pairs (missing 328, extra 0)**. One cause: the core frames run with `DAT_0067eca4` = 1 in all 585 captured frames, where
0x0043c5b0 returns before any draw; the original reference is taken at `DAT_0067eca4` = 3 (the burst tool waits for 3).
On the exe that global is driven by the hand menu stand-in 0x0043dfd0. (2) The AppDestroy AV is fixed: AppDestroy
0x00402a40 now returns on the core (VS and object build). The boot then stops at the next teardown step,
SoftwareTidyUpBeforeExiting 0x004938c0 -> RwEngineStop, with 103 clumps alive: 0x0045b930 skips the nine powerup
type-table destroy entries because their slots are 0 on the port, and 7 of the 9 destroy functions have no source.
**Self-test: PASS on both builds and every arm** (binding self-test `0 of 11 checks failed`, `[RWC-DEVICE] selftest
PASS (0 fail)`). `ddkobj.py check`: IDENTICAL 1216/1249, unchanged from run 44 (output byte-identical). The six arms pass
as in run 44. **RO-FIRST-DIVERGENCE's compare has to be re-run after this row merges.**

**Run 44 summary.** With `MASHED_RWC_FRAME_N=120` both builds now boot through AppInitialiseOnBootup, enter the main
loop and render **120 frames with `RwCore_00492e90` on the RW core**, then end with **0x5257F001** (VS2022 and object
build alike). Sections 6 to 10 are run 44. **Self-test: PASS on both builds and every frame arm**: the
RwTexDictionaryCreate binding self-test (`self-test PASS (0 of 11 checks failed)`, calls 1 and 2) and `[RWC-DEVICE]
selftest PASS (0 fail)`. `ddkobj.py check`: IDENTICAL for every substituted unit with a 100% source (section 8). **Not
run: the draw-for-draw compare** (section 10: there is no core-side draw dump yet). That is why the verdict is HANDOFF
and not GREEN. G-CTRL / B / 12 captures: not run (top-down row, the knob is off for those hashes).

| arm | exit | note |
|---|---|---|
| vs_off | 0 | `WinMain 0x00492370 returned 0 after 600 frames` |
| vs_frame | 0x5257F001 | present 1 (loading disc), both self-tests PASS |
| vs_frameN (N=120) | 0x5257F001 | 120 presents on the core, font / Rt2d camera set, engine teardown skipped (section 7) |
| ddk_off | 0 | vs_off vs ddk_off: 1146 / 1146 lines, 1 differs (the cfglog clock line) (`verify/rwcdevice_20261111/logcmp.py`) |
| ddk_frame | 0x5257F001 | as vs_frame |
| ddk_frameN (N=120) | 0x5257F001 | vs_frameN vs ddk_frameN: 195 / 195 lines, 1 differs (the clock line) |

## 1. Root cause of the AV at EIP 1 (RWC-FRAME-492E90 section 5, item 3)

One defect class, found in five executed provisional sources. Ghidra splits a stack buffer into separate scalar
locals. The source passes the first one's address to a reader (or clears / copies through a pointer from it), and the
port's VS2022 frame does not keep the pieces contiguous, so the read overruns the frame. The symptoms were a `ret` into
a stack value (EIP 1, [esp] = 0x37002 = the stream version) and a stack-cookie failure (0xC0000409). Each fix was found
by a boot with a temporary log trace in 0x004e7420 (removed before commit) and checked against the original's listing.

| rva | function (RW) | buffer in the original | fix | score before / after |
|---|---|---|---|---|
| 0x004e7420 | RpClumpStreamRead | 12-byte header at [esp+0x48] (0x004e7493) | rewritten from the disassembly (section 2) | 37.9 (ddk) / 52.3 (rw9044) |
| 0x004e9330 | RpGeometryStreamRead | 0x10 header [esp+0x18] (0x004e9377), 0x18 morph target [esp+0x28] (0x004e953b) | `int hdr[4]`, `int mt[6]` | 51.5 / 52.2 |
| 0x004e7060 | atomic stream read | 16-byte struct [esp+0x20], zeroed 0x004e70a8..0x004e70bb, read 0x004e70bf | `int ai[4]` | 39.5 / 49.8 |
| 0x0053fec0 | material texture-pair installer | 32-byte name [esp+0x18] (0x0053ff84 / 0x0053ff89 rep stosd) | `char local_20[32]` | 50.9 / 67.3 |
| 0x004e8120 | RpMaterialStreamRead | 7-dword struct [esp+0x1c] (0x004e816c rep stosd x7, read 0x004e8179) | `int local_1c[7]` | 50.2 / 53.8 |

All five stay provisional (L-126), not C4. `split_local_scan.py` (this folder) lists 80 candidates of this shape in
30 provisional sources of the closure. Most are separate size / version out-parameters and harmless. They were not
all checked.

## 2. 0x004e7420 source

`re/match/rwl02/004e7420.c` is rewritten from the disassembly under `TOOLCHAIN rw9044`: the original's 0x44 frame,
one RwError per error site, the geometry list read as an inlined helper (rw9044 inlines it and reuses the stream
parameter home for the count, as the original does at 0x004e76f0), the geometry list free inlined at five sites and
called (0x004e7010) at two, as in the original. Same call sequence as the original on every path. Residual (in the
source header): the ebx / edi swap (zero / counter vs stream), and the original's unfolded `lea eax,[esp+0x28]; test`
at 0x004e77d2 with the fail block placed first. `equiv.py`: NOT-PROVEN call-regargs (the register swap is visible to
callees). The same source with all six frees inlined scores 63.2%.

## 3. Boot progress (vs_frameN, MASHED_RWC_FRAME_N=120, private root D:/MashedWT/RWC-FRAME-CLUMP_root)

| build | where it stopped |
|---|---|
| vs1 (new 0x004e7420 only) | AV EIP 1, now inside 0x004e9330 (the clump exists) |
| diag3 (+ 0x004e9330, 0x004e7060) | AV in 0x0053fec0 <- lifted 0x0053f3c0 <- 0x004e1b60 <- 0x004e8120 |
| d4 (+ 0x0053fec0) | 0xC0000409 stack cookie in the material read |
| d5 (+ 0x004e8120) | **cup.dff and Tri.dff clump reads return** (1 geometry, 1 atomic). AV in `Orig_0048fef0` from `PortTop_TriPool_Call0048ff20`: the librw atomic view applied to a core atomic |
| d6 (+ `MashedRw_AtomicView_0048fef0` returns the atomic on the core) | past the tri pool. AV in `RwCore_004e69a0` (RpAtomicClone) from the powerup init `Orig_004548e0`: the atomic comes from `MashedRw_ClumpFirstAtomic_004b3f90`, a librw binding run on a core clump; `MashedRw_AtomicPartCode_004b5190` also ran on core atomics |
| d7 / d8 (+ game-side helpers on the core, section 4) | d7 rva-lint 3 ORIGTOP-ONLY pairs (allowlisted); d8 build OK, **boot not run** (L0 STOP) |

## 4. Game-side RW helpers on the core (not yet booted)

`gen_rwcore_tus.py` takes 0x004b3f90, 0x004b5190, 0x004b52f0, 0x004b5300 as roots (game RVAs followed like RW code,
with 0x004b4070, 0x004b52c0, 0x0055deb0). `route_bindings.py` then routes their four bindings to the core bodies
(`[RWC-FRAME]` guards, routed 71 -> 75). New sources in `re/match/rwfc/`, all 100% BYTE-IDENTICAL with relocheck OK:
0x004b3f90 and 0x004b4070 (ddk /O2 /Ob1 /G5), 0x004b5300 (ddk /O2 /GS /Ob1), 0x0055deb0 (rw9044). `match_ledger.tsv` has
no rows for them, so `RwCore_Bodies.tsv` labels the three game ones "provisional" with no score until the ledger is
rebuilt. `MashedRw_AtomicView_0048fef0` returns its argument under the frame route (no librw use, no new binding).
L-124 holds: no librw binding added.

Promotions: `re/promotions/RWC-FRAME-CLUMP.tsv` = 0x004b3f90 route match (dry run: 1 pass). 0x004b4070 and 0x004b5300
were refused by the dry run because they have no hooks.csv row (C1 by identity first, through re-classify). 0x0055deb0
is already C4.

## 5. Run 43 raw data

`D:/MashedWT/RWC-FRAME-CLUMP_data` (build logs, vs_frameN logs per iteration; `vs_frameN_d8.log` is a copy of d6, d8
did not boot), sha256 in `RAW_SHA256_RWC-FRAME-CLUMP.txt`. Run 44 booted d8's tree first (d9, section 6).

## 6. Run 44: crash loop from the powerup init to the main loop (vs_frameN, N=120)

Each row is one build of `iter.sh` (generate the core TUs, route the bindings, build, boot). The AV sites are resolved
against `mashed_re.map` by `mapsym.py`.

| build | where it stopped | cause | fix (all under `RwCore_FrameRoute()`, MASHED_RWC_FRAME only) |
|---|---|---|---|
| d9 | AV write 0x28 in `Orig_00459290` (powerup init, `lazer.dff` slots) | `MashedRw_MatrixCreate_004c57a0` (LibRw/RwBridge.cpp) returned 0: librw has no engine on the frame route, and route_bindings.py scans only RwApiBind.cpp | the binding calls `RwCore_004c57a0` (re/match/rwl01/004c57a0.c, 100%). It is the only `MashedRw_<rva>` binding outside RwApiBind.cpp |
| d11 | 19 s boot, through AppInitialiseOnBootup into the first frame: AV read 0 in `RwCore_00552d10` (Rt2d CTM push) from `RwCore_00427e00` | the Rt2d matrix stack 0x00912a84[0] is 0: the 0x00427ca0 HUD init ran as the stand-in ("needs RwGlobals +0x118") | `PortTop_00402750_Call00427ca0` (Match/PortTop_00552b60.cpp) runs the matched 0x00427ca0 under the frame route too (the core's RwGlobals is installed at 0x007d3ff8) |
| d12 | AV read 0x134 in `RwCore_005554d0` from `RwCore_00427680` | font 0x0067d838 = 0: 0x00427ca0's font loader 0x00427880 is the stand-in `PortTop_StandInHIB_00427880` | the selector `MatchS06_00427ca0_Sel00427880` calls the exe body `FUN_00427880` (Match/MatchS06_00427880.cpp, 100%), whose exe forwarders now use the PORT-TOP-04F stream bindings (the core RwStreamOpen has no RtFS) and `RwCore_00554390` for the font read. 0x00554390 is a generator root. 0x00556e90 goes to `RwCore_00556e90` (97.5%, provisional) through `MatchS06_00427ca0_Sel00556e90` |
| d13-d15 | build: 7 then 3 ORIGTOP-ONLY rva-lint pairs, then C2664 in `re/match/rwl04/00553620.c` (18.7%) | new core closure members next to exe-only / asi-only bodies; a provisional source VS2022 C++ refuses | allowlist lines (`re/tools/dual_copy_allowlist.txt`, tagged RWC-FRAME-CLUMP run 44); 00553620.c added to `LIFT_INSTEAD` (lifted, as the generator's rule says) |
| d16 | AV read 0x14 in `RwCore_00552e40` | Rt2d camera 0x00912c0c = 0: 0x00552b60 is called with camera 0 by 0x00427ca0, as in the original; the camera comes from 0x004275d0(0x004671a0(0)), still the stand-in | 0x004275d0 (re/match/s06, 100%) is a generator root; `PortTop_00402750_Call004275d0` (Match/PortTop_00552b60.cpp) calls `RwCore_004275d0`. Its game callee 0x00427580 binds to the exe body `FrontendQuadParamInit427580` through `EXT` |
| d18 | **12 s: 481 frames of `0x00492e90` on the core**, then AppDestroy 0x00402a40 AVs in `RwCore_004e6e00` from `Orig_0041a3d0` | the N=120 stop never fired: core callers of 0x004c7730 bound to the body, so only the binding's present counted | `CALL_VIA` `004c7730` -> `RwCore_FrameShowRaster` (gen_rwcore_tus.py; one core caller, 0x004c1be0). The rsp line for CALL_VIA files now skips a file already listed |
| d19 | present 120, then AV read 0x10 in `RwCore_004ccba0` (RwFreeListFree) from `RwCore_004e4130`, inside `RwCore_EngineStop` | RwEngineStop's plugin closes run over the game's live objects: a closed freelist. The original frees them in AppDestroy first | N > 1: `RwCore_FrameShowRaster` skips EngineStop / Close / Term and logs it; N = 1 is unchanged |
| d20 | **0x5257F001 after present 120** | | |

The AppDestroy AV of d18 (teardown on the core after the 600-frame loop) is not chased in this row (section 9).

## 7. Changes (all default-OFF: MASHED_ORIG_TOP + MASHED_RWC_CORE + MASHED_RWC_FRAME)

- `LibRw/RwBridge.cpp`: `MashedRw_MatrixCreate_004c57a0` -> `RwCore_004c57a0` under the frame route.
- `Match/PortTop_00552b60.cpp`: the 0x00427ca0 selector runs the matched body under the frame route; new selector
  `PortTop_00402750_Call004275d0` (`Match/PortTop_00402750.cpp` binds 0x004275d0 to it).
- `Match/MatchS06_00427ca0.cpp`: selectors for 0x00427880 and 0x00556e90 (stand-ins off the frame route, as before).
  `Match/PortTop_00556d70.cpp` uses the 0x00556e90 selector too (the brush create sets the default colours).
- `Match/MatchS06_00427880.cpp`: the exe forwarders call the PORT-TOP-04F stream bindings and `RwCore_00554390`. Its
  only exe caller is the frame-route selector. The .asi branch keeps the raw-address forwarders.
- `RwCore/RwCore_Knob.cpp`: N > 1 ends without the engine teardown. The final log line names the present count.
- `verify/rwccorerun_20261111/gen_rwcore_tus.py`: roots 0x00554390 and 0x004275d0, XFOLLOW 0x004275d0 and the four
  re/match/s28 callees 0x005c4ba0 / 0x005c4bb0 / 0x005c4c60 / 0x005c4df0, `EXT` 0x00427580, `LIFT_INSTEAD` 00553620.c,
  `CALL_VIA` 0x004c7730. Closure 1695 (553 match100, 293 provisional, 681 lift, 16 hand, 14 exe, 101 missing, 37 stop),
  845 TUs. 36 new core bodies: 22 match100, 12 provisional (listed in `PROVISIONAL_RWC.tsv`), 2 lift.
- `re/tools/dual_copy_allowlist.txt`: 9 ORIGTOP-ONLY lines (core copies next to exe-only / asi-only bodies).
- `re/tools/ddkobj/ddkobj.py`: section 8.

L-124 holds: no librw binding added. L-126: the provisional new members run on the knob path only, listed in
`PROVISIONAL_RWC.tsv`, none admitted or C4.

## 8. Object build and `ddkobj.py check`

Final: `IDENTICAL 1216/1249, DIFFERENT 18, not-in-map 15, units linked 1200/1483, refusals 11` (`BYTECHECK.tsv` in
this folder). All 22 new match100 core units are IDENTICAL, as are `RwCore_004c7730` and `RwCore_004275d0`. The 18
DIFFERENT: the predecessor's 11 (RESULT_RWC-FRAME-492E90.md section 4, unchanged) plus 7 new provisional core sources
(0x00552890 91.2, 0x005530f0 67.8, 0x005533d0 55.0, 0x005538a0 19.7, 0x00554200 69.5, 0x00554940 12.5, 0x005572c0 17.9).

**ddkobj.py fix (needs a reviewer's look, it changes the object build beyond this row).** The first object build
(ddk20) linked, but ddk_frameN stopped on the d12 signature (font 0). Cause: re/match/s06/00427ca0.c holds a `static`
copy of 0x00427880 for the C compile, and the DDK gives it a register convention (the original passes the name in EAX:
`lea eax,[esp+0x28]` at 0x00427d41, `call 0x00427880` at 0x00427d45). ddkobj.py externalizes such a helper copy to the
wrapper's bound name (`process_unit`, the `plain in excluded` branch) and keeps the in-object call sites, so the
external got its argument in EAX: the PORT-TOP-04F open logged `0x004cc230(2, 1, "")`. Before this row every such
target was an argument-ignoring stand-in or an exe body called the same wrong way. The branch now runs
`static_convention_ok` (the check MATCH statics already get) and refuses the unit with
`fail:externalized-static-register-convention`, so it keeps its VS2022 body. `ddkobj.py selftest`: OK, 0 failures.
Seven units are refused by it (`DDKOBJ_EXTREGCONV_REFUSALS.txt`): 0x004111c0 (FUN_004103a0), 0x00427ca0
(FUN_00427880), 0x00427e00 / 0x00427ff0 / 0x00428140 (FUN_00427680, FUN_004277a0), 0x00427f00 (FUN_00427680,
FUN_00427840), 0x00458920 (FUN_00458570). Effect on the count: IDENTICAL 1225 -> 1216. [UNCERTAIN] whether the other
six were computing wrong results in the object build before: their call sites pass arguments in registers to a
cdecl target by the same mechanism, but no run compared them.

## 9. Open items

1. Run 44 items 1 (draw compare) and 2 (AppDestroy AV) are done in run 45, section 11. Their open residue is listed there.
2. (merged into item 1)
3. From run 43, still open: strict members (L-144) 0x004e7420 52.3, 0x004e9330 52.2, 0x004e7060 49.8, 0x004e8120 53.8,
   0x0053fec0 67.3, 0x004e8c70 28.9, 0x004cc5e0 59.8, 0x005c47e0 80.0. Lifted executed members (L-141): 0x004f3e90,
   0x0053f3c0. Run 44 adds executed provisional members 0x00554390 33.5, 0x00556e90 97.5 and the 12 in section 7.
4. Stand-ins still logged on the N=120 boot: 0x004b5580 (six clumps, callback 0x004b5560 has no source), 0x0048ae00,
   0x00558240, 0x005c4ad0, 0x00494c80, powerup type-table entry 2 (0x00455400, 94.0%).

## 10. Handoff and raw data

Next session: start from this branch (`orch/RWC-FRAME-CLUMP`), build with `bash verify/rwcframeclump_20261113/iter.sh
<tag>`. **test-slot now sends `bash` runs to the notebook hub when it is online, where they fail with FileNotFoundError
(no bash on its PATH)**; the game runs need this PC's private root, so set `PROYECTOS_TEST_HUB=off` (and
`PROYECTOS_TEST_MEM_GB=3`: the 8 GB default waits up to 10 min for memory) before `iter.sh` or `run_arms.sh`. The
branch is in a mergeable state for a top-down row: all six arms pass, both self-tests PASS, every 100% unit IDENTICAL.

Raw data: `D:/MashedWT/RWC-FRAME-CLUMP_data` (`vs_frameN_d9..d20.log`, the six final arm logs, `build_d20.log`,
`build_ddk21.log`, `build_ddk22.log` (the ddkobj relink after the fix), `ddkcheck22.out`), sha256 in
`RAW_SHA256_RWC-FRAME-CLUMP.txt` (run 44 block). Every game launch went through `test-slot` -> `mashed_lock.py run`.
No build ran during a PRIORITY-HOLD or SOLO (iter.sh and the object builds wait on `mashed_lock.py status`).

## 11. Run 45 (2026-11-13): core-side draw dump, draw compare, AppDestroy teardown

All run 45 code is default OFF (MASHED_ORIG_TOP + MASHED_RWC_CORE + MASHED_RWC_FRAME, the dump also MASHED_RWC_DRAWDUMP).
L-124 holds: no librw binding added. No tracker edit, no promotion.

### 11.1 Core-side Im2D draw dump

`RwCore/RwCore_Knob.cpp`, `MASHED_RWC_DRAWDUMP=<json>` (frame route only). After the core's RwEngineStart it wraps the
Im2D RenderPrimitive slot of the engine instance, `[[0x007d3ff8] + 0x30]`, the slot `re/frida/menu_draw_burst.py` hooks on
the original. One record per call: the raw vertex blob (`num * 0x1c` bytes, `num` outside 1..64 read as 4, the burst
tool's rule) and 5 return addresses (exe offsets, `--map mashedmod/build/mashed_re.map`), schema `{"v", "r"}` plus
`prim` / `num`. The wrapper calls the saved slot, so the draw is unchanged. Indexed draws (+0x34) are counted per frame.
Frame delimiter `MASHED_RWC_DRAWDUMP_DELIM` (hex RVA, default 0x00492e90): marked by `PortTop_StandIn_00492e90`
(`Match/PortTop_00492370.cpp`) before the core frame render, or 0x0042e3a0 through a new `CALL_VIA` entry
(`gen_rwcore_tus.py`, `RwCore_DrawDumpFrame_0042e3a0`, called by 0x0043c5b0). `MASHED_RWC_DRAWDUMP_SKIP` /
`_FRAMES` select the frames. Under the dump knob the frame camera's begin-update (`[DAT_006905b0 + 0x18]`, called by
RwCameraBeginUpdate 0x004c1a00) is wrapped and its result logged.

`re/frida/menu_draw_burst.py --delim <rva>` (default 0x0042e3a0, unchanged) moves the original side's delimiter. Both
sides used 0x00492e90 in this run: the 0x0042e3a0 delimiter saw **0 entries in 120 core frames** (first vs_dump).

### 11.2 Draw compare: RED, one cause

| side | file | frames | draws per frame | DAT_0067eca4 | DAT_0067ecb0 | DAT_00771968 |
|---|---|---|---|---|---|---|
| original (`menu_draw_burst.py --frames 4 --delim 0x00492e90 --peek ...`) | `orig_burst_492e90.json` | 4 | 82, 82, 82, 82 | 3 | 33 | 1 |
| RW core (vs_dump, N=590, all frames) | `core_burst.json` | 585 | 0 in every frame (1 Im2D prim call in the whole run) | 1 in all 585 | 33 | 1 |

`drawlist_diff.py orig_burst_492e90.json core_burst.json` (orig_f0..3 against core_f100..103, `--scale-b 1`):
**VERDICT RED (match 0, mismatch 0, missing 328, extra 0)**, `DRAWDIFF_r45.txt`.

Cause, from the sources: in phase 1 (`DAT_00771968` = 1) and screen 33 the frame render 0x00492e90 calls 0x0043c5b0
between RwCameraBeginUpdate and RwCameraEndUpdate (re/match/s15/00492e90.c, case 1). The begin-update returns the
camera on every core frame (`drawdump: camera 08F6F364 begin-update ... = 08F6F364`, calls 1 to 180), so the gate is
open. 0x0043c5b0 returns before any draw when `DAT_0067eca4` is 0 or 1 (re/match/s08/0043c5b0.c: `== 0` return,
`== 1` calls 0x0042aae0(1) and two render states, then returns). Its 0x0042e5b0 / 0x0042e3a0 calls and the menu rows
run only for `DAT_0067eca4` >= 2. 81 of the 82 original draws per frame come from 0x0043c5b0 (return addresses
0x0043c678 / 0x0043c680, through 0x0042e3a0 and 0x0042e8b0 into 0x004730a3 / 0x00473213 / 0x004733a2 / 0x00474464 and
others). The 82nd comes from the title layer 0x00403050 (chain 0x00450c7a <- 0x00428750 <- 0x00428785 <- 0x00403083 <-
0x00493012), which the core frame calls for screen 0x21 and which also issues no Im2D draw there. [UNCERTAIN] why the
title layer draws nothing on the core: its gate was not traced in this run. The burst tool
waits for `DAT_0067eca4` = 3 before it captures (menu_draw_burst.py line 263). On the exe `DAT_0067eca4` stays 1 through
600 frames: the menu step that advances it is 0x0043dfd0, which runs as the hand stand-in on the top-down path
(`rva 0x0043dfd0: stand-in (PORT-TOP-05) hand menu navigation + input globals`, every frame). So the remaining draw
rows name 0x0043dfd0 (the menu stand-in row) first. A per-draw comparison of the 82 draws needs the core frame to reach
`DAT_0067eca4` = 3. [UNCERTAIN] which writer advances 0x0067eca4 from 1 to 3 in the original: not traced in this run
(needs a write watch on the original, e.g. `--watch-write` style, or the 0x0043dfd0 decomp).

### 11.3 AppDestroy teardown on the core

New arm `vs_frameT` / `ddk_frameT`: `MASHED_RWC_FRAME_N=100000`, so the 600-frame loop ends and AppDestroy 0x00402a40 runs
on the core.

1. Run 44 d18 AV (read 0x00000008 at RwCore_004e6e00+0x15 from `Orig_0041a3d0`): 0x0041a3d0 is
   `RpClumpDestroy(DAT_0063c620)` (re/match/s03/raw_0041a3d0.c). The only writer of 0x0063c620 in re/match is 0x0041a1e0
   (re/match/s03/h1_0041a1e0.c, the BTBPanel.dff load), a Panel-family stand-in on this path, so the argument is 0. The
   original's RpClumpDestroy has no null check, and in the original 0x0041a1e0 always runs first. The next AV, after the
   fix below, was the same class: read 0x00000008 at RwCore_004c5930+0x22 from `Orig_0041ccf0`
   (`RwTexDictionaryDestroy(DAT_0063cdd0)`).
   **Fix:** the ten routed RW Destroy bindings (0x004c5930, 0x004c5a60, 0x004e5700, 0x004d41e0, 0x004c1cf0, 0x004c0c20,
   0x004e6e00, 0x004c7650, 0x004e6920, 0x004e4d90) call a knob wrapper `RwCore_FrameDestroy0_<rva>` (route_bindings.py
   `SPECIAL`) that skips a 0 object, logs it (first 8 per RVA), returns what the librw binding returns for 0 (1, the
   camera 0) and runs the core body for any other value. The librw bindings skip 0 the same way. Skips in vs_frameT:
   RpClumpDestroy 8, RwTextureDestroy 4, 0x004d41e0 2, RwTexDictionaryDestroy 1 (`TEARDOWN_vs_frameT_r45.txt`).
2. `Match/PortTop_0045b930.cpp`: the selector for 0x004593b0 runs the matched body on the frame route too (it ran only
   with RWMOD, because its callee 0x004c5770 calls RwGlobals +0x11c). On the frame route RwGlobals is the core's engine
   instance, and the records 0x0068ba30..0x0068bb90 hold the clumps the matched 0x00459290 built (0x0045bae0 runs matched,
   POWERUPINIT-A). It frees 4 clumps: 107 -> 103 alive at RwEngineStop.
3. **Result: AppDestroy 0x00402a40 returns on the core** (`0x00402a40 AppDestroy (matched) returned`, VS and object
   build). The boot then stops in SoftwareTidyUpBeforeExiting 0x004938c0 -> RwEngineStop 0x004c2f60 -> module close
   0x004d8060 -> clump module close 0x004e6250 (lifted) -> RwFreeListForAllUsed 0x004ccce0 -> 0x004e62f0 -> 0x004e8ea0 ->
   0x004f3b60 -> 0x004e8030 -> 0x004d8060 -> plugin destructor 0x0053ec60 (lifted) -> RwFreeListFree 0x004ccba0: AV read
   0x00000010, the plugin's freelist DAT_007dc5e8 is 0 (0x0053ec78). Same chain on the object build.
4. Census (`RwCore_EngineStop`, read only, frame route): **103 clumps alive at RwEngineStop**. The identified ones
   (pointer match against the boot log, 6 of the 64 logged) are POWERUPINIT-A objects: type-table entries 0, 1, 3, 4, 5
   (first clones at 0x006886b8, 0x00688260, 0x0068a368, 0x0068bb80, 0x0068d174) and Bomb.dff (DAT_00636c00).
   Cause of the survivors: 0x0045b930 destroys the powerup types through the +0x18 slots of the type table 0x005f9998
   (9 x 0x40, re/match/s09/0045b930.c `PTR_FUN_005f99b0[i].m_fn`). On the port those slots are 0 (the image pad holds
   the range zero-filled, `MASHED_OT_CODEPTR` off), so every destroy entry is skipped. POWERUPINIT-A stores the +0x14
   init slots only (Match/PortTop_0045bae0.cpp). The original's +0x18 values (read from MASHED.exe.unpatched):

   | i | type | +0x14 init | +0x18 destroy | destroy source |
   |---|---|---|---|---|
   | 0 | 0x09 | 0x00456760 | 0x004568a0 | re/match/s09/004568a0.c, 100% |
   | 1 | 0x0a | 0x004548e0 | 0x004540c0 | re/match/s09/004540c0.c, 100% |
   | 2 | 0x0b | 0x00455400 | 0x004555d0 | none |
   | 3 | 0x0c | 0x004583a0 | 0x00457b10 | none |
   | 4 | 0x10 | 0x0045a6b0 | 0x0045a790 | none |
   | 5 | 0x11 | 0x0045b830 | 0x0045b2d0 | none |
   | 6 | 0x12 | 0x00454e70 | 0x00454af0 | none |
   | 7 | 0x13 | 0x004576f0 | 0x00456ca0 | none |
   | 8 | 0x07 | 0x00452f70 | 0x004530b0 | none |

   Next for the teardown: store the +0x18 slots as POWERUPINIT-A stores +0x14 (matched 0x004568a0 / 0x004540c0, the seven
   others need sources first), then rerun vs_frameT. [UNCERTAIN] the destroyer of Bomb.dff (DAT_00636c00) and of the 97
   unidentified clumps (the census logs 64 of 103).

### 11.4 Arms, builds, checks (final tree)

| arm | exit | note |
|---|---|---|
| vs_off / ddk_off | 0 / 0 | `WinMain 0x00492370 returned 0 after 600 frames`; logcmp 1146 / 1146, 1 differs (the cfglog clock line) |
| vs_frame / ddk_frame | 0x5257F001 | present 1, both self-tests PASS; the N = 1 engine teardown now also logs the census line (0 live clumps) |
| vs_frameN / ddk_frameN (N=120) | 0x5257F001 | present 120; logcmp 195 / 195, 1 differs (the clock line) |
| vs_frameT / ddk_frameT (N=100000) | 0xC0000005 | AppDestroy returns, AV in RwEngineStop (11.3) |
| vs_dump (N=590) | 0x5257F001 | 585 frames dumped, 0 draws each (11.2) |

Self-test on every arm with the frame route: binding self-test `self-test PASS (0 of 11 checks failed)` and
`[RWC-DEVICE] selftest PASS (0 fail)`. Builds: VS2022 Build OK, object build (`MASHED_DDKOBJ=1`) Build OK, link rc 0.
`ddkobj.py check`: IDENTICAL 1216/1249, DIFFERENT 18, not-in-map 15, the `result` column of BYTECHECK.tsv unchanged from
run 44 and `ddkcheck_r45.out` byte-identical to run 44's `ddkcheck22.out`. Refusals unchanged: the 7
`fail:externalized-static-register-convention` units of section 8 (ddkobj.py guard logic not touched, row
DDKOBJ-REGCONV-FALLBACK-7). G-CTRL / B / 12 captures: not run (top-down row; the knob is off for those hashes).
`exe_sources.rsp`: the generator reordered its block (same file set); the committed order is restored so the default
exe's link order does not move.

Process notes: every game launch went through `test-slot` -> `mashed_lock.py run` with `PROYECTOS_TEST_HUB=off`. One
object build (`build_ddk_r45f.log`) started while the merge train held the machine lock (`HELD ... merge-train:20261113_212426`,
not PRIORITY-HOLD / SOLO); the other builds started with the lock free. One heredoc was used for a source edit (no
escapes involved).

**RO-FIRST-DIVERGENCE's compare has to be re-run after this row merges** (this row changes the frame route's RW
bindings and the 0x004593b0 selector).

Changed files: `RwCore/RwCore_Knob.cpp`, `RwCore/RwCore_0042aad0.cpp` (generated, CALL_VIA 0x0042e3a0),
`LibRw/RwApiBind.cpp` (generated [RWC-FRAME] lines), `Match/PortTop_00492370.cpp`, `Match/PortTop_0045b930.cpp`,
`re/frida/menu_draw_burst.py`, `verify/rwccorerun_20261111/{gen_rwcore_tus.py, route_bindings.py, ROUTED.tsv}`.
Committed extracts: `DRAWDIFF_r45.txt`, `CORE_DRAWDUMP_r45_summary.txt`, `TEARDOWN_vs_frameT_r45.txt`,
`orig_burst_492e90.peek.json`. Raw data `D:/MashedWT/RWC-FRAME-CLUMP_data`, sha256 in `RAW_SHA256_RWC-FRAME-CLUMP.txt`
(run 45 block).
