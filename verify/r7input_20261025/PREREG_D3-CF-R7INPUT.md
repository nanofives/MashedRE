# PREREG D3-CF-R7INPUT: the input path that writes ctrl byte 0xa, default OFF (`MASHED_R7_INPUT`) (2026-10-25)

Branch `orch/D3-CF-R7INPUT` from `orch/integration` @ `2e80adace`. `orch/D3-CF-K357` @ `a30d080b5` merged in (`d41cdb478`,
`git -c merge.autoStash=false`, merge base `b5c84688e`, K357 was not an ancestor of `2e80adace`). The merge carries
`MASHED_R7_EXIT` (default OFF) and keeps the U9779 flip of integration (`MASHED_ROUND_END7`, `MASHED_R7_PHYS`,
`MASHED_ELIM_RESOLVE`, `MASHED_RACE_TAIL_LATE` default ON, opt-outs `MASHED_NO_*`). Decision L-10 = (1): `MASHED_R7_EXIT`
stays default OFF. This PREREG is committed alone, before any arm. Exe under test: the port commit's build,
`MASHED_NO_DEPLOY=1`, sha8 `d9a1c7c5`. `original\mashed_re_dev.asi` is not redeployed.

## 1. What the original does (MASHED.exe.unpatched, capstone + headless decomp on `Mashed_pool10`)

FUN_00410860's exit gate (K357 section 1) reads byte `0x007f1042 + idx*0x4c` = byte 0xa of the ctrl block
`0x007f1038 + idx*0x4c`, for each `idx = [0x007f1a14 + v*0x10]`. The port has no writer of that byte. The original's writer:

| step | site |
|---|---|
| FUN_004967e0 runs FUN_00496530 with EBX = i for every i in 0..3 with FUN_00497450(i) != 0 | `0x004967e0`, `0x00497450` (`[0x007e96fc + i*0x200] != 0`) |
| block base `ebp = i*0x4c` | `0x00496536` |
| shadow copy of the block to `0x007f14f8 + i*0x4c`, then the block is zeroed | `0x00496552` (rep movsd), `0x00496568` (rep stosd) |
| `al = FUN_00497310(i, 4)`, then `mov [ebp + 0x7f1042], al` | `0x004965ac..0x004965af`, `0x004965b7` |
| FUN_00497310 type-2 branch: `code = [0x007e95c0 + i*0x200 + 0x108 + 4*4]`, result `0xff` if bit `code` of the bitmap `0x0077313c` is set, else 0 | `0x004973fd`, `0x00497406`, `0x0049740d..0x00497428` |
| the bitmap: FUN_004972b0 zeroes 32 bytes at `0x0077313c` and calls FUN_00496100, which sets bit k when byte k of the 256-byte keyboard `GetDeviceState` buffer (vtable +0x24 of `[0x00772fb8]`) is negative | `0x004972ec..0x004972fd`, `0x00496150`, `0x004961c0..0x004961d8` |
| default binding table for a type-2 slot (FUN_00498510): action 4 = `0x1c` | `0x00498698` (`mov [esi+0x10], 0x1c`, esi = slot + 0x108) |
| slot types: i < J -> 1, i == J -> 2, else 0 (J = attached game controllers) | `0x0049861a`, `0x0049865f`, `0x004986cd` |

