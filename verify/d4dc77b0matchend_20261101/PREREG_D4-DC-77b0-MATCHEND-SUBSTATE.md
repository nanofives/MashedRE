# PREREG D4-DC-77b0-MATCHEND-SUBSTATE: match-end sub-states 0xb / 9, knob `MASHED_MATCHEND_SUBSTATE` (2026-11-01)

Branch `orch/D4-DC-77b0-MATCHEND-SUBSTATE` from `orch/integration` `6d9d01a7f`. Committed alone, before every knob arm.
Raw data `D:\MashedWT\D4-DC-77b0-MATCHEND-SUBSTATE_data`. Private root `D:\MashedWT\D4-DC-77b0-MATCHEND-SUBSTATE_root`.
Input: `verify/d4dc77b0runtime_20261101/RESULT_D4-DC-77b0-RUNTIME-PARKED.md` (branch `orch/D4-DC-77b0-RUNTIME-PARKED`),
leg 2 case A and leg 3.

## 1. Static reading (`original\MASHED.exe.unpatched`, capstone)

FUN_00410510's conclusion LAB_0041062a (reached when ESI, the result, is non-zero, `0x00410622 cmp esi,ebp` /
`0x00410624 je 0x00410838`):

| address | instruction | port under the knob |
|---|---|---|
| 0x0041062a / 0x0041062f / 0x00410635 | call FUN_0045bed0, FUN_0045bf30, FUN_00458f80(0) | **not ported** (powerup teardown, named gap) |
| 0x00410645 | `mov [0x0063ba8c], 0xb` | yes |
| 0x0041064f | `mov [0x0063b90c], ebx` (ebx = 1 from 0x00410640) | yes |
| 0x00410655 | `mov [0x0063b910], 0x442f0000` (700.0f) | yes |
| 0x0041065f / 0x00410669 | `mov [0x0063b918], 0` / `mov [0x0063b91c], 0` | yes |
| 0x00410673 | `je 0x0041080f` when ESI == -1 | yes |
| 0x00410679..0x00410693 | ESI != -1: edi = ESI - 1, FUN_00448700(1, edi) (call 0x0041067e), [0x0063b914] = edi, [0x007f0fcc] = (edi == 0) | yes |
| 0x0041080f..0x0041081f | ESI == -1: FUN_00448700(1, 0) (call 0x00410811), [0x0063b914] = 1, [0x007f0fcc] = 0 | yes |
| 0x00410825..0x00410830 | FUN_00426c00() == 0x26 -> FUN_0044df80(0) | **not ported** (named gap) |

FUN_00448700 (`0x00448700`, already ported as `VehicleSeedWritePair`, `Vehicle/VehicleSeed.cpp`): 100 x FUN_004464c0(0x00897fe0),
then `[0x00897ffc] = param_1`, `[0x00898000] = param_2`. FUN_00443080 (`0x00443080`) is `mov eax,[0x00897ffc]; ret`.

FUN_004111c0 case 0xb (`0x00410ab0..0x00410b25`), after its FUN_0040fc00 call:

```
0x00410ab0  fld [0x0063b91c] ; fadd [0x007f100c] ; fst [0x0063b91c]
0x00410ac2  fcomp [0x005cc574] (2.0f) ; fnstsw ax ; test ah,5 ; jnp 0x00410b25     -> continue only when !(b91c < 2.0f)
0x00410acf  ecx = 0x007f1a14 ; edx = 0 ; loop to 0x007f1a54 step 0x10 (0x00410b1a..0x00410b23)
0x00410ad6    eax = [ecx] ; == -1 -> next ; eax *= 0x4c
0x00410ae0    byte [eax+0x7f1042] != 0 && byte [eax+0x7f1502] == 0 -> store
0x00410af0    byte [eax+0x7f1041] != 0 && byte [eax+0x7f1501] == 0 -> store, else next
0x00410b00  store: [0x0063ba8c] = 9 (0x00410b02) ; FUN_0040e460(1) (0x00410b0c, [0x0063b908] = 1) ; [0x0063b90c] = edx (0)
```

The loop does not break after a store. Case 9 (`0x0040ddf0`) calls FUN_0040ddb0 (FUN_00417750, FUN_00426c50, FUN_00475a60,
FUN_0045bf30, FUN_00476430, jmp FUN_004046a0) and stores 0xa (`0x0040ddf5`). It does not call FUN_0040fc00, so no
FUN_004177b0 call in 9 (measured: RUNTIME-PARKED leg 2).

## 2. The exe today (`orch/integration` `6d9d01a7f`)

- The rule engine block of `TrackRenderer::UpdateRace` is the FUN_00410510 caller (FUN_00411170 contract). On `r > 0` it
  sets `match_winner_ = r - 1`, on `r == -1` `match_winner_ = 0` + `match_draw_`. It writes no sub-state cell.
  `RuleEngine.cpp` leaves `OnConclude` empty in the exe.
- `aib_game_sub_mode()` (FUN_0040e350 host) returns 7 under `s_roundEnd7`, else 0x0063ba8c, 6 while that is 0. So after a
  match end it returns 6: RUNTIME-PARKED x_r0_t0c measured 11387 `AiPreTickRubberBand` entries in sub-mode 6 after
  `R6 MATCH OVER`.
