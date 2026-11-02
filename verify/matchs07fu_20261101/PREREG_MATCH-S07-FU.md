# PREREG MATCH-S07-FU, 2026-11-01

Committed alone, before any arm runs. Code under test: `orch/MATCH-S07-FU` @ 758d209c0 (exe built from that tree,
`MASHED_NO_DEPLOY=1`).

## What changes and where

Eight MATCH-S07 matched sources become their RVAs' only port bodies (`Match/MatchS07_<va8>.cpp`). Exe reach from
`_WinMain@16` on this build (`verify/matchs07_20261101/tools/exe_reach.py`, direct call/jmp graph):

| rva | symbol | exe direct callers | reachable from WinMain |
|---|---|---|---|
| 0x0042b960 | CarSlotInit1P | 0 | no |
| 0x0042b9e0 | CarSlotAssign | 1 (RenderFrame, Team Select) | yes |
| 0x0042d290 | MenusLapTimeFmt | not in the exe (.asi only) | n/a |
| 0x0042f7b0 | FrontendCursorUpdate | 1 (RenderFrame, Ability Select, knob arm only) | yes |
| 0x0042ef40 | VehicleUnlockFlagGet | 0 | no |
| 0x00431d00 (+0x00431b80) | CarSelectReset | 0 | no |
| 0x0042e3a0 | MenuChromeShellA | 0 (the one call is under `if (false && ...)`) | no |
| 0x0042fb70 | MenuSpriteDispatchB | 0 | no |

Two default-OFF knobs keep the pre-swap exe behaviour on the two reachable paths:

- `MASHED_CARSLOT_ASSIGN_SLOTSTATE`: ON, CarSlotAssign's calls to FUN_0040e480 run `CarSlotStateSet` as the original
  does; OFF (default), the exe skips them, as the old body did.
- `MASHED_ABILITY_CURSOR_MATCHED`: ON, Ability Select calls the matched FrontendCursorUpdate (decrement guard `!= 0`);
  OFF (default), it calls `FrontendCursorUpdatePreSwap` (the old body, guard `> 0`).

## Arms

Runner `verify/savelink_20261023/run_arms.py`, private root `D:/MashedWT/MATCH-S07-FU_root`, raw output
`D:/MashedWT/MATCH-S07-FU_data/arms`, test slot + `mashed_lock.py run --label MATCH-S07-FU`.

| arm | knobs | files hashed |
|---|---|---|
| H_DEF, B_DEF_K0, B_DEF_K1, B_DEF_K26 | none (default exe) | step / gates / ptrace (H), step (B) |
| H_S7FU, B_S7FU_K0, B_S7FU_K1, B_S7FU_K26 | `STACK['S7FU']` = both knobs ON | same |

## Predictions

- **P1 (default unchanged).** H_DEF = G-CTRL `8284be46 / ba407a16 / 31fe043e`, B_DEF_K0/K1/K26 = `1d5bf4f9` x3
  (`re/orchestrator/merge_train.json` refs). Reason: no swapped body runs on the default path of the gate scenario;
  the two reachable paths keep their pre-swap code by default.
- **P2 (knobs inert on the gate scenario).** H_S7FU and B_S7FU_K* give the same hashes as P1. Reason: the gate
  scenario boots straight into a Training race (`MASHED_GAME_MODE=10`, `base_env`), so neither Team Select
  (CarSlotAssign) nor Ability Select (FrontendCursorUpdate) runs. Witness: `ABIL_TICK` absent from each arm's
  `mashed_re.log` (it is printed the first time the Ability Select step runs).

## What a result means

- P1 fails: a swap changed the default build. RED, a real port divergence; file an L-n decision, do not ship.
- P1 holds, P2 fails: a knob reaches the gate scenario. Report the moved hashes; the knob stays OFF.
- Both hold: the default build is unchanged and the knobs are inert on these gates. This does **not** measure what
  either knob does on Team Select or Ability Select: these gates never run those screens. A flip of either knob needs
  an arm that navigates to that screen (named in the RESULT as the next step), and is a roadmap decision row.
