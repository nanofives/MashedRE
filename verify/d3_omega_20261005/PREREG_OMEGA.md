# PREREG — U-9193: read the ORIGINAL's AI-car angular velocity at a KNOWN program point

**Status when committed: UNRUN.** No code has been changed, no game has been launched, no number
computed. Results go in `RESULT_OMEGA.md` here.

Parents: **U-9193** (is `+0x9c0` the yaw rate, or is the `.msd` sampling a zeroed phase?) and
**U-9191** (is car 1's confirmed 0.9866 deg heading residual GENERATED or ACCUMULATED?). Gate: **D3
criterion (b)**.

---

## 1. Why an entry hook, and what already exists

Leg 3's KA-1 failed because `+0x9c0` read from a **per-render-frame `.msd` snapshot** is not a
per-frame yaw rate: correlation **0.068251**, non-zero on only **317 of 3623** frames, support
matching turning exactly but the ratio scattered over six orders of magnitude and sign-inverted.
U-9193 records two candidate explanations and deliberately does not choose: either `+0x9c0` is not
`omega.y`, or the snapshot catches it at a phase where it is zeroed or part-accumulated. **A known
program point settles that**, and nothing else will.

**`--axis-probe` already exists** (`scenario_launch.py:997-1059`) and is closer to fit than the
hand-off recorded. Three corrections to that record, found by reading it:

- **It is NOT player-only.** `axisProbeArm(recBaseHex, car)` takes `--statediff-car`, so
  `--statediff-car 1` already targets AI car 1. The inventory this session relied on said these
  probes "target the PLAYER"; that was true of their usage, not their capability. **Second inventory
  error this session** (the first was `o_t3` reported missing when it exists).
- **It does not read the angular velocity.** The row is `+0x9d4/d8/dc`, `+0xb14/18/1c`, `+0x9e4`,
  `+0x9e0`, `+0xbf8`, `+0x1f0` and four wheel axis-Ys. **No `+0x9bc`/`+0x9c0`/`+0x9c4`, and no
  position.** The `--axis-probe` help text mentions "angular velocity" but the row does not carry it.
- **Its A6b hook has NO ESI filter** (`:1050-1053` calls `axSample(1)` unconditionally while A6a at
  `:1046` filters). With one car that is harmless; sampling an AI slot while four cars are live means
  **every car's A6b call records the target car's record**. That is a latent misattribution and it is
  the first thing this leg has to make visible rather than inherit.

## 2. What is added — additive, read-only, no new harness

Per the project rule against one-off harnesses, `--axis-probe` is **extended**, not replaced.
Appended to the existing row so every current consumer of `.axisprobe.csv` keeps its columns:

| added | offset | why |
|---|---|---|
| `wx`, `wy`, `wz` | `+0x9bc`, `+0x9c0`, `+0x9c4` | the quantity U-9193 is about, now at a known phase |
| `px`, `pz` | `+0x958`, `+0x960` | the original's record position, for any later cross-side pairing |
| `esi` | — | `this.context.esi` **at both sites**, so car attribution is **verifiable instead of assumed** |

`esi` is the fix for the A6b gap. Rather than guessing whether ESI holds the record pointer at A6b —
which is **not established** and which I will not assume — the raw value is recorded and the analysis
decides. Reads only. No `onLeave`. No mid-function probe (memory
`frida-interceptor-is-entry-only`).

**Hot-path budget.** A6a runs once per car per frame; with 4 cars at ~60 fps that is ~240 calls/s per
site, ~480/s across both — under the ~1000/s destabilisation line measured for this game, but not far
under. The run is kept short and the sample count is reported so the budget is auditable.

**Binary anchor is verified before the probe is armed.** RVAs are invalid if the image differs.

## 3. Known-answer checks — all three must pass before any verdict

- **KA-A, already built in:** `DAT_00614708` must read `(0, 0, 1)`. Proves the probe reads the image
  at the right VA before any float read carries a verdict.
- **KA-B, new, and it is the A6b gap made explicit:** on A6b-POST rows, `esi` must equal one of the
  four record pointers `0x008815a0 + n*0xd04` on **≥ 95 %** of rows. PASS means ESI holds the record
  at A6b and POST rows can be attributed per car. **FAIL means POST rows are unattributable** — they
  are then reported as such and **only A6a-PRE is used**. Either way the existing probe's silent
  assumption stops being silent.
- **KA-C, coverage, armed before the first run** (memories
  `arm-coverage-counters-before-first-run`, `absent-log-proves-nothing-run-a-control`): A6a-PRE rows
  for the target slot **> 0**, and **≥ 30** of them in the active-drive regime (`+0xbf8 == 0` and
  `speed > 1.0`). Below that the leg is **NO-VERDICT**, not a finding. The CSV is written even when
  empty.

## 4. Gates

| gate | PASS | FAIL |
|---|---|---|
| **G-OMEGA** — which of U-9193's two candidates | `+0x9c0` non-zero on **≥ 50 %** of A6a-PRE active-drive rows → the `.msd`'s 317/3623 (**8.75 %**) was a **ZEROED/PART-ACCUMULATED PHASE**, and U-9193 resolves that way | non-zero on **≤ 15 %** (i.e. consistent with the snapshot's 8.75 %) → the phase explanation is **refuted** and **`+0x9c0` is not `omega.y`**; U-9193 resolves the other way and the real yaw-rate field is then unlocated |
| **G-RATEID** — is it the yaw rate at a known phase | single-scale fit `wrap180(Δ atan2(+0x9dc, +0x9d4)) = k · (+0x9c0)` across consecutive A6a-PRE rows of the same car reaches Pearson **≥ 0.95** | below → `+0x9c0` is **not** the yaw rate even at a known phase, and U-9191's generated-vs-accumulated question needs a **different field**, which this leg will not have found |
| **G-ZERO** — does U-9175 extend | `+0x9bc` and `+0x9c4` are **exactly 0.0** on every A6a-PRE row, as they were on all 3623 `.msd` frames | any non-zero → the "no pitch/roll torque" reading is phase-dependent, which is itself a finding and must be reported with its count |

Between 15 % and 50 % on G-OMEGA the verdict is **INCONCLUSIVE**, reported with the fraction and n,
and U-9193 stays open. Registered now so a middling fraction cannot be read either way.

**Registered prediction: G-OMEGA PASSES (the `.msd` was a zeroed phase), and G-RATEID PASSES.**
Reasoning: the `.msd`'s support matched turning **exactly** — on 0 of 289 turning frame-pairs was
`+0x9c0` zero — which is very hard to get from a field that is not the yaw rate, and much easier to
get from the right field sampled after a per-frame zeroing. Offered with **low** confidence: two of my
predictions were wrong earlier today, and G-OMEGA's FAIL branch is written to be equally publishable.

## 5. The cross-side leg is CONDITIONAL and may not be reachable

If and only if G-RATEID passes, compare the port's `rec_9c0` (already in its dump since leg 3)
against the original's at matched position, to answer U-9191's **GENERATED vs ACCUMULATED**.

**Registered limitation, up front:** the port writes `+0x958`/`+0x960` as identically `0.0`, so that
pairing must still use the port's `own_x`/`own_z` against the original's `+0x958`/`+0x960` **read at
A6a entry** — and the phase relation between the original's A6a entry and the port's AI call is **not
established**. So this leg carries its own pairing-validity measurement **in yaw-rate units** (the
original's own A6a-to-A6a step change in `+0x9c0`, needing a 10x margin), and is **VOID** without it.
**I am registering that this step may well not be reachable in this session, and that reporting it as
unreachable is the correct outcome rather than forcing a number.**

## 6. What a result here does and does not license

- **No C-level moves.** This adds probe columns and an analysis pass; nothing is reimplemented at an
  RVA, so no `diff-original` leg is available or owed.
- **A D2 WATCH row becomes owed only if the cross-side leg runs AND shows a rate divergence.** The
  angular velocity is a D2 surface, but G-OMEGA and G-RATEID on their own are field identification on
  the **original**, which implicates no port code.
- **It does not close criterion (b).** The 2026-10-02 counterfactual matrix had **no arm passing (b)
  on any car**.
- **The existing `--axis-probe` results stay valid.** Columns are appended, the ESI filter at A6a is
  untouched, and no existing read is changed — so U-9175's committed conclusions are unaffected.
