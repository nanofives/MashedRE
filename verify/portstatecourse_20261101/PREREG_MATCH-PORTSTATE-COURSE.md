# PREREG MATCH-PORTSTATE-COURSE (2026-11-01)

L-78 (c), port-state migration 1/3 (Mariano 2026-11-01): `mashed_re.exe` keeps the original course table alive
behind a default-OFF knob. Committed alone, before any arm runs.

## What the original holds (static, read from `original/MASHED.exe.unpatched`)

All four ranges are file-backed `.data` (`.data` VA 0x005ea000, RawSize 0x4d000), and the exe's image pad
(`exe_main.cpp` B17) holds them zero-filled.

| range | bytes | content | readers in the original (imm32 scan of `.text`) |
|---|---|---|---|
| 0x005f2570..0x005f2673 | 0x104 | 13 records of 0x14 bytes = 5 dwords each. Dword 0 equals a `Course_Id` of the hook table below (0x1e, 0x03, 0x27, 0x00, 0x26, 0x0b, 0x19, 0x02, 0x24, 0x22, 0x25, 0x21, 0x1a) | none direct. Reached only through the pointer array |
| 0x005f2728..0x005f275b | 0x34 | 13 pointers, entry i -> one record above (0x005f2570, 0x005f25fc, 0x005f2660, 0x005f2584, 0x005f264c, 0x005f25c0, 0x005f25ac, 0x005f2598, 0x005f2624, 0x005f25e8, 0x005f2638, 0x005f25d4, 0x005f2610) | through `[0x005f2770]` (40 `.text` sites, e.g. 0x0040ce86, 0x0040d431, 0x0040d447) |
| 0x005f33f8..0x005f379f | 0x3a8 | hook table, 13 records of 0x48 = `{char name[0x10]; u32 Course_Id; u32 slot[13]}`; the slot dwords are `.text` addresses (0x00448940 ..) | 0x0041e889, 0x0041e992 |
| 0x005f37a0 | 4 | 13 (record count) | 0x0041e872, 0x0041e982 |

`0x005f2770` (= 0x005f2728) is already seeded by default in the exe (`TrackRenderer.cpp` `Ai_BridgeLoad`, H2,
opt-out `MASHED_NO_SLOTSTATE_SEED`). `0x005f275c..0x005f276b` (the slot-state cells at `+0x34`) are runtime state
and are outside every range here. `0x005f2674..0x005f2727` (nine more 0x14 records) has no pointer or imm32
reference in `.text` / `.data` / `.rdata` and is not seeded.

`0x0063d7e4` is `.bss`. The original writes it only in the selector 0x0041e870 (matched,
`re/match/s04/0041e870.c`, call site 0x00426eaf in 0x00426e10): it stores the last hook record whose `+0x10`
equals the argument, or 0.

## The change (one knob)

`MASHED_PORTSTATE_COURSE` (default OFF; exe only). At the exe's course load (`TrackRenderer.cpp`, next to
`Ai_BridgeLoad`, with the `Course_Id` parsed from `COURSE.LUA`):

1. Seed the four ranges above with the original's bytes (a source table transcribed from
   `MASHED.exe.unpatched`, each range cited by address).
2. Call the matched selector 0x0041e870 (`Match/MatchS04_0041e870.cpp`, added to the exe target) with that
   `Course_Id`, so `0x0063d7e4` holds the original's record.
3. Log one `[portstate-course]` line: an FNV-1a 32 of each seeded range, the selected record and its name, and
   the values the blocked T04 / T05 / T13 rows would read (below).

The slot dwords stay the original's `.text` addresses. In the exe they are callable only where B16 placed a thunk,
so the knob calls through none of them.

Knob OFF the exe executes no new code (the hook point tests the knob first).

## Predictions

- **P1 (default unchanged).** Knob OFF: G-CTRL `8284be46 / ba407a16 / 31fe043e`, B `1d5bf4f9` x3
  (`re/orchestrator/merge_train.json` `gctrl.ref` / `b.ref`; arms `H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`).
- **P2 (bytes).** Knob ON: each logged FNV-1a equals the FNV-1a of the same range read out of
  `original/MASHED.exe.unpatched` (script `verify/portstatecourse_20261101/fnv_ranges.py`).
- **P3 (selector).** Knob ON on the gate scenario (Training, `Course_Id` 30): `0x0063d7e4` = 0x005f33f8
  (record `"training"`, the only record with `+0x10` = 30).
- **P4 (gates with the knob).** Knob ON: the same four hashes as P1 (arms `H_PSC B_PSC_K0 B_PSC_K1 B_PSC_K26`,
  `STACK['PSC'] = ['MASHED_PORTSTATE_COURSE']`). Reason: no exe TU reads the four ranges or `0x0063d7e4` on the
  gate path. Every exe reader of `[0x005f2770]` adds `+0x34` (slot states: `CarSlotState.cpp`,
  `AiTargetClosest_naked.inc` 0x0040e383, `TrackRenderer.cpp:147`, `AiStandalone.cpp:1151`); the exe readers of
  `0x0063d7e4` are asi-only TUs (`TrackNodeLeaves_o1.cpp`, `PromoLoop_round41.cpp`, `FrameWorldPasses.cpp`) or
  the diagnostic `MASHED_L1B_INPUTS` CSV (`SubStateCallees.cpp:494`), which the gate arms do not arm.

**If P4 fails** (any hash moves knob ON): no flip, the knob stays default OFF, and the move is filed as its own
decided-flip decision with the moved hashes. **If P2 or P3 fails**, the seed or the selector call is wrong:
fix it before any re-check.

## Re-check of the blocked rows (after the arms)

For each T04 / T05 / T13 row whose stated blocker names `0x005f2770`, `0x005f33f8` / `0x005f37a0` or
`0x0063d7e4`, record with the knob ON: is that read now the original's value, which other blocker remains,
and whether a swap is now sound (L-72 soundness rule of T13: every global the matched source reads is live in
the exe, and an exe caller exists). Rows and what the knob-ON log line samples for them:

| row | rva | read that the knob makes live | logged value |
|---|---|---|---|
| T13 | 0x0040ce80 | `*(*(int**)0x005f2770 + v) + 4`, v = `[0x0067ea94]` when `[0x0067e9fc]` == 10 else `[0x0067f17c]` (U-9783, 0x0046b56c..0x0046b585) | e9fc, v, and the dword when 0 <= v < 13 |
| T05 | 0x0041e870 | the scan itself | `0x0063d7e4` |
| T05 | 0x0041e8b0 / 0x0041e970 / 0x0041e9d0 / 0x0041ea90 | `[0x0063d7e4] + 0x14 / 0x44 / 0x14 / 0x44` | the two slot dwords |
| T04 | 0x00410b30 | `*PTR_PTR_005f2770[DAT_0063ba78]` (argument of 0x004270f0) | `[0x0063ba78]`, and the pointer and its dword 0 when 0 <= it < 13 |

No re-classify is prepared by this step unless a row becomes a sound swap with an exe caller in the default
build.
