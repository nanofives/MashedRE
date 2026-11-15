# RESULT RWC-FRAME-CLUMP (runs 43 to 49; GREEN for the row scope: the cup.dff AV is gone on the head, N>1 boots and the draw compare re-run on the ON default; draw compare 80 of 82 per frame, the 2 missing draws in their own open rows)

Runs 43, 44 and 45, 2026-11-13, run 46 2026-11-14, run 49 2026-11-15. Branch `orch/RWC-FRAME-CLUMP`, merged with
`orch/integration` d21abc196 in run 49 (d1e5ac586 in run 46, first based on 69cd03777). Top-down row.

**Run 49 summary (section 13).** (1) **The old AV is gone on the integration head.** Head d21abc196 (after
RWC-FRAME-BOOT-AV 4d7419f39 and IMG-5-ORIGTOP-DEFAULT 2f667e727), built detached with no branch change: vs_frameN and
ddk_frameN (`MASHED_RWC_FRAME_N=120`) load cup.dff (`0x004cc230(2, 1, "cup.dff"): 24976 bytes`), reach present 120 and
end with the route's designed exit 0x5257F001, no `AV at` line, every self-test PASS. (2) The branch is merged with that
head (merge commit 5bc88749d, no conflict) and runs on the ON default (`MASHED_PORTSTATE_IMAGE` + `MASHED_OT_CODEPTR` on
under `MASHED_ORIG_TOP`). All ten arms end as designed on both builds. (3) **Draw compare, re-run: unchanged from run 46**:
80 of 82 original draws match per frame with `--tol-anim 8`, 38 strict, the same 2 missing draws (rows
RWC-FRAME-MOVIE-BACKDROP, RWC-FRAME-TITLE-LOGO, both open); VS2022 dump vs object-build dump GREEN 640 / 640. One VS2022
dump showed 2 extra draws per frame from 0x00474890 (a time-phase crossfade window of 0x0042e5b0), and a repeat launch of
the same exe did not: run-dependent, section 13.4. (4) The phase ladder stays a listed stand-in: PORT-TOP-BODY-43DFD0 is in
handoff, not merged. The 0x0043dfd0 row of `verify/porttop_20261108/BINDINGS.tsv` now names it with the reader and writer
evidence for DAT_0067eca4 / DAT_0067eca8 (section 13.3). **Self-test: PASS** on every arm: `[portstate-image] self-test
PASS (0 of 8 checks failed)`, `[OT_CODEPTR] self-test PASS (0 of 367 checks failed)`, on frame-route arms the binding
self-test `self-test PASS (0 of 11 checks failed)` and `[RWC-DEVICE] selftest PASS (0 fail)`, on the frameT arms
`[OT_TIMER] self-test PASS (0 of 6000 checks failed)`. Both builds `=== Build OK ===`. `ddkobj.py check`: IDENTICAL
1547/1583, DIFFERENT 21, not-in-map 15, the same 21 units as the head's own object build (this branch adds no DIFFERENT unit;
the 21 are the head's and are classified by DDKOBJ-DIFFERENT-REASONS, not by this row). G-CTRL / B / 12 captures: not run (top-down row).

**Run 46 summary (section 12).** (1) The core frame reaches `DAT_0067eca4` = 3: a phase-ladder stand-in of 0x0043dfd0
(source lines 310..338) runs on the RW core frame route, with the matched clock (`MASHED_OT_TIMER=1`) writing
`DAT_007f1004`. (2) The nine powerup teardown slots (type table 0x005f9998 + 0x18) hold the matched teardown bodies on
the frame route, and **vs_frameT / ddk_frameT now end `WinMain 0x00492370 returned 0 after 600 frames`, rc 0, with 1
clump alive at RwEngineStop** (run 45: AV in RwEngineStop, 103 alive). (3) **Draw compare: RED.** Strict: 38 of 82
original draws match per frame, 42 moved, 2 missing (run 45: 0 of 82). With `--tol-anim 8` (same colour, moved or
resized by up to 8 px, the checker jitter phase): **80 of 82 match per frame, 0 mismatched, 2 missing**, on both builds.
The 2 missing draws are rows RWC-FRAME-MOVIE-BACKDROP and RWC-FRAME-TITLE-LOGO. **Self-test: PASS** on every arm with
the frame route (binding self-test `self-test PASS (0 of 11 checks failed)`, `[RWC-DEVICE] selftest PASS (0 fail)`,
and on the OT_TIMER arms `[OT_TIMER] self-test PASS (0 of 6000 checks failed)`). Both builds Build OK. `ddkobj.py
check`: IDENTICAL 1241/1275, no 100% unit DIFFERENT. G-CTRL / B / 12 captures: not run (top-down row).

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

