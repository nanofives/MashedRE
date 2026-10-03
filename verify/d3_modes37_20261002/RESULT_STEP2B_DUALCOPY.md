# STEP 2B RESULT — the dual-copy hypothesis for AI criterion (b), tested and ANSWERED

The kickoff names a second live candidate for (b): *"8 AI rows were demoted C3→C2 on
2026-09-29 by the dual-copy fix (`DUAL_COPY_FIX_2026-09-29.md`), a live candidate
explanation for (b) that has not been tested yet"*, to be tested *"if the port leaves (b)
failing"*. The port was not written (STEP 2 G2-SIM refuted its premise) and (b) is still
failing, so the test is due. **It is now done, statically and decisively, and no game code
was edited.**

## Answer

> **Exactly ONE of the 8 demoted rows can bear on (b) in the scored window: `0x00416250`.
> Its defect is `int mode = 0;` at `AiStandalone.cpp:844`. The two other divergences the
> demotion note calls "load-bearing" — the pinned `rate1` and the velocity-derived heading
> — live in code paths this window provably does not execute.** And `0x00416250`'s defect
> is the *same* `int mode = 0` that STEP 2 already measured: large, and **not sufficient**.
>
> **So the dual-copy hypothesis and the modes-3/7 hypothesis are one hypothesis, it has
> been measured, and it does not close (b).**

## Which control-step variant the window runs — proved, not assumed

`VehicleStep` (`AiStandalone.cpp:1600-1617`) picks the variant from
`s_host.game_mode_fd0()` (`DAT_007f0fd0`):

```
fd0 == 4 || fd0 == 9  ->  ControlStepM49  (FUN_00416a30)   AiStandalone.cpp:1611
fd0 == 8              ->  ControlStepM8   (FUN_00417da0)   :1613
else                  ->  ControlStep     (FUN_00416250)   :1615
```

**Proof it is `ControlStep`, from the running original's own data and nothing else:**
the original's window contains **`ai_mode == 7` on 80 of car 1's 220 calls**
(`verify/d3_modes37_20261002/o1.msd.aistep.csv`, STEP 2 G2-MODE). Mode 7 is committed at
**`0x0041642f`**, which is inside **`FUN_00416250`** (`Ai/AiControlStep.cpp:172`).
`ControlStepM49` has no targeting chain and **never commits a mode at all**
(`AiStandalone.cpp:977-1050`; `AiControllerAB.cpp:587` "M49 never commits").
Therefore `FUN_00416250` is what ran. The port's own `VehicleStep` takes the same `else`
branch, so both sides are on the same variant.

## Row-by-row verdict on the 8 demotions, against the window

| # | RVA | exe-copy defect (from the demotion note) | lives in | bears on (b) here? |
|---|---|---|---|---|
| 1 | `0x00416250` | **`int mode = 0;` → the whole targeting chain unreachable, modes 1,2,3,5,7,8,9,10 never run** (`AiStandalone.cpp:844`) | **`ControlStep` — the window's own variant** | **YES — and it is the only one** |
| 2 | `0x00416a30` | `rate1` pinned `0.0f` (`:983`) → `if (rate1 <= k20f) mag *= kSteerExtra;` (`:1002`, `:1023`) always fires | `ControlStepM49`, reached only when `fd0 ∈ {4,9}` | **NO — path not executed** |
| 3 | `0x00417da0` | same, `rate1 = 0.0f` at `:1102`, firing at `:1121`/`:1142` | `ControlStepM8`, reached only when `fd0 == 8` | **NO — path not executed** |
| 4 | `0x00415e20` | heading taken from **velocity** (`:175`) instead of body-forward | `SteerAngleError`, called **only** from `ControlStepM49` (`:990`) and `ControlStepM8` (`:1109`) | **NO.** `ControlStep` calls **`SteerAngleErrorFwd`** at `:858` — the **correct** body-forward form (`FUN_0046d510` → `rec+0x9d4/+0x9dc`), with the `<= 0` wrap idiom. The source says so at `:198`. |
| 5 | `0x004177b0` | rubber-band fed from globals no exe TU writes | `AiPreTick` / `RuleEngine` | not in the control-step path; **untested here**, `[UNCERTAIN]` |
| 6 | `0x00418560` | three missing arms + spline-index reset | `SelectSpline`, upstream of the window | **untested here**, `[UNCERTAIN]` — it feeds `curv`/`err` and is a live candidate for STEP 2's carrier 2 |
| 7 | `0x00418860` | missing `DAT_007f0fd0 == 7` force-step of vehicle 0 | per-frame tick, vehicle **0** = the player | **NO — wrong vehicle**; (b) scores cars 1..3 |
| 8 | `0x00443080` | exe returns literal `0` instead of `*(uint32*)0x00897ffc` | `TrackRenderer.cpp:96` | **NO.** The literal is **measured correct**: `tgt_7ffc` is 0 on 6259/6259 AI steps of `verify/d3_ai_20260926/o_inputs.msd.aistep.csv`, and this session's cross-side collateral puts `tgt_7ffc` **within the noise floor in every shared band**. It is a C2-grade stub, not a behavioural defect. |

