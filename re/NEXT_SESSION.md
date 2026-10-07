# Next session kickoff

> ## UPDATE 2026-10-08 (U-9186 leg H3 stepdump arm RAN, commit `e06c9451`): **the three gates H3 left unrun all PASS — the default build is bit-identical across three builds (one SHA-256 for all four captures). `H3-GATEFIRE` is WITHDRAWN: it is a PORT leg, not instrumentation, and the two branches are NOT equally blocked. NO C-level, nothing default-ON, `original/` untouched, `.asi` untouched.**
>
> Read [`RESULT_H3_STEP.md`](../verify/d3_u9186_20261008/RESULT_H3_STEP.md), then
> [`AMEND_H3.md`](../verify/d3_u9186_20261008/AMEND_H3.md) §3.1 for the census.
>
> - `H3-KNOBOFF` **PASS** and **not** the near-tautology H1a's was (`H1-CALLERS = 0`): H3's call
>   site runs 14,400 times in the default build and still changes nothing. `E2off_1`, `H1step`,
>   `H3step`, `H3step_r2` all hash to `86b7b2bb`. `--common-cols` dropped no column (no schema
>   drift). `H3-NOREG-E` 3/3. `H3-DET` PASS. `G-BANDS-UNEDITED` empty before and after.
> - `H3-NOREG-B` **PASS as UNCHANGED**, which is not "(b) passes": `v2` still fails the same five
>   bands with the same numbers (`c0_distinct=39`, `c1_distinct=75`, `steer_distinct=113`,
>   `c1_median=5.5`, `abs_steer_median=44.5`). That failure is the point of U-9186.
> - **`H3-GATEFIRE` withdrawn.** `ControlStep` calls none of the three predicates —
>   `AiStandalone.cpp:844` is a hardcoded `mode = 0` with the chain elided at source level, not
>   stubs returning 0. `FUN_00414a70` has **no body anywhere** (`hooks.csv:645`); `FUN_004148b0` /
>   `FUN_00416060` are `.asi`-only and RVA-saturated.
>
> ### START HERE — the branch-scope question is a USER call
>
> 1. **The 64-branch is the cheap half and the registration did not know it.** Its LOS input is
>    already live: `TrackRenderer.cpp:359` loads the `.AI` tile grid to `0x007f1a9c` and
>    `AiStandalone.cpp:282-288` already implements `FUN_00416060`'s exact tile test. What is left is
>    `FUN_004148b0`'s substrate — `0x0089a4c4` / `0x00442cc0` / `0x0040e470` have exe-side
>    references, **`0x0089a4c8` and `0x005f2dd8` have none**. `0x005f2dd8` is the `0x005f2770`
>    class: extract it from `MASHED.exe.unpatched` as `H3-CONSTS` did.
> 2. **The 36-branch is a port from zero** — `FUN_00414a70` plus its unported callee `FUN_00414300`.
> 3. **[UNCERTAIN], and it gates both:** the census above is **static**. An exe-side reference
>    proves the address is addressed by compiled code, **not** that the value is live in a race.
>    `RESULT_STEP2.md:223`'s risk that `LeaderTimer` reads `.bss` zeros is narrowed, not closed.
>    **Open whichever leg wins with a RUNTIME substrate census**, or the counter reports a number
>    with no meaning (the `_abs`-column failure mode).
> 4. Still open and independent: **H2** (`MASHED_SLOTSTATE_SEED` default-ON, priced by `H1-ALL` at
>    75% of rows), **U-9191** item (b), **U-9195**, the `0x00409b0e` jumptable.
>
> **Standing note earned here:** a gate can be registered as *instrumentation* when the thing it
> counts does not exist on the measured side. Before registering a counter, check the predicate has
> a body **and** a live substrate on the side being measured — the same discipline as "the
> instrument must touch the changed path", one level earlier.
>
> **Do NOT ship the `MASHED_RACEMETRIC_ARC` ON arm.** Blocks below are previous headlines.

> ## UPDATE 2026-10-08 (U-9186 leg H3 RAN = DEFERRED leg B, commits `1560986e` + the decision record): **the `0x008989b0` producer WORKS — live per-car distances for the first time (26,994/53,992 rows non-zero, 19,562 distinct) — and is INERT at the behaviour level. The leg-B prohibition was LIFTED by USER DECISION (Mariano 2026-10-08). Default-OFF behind `MASHED_REFDIST`. NO C-level, `original/` untouched, `.asi` untouched.**
>
> Read [`RESULT_H3.md`](../verify/d3_u9186_20261008/RESULT_H3.md), then
> [`VEHICLE_TABLE_BIND_SCOPE_2026-10-08.md`](analysis/VEHICLE_TABLE_BIND_SCOPE_2026-10-08.md).
>
> - `H3-CONSTS` PASS — 80.0 / 20.0 / 100.0 / 0.8 read from `MASHED.exe.unpatched` (`.rdata`
>   `0x1cc730`/`0x1ccd6c`/`0x1cc568`/`0x1cc9bc`). `race_pct` is 0..100 per lap, so ">80 and <20"
>   is the pair straddling the line and `-100.0` un-wraps it.
> - `H3-PAIR` live (7 distinct `(ref,other)` pairs vs 1 on OFF). `H3-DET` identical per arm.
> - `H3-WROTE` **FAILED as registered** (100% required, 74.9963% measured). Cause is the scorer:
>   13,497 of 13,500 mismatches are car 0, which the gate chain correctly skips while the probe
>   computed a distance for every car. On the gate-passing denominator: **40,491/40,494 =
>   99.9926%**. The 3 residual rows are all **frame 0** and the port is the side that is right.
>
> ### START HERE — three things are owed before anything new
>
> 1. **`H3-GATEFIRE` was registered as "the one that matters" and was NOT BUILT.** Nothing is
>    claimed about whether U-9186's branches fire. Build the default-OFF counter on
>    `FUN_00414a70 == 2` and `FUN_004148b0 != 0 && FUN_00416060 != 0` and confirm they approach the
>    original's **36 / 64** calls. The committed captures `o_t1`/`o_t2`/`o_t3` carry the exact 149
>    diverging calls.
> 2. **`H3-INERT` is expected, not a defect.** `ControlStep` still hardcodes `mode = 0`
>    (`AiStandalone.cpp:844`), so the branches cannot fire whatever `0x008989b0` holds. H3 removed
>    the *input* blocker, not the stub. Do not read inertness as the port failing.
> 3. **`H3-KNOBOFF` / `-NOREG-E` / `-NOREG-B` were not run** — the gates dump is a different schema
>    from the stepdump those scorers take. Add a stepdump arm to `run_h3.ps1`.
>
> Only after 1-3 does wiring the `FUN_00416250` branches to `ctrl` make sense; that is a separate
> registered leg. **H2** (the `MASHED_SLOTSTATE_SEED` default-ON question) is still open and is
> cheap. Also still open and independent: **U-9191** item (b), **U-9195**, the `0x00409b0e`
> jumptable.
>
> **Standing notes earned this session.** (a) A gate's instrument must touch the path the change
> alters — `G-TOOK`, `H1-CONVERGE` and `H3-WROTE` all failed on this, each costing a rebuild and a
> re-run for a subject that had passed (`PREREG_H1.md` §A2). (b) `scripts/lint_rva_bodies.py`
> binds a function to the **first RVA token in the comment above it**; a comment that merely
> *mentions* an RVA will be read as a body declaration. (c) The lint's allowlist is a burn-down
> list — prefer consolidating a pair into one shared TU over adding a new one.
>
> Blocks below are previous headlines, left as history.

> ## UPDATE 2026-10-08 (U-9186 leg H1a RAN, commits `714a97e1`/`6e805552`): **the vehicle-record rebind LANDED, all gates pass. `VehicleSlotGetter`/`VehicleCarStateRead` now return the port's live record standalone (100% of 53,992 rows), and the default build is byte-for-byte unchanged. H1a is a PRECONDITION for H3, not a behaviour change. NO C-level, nothing default-ON, `original/` untouched, `.asi` unedited.**
>
> Read [`RESULT_H1.md`](../verify/d3_u9186_20261008/RESULT_H1.md), then the scope
> [`VEHICLE_TABLE_BIND_SCOPE_2026-10-08.md`](analysis/VEHICLE_TABLE_BIND_SCOPE_2026-10-08.md).
>
> - `H1-CONVERGE` **100.0000%** (3 arms), `H1-GATE2` `veh_type_fn == 1` **100%** (was `0`),
>   `H1-ALL` **75.0000%** with `MASHED_SLOTSTATE_SEED=1` — the three AI cars.
> - `H1-KNOBOFF` byte-identical to `E2off_1.csv`; `H1-NOREG-E` 3/3; `H1-NOREG-B` unchanged.
> - **Control held**: the `_abs` columns stayed dead, so the rebind moved the *binding*, not memory.
> - **`H1-CALLERS` = 0.** Neither function has a standalone caller (`ScoreMasks_ah3.cpp` is
>   `.asi`-only), so `H1-KNOBOFF`'s byte-identity is near-tautological and **weak evidence**. The
>   evidence the rebind works is `H1-CONVERGE` + `H1-GATE2`.
>
> ### START HERE
>
> 1. **H1b** — `0x0046d4a0` exe body. `.asi`-only today with an empty `exe_file`, so it needs a
>    second body at one RVA: duplicate-RVA hazard, `hooks.csv` entry through `re-classify`.
> 2. **H2** — the `MASHED_SLOTSTATE_SEED` default-ON question. `H1-ALL` now shows what it buys.
>    It stops being inert once H3 exists, so the scope's §5 gates run at that point, not before.
> 3. **H3** (`FUN_00442a60`) is **still under the `DEFERRED.md:15` "DO NOT start leg B"
>    prohibition.** H1a does not lift it. The scope argues the prohibition's risk basis has changed
>    — it assumed synthesis was needed, which G1 run 2 refuted — but lifting it is a USER call.
> 4. Also still open and independent of this lane: **U-9191** item (b), **U-9195**, the
>    `0x00409b0e` jumptable.
>
> **Gate-writing note, earned twice this session** (`PREREG_H1.md` §A2): `G-TOOK` and
> `H1-CONVERGE` both failed because the instrument did not touch the thing under test. Before
> registering a gate, name the code path the change alters and the path the instrument exercises,
> and require them to be the same.
>
> **Do NOT ship the `MASHED_RACEMETRIC_ARC` ON arm.** Blocks below are previous headlines.

> ## UPDATE 2026-10-08 (U-9186 leg G1 RAN, commits `ef420bfc`/`b8f6d1de`): **`FUN_00442a60` is unportable-to-effect today and the blocker is the PER-VEHICLE TABLE SUBSTRATE (`0x008815a4` / `0x00881f90` / `0x00881ec8`), not any one function. The registered control `G1-POS` FAILED; no port was written, by the pre-registration's own decision rule. NO C-level, nothing default-ON, `original/` untouched.**
>
> Read [`RESULT_G1.md`](../verify/d3_u9186_20261008/RESULT_G1.md).
>
> - **Measured, 53,992 rows per arm, three arms.** `G1-ALL` = **0** on all three: no row exists
>   where a ported `FUN_00442a60` would write a non-zero distance.
> - **`G1-POS` FAILED.** The per-vehicle record at `0x00881ec8 + v*0xd04 + t*0x40` reads **one
>   (x,z) for all four cars on every row**, in every arm. So the gate counts indict the substrate,
>   not `FUN_00442a60`.
> - **Three gates, three states.** Slot-state: **solvable today** via `MASHED_SLOTSTATE_SEED`
>   (3 of 4 cars read state `2`, player slot 0 stays `0`). `veh_type` (`0x008815a4 + v*0xd04`):
>   **`0` on 100% of rows in every arm, seed-independent** — the hard gate, and it had never been
>   measured before. Position: dead, no knob.
> - **`GATE3`'s 100% pass is vacuous** — a blank table passes an `== 0` test for the wrong reason.
>   Any future gate chain over this substrate must not count it.
> - **The probe is sound**: on `G1both` it reads 17,924 distinct values from `0x008a96ec` (bridge
>   on) while reading one constant from the vehicle record in the same run.
>
> ### START HERE — read the standing prohibitions first
>
> `DEFERRED.md:15` (D-11072) says **"DO NOT start leg B"** (the `FUN_00442a60` port) and
> `RACE_POSITION_RECON_SCOPE_2026-10-06.md` §4 says "Risk: high. Do not start Leg B first."
> `UNCERTAINTIES.md:61` had already recorded the static blockage on 2026-10-06. G1 re-confirmed it
> in-game and added the two gates that record never tested. **Do not re-derive this a third time.**
>
> Options, in priority order:
>
> 1. **Decide the substrate question, which is a USER call, not an analysis one.** Can the
>    standalone synthesize `0x008815a4` / `0x00881ec8` from `race_[]` + `ai_cars_`, the way
>    `0x008a96ec` was synthesized from `arcprog`? That is a bridge of the same shape and the same
>    fidelity trade-off Mariano already decided YES on for race position (`RACE_POSITION_RECON_SCOPE`
>    §1). If YES, U-9186 becomes reachable; if NO, D3 criterion (b) has no live route and should be
>    re-scoped. **[UNCERTAIN]** — nothing measures this yet.
> 2. **U-9191** item (b): car-1 body-heading residual, independent of this substrate.
> 3. **U-9195** participant-count; the `0x00409b0e` jumptable.
>
> **Do NOT start leg B.** **Do NOT ship the `MASHED_RACEMETRIC_ARC` ON arm.** The blocks below are
> previous headlines, left as history.

> ## UPDATE 2026-10-07 (D-11072 leg E2 RAN, commits `ae906890`/`ee98e0ab`/`3dcfa32e`): **the monotone metric reaches the rule engine, reorders the cars on 29.50% of frames, and changes NOTHING else — `rmetric` is the only column that differs between arms across all 19,418 rows. The swap is INERT at the outcome level. The race-position STATE lane is FINISHED; D3's criterion (b) blocker does NOT recede into this consumer. NO C-level, nothing default-ON, `original/` untouched.**
>
> Read [`RESULT_E2.md`](../verify/d3_consumer_20261007/RESULT_E2.md). Gates:
> `PREREG_CONSUMER.md` §4 as amended by [`AMEND_E2.md`](../verify/d3_consumer_20261007/AMEND_E2.md).
>
> - **Every gate scored.** `E2-WROTE` PASS (11671/11671 in-scope rows, both arms). `E2-DIFF` PASS
>   (318/1078 3-car frames = 29.4991% vs a 1% bar). `E2-KNOBOFF` PASS — the new build's OFF arm is
>   identical to `F2a` over all 77 pre-existing columns, so the knob perturbed the default path by
>   exactly nothing. `E2-DET` PASS. `E2-NOREG-E` PASS 3/3. `E2-NOREG-B` unchanged (`v2`'s 5 bands
>   are the pre-existing baseline, guaranteed by `E2-KNOBOFF` — not a regression).
> - **`E2-EFFECT` INERT, with stronger evidence than the gate asked for.** A cell-for-cell diff of
>   the arms names `rmetric` as the *only* differing column, on all 19,418 rows; all six
>   `mashed_re.log` copies share one hash. `UpdateFinishOrder`'s output is read by nothing that
>   affects state under `rule=4`. **[UNCERTAIN]** for rules 0 and 10 — untested.
> - **`E2-LAPAGREE` NON-ZERO: 214/19418 = 1.1021%.** `AMEND_E2.md` A4's registered hazard is real —
>   the pre-registered formula pairs `race_[].laps` with a fraction derived from the separate
>   `race_[].arclaps`. Implemented as registered rather than quietly corrected. No consequence while
>   the swap is inert, but it **blocks any future ship of the ON arm**.
> - **Run 1 failed `E2-WROTE` at 60.10% on a defective witness** (a frozen slot read as a formula
>   error) and is preserved as `*_r1`. A7's `rtick` counter replaced an inferred denominator with a
>   measured one. Worth reading before writing the next witness.
>
> ### START HERE
>
> The D-11072 lane is closed as a route to criterion (b). **The named carrier is unchanged: the
> three unported `FUN_00416250` branches (U-9186).** Options, in priority order:
>
> 1. **U-9186 / `FUN_00442a60`.** E2 removes the rule engine as an alternative route, so the
>    progress-producer port is now the only live path to the AI over-speed COMMAND defect.
>    Everything it needed from the race-position substrate exists and is proven consumed.
> 2. **Re-run E2 under `rule=0` and `rule=10`** if the inertness needs generalising. Cheap: the
>    harness is built (`run_e2.ps1`, `check_e2.py`), one knob change, ~12 min for six runs.
> 3. **Redirect**: U-9191 item (b) car-1 body-heading residual; U-9195 participant-count; the
>    `0x00409b0e` jumptable.
>
> **Do NOT start leg B.** **Do NOT ship the `MASHED_RACEMETRIC_ARC` ON arm** — inert, and A4's
> formula inconsistency is unresolved. The blocks below are previous headlines, left as history.

