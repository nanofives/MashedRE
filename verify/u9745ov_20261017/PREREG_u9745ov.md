# PREREG u9745ov: car 2's steer during the second override (2026-10-17)

Branch `work/u9745ov` from `work/u9749` `4e98d54b5` (player heading assist default ON). Model claude-opus-5-5, account2.
Written and committed before any knob arm runs.

## 1. Measured so far (no knob)

- Car 2's physics through the second override matches the original (`B_SAR2F_K1` dump against `o_c2r1.msd`): heading within
  6e-5, yaw rate within 1.4e-4, speed 1340.51 / 1341.14 on window calls 57..70.
- `FUN_00416250` never reads the override timer `[0x0089a4fc + v*0x74]` (writers / readers: `FUN_00417370`, `FUN_004173b0`,
  `FUN_00418560` only, `datarefs_89a4fc.txt`); the replay at `0x004187d5..0x004187f0` overwrites the control bytes after it.
- At call 60 the port sets the global slow-line flag `DAT_0089a368` (the `FUN_004177b0` band-0 roll, prob 20 for row 2 from
  `0x005f30a0`, one shot per race) and car 2's lookahead jumps. `o_c2r1` / `o_pw1` lost that roll (flag 0 on all 220 calls);
  so did 13 of 18 `d3_regimes` captures. `o_reg_r13` (the regime-B reference), r15, r18 won it (flag 1) and r19 (the regime-C
  reference), r20 got 2, **all at call 61**. The port's K1 gets 1 and K26 gets 2, **at call 60**.
- Against `o_reg_r13` car 2's steer from call 60 is the original's shifted one call earlier (port 6, 4, 2, -7 on calls 60..63,
  original 9, 6, 4, 2, -7 on 60..64) and equal from call 64 on.
- Cause from the bytes (`disasm_40fc00_calls.txt`, `disasm_0ff8_writers.txt`): `FUN_0040fc00` stores `DAT_007f1008`
  (`0x0040fc63`), calls `FUN_00425a40` (`0x0040fc6c`, the AI tick and every car's physics) and the rest of the frame, and adds the
  budget to `DAT_007f0ff4` / `DAT_007f0ff8` only at its end (`0x0040fe4f..0x0040fe64`, RET `0x0040fe6c`). The port's
  `Ai_AdvanceClock` adds both before the AI tick, so every port reader sees one budget more. Readers of `DAT_007f0ff8`
  (`datarefs_7f0ff8.txt`): `FUN_00416250` (`0x004168d2`), `FUN_00416a30`, `FUN_00417180`, `FUN_004177b0` (`0x00417b2f`),
  `FUN_00417da0`, `FUN_00422ba0`, `FUN_004252c0`, `FUN_0046ddb0` (`0x0046e0a9`; the port's A5 reads a stub `g_raceTimer` instead).

## 2. Knob `MASHED_AI_CLOCK_LATE=1` (default OFF)

`Ai_AdvanceClock` stores `DAT_007f1008` only; `Ai_AdvanceClockLate` adds the budget to `DAT_007f0ff4` / `DAT_007f0ff8` at the
end of the race frame (after the AI tick, the physics and the event drain).

## 3. Arms (`verify/u9749_20261017/run_arms.py`; tags `CL` = the knob, `SACL` = SA + CL)

| gate | measure | pass rule | expectation |
|---|---|---|---|
| G-NOOP | knob off on the new exe: `B_SA_K1`; `H_DEF` | `c517e79f`; `a34714b4 / 0f8ef6df / 852a255d` | PASS |
| G-ROLL | `B_CL_K0` / `_K1` / `_K26`, car 2 window | flag 0 / first 1 at call 61 / first 2 at call 61 | PASS |
| G-STEER2 | `B_SACL_K1` car 2 steer on calls 56..80 against `o_reg_r13` | equal on every call | PASS on 60..64; later calls not predicted |
| D3 (b) | `B_CL_K{0,1,26}`, `B_SACL_K*` against DEF / SA | default-ON candidate only if no car that passes on DEF turns to FAIL | not predicted |
| D2 | `D2_CL_1..3`, each alone | the d2close bounds, H1 398/400, >= 1600 lines | PASS, unchanged (the player's physics reads no `0ff4` / `0ff8` in the port) |
| 13 tracks | `run13.py` with CL | 0 RED, rows against 866 | not predicted |
| G-CTRL | `H_CL` | reported | moves |
