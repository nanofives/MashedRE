# RESULT — `W-SHAPE` port side: the instrument works, the QUANTITY is dead. `WS-LIVE` **FAILS**, and it fails on the original too.

Date 2026-10-08. Pre-registration: [`PREREG_WSHAPE.md`](PREREG_WSHAPE.md), written before the new
build was run. **No C-level.** `original/` untouched, nothing default-ON.

Raw: `WS_ctl.step.csv`, `WS_fire.step.csv`, `WS_{ctl,fire}.log`. Driver `run_wshape.ps1`.

## 0. Verdict first

> **`W-SHAPE`'s port side is NOT closed, and cannot be closed this way.** `ctrl[4]`/`ctrl[5]` at
> ControlStep entry are **0 on 42,654 of 42,654 port rows** — and, checked here for the first time,
> **0 on 5,318 of 5,318 original `o_t3` rows**. Both sides zero `ctrl` before every call, so the
> entry pose carries **no information on either side**.
>
> **The probe is sound; the quantity is degenerate.** `c4_in` disagrees with the end-of-frame `c4`
> on **99.2%** of port rows, which is exactly what proves it samples before the write. A broken
> probe reading the post-write value would have agreed everywhere.
>
> **This corrects my own `RESULT_FIRESHAPE.md` §4** (corrected inline there).

## 1. Gate verdicts, as registered

| gate | verdict | evidence |
|---|---|---|
| `WS-KNOBOFF` | **PASS** | `WS_ctl` is **identical to `GF0step.csv` over all 19,418 shared keys** (`R-PREFIX = 13498`, the whole overlap). `--common-cols` dropped exactly **4** columns: `b2_ret`, `b2_los`, `c4_in`, `c5_in`. The entry-snapshot edit changed no computed value. |
| `WS-ARMED` | **PASS** | 18 rows with `b2_ret == 1`, v2, offsets **643-660** — reproducing `CW_port.step.csv` exactly (same car, same count, same offsets). |
| `WS-LIVE` | **FAIL** | `c4_in` and `c5_in` are **uniformly 0** across all 42,654 rows (distinct = 1 each, per car as well). The registered PASS required not-uniformly-`-1` **and** not-uniformly-`0`. |
| `WS-C4` | **PASS, but VACUOUS** | `c4_in == c4` on 18/18 firing calls. Since `c4_in` is 0 by construction on *every* call, this cannot discriminate the two readings it was written to separate. Reported as the registered threshold's verdict; the blindness is the finding. |
| `WS-C5OUT` | **PASS** | `c5 == 255` on 18/18 firing calls. |
| `WS-C5IN` | reported | `c5_in = 0` on all 18 firings. The port capture is **as blind as the original's** — it never fires on a call that entered with the brake applied, because no call ever enters with the brake applied. |

`WS-LIVE` is the gate that did its job. It was registered precisely so a constant-zero read could
not be reported as a clean `WS-C4` PASS (memory `scratch-field-false-green`,
`all-zero-reads-prove-nothing-alone`). Without it this leg would have reported "W-SHAPE closed,
entry equals exit on every firing call" — true, and meaningless.

## 2. Why the entry pose is zero — and that it is symmetric

`AiStandalone.cpp`'s branch comment already predicted the port side:

> `ctrl[4]` is NOT written. It keeps its entry value (**VehicleStep zeroes it**), which is why c4
> logs as 0 — the branch never touches it.

What was **not** known is that the original behaves the same way. `o_t3` carries `c4_in`/`c5_in`
and I had only ever read them on the 64 firing calls. Over the whole capture:

| | rows | `c4_in` distinct | `c5_in` distinct |
|---|---:|---:|---:|
| ORIGINAL `o_t3` | 5,318 | **1** (all 0) | **1** (all 0) |
| PORT `WS_fire` | 42,654 | **1** (all 0) | **1** (all 0) |

So the entry-value column is dead on both sides. It is kept in the schema and marked dead in
source, the same treatment the `*_abs` family got in `RESULT_GF0.md`.

## 3. What this leaves the `ctrl[4]` claim standing on

The claim — branch 2 does not write `ctrl[4]` — is **established statically and only statically**:
the branch is three stores and a return, transcribed verbatim from `ctrlstep_decomp.txt:115-122`,

```
param_3[5] = 0xff;  *param_3 = 0;  param_3[1] = 0;  return;
```

with no store to `param_3[4]`. That is a second static witness, not behavioural evidence, and per
the project's own rule it cannot move a C-level. **No dynamic witness for it exists or can be built
from the entry-value route**, because the quantity it would read is constant on both sides.

A dynamic witness would need `ctrl[4]` to be non-zero at entry on a call where the branch fires.
That requires suppressing the pre-step zeroing, which changes the thing being measured — so I am
not proposing it.

## 4. One asymmetry worth recording, not a claim

Because `c4_in` is always 0, `c4_in == c4` reduces to "`c4 == 0`":

| | `c4 == 0` |
|---|---|
| ORIGINAL `o_t3` | 4,771 / 5,318 = **89.7%** |
| PORT `WS_fire` | 337 / 42,654 = **0.8%** |

The original holds the accel byte at 0 on the large majority of logged calls; the port almost never
does. **This is NOT presented as a defect measurement** — the two captures are different scenarios
(QuickRace vs `MASHED_NO_ELIM`), different lengths, and the port's missing phase-3 hold alone
accounts for part of it (`RESULT_CALLWISE2.md` §2). It is recorded because it is large and because
it touches the known AI over-speed lane, not because this leg measured it properly.

## 5. What is NOT claimed

- **No C-level.** Instrumentation plus offline scoring is not a `diff-original` Frida diff.
- **Not that branch 2 is wrong**, and not that it is right. Its output pose still agrees 82/82
  (`RESULT_FIRESHAPE.md` §3) and its timing and car distribution still disagree, both downstream
  of `D-11073`.
- Not that §4's 89.7% vs 0.8% is a measured defect — §4 names its own confounds.
- Nothing moved default. `MASHED_WIRE_B2`, `MASHED_SLOT_PLAYER`, `MASHED_SLOTSTATE_SEED` all stay
  OFF.

## 6. Schema generation note

The stepdump is now **84 columns**. `86b7b2bb` is the SHA-256 of the **80-column** generation and a
byte hash against it is no longer available — that break happened earlier this session when
`b2_ret`/`b2_los` took the schema to 82 (`CW_port.step.csv`, `f1daa410`). Knob-off controls are
checked with `det_prefix.py --common-cols` from here, as `WS-KNOBOFF` did. Current knob-off
capture: `WS_ctl.step.csv`, sha8 **`7ced2aa3`**, 84 columns, 19,418 rows.