> ## UPDATE 2026-10-07 (D-11072 legs F1+F2 RAN, commits `79e7e2eb`/`11eea794`/`87433a24`/`e832eaf8`): **the reproducibility blocker is CLEARED. A 240 s standalone race is bit-reproducible under `MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=14400` — three repeats gave BYTE-IDENTICAL stepdumps. E1's blocker was a missing harness knob, not a code defect: `run_e1.ps1` never set it. Leg E2 is UNBLOCKED. NO C-level, zero source change, `original/` untouched.**
>
> Read [`RESULT_F2.md`](../verify/d3_determinism_20261007/RESULT_F2.md), then
> [`RESULT_F1.md`](../verify/d3_determinism_20261007/RESULT_F1.md).
>
> - **F1 (`79e7e2eb`) reported a control failure, not a localization.** `R-PREFIX`'s registered
>   control `L4`-vs-`L10` came out at 1306, equal to the identical-knob pairs. Cause, measured:
>   `L4c` and `L10` differ in 38 of 7411 shared rows while identical-knob `L4b`/`L4c` differ in
>   4488. `MASHED_ROUND_RULE` is near-inert on driving state, so nothing paired against it is an
>   early-divergence control. F1 also found `L4` shares **zero** rows with `L4b`/`L4c` between
>   frames 1306 and 2306 — per-car dump membership is itself a run-to-run variable.
> - **F2 (`87433a24`) passed every gate.** `F2a`/`F2b`/`F2c` are byte-identical (SHA-256
>   `506A210F…`) across 103 s / 183 s / 91 s of wall clock. `R-ROUND` identical, 5 rounds each,
>   where E1's three identical-knob runs gave 5/3/3 rounds and three different matches.
> - **`R-PREFIX` is rehabilitated.** The control F1 lacked now exists: `MASHED_SIM_HZ=59` changes
>   `kSimStep` itself, gives `R-PREFIX` **0** against full overlap, and diverges at frame 0 on
>   exactly the predicted quantity — `round(3000/59)=51` vs `round(3000/60)=50`.
> - **One registered deviation, on the record.** `G-TOOK` FAILED as written: it read the stepdump's
>   `frame` column, which is a dump-local static (`TrackRenderer.cpp:3994`), not `g_det_frame`.
>   Corrected witness: `step_1008` is exactly 50 on all 19,418 rows of every repeat. Reading the
>   remaining gates after that failure departs from `PREREG_F2.md` §4; see `RESULT_F2.md` §1.
> - **Scope limit that carries forward.** Standalone side, scripted-capture regime only.
>   `MASHED_DETERMINISTIC` stays OFF by default and suppresses live input. **E1's captures are not
>   comparable to deterministic ones** — any baseline must be re-taken under the same knobs.
>
> ### START HERE — run leg E2 (`PREREG_CONSUMER.md` §4)
>
> E2's precondition is now satisfied by measurement. Two things must happen first:
>
> 1. **`MASHED_RACEMETRIC_ARC` does not exist in source yet** — grep finds it only in
>    `PREREG_CONSUMER.md` and this file. Implement the default-OFF knob at
>    `TrackRenderer.cpp:5132-5134` (`rc.metric[i] = race_[i].laps + race_[i].arcpct * 0.01f`),
>    rebuild, then run both arms × 3 repeats under the F2 knobs.
> 2. **`E2-KNOBOFF` needs an amendment, pre-registered before running.** As written it compares the
>    knob-OFF arm against "the committed baseline stepdump" — that baseline is E1's
>    non-deterministic capture and is **not** comparable (`RESULT_F2.md` §4). Take a fresh OFF-arm
>    baseline under `MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=14400` and compare against that. This
>    is a correction to the reference, not a loosened threshold — state it in the amendment.
>
> `E2-EFFECT` is now measurable and must NOT be declared unmeasurable. Reuse
> `verify/d3_determinism_20261007/run_f2.ps1` (waits for the `DET_FRAMES` self-exit, stops only the
> PID it spawned) and `check_f2.py` / `re/tools/det_prefix.py` for the determinism gates.
>
> **Do NOT start leg B.** The block below is the previous headline, left as history.

> ## UPDATE 2026-10-07 (D-11072 leg E1 RAN, commits `bcdd3fcb`/`4a0efcc2`/`6ade0f85`): **the race-position metric already has a LIVE consumer and it was never behind `0x007f0fd0` — it is the rule engine, gated on `TrackRenderer::rule_`, reachable today with `MASHED_ROUND_RULE` and no code change. Swapping it to leg A's monotone `arcpct` is NOT inert (28.6–38.6 % ordering change). NEW BLOCKER: round-level match outcomes do not reproduce across repeats, so E2's registered outcome gate is unmeasurable as written. NO C-level, zero source change, `original/` untouched.**
>
> Read [`verify/d3_consumer_20261007/RESULT_E1.md`](../verify/d3_consumer_20261007/RESULT_E1.md). Pre-registered UNRUN `bcdd3fcb`, RAN `4a0efcc2`.
>
> - **The finding.** The port never writes `DAT_007f0fd0` (`aib_game_mode_fd0()` is a hard
>   `return 0`, `TrackRenderer.cpp:96`), but it carries the same rule as `TrackRenderer::rule_`
>   (`exe_main.cpp:2185` → `RaceModes::RaceRule`, the verbatim `FUN_0043dfd0` cup-event table).
>   The rule engine runs default-ON every frame (`TrackRenderer.cpp:5119`) and feeds
>   `UpdateFinishOrder`/`SegmentCheck`/`EvaluateResult` a metric built at `TrackRenderer.cpp:5132`
>   from the **non-monotone** `race_[].progress` — exactly what leg A's `arcpct` replaces.
>   `MASHED_ROUND_RULE=<n>` (`exe_main.cpp:8508-8515`) reaches rule 4/10 on the existing Training
>   recipe.
> - **It executes.** 5 `RULE-EVAL` evaluations per 240 s run, rule-specific: `L0` (rule 0)
>   concludes the match (`r=4`), `L4` does not, `L10`'s timer varies per round.
> - **The swap has content.** Ordering of the 3 AI cars differs between `lap+racepct/100` and
>   `lap+arcpct/100` on 37.75/38.64/38.31/35.41/28.60 % of 3-car frames across the five runs
>   (max per-car delta 0.0499), vs the pre-registered `E2-DIFF` bar of 1 %.
> - **Two gates failed, reported as written.** `E1-EVAL` failed because the control discriminated
>   in the opposite direction to the prediction. `E1-DET` failed outright: three rule-4 runs with
>   identical knobs gave three different matches (round-2/3 scores, 5 vs 3 rounds, max lap 8/1/4).
>   Aggregate scorers stay stable — it is **round-level outcomes** that are wall-clock-sensitive.
> - **`0x007f0fd0` mirror sub-lane is cheap if wanted:** a worker survey found only three readers
>   that execute in a standalone race, all in `AiPreTickRubberBand`
>   (`AiStandalone.cpp:1469/1501/1541`), plus one frontend-only `ModeCodeLookup`
>   (`Frontend/BatchAA_s4.cpp:64`, `[UNCERTAIN]` reachability). It reaches the mode-4/9
>   `GearConstSet` AI-speed scaling, which bears on criterion (e).
>
> ### START HERE — pick one; the fork is real
>
> 1. **Make long-run outcomes reproducible**, then run E2 with its registered outcome gate.
>    This is the blocker E1 found and it unblocks every future behaviour claim over a full match.
> 2. **Run E2's measurable subset now** (`MASHED_RACEMETRIC_ARC=1`, `PREREG_CONSUMER.md` §4):
>    `E2-WROTE`, `E2-DIFF`, `E2-KNOBOFF`, `E2-NOREG-E/-B`, `E2-DET` on the deterministic 60 s
>    scenario. `E2-EFFECT` must be declared unmeasurable, not restated.
> 3. **The `0x007f0fd0` mirror** (`PREREG_CONSUMER.md` §5) — small blast radius, touches (e),
>    which has a deterministic scorer. Note it scales AI speed and could move (e) either way.
> 4. **Redirect**: U-9191 item (b) car-1 body-heading residual; U-9195 participant-count;
>    the `0x00409b0e` jumptable.
>
> **Do NOT start leg B.** The block below is the previous headline, left as history.

> ## UPDATE 2026-10-07 (D-11072 race_pct bridge write RAN, commit `275d9b26`): **the bridge is CORRECT and consumed by a LIVE reader, but behaviourally INERT at `fd0=0`. The race-position STATE problem is SOLVED (state live + proven consumed); the next blocker is the game-mode gate on the consumer, NOT the progress substrate. NO C-level, nothing default-ON, only `0x008a96ec` written, `original/` untouched.**
>
> Read [`verify/d3_racepos_20261006/RESULT_BRIDGE.md`](../verify/d3_racepos_20261006/RESULT_BRIDGE.md). Pre-registered UNRUN `257fcd39`, RAN `275d9b26`.
>
> - **What landed:** a default-OFF knob `MASHED_RACEPCT_BRIDGE` writes the monotone `arcpct` into
>   the original per-car race_pct slot `*(float*)(0x008a96ec + v*0x30c)` in `UpdateRace` (which runs
>   before the AI tick, so the read at `AiStandalone.cpp:1468` sees it same-frame).
> - **8/8 gates PASS.** The one that matters, `G-LIVE` (control): `val_880` (= `AiPreTickRubberBand`'s
>   `ra*0.01+lap` store) tracks `arcpct*0.01+lap` at 1.0000 on the Y arm and is **0 on every N row** —
>   the port's live AI code demonstrably consumes the bridged race position. `G-EFFECT`: (e)
>   bit-identical, (b) same 5 bands → inert. `G-INERT` 0 of 374,662 cells.
> - **Why inert:** `ra → val → 0x0089a880` drives behaviour only through `fd0 ∈ {4,7,8,9}` gates
>   (`AiStandalone.cpp:1472/1492/1505`), and the standalone's `fd0` (global `0x007f0fd0`) is 0.
>
> ### START HERE — reach the consumer (this is a game-mode-wiring problem now, not a progress one)
>
> The race position is live and consumed; to make it *do* something, the `fd0`-gated consumer must
> be reached. Two sub-lanes:
> 1. **Set `0x007f0fd0` to a championship/elimination mode (`fd0 ∈ {4,7,8,9}`)** — investigate what
>    the original writes there for a real round and whether a standalone recipe/knob can reach it,
>    then re-run the bridge arm and see if the finish-order slots / mode-4/9 scaling now move. A
>    default-OFF liveness counter on the `:1472` block first (did it fire), per the discipline.
> 2. **The `0x008a96e8` boost-order path (U-9186, a *different* field):** `Fi_UpdateBoostOrder`
>    (`ForceIntegratorStubs.cpp:176`) reads it ungated to pick the start-boost pair — but its
>    lights-window effect is predicted null (equal progress at the grid). Measure before investing.
>
> **Do NOT start leg B.** Alternatives if redirecting (over-speed not urgent): U-9191 item (b)
> car-1 body-heading residual; U-9195 participant-count; `0x00409b0e` jumptable.
>
> The block below is the previous headline, left as history.

