# RESULT — KA-SPECT: my prediction was WRONG, my gate was ILL-POSED, and the real blocker is the SCHEDULE

**RAN 2026-10-05.** Pre-registration `PREREG_SPECT.md`, committed **unrun** at `3f68423e`. Anchor
verified before arming (`MASHED.exe.unpatched` `BDCAE093…3C0E`, `launch.exe` `0150…8DA2`).

**No C-level moved. No band moved. No code written. No port started.** Two original-side runs,
`--peek` only, **no Interceptor and no hook anywhere**.

---

## 1. KA-SPECT's registered gate is VOID. It could not have returned LIVE.

**This is the third ill-posed gate I have written today and it is reported as void rather than
re-scored.**

The registered discriminator was the number of distinct 4-tuples across `S_drive`: **LIVE** needed
**>= 10**, **FROZEN** needed **<= 2**. The instrument samples every **3.6 - 4.0 s**
(measured: +0.0, +3.6, +7.6, +11.6, +15.2, +18.8, +22.4, +26.0). Car 1's **entire** driving phase is
**220 frames ≈ 3.7 - 4.0 s**.

So `|S_drive| ≈ 1 sample`. The LIVE clause (`>= 10` distinct tuples drawn from ~1 sample) was
**unsatisfiable by construction**, and the FROZEN clause (`<= 2`) was **trivially satisfiable**. The
gate could only ever return FROZEN, DEAD or INCONCLUSIVE.

**Verdict as registered: INCONCLUSIVE** — 3 distinct tuples run-wide, which is neither `>= 10` nor,
scored over `S_drive`, a meaningful `<= 2`. **No port is licensed by this gate.**

**Why it happened, stated so it is not repeated:** I checked the gate's *denominator* (and said so
explicitly, after today's two retractions) but never checked the instrument's *sampling rate against
the duration of the population*. A denominator can be the right population and still contain one
sample.

## 2. My registered prediction was WRONG, and the substance is the opposite of it

I registered: *"I expect FROZEN or DEAD, because the only call path is a post-race camera. If it is
LIVE, my reading of the call chain is wrong and I will say so."*

**The array is recomputed during the race.** Three distinct tuples, two transitions:

| time | zero slot | `0x008989b0` | `b4` | `b8` | `bc` |
|---|---|---|---|---|---|
| +0.0 .. +15.2 s | **0** | 0.000000 | 1.553424 | 1.833030 | 2.749546 |
| +18.8 s | **1** | 4.107919 | 0.000000 | 3.778889 | 2.097618 |
| +22.4 s .. +58.3 s | **2** | 10.043903 | 2.920188 | 0.000000 | 1.369535 |

- **Exactly one slot is 0.0 in every tuple, and which slot moves: 0 → 1 → 2.** Per the C2 plate the
  stored value is the planar distance from a reference position, so the zero slot *is* the reference
  car. **The reference car changes twice.**
- The game is in **`ph=3` (driving)** on every status line of the run (+4, +9, +14, +19, +24 s).
- The transitions at ~+18.8 s and ~+22.4 s sit at the end of car 1's driving phase: the capture is
  3286 frames over ~60 s (~18 ms/frame), so car 1's window (frames 845..1064) is ≈ **15.2 - 19.2 s**,
  and car 1 is eliminated at frame 1065 ≈ **19.2 s**. The reference-car switches coincide with
  elimination events.

**So my reading of the call chain was wrong, and I say so:** `FUN_00448220` is *named*
`Frontend::PostRaceResultCamera` and `FUN_00446520` is *described* as a race-result camera state
machine, but one of `FUN_00446520`'s three documented branches is **DM-spectator (mode 5)**, and in
elimination mode (`--mode 10`) that branch is exercised **mid-race**, once per elimination. A name
containing "PostRace" is not evidence about when a function runs.

## 3. The premise for porting was already established — by U-9187, not by this leg

KA-SPECT was aimed at a question the record had already answered. **U-9187 measured the consumer
directly**: `Prog[0..3]` non-zero on **464 / 125 / 486 / 461 of 512** original `FUN_004148b0` calls
(max 9.988 / 3.790 / 4.466 / 2.750). Those maxima are consistent in magnitude with the tuples above
(10.04, 4.11, 3.78, 2.92), which is a cross-check between two independent instruments.

So *"the array is live and non-zero where the consumer reads it"* did not need re-testing, and this
run's only unique contribution is **the producer fires mid-race, at elimination events**. The
redundancy is mine to own: I should have read U-9187's numbers as already answering it.

## 4. THE REAL BLOCKER, and it is not the one the plan named

Porting `FUN_00442a60` is cheap and well-supported — 530 bytes, fully C2-plated, and **all seven
depth-1 callees already have bodies** (2x C4, 2x C3, 3x C2). That part of the plan holds.

