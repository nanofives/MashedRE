# RESULT RWC-FRAME-MOVIE-BACKDROP (run 50)

Row: core frame draw compare residual 1 of 2 from RWC-FRAME-CLUMP run 46: the 512 x 512 backdrop A[0] (0x00473c20 from
0x0042e5b0 @0x0042e65a) was not drawn on the RW core because its gate 0x00494ef0 -> 0x00493f70 read DAT_00771a04 = 0.
Top-down row. Worker cmwinxayc29ehp015e07kncrg (claude2, Opus 5.5), lead cmwikyrn41t37p015qalzoapy. Branch
`orch/RWC-FRAME-MOVIE-BACKDROP` from `orch/integration` d446c4b69 (no difference from 9a44eee36 under `mashedmod/`,
`re/match/` or `re/tools/ddkobj/`).

## Verdict

**GREEN for the row scope.** The writer of 0x00771a04 is measured on the original, the movie block of 0x0043dfd0 now runs
on the core frame route, and **A[0] matches in every compared frame pair on both builds** (draws per core frame 80 -> 81,
missing per frame 2 -> 1). The one remaining missing draw is A[81], row RWC-FRAME-TITLE-LOGO (sibling, not this row). The
draw compare as a whole stays RED on that row and on run-dependent extras of 0x00474890 (section 4), both outside this
row. **Self-test: PASS** on every arm (section 3). Both builds `=== Build OK ===`. `ddkobj.py check`: IDENTICAL 1557/1594,
DIFFERENT 22, the base's 21 plus the new unit 0x004938e0 (80% admitted source, section 5). G-CTRL / B / 12 captures: not
run (top-down row, the knob is off for those hashes). L-124 holds: no librw binding added.

## 1. Writer of 0x00771a04 on the original (measured; closes the row's [UNCERTAIN])

`run_watch.sh`: `re/frida/detorig.py` og1 boot recipe, private root `D:\MashedWT\RWC-FRAME-MOVIE-BACKDROP_root`,
`--watch 0x00771a04 --watch-from install` (hardware write watch armed at the PE entry, re-armed at the warp), tick cap 600,
warp at N=3529, race anchor N=3656, rc 0. 13 hits (`WATCH_771a04.jsonl`), all on the main thread (tid 17184). The hit pc is
the instruction after the store; stores from the disassembly of `MASHED.exe.unpatched` (SHA-256 BDCAE093...3C0E):

