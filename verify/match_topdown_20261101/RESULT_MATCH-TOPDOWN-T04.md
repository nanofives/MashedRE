# RESULT MATCH-TOPDOWN-T04 (2026-11-01)

Verdict: **BLOCKED.** 0 of the 16 RVAs can be swapped soundly into `mashed_re.exe`. Each has a one-line reason below.
No source change, no build, no game run, no tracker edit. The blocker is one fact: the exe's race render is librw
(`LibRw/RwRaceSubmit.cpp` passes fed by `D3d9Render/TrackRenderer.cpp`), and the matched frame 0x00410b30 and its
subtree draw through the original's RenderWare device and the original's per-subsystem tables, which the exe does not
populate. Unblocking it is a port-state migration (the exe keeping those tables and the course object live), not a swap.

L-72 (Mariano 2026-11-01): swap matched callers in, top-down. T04 is the render-frame unit of
`verify/match_topdown_20261101/RESULT_MATCH-TOPDOWN-MAP.md` (matched top 0x00410b30, called by FrameDispatch
0x00492e90; port RVA-less in `TrackRenderer.cpp`, `RwRaceSubmit.cpp`, `TrackHookSlots.cpp`). The soundness rule is
T13's: the matched source reads the original's globals, so a swap is sound only where the exe keeps those addresses
live. In the exe the original range is zero-filled exe memory (`exe_main.cpp` B17 image pad), `.rdata` constants
included. T03 (`RESULT_MATCH-TOPDOWN-T03.md`) could swap its top because the exe's HUD pass already had one place that
made the same choice from the same live words; T04 has no such place.

## The top: 0x00410b30

`re/match/s02/00410b30.c` (S02, 100%). What it needs, by line of the matched source:

| what the frame does | in the exe |
|---|---|
| RW render state through `((RwStateFn)DAT_007d3ff8[8])(...)` (device +0x20), 8 calls | `*(0x007d3ff8)` is the B15 fake device of `D3d9Render/RwIm2DBridge.cpp`; +0x20 is `Bridge_SetRenderState`, which sets the Im2D bridge's own D3D9 state. librw's render-state cache never sees it, so the race passes' state would drift |
| `FUN_004270f0(*PTR_PTR_005f2770[DAT_0063ba78])` and `FUN_00426640(...)`: the course render with the course object | the table behind 0x005f2770 has no exe writer (T13, U-9783), so the argument reads 0; 0x004270f0 has no exe body |
| `DAT_0063ba98`, `DAT_007f0f30`, `DAT_007f0f3c`, `DAT_0063b920` | no exe TU mentions any of the four |
| ~30 callees: car shadows 0x0040bde0, cars 0x0045b350 / 0x00420050, headlights 0x00413a00 / 0x00413a40, particles 0x00475e50 / 0x00475d30 / 0x00477810 / 0x00477a10, 0x0048fd70, 0x00490490, overlays 0x00429e10, ... | the exe draws each of these through RVA-less librw passes on port-owned state (`RwRaceFx.cpp` DrawObjShadows / DrawHeadlights / lens flare, `TrackRenderer.cpp` cars and props, `TrackHookSlots.cpp` track slots), in the order `RaceSubmit_Render` sets |
| `if (FUN_00426ba0() != 0) FUN_00429e10()` | 0x00426ba0 reads the course object DAT_0066d704, no exe writer (T03) |

Swapping the top would mean re-hosting the whole race render on the original's device and tables. That is an
architecture change of the default render path, not a by-name call from a reached port body.

## Per-RVA table

"exe body" = an RVA-bound body in an `exe_sources.rsp` TU (`RH_ScopedInstall` / `// 0x<rva>` grep). "exe callers" =
E8/E9 sites to the symbol in the T03 build of this base (`D:/MashedWT/MATCH-TOPDOWN-T03_data/exe_g1`, same
`orch/integration` render sources).

