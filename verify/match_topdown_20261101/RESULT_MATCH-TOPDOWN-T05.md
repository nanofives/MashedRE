# RESULT MATCH-TOPDOWN-T05 (2026-11-01)

Verdict: **GREEN, TEXT-ONLY.** 0 of the 12 unlock RVAs can be swapped soundly into `mashed_re.exe`; each has a
one-line reason below. No build input changed. T05 and T10 share this branch (`orch/MATCH-TOPDOWN-T05`, same
game-mode machine 0x004929d0, same Course::Finish 0x0040d270); T10's table is
`verify/match_topdown_20261101/RESULT_MATCH-TOPDOWN-T10.md`.

L-72 (Mariano 2026-11-01): swap matched callers in, top-down. T05 = the track-node dispatch subtree from
`RESULT_MATCH-TOPDOWN-MAP.md`: connector `0x004929d0 > 0x0040d440 > 0x0040d270* > 0x0040d020 > 0x00426e10*`
(`*` = unmatched on the map), 12 matched functions in 11 roots, all new bodies, 0 x87.

## Why nothing swaps: the exe does not run this chain, and its state is dead in the exe

The whole subtree is the original's **course load**: 0x0040d440 (case 2 of the game-mode machine,
`re/analysis/game_state/0x004929d0.md`) calls Course::Finish 0x0040d270 with `PTR_PTR_005f2770` and
`DAT_0063ba7c`, which reaches 0x0040d020 -> 0x00426e10 (per-node dispatch) -> the leaves below. The exe loads a
course through RVA-less port code (`D3d9Render/TrackRenderer.cpp`, `Race/PathNodes.cpp`) into port objects:

- No exe body exists for any connector function. 0x0040d440 and 0x0040d020 have bodies only in the asi-only TU
  `Track/CourseLoadDispatch.cpp` (raw forwarders to 0x0040d270 / 0x00426e10). 0x0040d270 is unmatched (441 B,
  S02 raw g2s 45.9%, "not attempted"). 0x00426e10 is matched in MATCH-S06 (`re/match/s06/00426e10.c`, branch
  `orch/MATCH-S06`, in review) but S06 kept it **asi-only**: its callees have port bodies only in asi-only TUs,
  and the exe link failed with it in (`verify/matchs06_20261101/RESULT_MATCH-S06.md`, "Swap (port leg)").
- The tables the chain reads are original initialised `.data` that the exe never fills: `0x005f2770` (course
  handler table, no exe writer, T13 row 0x0040ce80 / U-9783) and the hook table `0x005f33f8` with count
  `0x005f37a0` (function pointers into MASHED.exe; the exe models the slots in port tables in
  `Render/TrackHookSlots.cpp`). `DAT_0063d7e4` is 0 in the exe (`Race/SubStateMachine.cpp:28-30`).
- In the exe the original data range is zero-filled memory (T13 RESULT), so an address no exe TU writes reads 0.

A swap here would mean replacing the exe's course loader with the original chain, starting at an unmatched
441 B function. That is not a swap under L-72 (no exe call site exists for any root), and wiring any one leaf
in by hand would change state the exe reads as 0 today (behaviour change: default-OFF knob + PREREG + decision).

## Per-RVA table

`match.py` = `re/tools/match/match.py <src> --toolchain ddk --bytes` (cl 13.00.9176 / c2 13.00.9178, flags
`/O2 /GS /Ob1`), re-run on this branch (log `D:/MashedWT/MATCH-TOPDOWN-T05_data/match_rerun.log`). `exe body` =
`scripts/rva_body_scan.py` on this branch. `exe callers` = E8/E9 rel32 sites to the symbol in this branch's
`mashed_re.exe`, scanned against its `/MAP` (baseline build of orch/integration `3a3186c4a`, no source change).