> ## UPDATE 2026-10-06 (D-11072 leg A monotone metric BUILT, commit `201ab935`): **the monotonicity blocker is CLEARED — the port now has a monotone, physical-arc-length race-progress metric (`race_[].arcprog`/`arcpct`, 0 backward-midlap on all 3 cars vs the old 484/669/1273), deterministic and INERT on (e)/(b). The leg-A scale map is now a registrable near-identity. START HERE: run the §4 bridge-write pre-registration. NO C-level moved, nothing default-ON, `0x008a96ec` not written, `original/` untouched.**
>
> Read [`verify/d3_racepos_20261006/RESULT_LEGA_MONOTONE.md`](../verify/d3_racepos_20261006/RESULT_LEGA_MONOTONE.md). Commit `201ab935`.
>
> - **The metric.** New parallel field `race_[].arcprog` = physical arc length along the gate
>   ring: a forward-only `arcseg` pointer advanced at each gate's perpendicular crossing, within-
>   segment along-track projection `t`, weighted by a per-track cumulative-length table built once
>   from `gates_`. Dumped as `arcpct` (0..100 per lap). Read ONLY by `MASHED_AI_STEPDUMP` and the
>   future bridge — it does **not** feed finish order (`progress` still does), so it is inert.
> - **Acceptance met:** backward-midlap **0/0/0** (matching the original's 0), deterministic over
>   3 repeats, criterion (e) **bit-identical** to baseline and (b) the same 5 bands.
> - **One residual, declared:** a small **forward** jump (max 1.9%) per gate crossing from
>   polyline corners (the original's Catmull path is smooth). Monotone-preserving; a fidelity
>   ripple for ordering/threshold consumers, removable only via spline projection — not done.
>
> ### START HERE — run the §4 bridge-write pre-registration (the leg-A deliverable is ready)
>
> `RESULT_LEGA_MONOTONE.md` §4 registers the candidate map UNRUN: `arcpct` and the original
> `race_pct` are both physical-arc 0..100 per lap, so the bridge write is a near-identity
> `*(float*)(0x008a96ec + v*0x30c) = arcpct(v)` for AI cars, behind a default-OFF knob. Its
> inert-first gate (scope-doc §3/§5): a default-OFF probe confirms `FUN_00408ad0(v)` /
> `FUN_00408a50(v)` go non-zero and monotone, and the bridged ordering of the 4 cars matches the
> original's on a matched capture, **before** any consumer is trusted; then car<->car
> no-regression (G-NOREG-E/-B, G-DET, G-KNOBOFF) + modes oracle rule 3 (leg A reaches the
> elimination tiebreak) + the power-up sweep (it reaches the fire gates). Pre-register UNRUN,
> commit, then run. **Do NOT start leg B before the bridge write.**
>
> Alternatives if you'd rather redirect (the over-speed is not urgent): U-9191 item (b) car-1
> body-heading residual; U-9195 participant-count inertness; or the `0x00409b0e` jumptable.
>
> The block below is the previous headline, left as history.

> ## UPDATE 2026-10-06 (D-11072 legs C+A RAN, commits `69e207b9..2708befc`): **fidelity decision YES; leg C is DONE and INERT by design (8/8 gates); leg A finds the port's progress metric is NON-MONOTONE and so NOT bridge-able into the original's spline `race_pct` as-is. The new blocking prerequisite is a monotone, arc-length port progress metric. DO NOT start leg B. NO C-level moved, nothing default-ON, no bridge code, `original/` untouched.**
>
> Read [`verify/d3_racepos_20261006/RESULT_LEGC_LEGA.md`](../verify/d3_racepos_20261006/RESULT_LEGC_LEGA.md).
> Pre-registered UNRUN at `69e207b9`; RAN result `2708befc`. New tool `re/tools/racepct_scale.py`.
>
> - **USER DECISION (Mariano):** a bridged, non-bit-identical race-position substrate is
>   acceptable in the default build, knob-gated, **no C-level** follows. Recorded in the scope
>   doc §1 and the prereg §0.
> - **Leg C — DONE, 8/8 gates, behaviourally INERT (as predicted).** `MASHED_SLOTSTATE_SEED=1`
>   seeds `*(u32*)0x005f2770 = 0x005f2728` once at boot; the literal `FUN_0040e470(v)` then reads
>   the poked `2` on every seeded row and the deref-free control `ss_raw` refutes the null. It
>   changes **no** (e)/(b) behaviour because the standalone's only consumer of the concept is the
>   synthesized constant `aib_veh_type` = `v==0?0:2` (`TrackRenderer.cpp:93`). The seed is
>   bit-faithful (it restores a `.data` initializer the binary carries) and safe default-ON, but
>   buys nothing alone. **The behavioural step is C2** — re-point `aib_veh_type` at the table so an
>   unmarked car reads 0 (death-aware) — **not written, not run.**
> - **Leg A — the pre-registered NOT-COMMON verdict stands, via a corrected (stronger) rationale.**
>   The flat-frac gate I registered is degenerate (original flat_frac is 0 exactly, so `5×0` is an
>   impossible threshold) and the staircase prediction was refuted. The real obstruction, measured
>   as declared collateral: **within-lap monotonicity** — the original `race_pct` has **0** backward
>   steps per car; the port `racepct` = `fmod(laps*n+gate+frac, n)/n*100` runs **backward on
>   484/681/1287 moving steps** per car because `frac` keys on euclidean distance to the gate
>   **center** off the racing line. A non-monotone metric can't feed the original's spline
>   `race_pct` consumers.
>
> ### START HERE — the one lane that unblocks D-11072, or pick another
>
> 1. **Build a monotone, arc-length-proportional port progress metric** (project the car onto the
>    AI spline the port already drives, instead of blending distance to gate centers), then re-run
>    `re/tools/racepct_scale.py lega` — if its collateral monotonicity shows ~0 backward steps,
>    leg A's scale map becomes registrable and leg B is unblocked. This is now the gate for the
>    whole race-position bridge (and its wider payoff: the AI powerup fire-gate chain + mode-3/7).
> 2. **Ship step C2** (re-point `aib_veh_type` at the seeded table, default-ON knob) — small, and
>    it is the only behavioural content leg C has. Gate it on (e)/(b) + determinism like car<->car.
> 3. **Or a different lane entirely** (the over-speed is not urgent — (e) MET, (b) 5 bands):
>    U-9191 item (b) car-1 body-heading residual; U-9195 participant-count inertness re-measure;
>    or recover the `0x00409b0e` jumptable and finish save-acceptance.
>
> The block below is the previous headline, left as history.

> ## UPDATE 2026-10-06 (U-9185 re-baseline + U-9186 measure-inert-first, commits `19e6b2e1..dea053ba`): **car<->car kept default-ON; the ship already cut the over-speed ~a third; porting the over-speed COMMAND branches is INERT, AND so is porting their producer — the fix recedes two layers into a race-position reconstruction. START HERE: pick another lane, or scope that reconstruction deliberately.**
>
> No C-level moved anywhere in this run. No code shipped by the U-9185/U-9186 work (the
> car<->car KEEP was a decision only; the knob already defaulted ON).
>
> - **KEEP decision (Mariano):** `MASHED_CARCAR_CONTACT` stays default-ON (ROADMAP §D3,
>   `19e6b2e1`). No code change — it already shipped that way.
> - **U-9185 re-baselined** (`verify/d3_overspeed_20261006/REBASELINE.md`, `a9fce015`): the
>   shipped car<->car contact cut the AI window over-speed from +41/+40/+34 % to **+26/+28/+24 %**
>   — ~a third, the largest single reduction on this metric, a side effect of the steering gain.
>   U-9185's `3421/3346/3496` is stale. The **COMMAND attribution is reconfirmed on the shipped
>   build** (`ai_speed_onset.py`): onset COMMAND on all 3 cars, physics exonerated <1 % pre-onset.
> - **U-9186 measure-inert-first** (`verify/d3_overspeed_20261006/RESULT_U9186_INERT.md`,
>   `37ed351f`): **porting the two portable `FUN_00416250` branches would be INERT and no code was
>   written.** Both read per-car progress/rank state that has NO standalone writer (BRANCH 1
>   `FUN_00408a50` @ `0x008a96e8`; BRANCH 2 `RefDist`/`FUN_00442cc0` @ `0x008989b0`, leader
>   `0x0089a364`, catch-up `0x0089a4c4/4c8`) — all blank-mapped zeros, documented in the port's
>   own header `AiStandalone.cpp:704-709`.
>
> ### THE PREREQUISITE recedes TWO layers — it is a race-position reconstruction, not a producer port
>
> Checked 2026-10-06 (`verify/d3_overspeed_20261006/RESULT_U9186_PRODUCER.md`, `dea053ba`):
> porting **`FUN_00442a60`** (`Spectator::ComputeDistances`) does **not** unblock it. Its call
> site exists (the race camera `FUN_00446520` = `Race/RaceCamera.cpp` calls it) and its distance
> math uses live positions, but its reference-car **selection** reads state the standalone never
> produces — `FUN_0040e180` reads the `0x005f2770 → 0x005f2728` slot-state table (a load-time
> `.data` pointer the standalone never loads; base 0 → AV/guarded-zero, documented
> `AiStandalone.cpp:1387-1403`) and `FUN_00408ad0` reads per-car progress `0x008a96ec` (writer
> `FUN_00408610`, unported). So it writes **zeros** → `RefDist` stays 0 → the branches still
> never fire. **The real requirement:** reconstruct the standalone's race-position bookkeeping —
> (a) per-car progress `0x008a96ec` (`FUN_00408610`, or a spline-derived standalone progress
> since the AI already drives the splines), AND (b) the `0x005f2728` slot-state table (allocate
> + point `0x005f2770` + fill). A sizeable dedicated effort, pre-registered on its own, with a
> default-OFF gate-fire counter proving `RefDist` goes non-zero before any behaviour wiring.
>
> ### The race-position reconstruction is now SCOPED — DEFERRED D-11072
>
> `re/analysis/RACE_POSITION_RECON_SCOPE_2026-10-06.md` (`33633860`) lays out the effort as
> three legs with per-leg inert-first checkpoints: **C** slot-state table (CHEAP — boot-seed
> `*(u32*)0x005f2770=0x005f2728`), **A** progress bridge (MEDIUM — the port already has
> `race_[].progress`, but it is a gate ordinal vs the original's spline `race_pct`, so a
> pre-registered scale map and a **USER fidelity decision** are needed), **B** reference
> distance (EXPENSIVE — build on A+C). Payoff is wider than the over-speed: live RefDist
> unblocks the whole AI powerup fire-gate chain and the mode-3/7 producer. **To execute it:**
> make the §1 fidelity decision, then start with leg C's liveness probe + leg A's scale
> measurement (one session, no behaviour risk) before committing to leg B.
>
> ### Or pick a different lane — the over-speed is not urgent
>
> The command defect is blocked two legs deep behind D-11072, and the car<->car ship already
> took a third out of it ((e) MET, (b) 5 bands). Alternatives:
> 1. **U-9191 item (b)** — car 1's body-heading physics residual (~0.89 deg at matched
>    position), the genuinely-physics path left on (b).
> 2. **U-9195** — is the car<->car change inert outside 4 participants? (a bounded re-measure).
> 3. **Recover the `0x00409b0e` jumptable** and finish save-acceptance.

The block below is the previous headline, left as history.

> ## UPDATE 2026-10-06 (U-9196 RESOLVED, commits `94485a1c`+): **the car<->car (b) movement is GENUINE behaviour, not a shallow coincidence — verdict PARTIAL. The car<->car lane is COMPLETE.**
>
> **No C-level moved** — `0x00469df0` stays C2. No game run, no build. Read
> [`verify/d3_carcar_20261005/RESULT_U9196.md`](../verify/d3_carcar_20261005/RESULT_U9196.md).
>
> - The feared failure (low median WITHOUT the original's correction tail — "smooth-but-not-
>   cornering") is **REFUTED on both cars**. The original steers bimodally (low median + fat
>   rail-correction tail); both port cars reproduce that shape. New tool `re/tools/ai_steerdist.py`.
> - Verdict **PARTIAL**: **car 1 GENUINE** (exact median match 7=7, both tail gates pass);
>   **car 3 SHALLOW on `G-SHAPE-MED` alone** (median 11 vs 23) — but its tail passes in full,
>   so it corners for real and merely under-steers its reference by ~half. Car 1 also jitters
>   more between corrections (distinct 83 vs 33). Position-matched collateral (R=0.12): median
>   steering diff 7.0 (car 1) / 10.0 (car 3).
> - **Does NOT close (b)**: car 2 still fails entirely, one Training recipe, U-9195's
>   participant-count caveat untouched, C2 unchanged.
>
> ### THE ONE OWED ITEM — a USER DECISION, not more analysis
>
> **Keep or revert `MASHED_CARCAR_CONTACT` default-ON.** It is a genuine step toward the
> original's AI steering on 2 of 3 cars (exact on car 1), regresses no gated (e) statistic, and
> costs car 2 one band (4 → 5). It ships in the default build today. The RESULT lays out the
> trade; the call is the user's.
>
> ### The car<->car lane is otherwise COMPLETE — pick a new lane
>
> Legs 1–3 + U-9196 all landed (`3404612d..94485a1c`). The remaining D3/physics options:
> 1. **U-9185 — the 1.3258x distance over-run** (`verify/d3_noboost_20261003`), the +15..31 %
>    AI over-speed inside the window. The only substantive physics path left on (b), and it is
>    a separate carrier from the steering U-9196 just addressed.
> 2. **U-9195 — is leg 2/3 inert outside 4 participants?** Re-run the car<->car arms at a
>    participant count outside {4,8,9}, with a default-OFF counter on the drafting branch first
>    (so "did not fire" separates from "fired and was overwritten").
> 3. **Recover the `0x00409b0e` jumptable** and finish the save-acceptance question.

The block below is the previous headline, left as history.

> ## UPDATE 2026-10-06 (CAR<->CAR LEG 3, commits `523cc5d9`+): **the pair loop is WIRED, 8 of 8 gates PASS, and the contact deterministically moves (b) from 13 failing bands to 5.**
>
> **No C-level moved** — `0x00469df0` is still C2; a call site is not a behavioural diff.
> **`original/` untouched.** 73 insertions / 0 deletions in one file. exe SHA-256
> `9C09212FD227F6906C546DE9117EF57E4464A4F8C6F0AC5E639B6A5DD853CFDC`. Default-ON, revert
> `MASHED_CARCAR_CONTACT=0`. Read
> [`verify/d3_carcar_20261005/RESULT_LEG3.md`](../verify/d3_carcar_20261005/RESULT_LEG3.md).
>
> - **8 gates PASS:** G-CALLED (358 entries, 24 non-zero returns), G-PAIR (j>i 358/358),
>   **G-NOOPP (exactly 0 under `MASHED_MEASURE_NOOPP=1`** — opponents unstepped → their
>   `[+0x9a8]` slot stays zero → proximity fails), G-NOREG-E (gated (e) digit-identical 3/3),
>   G-NOREG-B (5 bands, ≤13), G-BANDS-UNEDITED, G-KNOBOFF (reverts to 13 bands + baseline
>   digits), G-DET (3 ON runs identical).
> - **THE SURPRISE:** (b) 13→5 bands, deterministic. Cars 1 and 3 enter the original's
>   `abs_steer_median` envelope `[0,23]` (52.5→7.0, 49.0→11.5); car 2 stays out (57.5→44.5,
>   4→5 bands). **Not a stuck-car artifact** — window speed 3057/3254, near-stopped 2/1 of 220,
>   same as OFF; the cars race ~90% speed while steering ~7x less (cleaner line).
> - **It is NOT a (b) claim yet (U-9196).** Mechanism unknown; a median in the envelope is not
>   the per-frame distribution matching it; the 2026-10-02 matrix tested AI *command* levers,
>   never this *physics* lever. **The one thing that decides it: a cross-side per-frame
>   comparison of the ON arm's steering series against the original's
>   `verify/d3_elim_20261003/o_t*.msd.aistep.csv`.**
>
> ### START HERE — close U-9196, which is the live question behind a possible (b) win
>
> 1. **Cross-side steering-series check.** Compare the ON arm (`verify/d3_carcar_20261005/L3on1.csv`,
>    cars 1 & 3) per-frame against the original's `o_t*.msd.aistep.csv` at matched position
>    (`ai_posmatch.py`), not just the median. If the *distribution* matches, this is a real (b)
>    movement on two cars from the car<->car physics; if only the median coincides, it is not.
>    Memory `score-the-bands-from-endpoint-first`, `a-band-scored-off-regime-is-not-a-measurement`.
> 2. **Mechanism probe.** A default-OFF probe on which record field the contact writes
>    (`+0x144/+0x148/+0x14c` angular, `+0x9b0..` linear) that changes the heading the AI steers
>    from — reasoned story is not a measurement.
> 3. **Then the keep/revert decision on `MASHED_CARCAR_CONTACT` default-ON is the user's**, given
>    a possible (b) improvement on 2 of 3 cars with car 2 slightly worse.
>
> **Open caveat from leg 2 (U-9195):** the ring-slot publication leg 3 relies on is inert on
> the 4-participant recipe only; outside `g_playerCount` ∈ {4,8,9} the drafting term is live.
> Leg 3 ran at 4 participants, so that is untested here too.

The block below is the previous headline, left as history.

> ## UPDATE 2026-10-06 (CAR<->CAR LEG 2, commits `283758b7`+): **ring slot `[+0x9a8]` is published, 6 of 6 gates PASS, (e) and (b) did not move a digit — and LEG 3 IS NOW UNBLOCKED.**
>
> **No C-level moved. No band moved. `original/` untouched.** 49 insertions / 0 deletions in
> one file; the pre-existing `[+0x9ac]` write is byte-for-byte unchanged. exe SHA-256
> `ABE7BA36035B94C8294158B2071FE2E7FD6D3160D99EC6EB10AECD8F8D31E3A2`. Read
> [`verify/d3_carcar_20261005/RESULT_LEG2.md`](../verify/d3_carcar_20261005/RESULT_LEG2.md).
>
> - **Gates:** `G-NOREG-E` PASS (`launch` 1426.4 / 2053.0 / 2055.2 and `ft_median_m0`
>   2550.6 / 2053.0 / 2278.2 identical on every digit, 3 of 3 cars x 3 of 3 runs);
>   `G-NOREG-B` PASS (**exactly 13 of 30**, 5/4/4, same band names); `G-BANDS-UNEDITED`,
>   `G-KNOBOFF`, `G-DET` PASS; **`G-SLOT0-LIVE` PASS** — slot 0 goes from **0 of 7775** rows
>   agreeing with `own_x`/`own_z` to **0.999618 / 0.999624 / 0.999625**. The knob
>   (`MASHED_RING_SLOT0=0`, one read site) is a true revert.
> - **My pre-registered "this is NOT inert" expectation FAILED, and that is the real output.**
>   The prereg named three readers; there are **four**. Two (`PhysicsChainHooks.cpp:536`,
>   `:2749`) are in an **asi-only TU** and never execute in the exe. One
>   (`VehicleControl.cpp:103`) forwards the pointer to `Vehicle_Integrate2`, whose exe body
>   declares it **unused** (`Integrate2.cpp:123`). The fourth — **`ForceIntegrator.cpp:112-118`,
>   A5 Phase 2's drafting grip term, never named in the prereg** — is live and its `delta`
>   really did go from identically `(0,0,0)` to real inter-car vectors, but its only output
>   `local_70` is unconditionally overwritten by the original's own law at
>   `ForceIntegrator.cpp:137` (`if (g_playerCount == 4) local_70 = 1.0f`), and this recipe has
>   `participants=4`. **So the no-change gates are predicted, not lucky.**
> - **New row U-9195**: outside `g_playerCount` ∈ {4, 8, 9} that fourth reader is LIVE and leg 2
>   is **not** inert. It does **not** block leg 3 (same 4-participant recipe). Its path to
>   resolution starts with a default-OFF counter on the `kDraftDot <` branch, so "did not fire"
>   is distinguishable from "fired and was overwritten".
>
> ### START HERE — run LEG 3, the pair loop. It is pre-registered and unrun.
>
> [`PREREG_CARCAR.md`](../verify/d3_carcar_20261005/PREREG_CARCAR.md) §4. Insert the pair loop
> **immediately before the `break;` that ends the `for (int pass = 0; pass < 2; ++pass)` retry
> in `Vehicle/VehiclePhysics_StepCar`** — it was `Vehicle/VehiclePhysicsRun.cpp:1093` before
> leg 2 and is **`:1142`** after it (re-locate it by the `// Not-contacted path` comment two
> lines above rather than by the number), calling
>
> ```
> Collision::VehicleCarCarContact(recJ, recI, pass)   // 0x00469df0 via 0x00470bcd
> ```
>
> **with `recJ` FIRST** — the other car, `in_EAX`; `recI` second — the self car, `param_1`.
> Default-ON with the revert `MASHED_CARCAR_CONTACT=0`. The five gates of `0x00470b23..0x00470bc1`
> in order, `Fi_GameMode()` returns 6 which is in `{6,7,10,0xb}`, and `g_participantCount` is
> the count. Gates: `G-CALLED` (>= 1 entry **and** >= 1 non-zero return, with the `(i,j)`
> histogram), **`G-NOOPP` (exactly 0 entries under `MASHED_MEASURE_NOOPP=1` — the control that
> can fail)**, `G-PAIR`, then `G-NOREG-E` / `G-NOREG-B` / `G-BANDS-UNEDITED` / `G-KNOBOFF` /
> `G-DET` re-run against **leg 2's** numbers, not the pre-leg-2 ones. **Registered: a call site
> is not a behavioural diff, so NO C-level moves on `0x00469df0`.**
>
> **Do not assume it closes (b).** The 2026-10-02 matrix had no arm passing (b) on any car, and
> `ROADMAP.md:1929-1932` records the opponents already moving the player's median speed
> **2538 → 691** with `0x00469df0` never running — shared mutable state, a different question.
> And read `RESULT_LEG1.md` §4 / this block before suspecting the C2 transcription: memory
> `broadphase-standin-plane-test-was-the-sink`.

The block below is the previous headline, left as history.

> ## UPDATE 2026-10-05 (CAR<->CAR CALL SITE, 3 commits `3404612d..749f60ab`): **the call site is fully decoded, leg 1 passed 6 of 6, and the wiring is blocked on ONE unpublished ring slot.**
>
> **No C-level moved. No band moved. No (e)/(b) run. `original/` untouched.** The only
> source change that ships is 19 **appended** columns on the default-OFF
> `MASHED_AI_STEPDUMP` plus comment corrections. Both builds clean.
>
> Read, in order: [`re/analysis/CARCAR_CALLSITE_2026-10-05.md`](analysis/CARCAR_CALLSITE_2026-10-05.md),
> then [`verify/d3_carcar_20261005/PREREG_CARCAR.md`](../verify/d3_carcar_20261005/PREREG_CARCAR.md)
> and [`RESULT_LEG1.md`](../verify/d3_carcar_20261005/RESULT_LEG1.md). **Do not re-derive
> what they establish**, in particular do not hunt the hull producer again.
>
> ### Settled, with citations
>
> - **One call site**: `FUN_004709a0` @ `0x00470bcd`. `--callers` and `--datarefs` agree
>   (both were run; `--callers` returns `(none)` for Ghidra-missed callers).
> - **Argument order is the trap.** `ECX` is the `__thiscall` `this` and holds the **self**
>   car; `EAX` holds the **other** car. The port binds `vehA` to `in_EAX`, so the call is
>   **`VehicleCarCarContact(rec(j), rec(i), pass)` — other car FIRST.** `param_2` (the `j`
>   index) is unused. The TU header and both plates said `in_EAX` was `this`; corrected.
> - **Insertion point: one statement**, `Vehicle/VehiclePhysicsRun.cpp:1093`'s `break;`.
>   Both of the original's fall-through paths into the `j` loop converge there; its third
>   path already matches the port's `continue;` at `:1088`.
> - **The hull chain is ALREADY LIVE in `mashed_re.exe`** and is exact. `+0xa28..+0xa54` is
>   world points 4..7 of the 18-point array `FUN_00469aa0` transforms into `+0x9f8`,
>   body-sourced from `FUN_0046b1c0`'s AABB top face — so its three edge lengths are
>   invariants of the 6-float box: `0.437600 / 0.977100 / 1.070616`. New tool
>   `re/tools/hull_invariants.py`: **10,278 of 10,278** original frames (4 captures, 2 cars)
>   and **7772 of 7772** port rows within `1e-3`, median deviations `<= 1.9e-07`.
>   `Collision/ContactStubs.cpp`'s "producer is NOT yet identified" was a **stale comment**
>   and is fixed; **U-9155's own row is a different producer (the box) and stays OPEN.**
>
> ### THE BLOCKER, and it is the whole next session
>
> **Ring slot `[+0x9a8]` is never published in the port.** Written once at
> `Vehicle/VehicleInit.cpp:252`; `SyncContactRingMatrix` publishes through `[0x9ac]`
> instead. Measured: slot 0 all-zero on **7775 of 7775** rows of `P1.csv` (and `rec_958`
> was already 0.0 on 7705 of 7705 rows of `verify/d3_arm_20261005/A1.csv`;
> `re/tools/ai_yawrate.py:71-73` already says so, so **U-9191 leg 3 is unaffected — do not
> re-file this**). Consequence: the proximity gate at `0x00470b44..0x00470ba9` compares two
> `(0,0,0)` centroids and reduces to `0 < radSum²` — **true for every pair at every
> substep** — and `CarCarContacts.cpp:93`/`:97`'s angular lever arms become absolute world
> positions. **So leg 3 is not startable before leg 2, and that is a result, not a
> scheduling note.**
>
> ### START HERE — run LEG 2, which is pre-registered and unrun
>
> `PREREG_CARCAR.md` §3. Publish the ring into **both** slots in `SyncContactRingMatrix`,
> default-ON with the revert `MASHED_RING_SLOT0=0`, **and gate it ALONE with no car<->car
> call added.** It is **registered in advance as NOT inert**: `Vehicle/VehicleControl.cpp:103`,
> `Vehicle/PhysicsChainHooks.cpp:536` and `:2749` all read `[0x9a8]*0x40 + 0x928` and
> currently receive a **zero matrix**, so A4's and A6b's `wheelBlock` input changes inside
> the D2-certified player solver (`Vehicle/BodyOrientationIntegrate.cpp:171-179` already
> flags that reconciliation as owed).
>
> Five gates plus the inverse control `G-SLOT0-LIVE`, whose "before" value leg 1 has
> already established: `G-NOREG-E` (`launch` 1426.4 / 2053.0 / 2055.2 and `ft_median_m0`
> 2550.6 / 2053.0 / 2278.2, 3 of 3 cars), `G-NOREG-B` (<= 13 of 30 bands), `G-BANDS-UNEDITED`,
> `G-KNOBOFF`, `G-DET` (3 repeats). **Registered FAIL clause: if `G-NOREG-E` or `G-NOREG-B`
> fails, revert the commit and report — do not flip the default or widen a threshold.
> Leg 3 does not run.** Port capture recipe is `re/tools/sa_capture.py` as used for
> `P1`/`B1` in `RESULT_LEG1.md` §1.
>
> **Do not assume any of this closes (b).** The 2026-10-02 matrix had no arm passing (b) on
> any car, and `ROADMAP.md:1929-1932` records the opponents already moving the player's
> median speed **2538 → 691** with `0x00469df0` never running — that coupling is shared
> mutable state and is a different question.

The block below is the previous headline, left as history.

> ## UPDATE 2026-10-05 (LONG SESSION, 38 commits, `cc79f0ba..da9ac1ea`): **two things shipped, four retracted or voided, and one strategic map built.**
>
> **No C-level moved all session. No band moved. `original/` clean throughout.** The one
> behaviour-changing edit is in the exe's save path and passed 5 of 5 gates.
>
> ### What SHIPPED, and is the only thing to build on
>
> 1. **`Save/GameSave.cpp` is in `mashed_re.exe` and its four functions are LIVE** —
>    `0x004099e0` `SaveStatusClear`, `0x00404e50` `SaveLoad`, `0x00404f50` `SaveWrite`,
>    `0x00404f80` `SaveFileExists`. `RESULT_GAMESAVE_EXE.md` (4/4 gates) then `RESULT_WIRE.md`
>    (5/5). `Race/GameFlow.cpp` now routes its save/load through them; the standalone had **two**
>    save images and now has one. **G-LIVE proved they are called**, not merely linked —
>    `GameSave_LastReadBytes() == 151456`.
>    **Deviation, registered:** the exe build substitutes CRT file ops and private storage for three
>    callees that live in unmapped space. **The four rows keep the C4 they earned on the `.asi`; no
>    C4 is claimed for the exe copy.**
> 2. **A live bug fixed in `re/tools/gamesave_parse.py`:** `PROFILE_SIZE` was `0x2443C` (148,540)
>    with a comment saying 150,076. The right value is **`0x24A3C`**. The sizes still summed to the
>    file size, so nothing failed — the profile/tail boundary just sat `0x600` too low and every
>    caller got 600 hex bytes of profile prepended to `tail`. 16/16 tests pass.
>
> ### THE STRATEGIC FINDING — read this before planning any port work
>
> **`re/analysis/ASI_ONLY_TRIAGE_2026-10-05.md`.** Of 888 C3/C4 rows with a source file, **619 build
> only into the dev `.asi`**, which never ships. By subsystem the unshipped verified work is
> render 247, audio 142 (1 shipping), gameplay 129 (7 shipping), save 29 (5).
>
> **But do NOT bulk-add them to `exe_sources.rsp`.** `RH_ScopedInstall` is a **no-op in the exe**
> (`Stubs/HookSystemNoOp.cpp:19`), so a linked body is a **dead export unless something calls it by
> name** — and `ROADMAP.md:235-240` already rejected the bulk-link idea. **The lever is CALL SITES.**
> Proof it is real: `Collision/CarCarContacts.cpp` is in the exe with a byte-faithful `0x00469df0`
> and **zero call sites**.
>
> ### What was RETRACTED or VOIDED, so it is not rebuilt
>
> - **U-9194 (omega-arm divergence) — RETRACTED the same day it was filed.** The port's arm is
>   **correct**: `+0x10 == 0` on **222 of 222 moving frames** across three captures, and **220 of
>   220** inside the (b) window. The "70.60 %" came from a denominator dominated by **2558 frames of
>   a parked, eliminated car**. `RESULT_ARMRETRACT.md`.
> - **U-9191's generated-vs-accumulated: still OPEN**, and now for a measured reason — the position
>   join's **median** induced heading error is **0.087865 deg**, so any gate on it needs a threshold
>   above ~0.88 deg. Leg 3's 0.9866 deg still clears at 11.2x.
> - **The save-acceptance question is UNANSWERED after eight steps.** Bounded to one named unknown:
>   the **`0x00409b0e` jumptable** (`switchdataD_00409e40`, "Too many branches") **and its caller**.
>   `RESULT_TICKCHAIN.md` has the full chain; the save flow is a **six-variable state cluster**
>   (`DAT_008a9584/9588/958c/9590/9594/9598`), not one selector.
>
> ### Harness and tracker changes you can rely on
>
> - `scenario_launch.py --no-warp` (boot to menu, hold) and `--poke-u32 rva=val` (contrived, C3).
> - `orig_nav_hold.py` honours `MASHED_ROOT`, has a `peek`, and a `--scan`. **Nav IS validated**
>   (depth 1→2 on push); the earlier "push doesn't navigate" note was my missing-baseline error.
> - `re/tools/`: `ai_armrate.py`, `ai_armregime.py`, `asi_only_triage.py`, `gamesave_spandiff.py`,
>   `stale_uncertain_refs.py`.
> - **`UNCERTAINTIES.md`: 34 struck-ID rows moved to Resolved**, 6/6 gates including a byte-identical
>   multiset check. Active 3099→3065. **The "1362 marker-resolved" figure I published mid-session was
>   a regex artefact** — the real Type-cell count is **18**, and **no automatic rule is safe**
>   (`U-9191`'s Type cell matches "RESOLVED" while the row is open).
>
> ### START HERE — pick one
>
> 1. **Give an already-ported body a call site.** The highest-leverage item in the triage, and
>    `0x00469df0` is the worked example sitting in the exe doing nothing.
> 2. **Close the 1.3258x distance over-run** (U-9185) — the only substantive physics path left on
>    (b), and the reason U-9191's comparisons return opposite signs under different controls.
> 3. **Recover the `0x00409b0e` jumptable** and finish the save-acceptance question.
>
> **Do not assume any of these closes (b).** The 2026-10-02 counterfactual matrix had **no arm
> passing (b) on any car**.
>
> ### The methodological thread, because it cost most of the day
>
> Four separate confident claims of mine needed retracting, and **every one was caught by a second
> instrument or a control, never by re-reading**: a denominator over the wrong population; a gate
> whose LIVE branch was unsatisfiable at the instrument's sampling rate; a control whose expected
> value was indistinguishable from the default; and a regex matching anywhere in a row.
> **Write the control that can fail, and run it before believing the main arm.**

The block below is the previous headline, left as history.

> ## UPDATE 2026-10-05 (D3 ARM RETRACTION, commit `95c0db13`): **U-9194 is RETRACTED hours after being filed. The port's omega arm is CORRECT for a driving car — 222 of 222 moving frames on three captures.**
>
> **Read [`verify/d3_arm_20261005/RESULT_ARMRETRACT.md`](../verify/d3_arm_20261005/RESULT_ARMRETRACT.md)
> BEFORE the ARM block below, which it supersedes on every arm claim.** No C-level moved, no band
> moved, no code behaviour changed (the only source edit is comment-only). Tool
> `re/tools/ai_armregime.py`.
>
> - **`+0x10` is NOT a per-frame fork. It has exactly ONE transition.** 0 for frames 0..1064, then 1
>   for 1065..3622. `+0x4` is its exact complement, `+0x2c`/`+0x30` go `0 -> 50`, four per-wheel pairs
>   go `(0.15, 0.0125) -> (0.25, 0.25)`, and that single step moves **135 of 833** record dwords with
>   an **identical fingerprint on all three captures**.
> - **The 70.60 % figure measured a PARKED CAR.** The capture is `--mode 10`; car 1 is **eliminated**
>   at the transition and never moves again — 23.548 of distance in frames 800..1199, then **0.000**
>   across the remaining 2423 frames. `+0x10` flips to 1 exactly when it stops.
> - **The right denominator, with a held-out confirmation** (criterion fixed in writing before
>   `o_t2`/`o_t3` were opened): `+0x10 == 0` on **222 of 222 = 100.000 %** of moving frames on
>   `o_t1`, `o_t2` **and** `o_t3`. **Zero** moving frames take the other arm. Inside the (b) window
>   (frame_idx 845..1064): **220 of 220**. So where U-9191's 0.9866 deg is measured, **the arms agree.**
> - **Do NOT port a `+0x10` producer.** An always-zero `+0x10` selects exactly the arm the original
>   uses while the car moves. `BodyOrient_OmegaFromAngVel` having no call site is **not** a defect.
> - **KEEP THIS:** **do not lengthen the (b) window past frame_idx 1064.** The existing refusal was
>   statistical; it now has a mechanism — a longer window averages a driving car with a parked one.
> - **The lesson, and it is the session's real output:** *stating a denominator is not the same as
>   checking it is the right population.* G-ARM spelled its denominator out and still chose the wrong
>   one.
>
> ### START HERE — U-9191 is unchanged, and neither remaining path is "add a column"
>
> 1. **Close the 1.3258x distance over-run** (U-9185 / `verify/d3_noboost_20261003`, the +15..31 %
>    speed defect), so frame-matched and distance-matched comparisons stop returning opposite signs.
>    **This is now the only substantive physics path of the two.**
> 2. **Or build a frame-marked join** (memory `next-sample-pairing-needs-a-frame-marker`). This is
>    instrument work: the position join's measured **median** induced heading error is **0.087865 deg**,
>    which caps any admissible claim at ~0.88 deg and so rules out the sub-degree tercile test.
>
> **Do not assume closing either closes (b).** The 2026-10-02 counterfactual matrix had **no arm
> passing (b) on any car**.

The block below is this session's EARLIER headline. **Its arm claims are RETRACTED** by the block
above; its G-INERT, G-ONSET and G-SPAN results stand.

> ## UPDATE 2026-10-05 (D3 ARM, commit `d0ce578e`): ~~The port and the original take DIFFERENT omega arms — 70.60 % vs 0 %~~ — **RETRACTED, see above.**
>
> **No C-level moved, no band moved, no hooks.csv change, no D2 WATCH.** Read
> [`verify/d3_arm_20261005/RESULT_ARM.md`](../verify/d3_arm_20261005/RESULT_ARM.md);
> `PREREG_ARM.md` was committed **unrun** at `5f3d2bdd` and **amended unrun** at `87531150` (G-PORTARM
> added after G-ARM failed and before the capture ran). New row **U-9194**; **U-9191 amended**.
> New tool `re/tools/ai_armrate.py`. Anchor verified before arming.
>
> - **G-ARM FAIL, and the failure is the finding.** `FUN_0046e9e0` forks on record `+0x10` (the
>   plate's `ESI[4]`). The **original** takes the non-zero **torque-seed** arm on **2558 of 3623**
>   frames (**70.60 %**) and the zero **steer** arm on 1065 of 3623 (29.40 %), measured offline on the
>   committed `o_t1.msd`, car 1, an AI car. `VehiclePhysicsRun.cpp:1001` calls
>   `BodyOrient_OmegaFromSteer` **unconditionally**, so the **port** takes the zero arm on **100 %** of
>   substeps — and `BodyOrient_OmegaFromAngVel` has **ZERO call sites anywhere**.
> - **G-PORTARM MISMATCH-UPSTREAM — do not "fix" this at the caller.** The port's `rec_10` is `0` on
>   **220 of 220** windowed rows and **no file under `mashedmod/src/` writes record `+0x10`** (both
>   sites are reads: `Integrate2.cpp:527`, `PhysicsChainHooks.cpp:2139`). Wiring the gate against an
>   always-zero field reproduces exactly what the port already does. **The missing port is the `+0x10`
>   PRODUCER, and that is new reversing.** `o_t1.msd` gives a per-frame expected series to validate a
>   candidate producer against: **2558 / 1065 of 3623**.
> - **G-ACC was NOT run**, per G-ARM's own registered FAIL clause. **Do not compare `+0x144`/`+0x148`/
>   `+0x14c` cross-side until U-9194 is resolved** — the arms provably differ, so it is the
>   torque-vs-rate error in another costume.
> - **The position join is 3.5x too coarse for a sub-degree gate, and this is now measured.** Its
>   **median** induced heading error is **0.087865 deg** over the 105 matched pairs (0.088920 on the
>   103 that move, so not a construction zero). **Any future gate on this join needs a threshold above
>   ~0.88 deg.** Leg 3 is unaffected — its 0.9866 deg clears it at **11.2x**. Recorded but **not
>   adjudicated**: `PREREG_LEG3.md` §5c's *"median 0.0000 deg"* for the same quantity lands within
>   2.5 % of `RESULT_LEG3`'s own **p90 of 0.0902**.
> - **G-SPAN passed its clause and must NOT be quoted as a finding.** 57.6269 deg against >= 0.50,
>   frame-count matched — but the port covers **1.3258x** the arclength in the same 220 frames, and
>   over the common spatial span the difference is non-monotonic and **ends at the opposite sign**
>   (-9.7987 deg). Per-tercile ratios swing **2.09 / 12.77 / 0.53** because at the same tercile the two
>   sides are on **different stretches of road**. **COMBINED = INCONCLUSIVE**, as the pre-registered
>   rule requires.
> - **Instrument inert:** four appended columns (`rec_144`, `rec_148`, `rec_14c`, `rec_10`),
>   **0 mismatching cells of 323022** (42 cols x 7691 rows) against the committed `L1.csv`.
> - **Chased and NOT filed:** the exactly-90.0000 deg start-heading gap is `atan2(0,0)` on the port's
>   first windowed row; its second reads -90.2997 against -90.0000. Guarded, nothing affected.
>
> ### ~~START HERE~~ — SUPERSEDED. Item 1 below is RETRACTED; use the block at the top of this file.
>
> U-9191 cannot be settled by another column or a tighter radius. The two viable paths:
>
> 1. ~~**Port the `+0x10` producer (U-9194).**~~ **RETRACTED 2026-10-05 — DO NOT DO THIS.** The arms
>    agree on every frame where the car drives (222 of 222, three captures), so an always-zero `+0x10`
>    is already the correct arm and no producer is owed. The "2558 / 1065 of 3623 series" named here as
>    a validation target is a **parked, eliminated car**. See `RESULT_ARMRETRACT.md`.
> 2. **Or close the 1.3258x distance over-run first** (U-9185 / `verify/d3_noboost_20261003`, the
>    +15..31 % speed defect), so frame-matched and distance-matched comparisons stop returning
>    opposite signs and G-SPAN-shaped evidence becomes readable at all.
>
> **A frame-marked join** is the third option and it is instrument work, not physics: it is what a
> sub-degree generated-vs-accumulated test needs, since the position join caps admissible claims at
> ~0.88 deg.
>
> **Do not assume closing any of these closes (b).** The 2026-10-02 counterfactual matrix had **no
> arm passing (b) on any car**.

The block below is the PREVIOUS session's headline, left as history.

> ## UPDATE 2026-10-05 (D3 RATEFIELD, commit `38450632`): **U-9193 RESOLVED — `+0x9c0` is the yaw TORQUE. The yaw rate is `+0x148`, and it was documented here since 2026-05-12.**
>
> **No C-level moved, no band moved, no code changed, no game run, no D2 WATCH.** Read
> [`verify/d3_omega_20261005/RESULT_RATEFIELD.md`](../verify/d3_omega_20261005/RESULT_RATEFIELD.md).
>
> - **`+0x9bc`/`+0x9c0`/`+0x9c4` is the TORQUE triple. `+0x144`/`+0x148`/`+0x14c` is the angular
>   velocity.** Already stated in `re/analysis/vehicle_promote_c2/0046e9e0.md:27-28` (committed
>   **2026-05-12**) and in `vehicle_dynamics/0046e9e0.md:30,49-51`. Confirmed by an independent static
>   read of `FUN_0046e9e0`.
> - **READ THE PLATE BEFORE BUILDING A PROBE.** Two of this session's legs (leg 3's `.msd` KA-1 and the
>   Frida entry-hook probe) tested `+0x9c0` as a rate and were measuring the wrong field. One grep of
>   `re/analysis/**/0046e9e0.md` would have prevented both.
> - **It explains everything that looked strange:** `+0x9c0` tracks turning exactly but is not
>   proportional to Δheading because a **torque is not a rate**; `+0x9bc`/`+0x9c4` being exactly 0.0 is
>   the **pitch/roll torque** on flat ground — **U-9175's physics was right, only its label was wrong.**
> - **U-9175's naming is corrected, scoped:** its label and mechanism are wrong; **all its numbers and
>   its conclusion stand.** Same mislabel in `BodyOrientationIntegrate.cpp:63` (`kAngVel`) — owed a
>   **comment-only** fix, because the port's **behaviour** is faithful (both omega arms, the `+0x144`
>   accumulator with the right gate, the y-term omission).
> - **For U-9191 this REMOVES a candidate, it does not supply one.** The port's rate construction is
>   structurally faithful, so car 1's **0.9866 deg** divergence is **not** a wrong-input bug.
>
> ### START HERE — compare the accumulator, but establish the arm first
>
> 1. **Add `+0x144`/`+0x148`/`+0x14c` to the port's `AiStepDump`** (one more triple; the inert-knob
>    pattern is proven twice over). The original's `.msd` **already carries them**, so **no new
>    original-side run is needed**.
> 2. **First establish which omega arm each side takes per frame** — `ESI[4]` (`+0x10`) `== 0` vs
>    `!= 0`. `BodyOrientationIntegrate.cpp:30` already records this fork as unresolved. **Comparing the
>    accumulator without knowing the arm would repeat exactly the error of comparing a torque to a
>    rate.**
> 3. **Then** answer U-9191: is car 1's 0.9866 deg **GENERATED** or **ACCUMULATED**?
> 4. **Do not assume closing it closes (b).** The 2026-10-02 matrix had **no arm passing (b) on any
>    car**.
>
> **Ghidra MCP was unreachable this session.** `re/tools/decomp_pc.py` is the sanctioned no-MCP path
> (`-readOnly` against a pool clone, still Ghidra on the anchored binary). Constants were read from
> `original/MASHED.exe.unpatched` bits-first, not from Ghidra's `_DAT_` rendering.

