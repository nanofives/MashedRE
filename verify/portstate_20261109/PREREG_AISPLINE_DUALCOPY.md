# PREREG PORT-AISPLINE-DUALCOPY: CatmullRom calls the one exe body of 0x00443300

Step PORT-AISPLINE-DUALCOPY (ledger D4). Lead cmw7xu1960f37p41557tefpnd. Branch `orch/PORT-AISPLINE-DUALCOPY` from
`orch/PORT-AISPLINE-FLIP` `c0afb40de` (L-104 (a): `AiSplineEval_x87` default = `AiSplineEval_x87_ddk`), with
`orch/PORT-X87-PC24` `ab22937fa` merged (knob `MASHED_X87_PC24`, default OFF). Committed alone, before any arm.

## 1. Change

`Ai/AiStandalone.cpp` `CatmullRom` (the exe's race spline, `:306`): in the standalone it calls `AiSplineEval_x87_ddk`
(`Match/MatchS08_00443300.cpp`) and its d3b `compL` arm (AI LUT mask bit 0x4) is deleted. The `count <= 0` guard stays
(port-only). The `.asi` keeps its source-order form (`AiLutMask()` is 0 there, so nothing changes in the `.asi`).
Side effect: the look replay (`MASHED_AI_LOOK_REPLAY`, `s_aiLutOverride = 0` in mode 0) used the source-order `comp`
form in the exe; it now gets the DDK body too. Report-only tool, no gate reads it.

## 2. Why it should be value-neutral (static, before arms)

- The deleted `compL` arm is in `AiStandalone.cpp`, an `/arch:SSE2` TU (not in `x87_tus.txt`). Its two lambdas in
  `mashed_re.exe` `55e5c513` (`0x0007bb90`, `0x0007bc60`) are scalar SSE2 only (`mulss` / `addss` / `subss`, one `fld`
  for the float return), so each op rounds to float and the result does not depend on the x87 CW.
- `AiSplineEval_x87_ddk` (`x87_tus.txt`, `/arch:IA32`) assigns every op to a float, so each intermediate goes through
  `fstp dword` (PORT-AISPLINE-DDKORDER section 2) and does not depend on the CW either.
- Both evaluate the same ops in the same order (0x00443373..0x00443427): `((A*t2 + B*t3) + (P2-P0)*t) + (P1+P1)`, x0.5,
  with A and B formed the same way. Index wrap and [0,1] clamp are the same. Point layout: `Path_00443300` = 64
  `{x, z}` floats then `count` at +0x200, which is what `CatmullRom` read (`spline + i*8 + o`, count at +0x200).

## 3. Predictions

- **P1.** Knob OFF: G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3 (`merge_train.json`). Warzone W_DEF
  `eeb92153` (step) / `a329ca22` (pontoon) / `fa8618df` (seek), the value PORT-X87-PC24 and PORT-AISPLINE-DDKORDER measured
  with and without `MASHED_AILEAF=1` (W has no `merge_train.json` reference).
- **P2.** Knob ON (`MASHED_X87_PC24=1`): the same hashes (PORT-X87-PC24 measured ON = OFF on H, B and W).
- **P3.** W_AL (`MASHED_AILEAF=1`) = `eeb92153 / a329ca22 / fa8618df`.
- **P4.** `scripts/lint_rva_bodies.py`: OK, no new duplicate body. The allowlist does not change: 0x00443300 has no
  row there (the scanner never bound `CatmullRom`'s banner to the RVA).

A gate hash that moves makes the swap a behaviour change: it then ships behind a default-OFF knob and the hashes go
back to the lead.

## 4. Arms

Through `mashed_lock.py run`, test-slot outside, root `D:/MashedWT/PORT-AISPLINE-DUALCOPY_root`, data
`D:/MashedWT/PORT-AISPLINE-DUALCOPY_data`: H_DEF, B_DEF_K0/K1/K26, H_X24, B_X24_K0/K1/K26, W_DEF, W_X24, W_AL.