`original\` holds no `contcfg%d.bin`, so the FUN_004971b0 override does not apply on this install and the original runs on the
default `0x1c`.

## 2. The port (`mashedmod/src/mashed_re/exe_main.cpp`, `CookCtrlByteAFromKeyboard`, default OFF)

`MASHED_R7_INPUT=1`: once per main-loop frame, after `WriteMashedInputGlobalsFromKeyboard`, for every slot i with
`g_input_slot_type[i] == 2` (the port's copy of the FUN_00498510 types, `DeriveInputSlots`), store
`(g_keys[0x1c] & 0x80) ? 0xff : 0x00` at `0x007f1042 + i*0x4c`. Only byte 0xa is stored. Not ported: the type-1 (joypad)
branch (the standalone polls no joystick), the block zeroing and the shadow copy, the contcfg override. Knob OFF: nothing is
written. `MASHED_R7_INPUT_PRESS=<n>` (arm test only) adds a one-frame press on the n-th cook frame with
`RoundEnd7ExitActive()` true. It goes through the same store, `g_keys` is not touched. Two log lines: `R7IN byte0xa ...` on
every nonzero store (with `test=` and `det_frame=`), and `R7X EXIT det_frame=<f> round <a> -> <b> match_winner=<w>` right
after K357's gate fires and `NextRoundOrEnd()` returns (`MASHED_R7_EXIT` only).

Machine fact: the K357 logs read `input slots: game controllers 1, types 1 2 0 0`, so the keyboard is slot 1 and the store
lands in block 1. During a race the port's AI loader sets `0x007f1a14 + v*0x10 = v` (TrackRenderer.cpp, AiData load), so the
gate sees block 1 through v = 1. In the port the bytes of block 1 that other code reads are 0..7 (physics, TrackRenderer.cpp
`io.input[k] = blk[k]`, k < 8) and 7 / 8 (power-ups). None reads byte 0xa except K357's gate.

## 3. Arms (`verify/r7input_20261025/run_arms.py`, private root `D:\MashedWT\D3-CF-R7INPUT_root`, `mashed_lock.py`)

Stacks: `I` = `MASHED_R7_INPUT`, `X` = `MASHED_R7_EXIT`, `XI` = both, `XIP<n>` = XI + `MASHED_R7_INPUT_PRESS=<n>`.

| id | arm | prediction |
|---|---|---|
| P0 | `H_DEF`, `B_DEF_K0 / K1 / K26` | G-CTRL `ef5f2e10 / 57f7ff83 / b4f4b55a`, B `c37d4c0c / 0a8f9806 / 7013671d` (the flipped integration default) |
| P0b | `B_I_K0` | `c37d4c0c` (the store alone, no gate reads it) and no `R7IN` line |
| P0c | `B_X_K0`, `B_XI_K0` | equal to each other. Expected `3a03670c` (K357's `B_U7PX_K0`, whose U7P stack is the default here). A different value with P0c's equality holding is an integration delta, reported, not a failure of this step |
| P1 | `B_XIP150_K0` | exactly one `R7IN` line, `test=1 r7frame=150`; one `R7X EXIT` line on the same `det_frame` (counter `2*150 = 300 > 199`, `0x004109b9`) reading `round 1 -> 2 match_winner=-1` (K357's `B_U7PX_K0` scores 4,5,7,8 after round 1: no match win). The step CSV differs from `B_X_K0` |
| P2 | `B_XIP50_K0` | one `R7IN` line, `test=1 r7frame=50`; no `R7X EXIT` line (counter 100 < 200 on the press frame); step CSV sha8 equal to `B_X_K0`'s |
| P3 | every arm | no `R7IN` line with `test=0`. One with `test=0` is a real keystroke (the keyboard is opened `DISCL_BACKGROUND`), and that arm is re-run |
| report | `H_XI`, `B_XI_K1 / K26` | hashes reported. K357's `H_U7PX` / `B_U7PX_K1 / K26` (`5943c5f1 / b493262b / 09551e6a`, `2516d903 / 53778664`) are the reference |
| P4 | `D2_XI_1 / 2 / 3` under `run --solo`, `verify/d2rebase_20261019/score_d2_v2.py port` | PASS 3/3 in the d2rebase band. U9779-M's D2_DEF numbers (slip1500 0.1900, slip2000 0.2548-0.2549, drvmed 1970.58, H1 398/400) are the reference. motion_diag at least 1600 lines, else re-run |
| P5 | 13 tracks, `verify/rsky_20261016/run13.py run ... MASHED_R7_EXIT=1 MASHED_R7_INPUT=1`, root `D:\MashedWT\D3-CF-R7INPUT_root13`, scored against `verify/rres_20261016/cites_rres.json` | 0 RED, INSTRUMENT GATES PASS, score table equal to `verify/u9779m_20261025/data/tracks_M_score.txt`. Rows that differ are reported per track |

No MASHED run starts before the lead's go message. Piz-collision crashes (U-9677) are re-runs.

## 4. Outcome rules

- P0 fails: the port changed the default. RED, stop.
- P1 or P2 fails: the store or its timing is wrong. RED for the port, D2 / 13 tracks still reported.
- P4 / P5 out of band with the knobs ON: reported as the cost of the knobs, input to the flip decision.
- All hold: verdict DECISION. The flip of `MASHED_R7_EXIT` (+ `MASHED_R7_INPUT`) goes back as a ledger decision row with the
  measured hashes. Nothing is flipped here.

## 5. Scope

[UNCERTAIN] the joypad branch: with a controller on slot 0 the original's player 1 could exit sub-state 7 with button 0
(type-1 default binding 0, `0x00498640`). The standalone polls no joystick, so that path is not ported or measured.
[UNCERTAIN] the slot table: in the original the human car's `0x007f1a14` entry is the profile that pressed confirm in the
frontend. The port seeds `v -> v`. On this machine the gate still sees the keyboard's block through the AI car on v = 1, which
is a port-side table and not the original's mapping.