Note 2026-11-14: the advice above to set `PROYECTOS_TEST_HUB=off` is superseded. The hub was fixed 2026-11-13 21:00 and
Mashed is out of `hub.exclude`. Paths outside the repo, builds and harness-lock runs stay local automatically, so do not
set `PROYECTOS_TEST_HUB=off` for these runs.

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

## 12. Run 46 (2026-11-14): core frame to DAT_0067eca4 = 3, powerup teardown slots, draw compare 80 of 82

Worker on claude3 (Opus 5.5 high), lead cmwgrs33714x7mz1b0c5e2219. Merged `orch/integration` d1e5ac586 into the
branch first (`git -c merge.autoStash=false merge`, merge commit 7b4454fb1, no conflict). The merge brings
IMG-5-TEARDOWN-BODIES: matched bodies for the nine teardown targets (Match/PortTop_<rva>.cpp) and the provisional
0x00455400. All run 46 code is default OFF: it runs only on the RW core frame route (`RwCore_FrameRoute()`, i.e.
MASHED_ORIG_TOP + MASHED_RWC_CORE + MASHED_RWC_FRAME). L-124 holds: no librw binding added. No tracker edit, no
promotion. The run-45 null guard (row RWC-NULL-DESTROY-GUARD) is untouched.

### 12.1 Powerup teardown slots (type table 0x005f9998 + 0x18)

`Match/PortTop_0045bae0.cpp`: on the frame route, before the matched 0x0045bae0, each of the nine +0x18 slots that is 0
or holds its entry's original value gets the matched teardown body: 0x004568a0, 0x004540c0, 0x004555d0, 0x00457b10,
0x0045a790, 0x0045b2d0, 0x00454af0, 0x00456ca0, 0x004530b0 (the section 11.3 table, all with a matched source since
IMG-5). The same rule as the +0x14 init slots, so a MASHED_OT_CODEPTR binding (non-zero, not the original value) is kept.
Log: `type table +0x18 slots: 9 teardown pointers stored (9 matched), 0 already non-zero, of 9`.
Only reader of the slots: a byte scan of MASHED.exe.unpatched for the dword 0x005f99b0 finds 1 absolute reference,
.text 0x0045b957, inside 0x0045b930 (`PTR_FUN_005f99b0[i].m_fn`, re/match/s09/0045b930.c). The +0x14 base 0x005f99ac
has 1 reference, 0x0045bb45 (in 0x0045bae0).

Result, vs_frameT and ddk_frameT (`MASHED_RWC_FRAME_N=100000`, the 600-frame loop ends): AppDestroy 0x00402a40 returns,
**`[RWC_CORE] live clumps at RwEngineStop: 1`** (run 45: 103 and an AV in the clump module close), RwEngineStop
0x004c2f60 returns, **`WinMain 0x00492370 returned 0 after 600 frames`, rc 0**. The one survivor is Bomb.dff:
`live clump 1 at RwEngineStop: 0F827D38` = `DAT_00636c00 (Bomb.dff) = 0F827D38` (0x0045bae0 log line). Row
RWC-FRAME-BOMB-DESTROY. Control: the same arm with MASHED_OT_TIMER off (`vs_frameT_r46_timer0.log`) also ends rc 0 with
1 live clump, so the teardown result does not depend on the timer knob. [UNCERTAIN] the share of the 103 -> 1 drop due
to the +0x18 store and not to the merged IMG-5 bodies: a slots-off control on this tree was not run (in row
RWC-FRAME-BOMB-DESTROY).

### 12.2 Core frame to DAT_0067eca4 = 3

