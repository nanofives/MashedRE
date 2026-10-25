# RESULT D3-CF-BOX: box-state producers of DAT_0068d1f0, slice 1

**Verdict: DECISION.** Slice 1 is ported behind `MASHED_BOX_PRODUCERS` (default OFF). It covers FUN_004111c0's race-start write,
FUN_0040e590's grid write and FUN_00422fd0's kill write. With the knob unset, all six default gate hashes are unchanged on both
builds. With it set, the port's box-state write sequence equals the original's row for row on the u9779 recipe: 15 of 15
rows, against O1 and O2, which equal each other. The knob stays OFF. Flipping it is the lead's decision row: it moves G-CTRL
(section 5).

PREREG `PREREG_D3-CF-BOX.md` (`85720dc3b`) and amendment 1 (`76e7556c7`) were each committed alone, before the arms they
govern. Branch `orch/D3-CF-BOX` from `282ea2ec7`. Pool clone `Mashed_pool10` (read-only, released).

## 1. The original's producers (listing, anchor binary)

`FUN_0045ba00` (`0x0045ba00..0x0045ba0f`): `DAT_0068d1f0[idx] = val`. There are 7 call sites, and each pushed value was read off
the listing:

| call | return | caller | value | slice 1 |
|---|---|---|---|---|
| `0x0040ff6b` | `0x0040ff70` | FUN_004111c0 race start, slot dword `*(*(int*)0x005f2770+0x34+i*4) == 0` (`0x0040ff56`) | 4 (`PUSH 0x4` `0x0040ff68`) | ported |
| `0x0040ff9c` | `0x0040ffa1` | same loop, dword non-zero | 1 (`0x0040ff99`) | ported |
| `0x0040eeae` | `0x0040eeb3` | FUN_0040e590, every slot whose dword is non-zero | 1 (`0x0040eeab`) | ported |
| `0x00423005` | `0x0042300a` | FUN_00422fd0 kill | 2 (`0x00423002`) | ported |
| `0x0042512f` | `0x00425134` | FUN_00424eb0, `DAT_007f0fd0` 9 / 7, car state 0xff | 1 (`0x0042512c`) | **not ported** |
| `0x004251c8` | `0x004251cd` | FUN_00424eb0, `DAT_007f0fd0` 4, car state 0xff | 1 (`0x004251c5`) | **not ported** |
| `0x0040c578` | `0x0040c57d` | FUN_0040be50 case 6, `DAT_007ea9e4[i*8] == -0xff80`, slot `DAT_007ea5e4[i] - 0xff00` | 2 (`0x0040c575`) | **not ported** |

The elimination block of FUN_00410d10 calls FUN_00422fd0 at `0x00411025`. The dispatcher reads the box state at `0x0045bc6b`
and writes 3 at `0x0045bc85` (already ported, `PowerupSystem::Tick`). Its pass needs FUN_0040e350() == 6 (`0x0045bc2b`).

## 2. The change

| commit | what |
|---|---|
| `033891cd6` | `TrackRenderer.cpp`: `MASHED_BOX_PRODUCERS` and `MASHED_BOX_TRACE`. With the knob set: StartRound writes start (race init only) and grid, the two UpdateRace elimination sites and `KillCar` write 2, the per-frame write keeps only the 4 for slots with no car, and the dispatcher gets `aib_game_sub_mode()` in place of the constant 6. `scenario_launch.py --box-probe`: entry hook `0x0045ba00`, instruction hook `0x0045bc85`, rows into `.elim.csv`. `run_arms.py` copy with the BOX / BOXR stacks |
| `c4ffa0975` | amendment 1: at race init the grid block is written twice (measured in O1), plus the ER control stack and `boxseq.py` |

With the knob unset, the code path is the old one: the per-frame 1 / 4 write and dispatcher state 6.

## 3. Original captures (private root `D:\MashedWT\D3-CF-BOX_root`, `mashed_lock.py`)

`orig/` holds the `.elim.csv` and provenance for each capture. `boxseq_out.txt` holds the reducer output.

- **O1, O2**: the u9779 `o_ep_1` argv plus `--box-probe --hold 40`. Both runs give the same 15 box rows:
  - 4 x value 1 from `0x0040ffa1` at `DAT_0063ba8c` 1.
  - 4 x value 1 from `0x0040eeb3` at `DAT_0063ba8c` 2, then another 4 at 3. FUN_0040e590 runs twice before GO.
  - At frame 1084 (sub-state 6): `kill(0)` then 2 (`0x0042300a`), `resolve3`, `kill(1)` then 2, `kill(2)` then 2, then
    sub-state 7.
  - No `box3` row, and FUN_00410d10 runs 220 times. The capture ends in sub-state 7, so no second round is observed.