The block below is the PREVIOUS session's headline, left as history.

> ## UPDATE 2026-10-05 (D3 OMEGA session, commit `0af828b1`): **`+0x9c0` is NOT the yaw rate — proved at a known program point.** — the field it *is* was identified the same day, see above.
>
> **No C-level moved, no band moved, no port code changed, no D2 WATCH.** Read
> [`verify/d3_omega_20261005/RESULT_OMEGA.md`](../verify/d3_omega_20261005/RESULT_OMEGA.md);
> `PREREG_OMEGA.md` was committed **unrun**. Anchor verified before arming.
>
> - **Reading `+0x9c0` at A6a ENTRY changed NOTHING.** Matched denominators, car 1: active-drive
>   **312 of 437 = 71.40 %** at the hook against **312 of 436 = 71.56 %** on the `.msd`; all rows
>   **317/2433** against **317/3623**. The **same 312 and the same 317**. So U-9193's
>   "zeroed phase" candidate is **dead** and **`+0x9c0` is not `omega.y`** — do not use it as a rate
>   anywhere.
> - **G-RATEID fails at the known phase too**: Pearson **0.057699** against 0.068251 from the
>   snapshot. 288 turning pairs, `+0x9c0` zero on **0** of them (support matches turning perfectly),
>   ratio median **-6.8962**, p10 -1.276e4, p90 +73.73, sign-inverted.
> - **G-ZERO extends U-9175:** `+0x9bc` and `+0x9c4` are **exactly 0.0 on all 2433** A6a-PRE rows, so
>   "the original's omega.x/z are exactly 0" holds for an **AI car at a known phase** and is not a
>   phase artifact.
> - **`--axis-probe` was NOT player-only** — it is parameterised by `--statediff-car`. It now also
>   logs `wx`,`wy`,`wz`,`px`,`pz`,`esi`, appended so existing consumers are unaffected.
> - **KA-B FAILED — a defect in the inherited probe.** `a6bEsiRec = 0 of 9672`: ESI does **not** hold
>   a record pointer at A6b, so its POST rows are **redundant duplicates of one record** (3.98x the
>   PRE count). Harmless for U-9175's one-car arm; it would **4x over-weight any per-row POST
>   statistic on a multi-car arm**. Fix or document before using POST rows.
>
> ### START HERE — stop probing offsets, read the writer
>
> 1. **The body forward row `+0x9d4`/`+0x9dc` demonstrably DOES rotate, so the rate exists
>    somewhere.** `BodyOrientationIntegrate` (`FUN_0046e9e0`) is **already named as its writer**.
>    **Read what that function actually READS** in Ghidra (use the `ghidra-pool` skill) and let the
>    disassembly name the rate input. Do **not** probe more offsets by trial — that is what just
>    failed twice.
> 2. That same read settles U-9193's second question: **what drives the original's orientation when
>    `omega.x`/`omega.z` are identically zero**, and whether the port's `BodyOrientationIntegrate`
>    inputs correspond to the original's at all.
> 3. **Only then** return to U-9191's open question — is car 1's confirmed **0.9866 deg** heading
>    residual **GENERATED** at the matched instant or **ACCUMULATED** before it? The port side already
>    carries the fields in its dump.
> 4. **Do not assume closing this closes (b).** The 2026-10-02 counterfactual matrix had **no arm
>    passing (b) on any car**.
>
> **Two drafting failures of mine today, both recorded in the results:** leg 3's KA-1 was mis-scoped
> (it gated a leg that did not use the field it tested) and this session's **G-OMEGA was ill-posed** —
> its PASS clause used the active-drive denominator while the figure it was to be compared against was
> computed over all frames, so its nominal PASS is **void**. Check a gate's denominator against the
> number it will be compared to **before** committing it.