| hits | value | pc | store | function | return address (bt) |
|---|---|---|---|---|---|
| n 3 | 0x1 | 0x00494ab0 | 0x00494aa6 `mov [0x771a04], 1` | 0x00494a80 | 0x0049539c (call 0x00494a80 at 0x00495397 in 0x00495350) |
| n 4, 6, 9, 12 | 0x0 | 0x00494300 | 0x004942f6 `mov [0x771a04], 0` | 0x004942b0 (EC_COMPLETE with [0x771a08] = 0) | 0x00494485 (0x00494480) |
| n 4, 7, 10, 13 | 0x0 | 0x0049447f | 0x0049447a `mov [0x771a04], eax` (eax = 0 at 0x00494470) | 0x00494460 | 0x004954af (call at 0x004954aa in 0x00495350) |
| n 5, 8, 11 | 0x1 | 0x00494ab0 | 0x00494aa6 | 0x00494a80 | 0x004954c6 (0x00495350's movie loop) |
| **n 229** | **0x1** | **0x00494ab0** | **0x00494aa6** | **0x00494a80** | **0x0043e04a: the call at 0x0043e045 in 0x0043dfd0** |

The first 12 hits are the boot movies of 0x00495350 (four start / end / close cycles). Hit n 229 is the menu: 0x0043dfd0
calls the thunk 0x00494f00 (`jmp 0x494a80`, 0x00494f00) with (0, 0, 1) when 0x00494ef0 returns 0. No write follows in the
600 ticks (menu, warp and race). So the writer the backdrop gate reads is 0x00494a80 from 0x0043dfd0's movie block, as
the run-46 code reading said, now measured.

## 2. Change (all under MASHED_ORIG_TOP + MASHED_RWC_CORE + MASHED_RWC_FRAME, `RwCore_FrameRoute()`)

The movie block of 0x0043dfd0, from the disassembly (re/match/s08/0043dfd0.c lines 296..302):
`0x0043e030 push 0; 0x0043e031 call 0x494ef0`; == 0: `0x0043e042..0x0043e045 push 1, push 0, push 0, call 0x494f00`
(-> 0x00494a80); != 0: `0x0043e04f..0x0043e050 push 0, call 0x494f10` (`jmp 0x494480`, 0x00494f10), then
`0x0043e058 call 0x494f30`; then 0x0043c000 at 0x0043e05d.

- `Match/PortTop_StandIns05.cpp`: `PortTop_RwcMovieBlock_0043dfd0` runs that block before the run-46 phase ladder in the
  0x0043dfd0 stand-in (source order). Bodies: `RwCore_00494ef0` (re/match/s16/00494ef0.c, 100%, existing core TU),
  `FUN_00494a80` (Match/MatchS16_00494a80.cpp, 100%, already in the exe; its RwRasterCreate binding already routes to
  `RwCore_004c77c0` on this route), `PortTop_Movie_00494480` and `PortTop_Movie_00494f30` (new TU below). Logs calls 1, 2
  and 60. Declared deviation: `PortTop_MovieBind_Pump()` runs before 0x00494480 on the != 0 branch (the original's texture
  renderer copies frames on the DirectShow streaming thread; the binding's graph needs a caller on this thread).
  Not made here, as before: 0x0043c000 and 0x0040acd0.
- `Match/PortTop_MovieBind.cpp`: on the frame route the 0x004944c0 binding creates the movie texture on the core's device
  `[0x007d4110]` (the device the matched RW d3d9 driver stores; RwCore_Knob.cpp logs it as "device [0x007d4110]"), so the
  core's SetTexture of raster 0x00771a50 uses a texture of its own device. Off the route: the exe device, unchanged.
- `Match/PortTop_MovieCore.cpp` (new, exe only, `RVA-LINT: ORIGTOP-ONLY`): the matched sources of 0x00494480, 0x004942b0,
  0x00493fc0, 0x00494f30 (all 100% byte-identical, relocheck OK, `re/analysis/match_ledger.tsv`) and 0x004938e0 (80.0%,
  `re/match/ADMITTED.tsv` EQUIV, not C4) under `PortTop_Movie_` names. Argument flow of 0x00494480 from the disassembly:
  `0x00494485 mov eax,[0x6147d8]; 0x0049448a mov ecx,[eax*4+0x771a10]; 0x00494491 push ecx; 0x00494492 call 0x493fc0;
  0x00494497 push eax; 0x00494498 call 0x4938e0`, so 0x004938e0 gets (raster [0x00771a18], texture
  [0x00771a10 + 4 * [0x006147d8]]). The source's converter form passes one argument to each callee; in the port both are
  bound to adapters that pass those values explicitly (the DDK object of 0x00494480 calls the same adapters, so the two
  builds agree).
- `mashedmod/exe_sources.rsp`: one line, `Match\PortTop_MovieCore.cpp` after `Match\PortTop_MovieBind.cpp`.
- `re/tools/dual_copy_allowlist.txt`: four `ORIGTOP-ONLY CROSS-TARGET` lines (0x004938e0, 0x00493fc0, 0x00494480,
  0x00494f30; the .asi keeps its own copies).

Logged on the core (vs_dump, `ARMS_r50_summary.txt`): call 1 `0x00493f70 read 0 -> 0x00494a80(0, 0, 1); DAT_00771a04 =
00000001, DAT_00771a08 = 00000001, DAT_00771a18 = <raster>`, with `[OT-BIND-D3D] 0x004944c0(0) ... frontend.mpg ... video
512x512 tex 512x512`; call 2 and call 60 `0x00493f70 read 1 -> 0x00494480 + 0x00494f30`, DAT_00771a50 = DAT_00771a18,
DAT_00771a10 = the published texture, DAT_006147d8 = 1.

## 3. Arms, builds, self-tests

Private root `D:\MashedWT\RWC-FRAME-MOVIE-BACKDROP_root` (robocopy of `original\`, `mashed_image.bin` extracted with
`re/tools/image/extract_image.py`, sha256 81CC7775...AFCD8). `run_arms.sh` = the RWC-FRAME-CLUMP arms with this root.
Exe sha256: mashed_re.exe 1383c0782e229ffe..., mashed_re_ddk.exe 1f1a241fcd7317bf....

| arm | exit | note |
|---|---|---|
| vs_off / ddk_off | 0 / 0 | 1827 lines each, `WinMain ... returned 0` path unchanged (the stand-in's frame-route branch is not taken) |
| vs_frame / ddk_frame | 0x5257F001 | present 1, before 0x0043dfd0 runs (no movie line) |
| vs_frameN / ddk_frameN (N=120) | 0x5257F001 | movie block calls 1, 2, 60 logged as in section 2 |
| vs_frameT / ddk_frameT (N=100000, OT_TIMER) | 0 / 0 | `WinMain 0x00492370 returned 0 after 600 frames`; AppDestroy enters with `DAT_00771a04=1` and returns; `live clumps at RwEngineStop: 1` (Bomb.dff, row RWC-FRAME-BOMB-DESTROY, as in runs 46 / 49) |
| vs_dump / ddk_dump (N=590, OT_TIMER), twice | 0x5257F001 | `drawdump core_fN: 81 Im2D prim draws` in all 8 frames (run 49: 80), phase 3 |

0 `AV at`, 0 `FAIL` lines in all arm logs. **Self-tests PASS**: `[portstate-image] ... self-test PASS (0 of 8 checks
failed)`, `[OT_CODEPTR] ... self-test PASS (0 of 367 checks failed)` on every arm; on frame-route arms the
RwTexDictionaryCreate binding `self-test PASS` (calls 1 and 2) and `[RWC-DEVICE] selftest PASS (0 fail)`; on vs_frameT /
ddk_frameT `[OT_TIMER] self-test PASS (0 of 6000 checks failed)`.

Builds: VS2022 (`MASHED_NO_DEPLOY=1`) `=== Build OK ===`, rva-lint OK (318 known, NEW=0, after the four allowlist lines;
the first build stopped on exactly those four ORIGTOP-ONLY pairs). Object build (`MASHED_DDKOBJ=1`, NO_DEPLOY)
`=== Build OK ===`, `[ddkobj] link rc=0`. No build ran during a PRIORITY-HOLD or SOLO (`mashed_lock.py status` free before
each; the arm batches queued behind a merge-train solo window and started after it).

## 4. Draw compare

`drawlist_diff.py --scale-b 1`, original `orig_burst_492e90.json` (the run-45 capture, peek `0x0067eca4 3, 0x0067ecb0 33,
0x00771968 1`) frames 0..3 vs core frames 0..3 (`MASHED_RWC_DRAWDUMP_SKIP=200 MASHED_RWC_DRAWDUMP_FRAMES=8`):

| dump | `--tol-anim 8` | strict |
|---|---|---|
| VS2022 r50 | RED match 324 / mismatch 0 / missing 4 / extra 0 | RED match 156 / mismatch 168 / missing 4 / extra 0 |
| object build r50 | RED match 324 / missing 4 / extra 8 | RED match 156 / mismatch 168 / missing 4 / extra 8 |
| VS2022 r50b (repeat) | RED match 324 / missing 4 / extra 8 | RED match 156 / mismatch 168 / missing 4 / extra 8 |
| object build r50b (repeat) | RED match 324 / missing 4 / extra 0 | RED match 156 / mismatch 168 / missing 4 / extra 0 |

Run 49: match 320, missing 8 (A[0] and A[81] in each frame). Now **A[0] (0, 0, 512 x 512, `@0x473ebd,0x42e65a`) matches
in all 16 frame pairs**; missing 4 = A[81] (80, 80, 480 x 240, `@0x450c7a,0x428750`) once per frame, row
RWC-FRAME-TITLE-LOGO. The strict mismatches are the 42 jitter rows of run 46 (row RWC-FRAME-JITTER-STRICT).

The extra 8 (2 per frame, x 288 / 464, y 64, 176 x 352, alpha stepping 0x10 per frame) are at
`_RwCore_00474890+0x3B9` / `+0x512` (mapsym on `mashed_re_ddk.map`): the crossfade window of 0x0042e5b0 that run 49
(RWC-FRAME-CLUMP section 13.4) recorded as run-dependent. Here the first object-build dump and the repeated VS2022 dump
have them and the other two do not, so the count follows the launch, not the build. They are not from this change
(0x00474890 is called by 0x0042e5b0 independently of the 0x00493f70 gate, re/match/s07/0042e5b0.c lines 113..126). No row
exists for them; proposed below.

## 5. Object build check

`ddkobj.py check`: IDENTICAL 1557/1594, DIFFERENT 22, not-in-map 15, units linked 1544/1756, refusals 11
(`DDKCHECK_summary.txt`, `BYTECHECK.tsv`). Base: the fresh check of 9a44eee36 by the RWC-FRAME-TITLE-LOGO worker
(`D:/MashedWT/RWC-FRAME-TITLE-LOGO_data/ddkobj_check_base.out`, 2026-11-16 00:14, sha256 5817e6d6...e2b198): IDENTICAL
1553/1589, DIFFERENT 21. The 21 are the same RVAs in both lists (classified by DDKOBJ-DIFFERENT-REASONS). This branch adds
5 units: 0x00494480, 0x004942b0, 0x00493fc0, 0x00494f30 IDENTICAL, and **0x004938e0 DIFFERENT (new)**: 4 differing bytes
of 32 at +0x8. Classification: its source is 80.0% (`match_ledger.tsv`: "not byte-identical: 4 differing unmasked bytes of
28"), admitted by equiv.py (`ADMITTED.tsv`: EQUIV), so the DDK object is its 80% source compiled, not the original's bytes.
No unit with a 100% source is DIFFERENT.

## 6. Decision, open rows, process

Decision: none. No tracker edit, no promotion. Proposed open rows (the lead files them):
- RWC-FRAME-CROSSFADE-PHASE: the 0x00474890 extra draws (section 4) follow the launch; align DAT_007f1010 with the
  original capture's phase (same class as RWC-FRAME-JITTER-STRICT), then re-run the draw compare.
- [UNCERTAIN] whether the frameT AppDestroy now runs the close 0x00494460 through 0x00494f20: AppDestroy enters with
  `DAT_00771a04=1` and returns, but no close line is logged. Test: log the 0x00494460 call on the frame route.

`verify/porttop_20261108/BINDINGS.tsv` is not edited here (the sibling row edits it): its 0x00494a80 row still says
"under-stand-in"; on the frame route it is now reached from the 0x0043dfd0 stand-in's movie block.

Every game launch went through `test-slot` (outside, no hub override) -> `mashed_lock.py run --label
RWC-FRAME-MOVIE-BACKDROP` (inside). Kills: none (knob_boot.py and detorig.py end their own children).

Raw data `D:/MashedWT/RWC-FRAME-MOVIE-BACKDROP_data` (`w771a04.log`, `w771a04/`, build logs, `ddkcheck.out`, arm logs,
`core_burst_*.json`, `orig_burst_492e90.json`), sha256 in `RAW_SHA256_RWC-FRAME-MOVIE-BACKDROP.txt`. The vs_dump /
ddk_dump logs are the repeat's (r50b); the r50 dumps' draw counts are in the r50 DRAWDIFF files.
