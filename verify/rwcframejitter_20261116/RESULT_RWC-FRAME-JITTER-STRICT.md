# RESULT RWC-FRAME-JITTER-STRICT (run 51, 2026-11-16): GREEN, strict draw compare 82 / 82 per frame on both builds

Branch `orch/RWC-FRAME-JITTER-STRICT` from `orch/integration` 59935c224. Top-down row (dump-only knob, default OFF).
PREREG: `PREREG_RWC-FRAME-JITTER-STRICT.md` (committed alone, 2430e88ab).

## Verdict
With the original's per-frame clock pinned on the core, `drawlist_diff.py --scale-b 1 --tol-anim 0` (strict) gives
**82 matched / 0 mismatched / 0 missing / 0 extra on all 4 frames, on the VS2022 build and on the object build**.
The row target was 80 / 82 strict, then 82 / 82 after the two residual rows: both of those rows
(RWC-FRAME-MOVIE-BACKDROP, RWC-FRAME-TITLE-LOGO) are merged on the base head, and the core now draws A[0] (`@0x473ebd`)
and A[81] (`@0x450c7a`). Unpinned control on the same exe: 40 strict + 42 `MISMATCH(moved)`, all 42 the jitter rows
(the run-46 picture). So the alignment closes the row.

Bit level (`bitcmp.py`, `BITCMP.txt`, multiset match per frame on raw vertex bytes):

| core dump | x, y, z, rhw, colour bit-identical | note |
|---|---|---|
| object build, pinned | **82 / 82** every frame | |
| VS2022, pinned | 69 / 82 every frame | the 13 others are 12 draws from `_RwCore_00473220+0x1c2` and 1 from `_RwCore_004733b0+0x1c2` (mashed_re.map), the arc bars of 0x00473ee0: VS2022 recompiled floats, inside the 1 px compare tolerance. Expected per L-125 |
| VS2022, not pinned | 27 / 82 | |

## The counter (cited)
- **DAT_007f1010** (float). The jitter of 0x00473ee0 reads it once per quad: `fld dword ptr [0x7f1010]` at 0x00474167,
  `fadd st(0), st(0)` at 0x0047416d, stored at 0x0047416f (`[esp+0x58]`), then added to each corner index before `fsin`
  at 0x00474182, 0x0047419a, 0x004741d2, 0x004741f2, 0x00474208, 0x00474220, 0x00474234 (capstone over
  `original/MASHED.exe.unpatched`; source form re/match/s11/00473ee0.c `w2 = DAT_007f1010 + DAT_007f1010`).
- Writer on the menu: 0x00493390 (re/match/s15/00493390.c, `DAT_007f1010 = (float)now * 3.3333333e-07f`), run from the
  clock 0x00493480, outside 0x00492e90.
- Original capture with a read at every draw (`--peek-draw 0x007f1010`): one value per frame, constant across the
  frame's 82 draws: f0 0x447D34AD (1012.823059), f1 0x447D362C (1012.846436), f2 0x447D3743 (1012.863464),
  f3 0x447D3857 (1012.880310). Core, unpinned: about 241 (0x43713F88) at dump frame 200, so the two sides
  always disagreed by the clock, not by the code.

## What changed (default OFF)
- `re/frida/menu_draw_burst.py`: `--peek-draw 0x..` (dwords read at every draw, stored as `"pk"` per record; schema
  unchanged without it), `--boot-wait` / `--burst-wait` (defaults 25 / 15 s as before). The first capture on this
  loaded machine (26 GB committed) got 0 frames within the fixed 25 s; the rerun with 120 / 60 s got 4 frames.
- `mashedmod/src/mashed_re/RwCore/RwCore_Knob.cpp`: `MASHED_RWC_DRAWDUMP_TPIN=<hex,...>`, read only when
  `MASHED_RWC_DRAWDUMP` is set. At the 0x00492e90 delimiter that opens dumped frame k it writes word k to DAT_007f1010
  and logs the old and new value. The `close:` log line is written at the next delimiter, after the next frame's clock
  update, so it shows the clock, not the pin. That the pin held across the frame's draws is shown by the data instead:
  the 42 jitter quads' x coordinates reproduce bit-exact from a float32 model at the pinned word (`recover_t.py`
  scoring: 168 of 336 coordinates every frame on both builds, all 168 x values, against 28 unpinned), and by the strict compare.