**But the producer's SCHEDULE is not in the producer.** It has exactly one caller, and the chain is:

```
FUN_0040d470  ->  FUN_00448220              ->  FUN_00446520            ->  FUN_00442a60
                  Frontend::PostRaceResult-     race-result / DM-           the ONLY writer
                  Camera, 1234 B                spectator camera FSM,       of 0x008989b0
                  C2 mapped, NO BODY            7411 B, C2 mapped,          530 B, C2, no body
                                                NO BODY
```

The values depend on **which car is the reference**, and that decision is made by the 7411-byte
state machine, not by the producer. **A ported `FUN_00442a60` would therefore reproduce the
computation and not the schedule: a correct function that nothing calls.** That is precisely the
shape of `BodyOrient_OmegaFromAngVel`, which this same session found sitting with zero call sites.

And **seeding is refused**: `verify/d3_modes37_20261002/RESULT_STEP2.md:201-203` — *"seeding the
globals would not be a port"* — so writing plausible distances into `0x008989b0` is not an option.

**Revised scope for the 64-call branch, measured rather than estimated.** U-9187 named four missing
inputs; the chain is now five items deep:

| # | item | state | note |
|---|---|---|---|
| 1 | `FUN_00442a60` producer | C2, no body; **all callees ported** | cheap, and a hard prerequisite — it is the only writer |
| 2 | `FUN_00446520` camera FSM | C2 `mapped`, **no body**, 7411 B | **the schedule**; decides the reference car. NEW to the scope — not in U-9187's list |
| 3 | `FUN_00448220` `Frontend::PostRaceResultCamera` | C2 `mapped`, **no body**, 1234 B | reaches the FSM. NEW to the scope |
| 4 | exe-side bodies for `0x0040e470`, `0x00442cc0`, `0x0046d4a0` | C3 `impl`, **empty `exe_file`** | the three TUs are in `asi_sources.rsp` only (U-9187) |
| 5 | `idx364` (orig **-1** vs port **0**) and `bias374` (**0** vs **{0,1,2,3}**) | open | decide whether the 64 calls are *reproduced* or merely *reachable* (U-9187) |

**Items 2 and 3 are new** and they are the expensive ones: 8,645 bytes of unported C2 camera code
whose only purpose, for this branch, is to decide when the producer runs and off which car.

## 5. What is NOT concluded

- **No port was started**, because G-PORT was registered as running **only on LIVE** and the gate did
  not return LIVE. Proceeding would mean acting on a gate I have just declared void, and re-posing a
  gate after seeing the data to license work I wanted to do is the exact failure mode to avoid.
- **Nothing about (b) or (e) moved.** No code was written, so the default build is untouched and
  `G-NOREG` had nothing to score.
- **Nothing about U-9191.** Unchanged.
- **The elimination-event reading of the transitions is an INFERENCE from timing**, not a measured
  causal link. Two transitions at ~+18.8 s and ~+22.4 s coincide with the end of car 1's driving
  phase; I did not hook the elimination path to confirm it. Marked as such.

## 6. Instrument defects in my own work this leg

1. **The ill-posed gate** (§1) — sampling rate never checked against the population's duration.
2. **The redundant question** (§3) — U-9187 had already measured the consumer.
3. **A truncated log, mine not the harness's.** `verify/d3_spect_20261005/s2.peek.log` stops at
   +26.0 s because I piped `Tee-Object` into `Select-Object -First 8`, which stopped PowerShell
   consuming the stream. The first run (`s1`, untee'd, not persisted) is what establishes the tuple
   is still frozen at +58.3 s. **The two runs must be read together**, and the two agree on the
   transition band (+18.8/+22.4 vs +21.8) while differing in exact wall-clock, so the runs are not
   frame-identical in time — expected, and no number here depends on it.
4. **A well-posed replacement exists and is cheap**, if this is resumed: `FUN_00442a60` fires roughly
   **2-3 times per minute**, so a single `Interceptor` entry hook on it is unambiguously safe under
   the hot-path rule (which is about >1000 calls/s) and would give exact call counts and frames. That
   is a `--spect-probe` flag on `scenario_launch.py`, the established pattern used by `--axis-probe`
   / `--fixup-probe` / `--wheelstate-probe`, **not** a new one-off harness.

## 7. Artifacts

- `s1.msd` / `s1.msd.aistep.csv` / `s1.msd.provenance.json` — run 1 (3286 frames, 5131 AI calls;
  `tgt148b0 = 512`, which **reproduces U-9187's 512 exactly**).
- `s2.msd` / `s2.msd.aistep.csv` / `s2.peek.log` — run 2, peek series persisted but truncated at
  +26.0 s per §6.3.