In the original the word goes 1 -> 2 -> 3 inside 0x0043dfd0 (re/match/s08/0043dfd0.c lines 317..338): phase 1 adds
`(int)(DAT_007f1004 * _DAT_005cc9fc)` to DAT_0067eca8 (0x0043e0a7) and at >= 0xff sets 2 and calls
`FUN_0043d2a0(DAT_0067ecac, 0)`, phase 2 calls `FUN_004325c0()` and adds `(int)(DAT_007f1004 * _DAT_005cda88)`
(0x0043e101) and below 0 sets 3. Constants read from MASHED.exe.unpatched: 0x005cc9fc = 0x447a0000 = 1000.0,
0x005cda88 = 0xc3fa0000 = -500.0. On the exe 0x0043dfd0 is the hand stand-in, whose menu keeps its phase in
`g_anim_phase` (Frontend/MenuNavSM.cpp:795) and writes 0x0067eca4 only for the length of one 0x004325c0 call
(Anim_Tick restores it), so the word stays 1.

`Match/PortTop_StandIns05.cpp`: `PortTop_RwcPhaseLadder_0043dfd0`, called from the 0x0043dfd0 stand-in on the frame
route only, transcribes those lines. Declared deviations of this stand-in: 0x0067f19c == 2 skips the ladder but is not
written; FUN_0043d2a0 is not called (54.3%, no exe body, the hand menu builds its own screen stack); FUN_004325c0 is not
called there (the hand Anim_Tick already runs the matched body once per frame); the constants are literals (the exe image
may not hold the original .rdata). Phases 3 and up (the 3 -> 4 write when DAT_0067e914 != 0, the menu rows) are not
ported here (PORT-TOP-08). The ladder needs DAT_007f1004: it is 0 on the frame route unless `MASHED_OT_TIMER=1` runs the
matched clock 0x00493480 (first vs_dump: `DAT_007f1004 = 0.000000` at call 60, phase stuck at 1). The dump and teardown
arms now set it.

Log (vs_dump, `CORE_DRAWDUMP_r46_summary.txt`): `DAT_0067eca4 1 -> 2, DAT_0067eca8 = 255, DAT_007f1004 = 0.016667,
DAT_0067e914 = 0, DAT_0067ecac = 22`, then `2 -> 3, DAT_0067eca8 = 0`; call 60 reads `DAT_0067eca4 = 3`.
[UNCERTAIN] from code reading: that the original reaches 3 at the same frame and with the screen 0x0043d2a0(22, 0)
builds. Row RWC-FRAME-LADDER-CHECK (original-side write watch of 0x0067eca4 / 0x0067eca8).

### 12.3 Draw compare (original 4 frames at DAT_0067eca4 = 3 vs core frames 200..207)

Original side: the run-45 capture `orig_burst_492e90.json` (`menu_draw_burst.py --frames 4 --delim 0x00492e90`, peek
`0x0067eca4 3, 0x0067ecb0 33, 0x00771968 1`). Core side: vs_dump / ddk_dump, `MASHED_RWC_FRAME_N=590 MASHED_OT_TIMER=1
MASHED_RWC_DRAWDUMP_SKIP=200 MASHED_RWC_DRAWDUMP_FRAMES=8`, delimiter 0x00492e90. Every dumped core frame: `80 Im2D prim
draws, 0 indexed; DAT_0067eca4 3 DAT_0067ecb0 33 DAT_00771968 1`. `drawlist_diff.py --scale-b 1`, orig_f0..3 vs
core_f0..3:

| build | tolerance | matched | mismatched | missing | extra | verdict |
|---|---|---|---|---|---|---|
| VS2022 | strict (`--tol-anim 0`) | 152 (38 / frame) | 168 (42 / frame) | 8 (2 / frame) | 0 | RED |
| VS2022 | `--tol-anim 8` | 320 (80 / frame) | 0 | 8 (2 / frame) | 0 | RED |
| object build | strict | 152 | 168 | 8 | 0 | RED |
| object build | `--tol-anim 8` | 320 | 0 | 8 | 0 | RED |

Run 45: 0 of 82 per frame. VS dump vs object-build dump: strict 304 matched / 336 mismatched, all mismatches on the two
builds' 0x00473ee0 jitter rows; with `--tol-anim 8` GREEN 640 / 640.