The block below is the PREVIOUS session's headline, left as history.

> ## UPDATE 2026-10-05 (D3 LEG3 session, commits `52599239` .. `d717a713`): **U-9191 is CONFIRMED by an independent instrument. The rate legs failed their KA, so generated-vs-accumulated is still OPEN.**
>
> **No C-level moved, no band moved, no D2 WATCH.** New row: **U-9193**. Read
> [`verify/d3_leg3_20261005/RESULT_LEG3.md`](../verify/d3_leg3_20261005/RESULT_LEG3.md);
> `PREREG_LEG3.md` was committed **unrun** at `52599239` and amended **unrun** at `01ada11e` and
> `4429c0ec`.
>
> - **G-DIRECT PASS — U-9191's residual is real.** Measured **directly** from the record's forward
>   row on both sides, no `err` inversion, no poll: **0.9866 deg (n 105)** against U-9185's published
>   **0.9796** on the same-size population. Two different derivations, **0.7 %** apart. The gate was
>   written so it could kill the parent row; it confirms it. **U-9192's identity finding still stands
>   as a methodological correction — the number it produced just happened to be right.**
> - **Only medians are admissible from leg 3.** The pairing's p90 induced error (0.0902 deg) sits on
>   the registered 10x line (0.0892). No tail statistic is reported, and none should be quoted.
> - **KA-1 FAILED (corr 0.068251).** `+0x9c0` is **not** a per-frame yaw rate at snapshot phase, so
>   **the rate legs are abandoned and no cross-side rate number exists.** Finding: the original's
>   `+0x9bc` and `+0x9c4` are **exactly 0.0 on all 3623 frames** (extends U-9175 from the player to
>   an AI car), `+0x9c0` is non-zero on only **317**, its support matches turning exactly but
>   `Δheading / +0x9c0` scatters over six orders of magnitude, sign-inverted. **U-9193.**
> - **The port's dump now carries seven more record fields** (`rec_958`, `rec_960`, `rec_9d4`,
>   `rec_9dc`, `rec_9bc`, `rec_9c0`, `rec_9c4`), appended so nothing moved, proven inert at
>   **6479/6479** byte-identical pre-existing columns.
> - **Do not re-use these two assumptions:** the port writes `+0x958`/`+0x960` as **identically 0.0**
>   (it keeps position in `a.pos[]`), and a per-frame `.msd` snapshot cannot be paired to a per-call
>   dump without a measured phase term.
>
> ### START HERE — the open question needs a Frida entry hook, not an offline pass
>
> 1. **Read the original's AI-car yaw rate at a known program point.** `scenario_launch.py`'s
>    `--axis-probe` and `--lat-bracket` already sample `+0x9d4`/`+0x9dc` and
>    `+0x9bc`/`+0x9c0`/`+0x9c4` **at entry hooks** and are the proven shape — but **both target the
>    PLAYER** and need an AI-slot filter. Entry hooks only (memory
>    `frida-interceptor-is-entry-only`).
> 2. **Settle what `+0x9c0` is before using it** — U-9193's two candidates are "not `omega.y`" and
>    "sampled at a zeroed phase". Do not guess between them.
> 3. **Then answer U-9191's question:** is car 1's 0.9866 deg **generated** at the matched instant or
>    **accumulated** before it? The port side already has the fields in its dump.
> 4. **A D2 WATCH becomes owed only if the rate turns out to diverge** — the angular velocity is a D2
>    surface. Nothing is owed yet.
> 5. **Do not assume closing this closes (b).** The 2026-10-02 counterfactual matrix had **no arm
>    passing (b) on any car**.
>
> **Three assumptions in my pre-registration were wrong** (`+0x9c0` as a rate, the port writing
> `+0x958`, and pairing without a phase term) and **one gate was mis-scoped by me**. All are recorded
> in the result. A better-informed registration would check the port's **field coverage** before
> registering a pairing key — do that next time.

