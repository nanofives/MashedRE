# STEP 2B PRE-REGISTRATION — amendment after G2-KA FAILED. **UNRUN.**

## READ THIS FIRST — A PRE-REGISTERED GATE FAILED AND THE REGISTERED ANALYSIS DID NOT RUN

`PREREG_STEP2.md` §2.0 registered: *"KA-2.0: the reconstruction must reproduce the
ORIGINAL's own logged `c4` and `c5` on ≥ 95 % of its 220 window calls, each of cars 1..3 …
If it fails on the ORIGINAL, the model is not a model of the original, every rule-occupancy
claim below is void, and I STOP and report that."*

**It failed.** `re/tools/ai_speed_budget.py` on `o_e1` vs `ea1`:

| car | ORIGINAL | PORT |
|---|---|---|
| 1 | **140/220 = 0.636 FAILED** | 220/220 = 1.000 PASS |
| 2 | **206/220 = 0.936 FAILED** | 220/220 = 1.000 PASS |
| 3 | **165/220 = 0.750 FAILED** | 220/220 = 1.000 PASS |

The tool stopped at the gate and printed no occupancy table. **§2.1, §2.2, §2.3, §2.4 and
§2.5 of `PREREG_STEP2.md` DID NOT RUN and are not reported anywhere.** This file replaces
them. The old thresholds `DN_THRESHOLD = 0.5`, `OCC_THRESHOLD = 0.10`, `CF_THRESHOLD = 0.10`
are **withdrawn, not reused**.

## Why it failed — stated as a finding, with its numbers, before the replacement

The model in `ai_band_sim.simulate` / `ai_speed_budget.rules_for` transcribes the **mode-0
arm only** of `FUN_00416250`'s accel/brake tail. The port pins `mode = 0`
(`AiStandalone.cpp:844`, *"targeting chain FUN_00414570.. STUBBED"*), so for the port the
model is exact by construction — 220/220 on all three cars. The ORIGINAL does not stay in
mode 0:

| car | original window `ai_mode` histogram | mismatching calls | their `ai_mode` |
|---|---|---:|---|
| 1 | `{0: 124, 7: 80, 3: 16}` | 80 | **`{7: 80}` — 100 % at mode 7** |
| 2 | `{0: 171, 3: 49}` | 14 | **`{3: 14}` — 100 % at mode 3** |
| 3 | `{0: 196, 3: 24}` | 55 | `{0: 43, 3: 12}` |

PORT: `{0: 220}` on all three cars.

`ai_mode` is the dword at `0x0089a4cc + v*0x74 + 0x60` = **`0x0089a52c + v*0x74`**, which is
exactly the address `FUN_00416250`'s mode tail reads (`AiStandalone.cpp:922`,
`I32(a74(0x0089a52cu, v))`) and that `0x00416590` writes. So the logged column IS the tail's
input, not a proxy.

On car 1, **49 of the 80 mismatches are the model predicting `(c4, c5) = (255, 0)` where the
original logs `(64, 0)`** — `64 = 0x40`, the literal signature of the `m == 7` tail at
`AiStandalone.cpp:924` (`ctrl[4] = 0x40`). The tail is already written in the port; what is
missing is the mode **value**.

**So the gate did its job: it refused to let an incomplete model produce an occupancy table,
and the reason it refused is itself the first candidate carrier.**

## 2B — the replacement analysis

Entirely offline, on `verify/d3_elim_20261003/o_e1.msd.aistep.csv` (ORIGINAL, with `o_e2` as
the determinism repeat) and `ea1.csv` (port arm A, boost ON, with `ea2`/`ea3`). No new
capture, no build.

### Threshold honesty, declared

While diagnosing the KA failure I inspected the speed-vs-call-index curve at 14 coarse
indices per car. **The onset threshold below was therefore chosen with that table already
seen, and this file says so rather than claiming blindness.** The answer is made
threshold-independent instead: the onset call is reported at **three** thresholds
(0.5 % / 25 units, 1 % / 50 units, 2 % / 100 units) and **a carrier is named only if all
three give the same classification in §2B.2.** If they disagree the onset is reported as a
range and no carrier is named.

### 2B.1 — The divergence onset, at matched call index

Per car: the first window call `k` with
`|speed_port(k) − speed_orig(k)| > max(P * speed_orig(k), U)`
for `(P, U)` in `{(0.005, 25), (0.01, 50), (0.02, 100)}`, requiring `speed_orig(k) > 100` so
the ratio is defined. Report `k`, both speeds, both `c4`, both `c5`, both `ai_mode`, for
`k-2 .. k+2`.