- The 42 moved rows per frame: all A-side `@0x474464,0x42e89c`, B-side `RwCore_00473ee0+0x70c`, same colour, displaced
  or resized by under 8 px (strict `MISMATCH(moved)` rows in `DRAWDIFF_r46_strict.txt`). With `--tol-anim 4` 155 match,
  with 8 and 12 all 320. They match only under the animation tolerance: row RWC-FRAME-JITTER-STRICT (align the jitter
  counter, then strict).
- Missing A[0] (x 0 y 0 w 512 h 512, `@0x473ebd,0x42e65a`): the backdrop quad of 0x00473c20 from 0x0042e5b0
  (re/match/s07/0042e5b0.c line 65). 0x00473c20 draws only when `FUN_00494ef0(0)` != 0 (re/match/s11/00473c20.c line 68),
  a thunk of 0x00493f70 = `return DAT_00771a04` (re/match/s15/00493f70.c). Logged on the core: `DAT_00771a04 = 00000000`.
  [UNCERTAIN] from code reading: the writer is the movie module 0x0043dfd0 line 300 calls (0x00494a80 / 0x00494480 /
  0x00494f30, stand-ins on this path). Row RWC-FRAME-MOVIE-BACKDROP.
- Missing A[81] (x 80 y 80 w 480 h 240, `@0x450c7a,0x428750`): the title quad, 0x00403050 -> 0x00428760 -> 0x00428610 ->
  0x00450b10 (all on the core: RwCore_00403050 / _00428760 / _00428610 / _00450b10). 0x00450b10 draws with no gate
  (re/match/s08/00450b10.c line 94). 0x00403050 calls 0x00428760 only when `DAT_00771964 != 0` (re/match/s01/00403050.c
  line 27). Logged on the core: `DAT_00771964 = 00000000`. Its writer is the matched 0x004283a0 (`DAT_00771964 =
  DAT_0067d964 = FUN_004c5cb0("MashedNEWLogo", 0)`, re/match/s06/004283a0.c), run from 0x00402750 after the font36.piz
  mount (logged OK, 26 entries); FUN_004c5cb0 routes to the provisional RwCore_004c5cb0 (ROUTED.tsv). [UNCERTAIN] why
  the core read returns 0: not traced. Row RWC-FRAME-TITLE-LOGO. This replaces the run-45 [UNCERTAIN] on the title gate.

### 12.4 Arms, builds, checks (final tree)

| arm | exit | note |
|---|---|---|
| vs_off / ddk_off | 0 / 0 | `WinMain 0x00492370 returned 0 after 600 frames`; logcmp 1186 / 1186, 1 differs (the cfglog clock line) |
| vs_frame / ddk_frame | 0x5257F001 | present 1, both self-tests PASS, 0 live clumps at the N = 1 engine teardown |
| vs_frameN / ddk_frameN (N=120) | 0x5257F001 | logcmp 207 / 207, 1 differs (the clock line) |
| vs_frameT / ddk_frameT (N=100000, OT_TIMER) | 0 / 0 | WinMain returned 0, 1 live clump (12.1); logcmp 242 / 242, 7 differ (clock line + the wall-clock `[OT_TIMER] call` lines) |
| vs_frameT, OT_TIMER off (control) | 0 | 1 live clump, WinMain returned 0 |
| vs_dump / ddk_dump (N=590, OT_TIMER) | 0x5257F001 | 8 frames dumped at phase 3, 80 draws each (12.3); logcmp 222 / 222, 6 differ (clock + `[OT_TIMER] call`) |

Self-test on every frame-route arm: `self-test PASS (0 of 11 checks failed)` (RwTexDictionaryCreate binding, calls 1
and 2) and `[RWC-DEVICE] selftest PASS (0 fail)`; on the OT_TIMER teardown arms `[OT_TIMER] self-test PASS (0 of 6000
checks failed)` (vs_frameT, ddk_frameT). 0 `FAIL` lines in any arm log.

Builds: VS2022 (`MASHED_NO_DEPLOY=1`) `=== Build OK ===`, rva-lint OK (249 known, 0 new); object build
(`MASHED_DDKOBJ=1`, NO_DEPLOY) `=== Build OK ===`, `[ddkobj] link rc=0`, units 1501, ddk objects 1222.
`ddkobj.py check`: **IDENTICAL 1241/1275, DIFFERENT 19, not-in-map 15** (`BYTECHECK.tsv` refreshed). The 19 DIFFERENT are
the 18 of run 45 plus 0x00498510, a 95.3% provisional source from the merge (BIND-CRT-498510, PROVISIONAL.tsv). No unit
with a 100% source is DIFFERENT. Exe sha256 prefixes: mashed_re.exe 71cb8f749ebfca5d, mashed_re_ddk.exe
dda09c71bfeb1927 (rebuilt from the branch, not committed). G-CTRL / B / 12 captures: not run (top-down row, the knob is
off for those hashes).