## Checks
- Builds: VS2022 `mashedmod\build.bat` OK, object build `MASHED_DDKOBJ=1` OK (`MASHED_NO_DEPLOY=1` both).
- `ddkobj.py check`: IDENTICAL 1561 / 1599, DIFFERENT 22, not-in-map 16, units linked 1549 / 1761, refusals 11
  (externalized-static-register-convention 7, wrapper-redefines-globals 4). The DIFFERENT list is the run-49 list
  (`verify/rwcframeclump_20261113/BYTECHECK.tsv`, 21) plus 0x004938e0, which came in with the base head
  (re/match/s15/004938e0.c is an 80.0% source, so DIFFERENT is expected). This row touches no matched unit.
- Knob boots (`verify/porttop_20261108/knob_boot.py`, `MASHED_ORIG_TOP=1 MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1
  MASHED_RWC_FRAME_N=590 MASHED_OT_TIMER=1 MASHED_RWC_DRAWDUMP_SKIP=200 MASHED_RWC_DRAWDUMP_FRAMES=4`): vs_tpin,
  ddk_tpin, vs_notpin all end with the route's designed exit 0x5257F001. **Self-test: PASS** in every log
  (`[portstate-image]` 0 of 8, `[OT_CODEPTR]` 0 of 367, `[RWC_CORE]` texdict 0 of 11, `[OT_TIMER]` checks failed 0),
  0 `FAIL`, 0 `AV at`. Each dumped frame: 82 Im2D prim draws, `DAT_0067eca4 3 DAT_0067ecb0 33 DAT_00771968 1`
  (`TPIN_LOG_extract.txt`).
- No G-CTRL / B / 12 captures (top-down row, WORKER_RULES).

## Observed, out of scope
- The u, v bytes of 80 of 82 draws differ on every build, pinned or not (`x,y,col,u,v` 2 / 82). Example: jitter
  quad 0, original v 0.0625 / 0.9375, core v 0.0693359375 / 0.9306640625, u equal. re/match/s11/00473ee0.c writes
  x, y, z, rhw and colour of DAT_00898a20 and no u, v, so these bytes are what an earlier draw left in the scratch
  vertices. drawlist_diff does not score them. One [UNCERTAIN] register line (RWC-JITTER-STALE-UV).

## Process
Every launch: `test-slot` outside (no hub override), `mashed_lock.py run --label RWC-FRAME-JITTER-STRICT` inside.
Memory gate waits of 10 min. Lock free before both builds (no PRIORITY-HOLD / SOLO). No PID killed. Private root
`D:/MashedWT/RWC-FRAME-JITTER-STRICT_root` (robocopy of `D:/MashedWT/HUDINIT-C_root`, MASHED.exe sha256 B9977DAB...
same as `original\MASHED.exe`, `.unpatched` = the anchor BDCAE093...). No `original\` write.

## Files
Committed: PREREG, this RESULT, `DRAWDIFF_core_tpin_ddk_t0.txt`, `DRAWDIFF_core_tpin_vs_t0.txt`,
`DRAWDIFF_core_notpin_vs_t0.txt`, `BITCMP.txt`, `TPIN_LOG_extract.txt`, `orig_burst_tpeek.peek.json`, `bitcmp.py`,
`recover_t.py` (float32 jitter model, reproduces x only; its y model is not the original's x87 sequence),
`RAW_SHA256_RWC-FRAME-JITTER-STRICT.txt`.
Raw (`D:/MashedWT/RWC-FRAME-JITTER-STRICT_data`, sha256 in `RAW_SHA256_RWC-FRAME-JITTER-STRICT.txt`):
`orig_burst_tpeek.json` 3e98aa9f..., `core_tpin_vs.json` cfbbcd9a..., `core_tpin_ddk.json` 67a6cc90...,
`core_notpin_vs.json` 78794c0a..., `vs_tpin.log` c3d20aae..., `ddk_tpin.log` ec995fcc..., `vs_notpin.log` 010e035e...,
`mashed_re.exe` c88af1e7..., `mashed_re_ddk.exe` 83991261....

## Reproduce
```
py -3.12 re/frida/menu_draw_burst.py --frames 4 --delim 0x00492e90 --label orig --boot-wait 120 --burst-wait 60 \
    --peek-draw 0x007f1010 --peek 0x0067eca4,0x0067ecb0,0x00771968,0x007f1010 --out <data>/orig_burst_tpeek.json
knob_boot.py <arm> <root> 240 <flags above> MASHED_RWC_DRAWDUMP_TPIN=447d34ad,447d362c,447d3743,447d3857 MASHED_RWC_DRAWDUMP=<json>
py -3.12 re/tools/drawlist_diff.py <orig> <core> --scale-b 1 --tol-anim 0
py -3.12 verify/rwcframejitter_20261116/bitcmp.py <orig> <core> ...
```