| rva | outcome | matched source | match.py | relocheck | exe body | exe callers | reason |
|---|---|---|---|---|---|---|---|
| 0x004053d0 | left | `re/match/s01/004053d0.c` | 100.0% 12/12, 45 B identical | OK checked=0 unchecked=5 | none (asi `Render/LowRvaSetters_o2.cpp` TrackDataSlotSet) | 0 | writes the clip-module slot `0x00639d70/74/78`; the exe has no camera-path clip module and relies on those reading 0 (`TrackRenderer.cpp:5294`, `Race/CameraEntries.cpp:29-30`). Its call site is inside the 0x00426e10 dispatch. A call would be a behaviour change |
| 0x004095a0 | left | `re/match/s01/004095a0.c` | 98.6% insn 74/73, 214 B identical | OK checked=4 unchecked=6 | none | 0 | builds `"toastart/common/led/LE%d.LED"` into `0x0063b7f0` (no exe reference to `0x0063b7f0`); only caller 0x00409710 (below) |
| 0x00409680 | left | `re/match/s01/raw/00409680.c` | 100.0% 7/7, 20 B identical | OK checked=0 unchecked=1 | none (asi `Render/LowRvaSetters_o2.cpp` LedArrayInit) | 0 | fills 0x480 dwords at `0x0063a5f0` with 0xbf800000; the exe's camera override table is port-owned (`Race/RaceCamera.cpp:93` reads the on-disk image), no exe TU reads `0x0063a5f0` |
| 0x00409710 | left | `re/match/s01/raw/00409710.c` | 100.0% 42/42, 119 B identical | OK checked=8 unchecked=2 | none | 0 | loads `LE%d.LED` from `led.piz` into `0x0063a5f0` through the original piz/RW stream calls (0x00495280 / 0x004952f0 platform file I/O, 0x004cc230 / 0x004cc5e0 / 0x004cbd30 / 0x004cc160); the exe loads that table itself into port storage (`RaceCamera.cpp`) |
| 0x00412050 | left | `re/match/s02/00412050.c` | 100.0% 49/49, 165 B identical | OK checked=11 unchecked=0 | none | 0 | creates 12 x 2 objects through 0x0047fad0 / 0x0047cdc0 (no exe body, deep gaps) from `DAT_0063bc40`, writes `0x0063bb5c[6]`; no exe reference to either address |
| 0x0041e870 | left | `re/match/s04/0041e870.c` | 100.0% 14/14, 49 B identical | OK checked=4 unchecked=0 | none (asi `Match/MatchS04_0041e870.cpp`) | 0 | scans the hook table `0x005f33f8` (count `0x005f37a0`) and stores the course's record in `0x0063d7e4`; both table and count are zero in the exe (port model `Render/TrackHookSlots.cpp`), so the scan finds nothing |
| 0x0041e8b0 | left | `re/match/s04/0041e8b0.c` | 100.0% 2/2, 9 B identical | OK checked=1 unchecked=0 | none (asi `Match/MatchS04_0041e8b0.cpp`) | 0 | `jmp [ [0x0063d7e4] + 0x14 ]` (slot 0 thunk, `Render/ArcticTrackNodeSlot0.cpp:15-16`); `0x0063d7e4` = 0 in the exe, so the jump reads address 0x14 |
| 0x0041e970 | left | `re/match/s04/0041e970.c` | 100.0% 2/2, 9 B identical | OK checked=1 unchecked=0 | none (asi `Match/MatchS04_0041e970.cpp`) | 0 | slot thunk through `[0x0063d7e4]`, same as 0x0041e8b0 |
| 0x0041e9d0 | left | `re/match/s04/0041e9d0.c` | 100.0% 3/3, 9 B identical | OK checked=1 unchecked=0 | none (asi `Match/MatchS04_0041e9d0.cpp`) | 0 | slot thunk through `[0x0063d7e4]`, same as 0x0041e8b0 |
| 0x0041ea90 | left | `re/match/s04/0041ea90.c` | 100.0% 3/3, 9 B identical | OK checked=1 unchecked=0 | none (asi `Match/MatchS04_0041ea90.cpp`) | 0 | slot thunk through `[0x0063d7e4]`, same as 0x0041e8b0 |
| 0x004260e0 | left | `re/match/pilot/a7_004260e0.c` | 100.0% 211/211, 525 B identical | OK checked=10 unchecked=15 | none | 0 | builds a `ToastArt/tracks/...` path in `0x008991e0` (no exe reference); the `dos` arm copies the original `.rdata` string `0x005cd4ac`, zero in the exe. The exe builds track paths in port code |
| 0x004715a0 | left | `re/match/pilot/a3_004715a0.c` | 93.4% insn 136/127, 477 B identical | OK checked=2 unchecked=9 | none | 0 | rebuilds 8 scenario records at `0x0086ece0` from `0x0086cbc4` through 0x0047ce40 (no exe body); the exe keeps scenarios in port vectors (`TrackRenderer.cpp:2575-2596`, `scen_seqs` / `scen_objs`), no exe reference to either table |

Connector, for the record (not T05 swap candidates):

| rva | status | note |
|---|---|---|
| 0x0040d440 | matched (`re/match/s02/0040d440.c`, 100% 9/9, 34 B, relocheck OK 5/0) | T10 row; asi-only body `Course_LoadCurrent_40d440` |
| 0x0040d270 | unmatched gap (441 B, S02 not attempted) | blocking gap; hides 31 matched functions (map) |
| 0x0040d020 | matched (`re/match/s02/0040d020.c`, 100% 8/8, 21 B, relocheck OK 1/0) | asi-only body `Track_LoadByIndex_40d020`; no exe body, no exe caller |
| 0x00426e10 | matched in S06 (in review), asi-only new body | blocking gap per the map; S06 could not link it into the exe |

## What T05 needs before any swap

1. 0x0040d270 matched (S02 residue pass, the map's first suggested row).
2. An exe course-load path that keeps `0x005f2770`, `0x005f33f8` / `0x005f37a0`, `0x0063d7e4` live, or the
   matched chain's tables rebound to the port's track objects. Either is an architecture-level port change
   (CLAUDE.md "Stop and ask"), not an L-72 swap. Until then the 12 stay matched with no exe body.

## Gates

Not run: no build input changed. The baseline build of this branch (= orch/integration `3a3186c4a`) was used
only for the `/MAP` caller counts: `=== Build OK ===` both targets, `[rva-lint] 25 finding(s) ... NEW=0`,
`[asm-lint] OK`.

## Re-classify

None. No level changes for the 12 rows.

## Raw data (D:/MashedWT/MATCH-TOPDOWN-T05_data, not committed)

```
81e39d800314d4ad10c36b0c9421ac850093169fbf1fc3550cda7a59ebcd8609  mashed_re.exe (baseline build, orch/integration 3a3186c4a)
a7780b0705f0b0b17c44254522bdb2c5ad1ce939ddc61f538941e0c1e75bd1e7  mashed_re.map
b2704ca8279855ae65ca654f4e8d260aaa2700ae5612a086387698d5ffbbe4ac  build0.log
2ac3848f7d1f2e88a72fa8df1e193550654a2588dc7a7e598e3c200e2b65cfd8  match_rerun.log (T05, T10 and T07 sources)
```
