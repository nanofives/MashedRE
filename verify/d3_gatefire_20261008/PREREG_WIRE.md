# PRE-REGISTRATION (UNRUN) — wiring branch 2 to `ctrl`

Date 2026-10-08. **UNRUN at commit time.** Authorized by **USER DECISION (Mariano, 2026-10-08)**.

This is the leg `PREREG_GATEFIRE.md` §7 listed as a **non-goal** of that pre-registration
("Wiring `FUN_00416250`'s branches to `ctrl`"), and whose decision rule said it "becomes the
**next** registered leg, not this one". This is that registration.

Default-OFF behind `MASHED_WIRE_B2`. No C-level. `original/` untouched. `.asi` untouched.

## 0. Honest statement of the entry condition

`PREREG_GATEFIRE.md` §6 gated this leg on "`*-FIRE` **approaching the target**". What was measured
is `GF1-FIRE = 201` against an original figure of **64**, and **the two are not comparable**: the
original's 64 is over a 220-call window per car in a different scenario; the 201 is over a
13,858-frame capture, is **one car** in **one contiguous episode**, and was taken with a probe that
**mutates** the state it reads (`RESULT_SP.md` §3). So the entry condition is met in the weak sense
("it fires at all"), **not** in the strong sense the wording implies. Recorded here rather than
glossed, because it bounds every conclusion below.

## 1. What ships — the exact arm, verbatim from the decompilation

`decomp_pc.py 0x00416250` (read-only pool clone), `ctrlstep_decomp.txt:114-127`:

```c
if ((local_34 == 6) && (iVar5 = FUN_00443080(), iVar5 == 0)) {
    if (local_48 == 0) {                                     // mode == 0
      iVar5 = FUN_004148b0(param_1,&local_24,&local_2c,param_2);
      if ((iVar5 != 0) && (iVar5 = FUN_00416060(&local_1c,&local_2c), iVar5 != 0)) {
        param_3[5] = 0xff;     // ctrl[5] = 0xff
        *param_3  = 0;         // ctrl[0] = 0
        param_3[1] = 0;        // ctrl[1] = 0
        return;                // BEFORE the mode commit and the steer bands
      }
      iVar5 = FUN_00415020(param_2);                         // still stubbed -> no mode 5
      if (iVar5 != 0) { local_48 = 5; }
    }
    if ((&DAT_0088fc88)[param_2 * 0x2d] == 0) { ... }        // the held-powerup block
```

**Three details that a reconstruction-from-memory would have got wrong**, all now pinned:

1. **`ctrl[4] is NOT touched.** It keeps its entry value (zeroed in `VehicleStep`). The logged
   `(c4, c5) = (0, 255)` in `RESULT_STEP2.md:108` is c4 retaining `0`, **not** the branch writing it.
2. **The LOS endpoints are `(&local_1c, &local_2c)`** — `local_1c` is the **own** position
   (`:49` `local_1c = *(undefined4 *)(local_30 + 0x30)`, the same pair `FUN_00443440` takes as the
   spline-curvature position), and `local_2c` is the XZ **`FUN_004148b0` just wrote**. The existing
   `GF1` probe already used own → leader, so it matches.
3. **The return precedes the mode commit at `0x00416590` and the steer-history stores** at
   `0x004165cc` / `0x0041670c`. Those history globals keep their previous values on a firing frame
   — a side effect of *not* running, which a naive "set ctrl and fall through" would destroy.

**Placement in the port:** inside `ControlStep`'s existing
`if (gameMode == 6 && s_host.ai_target_enable() == 0)` block (`AiStandalone.cpp:845`), **before**
the `held_powerup` branch at `:847`, matching the original's order.

## 2. Gates

Scenario and arms as `run_gf0.ps1`: `MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=14400`, Training,
rule 4. **`MASHED_GF1` must be OFF in every scoring arm** — it mutates `TimerAt`/`RankAt`
(`RESULT_SP.md` §3). Knob stack for the ON arm: `MASHED_SLOTSTATE_SEED` + `MASHED_SLOT_PLAYER`
(branch 2 cannot fire without them) + `MASHED_WIRE_B2`.

| gate | threshold |
|---|---|
| `W-TOOK` | the wired arm's `ctrl` differs from the OFF arm on ≥1 frame. **A knob that changes nothing is a failed wiring, not a pass** (memory `verify-the-harness-knob-actually-took`) |
| `W-SHAPE` | on every frame where the branch fires: `ctrl[5] == 0xff`, `ctrl[0] == 0`, `ctrl[1] == 0`, and **`ctrl[4]` equals its entry value**. Any frame violating this is a transcription defect |
| `W-NOREG-E` (**can fail, and failing blocks the leg**) | criterion (e), 6 digits vs the committed baseline |
| `W-NOREG-B` (**can fail, and failing blocks the leg**) | criterion (b), the same 5 bands on `v2`, `v1`/`v3` still PASS |
| `W-KNOBOFF` | knob-off stepdump hashes **`86b7b2bb`** |
| `W-DET` | 3 repeats identical within the ON arm |

**`W-NOREG-E`/`-B` are the point of this leg.** Every prior leg was inert and could not move them;
this one can. If they regress, the wiring does not ship regardless of how faithful it is.

## 3. Registered hazards

- **`ctrl[4]` must be left alone** (§1.1). Writing `ctrl[4] = 0` explicitly would coincidentally
  match the logged `(0,255)` while being wrong on any frame where the entry value differs.
- **The early `return` is load-bearing** (§1.3): it skips the mode commit and the steer-history
  stores. Implementing the branch as "write ctrl then continue" changes state the original leaves
  untouched.
- **`FUN_00415020` stays stubbed**, so the `local_48 = 5` path below the branch remains
  unreachable. This leg does not change that.
- **The firing population is one car in one episode** (§0). A (b)/(e) result that barely moves may
  reflect that, not the wiring's harmlessness.
- `LeaderTimer` **writes** `TimerAt`/`RankAt`. Wired, those writes become part of the default-ON
  behaviour if this ever ships — legitimately so (the original does them), but they are state the
  port did not previously mutate.

## 4. Decision rules

- `W-TOOK` fails → the wiring is inert; report and stop, do not widen scope to force an effect.
- `W-SHAPE` fails on any frame → transcription defect; fix before reading (b)/(e).
- `W-NOREG-E` or `-B` regress → **the leg does not ship.** Report the moved digits/bands by name;
  do not re-cut a denominator or re-baseline.
- (b)/(e) unchanged **and** `W-TOOK` passes → the wiring is faithful and behaviourally neutral in
  this scenario; that is a result, not a disappointment, and default-ON remains a separate decision.
- Any improvement in (b)'s `v2` bands → report it, but **do not claim U-9186 is fixed** on one
  scenario with one firing episode.

## 5. Non-goals

Default-ON. Branch 1 (`FUN_00414a70`, still unported). `FUN_00415020` / mode 5. Any C-level. Any
edit to `original/` or the `.asi`. Modelling the race sub-state machine (`RESULT_M5.md`). Touching
the `bias374` ramp or site 99.
