# RESULT RO-STACKROW-READERS: does a stale-stack value in ring 0x007ec9e4 reach race state

Date 2026-11-11. Worker session cmwbx2zbt2ivgsc1bk1mvi2zq (claude2), parent cmwbssxl22bg4sc1ba3ghf7dh.
Branch `orch/RO-WARP-NORMALIZE` (shared with RO-WARP-NORMALIZE and RO-AUDIO-ISOLATION). Review: **TEXT-ONLY**.

## Verdict

**No. The three readers `0x0040bebe`, `0x0040c2d8` and `0x00422e60` read ring fields `+0x24..+0x2c` only in rows
whose producer wrote them. Nothing needs pinning. The stale fields need only the compare rules below, on both sides.**

1. **Static.** The readers run only for row types 8 (`0x0040bebe`) and 4 (`0x0040c2d8`, `0x00422e60`).
   - The type-4 producer `FUN_00469df0` writes dwords 0..14 of its row. It zeroes the locals involved and then
     computes them.
   - The type-8 producer `FUN_0047def0` writes dwords 0, 1 and 9..14.
   - The fields that `FUN_00469aa0` leaves stale in type-3 rows (`+0x28..+0x3c`) have no type-3 reader.
2. **Dynamic, poison.** A native listener at the entry of every ring consumer overwrote every unwritten field with
   `0x7fc0dead` from the race anchor on (run `sp_a`, 5,831 listener calls). The fields: type 3 dwords 10..15, type 4
   dword 15, type 8 dwords 2..8 and 15. Against the clean run `n3_b`:
   - race state 15/15
   - after the per-type rule below, **normalised 15/15**: no dword outside the poisoned fields changed
3. **Positive control.** The same listener also overwrote type-4 dword 9 (`+0x24`) with 32768.0 (run `sp_pos`). That
   field is written by the producer and read at `0x00422e6a`.
   - The per-tick compare first diverges at tick 385, across 12 or more blocks (`0x00613000`, `0x0068f400`, ...).
   - Race state is 4/15: car records differ by 977 dwords at checkpoint 4, and pickup slots and both RNGs differ too.

   So these readers do reach race state, through the producer's values. The poison test would have caught a
   stale-field read.
4. **A second stale-stack sink, found by the poison run.** It is the particle ring at `0x006fe790`. `FUN_00486370`
   copies a 16-dword RwMatrix with `rep movsd` (`0x004863e4`) into record `+0x30`. The matrix pads m[7], m[11] and
   m[15] (record `+0x4c`, `+0x5c`, `+0x6c`) hold what the caller's stack matrix left there. In the clean run `n3_w2`
   the write watch saw the code address `0x004671a5` stored into `0x006fe7ec`. These pads did not reach race state
   (`sp_a`). On the exe they differ by construction, so they get the rule RWPAD.

## Data flow, by RVA

The ring is 256 rows × `0x40` at `0x007ec9e4`:
- type `DAT_007e9de4[row]`
- time `DAT_007ea1e4[row]` (`DAT_007f1030` when produced)
- car `DAT_007ea5e4[row] - 0xff00`
- 8-dword header `DAT_007ea9e4[row*8]`
- cursor `DAT_007e9de0`

Every payload store is a 16-dword copy loop `mov [reg*4 + 0x007ec9e4], reg` from a stack array. There are 19 store
sites, each next to its type store (`ring_refs` in this RESULT's scan: every `.text` memory operand with displacement
in `0x007ec9e4..0x007eca24`, any base). No instruction has a displacement in `0x007eca24..0x007f09e4`, and no
immediate operand falls in the ring range (capstone linear sweep).

**Producers (what each row type gets):**