**Corollary worth stating plainly, because the demotion note implies otherwise:** the note
(`DUAL_COPY_FIX_2026-09-29.md:177-180`) calls out `rate1` and the velocity heading as
*"the load-bearing ones"* and adds *"D3 criterion (b) is the phase's sole open blocker and
it runs on these."* **It does not run on those two.** It runs on `FUN_00416250`, and both
named defects sit in the M49/M8 variants that `fd0` does not select on this recipe. The
demotions themselves stand — the evidence genuinely does not cover the exe copies — but
their stated connection to (b) is corrected here.

All of `0x00416250`'s other listed exe divergences reduce to the same root: the two
missing early returns (`0x004163fd`/`0x00416405` and `0x004164ea..0x004164f0`) are gated
on stubbed targeting predicates, and the mode-2 dot product that zeroes its y term
(`AiStandalone.cpp:940` vs the `.asi`'s `AiControlStep.cpp:336`) sits in a tail that is
unreachable while `mode` is pinned. **One defect, several symptoms.**

## Collateral review

Changed game code this session: **none**. So there is no pre/post pair on the shipping
side to review. The two arms that exist are reviewed instead.

### Same-side floor — measured directly, and it is 0

`r1` / `r2` / `r3` are **byte-identical on every column over their entire common prefix**
(`r1-r2`, `r2-r3`, `r1-r3` all return "first differing row: None"; lengths 4193 / 7975 /
7173 rows). The port is fully deterministic on this recipe; the three runs differ only in
how far they got before the early exits.

**Therefore `collateral.py`'s derived floor on this data is too generous** (it produced
e.g. `c1` 189, `look_z` 6.73 from the same repeat pair), and its "first frame past the
floor" column is a conservative upper bound, not the first divergence. The direct diff is
the authority and is reported instead.

### Paired, same side: `r1` (default) vs `p1` (`MASHED_D2_BATCHMODE=plane`)

`py -3.12 re/tools/statediff/collateral.py --a csv:…/r1.csv --floor-a csv:…/r2.csv
--b csv:…/p1.csv --floor-b csv:…/r3.csv --speed rec_9e4 --mode paired --top 25`
→ `collateral_paired.txt`

**10 of 35 fields diverge, and every one of them is `outside` scope**: `look_z`,
`look_best`, `look_idx`, `march_n`, `march_idx0`, `hist_d8`, `curv`, `rec_b0c`,
`rec_9e4`, `c1`. **25 fields are within the floor on every aligned frame**, including
`ai_mode`, `c0`, `c4`, `c5`, `flag_a368`, `own_x`, `own_z` and `tgt_7ffc`.

The scored window closes at **row 659** (220 calls × 3 interleaved cars). The direct diff
puts the first `r1`-vs-`p1` divergence at **row 1365**, 706 rows later. So **no outside-scope
row intrudes into the measurement.**

### Cross-side, at matched call index (the primary cross-side comparison)

`re/tools/ai_mode_split.py` compares original against port at the **same within-window
call index** — median index **109.5 on both sides** — which is the matched-time
comparison. Its table is in `RESULT_STEP2.md` §G2-STEER. Outside-scope rows it surfaces:
`curv` (2-8x), `rec_9e4` (+33-52 %), both reported there with the position confound
stated.

### Cross-side, speed-banded (reported, and weak — not relied on)

`--mode banded`, `o1.msd.aistep.csv` vs `r1.csv` → `collateral_crossside.txt`. **Only ONE
shared band exists (2000-2600), n = 69 vs 48, and its median frame indices are 934 vs
530.** Those are different moments, so magnitudes across it are not a measurement
(memory `band-on-speed-compares-different-moments`, `a-band-scored-off-regime-is-not-a-measurement`).
It is recorded for completeness and **no conclusion is drawn from it**. The one useful
negative it does give: **24 fields are within the noise floor in every shared band**,
including `ai_mode`, `c0`, `c4`, `c5`, `tgt_7ffc`, `flag_a368` and `rec_b0c`.

## D2 WATCH

No D2 REOPEN CANDIDATE from this step. No D2 code changed.
