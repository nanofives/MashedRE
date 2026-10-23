# gamesave.bin "profile block" = the 13-slot ghost buffer (U-3560 input, 2026-10-23)

Read from `Mashed_pool10` (read-only) on 2026-10-23. Every claim cites the address it was read at. Not yet filed in UNCERTAINTIES.md
(tracker writes go through `re-classify`).

## Shipped save (explicit count, `original/gamesave.bin` sha256 `bd18788182b2343e`)
348 non-zero bytes = 4 (magic at `0x0`) + 344 (span `0x24A40..0x24F5F`). File `0x4..0x24A3F`: **0** non-zero bytes. Tail: 0.

## Size table `DAT_005f29d0` (13 pairs of {dword id, float}, terminated by id -1), raw bytes from the image

| id (hex) | float |   | id (hex) | float |
|---|---|---|---|---|
| 0x1e | 20.0 | | 0x19 | 50.0 |
| 0x03 | 30.0 | | 0x00 | 40.0 |
| 0x27 | 50.0 | | 0x24 | 40.0 |
| 0x02 | 40.0 | | 0x22 | 50.0 |
| 0x26 | 80.0 | | 0x25 | 80.0 |
| 0x0b | 50.0 | | 0x21 | 80.0 |
| | | | 0x1a | 60.0 |

`FUN_004113b0` sums `FUN_00482900(pair[1], 10)` over the table and requires the total == `0x24a3c` (else prints "GHOST Size ERROR" and spins).
`FUN_00482900` = `(int)(p1*60/p2)*0x24 + 0x19c` (`Save/ReplayGetSize.cpp`). Check: sum of floats = 670.0, 670*6 = 4020 frames x 0x24 =
144,720, + 13 x 0x19c = 5,356, total 150,076 = `0x24a3c`. So the block is exactly 13 records, one per table entry: a 0x19c-byte header plus
`float*6` frame nodes of 0x24 bytes each. The id's meaning (compared against `FUN_00426c00()`) is `[UNCERTAIN]` (a track/course id is not shown here).

## Allocation (`FUN_004113b0`)
`DAT_008a94a8 = (*(DAT_007d3ff8+0x108))(0x24a3c, 0)`, then zero-filled (rep stosd + tail stosb). It is never formatted into slots: the block stays
all zero until a record is copied in.

## Who writes a record: `FUN_004117b0` (ReplaySave), gated
- Runs only if `DAT_005f29c8 == 0` (`0x004117c0`), `DAT_0063bb10 != 0` (`0x004117cd`) and `FUN_00430820()` (= `DAT_0067ed6c`) `== 0`. Then
  `FUN_00483ca0(DAT_0063bb10, &DAT_008a94a8)` and `FUN_004099a0()` (prints "starting autosave", sets `DAT_008a9584 = 4`, `DAT_008a9594 = 1`,
  `DAT_008a95b0 = 0`, only when `DAT_008a95ac != 0`). Sets `DAT_005f29c8 = 1` (`0x0041184c`).
- Called only through `0x0040de00` (one caller, `FUN_004111c0`).
- `FUN_00483ca0(src, &block)`: walks the block from its start until a record with `[0] == 0x10b && [2] == src[2]`, or `[0] == 0 && [2] == 0`
  (stride `[1]` bytes), copies `0x67` dwords (0x19c bytes) of `src` there, then `src[0x5b] * 9` dwords from the pointer `src[5]`. So the header
  is copied verbatim, including `src[5]` (a heap pointer) `[UNCERTAIN: whether the reader rebases or ignores it]`.

## What arms it: `FUN_00411870(param_1)`
- Returns unless `FUN_0042f6a0()` (= `DAT_0067e9fc`) `== 2` and `DAT_0063bb20 == 0`.
- Looks up the table float for `FUN_00426c00()` (`fStack_10`), compares the recording `DAT_0063bb14` time at `+0x174` against the best
  `DAT_0063bb10 +0x174` ("Replay Time comparing %f to %f"). If the recording is not slower than the best (or there is no best), the recording's
  time x `_DAT_005cc948` is below the table float, and `param_1 != 0`: `DAT_0063bb10 = DAT_0063bb14`, `DAT_005f29c8 = 0` (arms ReplaySave),
  buffers swap via `DAT_0063bb18` / `DAT_0063bb04`.
- Callers: `FUN_0040e560` (calls it with 0, so never arms) and `FUN_00429310` `[not read yet]`.

## Consequence for a round trip
A record exists only after a run in mode `DAT_0067e9fc == 2`, under the table float for that id, through `FUN_00429310`'s call with
`param_1 != 0`. Nothing in the frontend or a warped race does that. Nothing observed so far ever wrote a non-zero byte there.

## Natural arming (read 2026-10-23, `Mashed_pool10`, read-only)

- `FUN_00429310` is the per-frame TimeTrial tick (one caller, `FUN_0040fc00`, which the race state machine `FUN_004111c0` calls in states 3..7,
  10 and 0xb). It returns unless `FUN_0042f6a0()` (= `DAT_0067e9fc`) `== 2`.
- Sector counter `DAT_008991bc` (writers `0x004293eb`, `0x00429852`, `0x004298d6`, `0x0042990e`): with `p = FUN_00408ad0()` (the race-pct slot
  load), it steps +1 while `10*k < p < 10*k + 10`; at `k == 10` it steps to 11 only when `p < _DAT_005cc55c` (= 10.0f, bytes `00 00 20 41`).
- At `DAT_008991bc == 0xb` and `FUN_0040e350()` (= `DAT_0063ba8c`) `== 6`: prints "time trial time is %f" / "gametimer is %f", writes the two
  result rows (`FUN_004115c0` / `FUN_00411580` -> `FUN_00430b00`), then **`FUN_00411870(1)`** (the only arming call; `FUN_0040e560` passes 0),
  then `FUN_0040e360(7)`.
- `FUN_00411870(1)` arms (`DAT_005f29c8 = 0`) only when the recording time `rec+0x174` x `_DAT_005cc948` is below the size-table float for
  `FUN_00426c00()` and not slower than the held best. `_DAT_005cc948` bytes `3e c3 ae 39` = 3.3333e-4 (1/3000 to float precision). The G1
  capture's `rec+0x174` = 114050 -> 38.0 s against the 20.0 entry for id 0x1e, so that run would NOT have armed.
- `ReplaySave` runs from `FUN_004111c0` state 10 (`thunk_FUN_004117b0`), reached through state 0xb -> 9 (`FUN_0040e460`) -> `FUN_0040ddb0` -> 10.
  It copies the record, then `FUN_004099a0` starts an autosave only when `DAT_008a95ac != 0`. `DAT_008a95ac` is the save-state counter
  that `SerializeToBuffer` writes at file `0x24EFC`; the shipped save carries 0 there.

Consequence: a natural ghost needs a TimeTrial run that completes the course (sector counter to 11) while the race state is 6, in less time
than the table float for that id (20 s for 0x1e; 30-80 s for the others), and faster than any held best. Even then it reaches the disk by
autosave only if the save-state counter is non-zero, which the shipped save is not; otherwise it lives in memory until a manual save.
No unattended harness here drives a lap; the arming path is therefore read, not run.