### 2B.2 — Classify the onset. Registered now:

- **COMMAND** iff `c4_orig != c4_port` **or** `c5_orig != c5_port` at the onset call or in
  the two calls before it.
- **NON-COMMAND** iff the commands agree over `k-2 .. k` and the original's speed is
  *falling* (`speed_orig(k) < speed_orig(k-2)`).
- **AMBIGUOUS** otherwise, and reported as ambiguous.

Per car, reported separately. **No "all three cars" conjunct is used to suppress a per-car
finding, and no per-car finding is generalised to the others.**

### 2B.3 — The pre-onset budget, and what it settles

Report `median |Δspeed|` and `max |Δspeed|` over window calls `[0, k)`, per car, in absolute
units and as a fraction of the original's speed.

**Registered reading, written before the numbers:** if the pre-onset `max |Δspeed|` is under
**1 %** of the original's speed over that span, then across those calls the port's **drive
force (`+0xb14`/`+0xb1c`, A4 `0x00470670`), A5 drag (`0x0046ddb0`), A6a's clamps including
grip-clamp #6 (`0x00467650`), the contact solver (`0x0046f6c0`) and the gear/rev channel
(`+0xb0c`, `0x00470724`/`0x0047072c`) jointly reproduce the original to under 1 %**, and
none of them is the carrier of the over-speed over that span. That is a **stronger** statement
than `PREREG_STEP2.md` §2.5's per-term budget would have produced, and it is obtained with
**no new instrumentation on either side** — so §2.5 is **superseded, not skipped**, and does
not run unless §2B.2 returns NON-COMMAND or AMBIGUOUS on a car.

If the pre-onset `max |Δspeed|` is **above** 1 %, §2.5 runs as originally registered.

### 2B.4 — For a COMMAND onset: name the rule

On the original's window, count the calls where `ai_mode == 7` **and** the logged `c4 == 64`
— the direct, unambiguous signature of the unported `m == 7` tail — and the calls where
`ai_mode == 3` and the logged `(c4, c5)` differs from the mode-0 model's prediction. Report
both as fractions of the window.

### 2B.5 — Live confirmation on the running ORIGINAL (STEP 3's first leg)

Offline agreement between two committed captures is **not** a live confirmation. Before
anything is coded, the relation *"`ai_mode == 7` at `0x0089a52c + v*0x14`… `+ v*0x74` ⇒ the
byte written to `ctrl[4]` is `0x40`"* is confirmed on the **running original** with an entry
hook that reads the mode global and the ctrl block in the same call. Registered gate:
**≥ 99 % of live mode-7 calls must show `ctrl[4] == 0x40`**, on a run with ≥ 50 such calls.
Fewer than 50 and the leg is reported as under-powered, not as a pass.

## Gates for 2B

- **G2B-SENS** — the three thresholds must agree on the §2B.2 classification per car.
- **G2B-DET** — `o_e1` vs `o_e2` must give the same onset classification; `ea1`/`ea2`/`ea3`
  likewise. Already shown digit-identical on both scorers, but the onset is re-checked.
- **G2B-LIVE** — §2B.5's ≥ 99 % on ≥ 50 calls.
- **G2B-NOFIT** — no constant is fitted; no free parameter is introduced.
- **G2B-SCOPE** — a per-car NON-COMMAND or AMBIGUOUS verdict is reported as such and is
  **not** absorbed into a COMMAND verdict found on another car.

## What this step explicitly does NOT claim

- It does **not** claim that porting the mode producer would close (b). The 2026-10-02
  modes-3/7 arms refuted that for the **steer bands**, and that refutation stands untouched.
  This step is about **speed**, which those arms did not measure.
- It does **not** cost or schedule the mode-producer port. STEP 3 decides that, and if the
  faithful fix turns out to be larger than one RVA it is reported as **named but not landed**,
  with the callee set and its C-levels, rather than half-landed.

## D2 WATCH (D-11071)

If §2B.2 returns NON-COMMAND on any car, the carrier is a shared vehicle-physics output that
also drives the player and that car is a **D2 REOPEN CANDIDATE**, re-scored against D2's solo
arm (3 runs, unchanged `d81a8df6` bounds) before any fix is proposed.