`gen_rwcore_tus.py` regenerated after the merge: RwCore_Missing.cpp drops the stub for 0x004dcc4c (a label inside a
known function), renumbering the stop codes after it; RwCore_004dcbb0.cpp drops the matching define; LIFT_REFUSED.tsv /
RwCore_Bodies.tsv lose the row. `exe_sources.rsp`: the generator reordered it (same 1823 files); the committed order is
restored so the default exe's link order does not move.

Process notes: every game launch went through `test-slot` (outside, no hub override) -> `mashed_lock.py run` (inside),
private root `D:/MashedWT/RWC-FRAME-CLUMP_root`. No build ran during a PRIORITY-HOLD or SOLO (`mashed_lock.py status`
before each build showed the machine lock free or `HELD` by SOAK-P0, never PRIORITY-HOLD / SOLO). The final arm batch
waited 10 min on the test-slot memory gate and then started under the cap. `mashed_lock.py status` takes 38 s here
because its priority-file glob scans %TEMP% (606,377 entries): row HB-MASHEDLOCK-TEMPGLOB.

**RO-FIRST-DIVERGENCE's compare has to be re-run after this row merges** (the frame-route teardown slots and the phase
ladder change what the core frame runs).

Changed files: `Match/PortTop_0045bae0.cpp`, `Match/PortTop_StandIns05.cpp`, the regenerated `RwCore/RwCore_*.cpp` and
`verify/rwccorerun_20261111/{LIFT_REFUSED.tsv, RwCore_Bodies.tsv}`, `verify/rwcframeclump_20261113/run_arms.sh` (arms
vs_frameT / ddk_frameT / vs_dump / ddk_dump). Committed extracts: `DRAWDIFF_r46_tolanim8.txt`, `DRAWDIFF_r46_strict.txt`,
`CORE_DRAWDUMP_r46_summary.txt`, `FRAMET_r46_extract.txt`, `BYTECHECK.tsv`. Raw data `D:/MashedWT/RWC-FRAME-CLUMP_data`
(`core_burst_r46.json`, `core_burst_ddk_r46.json`, the ten arm logs, `vs_frameT_r46_timer0.log` / `_timer1.log`,
`build_r46d.log`, `build_ddk_r46.log`, `ddkcheck_r46.out`, `orig_burst_492e90.json`), sha256 in
`RAW_SHA256_RWC-FRAME-CLUMP.txt` (run 46 block).

Open rows filed in run 46: RWC-FRAME-MOVIE-BACKDROP, RWC-FRAME-TITLE-LOGO, RWC-FRAME-LADDER-CHECK,
RWC-FRAME-BOMB-DESTROY, RWC-FRAME-JITTER-STRICT, HB-MASHEDLOCK-TEMPGLOB.

## 13. Run 49 (2026-11-15): AV re-measure on the head, merge with the ON default, arms and draw compare re-run

Worker cmwiaa4ko141jp015jk0u074g (claude2, Opus 5.5), lead cmwia8ww813v5p0158tzg90ck. No code change in this run:
the branch's run-46 code is merged with the head and re-measured. L-124 holds (no librw binding added). No tracker edit,
no promotion.

### 13.1 The old AV on the integration head (first step as briefed)

The worktree was checked out detached at `orch/integration` d21abc196 (descendant of RWC-FRAME-BOOT-AV 4d7419f39 and of
IMG-5-ORIGTOP-DEFAULT 2f667e727), built with `MASHED_NO_DEPLOY=1` (VS2022, then `MASHED_DDKOBJ=1`), and booted with
the run-44 arm settings (`MASHED_ORIG_TOP=1 MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1 MASHED_RWC_FRAME_N=120
MASHED_DET_FRAMES=600`, private root `D:/MashedWT/RWC-FRAME-CLUMP_root`), inside test-slot -> mashed_lock.py run.