| type | producer (type store / copy) | payload dwords written |
|---|---|---|
| 3 | `FUN_00469aa0` (`0x00469cc8` / `0x00469da3`) | 0..9: `local_40[0..3]`, `local_30`, `local_2c`, `local_28`, `local_24`, `local_20`, `local_1c`. 10..15 are stack beyond `local_1c` (LOADHOLD: `+0x2c` held return address `0x00470948`) |
| 4 | `FUN_00469df0` (`0x0046aed4` / `0x0046afb6`, `0x0046b089` / `0x0046b166`) | 0..14: `local_80[0..14]`, from `local_124..11c`, `local_154..14c`, `local_160..158`, `local_100..f8`, `local_10c..104`. The last six are set to 0.0, then computed (decompilation lines `local_f8 = 0.0` ... `local_10c = 0.0`). 15 is stack |
| 8 | `FUN_0047def0` (`0x0047e5f1` / `0x0047e653`, `0x0047e7b2` / `0x0047e813`) | 0, 1, 9..14: `afStack_6c[0]` (1.0 or `DAT_0086cba4[...]`), `[1]` (`DAT_00881f84[...]`), `[9..11] = pfVar3[3..5]`, `[12..14] = pfVar3[0..2]` |
| 10 | `FUN_0047def0` (`0x0047e26e` / `0x0047e2d3`, `0x0047e3c7` / `0x0047e433`) | `afStack_6c[0 .. 3n-1]` (copy loop guarded by `0 < n`), `[12..14]`, with n = `pfVar3[0x2b]` stored as header dword 2 |
| 6, 9 | 10 type-6 sites (`0x0046919a` / `0x00469283` ... `0x0047dc6c` / `0x0047dd44`), 2 type-9 sites (`0x0047e009`, `0x0047e0fc`) | 16-dword copies, written subset not traced [UNCERTAIN] |
| 1, 5, 7, 0xb, 0x2ff01, 0x3ff01..0x3ff04 | `0x004712b3`, `0x00425829`, `0x004675ed`, `0x0044bb5a` / `0x0044c413`, `0x0040b39f`, `0x004255a1..0x004259ca` | none: the type, time, car and header are stored and the payload keeps the previous occupant's dwords |

**Readers (every displacement read of the ring):**

| reader | function, case | ring dwords read | where the values go |
|---|---|---|---|
| **`0x0040bebe..0x0040bee8`** | `FUN_0040be50` case 8 (after `call FUN_0047d150` at `0x0040beb9`) | 9..14 (`+0x24..+0x38`), plus 1 (`0x0040beae`) | `FUN_004922e0(car,1,2,0xff)` if header dword 2 == 0 and dword 1 > `[0x005ccd10]`, else `FUN_0048aa50(&row[9], 10, ..., &row[12], ...)` (particle) if header dword 2 == 3 |
| **`0x0040c2d8..0x0040c306`** | `FUN_0040be50` case 4 (after `call FUN_0048aa50` at `0x0040c2d3`) | 9..14, plus 0..2 (`0x0040c28b..0x0040c297`) | `FUN_004c3ac0` lengths -> `_DAT_007f0f94`, `_DAT_007f0f98`; `FUN_00456140`, `FUN_00487020` |
| **`0x00422e60..0x00422e70`** | `FUN_00422ba0` type 4 | 9..11 (`+0x24..+0x2c`) | `FUN_004c3ac0` length -> `DAT_008995dc[a*0x4e+b]` and `[b*0x4e+a]` (car-pair table), then `FUN_00417370(car)` when `FUN_0040e470(car) != 1` |
| `0x00422c58` (lea +0x00, 12-dword copy), `0x00422c5e` (lea +0x30) | `FUN_00422ba0` type 10 | 0..11, 12..14 | `FUN_0044e0a0(local_58)`: `FUN_004b4550(out, row, n)` reads dwords 0 .. 3n-1 and always 0..2 [UNCERTAIN: whether n = 0 occurs. 0 type-10 rows in this scenario]; dword 14 against `[0x005cc32c]` |
| `0x00422f49..0x00422f55` | `FUN_00422ba0` type 3 | 4..6 | `FUN_004c3ac0` -> `FUN_004922e0`, `FUN_00480720`, `FUN_00480d60` |
| `0x00422cd6` | `FUN_00422ba0` type 8 | 1 | `FUN_0047cea0` |
| `0x0040bf96..0x0040bfe8`, `0x0040c122`, `0x0040c14a` | `FUN_0040be50` case 3 | 0..6 | `FUN_00472650` + row -> `FUN_0048aa50`, `FUN_00456140`, `FUN_00475210`, `FUN_00486610` |
| `0x0040c53e..0x0040c54a` | `FUN_0040be50` case 6 (header dword 0 == `-0xff80`) | 0..2 | `FUN_0048b230`, `FUN_0041eda0`, `FUN_0045ba00` |
| `0x0046495c` | `FUN_004648b0` case 8 | 1 | `FUN_00464670` (hooks.csv: collision SFX) |
| `0x004649c8` | `FUN_004648b0` case `0xb` (`fld [row+0]`, `call FUN_004a2c48` at `0x004649ce`. The decompilation hides the x87 argument) | 0, which a type-`0xb` row never writes | `== 0 / == 1` -> `FUN_00464670(car,3,0,0)` (audio). Previous occupant's payload, not stack. 0 type-`0xb` rows here |