| rva | matched source | exe body | exe callers | reason |
|---|---|---|---|---|
| 0x00410b30 | `re/match/s02/00410b30.c` | none (port RVA-less) | - | above |
| 0x0040bde0 | `re/match/s02/0040bde0.c` | none | - | reads DAT_0063b8f0 / DAT_0063b8f4 (no exe mention) and calls 19 render callees (0x00412100, 0x00413f50, 0x00486220, 0x00487de0, ...); the exe draws object shadows in `RwRaceFx.cpp` DrawObjShadows from port state |
| 0x00412100 | `re/match/s02/00412100.c` | none | - | callee of 0x0040bde0; reads DAT_0063bb38, no exe mention |
| 0x00413f50 | `re/match/s03/raw_00413f50.c` | none | - | callee of 0x0040bde0; RW device slots, DAT_0063bd50 (no exe mention), callee 0x004770a0 no exe body |
| 0x00413a00 | `re/match/s03/h2_00413a00.c` | none | - | headlight build: DAT_0063bc58, DAT_0089bea0, DAT_008a94a0 have no exe mention; the exe's headlights are `RwRaceFx.cpp` DrawHeadlights (rfx2) |
| 0x00413a40 | `re/match/s03/perm_00413a40.c` | none | - | headlight draw: RW device slots (Im2D), DAT_0063bc54 / DAT_0089d4a0 / DAT_008a94a0 no exe mention |
| 0x00411ce0 | `re/match/s02/00411ce0.c` | `GhostSetupRender` (`Vehicle/GhostSetupRender.cpp`) | 0 | the exe body has no caller (callerless, L-68); RW device slots; reads DAT_0063bb0c / DAT_0063bb10 / DAT_0063bb28, which only that file mentions |
| 0x0041a960 | `re/match/s03/raw_0041a960.c` | forwarder in `GhostSetupRender.cpp` | 0 | callee of 0x00411ce0 only, which has no exe caller |
| 0x0041a840 | `re/match/s03/h1_0041a4a0.c` | none | - | records at 0x0063c630 (stride 0xc4), no exe mention; reached only via 0x0041a960 |
| 0x004189c0 | `re/match/s03/raw_004189c0.c` | none | - | no globals of its own, but its only work is calling 0x00419530 (unmatched, no exe body, calls RW Physics 0x005592f0) when 0x0040e350 == 6 and 0x0042fe50 != 0; no exe call site |
| 0x0041ebb0 | `re/match/s04/0041ebb0.c` | none (`TrackRenderer.cpp:10295` names its order only) | - | RW device slots, DAT_0063e49c / DAT_0063e4a0 no exe mention, callees 0x00476df0 / 0x004770a0 no exe body |
| 0x004072e0 | `re/match/s01/004072e0.c` | none | - | RW device slots; DAT_00639d80 / DAT_0063a5d0 are mentioned by exe TUs, DAT_0063a5d8 by none; callee 0x004b59c0 no exe body |
| 0x00406130 | `re/match/s01/raw/00406130.c` | none | - | callee of 0x004072e0; DAT_0063a4a0 no exe mention; calls RW 0x004e6680 and 0x00489c30 |
| 0x00404650 | `re/match/s01/00404650.c` | none | - | DAT_00636c00 no exe mention; calls RW 0x004c1480 / 0x004e6680 and 0x0041f290 |
| 0x0041f290 | `re/match/s04/0041f290.c` | none | - | reads the car record at 0x0063d9e0, which the exe keeps in port-only `s_rec` (`Race/CarRegistrant.cpp`, T13 0x0041f030); callee 0x004c0ed0 no exe body |
| 0x00403910 | `re/match/s01/004036a0.c` | none | - | RW device slots; DAT_00636b70 / b74 / b78 no exe mention; callee 0x004938e0 |

0x0040e350 (in the map's unlock list) is not counted here: MATCH-TOPDOWN-T13 swapped it (`GetRenderSubMode`).

## What would unblock it

Per subsystem, the exe would have to keep the original's state where the matched chain reads it: the course object
(0x0066d704, the 0x005f2770 table, U-9783), the per-subsystem tables listed above, and an RW device whose render-state
slot routes into librw's state cache instead of the Im2D bridge. Each of those is a port-state migration with its own
gate and parity risk, to be cut as its own row; the leaves then become swappable bottom-up from reached port bodies
(T13 shape), and the top last.

Deep gaps (map): 44, 0x00429e10 hides 11, 0x004270f0 5, 0x00419530 4, 0x00426640 3, 0x00448730 2, 0x00420050 1.

## Measurements

Static only: `grep` over the 341 `exe_sources.rsp` TUs for each global and RVA, the exe-caller scan of the T03 build,
and `D3d9Render/RwIm2DBridge.cpp:45-59,111-113,278` for the device slot. No `match.py` re-run (nothing swapped; the
S01-S04 results stand). Gates not run: no source change.
