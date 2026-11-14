# RESULT RWC-FRAME-CLUMP (runs 43 + 44; HANDOFF: the draw compare is not run)

Runs 43 and 44, 2026-11-13. Branch `orch/RWC-FRAME-CLUMP`, based on `orch/integration` 69cd03777. Top-down row.

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

1. **The draw-for-draw compare (not run).** The original side is `re/frida/menu_draw_burst.py` (Im2D calls through the
   RwGlobals slot, frames delimited by entries to 0x0042e3a0). The exe's `MASHED_DBG_DRAWSTREAM` dump
   (`D3d9Render/DrawStreamDump.cpp`) is fed from the librw Im2D bridge, which the frame route does not use. Needed: a
   dump hook on the core's Im2D render-primitive slot (`RwGlobals +0x30` / `+0x34` in `RwCore_Probe`) writing the "v"
   schema, a frame delimiter on the core (0x0042e3a0 or the 0x00492e90 call), then `drawlist_diff.py`. Menu state on the
   exe side still comes from the hand stand-in 0x0043dfd0 (logged every frame), so a compare names that row as well.
2. The AppDestroy teardown on the core (d18: `RwCore_004e6e00` from `Orig_0041a3d0`, after the 600-frame loop).
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
