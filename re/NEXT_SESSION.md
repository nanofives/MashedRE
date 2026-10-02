# Next session kickoff

Updated 2026-10-02 at the close of the **D2 re-close attempt 20** session. **U-9179 is
RESOLVED, its producer is FIXED, and the sink is gone.** `wcs_drift` fires **0** times (was
47), `0x0046f6c0`'s share of `T_post` is **-0.00 %** (was 84.33 %), `T_post` is **inside
both of attempt 18's bars for the first time in the D2 re-open**, recovery **H1 PASSES both
legs** (was FAIL on the median), and **2 of the 3 D2 metrics are inside their unchanged
`d81a8df6` bounds** — the 2000-2600 slip band being scored at all for the first time. The
transcribed integer substep loop was also ported (STEP 5, all three of its gates passed), so
the port now runs **2** substeps per frame like the original. The third metric,
`driving-median`, is **1.3 % below its lower bound**, so **D2 still does NOT close**. New
rows **U-9180** and **U-9181**. Branch `race/first-frame-parity`. Nothing is pushed.

> ## START HERE (D2 lane) — attempt 20. **The sink is closed. What is left is a 1.3 % gap on ONE metric, and its carrier is NOT identified — do not assume it is grip-clamp #6.**
>
> Read [`verify/d2_wheelstate_20261002/RESULT.md`](../verify/d2_wheelstate_20261002/RESULT.md)
> — all of it, it is short — then `RESULT_STEP2.md` §3-§4 for the producer chain if you need
> it. Pre-registrations `PREREG_STEP2.md` (`49bf17c9`), `PREREG_STEP34.md` (`35a4c7b4`),
> `PREREG_STEP5.md` (`052a678a`), all committed unrun. `UNCERTAINTIES.md` **U-9180** is the
> new blocker; **U-9179 / U-9178 / U-9160 are RESOLVED — do not re-open or re-derive them.**
>
> ### WHAT CHANGED, AND WHY IT IS NOT RE-LITIGABLE
>
> `ProduceTerrainBatch`'s admission test (`ContactProducer.cpp:67`) was a **plane-distance**
> test. A plane is unbounded, so it was never a locality test: on a coplanar track it
> admitted ground triangles from anywhere on the surface, the 256-entry store **saturated on
> 3565 of 3565 solver calls**, the loop stopped scanning, and the triangles under wheels 0
> and 1 never reached the classifier `0x0046cc40`. Their `key` (`+0x1ec`) therefore stayed at
> the init-loop `-1`, `0x0046f6c0`'s state machine demoted them at `0x0046f91a`/`0x0046f91f`
> (**arm `A-demote-key`, 99 of 356 wheel-rows**), `bVar16` became 2, the `0 < count <= 2.0`
> gate at `0x004701e8` opened, and the airborne drift fired 47 times in 29 frames.
> **The ORIGINAL has no cap** — `LAB_00468b80` increments `DAT_0088e60c` unconditionally at
> `0x00468d6c..0x00468d73`, no bound test anywhere in `0x00468b80..0x00468d7c`.
> The test is now **spatial** (AABB(tri) grown by the same `radius`), **no new constant**.
> `MASHED_D2_BATCHMODE=plane` is the **diagnostic pre-fix arm** and it reproduces attempt 19
> **bit-for-bit** (0.1983 n=19 / n=0 / 1019.77 n=76), so every A/B here has a verified
> baseline.
>
> ### THE CURRENT SCOREBOARD (3 runs, identical slip values; UNCHANGED `d81a8df6` bounds)
>
> | metric | bound | PORT | n | median `d` | ORIG | verdict |
> |---|---|---:|---:|---:|---:|---|
> | slip 1500-2000 | 0.18855..0.19635 | **0.1943** | 353 | 605 | 0.1937 (n=314, `d` 720) | **PASS** |
> | slip 2000-2600 | 0.24488..0.25487 | **0.2524** | 557-562 | 718 | 0.2498 (n=540, `d` 909) | **PASS** |
> | driving-median | 1904.70..1982.44 | **1852.66 / 1861.43 / 1854.65** | 1367/1340/1364 | 637 | 1937.89 (n=1154, `d` 824) | **FAIL ~1.3 %** |
>
> `T_post` at `d = 222..250`: ORIG **-0.00464**, PORT **-0.61485**, `|delta| 0.61021`,
> bars 5.5560 / 8.6373 — **inside both**. launch `L = 0` at **0.19 %**. recovery
> **398/400 = 99.5 %, median 1362.7** against the original's 398/400 and 1333.9.
>
> ### NEXT COMMAND — U-9180
>
> 1. **Do NOT re-run the old split window.** `d = 222..250` was chosen for a sink that no
>    longer exists, and `0x0046f6c0` is exactly 0.000000 there now. Re-run
>    `a19_split.py` on the post-fix build with a window **inside the scored population**
>    (median `d` ~**637**), because that is where `driving-median` is measured.
> 2. **Check the bands' median frame indices FIRST** (§26.10): PORT 605 / 718 / 637 against
>    ORIG 720 / 909 / 824 is the same regime but **not the same moment**, so any magnitude
>    read across them needs that stated (memory
>    `a-band-scored-off-regime-is-not-a-measurement`).
> 3. **Do NOT assume grip-clamp #6 is the carrier.** It is the only non-zero `T_post`
>    producer left, at **-0.612549**, which is 1/45th of attempt 19's sink and too small on
>    its own to be a 25-unit speed gap. Attempt 18 already **refuted** `k`/`l_60` as the
>    carrier by two independent routes and attempt 19 reproduced that refutation. Measure
>    before naming.
> 4. **U-9181 first, it is cheap and it is upstream of the new test.** `+0x4a4` reads
>    **0.67804** on the original and **692.302** on the port in every band, identical pre-
>    and post-fix. The port's broadphase is being handed a radius three orders of magnitude
>    larger than the original's, and that radius now drives the admission test. Start with
>    `py -3.12 re/tools/dispsweep.py 0x4a4`, then read the original's `+0x4a4` live — the
>    existing `scenario_launch.py --wheelstate-probe` already samples `0x0046f6c0`'s entry
>    and needs one extra column.
> 5. **The outer chunk loop `0x00471143..0x00471151` is still unported.** At `dt = 1/60`
>    `remI == 50` so it iterates once and is indistinguishable; it is `[UNCERTAIN]` only for
>    `dt` making `remI > 50`. Not a D2 lever on this arm.
> 6. **`ReassertContacts` is PART port-only.** Attempt 19 recorded it as the original's tail
>    `0x0047044b..0x004704b0`; measured this attempt, only the **counting** half is faithful.
>    The promotion of state-0 wheels back to 1 at `VehiclePhysicsRun.cpp:347` and the normal
>    writes at `:350-352` have **no counterpart in the original**. That is why the KA-P
>    entry-pair gate fails on the port (67.2957 %) while KA-O reproduces the running
>    original's own states on **4686 of 4687** pairs. If you touch the wheel states, this is
>    the thing that will confuse your replay.
> 7. **AI slots 1+ are in the fix's blast radius and were NOT measured.**
>    `ProduceTerrainBatch` is called per car at `VehiclePhysicsRun.cpp:1007`, so AI cars'
>    contact batches changed too. Every attempt-20 run was `participants=1`, so the magnitude
>    is **[UNCERTAIN]**. `VehiclePhysicsRun.cpp:702`'s fitted seed was not touched and nothing
>    was tuned. D3 must re-measure its modes 3/7 baseline against the post-fix build.
> 8. **Retry a port boot before believing a crash.** Two of this session's port boots failed
>    transiently (`speed=0.00`, early demo exit, no `motion_diag.log`) and the identical
>    control succeeded on the next boot. Registered in `PREREG_STEP34.md` §3; without it two
>    diagnostic arms would have been misrecorded as knob-induced crashes.
>
> ### TOOLS THIS ATTEMPT ADDED
>
> `re/frida/scenario_launch.py --wheelstate-probe` / `--wheelstate-probe-count` (entry hooks
> only on `0x0046f6c0` + the A6a frame marker; per-wheel `state` `+0x198`, `fv` `+0x194`,
> `key` `+0x1ec`, stride `0xc4`; writes `<out>.wheelstate.csv`; the count-first arm measured
> 2.012 calls/frame and confirmed `EDI == record` rather than assuming it),
> `re/tools/statediff/a20_wheelstate.py` (the transcribed state-machine replay + gates KA-O /
> KA-P / CV / EV and the decision rule), the `MASHED_D2SINK_SM` channel (`wcs_ent`, `wcs_sm`,
> `wcs_cls` — the last one is the classifier's per-wheel **rejection stage**), and the
> default-OFF `MASHED_D2_BATCHMODE` A/B (`plane` / `planefull` / `local`). All exe-only.
>
> ### GATES THAT FAILED, ALL REPORTED AS FAILURES
>
> **KA-P entry-pair 67.2957 %** (cause measured: `ReassertContacts`'s promotion — item 6).
> **KA3 52.7060 %** (the `%g` six-digit channel limit attempt 19 already diagnosed; forms no
> producer delta). **No decision rule was replaced.** One rule was **added** mid-attempt and
> is named in `RESULT.md` §8: the boot-retry rule in item 8.
>
> **Still open:** **U-9180** (D2's only remaining metric failure); **U-9181**; U-9177; U-9176;
> U-9156; U-9171; §20.14's `-0.1` duty cycle; D1-residue R1; the outer chunk loop.
>
> **The D3 modes 3/7 hold stands. D2 must close before it starts — and its baseline now has
> to be re-measured against the post-fix build.**
