# PREREG RWC-FRAME-492E90: the frame render 0x00492e90 on the RW core, menu-executed slice first

Run 42, 2026-11-13. Committed alone, before any arm. Top-down row (WORKER_RULES "Row kinds"): both builds,
`ddkobj.py check`, knob boots, self-test. No G-CTRL, no B, no 12 captures (those run with the knob off).

## Change

Only under `MASHED_ORIG_TOP=1 MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1` (all default OFF, as in
`verify/rwccorerun_20261111/PREREG_RWC-CORE-RUN.md`):

1. `PortTop_StandIn_00492e90` (Match/PortTop_00492370.cpp) calls `RwCore_00492e90`, the matched source
   re/match/s15/00492e90.c (100%) compiled as an RW core closure body, instead of the hand RenderFrame.
2. The closure generator (`verify/rwccorerun_20261111/gen_rwcore_tus.py`) follows 0x00492e90 and the game functions of
   its menu-executed slice (`SLICE_EXEC_reach_menu.tsv`: reachable from 0x00492e90 through functions the INT3 census
   `re/coverage/reach.tsv` marks `reached_menu = 1`; 44 game functions, 34 at 100%, 10 below) from their re/match
   sources. Every other game callee of a slice source is a logged stop (kind `stop`, never lifted, L-137 / L-141).
   RVAs the previous generation lifted stay lifted (no swap to the newer sub-100% sources).
3. New sub-knob `MASHED_RWC_FRAME_N` (default 1 = today's behaviour): the boot ends after the Nth RwRasterShowRaster
   on the core instead of the first.

Default path and knob-OFF paths: no change (the new TUs are `ORIGTOP-ONLY`, compiled only into the exe and reached
only under the frame knob).

## Arms (each inside `test-slot` -> `mashed_lock.py run`, private root `D:/MashedWT/RWC-FRAME-492E90_root`)

| arm | env | expected |
|---|---|---|
| vs_off / ddk_off | knob off | 0 after 600 frames; vs_off vs ddk_off log compare 0 differ |
| vs_frame / ddk_frame | `MASHED_RWC_FRAME=1` (N = 1) | 0x5257F001 and the Im2D compare of RWC-FIRST-FRAME-IM2D still IDENTICAL 18 / 18 |
| vs_frameN / ddk_frameN | `MASHED_RWC_FRAME=1 MASHED_RWC_FRAME_N=<n>` | reaches the main loop and 0x00492e90 on the core; ends at present n (0x5257F001), or names the first stop / AV, which is the next slice |

Pass for this row: both builds, `ddkobj.py check` with every 100% substituted function IDENTICAL (refusals named),
N = 1 arms unchanged, self-tests PASS, and the N > 1 arm's end point named with its evidence. A draw-for-draw compare
against `menu_draw_burst.py` applies once a frame of 0x00492e90 completes on the core.