The block below is the PREVIOUS session's headline, left as history.

> ## UPDATE 2026-10-05 (D3 HEADATTRIB session, commits `225ee06f` .. `b204e34d`): **item (b)'s heading share is RE-ATTRIBUTED and it splits per car. Car 1 is REAL at the physics basis.**
>
> **No C-level moved, no band moved, no game code, no build, no game run** — legs 1, 2 and 2b are
> entirely offline on committed captures. New rows: **U-9191**, **U-9192**. Read
> [`verify/d3_headattrib_20261005/RESULT_HEADATTRIB.md`](../verify/d3_headattrib_20261005/RESULT_HEADATTRIB.md);
> `PREREG_HEADATTRIB.md` was committed **unrun** at `225ee06f` and **amended unrun** at `d5da030e`.
>
> - **U-9185's framing of the heading share as one phenomenon across "2 of 3 cars" does not hold.**
>   `d_body_heading` median_abs, matched position → + identical `(c0,c1)` → + speed within 5 %:
>   **car 1 0.9796 (n 105) → 0.9417 (n 36) → 0.8918 (n 31) = REAL**; car 2 0.9085 → 0.1067 → 0.1064
>   (n 17) = **NO-VERDICT**; car 3 0.6506 → 0.2328 → 0.208 (n 18) = **NO-VERDICT**.
> - **Car 1 is a genuine physics divergence.** Its residual is stable across the whole registered
>   tolerance ladder (0.790 / 0.8918 / 0.9764 at 1/5/10 %) and does **not** collapse under speed
>   matching. Staleness, command and speed are each excluded on car 1. Carried by **U-9191**.
> - **Cars 2 and 3 look like U-9186's command defect** — their `d_err` collapses alongside their
>   heading term (1.2964 → 0.0468, 0.4661 → 0.0878) — but both are below the pre-registered n floor of
>   30 and are **NO-VERDICT, not artifact**. Do **not** loosen the match radius or lengthen the window
>   to reach a verdict on numbers already seen; either needs its own pre-registration.
> - **`ai_posmatch.py`'s `d_body_heading` is an ALGEBRAIC IDENTITY** on the other two reported
>   quantities (`:411`; max residual **5.684e-14** deg). It is **not** an independent measurement of
>   the body heading, so stop citing it as evidence that a residual is *in the physics*. The tool and
>   its committed numbers are fine and reproduce exactly — only the inference was wrong. **U-9192**.
> - **Staleness was tested and refuted.** U-9186's 100-of-660 frozen calls land almost entirely
>   outside the matched population: cars 1 and 2 have **zero** frozen pairs on either side, and
>   excluding car 3's 26 moves it **up** to 0.7269.
>
> ### START HERE — leg 3, already scoped, promoted by the registered gates
>
> 1. **Add `+0x9d4`/`+0x9dc` and `+0x9bc`/`+0x9c0`/`+0x9c4` to the port's `AiStepDump`** — five more
>    `VehiclePhysics_RecordF32` calls beside the `+0x9e4` and `+0xb0c` it already reads. The
>    inert-knob pattern is proven (`MASHED_AI_YAWW`, **6480/6480** byte-identical rows).
> 2. **Compare against `o_t1.msd` / `o_t2.msd`** via `re/tools/statediff/msd_fields.py` — they carry
>    the same offsets for **car slot 1, an AI car**, so **no new original-side run is needed**.
> 3. **Compare the yaw RATE, not the heading.** This is the entire point: a heading is an **integral**
>    of past yaw rate, and matched position + command + speed still does not match **history**, so car
>    1's 0.8918 deg may be **accumulated before** the matched instant rather than generated at it.
> 4. **VOID condition, not a caveat:** the `.msd` is one snapshot per render frame while the aistep
>    CSV has several AI calls per frame. Leg 3 must report its pairing residual and is **VOID** unless
>    that residual is at least **10x smaller** than the difference it claims.
> 5. **Do not assume closing this closes (b).** The 2026-10-02 counterfactual matrix had **no arm
>    passing (b) on any car**.
>
> **My registered prediction (H-STALE) was WRONG** — excluding the staleness made the residual
> slightly worse. Two defects in my own work are disclosed in the result: the tool first conflated "no
> `ret14a70` column" with "no qualifying row" (fixed, numbers unaffected), and the delegated capture
> inventory wrongly reported that `o_t3` does not exist.
>
> **No D2 WATCH** — this is a D2 surface seen through the AI, but C1 was not reached and no D2 code
> was read or changed.

The block below is the PREVIOUS session's headline, left as history.

> ## UPDATE 2026-10-05 (D3 YAWW session, commits `49fc1ca0` .. `927304d9`): **U-9188 is RESOLVED and its answer is RETRACTED. U-9185 item (b) is REOPENED.**
>
> **No C-level moved, no band moved, no scorer edited.** Only game-code change is one default-OFF
> knob. New rows: **U-9189**, **U-9190**. Read
> [`verify/d3_yaww_20261005/RESULT_YAWW.md`](../verify/d3_yaww_20261005/RESULT_YAWW.md);
> `PREREG_YAWW.md` was committed **unrun** at `49fc1ca0` and **amended unrun** at `5f6f49c4`.
>
> - **The 2026-10-04 heading residual does not exist.** Measured in-process at two program points
>   with no poll, the port's published `g_aib.fwd[v]` against the record's forward is **0.000000 deg,
>   max 2e-6, on all three AI slots on every frame a car is stepped**. `post_rec` is 0.000000 too, so
>   the sync is exact and the bridge has **no drift term**.
> - **What U-9188 measured was the per-frame heading TURN**, banked by a non-atomic
>   `ReadProcessMemory` poll straddling the record-write / `a.yaw`-sync boundary. Its claimed
>   **0.5214 / 1.1430 / 0.4300** against this run's per-frame turn **0.491309 / 0.491309 / 0.421121**
>   (cars 1 and 3 to 5.7 % and 2.1 %), and its own reported cap of **2.5564** against this run's turn
>   maxima **2.556407 / 2.556403**. Right cap, wrong quantity.
> - **A-REACH PASSED.** `:3393` = `:3423` = `:3683` = `:3717` = **0** with `faithful_nav=1` and
>   `phys=1` witnessed in the same dump. **Both of U-9188's conditional remedies are dead.** Items 1
>   and 2 of the previous handoff are **done and void** respectively.
> - **The writer enumeration was wrong at five — there are NINE.** `:2690` (grid placement, witnessed
>   1/1/1) and `:3304` were missing. And **`VehiclePhysics_ResetOrientation` never touches the
>   record** (`VehiclePhysicsRun.cpp:518-523`) — it resyncs the body **basis**.
> - **A real, separate defect exists: U-9189.** An eliminated-while-spinning car publishes a
>   permanently stale heading, pinned at **66.828354 deg** for ~1300 frames. It **cannot** carry (b):
>   `ai_ctrl_window.py:25-29` scores the first 220 calls with `c4 != 0` and this starts at frame 1232
>   on a non-racing car. Do not fix it to move (b), and each candidate repair needs its own
>   pre-registration.
> - **U-9190: `re/tools/sa_headwatch.py` is not fit for this question.** Its `H-JITTER` control bounds
>   a position while the claim is an angle, and its slot-0 floor agrees by construction. **Do not use
>   that floor as an acceptance target.** Retire it or add a frame marker, and re-derive or withdraw
>   any other result taken from it.
>
> ### START HERE — item (b) goes back to the physics basis
>
> 1. **Re-attribute U-9185 item (b)'s matched-position heading share** (`0.9796 / 0.9085 / 0.6506`) at
>    the **physics basis**, which the 2026-10-04 session recorded as **NOT REACHED**. Item (b)'s two
>    candidates are now **one**: the `(cos, sin)` reconstruction is exonerated, leaving the physics'
>    own `io.yaw`.
> 2. **Use the in-process pattern, not a poller.** A default-OFF knob reading both operands at known
>    program points costs one build and has **no sampling term**. `MASHED_AI_YAWW` is the worked
>    example and is proven inert (**6480/6480** byte-identical AI-step rows knob ON vs OFF).
> 3. **Do not assume closing anything here closes (b).** The 2026-10-02 counterfactual matrix had
>    **no arm passing (b) on any car**.
>
> **Three self-corrections, all registered before the run so none is hindsight:** candidate 5 (frame
> order) was my own registered leading hypothesis and was refuted by static reading; PREREG
> Correction 1 reused U-9188's misreading of `ResetOrientation`; and the writer enumeration was
> corrected from five to nine before counting, which is why `:2690` was instrumented at all.
>
> **No D2 WATCH row** — no D2 code was read or changed.

The block below is the PREVIOUS session's headline, left as history.

> ## UPDATE 2026-10-04 (D3 HEADING session, commits `ed2eba34` .. `ef09fe40`): **U-9185 item (b) is ANSWERED — the body-heading residual is a PORT-ONLY BRIDGE defect, not a physics one.** — **RETRACTED 2026-10-05, see above.**
>
> **No C-level moved, no band moved, no game code / `.rsp` / build.** New row: **U-9188**.
> Read [`verify/d3_heading_20261004/RESULT_HEADING.md`](../verify/d3_heading_20261004/RESULT_HEADING.md);
> `PREREG_HEADING.md` was committed **unrun** at `ed2eba34`.
>
> - **The port carries TWO heading values per AI car and they disagree inside the same
>   frame.** `g_aib.fwd[v]` (what `SteerAngleErrorFwd` reads) vs the record's
>   `+0x9d4`/`+0x9dc` (the field the ORIGINAL's `FUN_0046d510` returns): median
>   **0.5214 / 1.1430 / 0.4300** deg on cars 1/2/3, against a **same-instrument floor of
>   exactly 0.0000 deg on the player at every lag**. Same band as U-9185's matched-position
>   heading share (0.9796 / 0.9085 / 0.6506), and **car 2 is the largest on both**.
> - **Car 2 is qualitatively different**, not just larger: **327 of 933** samples above
>   10 deg and **16 above 90** (max 176.65), where cars 1 and 3 never exceed **2.5564** and
>   share a near-identical hard cap with only 5 distinct values above 2.5 — a quantised
>   rate limit.
> - **Staleness is REFUTED** by an integer-lag fit: the minimum is at L=0 and never
>   approaches the floor at any lag 0..4.
> - **Mechanism.** `TrackRenderer.cpp:3334` sets `a.yaw = io.yaw` straight out of
>   `VehiclePhysics_StepCar`, which wrote `+0x9d4`/`+0x9dc` from that **same** `io.yaw` — at
>   that instant they **agree**. `a.yaw` is then rewritten by port-only scaffold code before
>   `:3729` rebuilds the bridge value. `:3351` **resyncs** the record
>   (`VehiclePhysics_ResetOrientation` at `:3355`); `:3283`, `:3393`, `:3423`, `:3717` do
>   **not**. The disagreement **persists** rather than snapping back, so the firing writer
>   is a non-resyncing one.
> - **U-9185's limiter claim is HALF-CONFIRMED.** Correct for `:3683` (inside
>   `AiOptionBStep`, reached at `:3313` only when `!phys`), but a **second copy exists at
>   `:3423` inside `UpdateCar` itself**, in the gate-ribbon block.
>
> ### START HERE — cheap, and it is a count, not a port
>
> 1. **Add one default-OFF counter at each of `:3283` / `:3393` / `:3423` / `:3717`** and
>    count per AI slot over the (b) window. No behaviour change. **Watch car 2 separately.**
> 2. If **`:3423`** fires → stop the gate-ribbon limiter running on the ported path.
>    If **`:3717`** fires → add the `VehiclePhysics_ResetOrientation` its sibling at `:3351`
>    already does.
> 3. Re-run `py -3.12 re/tools/sa_headwatch.py` after any fix. **The slot-0 floor of
>    0.0000 is the acceptance target** and the tool prints it every run.
> 4. Only then re-score (b) — and **do not assume closing this closes (b)**. The 2026-10-02
>    counterfactual matrix had **no arm passing (b) on any car**.
>
> **Two self-corrections from this session, recorded so they are not repeated:** the
> pre-registered prediction that the bridge/record pair agrees "by construction" was **wrong
> by five orders of magnitude**, and the pre-registered `H-JITTER` gate was **ill-posed**
> (position units against degrees) and was replaced by the player-slot floor. A third,
> methodological: a lag fit that includes zero vectors silently agrees, because
> `atan2(0,0) = 0` — filter first.
>
> **C1 (the physics basis) was NOT reached**, so there is **no D2 WATCH row** from this
> session.

The block below is the PREVIOUS session's headline, left as history.

