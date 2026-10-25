# PREREG D3-CF-BOX: the box-state producers of DAT_0068d1f0, slice 1

Written and committed before any arm below runs. Branch `orch/D3-CF-BOX` from `282ea2ec7`.

## 1. What the original does (read-only pool clone `Mashed_pool10` of the anchor)

`FUN_0045ba00` (`0x0045ba00..0x0045ba0f`) is the bare setter `DAT_0068d1f0[idx] = val`. It has 7 call sites:

| call | caller | value | condition (read off the listing) |
|---|---|---|---|
| `0x0040ff6b` | FUN_004111c0 race start | 4 | slot loop `0x0040ff50..0x0040ffb6`, `*(*(int*)0x005f2770 + 0x34 + i*4) == 0` (`CMP [EDI+ECX],EBX` `0x0040ff56`, EBX = 0) |
| `0x0040ff9c` | FUN_004111c0 race start | 1 | same loop, the dword is non-zero |
| `0x0040eeae` | FUN_0040e590 grid placement | 1 | slot loop `0x0040ed34..0x0040eebb`, `*(PTR_005f2770 + 0x34 + i*4) != 0`, after FUN_004704c0 / FUN_0046d4a0 / FUN_0040e3a0 |
| `0x00423005` | FUN_00422fd0 kill | 2 | `param_1 < 0x10`, after FUN_0046c5c0 / FUN_0046c790(v,1) / FUN_004215c0 x2 |
| `0x0042512f` | FUN_00424eb0 | 1 | `DAT_007f0fd0` 9 or 7, car state 0xff respawn arm |
| `0x004251c8` | FUN_00424eb0 | 1 | `DAT_007f0fd0` 4, car state 0xff respawn arm |
| `0x0040c578` | FUN_0040be50 event case 6 | 2 | `DAT_007e9de4[i] == 6` and `DAT_007ea9e4[i*8] == -0xff80`, slot = `DAT_007ea5e4[i] - 0xff00` |

FUN_00422fd0 has 30 call sites. The one on the default race path is the elimination block of FUN_00410d10
(`CALL 0x00422fd0` at `0x00411025`, followed by `FUN_0040eee0(victim, 1)` at `0x0041102d`). FUN_0040eee0 and FUN_0040d590 kill
further cars through it (u9779, `verify/u9779_20261021/RESULT_u9779.md` section 1).

The only reader is the dispatcher FUN_0045bba0 (`0x0045bc6b`): 4 skips the slot pass, 2 writes 3 (`0x0045bc85`,
unconditional in that arm) then deactivates through FUN_0045bac0 (`0x0045bc98`) when `[EDI+0x18]` is non-zero, 3 skips the
pass. The whole per-slot pass runs only when FUN_0040e350() returns 6 (`0x0045bc2b`). The port has this gate
(`Powerup/PowerupSystem.cpp`, `Tick`).

## 2. What the port does today

`TrackRenderer::TickPowerupDispatch` writes `SetBoxState(s, alive ? 1 : 4)` on every frame. A car that is eliminated in a
round therefore reads 4: its pass is skipped, its armed power-up is never deactivated. States 2 and 3 never occur live.
The dispatcher is called with race state 6 always.

## 3. The change (slice 1), knob `MASHED_BOX_PRODUCERS=1`, default OFF

With the knob unset, nothing changes: the per-frame 1 / 4 write stays and no new write runs.

With the knob set:
- the per-frame write keeps only the 4 for slots that have no car (no producer ever writes such a slot);
- `StartRound` (the standalone's race init and its per-round regrid) writes 4 / 1 by "slot has a car" at race init
  (FUN_004111c0 `0x0040ff6b` / `0x0040ff9c`), then 1 for every slot with a car (FUN_0040e590 `0x0040eeae`);
- every standalone elimination writes 2 for the victim (FUN_00422fd0 `0x00423005`): the two UpdateRace elimination sites
  and `KillCar` (the team arms, the u9779 resolve arms, the rank-3 callback);
- the dispatcher gets `aib_game_sub_mode()` (the port's FUN_0040e350) instead of the constant 6. That is 6 unless
  `MASHED_ROUND_END7` or `MASHED_SUBSTATE` is on.

`MASHED_BOX_TRACE=<file>` (default OFF) writes one line per box write: frame, class (start / grid / kill / 3), slot, value,
previous value.

Out of slice 1, named, not ported: the two FUN_00424eb0 respawn writes (modes 9 / 7 and 4) and FUN_0040be50 case 6.

[UNCERTAIN] the order of the FUN_004111c0 and FUN_0040e590 writes at race init. The capture O1 decides it. The port emits
start, then grid.

## 4. Arms

Original, private root `D:\MashedWT\D3-CF-BOX_root`, under `mashed_lock.py`:
- `O1`, `O2`: the u9779 `o_ep_1` recipe (`--track 0 --mode 10 --cars 4 --car 0 --poke-ctrl-slots --statediff-car 3
  --statediff-aistep --alive-probe --elim-probe`) plus the new `--box-probe` (entry hook on `0x0045ba00`: idx, value, return
  address, previous value; instruction hook on `0x0045bc85`: slot, `[EDI+0x18]`, previous value), `--hold 40`.

Standalone, `run_arms.py` copied into this folder with stacks `BOX` (`MASHED_BOX_PRODUCERS` + trace) and `BOXR` (`BOX` +
`MASHED_ELIM_RESOLVE` + `MASHED_ROUND_END7`, the u9779 chain):
- `H_DEF`, `B_DEF_K0`, `B_DEF_K1`, `B_DEF_K26`: knob unset.
- `B_BOX_K0`, `B_BOXR_K0`, `H_BOX`: knob set.

## 5. Predictions and pass rules

- **P1 (gates).** Knob unset: G-CTRL `bacfc747 / 8dd1c004 / 6f9932b6`, B `f9deb9b9 / d3601fcc / 9da9a26a`. Any change is RED.
- **P2 (the original's grammar).** In O1 every `boxset` row has one of the return addresses `0x0040ff70`, `0x0040ffa1`,
  `0x0040eeb3`, `0x0042300a`, `0x00425134`, `0x004251cd`, `0x0040c57d`. Every `kill` row is followed by a `boxset` row with
  return address `0x0042300a`, the same slot and value 2. Rows from `0x00425134` / `0x004251cd` / `0x0040c57d` in O1 mean a
  slice-1 gap on this recipe and are listed in the RESULT.
- **P3 (port = original, the decision test).** Reduce O1 to the sequence of (class, slot, value) by return address
  (start / grid / kill) plus the `box3` rows (class 3), up to and including the first grid writes after the first round
  end. Reduce `B_BOXR_K0`'s trace the same way over the same span. P3 holds when the two sequences are equal. Frame numbers
  are not compared (the two clocks differ).
- **P4 (report only).** `H_BOX` / `B_BOX_K0` hashes and box-trace counts. No pass rule: they show whether the knob moves
  the default scenarios.

Verdict: P1 and P3 hold -> report DECISION (the lead files the flip), knob stays OFF. P1 fails -> RED. P3 fails -> the
mismatch is named and the knob stays OFF.
