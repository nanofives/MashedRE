# RESULT — `FUN_00418560` Branch A ported. INERT, and the reason is the real finding.

Date 2026-10-08. Follows `RESULT_RAMP.md` §6 option 1, chosen by **USER DECISION (Mariano,
2026-10-08)**: "Find and port the 0x007f0ff8 reset — FUN_00418560's mode-5 path."

**RAN**, 4 runs. Default-OFF behind `MASHED_MODE5_RESET`. No C-level. `original/` untouched, `.asi`
code path untouched. Nothing seeded.

Raw: `M5_GATES.txt`, `M5_RUN.txt`, `M5{off,on,on_r2}.gates.csv`, `M5step.csv`.

## 0. Verdict first

> **The branch is ported faithfully and it is INERT — zero columns differ between arms.** Not
> because the transcription is wrong, but because **the standalone never enters sub-mode 5**:
> `substate` is **`6` on 19,418/19,418 rows**. The branch tests `FUN_0040e350() == 5`, a value the
> port never produces.
>
> **That is the real finding, and it supersedes "the port is missing a reset".** The port is not
> missing a *reset* so much as the **race sub-state machine** that would call for one. The original
> cycles 5 / 4 / 6 / 7 / 0 / 11 / 9 / 10; the port holds **6** for the entire 240 s.

| gate | verdict | figure |
|---|---|---|
| `M5-RAMP` | **NO CHANGE** | both arms: `bias374` `{0,1,2,3,4}`, **0 reversals**, identical histogram (`4`×9834, `2`×1201, `3`×1201, `0`×661, `1`×601) |
| `M5-INERT` | **INERT** | **zero** differing columns between OFF and ON |
| `M5-KNOBOFF` | **PASS** | `M5step.csv` hashes `86b7b2bb`, the **seventh** build to match |
| `M5-DET` | **PASS** | `M5on` / `M5on_r2` byte-identical (`949e309b`) |

## 1. What was ported, and it is faithful

`VehicleStep` — the exe's `FUN_00418560` analogue — went straight from zeroing the ctrl bytes to
`BankSwitch`, **skipping Branch A entirely**. Now transcribed from
`re/analysis/ai_update/0x00418560.md:28-34`, every line RVA-cited:

```
FUN_0040e350()  == 5        0x0041858e / 0x00418593
  DAT_007f0ff8 = 0          0x00418598
  FUN_00413fe0()            0x0041859e
  elapsed = FUN_0040e4a0()  0x004185a3
  cd = [0x0089a4f8 + v*0x74] 0x004185ab
  cd < 0  : elapsed < (0x4a - cd)*100 -> ctrl[4] = 0xff   0x004185cb..0x004185e4
  else    : elapsed < (cd + 0x40)*100 -> ctrl[4] = 0xff   0x004185b5..0x004185ca
  return                    (mode 5 skips the whole step)
```

**A fidelity split was required and it is an improvement.** `Ai_ResetRace()` had
`FUN_00413fe0`'s body **bundled with** the race-clock zeroing from a different site
(`0x0040ff17..0x0040ff23`). Calling it from the mode-5 path would also have zeroed `0x007f0ff4`,
which `FUN_00418560` does **not** do — and that would break the `FUN_00416250` steer timer, which
measures `el = DAT_007f0ff4 - start` against 200. `Ai_ResetVehicleStates()` is now `FUN_00413fe0`
proper; `Ai_ResetRace` calls it plus the clock zeroing.

**Registered deviation.** `FUN_0040e4a0` reads `0x005f29b8`, image `.data` the standalone never
loads, so `elapsed` reads `0` and the countdown compare is always true — the accel hold would be
permanent for mode 5's duration rather than releasing part-way. It does not touch the `0x007f0ff8`
zeroing and mode 5 is transient. Recorded in the source, not left to be discovered.

## 2. Why it is inert — measured, not argued

`M5step.csv`, the port's own stepdump, 19,418 rows:

| column | value |
|---|---|
| `substate` | **`6` on 19,418/19,418** |
| `ai_mode` | `0` on 19,418/19,418 |
| `step_1008` | `50` on 19,418/19,418 |

The original, across two captures (`o_ramp`, `o_ramp60`), visits `substate` 6 / 5 / 7 / 0 / 11 / 9 /
10 / 2 / 3 / 4 — and **all 29 of its `tick_0ff8` resets occurred at substate 5 or 4**. The port
reaches neither.

So the chain is: the standalone never leaves racing sub-state 6 → `FUN_0040e350()` never returns 5
→ Branch A never runs → `0x007f0ff8` is never re-zeroed → `bias374` ramps monotonically to band 4.

## 3. What this changes about `bias374`

**`bias374`'s divergence is not a defect in the ladder, the reset transcription, or
`FUN_004177b0`.** `RESULT_RAMP.md` §1 already showed both sides fire band 1 at the same ~11.0 s
threshold. This leg shows the reset path is now present and correct. What remains is that the port
has **no race sub-state transitions at all** under the deterministic harness — one continuous
240 s racing state, by construction of `MASHED_DET_FRAMES`.

**Therefore `bias374` should not be treated as a GATEFIRE blocker.** It is downstream of a much
larger, already-known gap (the standalone's race-state machine), and the practical question for
branch 2 is narrower — see §5.

## 4. What is NOT claimed

- **No C-level.** The branch is byte-faithful to the plate but has never executed, so there is no
  behavioural evidence for it. It cannot be promoted on a transcription alone.
- That the port *should* enter sub-mode 5. Whether the standalone ought to model the countdown /
  round-restart states is a scope question this leg does not answer.
- That `FUN_0040e4a0`'s zero reading is harmless in general — only that it is harmless to the
  `0x007f0ff8` zeroing.
- Anything new about branch 2 firing. Unmeasured, as throughout.

## 5. Next

1. **The cheap test `RESULT_RAMP.md` §6 option 2 already identified.** `bias374` feeds only the
   limit-table index (`bias374 + iVar1*5`). The original is pinned at `0` with `iVar1 = 2` → index
   **10**, the only entry ever used (value `1`, `RESULT_WITNESS.md:106`). So branch 2 may need
   nothing more than `bias374 == 0`, which a knob could hold directly for a measurement **without**
   modelling the state machine. That is one run, and it would tell us whether this lane is clear.
2. The race sub-state machine is a far larger scope item and should be its own `DEFERRED.md` row,
   not a GATEFIRE leg.
3. `MASHED_MODE5_RESET` stays default-OFF. It is correct, inert, and costs nothing where it sits.