> ## UPDATE 2026-10-03 (D3 LEADER-WITNESS session, commits `3988dbf0` .. `27041abb`): **item 1 below was tested BEFORE being built, and it is INERT. The wiring was NOT performed, and its recorded scope was wrong. START HERE.**
>
> **No C-level moved, no band moved, no game code and no `.rsp` was edited.** New rows:
> **U-9187**. Read
> [`verify/d3_leader_20261003/RESULT_WITNESS.md`](../verify/d3_leader_20261003/RESULT_WITNESS.md);
> the pre-registration `PREREG_WITNESS.md` was committed **unrun** at `3988dbf0`.
>
> - **W-GATE = INERT.** Adding `AiLeaderTimer.cpp` / `AiTargeting.cpp` / `AiLineOfSight.cpp`
>   to `exe_sources.rsp` would be **both unsafe and inert**. **UNSAFE:** the body calls
>   `0x0040e470` (`:94`,`:104`), `0x00442cc0` (`:101`,`:106`) and `0x0046d4a0` (`:109`), all
>   inside `0x00400000..0x004fffff`, which `Compat/StandaloneRvaThunks.h:7` records as
>   **entirely unmapped** in the standalone and `MenuButtonDetect.cpp:71` records as an AV;
>   no thunk covers them. **INERT:** even thunked, the limit table `0x005f2dd8` reads
>   **0 of 64 non-zero** on the port against **14 of 64** on the original, so `:99`'s
>   `limit (0) <= RankAt (0)` returns 0 **before `Prog` is read at `:101`**.
> - **Root cause, and it generalises:** `exe_main.cpp:56` VirtualAlloc-maps
>   `0x00500000..0x009fffff` **BLANK**. So in `mashed_re.exe` every **runtime-written**
>   global is populated and **every initialised-image value reads zero**. Expect this for
>   any future port that reads a `_DAT_005*` table or constant.
> - **Measured both sides.** `Prog[0..3]` is **0.0 on all four slots in all 1089 port
>   samples** against non-zero on **464 / 125 / 486 / 461 of 512** original calls;
>   `TimerAt` **0** vs a live **0..1250 stepping by 50**; thresholds **0.0 ×4** vs
>   **6.5 / 5.5 / 6.0 / 4.0**. W-BASE passed every leg, so "all zeros" is a finding and not
>   a bad base.
> - **THE RECORDED SCOPE WAS WRONG.** "NO new reversing is needed" misses
>   **`FUN_00442a60`** (`0x00442a60`, `Spectator::ComputeDistances`), the producer of
>   `0x008989b0`, which is **C2 `new` with no body anywhere**. `AiStandalone.cpp:707` had
>   already named it unported.
>
> ### START HERE — the corrected order for the 64-call branch (U-9187)
>
> 1. **Port `FUN_00442a60` first.** Without the `0x008989b0` producer every other item stays
>    inert, and it is the **only** one needing new reversing.
> 2. Give `0x0040e470`, `0x00442cc0`, `0x0046d4a0` exe-side bodies (all three are **C3
>    `impl`** with an **empty `exe_file`** — no reversing, just a home the exe links).
> 3. **Do not re-measure these two — they are resolved in U-9187.** Limit table: only index
>    **10** is ever used and its value is **1** (head of 64
>    `[2,1,1,0,0,1,1,0,0,0,1,0,0,0,0,0]`). Thresholds: **6.5 / 5.5 / 6.0 / 4.0**.
> 4. Then settle the two upstream disagreements that survive all of the above, because they
>    decide whether the 64 calls are **reproduced** or merely made **reachable**:
>    `idx364` (**-1** orig vs **0** port) and `bias374` (**0** vs **{0,1,2,3}**).
> 5. Check call-by-call against the committed `o_t1`/`o_t2`/`o_t3`, which carry the exact 64.
>
> **Do not seed a global to make the arm fire** —
> `verify/d3_modes37_20261002/RESULT_STEP2.md`: *"seeding the globals would not be a port."*
>
> New read-only tooling: `re/tools/sa_leaderwatch.py` (`ReadProcessMemory`, no injection)
> and `scenario_launch.py --leader-probe` (**one entry hook** on `0x004148b0`, with a
> threshold known-answer check).
>
> Items **2, 3 and 4** of the previous START-HERE block below are **unchanged and still
> open**.

The block below is the PREVIOUS session's headline, left as history.

Updated 2026-10-03 at the close of the **D3 start-boost A/B** session.
Branch `race/first-frame-parity`. Nothing is pushed. **No game code was edited this
session**; the only code changes are analysis tools.

## The headline

> **UPDATE 2026-10-03 (D3 ELIMINATION + OVER-SPEED session, commits `e0b35ff9` ..
> `46cd19df`): the confound is ZERO and the over-speed is a COMMAND defect with all 149 of
> its calls accounted for. START HERE.**
>
> **No function changed C-level, no band moved, no scorer was edited.** The only game-code
> change is a default-OFF measurement knob (`MASHED_NO_ELIM`).
>
> - **The elimination confound explains 0 % of the arm A vs arm B delta.** Arm C
>   (`MASHED_NO_ELIM=1`) vs arm A: **0 of 2640 scored byte-slots** differ over the window,
>   **0 of 660** on speed/position, all six arms print the same (b)/(e) digits. U-9185 item
>   (a) RESOLVED; every number in `verify/d3_noboost_20261003/RESULT.md` stands.
> - **Two corrections.** `VehicleStep(0)` runs in **neither** port arm and not on the original
>   either. **Both** port arms eliminate the player (arm A window call 112, arm B 146). On the
>   **original** the player is alive through the whole window and dies on the first frame
>   after it, when the camera-zoom gate `FUN_00442df0() == 10.0` saturates — so the
>   elimination time is a **readout of the over-speed**, not an independent variable.
> - **A PRE-REGISTERED GATE FAILED** (`PREREG_STEP2.md` §2.0: the accel/brake model
>   reproduces the **port** 220/220 and the **original** only 0.636/0.936/0.750), so §2.1–§2.5
>   **did not run and are reported nowhere**. `PREREG_STEP2B.md` replaced them unrun.
> - **THE OVER-SPEED IS A COMMAND DEFECT.** Pre-onset, the port's whole force chain
>   reproduces the original's speed to **0.98 / 0.23 / 0.23 %** — drive force, A5 drag, A6a's
>   clamps incl. grip-clamp #6, the contact solver and `+0xb0c` are jointly **exonerated**.
> - **All 149 of 660 diverging calls = three unported branches of `FUN_00416250`**, by live
>   targeting-return signature, identical across three runs:
>   **36** `FUN_00414a70 == 2` → immediate return `0x00416405`;
>   **49** `FUN_00414c30 == 2` → mode 7 → `ctrl[4] = 0x40`;
>   **64** `FUN_004148b0 != 0 && FUN_00416060 != 0` → a second immediate return.
>   **0 unexplained.** (448 calls have no producer firing and the port's mode-0 tail is
>   bit-exact; 63 are mode 3 and agree.)
>
> ### START HERE — the next session's first job, in order
>
> 1. **Wire `FUN_004148b0` into the standalone — 64 of 149 calls, and NO new reversing is
>    needed.** `0x004148b0` is **C3 `impl` `Ai/AiLeaderTimer.cpp`** and `0x00416060` is
>    **C3 `impl` `Ai/AiTargeting.cpp`** with a committed `frida_diff`. What is missing: all
>    three TUs (`AiLeaderTimer.cpp`, `AiTargeting.cpp`, `AiLineOfSight.cpp`) are in
>    **`asi_sources.rsp` only**, and `AiStandalone.cpp:846` carries the comment
>    *"FUN_004148b0 / FUN_00415020 are stubbed (return 0)"* where the call belongs. The
>    decompiled arm to transcribe is quoted verbatim in
>    `verify/d3_elim_20261003/RESULT_STEP2.md`.
>    **MANDATORY FIRST: a pre-registered knob-took witness proving it is NOT inert.**
>    `LeaderTimer` reads per-car rank/progress tables that may be `.bss` zeros in the
>    standalone — the exact trap `verify/d3_modes37_20261002/RESULT_STEP2.md:201-203` already
>    measured for `FUN_00484c70` (*"seeding the globals would not be a port"*). Memory:
>    `verify-the-harness-knob-actually-took`.
>    The committed captures `o_t1`/`o_t2`/`o_t3` carry the **exact 64 calls** the fix must
>    change, so it is checkable call-by-call rather than only through a band.
> 2. Then `FUN_00414a70` (C2 `mapped`, no body) + its C2 callee `FUN_00414300` — 36 calls.
> 3. Mode 7 (49 calls, **car 1's entire carrier**) is **structurally blocked** until the
>    standalone owns a world-object list: `FUN_00414c30` iterates `FUN_00484c70` objects.
> 4. U-9185's item (b) — separate the two port-side heading candidates
>    (`TrackRenderer.cpp:3327` physics yaw vs the `(cos, sin)` reconstruction at `:3729`) at
>    matched position on car 2 — is untouched and still open.
>
> Trackers: **U-9186** filed, carrying the three sources. **U-9185** amended, keeping only
> item (b). Read `verify/d3_elim_20261003/RESULT_STEP1.md` and `RESULT_STEP2.md`.
>
> **One gate failed in each step and both are stated prominently rather than re-thresholded:**
> `PREREG_STEP2.md` §2.0 (above) and `PREREG_STEP2B.md`'s **G2B-LIVE**, whose registered
> *"≥ 99 % of mode-7 calls carry `c4 == 0x40`"* measures **61.3 %**; the exact relation is the
> converse, **49/49** on four runs.

The block below is the PREVIOUS session's headline, left as history.

> **USER DECISION 2026-10-03 (Mariano) — the decision the block below left open is MADE:
> KEEP the AI start boost.**
>
> `mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:703-707` stays in the default
> build, because it reproduces the original's launch: **(e) is MET 3/3, within 0.05 %** of
> the original's own 1425.7 / 2052.5 / 2055.0. Removing it is **rejected** — that trades a
> MET criterion for a still-NOT-MET one. `MASHED_NO_START_BOOST` remains a measurement knob
> only.
>
> The work that follows from it, in order:
> 1. **Control the player-elimination confound** (`race_[0].alive` differing at
>    `rt = 1.8667 s`, inside the scored window) so that every (b)/(e) number is single-cause.
> 2. **Attack the surviving +15..31 % AI over-speed** with the boost ON — the only route
>    that does not trade (e) against (b). Counterfactually it moves car 1's `c1_distinct`
>    80 → 50 and `steer_distinct` 86 → 61, both into band. Car 2's residual is `err`, not
>    speed.
>
> Recorded in `ROADMAP.md` §D3 ("D3 USER DECISION 2026-10-03"). No band moved, no C-level
> moved.

> **UPDATE 2026-10-03 (D3 START-BOOST A/B session, commits `2b3e2f47` .. `c79a3619`):
> the `MASHED_NO_START_BOOST=1` A/B that the START-HERE block below asked for is DONE.
> It did NOT close (b) and it REGRESSED (e) 6/6. There is now a USER DECISION open.**
>
> Arm A = default, arm B = `MASHED_NO_START_BOOST=1`, three repeats each, every repeat
> digit-identical, no failed boot, recipe/scorers/bands unchanged and both band files
> proven unedited. Fresh arm A reproduces `r1/r2/r3` on every printed digit.
>
> - **(b) 13 → 6 failing bands.** Car 1 5→4, car 2 4→2, **car 3 4→0 (passes outright)**.
>   `c1_median` 52.5/38.0/49.0 → **0.0** and `abs_steer_median` 52.5/57.5/49.0 →
>   **6.0/21.0/9.0**, all in band on all three cars. **But car 1 gains TWO NEW failing
>   bands**, `accel_distinct` and `brake_distinct`: it takes only `c4 = 255` and only
>   `c5 = 0` over the window. **(b) is still NOT MET on 2 of 3 cars.**
> - **(e) regresses 6/6, −46 % to −90 %.** `launch` 1426.4/2053.0/2055.2 → **200.5** on
>   every car, against the **ORIGINAL's own** 1425.7/2052.5/2055.0. **The original
>   demonstrably HAS a launch and arm A reproduces it to 0.05 %** — removing the seed
>   removes the port's only reproduction of a real original behaviour.
> - **Window speed +41/+40/+34 % → +31/+21/+15 %.** The seed is worth about HALF the
>   over-speed. A sustained **+15..31 %** survives the knob and is a **separate carrier**.
> - **The knob is proven live in flight**, not inferred: slots 1..3 read
>   `g_startBoosted` 0→1 with `+0xbf8 == 1` and `+0xbf4` decaying
>   **1100/900/700/500/300/100**; arm B never seeds them; **slot 0 is never seeded in
>   either arm**. `re/tools/sa_boostwatch.py`, `ReadProcessMemory`, no injection.
> - **Player physics is BIT-IDENTICAL between arms** over 4198 `player_trace` lines —
>   **no D2 REOPEN CANDIDATE on player physics.** One confound IS filed: `race_[0].alive`
>   differs at `rt = 1.8667 s`, **inside** the scored window, so the arm A vs arm B deltas
>   are the seed PLUS that divergence and this step did **not** separate them.
>
> Read [`verify/d3_noboost_20261003/RESULT.md`](../verify/d3_noboost_20261003/RESULT.md).
> **One gate was replaced and is stated prominently there**: the pre-registered base
> self-check `record[v] + 0x000 == v` is FALSE in the port and voided the first witness
> run; it was replaced by three stronger legs.

The block below is the PREVIOUS session's headline, left as history.

> **UPDATE 2026-10-02 (D3 OFFLINE session, commits `31fa2fe3` .. `077fc43c`): both
> START-HERE measurements below are DONE. `U-9183` and `U-9182` are RESOLVED, every
> pre-registered gate PASSED, and the recommended next target has CHANGED.**
>
> - **`curv` is faithful.** Position-matched, median `|dcurv|` is **0.0197 / 0.0131 /
>   0.0296** deg on n = 105 / 68 / 104. The 2-8x was a position artefact, on top of a
>   conditioning artefact (`mode == 0` selects the original's low-curvature calls; the
>   FULL-window `curv` medians are **94.14 / 11.40 / 38.28** original vs **52.04 / 52.81
>   / 50.08** port, and on car 1 the ORIGINAL is higher). **Do not port the curvature
>   chain, and `SelectSpline` is not a lead** — the original's `spline` argument equals
>   `0x801aa0 + ai_spline_idx*0x204` on 5463 of 5463 rows.
> - **`c0`'s arithmetic is bit-faithful** (recompute matches the logged byte **586/586**
>   on the port, **148/151** on the original). `c0_distinct` comes from ONE branch, MAG
>   `0x00416697`; everything else contributes only `0`. Car 1 split: **MAG 72 vs 24**,
>   MAGHI 62 vs 171, DEAD 54 vs 22, CTR1 32 vs 3, FF30 **0 vs 0**, CTRHI **0 vs 0**.
> - **What is left is a SUB-DEGREE residual across a HARD split.** At matched position the
>   median `|d signed err|` is only **0.945 / 1.296 / 0.466** deg, but the LO/HI split is a
>   discontinuity at `err = 0/360` and `|err|` is a 1-2 deg oscillation about it, so that
>   bias flips the band on **43 % / 24 % / 13 %** of matched calls. On car 1 it persists at
>   MATCHED SPEED (2606.0 vs 2606.8). New row **U-9185** carries it.
>
> Read [`re/analysis/D3_B_OFFLINE_2026-10-02.md`](analysis/D3_B_OFFLINE_2026-10-02.md)
> and [`verify/d3_offline_20261002/RESULT.md`](../verify/d3_offline_20261002/RESULT.md).
> **One gate was replaced, declared BEFORE the run** (`PREREG.md` A.0): the known-answer
> check could not run offline because the spline point array is runtime memory.

The paragraph below is the PREVIOUS session's headline, left as history.

> **D3 does NOT close, and the 2026-09-29 closure path is REFUTED by its own
> pre-registered gate.** Porting behaviour modes 3 and 7 would not close AI criterion (b),
> so **the port was not written**. (b) is now **decomposed into four measured carriers**
> rather than attributed to one cause, and the **dual-copy hypothesis turned out to be the
> same hypothesis**, already measured.

Read, in order and none of it long:
[`verify/d3_rebase_20261002/RESULT_STEP1.md`](../verify/d3_rebase_20261002/RESULT_STEP1.md),
[`verify/d3_modes37_20261002/RESULT_STEP2.md`](../verify/d3_modes37_20261002/RESULT_STEP2.md),
[`.../RESULT_STEP2B_DUALCOPY.md`](../verify/d3_modes37_20261002/RESULT_STEP2B_DUALCOPY.md).
Pre-registrations `PREREG_STEP1.md` (`867de577`) and `PREREG_STEP2.md` (`a317d25c`), both
committed **unrun**. New rows **U-9182**, **U-9183**, **U-9184**.

## Standing user decisions (recorded 2026-10-02, commit `1cbd4678`)

1. **D2 is PARKED, not closed, and stays re-openable.** 2 of 3 metrics inside their
   unchanged `d81a8df6` bounds; `driving-median` ~1.3 % under with its carrier **not
   identified**. Residuals are `DEFERRED.md` **D-11071**. **Re-pickup trigger:** any
   finding touching `+0x4a4`, the contact collector / `FUN_00538c80`, grip-clamp #6
   (`0x004687f0..0x0046897b`), the substep/chunk loop (`0x00470c70`), `ReassertContacts`,
   or player-car speed on Training → report a **"D2 REOPEN CANDIDATE"** row and do not
   change D2 code in that session.
2. **D3 is UNBLOCKED.**

## What is MEASURED and must not be re-derived

- **(e) PASSES 3/3** on the post-attempt-20 build: `launch` **1426.4 / 2053.0 / 2055.2**,
  `ft_median_m0` **2550.6 / 2053.0 / 2278.2** (n = 100 / 23 / 39). Bands unchanged.