| arm (head d21abc196) | exe sha256 | result |
|---|---|---|
| vs_frameN | mashed_re.exe `ac4b352ff915dbbe...` | rc 0x5257F001 after 6.8 s, 323 lines, `[RW_FS] 0x004cc230(2, 1, "cup.dff"): 24976 bytes`, presents 120, 0 `AV at` |
| ddk_frameN | mashed_re_ddk.exe `657a40e3510e478e...` | rc 0x5257F001 after 5.8 s, 323 lines, same cup.dff line, presents 120, 0 `AV at` |

Self-tests on both: `[portstate-image] ... self-test PASS (0 of 8 checks failed)`, `[OT_CODEPTR] ... self-test PASS (0 of
367 checks failed)`, RwTexDictionaryCreate binding `self-test PASS (0 of 11 checks failed)` (calls 1 and 2),
`[RWC-DEVICE] selftest PASS (0 fail)`. **The AV the row was opened for (`MASHED_RWC_FRAME_N=120` AVs in the cup.dff clump
load after the first present) does not reproduce on the head**, as RWC-FRAME-BOOT-AV reported for 830f22e19.

Setup note: the ON default loads `mashed_image.bin`. build.bat skips it in a worktree with no `original\` (`[skip]
mashed_image.bin: no ...\original\MASHED.exe.unpatched`), so the image was extracted once into the private root with the
build's own tool: `py -3.12 re/tools/image/extract_image.py --exe D:/MashedWT/RWC-FRAME-CLUMP_root/original/MASHED.exe.unpatched
--out D:/MashedWT/RWC-FRAME-CLUMP_root/original/mashed_image.bin` (sha256 81CC7775...AFCD8, 434400 B). The loader finds
it through `MASHED_ROOT\original` (Compat/PortStateImage.cpp line 148). Every arm logs `rdata seed: skipped,
MASHED_PORTSTATE_IMAGE loaded the image`.

### 13.2 Merge, builds, object build

`git -c merge.autoStash=false merge d21abc196`: merge commit 5bc88749d, no conflict (one file changed on both sides,
`Match/PortTop_00492370.cpp`, in different hunks). `gen_rwcore_tus.py` re-run on the merged tree: the RwCore TUs, bindings
and LIFT tables are unchanged apart from line endings; it reorders `exe_sources.rsp` (as in run 46) and re-adds 94 rows to
`PROVISIONAL_RWC.tsv` that MATCH-RWT-PORT-FOLLOWUPS (ea81a4bd6, ce2f65b40) dropped as byte-identical. Both outputs were
restored to the committed head version, so the merge keeps that row's edit. Proposed open row: GEN-RWCORE-PROVLIST-DRIFT
(the generator should read the dropped rows' status instead of re-adding them).

| build | result |
|---|---|
| VS2022 (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, rva-lint OK (313 known, no new duplicate bodies), mashed_re.exe `8d5f57574b30baf8...` |
| object build (`MASHED_DDKOBJ=1`) | `=== Build OK ===`, `[ddkobj] link rc=0`, units 1744, ddk objects 1533, mashed_re_ddk.exe `6be0cdc666273b0b...` |
| `ddkobj.py check` | IDENTICAL 1547/1583, DIFFERENT 21, not-in-map 15 (`BYTECHECK.tsv` refreshed) |

The 21 DIFFERENT units equal the head's own object build (`[ddkobj summary]` of the head build: same count, same list
0x004332a0 ... 0x005aea60). The branch adds no DIFFERENT unit. Refusals 11 (externalized-static-register-convention 7,
wrapper-redefines-globals 4), as on the head.

### 13.3 Phase ladder: stand-in with reader evidence (ledger note)

PORT-TOP-BODY-43DFD0 is in `handoff` (not merged), so the run-46 ladder (`PortTop_RwcPhaseLadder_0043dfd0`,
Match/PortTop_StandIns05.cpp, frame route only) stays a stand-in. It is now named in the 0x0043dfd0 row of
`verify/porttop_20261108/BINDINGS.tsv` with this evidence. Memory-form references to the two words in
MASHED.exe.unpatched (sha256 prefix bdcae093a30fbf22), from `memrefs.py` (this folder; `dword_refs.py` decodes 1 byte
back first and reads e.g. `mov ecx, [0x67eca4]` as `or eax, 0x67eca4`), owners from hooks.csv (`owner.py`, this
folder). Outputs: `MEMREFS_67eca4_r49.txt`, `MEMREFS_owner_r49.txt`.

| word | reads (site -> function) | writes (site -> function) |
|---|---|---|
| DAT_0067eca4 | 0x0042b8d0 -> 0x0042b8d0; 0x0042b8e0 / 0x0042b8f0 -> 0x0042b8d0 [UNCERTAIN: owner by nearest hooks.csv RVA below]; 0x0042b900 -> 0x0042b900; 0x00432080, 0x004321bb -> 0x00432080; 0x004325c0 -> 0x004325c0; 0x004331a0 -> 0x004331a0; 0x00433240 -> 0x00433240; 0x0043c5b3 / 0x0043c660 / 0x0043c702 -> 0x0043c5b0; 0x0043d7c0 -> 0x0043d7c0; 0x0043e08c, 0x004409ff -> 0x0043dfd0 | 0x004320ca .. 0x00432214 (10 sites) -> 0x00432080; 0x004331b1 -> 0x004331a0; 0x00433249 -> 0x00433240; 0x0043e0e3, 0x0043e126, 0x0043e39a, 0x00440a4d, 0x00440a97 -> 0x0043dfd0 |
| DAT_0067eca8 | 0x0042a9f0 -> 0x0042a9f0 (byte read); 0x0042abc0 -> 0x0042aae0; 0x00432afb -> 0x00432ad0; 0x00433ff4 -> 0x00433f40; 0x0043e0b8, 0x0043e112 (RW), 0x00440a29, 0x00440a6f -> 0x0043dfd0 | 0x00432b0a / 0x00432b1f -> 0x00432ad0; 0x00433fff / 0x0043401e -> 0x00433f40; 0x0043e0c6, 0x0043e0d9, 0x0043e112, 0x0043e120, 0x00440a37, 0x00440a40, 0x00440a79, 0x00440a9d -> 0x0043dfd0 |

One dword hit, 0x0042ab74 (inside 0x0042aae0 by owner lookup), has no memory-form decode (`NO-MEM-DECODE`): it is an
immediate or a displacement with a base. [UNCERTAIN] what it is, not disassembled from the function start.
The ladder writes only inside 0x0043dfd0's range (0x0043e0c6 .. 0x0043e126 in the original); the other writers
(0x00432080, 0x004331a0, 0x00433240, 0x00432ad0, 0x00433f40) are not touched by this row.

### 13.4 Arms on the merged branch (final tree)

| arm | exit | note |
|---|---|---|
| vs_off / ddk_off | 0 / 0 | `WinMain 0x00492370 returned 0 after 600 frames`; logcmp 1633 / 1633, 1 differs (the cfglog clock line) |
| vs_frame / ddk_frame | 0x5257F001 | present 1, 0 live clumps at the N = 1 engine teardown |
| vs_frameN / ddk_frameN (N=120) | 0x5257F001 | presents 120 on the RW core; logcmp 206 / 206, 1 differs (the clock line) |
| vs_frameT / ddk_frameT (N=100000, OT_TIMER) | 0 / 0 | `WinMain 0x00492370 returned 0 after 600 frames`, `live clumps at RwEngineStop: 1` (Bomb.dff, row RWC-FRAME-BOMB-DESTROY), ladder 1 -> 2 -> 3; logcmp 241 / 241, 7 differ (clock line + `[OT_TIMER] call` lines) |
| vs_dump / ddk_dump (N=590, OT_TIMER) | 0x5257F001 | 8 frames dumped at `DAT_0067eca4 3 DAT_0067ecb0 33 DAT_00771968 1` |

0 `FAIL`, 0 `AV at`, 0 `STOP` lines in all ten logs (`ARMS_r49_summary.txt`). Self-tests as in the summary.

**Draw compare** (`drawlist_diff.py --scale-b 1`, original `orig_burst_492e90.json` frames 0..3 vs core frames 0..3,
core dump `MASHED_RWC_DRAWDUMP_SKIP=200 MASHED_RWC_DRAWDUMP_FRAMES=8`):

| dump | strict (`--tol-anim 0`) | `--tol-anim 8` |
|---|---|---|
| VS2022, repeat launch (r49b) | RED match 152 / mismatch 168 / missing 8 / extra 0 | RED match 320 / missing 8 / extra 0 |
| object build (r49 and r49b) | RED match 152 / mismatch 168 / missing 8 / extra 0 | RED match 320 / missing 8 / extra 0 |
| VS2022, first launch (r49) | RED match 152 / mismatch 168 / missing 8 / extra 4 | RED match 320 / missing 8 / extra 4 |
| object build vs VS2022 (r49b) | | GREEN 640 / 640 |

The missing 8 = the same 2 rows per frame as run 46: A[0] (0, 0, 512 x 512, `@0x473ebd,0x42e65a`, row
RWC-FRAME-MOVIE-BACKDROP) and A[81] (80, 80, 480 x 240, `@0x450c7a,0x428750`, row RWC-FRAME-TITLE-LOGO). The ladder call-60
line still logs `DAT_00771964 = 00000000` and `DAT_00771a04 = 00000000` with the image loaded, so the ON default does not
open either gate. `DRAWDIFF_r49_tolanim8.txt` is the r49b VS2022 compare.

**The first VS2022 dump's 2 extra draws per frame.** core_f2..f7 of the first vs_dump hold 82 draws (f0, f1: 80); the
two extra are x 288 / 464, y 64, 176 x 352, alpha 0x10 on f2 rising by 0x10 per frame to 0x60 on f7, return offsets
0x21a109 / 0x21a262 = `_RwCore_00474890+0x3B9` / `+0x512` (mashed_re.map). The object build's dumps call the same two
sites once per frame (`_RwCore_00474890+0x3B9` / `+0x512` in mashed_re_ddk.map), the VS2022 first dump twice. A repeat
launch of the same VS2022 exe (sha256 8d5f5757...) dumped 80 draws in all 8 frames, so the count is run-dependent, not
a build difference. Caller: 0x0042e5b0 calls 0x00474890 once when only one of `a1` / `a2` is non-zero and twice in the
crossfade windows of `t = (int)(DAT_007f1010 * 60) & 0x1ff` (re/match/s07/0042e5b0.c lines 113..126), and the alpha
`v << 4` steps by 0x10 per frame. [UNCERTAIN] from code reading: that the first launch's dump frames fell in such a
window because DAT_007f1010 follows the wall clock under `MASHED_OT_TIMER=1`; DAT_007f1010 is not logged per frame.
Same class as RWC-FRAME-JITTER-STRICT (a time-phase counter not aligned with the original capture). Evidence:
`DRAWDIFF_r49_vs_vs_ddk_timephase.txt` (first-launch VS2022 vs object build, extra 12 = 2 x 6 frames).

### 13.5 Process

Every game launch went through `test-slot` (outside, no hub override) ->
`mashed_lock.py run --label RWC-FRAME-CLUMP` (inside). `mashed_lock.py status` showed the lock free before each build
(never PRIORITY-HOLD / SOLO); one arm batch queued behind a merge-train solo window and started after it. Each batch
waited 10 min on the test-slot memory gate and started under the cap. No PID killed. No `original\` write.

Committed extracts: `ARMS_r49_summary.txt`, `DRAWDIFF_r49_tolanim8.txt`, `DRAWDIFF_r49_vs_vs_ddk_timephase.txt`,
`MEMREFS_67eca4_r49.txt`, `MEMREFS_owner_r49.txt`, `memrefs.py`, `owner.py`, `BYTECHECK.tsv`. Raw data
`D:/MashedWT/RWC-FRAME-CLUMP_data` (`r49head/` for 13.1), sha256 in `RAW_SHA256_RWC-FRAME-CLUMP.txt` (run 49 block).

**RO-FIRST-DIVERGENCE's compare has to be re-run after this row merges** (unchanged from run 46).

Open rows unchanged from run 46: RWC-FRAME-MOVIE-BACKDROP, RWC-FRAME-TITLE-LOGO, RWC-FRAME-LADDER-CHECK,
RWC-FRAME-BOMB-DESTROY, RWC-FRAME-JITTER-STRICT (add the 0x0042e5b0 crossfade phase to its scope). Proposed new row:
GEN-RWCORE-PROVLIST-DRIFT (13.2).