- `aib_ai_target_enable()` (FUN_00443080 host) returns the constant 0. The original measured 1 at every FUN_004177b0 entry
  in 0xb (RUNTIME-PARKED leg 3, sites 4 / 5).
- The exe never writes 0x0063b908 / 0x0063b90c / 0x0063b910 / 0x0063b914 / 0x0063b918 / 0x0063b91c / 0x007f0fcc in the race.
- The exe cooks ctrl byte 0xa (`0x007f1042 + idx*0x4c`, action 4) each frame (`CookCtrlByteAFromKeyboard`, R7 input, default
  ON). It does not cook byte 9, and does not port the shadow copy into `0x007f14f8..` (so `0x007f1501` / `0x007f1502` stay 0).

## 3. Knob `MASHED_MATCHEND_SUBSTATE` (default OFF)

Under the knob:

- (a) **End block.** At the rule engine's match end (`r != 0`, the exe's LAB_0041062a), the stores of section 1 marked
  "yes", in that order, with ESI = r. FUN_00448700 is `VehicleSeedWritePair`.
- (b) **FUN_00443080.** `aib_ai_target_enable()` returns `[0x00897ffc]`. The only exe writer of that cell is
  `VehicleSeedWritePair`: (0, 0) at phase 3 entry and (1, w) from (a).
- (c) **Case 0xb.** Once per race frame while `[0x0063ba8c] == 0xb`, after that frame's AI tick and physics (the
  FUN_0040fc00 call) and not on the frame of (a): the section-1 accumulate, compare and slot loop, reading the cells
  `0x007f100c`, `0x007f1a14 + v*0x10`, `0x007f1041 / 0x007f1042 / 0x007f1501 / 0x007f1502 + idx*0x4c` as they are.
- Arm-test only: `MASHED_MATCHEND_PRESS=<n>` adds a one-frame press of the bound action-4 key on the n-th cook frame with
  `[0x0063ba8c] == 0xb` (the same lookup as `MASHED_R7_INPUT_PRESS`).

Not ported (named): the powerup teardown and FUN_0044df80 in (a). Case 9's FUN_0040ddb0 and its 0xa store: in state 9 the
exe keeps running its race frame (AI tick in sub-mode 9, phase 5 off), where the original runs no FUN_0040fc00. The exit to
results stays the exe's own 3.0 s `GameFlow_RequestResults` hold. The shadow copy `0x007f14f8` (so a held byte counts as a
press). Case 0xa (a frontend re-launch only, the exe has none). `Fi_GameMode()` stays 6 in 0xb (the physics gates treat
0xb like 6, PREREG_D3-CF-U9779 section 1) and in 9. The exe's other match-end sites (`NextRoundOrEnd`'s score > 11,
`ForceMatchEnd`) are not FUN_00410510 and are left as they are.

## 4. Arms

Runner `verify/d4dc77b0matchend_20261101/run_arms.py` (copy of `verify/savelink_20261023/run_arms.py`, adds stack `ME` =
`MASHED_MATCHEND_SUBSTATE=1`). Probe `exe_vsprobe.py` (copy of RUNTIME-PARKED's, extra `--env`).

| arm | what |
|---|---|
| H_DEF, B_DEF_K0, B_DEF_K1, B_DEF_K26 | default gates, knob unset |
| H_ME, B_ME_K0, B_ME_K1, B_ME_K26 | the same with the knob set |
| x_off | exe probe, x_r0_t0c recipe (`MASHED_R7_INPUT_PRESS=150`), knob unset |
| x_me | x_off + `MASHED_MATCHEND_SUBSTATE=1` |
| x_mep | x_me + `MASHED_MATCHEND_PRESS=150` |

## 5. Predictions

- P0: knob unset, G-CTRL = `8284be46 / ba407a16 / 31fe043e`, B = `1d5bf4f9` x3 (`merge_train.json`).
- P1: H_ME and B_ME_* hash equal to P0. Neither recipe reaches a match end (x_r0_t0 sat in state 7 after its first round
  end with no input, B runs 1200 frames), so (a) and (c) never run and (b) reads the 0 written at phase 3.
- P2: x_off reproduces RUNTIME-PARKED x_r0_t0c's shape: B entries in 6 with `ffc` = 0 after the match end, no 0xb.
- P3: x_me. Every B and T row up to the match-end frame is the same tuple sequence and call count as x_off. After it: the
  watch shows one 0x0063ba8c store 6 -> 0xb and one 0x00897ffc store 0 -> 1 in `VehicleSeedWritePair`, every later B entry
  reads ba8c = 0xb with ffc = 1, **0 B entries with ba8c = 6 after the 0xb store**, no store of 9.
- P4: x_mep. As x_me up to the press. The 9 store comes on the first cook frame >= 150 frames into 0xb whose
  `0x0063b91c` has passed 2.0. B entries after it read 9 with ffc = 1.
- P5: no crash or hang in x_me / x_mep. The player car (slot 0) is now stepped by `Ai_Standalone_Tick`'s
  `ai_target_enable() == 1` arm after the match end, as FUN_00418860 does. A crash there is RED for the knob, not a re-run.

Verdict rule: P0 and P1 hold and P3 / P4 hold -> GREEN, flip filed as a decision row. P0 or P1 fails -> RED (the knob is not
inert). P3 / P4 fail -> RED with the measured difference.