No reader reads dword 15 (`+0x3c`). Dwords 10..15 of type-3 rows have no reader at all. The only readers of dwords
9..14 serve types 4, 8 and 10, whose producers write those dwords. **So no stale-stack dword reaches a reader.**

The values the readers take from written fields do reach race state: `DAT_008995dc` and `FUN_00417370` from
`0x00422e60`, and the positive control's car records. Those values are computed by the producer, so they are part of
what the oracle compares.

## Run evidence

Ring census over the 15 checkpoints (`ring_census.py`, distinct row/type/time entries): 434 rows in each of `n3_a`,
`n3_b` and `sp_a`.
- types 3: 137, 4: 71, 5: 9, 6: 4, 7: 116, 8: 40, 9: 13, `0x2ff01`: 8, `0x3ff01`: 2, `0x3ff02`: 7, `0x3ff03`: 27
- no type 10 or `0xb`, so those two readers were not exercised

In `sp_a`, poison dwords sit in type-3 rows (dwords 10..15, 696 each), type-4 dword 15 (437) and type-8 dwords 2..8
and 15 (229). They also sit in rows of types 5, 7, `0x2ff01`, `0x3ff02` and `0x3ff03`, which inherit the payload of
the slot's previous occupant.

| pair | per tick (ignore os/audio masks + ring) | checkpoints |
|---|---|---|
| `n3_b` / `sp_a` | first non-ring NEW block `0x006fe400` at tick 932 (`0x006fe800` 934, `0x006fec00` 948), nothing else | race state 15/15; normalised 15/15 with RINGSTALE + RWPAD (`reports/sp_a_rescored.txt`) |
| `n3_c` / `sp_a` | same, tick 932 | |
| `n3_b` / `sp_pos` (positive control) | NEW at tick 385 in `0x00613000`, `0x0068f400`, `0x00690400`, `0x00715c00`, ... (490 blocks) | race state 4/15 |

The particle-pad dwords changed in `sp_a` because the listener itself runs on the game thread's stack below each
hooked consumer, which changes what later stack matrices pick up. Write watches (`reports/watch/sp_w.json`,
`n3_w2.json`):
- `0x006fe7ec` is stored by the `rep movsd` at `0x004863e4`: value `0x004671a5` in `n3_w2`
- `0x006fe7dc` is stored by the same `rep movsd`: value `0x100` in `sp_w`

## What pins it on both sides

Nothing has to be pinned: no stale value is read. The exe-against-original compare has to drop the stale dwords,
which differ by construction (different stack). `re/frida/detorig_report.py` now has:
- **STACKROW** (existing): ring dwords 10..15 of every row.
- **RINGSTALE** (new): per row type, the dwords its producer does not write (`RING_WRITTEN`), with the n-dependent
  set for type 10 and the whole payload for the no-payload types. Types 6 and 9 stay compared [UNCERTAIN, their
  written subsets were not traced]. If the exe shows a first divergence there, read the producers named above.
- **RWPAD** (new): particle ring `0x006fe790 + 0x7c·k` (k < 150), dwords `+0x4c`, `+0x5c`, `+0x6c`.

The exe port must not read the stale dwords either. A port of `FUN_0040be50` / `FUN_00422ba0` / `FUN_004648b0` that
keeps the same type gating inherits this. [UNCERTAIN] In `FUN_0047def0` the 8-dword header `DAT_007ea9e4[row*8]` is
copied from the stack too (loop from `&uStack_94`, 8 dwords). `FUN_00469aa0` stores its 8 header dwords one by one
from computed locals. The stale dwords of `FUN_0047def0`'s header were not traced, and they were equal original
against original in every pair here.

## Harness

- `re/frida/js/detorig.js`, `cfg.poison`: a TinyCC CModule `onEnter` on `0x00422ba0`, `0x0040be50` and `0x004648b0`,
  active from the race anchor.
- `re/frida/detorig.py`: `--poison stale|pos`, `--poison-value`.
- Runs `sp_a`, `sp_pos` (`batch4.sh`) and `sp_w`, `n3_w2` (`batch5.sh`), same configuration as batch 3 of
  RESULT_RO-WARP-NORMALIZE.md, all 15/15 checkpoints. Raw SHA-256: `raw_sha256_ro_orig_side.txt`.

Gates: not run (no build input changed).