- **(b) FAILS 3/3**, 13 bands, and **all five failing bands are STEER bands**
  (`c0_distinct`, `c1_distinct`, `steer_distinct`, `c1_median`, `abs_steer_median`).
  **accel, brake and `c0_median` PASS on all three cars.**
- **D2 attempt 20 moved the AI window by EXACTLY ZERO.** `MASHED_D2_BATCHMODE=plane`
  reproduces the default arm on every printed digit of both scorers. **The knob is proved
  live** — the full-CSV diff first differs at AI-step call **455**, 235 calls after the
  window closes (2823 rows of `rec_9e4`, 1765 of `c1`).
- **The port is fully deterministic** on this recipe: `r1`/`r2`/`r3` are byte-identical
  over their whole common prefix. Any `collateral.py` floor derived from such a pair on
  this data is **too generous**; use the direct diff.
- **The window provably runs `ControlStep` (`FUN_00416250`)** — the original's window
  carries `ai_mode == 7` on 80 of car 1's 220 calls, and mode 7 is committed **only** at
  `0x0041642f` inside that function.

## THE OPEN QUESTION — (b), decomposed

| carrier | best evidence | next move |
|---|---|---|
| **`int mode = 0;`** (`Ai/AiStandalone.cpp:844`) — the `0x0041665c` multiplier never switches off. The original's `mode != 0` calls carry `curv` medians **125.8 / 91.3 / 132.9**, i.e. it stops amplifying exactly where curvature is extreme | best single arm on cars **1 and 3**: `abs_steer_median` 52.5→15.0 and 49.0→24.5 | **real, large, NOT sufficient — do not re-test it** |
| ~~**`curv` 2-8x high**~~ — `FUN_00443440` @ `0x004162b0` (`AiStandalone.cpp:837`) | **CLOSED 2026-10-02.** Position-matched median `|dcurv|` **0.0197 / 0.0131 / 0.0296** deg (n = 105 / 68 / 104); full-window medians **94.14 / 11.40 / 38.28** orig vs **52.04 / 52.81 / 50.08** port | **U-9183 RESOLVED — do NOT re-open, and do not port the curvature chain** |
| **speed — SPLIT IN TWO 2026-10-03.** A linear multiplier of steer magnitude via `m = err*speed*0.0030034` (`0x00416656`) | **(i) the start seed** (`VehiclePhysicsRun.cpp:703-707`) is worth about HALF: removing it takes the window medians from **3421.7 / 3346.2 / 3495.9** (+41/+40/+34 %) to **3169.4 / 2904.7 / 3011.0** (+31/+21/+15 %) vs the original's **2419.5 / 2397.0 / 2617.6**. **(ii) the remaining +15..31 % is a DIFFERENT, UNIDENTIFIED carrier** | **(i) MEASURED — the A/B is DONE, see the headline; removing it costs (e) 6/6. (ii) is now the open speed lead. U-9185** |
| **sub-degree steering-error bias across a HARD split** — `SteerAngleErrorFwd` `0x00416596`; LO/HI split at `err = 0/360` (`0x004165c0` / `0x004166cf`) | at matched position median `|d signed err|` **0.945 / 1.296 / 0.466** deg (target dir 0.28/0.48/0.45, **body heading 0.98/0.91/0.65**); flips the band on **43 / 24 / 13 %** of matched calls, and persists on car 1 at **matched speed** (2606.0 vs 2606.8) | **U-9185** — second, after the start-boost A/B |
| ~~**`c0_distinct = 6`** on car 1~~ | **ANSWERED 2026-10-02.** `c0` comes from ONE branch, MAG `0x00416697`; car 1 split **MAG 72 vs 24**, MAGHI 62 vs 171. The arithmetic reproduces the logged byte **586/586** (port), **148/151** (orig) | **U-9182 RESOLVED — do NOT port the `c0` magnitude path, it is bit-faithful** |

### START HERE — the ONE next move, and it is NOT a port

> The two offline measurements this section used to list are **DONE** (`U-9183` and
> `U-9182`, both RESOLVED 2026-10-02), and **the start-boost A/B below is DONE too**
> (2026-10-03, `verify/d3_noboost_20261003/RESULT.md`). Do not re-run any of the three.
> The old text is kept below the rule as history.

### THE DECISION THAT IS NOW THE USER'S, and nothing should be ported until it is made

The A/B measured a real and **adverse** trade. Three options, none taken:

1. **Keep the seed (status quo).** (e) MET 3/3, (b) 13 failing bands. The AI third stays
   blocked on (b).
2. **Remove the seed.** (b) 6 failing bands with car 3 passing outright, (e) fails 6/6 by
   46-90 %. **This trades a MET criterion for a still-NOT-MET one.**
3. **Keep the seed and attack the surviving +15..31 % over-speed instead.** Per the
   counterfactual matrix (`ai_band_sim.py`, validation 220/220 = 1.000 on all three cars)
   closing it would move car 1's `c1_distinct` 80→50 and `steer_distinct` 86→61 **into
   band**. **The only option that does not trade one criterion against the other**, and it
   is not costed yet.

### THE NEXT MOVE, if the user wants more measurement before deciding

**1. Separate the seed from the player-elimination confound. Cheapest thing left.**
With the seed ON the player is eliminated at `rt = 1.8667 s`, **inside** the scored window
(`race_[0].alive` 0 vs 1, written at `TrackRenderer.cpp:4664` via `RE::SegmentCheck` +
`EliminationCheck`, which read the **AI cars'** positions). `round_mode_` is true,
`g_aib.alive[0]` is cleared at `TrackRenderer.cpp:3722`, and the AI tick loop at
`AiStandalone.cpp:1708` runs `for (v = 0; v < 4; ++v)` gated on `car_alive(v)` — so
`VehicleStep(0)` runs in arm B and does not in arm A. **Until that is controlled, no arm A
vs arm B delta is single-cause.**

**2. Then the body heading, and car 2 is the car to probe.** It is still the larger median
share on all three cars in both arms, and on **car 2** it **GROWS** 0.9085 → **1.6059** deg
when the seed is removed — car 2 is also the car whose remaining (b) failure the
counterfactual attributes to `err`, not speed. The two separable candidates are unchanged
(see item 2 of the history block below).

**3. Separately: find what carries the +15..31 % over-speed that survives the knob.** It is
**not** the seed. This is option 3 above.

<details><summary>History: the start-boost A/B as it was briefed, now DONE</summary>

**1. Run the start-boost A/B first. One environment variable, no code change.**
`VehiclePhysicsRun.cpp:703-707` applies a **port-only start boost to `slot != 0`, i.e. to
exactly the cars criterion (b) scores** — `+0xbf8 = 1`, `+0xbf4 = 1300`, guarded by
`MASHED_NO_START_BOOST`. The port's window speed is **3421.7 / 3346.2 / 3495.9** against
the original's **2419.5 / 2397.0 / 2617.6** (**+41 / +40 / +34 %**), and
`m = err*speed*0.0030034` (`0x00416656`) makes speed a linear multiplier on the steer byte
while the extra distance travelled pushes the port into higher-curvature track inside the
same 220 calls. Suspect the port-only scaffold before re-suspecting a byte-faithful
transcription.

> Re-capture the port side with `MASHED_NO_START_BOOST=1` and re-score **BOTH (b) and
> (e)**. (e) passes today **with** the boost, so the seed cannot be removed on (b)'s
> evidence alone — if (e) regresses, that is a trade-off for the user, **not** a fix.
> Pre-register the decision rule before the run, as every D3 session has.

**2. Only if that does not close (b): the AI cars' body heading.** It is the larger median
share of the matched-position residual (**0.9796 / 0.9085 / 0.6506** deg). It reaches
`SteerAngleErrorFwd` as `(cos(a.yaw), sin(a.yaw))` from `TrackRenderer.cpp:3729`, with
`a.yaw` round-tripped through `Vehicle::VehiclePhysics_StepCar` (`:3315` in, `:3327` out)
— **not** from `rec+0x9d4`/`+0x9dc`, which is what `AiStandalone.cpp:939`'s comment claims
and which nothing in the standalone reads. Two separable candidates: the physics' own yaw
(a D2 surface measured through the AI) and the scalar reconstruction standing in for the
record's forward basis row (a port-only bridge, cheaper to test). The
`yerr * (6.0f*dt)` turn-rate limiter at `TrackRenderer.cpp:3416` / `:3676` is **not** a
suspect — it is in the legacy "AI v2" `else` branch, which the ported path does not take.

**3. `FUN_00414c30` + producer chain is RULED OUT for this carrier.** Its effect on (b) is
through `ai_mode`, already refuted as sufficient by the modes-3/7 arms, and `curv` and
`c0`'s arithmetic are now proved faithful — so that chain cannot reach what is left.

</details>

**One question for the USER, deliberately not decided.** Criterion (b)'s `c0_distinct`,
`c1_distinct` and `steer_distinct` count which side of a hard discontinuity a 1-2 deg
oscillation lands on; a faithful port can fail them. Whether to re-specify (b) on a
band-invariant statistic is a ROADMAP decision. **No band was moved.**

<details><summary>History: the two offline measurements, now both DONE</summary>


1. **U-9183, position-matched curvature.** The index-matched comparison above is **partly
   circular**: the two sides traverse at different speeds, so at the same within-window
   call index they are at different track positions, and curvature is a property of
   position. Re-bin both sides by `look_idx` / `look_blk` / `ai_spline_idx` (all three are
   already columns in the committed captures) and compare **within bin**. Only if the gap
   survives is `FUN_00443440` a defect — and then `0x00418560` (`SelectSpline`, demoted
   C3→C2 with three missing arms and a missing spline-index reset) is the first suspect.
2. **U-9182, the branch split.** `c0` is written at `0x00416697` (low-err branch) and
   `0x004167b1` (counter-steer, high branch). The decode rule is already in
   `ai_band_sim.py:63-71` (`hist_d8 == 360` → low, `hist_dc == 0` → high), so count the
   per-car split on **both sides from existing captures first**. Only hook the original if
   that is inconclusive.

**Do both before spending a session on the `FUN_00414c30` + producer-chain port.** This
session's evidence says that port would not close (b) on its own.


</details>

### If you do write the modes port, the scope is already measured

`FUN_00484c70` is **already ported and C3 with a GREEN Frida diff** — but in the
**asi-only** `Util/PromoLoop_round20.cpp`, so `mashed_re.exe` has no copy. `DAT_006e70d8`
is written only by `FUN_00484c90` (`0x00484cd4`) and `FUN_00485070` (`0x0048508a`);
`DAT_006dccb8` by `FUN_00484c90` (`0x00484ca4`), indexed by the registrar `FUN_00484cf0`
(`0x00484d2d`/`0x00484d35`), which has **13 callers**, most in the already-ported power-up
range including the dispatcher `0x0045bba0`.
**Porting `FUN_00484c70` alone is INERT** — both globals are `.bss` zeros, the count is 0,
`FUN_00414c30`'s loop never runs, no mode is ever set. **Seeding them is not a port.**
`FUN_00414c30` (704 bytes, `0x00414c30..0x00414ef0`) has no port at all; the only
reference in `mashedmod/src` is the asi-side call-through at `AiControlStep.cpp:92`, which
jumps into `MASHED.exe` and cannot work in the standalone.

### Dead ends — do not re-open

- **`rate1` pinned `0.0f`** (`0x00416a30` `:983`, `0x00417da0` `:1102`) and the
  **velocity-derived heading** (`0x00415e20` `:175`) are **not reachable on this recipe**.
  They live in `ControlStepM49`/`M8`, selected only when `fd0 ∈ {4,8,9}`, and `ControlStep`
  calls the **correct** body-forward `SteerAngleErrorFwd` at `:858`. `U-9184` records this
  and corrects `DUAL_COPY_FIX_2026-09-29.md:177-180`, whose claim that (b) "runs on" those
  two is wrong. **The demotions themselves stand.**
- **`0x00443080`'s exe literal** is harmless: `tgt_7ffc` is within the noise floor in every
  shared band, on top of the committed 6259/6259.
- **Attempt 20 as a cause of any (b) number.** Measured at exactly zero, with a live-knob
  positive control.

## Recipes (unchanged, copy-paste)

```
# standalone (b)/(e) capture
py -3.12 re/tools/sa_capture.py verify/<dir>/<tag> 8,30,60 MASHED_MUTE=1 \
    MASHED_TRACK_VIEW=Training MASHED_CAR=1 MASHED_ROUND=1 \
    MASHED_WIN_POS=primary-bl MASHED_TITLE="<label>" \
    MASHED_AI_STEPDUMP=verify/<dir>/<tag>.csv
# original (b)/(e) capture -- one run covers all three AI cars
py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --car 0 \
    --poke-ctrl-slots --statediff-out verify/<dir>/o1.msd \
    --statediff-car 1 --statediff-aistep --hold 60
# scorers (BANDS ARE NOT TO BE MOVED)
py -3.12 re/tools/ai_ctrl_window.py --check <csv>      # (b)
py -3.12 re/tools/ai_speed_env.py   --check <csv>      # (e)
# new this session, read-only
py -3.12 re/tools/ai_mode_split.py <csv>               # G2-MODE / G2-JOINT / G2-STEER
py -3.12 re/tools/ai_band_sim.py --orig <o.aistep.csv> --sa <sa.csv> --out <report.txt>
#   now carries modeseq + curvseq counterfactual arms; its >=95% validation gate stands
py -3.12 re/tools/ai_posmatch.py --orig <o.aistep.csv> --port <r.csv> --part A|B|COLL
#   NEW 2026-10-02. Implements verify/d3_offline_20261002/PREREG.md verbatim: the
#   position-matched curvature test (A), the c0 branch split with both known-answer
#   checks (B), and a DESCRIPTIVE collateral/err-decomposition leg (COLL, no pass/fail).
#   Its constants and bands are pre-registered and ARE NOT TO BE MOVED.
# guards
pwsh -NoProfile -File re/tools/pu_replay/sweep.ps1
py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --car 0 \
    --poke-ctrl-slots --oracle --rule 3 --hold 50
```

## Standing rules

Launch muted; `MASHED_TITLE` on every run; `MASHED_WIN_POS=primary-bl`;
`--poke-ctrl-slots` on race captures; **never** `MASHED_NAV_DEMO`. Track PIDs, kill only
yours. Frida **entry hooks only**. Ghidra pool clones `-readOnly`; never write the master.
Never `unlock_*` on `original/MASHED.exe`. Build via `mashedmod\build.bat` from PowerShell.
Trackers only through `re-classify`, preserving each file's line endings — `DEFERRED.md`,
`UNCERTAINTIES.md`, `hooks.csv`, `STUBS.md` are **CRLF**; `ROADMAP.md`,
`re/analysis/CHANGELOG.md` and this file are **LF**. Pre-register every gate; if one fails,
STOP, and state prominently any gate or rule you replace.

## Still open

- **D3 (b) is still NOT MET** and is the phase's sole blocker. `U-9183` and `U-9182` are
  RESOLVED; **`U-9185`** now carries the remainder (the start-boost scaffold, the
  sub-degree heading residual, the knife-edge band split). `U-9184` stands as filed.
- `[UNCERTAIN]` the `look_z` tail on car 1 (matched-position median **0.7678**, p90
  **5.1128**) — below defect size at the median, but the only matched-position field
  other than speed with a non-trivial tail.
- **D2 parked, D-11071:** `U-9180`, `U-9181`. Two informational **D2 WATCH** rows were
  filed 2026-10-02 (`D3_B_OFFLINE_2026-10-02.md` §6): AI-slot speed is **+34..41 %** on
  track 0 / mode 10 with the start-boost scaffold in place (D-11071's parked metric was
  the **player** 1.3 % **short** on Training — opposite sign, different car class,
  different scenario, so it neither confirms nor refutes that carrier), and the body
  heading is a physics output. **No D2 code was read or changed.**