- **V1**: the d3close `b3r` argv (forced box 2 on slot 0), trimmed to `--statediff-puhook`, plus `--elim-probe --box-probe
  --hold 60`. The `0x0045bc85` hook fires: slot 0 at frame 1074, the forced value. The forced write itself produces no
  `boxset` row, so the forcing path does not enter `0x0045ba00` through the hooked entry.
  - At frame 1082: **`kill(1)` alone** (an AI car, the race goes on), then 2, then `box3` slot 1 in the same frame with
    `[EDI+0x18]` = `0x5f9b58` (non-zero, so FUN_0045bac0 deactivates).
  - At 1098: `kill(0)`, `kill(3)`, then sub-state 7.
- **P2 holds** on all three captures. 0 unknown return addresses. 3 of 3 kills (O1, O2, V1) are followed by a `0x0042300a`
  write of 2 to the same slot. No `0x00425134` / `0x004251cd` / `0x0040c57d` row in this recipe.

## 4. P3 / P3a

P3 as first written DIFFERED at index 8. That row is the original's second FUN_0040e590 block, and the port had one.
Amendment 1 added the second block (1 over 1, the values are the same). It also re-stated the span as the original's own rows,
because the P3 span runs to the first grid after the kills and no original capture reaches that grid.

**P3a holds.** `B_BOXR_K0`, cut to 15 rows, matches O1 and O2 as (class, slot, value) rows: start x4, grid x4, grid x4, then
kill 0, kill 1, kill 2. No 3 follows in either, because sub-state 7 gates the dispatcher in both.

Past the span (port only): `B_BOXR_K0` regrids at frame 402 and 752 (all four slots back to 1). `B_BOX_K0` (u9779 knobs off)
kills one car at a time (220 / 344 / 754). Each kill is followed by 3 on the next dispatcher call, which is V1's grammar. The
original latches in the same frame because FUN_00410d10 (FUN_004111c0 `0x004112cd`, u9779 RESULT section 1) runs before the dispatcher (`0x0040fcd0` in
FUN_0040fc00). The port's elimination runs after that frame's dispatch. Both latch on the next dispatcher pass after the kill.
[UNCERTAIN] whether anything reads the box state between the original's kill and its same-frame latch: no capture records a
reader there.

## 5. Gates and report arms (`run_arms.py`, `arms1/` exe `31381b32`, `arms2/` exe `8ff23b60`)

| arm | knob | sha8 | expected |
|---|---|---|---|
| `H_DEF` step / gates / ptrace | unset | `bacfc747 / 8dd1c004 / 6f9932b6` (both exes) | equal |
| `B_DEF_K0 / K1 / K26` | unset | `f9deb9b9 / d3601fcc / 9da9a26a` (both exes) | equal |
| `B_BOX_K0` | BOX | `f9deb9b9` | report |
| `B_BOXR_K0` / `B_ER_K0` | BOXR / control | `788db6af` / `788db6af` | report: the box knob does not move the u9779 chain |
| `H_BOX` | BOX | `2b695761 / d7cda332 / 2ec66daf` (both exes) | report: **G-CTRL moves** |

**P1 holds.** All runs exit 0 and self-exit. `H_BOX`'s trace has 18 kills and 18 latches to 3. In G-CTRL, eliminated cars now lose
their armed power-up (FUN_0045bac0) where the old 4 froze it. That is the original's behaviour by the listing and by V1. A flip
therefore re-bases G-CTRL to `2b695761 / d7cda332 / 2ec66daf`. B is unchanged.

## 6. Not closed, next

- FUN_00424eb0's two respawn writes (`0x0042512f` / `0x004251c8`, modes 9 / 7 and 4) and FUN_0040be50 case 6 (`0x0040c578`)
  are not ported. Neither fires on this recipe. Their slice needs a mode 4 / 7 / 9 capture and the event table
  `DAT_007e9de4` source.
- The second round is unobserved on the original (sub-state 7 holds to the end of a 40 s hold). The port's per-round grid is
  read off the listing only.
- `MASHED_BOX_PRODUCERS` sends `aib_game_sub_mode()` to the dispatcher. That differs from 6 only under `MASHED_ROUND_END7` /
  `MASHED_SUBSTATE`.
- Tracker: there is no hooks.csv move here. Any C-level change for `0x0045ba00` callers goes through `re-classify`.
- Decision for the lead: flip `MASHED_BOX_PRODUCERS` default ON (G-CTRL re-base above) or keep it OFF until the respawn and
  event producers land. Recommendation: flip. The ported writes match the original on every row observed, and the old
  per-frame 4 is a stand-in the original never writes for a slot that has a car.
