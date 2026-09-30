# Next session kickoff

Updated 2026-09-30 at the close of the **D2 re-close attempt 9** session (an audit of attempt 8
rather than an extension of it: a blind spot in attempt 8's own instrument was found and fixed,
which STRENGTHENED one of its claims and WITHDREW another; the cadence lane was refuted by its
own pre-registered rule; and a new fidelity defect was banked as measured-inert).
Branch `race/first-frame-parity`. Nothing is pushed.
Superseded kickoffs: the attempt-8, attempt-7, attempt-6, attempt-5 and attempt-4 ones, kept below.

## RENDER LANE — two defects fixed 2026-09-30: (a) grey chassis and (d) car brightness. Nothing owed to pick either up.

Separate lane from the D2 block below; both live on this branch.

### (a) grey car chassis — FIX LANDED 2026-09-30 (`5ddc0384`)

**Do not re-derive the diagnosis** — read
[`re/analysis/CAR_GRAY_CHASSIS_2026-09-29.md`](analysis/CAR_GRAY_CHASSIS_2026-09-29.md) for the
root cause and [`re/analysis/CAR_GRAY_FIX_ACCEPTANCE_2026-09-30.md`](analysis/CAR_GRAY_FIX_ACCEPTANCE_2026-09-30.md)
for the pre-registered rules (`2179ed1a`) and the `# RESULTS` section.

`LoadCar` / `LoadCarLiveries` handed the whole vehicle clump to the wheel heuristic,
`BuildDffBatches` and `RaceSubmit_RegisterModel`, so all 71 `ADVANTAGE0.DFF` atomics were
drawn. 27 are non-render (4 untextured car-sized collision hulls + 23 one-triangle locators,
material `(102,102,102)`). `CarDropNonRenderAtomics` erases every batch with
`(geo_flags & 0x84) == 0` right after `model.Parse`, so one filtered model feeds all three
consumers. `model.bbox` is deliberately not recomputed (it feeds `car_ground_off_` /
`car_len_` / `car_height_`).

* **G1/G2/G3 PASS, G4b/G4c PASS.** Car-box grey fraction **0.8487 → 0.0782**; hull tone
  `(102,102,102)` **1999 px → 0**; `kept=44` on all four ADVANTAGE DFFs; `CARLIGHT
  body_batches tot 67 → 40`; `wheels=4` with identical pivots; A2's deck-box dominant
  unchanged at **0.8517**; all nine A4 terrain/sea boxes and both frontend frames
  **bit-identical**.
* **G1d UNMEASURABLE as written** — `MASHED_DBG_DRAWSTREAM3D` emits no `"cars"` record on the
  **default librw build** (the tally lives in `RenderCarsRelit`, `TrackRenderer.cpp:4856`,
  reachable only via the `else if (relit_cars)` arm at `:5705`). Measured on the
  `MASHED_RENDER_LIBRW=0` arm instead and reported separately: cars `batches 284 → 176`,
  `textured 176 → 176`, every other category unchanged. **If you want a camera-invariant
  per-category tally on the default path, that instrument does not exist yet.**
* **G4a FAILS as written** (879 + 22 px outside the declared regions on the two Arctic
  captures). Its region-declaration procedure keys on the same achromatic GREY class the G2
  counter uses, and Arctic's tinted light (`amb (0.2,0.3,0.3)`) pushes the hull to
  `(21,31,31)` / `(55,70,70)`, each missing a threshold by **1**. Every one of the 901 pixels
  is a hull pixel inside a car's single diff component. The edit is **kept**; the reason is
  written down, not implied.

**Open, deliberately not touched:** note items **O3** (the duplicate low/high LOD sets are
still both drawn), **O4** (`MASHED_RPLIGHT=0` renders the car solid black), **O5** (props not
swept for the same over-draw), and the `[UNCERTAIN]` on whether the four hulls should feed our
collision path the way the original's part codes `0x3b..0x3e` feed `FUN_0053d400` @
`0x0053d400`. **U-9079** (the part code is not derivable from the DFF) is what keeps this a
measured equivalent rather than a verbatim port.

### (d) car brightness — FIX LANDED 2026-09-30

**Do not re-derive the diagnosis** — read [`re/analysis/CAR_BRIGHTNESS_2026-09-30.md`](analysis/CAR_BRIGHTNESS_2026-09-30.md)
(`# FIX APPLIED AND ACCEPTANCE RUN` is the newest section).

`ParseLightsDffFaithful` composed the track directional light's world at-vector one frame too
many (seeded from `rot[6..8]`, which is already the parent-space at-axis, then walked from the
light's own frame). Fixed by parent-starting the walk at
`mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:853` and its twin `:751`. One token each.
Both renderers read the same `sun_dir_` (`LibRw/RwRaceSubmit.cpp:571`), so one change covers
librw and the legacy D3D9 path. There is no RVA: the original performs **no** direction
arithmetic at all (`FUN_00479330` @ `0x00479330` adds the LIGHTS.DFF lights with
`RpWorldAddLight` as-is and lets `RwFrameGetLTM` supply the direction).

* **A1 PASS** — 13/13 tracks log the shipped asset's at-vector exactly.
* **A2 PASS** — TRAINING up-facing deck dominant **0.5000 → 0.8517** (n=1329) against the
  arithmetic prediction 0.8522.
* **A4 PASS** — terrain / ice / sky and both frontend frames **bit-identical** pre/post; only
  cars, copters and lit props move.
* **A3 FAIL as written** — floor fraction 70.4% → **12.8%** where the band is 25-40%. Not
  amended. The band's own "from" endpoint (~60-65%) does not reproduce at the measured poses
  either, and closing it needs **O6**: an original-side TRAINING capture at a known heading
  and camera, scored three ways with `surface_split.py`. That capture is the same one **O4**
  needs, and it is a concrete instance of what ROADMAP **D1-residue R1** still owes.

**Harness worth reusing** (in `verify/car_bright_fix_20260930/`, plus the grey-lane additions in
`verify/car_gray_fix_20260930/`: `run_race.py` there also collects `log/mashed_re.log` — a
DIFFERENT file from `./mashed_re.log`, and the one the car-load lines go to — and arms
`MASHED_DBG_DRAWSTREAM3D`; `dff_atomic_census.py` re-derives the per-vehicle atomic split from
the assets; `gray_frac.py` counts hull-grey vs paint): `run_race.py` drives the
standalone's own `MASHED_RACE_DEMO=1 MASHED_GOTO=6` flow — **no external keystrokes, never
takes the foreground**, dumps the real backbuffer, and with `MASHED_DETERMINISTIC=1` gives
pose-identical pre/post pairs so a regression guard is exact rather than jitter-bounded. Two
gotchas found: the frame-counter clock **freezes the car** (heading `-1.57603` on every
capture), so anything needing the car to turn must drop `MASHED_DETERMINISTIC`; and
`MASHED_VERIFY_OUT` must be pointed somewhere private — `verify/race1/` holds nine tracked,
cited stills. `a4_scope.py` (connected components of the differing-pixel mask) is what turns
an `imgdiff` cell grid into a statement about *which surfaces* moved.

---

> ## START HERE (D2 lane) — D2 is STILL REOPENED. Six routes are now CLOSED by measurement. Do not start the next attempt inside A6a or inside `0x0046ef70`.
>
> Read [`re/analysis/D2_REOPEN_2026-09-29.md`](analysis/D2_REOPEN_2026-09-29.md) **§22**
> (§22.4 first, then §22.2, then §22.1/§22.3 for the two rules). **Do not re-derive any of it.**
>
> **The §3 bounds are unchanged and are not renegotiable.** PASS intervals:
> `slip 1500-2000` **0.18855 .. 0.19635**, `slip 2000-2600` **0.24488 .. 0.25487**,
> `driving-median` **1904.70 .. 1982.44**. The arm is still §16.7
> (`MASHED_STEER_HOLD_AFTER=0`), `MASHED_MEASURE_SOLO=1`, `MASHED_TRACK_SEL=12`.
>
> ### Scored at HEAD, 3 of 3 — a NO-CHANGE control (attempts 8 and 9 added no physics)
>
> | metric | port | n | median speed | interval | verdict |
> |---|---:|---:|---:|---|---|
> | slip 1500-2000 | **0.2033** | 20 | 1683.53 (in band) | 0.18855..0.19635 | **FAIL** +3.4% |
> | slip 2000-2600 | **—** | 0 | — | 0.24488..0.25487 | **UNSCORABLE** |
> | driving-median | **1355.66** | 54 | 1355.66 | 1904.70..1982.44 | **FAIL** -30.2% |
>
> Whole-window median horizontal speed **26.36**, 1080/1080 grounded. `allowlisted=122 NEW=0`.
>
> ### PICK UP HERE — read the CLOSED list first, because it is most of the map now
>
> **SIX routes are closed by measurement. Re-opening any of them is wasted effort:**
> 1. **`0x0046ef70`'s impulse** — `local_54` = `(2417.77, -324.51, -141.34)` original against
>    `(2424.47, -325.4, -141.19)` port = **0.28% / 0.27% / 0.11%** (§22.2). The slot producer
>    agrees to 4-5 digits on arm, scale, normal and magnitude.
> 2. **the substep velocity chain, on BOTH contact and non-contact substeps.** The ORIGINAL is
>    bitwise unchanged across `0x0046f6c0` on 2945/2945, `0x00469aa0` on 2945/2945 and
>    `0x004709a0` on 2932/2932; the PORT's three `WheelContactSolver` write sites fire **0
>    times in 4000 substeps including all 160 that contact**, `vTop == vPostWheel` 160/160
>    (§22.2 + §22.4). Both sides write `+0x9b0` in exactly two places per frame.
> 3. **`+0x9e4`'s write order** — `0x004686cc` precedes grip-clamp #6 at `0x004687f0` on the
>    original too (capstone, 1243 instructions, reached `0x00468989`), so the port's
>    `+0x9e4/|velocity|` of **1.017098** against the original's **1.000000** is the port
>    clamp's own excess bleed read out downstream. Circular (§22.2).
> 4. **grip-clamp #6** — byte-faithful, both arms, six constants, two floors (§21.5).
> 5. **the `l_60` / `ld4` lane** — measured out; every factor agrees cross-side or is
>    slip-coupled, and the one independent defect is worth 10.9% (§21.10).
> 6. **the contact CADENCE** — **REFUTED** (§22.4, U-9159 struck). Inter-contact interval over
>    the ten contacts after each side's own first bounce: ORIGINAL **median 12**
>    (19,13,12,12,12,13,12,12,13) against PORT **median 10** (21,12,12,12,10,9,8,8,8,8),
>    ratio **0.833** against the pre-registered 0.5 floor. 1 fixup per contacting frame on
>    **160 of 160** — no doubling. §22.2's "19 frames against 1-2" was an inference and is
>    **WITHDRAWN**.
>
> **WHAT IS OPEN, stated as precisely as it can be.** The damp at `0x0046f5ba`/`0x0046f5c0` has
> a knee at `|m|/+0x9e4 = 0.7`, saturating at the `0.9` cap. **The original is on that cap on
> 15 of 23 fixups (65.2%); the port on 0 of 211 (0.0%).** With the cadence now known to AGREE,
> the whole difference is the speed trajectory through the post-bounce contact train:
>
> | | gains | median per gap | net over 10 contacts |
> |---|---:|---:|---:|
> | ORIGINAL | 7 of 9 | **+17.84** | **+338.74** (219.89 -> 405.82) |
> | PORT | 2 of 9 | **-17.84** | **-105.00** (251.48 -> 42.13) |
>
> **Gap 0 is the like-for-like one and it is the tightest number in the lane:** matched speed
> (219.89 against 251.48), matched free flight (19 frames against 21), full throttle on both,
> and the original nets **+11.63** while the port nets **-14.54** — a 26-unit swing. The port
> then locks on a fixed point at `|cos| ~ 0.827`, damp `~0.55`, speed `~55`, which is §21.5's
> 40-70 residency band.
>
> **[UNCERTAIN U-9156] That is a statement of the residual, not a term.** "The port loses speed
> between contacts where the original gains" is the `driving-median` failure restated in the
> post-bounce regime, and its mechanism is the bleed §21.2 measured at 100x and §21.10 then
> measured out from inside the loop. **So do NOT open the next attempt inside A6a or inside
> `0x0046ef70`** — both are exhausted. What has never been measured cross-side is the
> BETWEEN-CONTACT longitudinal dynamics as its own budget: the drive force actually applied per
> frame against the drag/grip actually removed, at matched speed, over gap 0's 19-21 frames.
> §20.10's "no drive-force deficit" was withdrawn and re-measured in a different band
> (100-200, gains-only), so it does **not** cover this.
>
> ### Banked, real, and measured NOT to be on the trap's path — U-9160
> The port runs **3** substeps per frame and **4** on a contacting frame, against the original's
> fixed **2**. `dt` over 4000 substeps: **2720 at `25.000000`, 1280 at `0.000004`**; the residue
> is `3.8146973e-06`, exactly `frameMs = (1.0f/60.0f)*3000.0f = 50.000004f` minus two 25s
> (`VehiclePhysicsRun.cpp:891`). Original: `0x00469ad4 mov ebx,2`, measured `4662/2331` and
> `2932/1466`. **The residue pass contacts 0 of 1280 and writes velocity 0 of 1280.** Target
> invariant if it is fixed: substeps per frame == 2, no contacting-frame exception beyond the
> documented `0x00470ab0` retry.
>
> ### The instrument lesson this attempt paid for — read before trusting any coverage claim
> `MASHED_SUBSTEP_VELPROBE`'s emit sat **after** `if (contacted != 0) continue;`, so it was
> skipped on exactly the substeps that contact, and the log read **`c9ec == 0` on 4000 of
> 4000** while `world_contact.log` from the SAME run held **211 fixups**. A "0 of N" result is
> only evidence if the instrument can be shown to cover the N. Cross-check every coverage
> claim against a second channel from the same run before committing it.
>
> ### Tooling (attempts 8-9, all read-only or default-OFF)
> - `scenario_launch.py --fixup-probe` — entry hooks on `0x0046ef70` (site 0), `0x004709a0` (1),
>   `0x00467650` (2, the FRAME marker), `0x0046f6c0` (3), `0x00469aa0` (4), with a register
>   self-check and the **live 18-slot contact set**. The only way to see the slots: the `.msd`
>   has them key `-1` on 2332 of 2333 frames and `+0x9ec` is 0 on every frame.
> - `MASHED_SUBSTEP_VELPROBE` — **corrected**; per-substep `|velocity|` at four points plus
>   `c9ec`, `contacted`, `fixups` and the three velocity-write counters, emitted from both
>   exits of the retry loop.
> - `re/tools/statediff/a9_bounce.py` — bounce-aligned cross-side comparator (measures its own
>   `player_trace`/`motion_diag` join offset: `md_line = pt_f + 1`, 0 of 1625 mismatches).
> - `re/tools/statediff/a9_fixup.py` — per-contact damp/knee table with the frame-boundary flag.
>
> Artefacts: `verify/d2_bounce_20260930/` and `verify/d2_cadence_20260930/`.
>
> **The D3 modes 3/7 hold stands. D2 must close before it starts.**


> ## SUPERSEDED (attempt 8) — the first diverging term named; the cadence claim in it is WITHDRAWN by attempt 9
>
> Read [`re/analysis/D2_REOPEN_2026-09-29.md`](analysis/D2_REOPEN_2026-09-29.md) **§22**
> (§22.2 first, then §22.1 for the rule it was measured under). **Do not re-derive any of it.**
>
> **The §3 bounds are unchanged and are not renegotiable.** PASS intervals:
> `slip 1500-2000` **0.18855 .. 0.19635**, `slip 2000-2600` **0.24488 .. 0.25487**,
> `driving-median` **1904.70 .. 1982.44**. The arm is still §16.7
> (`MASHED_STEER_HOLD_AFTER=0`), `MASHED_MEASURE_SOLO=1`, `MASHED_TRACK_SEL=12`.
>
> ### Scored at HEAD, 3 of 3, and it is a NO-CHANGE control (attempt 8 added no physics)
>
> | metric | port | n | median speed | interval | verdict |
> |---|---:|---:|---:|---|---|
> | slip 1500-2000 | **0.2033** | 20 | 1683.53 (in band) | 0.18855..0.19635 | **FAIL** +3.4% past the bound |
> | slip 2000-2600 | **—** | 0 | — | 0.24488..0.25487 | **UNSCORABLE** |
> | driving-median | **1355.66** | 54 | 1355.66 | 1904.70..1982.44 | **FAIL** -30.2% |
>
> Whole-window median horizontal speed **26.36**, 1080/1080 grounded. `allowlisted=122 NEW=0`.
>
> ### PICK UP HERE — the CONTACT CADENCE. The port asks the wall every 1-2 frames; the original every 19.
>
> **What §22.2 established, all measured, none of it to be redone:**
>
> 1. **The first diverging term is `d = +0`, channel C4** (post-bounce horizontal speed):
>    ORIGINAL `219.89064` against PORT `251.47744`, `|delta| 31.5868` vs `tol 25.1477`, every
>    earlier channel inside tolerance. **RVA: the last-contact damp `0x0046f5ba` / `0x0046f5c0`
>    inside `VehicleContactFixup` `0x0046ef70`.**
> 2. **That damp has a KNEE and it is the basin.** `|m|/+0x9e4 <= 0.7` saturates the damp at the
>    `0.9` cap (keep 90%); above it retention falls as `3*(1 - |m|/+0x9e4)`. `|m|` is the slot
>    impulse along the wall normal, so the quantity is `|cos(velocity, wall normal)|`.
>    **The ORIGINAL is on that cap on 15 of 23 fixups (65.2%). The PORT on 0 of 211 (0.0%).**
>    The original's `|cos|` falls monotonically across its 10-contact post-bounce train and it
>    escapes; the port's rises and **locks on a fixed point at `|cos| ~ 0.827`, damp `~0.55`,
>    `pre_h ~ 55`** — which is exactly §21.5's 40-70 residency band.
> 3. **THE OPEN QUESTION, and it is new.** Both sides leave the first bounce with nearly the
>    same velocity direction (`0.7739` original, `0.7475` port — the port's is the **more**
>    tangential of the two) and nearly the same nose (`(-0.8596, 0.5110)` against
>    `(-0.869, 0.495)`). What differs is the **interval between contacts: 19 frames on the
>    original, 1-2 on the port.** The original gets an order of magnitude more free flight to
>    rotate its velocity before the wall is asked again. **This is a contact-CADENCE question,
>    the first framing in the whole re-open that is neither inside A6a nor slip-coupled.**
>
> **NEXT COMMAND.** Take the cadence directly, both sides; the channels already exist.
> - ORIGINAL: `scenario_launch.py --fixup-probe`'s `frame` column. Its 23 fixups land at frames
>   978, 997, 1010, 1022, 1034, 1046, 1059, 1071, 1083, 1096, 1254, 1351, ... — 10 contacts in
>   118 frames, then a **158-frame gap**.
> - PORT: `MASHED_WORLD_CONTACT_LOG` ordinals plus `MASHED_SUBSTEP_VELPROBE`'s `c9ec` (new).
>   211 fixups in 1625 frames.
> - Then ask what RE-ARMS the contact: `VehicleContactHistoryUpdate` `0x00470ae8` /
>   `ContactHistoryLookup` `0x00468b40` and the 32-slot history at `veh+0xbfc`. **§19's result
>   stands** — the original's slot-0 history is all-zero on 22 live samples, so it re-latches
>   too — but "re-latches when re-penetrated" and "re-penetrates every frame" are different
>   claims and only the first was ever tested.
> - **Penetration depth is already ruled out**: both graze. ORIGINAL `-0.02056, -0.00038,
>   -0.00123, -0.00011`; PORT `-0.0159, -0.0040, -0.0023, -0.0006, -0.0017, -0.0004`.
>
> **DO NOT re-open any of these — each is proven faithful or proven circular in §22.2:**
> - **`0x0046ef70`'s impulse.** `local_54` = `(2417.77, -324.51, -141.34)` original against
>   `(2424.47, -325.4, -141.19)` port — **0.28% / 0.27% / 0.11%**. The slot producer agrees to
>   4-5 digits on arm, scale, normal and magnitude.
> - **the substep velocity chain.** The ORIGINAL's velocity is **bitwise unchanged** across
>   `0x0046f6c0` on 2945/2945, `0x00469aa0` on 2945/2945 and `0x004709a0` on 2932/2932 — it
>   writes `+0x9b0` in exactly two places per frame, A6a and the fixup. The PORT's three
>   `WheelContactSolver` velocity-write sites fire **0 times in 4000 substeps**, velocity
>   bitwise unchanged 4000/4000. The port is faithful here.
> - **`+0x9e4`'s write order.** The port's `+0x9e4 / |velocity|` at the fixup entry is
>   **1.017098** (n=211) where the original's is **1.000000** (n=23, 0 off by >1e-3), and that
>   looked like an ordering bug. It is not: a capstone sweep of `0x00467650..0x00468990` (1243
>   instructions, reached `0x00468989`) finds the only two `+0x9e4` stores at `0x00467673` and
>   `0x004686cc`, and **`0x004686cc` precedes grip-clamp #6 at `0x004687f0`**. The original
>   writes it before the clamp too, so the gap is the port clamp's own excess bleed read out
>   downstream — **circular**.
> - grip-clamp #6 (§21.5, byte-faithful) and the `l_60` / `ld4` lane (§21.10, measured out).
>
> **A near-miss worth internalising before you pair any cross-side samples.** Pairing the
> original's post-fixup velocity with the next `0x004709a0` substep entry produced a confident
> **false "71% tangential-impulse deficit"**. A6a `0x00467650` runs once per frame BEFORE the
> substep loop and it writes `+0x9b0`, so when the fixup lands in a frame's last substep the
> next substep entry is past a frame boundary. Adding A6a as probe **site 2** flags exactly
> those pairs (`*` in `a9_fixup.py`; 7 of the first 12 original rows), and with the correct
> pairing the impulse agrees. **Any "next sample after X" pairing across a per-frame boundary
> needs a frame marker in the same stream.**
>
> ### New tooling in §22 (all default-OFF or read-only)
> - `re/tools/statediff/a9_bounce.py` — first-bounce-aligned cross-side comparator. The §22.1
>   detector, the original-vs-original control (`orig_solo3` vs `orig_solo4` are **bit-identical
>   on all eleven channels** over `d = -5..+12`, so `S(C) = 0` and the tolerance is the
>   registered floor with no noise allowance), the aligned C1..C8 table, and the **measured**
>   `player_trace` <-> `motion_diag` join offset (`md_line = pt_f + 1`, 0 of 1625 mismatches
>   against ~1620 for every other shift).
> - `re/tools/statediff/a9_fixup.py` — the per-contact damp/knee table for both sides, with the
>   frame-boundary `*` flag.
> - `scenario_launch.py --fixup-probe` — entry hooks on `0x0046ef70` (site 0), `0x004709a0` (1),
>   `0x00467650` (2, frame marker), `0x0046f6c0` (3), `0x00469aa0` (4), with a register
>   self-check (`ESI == EDI == 0x8815a0` on 8/8) and the **live 18-slot contact set**. This is
>   the only way to see the slots: the `.msd` has them `-1` on 2332 of 2333 frames and `+0x9ec`
>   is 0 on every frame, because the render-tick snapshot lands after the substep loop cleared
>   them.
> - `MASHED_SUBSTEP_VELPROBE=<relative path>` — per-substep `|velocity|` at the four matching
>   points, `WheelContactSolver`'s three velocity-write counters, and `+0x9e4 / |velocity|`.
> - `MASHED_FIXUP_LOG` now emits a second line per fixup with `local_74`, the damp, the cap
>   flag, slot 0's key and the pre / pre-damp velocities.
>
> Artefacts: `verify/d2_bounce_20260930/` (`orig_fp{1,2,3}.msd.fixupprobe.csv`, `p1/`, `p2/`,
> `score{1,2,3}/`).
>
> **The D3 modes 3/7 hold stands. D2 must close before it starts.**


> ## SUPERSEDED (attempt 7) — the clamp-#6 / l_60 lane, measured out
>
> Read [`re/analysis/D2_REOPEN_2026-09-29.md`](analysis/D2_REOPEN_2026-09-29.md) **§21**
> (§21.6 first, then §21.5 and §21.2). **Do not re-derive any of it.** §21 withdraws three
> readings — §20.15's "the lateral is never GENERATED", §21.4's "the original is on the HIGH
> arm", and two of my own intermediate ones. Do not act on any of them.
>
> **The §3 bounds are unchanged and are not renegotiable.** PASS intervals:
> `slip 1500-2000` **0.18855 .. 0.19635**, `slip 2000-2600` **0.24488 .. 0.25487**,
> `driving-median` **1904.70 .. 1982.44**. The arm is still §16.7
> (`MASHED_STEER_HOLD_AFTER=0`), `MASHED_MEASURE_SOLO=1`, `MASHED_TRACK_SEL=12`.
>
> ### Scored at HEAD, 3 of 3 runs bit-identical (§21.6)
>
> | metric | port | n | median speed | interval | verdict |
> |---|---:|---:|---:|---|---|
> | slip 1500-2000 | **0.2033** | 20 | 1683.53 (in band) | 0.18855..0.19635 | **FAIL** +3.4% past the bound |
> | slip 2000-2600 | **—** | 0 | — | 0.24488..0.25487 | **UNSCORABLE** |
> | driving-median | **1355.66** | 54 | 1355.66 | 1904.70..1982.44 | **FAIL** -30.2% |
>
> Median horizontal speed over the whole 1080-frame window: **26.36**, 1080/1080 grounded.
> The car does not drive. Build gate met: `allowlisted=122 NEW=0`.
>
> ### PICK UP HERE — the clamp-#6 / `l_60` lane is MEASURED OUT. The two sides are in different basins of one feedback loop, and the basin is entered UPSTREAM of A6a.
>
> **§21.7-§21.10 finished the lane the block below opened. Do not re-run any of it.** The
> original's `l_60` was measured directly (not inferred) with an entry hook on RwV3dLength
> `0x004c3ac0`, whose argument is a POINTER — `scenario_launch.py --mag-probe`, reduced by
> `re/tools/statediff/a8_l60.py`. Gate 1 passed with a known-answer self-check: site
> `004686a9`'s vector equals the record's own `+0x9b0`/`+0x9b8` on **1424/1424** rows.
>
> | band 100-150 | ORIGINAL (n=35, med speed 132.73) | PORT (n=30) | ratio |
> |---|---:|---:|---:|
> | `grip*speed` | **33 157.4** | 17 992 | **1.84x** |
> | `l_60` | 285.243 | 141.24 | 2.02x |
> | `ld4` | 0.81376 | 0.3005 | **2.71x** |
> | `le4` (capped) | 119.972 | 100.28 | 1.20x |
> | above the 32768 knee | **18/35** | 8/30 | — |
>
> **The deficit is real and the original sits ON the knee.** But every factor feeding it now
> has a cross-side measurement, and each one either **agrees** or is **slip-coupled**:
> - `le4` **1.20x** — agrees.
> - `+0x9e8` / the spin factor `f`: **1.09x at 150-250 (AGREE)** while `ld4` there still
>   differs 1.93x, so `f` does not carry it; and its 100-150 ratio **6.75x** is §20.15's slip
>   ratio **6.73x** to two figures, i.e. the same measurement in another channel — circular.
> - the wheel forward axis: **the write is present on 100% of frames** (`Integrate2.cpp:692`'s
>   named failure mode fires on 0/1334 and 0/1628), so A5's rotation does reach the slots.
>
> **One real independent defect was found, and it is quantified as insufficient.** The
> ORIGINAL's front-axis deflection is **`-33.867` deg in every band — exactly its own steer
> angle, speed-INDEPENDENT**. The PORT's is `-30.480` / `-28.646` / `-26.812` at 100-150 /
> 150-250 / 1500-2000, i.e. **10.0% / 15.4% / 20.8% short and speed-DEPENDENT**. Rear pairs
> agree on both sides, so the error is purely front. Closing it moves `ld4` from `0.30050` to
> `0.35632` = **10.9% of the gap**, so it **cannot close D2** and no fix was authored on it.
> **Its target invariant is clean if you do fix it: front deflection EQUALS the steer angle,
> exactly, at every speed.** Its writer is **not located** — grep finds no write to
> `p[0x1f..0x21]` anywhere in `mashedmod/src` and A5's own port has no reference to those
> indices, which is what memory `offset-grep-misses-dword-index` predicts for a dword-index
> store off a computed base. **It needs Ghidra xrefs.**
>
> **So: the causality cannot be broken from inside this loop.** It is self-consistent in both
> directions — small slip -> small `ld4` -> small `l_60` -> large clamp `k` -> lateral and `av`
> bled -> small slip — and the two sides sit in **different basins** of it.
>
> **NEXT, in this order, and NEITHER is a fix:**
> 1. **Test bistability with a deliberately unfaithful knob.** Add default-OFF
>    `MASHED_A6_FORCE_HIGHARM=1` forcing clamp #6's high arm (`k <= 0.2`) regardless of
>    `grip*speed`, and run the §16.7 arm. If the port escapes to the original's basin (slip,
>    `ld4`, `l_60` all rising together) the loop is bistable and the question becomes what sets
>    the initial condition. If it does not, the loop is not the mechanism and all of §21 is
>    downstream of something else. **This is a diagnostic; do not ship it.**
> 2. **Find what puts the port in the low-speed basin.** That is a question about the first
>    contact and the ~5 frames after it, NOT about A6a: §20.14 measured that both first bounces
>    already AGREE (`cos` `-0.366` vs `-0.361`) and that the sides separate over the following
>    5-6 frames. Combined with §21.5's residency fact — **the original passes below 100
>    horizontal once per race, 6-8 frames of 6658, while the port spends 238 of 1352 at 40-70
>    alone** — that 5-frame window is the target.
>
> **New tooling in §21.7-§21.10** (read-only or default-OFF): `--mag-probe` /
> `--mag-probe-limit` on `scenario_launch.py` (entry-hook RwV3dLength and log its argument,
> tagged by return address; **count-first**, the function has 120 call sites image-wide),
> `re/tools/statediff/a8_l60.py`, `re/tools/statediff/a8_wheelaxis.py`.
> Artefacts `verify/d2_magpr_20260930/{count,read1}`.
>
> ### SUPERSEDED — the block that opened the lane §21.7-§21.10 closed
>
> **The chain, all measured, nothing inferred:**
> 1. The lateral is removed **inside A6a** and nowhere else below 150 speed. The three-site
>    bracket (`scenario_launch.py --lat-bracket`, new) gives `I_a6b = +0.0000` with **0 pos /
>    0 neg in all six bands up to n=606**, `I_s1 = +0.0000` likewise, and `I_s2 / L` of
>    0.0020 (70-100) / 0.0045 (100-150) against A6a's **0.1019**.
> 2. **Grip-clamp #6 is BYTE-FAITHFUL** — disassembled `0x004687f0..0x0046897b` against
>    `Integrate2.cpp:713-736`, both arms, all six constants, the two floors, the `1 - k`, the
>    early return, the full-stop block. The `0.1` bound is a **FLOOR** (`fcom`+`jp` at
>    `0x0046889b`/`0x004688a6`), as ported. **Do not go looking for a transcription bug there.**
> 3. `grip = l_60 / Rf(v,0x18c)` and **`+0x18c` is `1.0` on both sides** (1 distinct value
>    over 1352 original frames; `m18c=1` on every port `G6` line). So `grip == l_60`.
> 4. **The number to hit:** the port's `grip*speed` at 100-150 horizontal is **17 992**
>    (n=30) and must reach **>= 29 491** to put `k` on its `0.1` floor, which is what
>    reproduces the original's measured `I_a6a / L = 0.1019` (n=46). At 40-70 the port's `k`
>    runs to **0.9084** (grip*speed 3 006, n=457) — that is §21.2's 62%-per-frame collapse.
> 5. **`l_60 = sum ld4 * le4` and the `ld4` path is CIRCULAR** — `ld4` is the sine of a wheel
>    slip angle (`Integrate2.cpp:437-440`), the quantity being explained. `le4` is measured to
>    **agree in form on both sides** (`le4 ~ speed`, cap 1024; the original's `f` multiplier
>    from its own `+0x9e8` is 2.34 / 2.82 / 91.1 at 70-100 / 100-150 / 800-2000 against a
>    `|dst|` of order 1). **So no fix may be authored from the port side alone.**
>
> **Next command.** Get the ORIGINAL's `l_60` as a measurement. It is a local `double` in
> `0x00467650`, so the record does not carry it; the candidates are the `Mag3` call sites
> inside A6a's wheel loop (`Integrate2.cpp:425` `le4` and `:440` `ld4`) — if the original
> calls out for those magnitudes, an **entry** hook on the callee yields both vectors
> directly, which is the sanctioned technique (memory `frida-interceptor-is-entry-only`).
> Disassemble A6a's wheel loop first to find out whether they are calls or inlined `fsqrt`.
> If inlined, `l_60` is not reachable with entry hooks and the next best witness is the
> per-wheel force at record wheel base `+0x70/+0x78`, which is linear in `le4` through
> `lbc = p[0x15]*p[0x1b]*g_suspScale*le4*0.0009766` (`Integrate2.cpp:436`) — but note
> `re/tools/statediff/a8_wheelfit.py` has a withdrawn finding on exactly that route, so read
> memory `cross-side-fit-needs-both-sides-checked` before trusting it.
>
> **And weigh this first, because it may make the whole lane secondary.** The 110-second
> control (`orig_lb2`, **6658** frames) returns **the same n=2 at 40-70 and n=6 at 70-100** as
> the 2335-frame one. **The original passes below 100 horizontal once per race, for 6-8
> frames, and never returns; the port spends 238 of 1352 frames at 40-70 alone.** The port's
> residency in that band is itself the defect, and its runaway `k` there is coupled to it. A
> fix that only corrects the low-speed bleed may not move the scored table at all.
>
> ### New tooling this session (all default-OFF / read-only)
> - `re/frida/scenario_launch.py --lat-bracket` — entry-only samples of the player record at
>   A6a `0x00467650`, A6b `0x00468980` and the substep loop `0x004709a0`, ~180 calls/s. Prints
>   `latBracketStats` before the rows and writes the CSV even when empty.
> - `re/tools/statediff/a8_latinc.py` — per-frame velocity increment decomposed on the body
>   forward/right axes, with coherence, retention and signed slip, per speed band.
> - `re/tools/statediff/a8_latbracket.py` — reduces a bracket capture on the verified
>   `0,2,1,1` pattern; `--legacy3` for the older two-site captures.
>
> Artefacts: `verify/d2_latbr_20260930/orig_lb{1,2,3}`, `verify/d2_score_20260930/s{1,2,3}`.

> ## SUPERSEDED (attempt 6) — the contact ARM's y/z ratio
>
> Read [`re/analysis/D2_REOPEN_2026-09-29.md`](analysis/D2_REOPEN_2026-09-29.md) **§20**
> (and §16-§19 for the history). **Do not re-derive any of it.**
>
> **The §3 bounds are unchanged and are not renegotiable.** PASS intervals:
> `slip 1500-2000` **0.18855 .. 0.19635**, `slip 2000-2600` **0.24488 .. 0.25487**,
> `driving-median` **1904.70 .. 1982.44**. The arm is still §16.7
> (`MASHED_STEER_HOLD_AFTER=0`).
>
> ### PICK UP HERE — the port's LOW-SPEED slip is 6.7x too small, and the lateral is never GENERATED
>
> Read **§20.15 first**, then §20.11 and §20.14 for how the chain hangs together. **Three
> readings from the attempt-6 session are WITHDRAWN (§20.9, §20.10, §20.14) — do not act on
> them if you see them quoted anywhere:** there is no contact-arm `a.y/a.z` deficit (the
> port's real torque, instrumented, is `(0, +0.118183404, -0.310108006)`), no drive-force
> deficit (binned by speed the port gains 14.77/frame against 13.46), and the original's
> body heading is NOT "bitwise unchanged" at the contact frame (`d(bodyH)` is `+0.000001`,
> and the forward vector is bitwise unchanged on 0 of 1403 consecutive driving-frame pairs).
>
> **THE MEASUREMENT TO WORK FROM.** Slip `|wrap(velH - bodyH)|`, grounded, by horizontal
> speed band — ORIGINAL frames 981-1120 against PORT `f = 81..220`. This regime has never
> been scored: `a8_slip_axis.py`'s floor is 1500.
>
> | band | ORIGINAL median (n) | PORT median (n) |
> |---|---:|---:|
> | 50-100 | 1.2949 (4) | 1.3761 (52) |
> | **100-200** | **0.7816 (81)** | **0.1162 (19)** |
> | 200-400 | 0.3540 (39) | 1.0476 (4) |
>
> **6.7x less slip.** The original slides at 45 degrees to its nose; the port drives within
> 6.7 degrees of it. That is upstream of everything else and it explains the trap: a
> velocity 45 degrees off the nose meets Training's wall at `cos` 0.55-0.69, BELOW the
> `0.7` knee in `0x0046ef70`'s damp `min(0.9, 3*(1 - |m|/speed))`, so the original keeps
> the clamped **90%** on every bounce after the first; the port arrives at 0.77-0.80 and
> keeps **61-68%**, and its velocity reverses relative to its nose so the `-0.1` reverse
> gate latches (port below the gate on **625/1352** frames against the original's
> **25/1352** — 25x) and the negated steer torque turns the nose INTO the wall.
>
> **THE FORK IS ALREADY RESOLVED — do not re-run it.** Per-frame lateral
> `lat = |vel - (vel . fwd) fwd|` in the 100-200 band: ORIGINAL median `lat` **87.413**
> with ratio `lat(n+1)/lat(n)` **0.8939**; PORT **14.144** with ratio **1.0590**. The port
> is NOT bleeding the lateral away faster — its ratio is above one. **An over-strong bleed
> is refused; the lateral is never GENERATED.** A6a's grip-clamp #6 is cleared with it:
> `MASHED_A6_DIAG` measures the lateral-path `k` at **0.366** (100-200) and **0.694**
> (50-100), nowhere near the 1.0 that would annihilate it.
>
> **Two sub-candidates, and one is circular.** (1) the wheel/steer force that makes lateral
> velocity directly — `WheelContactSolver.cpp:280`/`:286`, i.e. `0x0046f6c0`'s friction
> impulse, plus A5's steer-force path; measurable without reference to the body's rotation.
> (2) the body yaw rate, since slip IS the body-vs-velocity lag and the port's pinned-phase
> `d(yaw)` is POSITIVE (`+0.0018..+0.0078`) against the original's `-0.0035/frame` — but
> that is coupled to the gate §20.14 attributes to the low slip, so **(2) must not be
> "fixed" before (1) is measured.**
>
> **Next command:** the per-frame lateral INCREMENT (not the ratio) on both sides in the
> 100-200 band, split by sign along the body right axis, next to the wheel-friction impulse
> `0x0046f6c0` writes. Small increment at near-zero yaw rate -> (2) dominates and the gate
> is the lever; small increment at a comparable yaw rate -> (1) is the defect and the
> friction impulse is the lever. `MASHED_A6ADUMP` and `MASHED_A6_DIAG`'s G7 lines already
> carry everything needed on the port side, and `MASHED_PLAYERTRACE` now carries
> `+0x9d4..dc`, so no new instrumentation is required.
>
> **New diagnostics this session, all default-OFF:** `MASHED_FIXUP_LOG=<relative path>`
> (`local_6c` plus the accumulator before/after every fixup), the arm/scale/`arm.y/arm.z`
> columns in `MASHED_WORLD_CONTACT_LOG`, and the eight extra channels in
> `MASHED_PLAYERTRACE` (`+0x144/148/14c`, `+0x9bc/c0/c4`, `+0x9ec`, `+0x9e0`, `+0x9d4..dc`,
> `+0x9c8..d0`, ring-0 up/at). `MASHED_STEER_AXIS_TERRAIN=1` reverts the §20.2 fix for A/B.
>
> **The contact arm is sound, do not re-derive it:** `arm = R_basis * (hullPoint * 3.6)`
> with no translation; the two reporting slots are the two ends of ONE vertical hull edge at
> local `x = -0.2188, z = +0.4538`, and `arm.y/3.6` is exactly U-9155's box y-min `0.03740`
> and y-max `0.30860`. `S+0x14` equals `|arm|` on both, which matters because the port's
> terrain solver never writes that field.
>
> **DONE, do not redo:**
> - **U-9156 is root-caused and HALF fixed.** `0x0046ef70`'s last-contact damp guard read the
>   wrong slot against the wrong sentinel. The original saves SLOT 0's base once before the loop
>   (`0x0046f013`/`0x0046f016`), reloads it at `0x0046f522`, reads `[rec+0x4ac]` = slot 0's KEY
>   at `0x0046f5ae`, and `0x0046f5b1 cmp ecx,-2` / `0x0046f5b4 je` skips the damp ONLY on `-2`.
>   The port tested SLOT 17's key against `-1`. Fixed; prediction MET (`+698.16` -> **`+194.39`**
>   vs the original's `+176.47`).
> - **The loop question is ANSWERED and the old reading is WITHDRAWN.** §16.1's "the port's loop
>   is larger and 3.31 units away" was an artefact of the 60 straight steps. At matched speed the
>   cornering law agrees to 6.5% (radius) / 3.6% (`d(velH)`) / 0.04% (`d(bodyH)`).
> - **§15.7's "the ORIGINAL touches the plane once in 2332 frames" is WITHDRAWN.** It hits walls
>   **twelve** times, alternating `pos.x ≈ -2.0` and `pos.x ≈ +2.0`.
> - **U-9155's STRUCTURE is corrected** (address unchanged): the box is field `+0x230..+0x247`
>   of a 0x2ac-stride record based at **`DAT_0063d9e0`**. The writer is still unidentified and
>   the static search is exhausted — §18.5 lists all six probes. **It does NOT bear on the trap**:
>   the original's live hull equals the seeded box to every printed digit.
> - **The dual-copy guard blind spot is fixed and proven** (`c231b116`): `scripts/rva_body_scan.py`
>   second pass + `scripts/test_rva_body_scan.py` 6/6 + an end-to-end re-run on the pre-fix tree.
>   13 new pairs exposed (U-9158), allowlisted `audit=UNREVIEWED-U9156`, 111 -> 122, NEW=0.
>
> ### PICK UP HERE — find the first DIVERGING TERM inside the feedback loop
>
> **Read §19 first — it withdraws the obvious next step.** "Port `0x00468d80` and `0x004694e0`"
> was written here at the close of attempt 5 and is **WRONG**: both already have full
> transcriptions (`Collision/CarWorldContacts.cpp:160-253` and `:262-377`) and `STUBS.md`
> S-3440/S-3441 are struck through as resolved. `hooks.csv`'s `status stub` was stale and is
> now `impl`. **Do not port them.**
>
> A second obvious step was also tried and **refuted by measurement**: nothing in the port
> writes the 32-slot contact history at `veh+0xbfc` that `ContactHistoryLookup` `0x00468b40`
> scans, so every contact re-latches every substep — which matches the symptom exactly. But
> `--peek` on the running original shows slot 0's **32 keys and 32 active flags all zero on 22
> samples over two 40 s live races**, so the original re-latches too. **The port is faithful
> there.**
>
> What IS a stand-in, and neither is on the trap's path today: `Rw_BroadphaseWalk`
> `FUN_00538c80` (no-op, replaced by `ProduceTerrainBatch`'s plane-distance filter) and
> `Obj_ListCount()==0`. The batch entry layout was checked index by index and is correct.
>
> **The trap's shape is understood; the diverging term is not.** `0x0046ef70`'s damp is
> `min(0.9, 3*(1 - min(1, abs(m)/speed)))` over all three velocity components, and `0x00468d80`
> builds `abs(m)` from `abs(vel + spin) * dot(norm, faceNormal)` — so `abs(m)/speed` is
> `abs(cos(velocity, wall normal))`: head-on annihilates the velocity, tangential costs 10%.
> The original rotates tangential and leaves; the port cannot, because the yaw rate scales with
> speed and it is pinned at ~25 (yaw frozen at `2.59..2.60` over 210 steps). **Both first
> bounces now agree closely** (tangential retained 22% vs 23%) and the two sides separate over
> the following ~20 frames.
>
> **Next command:** per-frame cross-side dump of record `+0x9b0` (velocity), `+0x144` (angular
> accumulator), `+0x9e4` (speed) and `+0x9ec` (active-contact count) over ORIGINAL frames
> 980-1100 against the same window on the corrected port arm, and find the first frame the
> **angular accumulator** diverges — that is the channel that decides whether the car rotates
> tangential. `MASHED_WORLD_CONTACT_LOG=<relative path>` logs every fixup with its reporting
> slots, depths, normals and magnitudes; `MASHED_WORLD_CONTACT=0` reverts the chain for A/B.
>
> Two smaller things, both registered rather than done:
> - **[UNCERTAIN]** the original's `vel.y` is **exactly `+0.0000`** on every pre-contact grounded
>   frame 976-980 where the port carries `+74.8..+77.9`. Next command: dump `+0x9b4` across a
>   grounded stretch on both sides and find the writer the port does not zero.
> - **`0x0046b1c0` is NOT consolidated to one body**, deliberately: the asi-only copy is the C3
>   Frida-GREEN naked-x87 one and the exe-only C++ copy has no `run_diff` of its own. The
>   precondition is now met (§16.6), so the burn-down step is one command:
>   `py -3.12 re/frida/run_diff.py vehicle_slot_aabb_expand` against the C++ body, then delete the
>   naked copy and drop the allowlist line.
>
> **Guards on the final build** (re-run them, don't assume): criterion (e) **PASS 3/3**;
> AI (b) **FAIL 3/3**, `c1_median` 52.5 / 38.0 / 49.0; power-ups **decision CLEAN 11/11** with
> `g3` contact diverging; oracle rule 3 **GREEN** MISMATCH=0; rva-lint **`allowlisted=122 NEW=0`**;
> Arctic non-regression **2044.85 (n=34)**. **Gotcha:** `MASHED_AI_STEPDUMP` must be a **relative**
> path — an absolute one silently produced no CSV this session.
>
> **The D3 modes 3/7 hold stands.** D2 must close before it starts.


## SUPERSEDED kickoff — D2 re-close attempt 4 (kept as history)

> ## START HERE — D2 is STILL REOPENED, but the wall now holds and the residual has ONE name
>
> Read [`re/analysis/D2_REOPEN_2026-09-29.md`](analysis/D2_REOPEN_2026-09-29.md) **§15**
> and U-9156. **Do not re-derive any of it.**
>
> **The §3 bounds are unchanged and are not renegotiable.** PASS intervals:
> `slip 1500-2000` **0.18855 .. 0.19635**, `slip 2000-2600` **0.24488 .. 0.25487**,
> `driving-median` **1904.70 .. 1982.44**. Port arm is still
> `a8_run_port.py <dir> 90 MASHED_MEASURE_SOLO=1 MASHED_TRACK_SEL=12` (Training).
>
> | metric | attempt 3 | **attempt 4** | verdict |
> |---|---:|---:|---|
> | slip 1500-2000 | 0.1605 (n=13) | **0.1605** (n=13) | FAIL -16.6% |
> | slip 2000-2600 | 0.3086 | **0.2497** (n=28) | inside |
> | driving-median | 2437.93 | **1938.68** (n=66) | inside |
>
> 4 of 4 runs bit-identical on every column. **D2 does NOT close** (§3c needs all three), and
> **the two that land inside are NOT a pass** — they are medians over 28 and 66 rows. Read
> §15.7 before quoting either.
>
> **DONE, do not redo:**
> - **U-9154 RESOLVED.** The car-vs-world contact chain runs: `Collision/ContactFixup.cpp` is a
>   new verbatim port of `FUN_0046ef70` (and its pushed matrix argument is DEAD — balanced-ESP
>   walk, no read of `[esp+0x90]` in 485 instructions); `Rw_VtableDispatch` is bound to the
>   measured device slot `+0xc` (`call [ecx+eax+0xc]` @`0x004c3db0`) = `RwV3dTransformPointsCPU`;
>   the substep runs `0x00470ae8` -> `0x00470aef` -> `0x00470afe` with the `0x00470ab0` retry;
>   `SyncContactRingMatrix` publishes `g_bodyBasis` into the `rec+0x928` ring.
>   `MASHED_WORLD_CONTACT=0` reverts the chain for A/B; `MASHED_WORLD_CONTACT_LOG=<path>` logs
>   every fixup with its reporting slots, normals and depths.
> - **`0x0046e9e0` is NOT to be re-ported.** Both halves already have bodies
>   (`BodyOrientationIntegrate.cpp` + `VehiclePhysicsRun.cpp`'s position accumulator); a third
>   copy is a new dual body. §14.6's "three unported functions" was wrong on that one.
> - **U-9155: the hull producer is found and its data measured.** `FUN_0046b1c0` (called at
>   `0x0040ed62`, immediately before A3) builds the 14 non-wheel contact points at
>   `rec+0x90..+0x137` from a 6-float box that `FUN_0041f000` copies out of
>   `DAT_0063dc10 + car*0x2ac`. That address is past `.data`'s raw size, so it was `--peek`ed
>   live: `+0.2188 +0.3086 +0.4538 -0.2188 +0.0374 -0.5233`, identical on cars 0/1/2/3.
>   Open half: nobody has found the WRITER of `DAT_0063dc10`, so the port seeds one box for all
>   slots.
> - **`0x0046b1c0` already had a C3 Frida-GREEN port** (`VehicleSlotAabbExpand.cpp`) that the
>   exe cannot call, and `scripts/lint_rva_bodies.py` anchored NEITHER body — it said `NEW=0`
>   with both in the exe. Split by target + allowlisted CROSS-TARGET. **Check for an existing
>   port before writing one; the guard will not tell you.**
> - **`RecoverOffMesh` is KEPT**, with new evidence: Arctic fires **46 -> 0**, Training 0 -> 0.
>   That is 2 tracks of 12, which is not unreachability. Re-pickup: 0 fires on all 12.
>
> ### PICK UP HERE — U-9156, and it decides whether D2 is a physics problem or a trajectory one
>
> With the wall solid, **the port's car gets TRAPPED against it.** All 73 fixups in the run sit
> at `x = -2.02 .. -2.06`; median speed over 1080 frames is **93**. The ORIGINAL touches the
> same `x = -2.500` plane **once in 2332 frames** (`orig_solo3.msd` frames 980-998, `pos.x`
> pinned for 18 frames at speed 85..212) and then drives away.
>
> The contact itself is right: reporting slots 5 and 9 are the hull corners at `x = box[3]`,
> normal `(1.000,0.000,0.000)`, depth `-0.037`. What differs is the approach — the port reaches
> the wall at **2283** where the original reaches it at **1717**.
>
> **The one command that decides it:** plot both loops on **Training's** `COLLISIONS.BSP` with
> `re/tools/statediff/loop_plot.py` (the tool §13.2 already used cross-track) and compare radius
> and centre.
> - If the port's loop is LARGER, this is U-9147's residue (drives too fast -> bigger radius ->
>   into the wall) and the fix is upstream of contacts. `slip 1500-2000` failing at -16.6% is the
>   same story.
> - If the loops match, instrument `0x0046ef70`'s per-slot terms and diff them against a Frida
>   capture of the original at frame 980. A hand estimate on the original's own numbers
>   reproduces its `+1893` delta at `-1716.69`, which argues AGAINST a restitution error — but
>   that is an estimate, not a measurement.
>
> Then **U-9152** (the `+0x928` vs `g_bodyBasis` storage split) — note `SyncContactRingMatrix`
> now publishes one into the other every substep, so half of it is already paid for.
>
> **Guards on the final build** (re-run them, don't assume): criterion (e) **PASS 3/3**;
> AI (b) **FAIL 3/3**, `c1_median` 52.5 / 38.0 / 49.0 (identical to attempts 1-3); power-ups
> **11/11 decision CLEAN** with `g3` contact diverging; oracle rule 3 **GREEN**
> (FinishOrder 2717/2717, MISMATCH=0); build with `rva-lint allowlisted=111 NEW=0`.
>
> **The D3 modes 3/7 hold stands.** D2 must close before it starts.


## SUPERSEDED kickoff — D2 re-close attempt 3 (kept as history)

> ## START HERE — D2 is STILL REOPENED. The arm was measuring the wrong track, and the residual is now one named unported function.
>
> Read [`re/analysis/D2_REOPEN_2026-09-29.md`](analysis/D2_REOPEN_2026-09-29.md) **§13-§14**
> and ROADMAP §D2's "Re-close attempt 3" block. **Do not re-derive any of it.**
>
> **The §3 bounds are unchanged and are not renegotiable.** PASS intervals:
> `slip 1500-2000` **0.18855 .. 0.19635**, `slip 2000-2600` **0.24488 .. 0.25487**,
> `driving-median` **1904.70 .. 1982.44**.
>
> ### The one thing to change in your muscle memory
>
> **The port arm is now `MASHED_TRACK_SEL=12`.** Every D2 solo run:
>
> ```
> py -3.12 re/tools/statediff/a8_run_port.py <dir> 90 MASHED_MEASURE_SOLO=1 \
>         MASHED_TRACK_SEL=12 "MASHED_TITLE=<label>"
> ```
>
> Why: `a8_run_port.py:16` hardcodes `MASHED_TRACK_SEL=0` = `kAreas[0]` = **Arctic**, while
> `scenario_launch.py` defaults `--track` to **0 = Training** and **no** reference capture
> passes `--track`. Every port-vs-original solo comparison before today compared **Arctic
> against Training** (U-9153, RESOLVED). `a8_run_port.py` itself was deliberately left
> alone so the archived runs stay reproducible from their own PROVENANCE — pass the override.
>
> | metric | attempt 2 (Arctic) | **attempt 3 (Training)** | verdict |
> |---|---:|---:|---|
> | slip 1500-2000 | 0.1445 | **0.1605** | FAIL -16.6% — **n=13 only**, do not quote as a precise deficit |
> | slip 2000-2600 | 0.2557 | **0.3086** | FAIL +23.5% |
> | driving-median | 1740.54 | **2437.93** | FAIL +25.4% |
>
> 3 of 3 runs bit-identical on every column; the U-9148 second attractor did not appear.
> Two metrics moved *further* out — kept as-is, because correcting the arm was not tuning.
>
> **DONE, do not redo:**
> - **U-9153 RESOLVED** — the cross-track arm, four witnesses, §13.1.
> - **§12.4's trajectory-vs-mesh question is ANSWERED: neither.** Both loops plotted on
>   their own track's `COLLISIONS.BSP` (`re/tools/statediff/loop_plot.py`, new, read-only;
>   `verify/d2_offmesh_20260929/loops_crosstrack.png`): ORIGINAL 212/212 on-mesh on Training,
>   151/212 on Arctic; PORT 203/203 on Arctic, 143/203 on Training.
> - **§12's `RecoverOffMesh` mechanism is an artefact of the wrong track.** 46 fires -> **0**
>   on the corrected arm, 3 of 3 runs. Do not re-chase the 35-resets-per-1080-frames story.
> - **§12.4's "bearings cover the whole circle" is WITHDRAWN as a reading.** The 46 Arctic
>   fires lie on three straight axis-aligned road edges (`x=-24.25`, `z=+35.95`, `x=-0.40`).
> - The original's world position is readable from an `.msd`: it is the **+0x928 RwMatrix
>   translation row**, record `+0x958/+0x95c/+0x960`. `loop_plot.py --check` validates the
>   basis first (0 of 6996 rows off unit norm). There is **no** large-range world position
>   anywhere else in the 0xd04 record.
>
> ### PICK UP HERE — U-9154, and it is a port job with a named target
>
> `VehicleContactScanUpdate` **`0x00469aa0`** is ported (`Collision/CarWorldContacts.cpp:387`)
> and **has no caller**. Its only call site in the image is **`0x00470ae8`**, inside
> **`VehicleCollisionBroadPhase` `0x004709a0`** (hooks.csv C2, `mapped`):
>
> ```
> 0x00470ab0  cmp ebp, 2 / jge            ; at most 2 retries
> 0x00470ad3  call 0x0046e9e0             ; A9 integrate       (C2, mapped — UNPORTED)
> 0x00470ae0  call 0x0046f6c0             ; wheel contacts     (ported)
> 0x00470ae8  call 0x00469aa0             ; contact scan        <-- never called by the port
> 0x00470afe  call 0x0046ef70             ; fixup, gated on [record+0x9ec] != 0
> 0x00470b0a  inc ebp / jmp 0x470ab0      ; a reported contact RE-RUNS the substep
> ```
>
> **Why it is the residual.** The port's driving-speed quantiles match the original's at the
> **top** (p95 0.99x, p99 0.97x, max 0.97x) and diverge only at the **bottom** (p25 1.65x,
> p10 2.47x). The original's low tail is **wall impacts** — `orig_solo3.msd` frame 980->981:
> `vel.x` `-1716.69` -> `+176.47`, `pos.x` pinned for 18 frames, speed 1832 -> 191. Training's
> `COLLISIONS.BSP` has the wall (41 of 69 triangles in that box are XZ-degenerate, planes at
> `x=-3.000` and `x=-2.500`), and `HeightOnSoup` (`TrackRenderer.cpp:2098`) discards exactly
> the degenerate ones, so the port drives through both (loop reaches `x=-4.721`).
>
> **Three blockers, all named:** `Rw_VtableDispatch` is a no-op stub
> (`Collision/ContactStubs.cpp:78`); A9 `0x0046e9e0` is `mapped`, not ported, so nothing
> reads the corrective velocity at record dword `+0x130` (byte `+0x4c0`) or the 18 contact
> slots at byte `+0x4ac`; the retry needs `0x004709a0` itself. Order: `0x0046e9e0`, then
> `0x004709a0` + the retry, then a real `Rw_VtableDispatch`. Re-run the §13.3 arm after each.
>
> [UNCERTAIN] whether that closes the whole +25.4%. The quantile agreement at p95/p99/max is
> consistent with it and is quantitative, but **nothing yet runs the chain** — it is an
> attribution, not a measured fix.
>
> **`RecoverOffMesh` (`TrackRenderer.cpp:2142-2164`) is KEPT.** The original has **no
> off-world respawn** on this path; the contact chain above IS its faithful replacement, so
> removing the scaffold before that chain lands would only restore the
> freeze-against-an-edge loop (`TrackRenderer.cpp:2806-2817`). It is already off the D2
> measurement path (0 fires).
>
> Then **U-9152** (the `+0x928` vs `g_bodyBasis` storage split) — and note U-9154 touches the
> same block, since the contact ring lives at `+0x928 + sel*0x40`.
>
> **Guards on the final build** (re-run them, don't assume): criterion (e) **PASS 3/3**;
> AI (b) **FAIL 3/3**, `c1_median` 52.5 / 38.0 / 49.0 (identical to attempts 1 and 2);
> power-ups **11/11 decision CLEAN** with `g3` contact diverging; oracle rule 3 **GREEN**
> (3059/3059, 2/2, 3963/3963, MISMATCH=0); build with `rva-lint allowlisted=110 NEW=0`.
>
> **The D3 modes 3/7 hold stands.** D2 must close before it starts.


## SUPERSEDED kickoff — D2 re-close attempt 2 (kept as history)

> ## START HERE — D2 is STILL REOPENED, but the map changed
>
> Read [`re/analysis/D2_REOPEN_2026-09-29.md`](analysis/D2_REOPEN_2026-09-29.md) **§9-§11**
> and ROADMAP §D2's "Re-close attempt 2" block. **Do not re-derive any of it.**
>
> **The §3 bounds are unchanged and are not renegotiable.** PASS intervals:
> `slip 1500-2000` **0.18855 .. 0.19635**, `slip 2000-2600` **0.24488 .. 0.25487**,
> `driving-median` **1904.70 .. 1982.44**.
>
> | metric | attempt 1 | **now** | verdict |
> |---|---:|---:|---|
> | slip 1500-2000 | 0.1332 | **0.1445** | FAIL, -24.9% |
> | slip 2000-2600 | 0.2179 | **0.2557** | FAIL, **+2.3% — 0.00083 over the upper bound** |
> | driving-median | 1818.42 | **1740.54** | FAIL, -10.4% — **moved the wrong way**, kept per §3d |
>
> 4 of 5 runs identical; run 3 was the known U-9148 second attractor and is reported.
>
> **DONE, do not redo:**
> - **U-9147 IS LOCALIZED.** The first divergent quantity is `d(bodyH)/frame`, a
>   speed-independent constant: **-0.04249 (port) vs -0.04462 (original)** = **100/105**.
>   `_DAT_00613108` is **105.0** in the running original (9 live `--peek` samples); the port
>   hardcoded 100.0. **FIXED** — `g_handlingTorque`, the handling table completed with the
>   measured tags 6/12/18, selector 6. `d(bodyH)/frame` and the axis-minus-forward offset
>   now match the original exactly. `MASHED_HANDLING_TYPE=0` reverts.
> - **U-9151 is CLOSED.** `0x004c4600` is a dispatcher; the multiply is the measured
>   `0x005cb2a0`, ported naked-x87 (`Math/RwMatrixMultiplyCpu.cpp`, hooks.csv row at C3).
>   Two GREEN diffs (12/12 and 10/10). The exe's A6b orient is **bound**: 7 runs of 7 exit 0,
>   25 natural samples, `xfok=1` on all.
> - **A6a is CLEARED by measurement**, with a tool that self-checks first
>   (`re/tools/statediff/a6a_replay.py`, self-check 1 worst rel. error 7.8e-07). Block #4
>   per-wheel law 0.7-1.9% per wheel; block-#5 yaw torque 1.1-1.7%; clamp #6 `k_vel` ±3%
>   with opposite signs in the two bands; wheel geometry 0.002 rad. `g_suspScale` was
>   MEASURED on the original at **692.3021850585938** — the port's to the last digit.
> - **§6.2's handling-globals elimination is WITHDRAWN** (the A3 walk chains through `e[3]`
>   and matches tag 6, giving 105).
> - `a8_wheelfit.py`'s cross-side fit stays withdrawn. Do not quote a lateral-coefficient
>   ratio from it.
>
> **PICK UP HERE — the residual is `RecoverOffMesh`, and its mechanism is MEASURED.**
> Read `re/analysis/D2_REOPEN_2026-09-29.md` §12. This **supersedes** §10.4's "the suspect
> is the transient" framing, and it **clears the cornering law**.
>
> `TrackRenderer.cpp:2161` writes `car_vel_ = (cos ry, sin ry) * sp` — velocity exactly
> along the heading — so every fire sets the scored slip angle to **zero by construction**.
> `:2160`'s reseed makes the reducer drop that one frame and score every frame after it.
> Measured: slip rebuilds monotonically **0.083 at d=8 → 0.261 at d≥41** frames since the
> last reseed, and the port fires **35 times in the 1080-frame window** — one per ~31
> frames, **shorter than the ~40-frame rebuild**, so the car never reaches its own steady
> slip. At d≥41 the port's slip is **0.2608**, *higher* than the original's 0.2498, so the
> ported law does not produce too little slip.
>
> Diagnostic `a8_slip_axis.py --reseed-shadow` (**NOT the gate**): a 30-frame shadow raises
> slip 1500-2000 **0.1445 → 0.2006** at a **flat** median speed (1776 → 1807). Limits, do
> not over-read: n falls 227 → 67, and the 2000-2600 rows ARE regime-shifted (n 325 → 150,
> speed 2374 → 2462).
>
> **Root cause is upstream of the recovery.** New `MASHED_OFFMESH_LOG` (default-OFF): 46
> fires in 1630 frames, bearings spread evenly over the whole circle around the centroid
> (−178..+177°, radius 9.99..45.42) — **not** one hole in `col_tris_`. The car genuinely
> leaves the drivable surface, with an explicit speed staircase (2276→1138, 1601→800,
> 1342→671, … 798→399) that *is* the −10.4% driving median.
>
> **[UNCERTAIN]** trajectory vs collision-mesh coverage. **Next command** (§12.4): plot the
> two world loops — port via `a8_run_port.py <dir> 90 MASHED_MEASURE_SOLO=1
> MASHED_PLAYERTRACE=1`, original via a `--statediff-out` capture read through the `+0x928`
> RwMatrix translation row `m[12..14]`. If the original's loop lies **inside** the port's,
> the trajectory is the defect; if they overlap and only the port reports off-mesh, the
> mesh is.
>
> Then **U-9152** (the `+0x928` vs `g_bodyBasis` storage split).
>
> **Guards as of this session** (re-run them, don't assume): criterion (e) **PASS 3/3**;
> AI (b) **FAIL 3/3** with `c1_median` 52.5 / 38.0 / 49.0; power-ups **11/11 decision
> CLEAN** with `g3` contact diverging; oracle rule 3 **GREEN**; build with `rva-lint NEW=0`.
>
> **The D3 modes 3/7 hold stands.** D2 must close before it starts.

## SUPERSEDED kickoff — D2 re-close attempt 1 (kept as history)

Updated 2026-09-29 at the close of the **D2 re-close attempt 1** session (U-9149 / U-9147).
Branch `race/first-frame-parity`. Nothing is pushed.
Superseded kickoff: the earlier 2026-09-29 one (player-regression U-9141 / U-9145).

> ## START HERE — D2 re-close attempt 1 is DONE and D2 is STILL REOPENED
>
> Read [`re/analysis/D2_REOPEN_2026-09-29.md`](analysis/D2_REOPEN_2026-09-29.md) and
> ROADMAP §D2's "Re-close attempt 1" block. **Do not re-derive any of it.**
>
> **The bounds are pre-registered and must not be renegotiated** (that note §3, committed in
> `d81a8df6` *before* any fix, from four original solo captures). PASS intervals:
> `slip 1500-2000` **0.18855 .. 0.19635**, `slip 2000-2600` **0.24488 .. 0.25487**,
> `driving-median` **1904.70 .. 1982.44**, plus ≥3 port runs agreeing within the original's
> own half-range. Current port, 3/3 identical: **0.1332 / 0.2179 / 1818.42** — FAIL, FAIL,
> FAIL. If you think a bound is wrong, change it **in writing with the reason, before the
> next measurement**, never after seeing a result.
>
> **DONE, do not redo:**
> - **U-9149 decoded.** `[esp+0x3c]` at `0x0047093b` is `E+0x0c` = A4's `param_3` slot,
>   reused at `0x004706a2` to hold `record + [record+0x9a8]*0x40 + 0x928`, an `RwMatrix`.
>   Same pointer A5 gets as arg 2. The `.asi` `Call_A6b` is **fixed and verified** (144
>   self-test samples, 64 airborne, `ndiff=0` and `xfok=1` on every one).
> - **A6b is NOT U-9147.** Disjoint gates: A6b needs `+0x9e0 == 0`, the metric scores
>   `+0x9e0 >= 3.5` (`a8_slip_axis.py:35`). All five dual-copy leads are now eliminated.
> - **Eliminated with reasons** (don't re-try): A6a's matrix argument (never read, 0 reads
>   at `E+0xc`); the four handling globals `0x00613108/14/30/3c` (A3 seeds them to exactly
>   the port's hardcoded values and the override key `[0x00613140]` is 0 = the default
>   entry; max variant ±5%); "the port chases the velocity heading"
>   (`BodyOrient_IntegrateStep` **is** wired at `VehiclePhysicsRun.cpp:812`).
>
> **PICK UP HERE — U-9147 is NOT localized, and three attempts at localizing it today were
> all withdrawn. Read `D2_REOPEN_2026-09-29.md` §6.0's third correction before touching it.**
>
> `a8_wheelfit.py`'s cross-side lateral-coefficient comparison is **unsound in both modes**:
> `port_frames` builds the port's `lat` from a 2-D velocity heading (`u = (cos velH, 0,
> sin velH)`) and ignores the `wld4` the port actually logs, so the two sides' fit bases
> differ. Arithmetic tell: with `p[-1] == 0` (measured, all four wheels, both sides) A6a's
> lateral scale obeys `f5 <= lbc` always (`Integrate2.cpp:441-450`), so the `a/lbc = 1.840`
> that was reported is impossible from that code path. **Do not quote a lateral-coefficient
> ratio from this tool until the instrumentation below lands.**
>
> **What IS established and can be relied on:** every input to the coefficient matches the
> original (`p[0x15]` 0.15, `p[0x16]` 0.0125, `p[0x1b]` 1091.8/1083.8/1084.6/536.3, `p[-1]`
> 0 on all four, `g_suspScale` 692.3 vs ~710, `le4` capped 1024 both sides); and A6a's own
> `|lat|` agrees **1.001 / 1.003 / 0.948 / 0.960** (port `wld4` vs the corrected original
> side — both are A6a's quantity, so this one IS apples-to-apples). The
> `min(le4,1024)`-clamp hypothesis is **refuted** (original is also 1024 everywhere).
>
> **Next step is INSTRUMENTATION, not a fix.** Add A6a's real lateral basis to the port's
> `[A8-ORIENT]` diag line — the vector `lac/la8/la4` (`Integrate2.cpp:448`) and the applied
> lateral scale `f5` (`:442`/`:449`) — and make `a8_wheelfit.py`'s `port_frames` read them
> instead of rebuilding `lat` from `velH`. Only then is `a/lbc` measurable on both sides.
> The `--lat-mode wheelpoint` fix to the ORIGINAL side is correct and stays.
>
> Then, in order: **U-9152** (the `+0x928` vs `g_bodyBasis` storage split),
> **`RecoverOffMesh`** (`TrackRenderer.cpp:2142-2164`, halves `car_speed_` 11-59x per 1080
> frames — bears on driving-median, the metric closest to its bound at -6.4%), and
> **U-9151** (the exe A6b binding, blocked on a CPU port of `RwMatrixMultiply 0x004c4600`;
> binding it as-is crashes the exe with `0xC0000005`).
>
> **Guards as of this session** (re-run them, don't assume): criterion (e) PASS 3/3; AI (b)
> FAIL 3/3 with `c1_median` 49/45/52.5; power-ups 11/11 decision CLEAN with `g3` contact
> diverging; oracle rule 3 GREEN; build with `rva-lint NEW=0`.
>
> **The D3 modes 3/7 hold stands.** D2 must close before it starts.

> ## ORDER OF WORK CHANGED 2026-09-29 (user decision): **D2 IS REOPENED**
>
> **Do the D2 re-close first. The D3 modes 3/7 port does not start until D2 closes again.**
>
> **Why.** The evidence that closed D2 compared a **three-opponent port run** against a
> **ONE-car original capture** (`orig_steerR.msd.provenance.json` has no `--cars`;
> `re/frida/scenario_launch.py:1739` defaults to 1). On the matched solo arm the port has
> **never** reproduced the original: slip 1500-2000 `0.1332` vs **`0.1913`** (**-30%**),
> slip 2000-2600 `0.2179` vs **`0.2498`** (**-13%**), driving-median `1818` vs **`1941`**
> (**-6%**). Evidence: `re/analysis/PLAYER_REGRESSION_2026-09-29.md` §5-§6.
>
> **This does not overturn the headline below.** There is still no player physics *regression
> between commits*. The port is short against the **original** at both commits — that is the
> D2 question the three-vs-one asymmetry hid.
>
> **The gate is now:** D2 metrics on the **SOLO arm** (`MASHED_MEASURE_SOLO=1`) against the
> original's **solo** capture, within bounds **pre-registered before the fix**. Pre-register
> first — `a-band-scored-off-regime-is-not-a-measurement` is a live precedent on this lane.
>
> **Blocking:** **U-9149** (A6b `0x00468980`'s context pointer from the stack slot at
> `0x0047093b` — dead in the exe, contradicted in the `.asi` forwarder; both `0x00468980` and
> `0x00470670` are now C2) and **U-9147** (the standing slip gap).
>
> ROADMAP §D2 carries the REOPENED block; the CLOSED block is kept below it as history.

**THE HEADLINE: there is NO player-car physics regression since D2 closed.** The
`-16% / -14% / -64%` the previous kickoff item 3 described was the controlled arm's own
asymmetry, and the D2 reference turns out to be a **one-car race**. On the reference's own
scenario HEAD reproduces `56ad3806` to `-3.1% / -0.1% / +3.3%`. Read
`re/analysis/PLAYER_REGRESSION_2026-09-29.md` and ROADMAP §D2's **second** amendment; do not
re-derive either. **U-9141 and U-9145 are RESOLVED.** (Still true — but see the REOPENED
block above: "no regression between commits" is not "matches the original".)

**Three user decisions are in force from 2026-09-29 and are already actioned** — do not re-ask them:
1. **U-9142: KEEP the spawn settle, default-ON**, `MASHED_NO_SPAWN_SETTLE=1` stays as the A/B revert.
   **AI criterion (b) is re-baselined with the settle ON** (ROADMAP §D3), and the (b) bands are NOT moved.
2. **U-9141: the D2 gate recipe has a CONTROLLED arm.** Corrected 2026-09-29b: the arm to use
   is **`MASHED_MEASURE_SOLO=1`** (no opponents — the reference's own scenario), not
   `MASHED_MEASURE_NOOPP=1` (opponents parked, a scenario neither side ran). `--max-lines 1080`
   stays. ROADMAP §D2 carries both amendments; the second supersedes the first's conclusions.
3. **D3 CLOSES BY PORTING BEHAVIOUR MODES 3 AND 7** (`FUN_00414c30` + the world-object query
   `FUN_00484c70`). **D3-R1 is no longer a carried residue** — AI (b) now fails on all three
   cars at correct speed, so there is one open criterion on three cars, not a car-1 residue.
   **ON HOLD from 2026-09-29: this port does not start until D2 re-closes** (see the ORDER
   block at the top). The decision about *how* D3 closes stands; only its start is deferred.

## READ FIRST — the tracker changed shape on 2026-09-29 (dual-copy session)

A separate 2026-09-29 session actioned the user's decision on the dual-copy audit. Three
things are different from every kickoff before it. **Do not re-derive any of them.**

1. **`hooks.csv` has a tenth column, `exe_file`** (last, so positional readers still work).
   `file` names the copy the evidence measured — by convention the `.asi`. `exe_file` names
   the TU compiled into `mashed_re.exe`. Empty = the exe has no port. Regenerate with
   `py -3.12 scripts/backfill_exe_file.py`. Four repair scripts that asserted a literal 9
   columns were fixed; everything else was already header-keyed.
   **The number to keep in mind: after the demotions, of 1184 C3/C4 rows only 203 have
   `exe_file == file`** — 183 name a different exe TU, and 798 are empty (the exe has no port
   at all, so the evidence does not cover the default build). Before the demotions the same
   split was 203 / 212 / 798 of 1213.

2. **`re/CONFIDENCE.md` has a new clause, "Which copy the evidence covers".** A row is C3/C4
   **for the shipping exe** only if `exe_file` is empty or `== file`, or the exe copy has its
   own evidence. When `exe_file != file` the level describes the `.asi` copy and **may not be
   cited in a parity, D3-criterion or DoD argument.** Fixing an exe copy by reading is
   C2-grade; a fixed copy does not restore the row.

3. **29 rows were demoted to C2** (9 × C4→C2, 20 × C3→C2) — `C4 184 → 175`, `C3 1029 → 1009`.
   **Eight of them are AI rows that D3 criterion (b) runs on**: `0x004177b0`, `0x00415e20`,
   `0x00416250`, `0x00416a30`, `0x00417da0`, `0x00418560`, `0x00418860`, `0x00443080`. Five
   more are the physics A-chain. This does **not** change the (b) measurement or the D3 gate
   table below — it changes what the trackers are allowed to claim about the bodies (b) runs
   on. Full record: `re/analysis/DUAL_COPY_FIX_2026-09-29.md`.

**And this is the part that bears on D3 (b) directly:** the AI copies the exe runs are
`Ai/AiStandalone.cpp`, not the `.asi` TUs the C3s were earned on, and they differ in ways that
plausibly *cause* (b) — `rate1` pinned `0.0f` (`AiStandalone.cpp:983`, `:1102`) makes the brake
gate permanently false and fires the curvature multiplier unconditionally; `int mode = 0`
(`:844`) kills eight targeting modes; `SteerAngleError` takes heading from velocity (`:175`)
while its own sibling at `:215` uses body-forward. Those are named in the audit's §8.2 as the
most direct levers on (b). Nobody has tried them yet — **this session changed no game code.**

A build guard now stops new pairs appearing: `scripts/lint_rva_bodies.py`, called from
`mashedmod/build.bat` before the compile step. It WARNs on the 110 known pairs in
`re/tools/dual_copy_allowlist.txt` and **FAILS the build on anything new**. If a build stops
with `[rva-lint] FAILED`, you have added a second body for an RVA — share one TU, do not
silence it. Burning that list to zero is a named ROADMAP D4 item.

## Where D3 stands

**D3 is NOT closed, and there is now exactly ONE gate failure left: AI criterion (b).**
Full record: `re/analysis/D3_DRIVE_FORCE_2026-09-29.md` and, for the session before it,
`D3_DRIVE_2026-09-28.md`. ROADMAP §D3 "D3 closure state 2026-09-29" is the authoritative
summary; do not re-derive either.

| third | state |
|---|---|
| Powerups (c) | **MET** 2026-09-28d. Sweep re-run 2026-09-29: 11 of 11 decision CLEAN, contact CLEAN on 10 of 11, `g3` DIVERGES unchanged (the known 2-query-of-546 R_FLAME residue). |
| Modes (a)-(d) | MET. Rule 3 oracle GREEN 2026-09-29, and that run *did* produce 2 segment-ends, so the rule-3 tail arm is covered. |
| AI (a), (c), (d) | MET. |
| AI (e) | **MET 2026-09-29** — all six gated values inside 0.05% of the reference against a ±2% band. U-9140 resolved: the cause was A6a's unported START BOOST block. |
| AI (b) | **NOT MET on all three cars, and now measured under matched speed for the first time.** This is the only thing between here and D3 CLOSED. |

### What is CLOSED and must not be re-opened or re-measured

- **U-9140 RESOLVED.** The 8-9x drive-force gap was A6a's `+0xbf8` start-boost block, never
  ported. Ported verbatim (`Integrate2.cpp`, cites `0x00467d3a..0x00467e44`), 5e6/wheel or
  8e6/wheel for the two least-progressed cars, measured on three separate original captures.
- **The `[A8-B14CADENCE]` question is SETTLED and the answer is "they do not differ".** The
  port's render-tick `+0xb14` equals its consumption-time value on 898/899 frames, and the
  original's snapshot is provably a single-pass value (A4 zeroes at entry and calls A6a once;
  plus `linTerm × captured +0xb1c` reproduces the original's own per-frame Δspeed on 9
  consecutive frames). Do **not** re-run a cadence probe.
- **U-9142 ANSWERED by measurement: KEEP the spawn settle.** `MASHED_NO_SPAWN_SETTLE=1` fails
  (e) on all three cars by -2.1% to -6.9%. It is no longer a user decision.
- The force→velocity conversion, the gear law, the gearbox constants, the wheel states and
  the drive-only accumulator law are all confirmed faithful. The launch is not a physics
  question any more.

## What is owed, in the order it should be taken

### 1. AI criterion (b) — the ONLY D3 blocker. Start here.

`re/tools/ai_ctrl_window.py --check <csv>` on a fresh standalone capture:

```
py -3.12 re/tools/sa_capture.py verify/<tag> 8,30,60 MASHED_MUTE=1     MASHED_TRACK_VIEW=Training MASHED_CAR=1 MASHED_ROUND=1 MASHED_WIN_POS=left-bl     MASHED_TITLE="D3 AI (b) <what>" MASHED_AI_STEPDUMP=verify/<tag>.csv
py -3.12 re/tools/ai_ctrl_window.py --check verify/<tag>.csv
py -3.12 re/tools/ai_speed_env.py   --check verify/<tag>.csv   # (e) must stay PASS
```

Where it stands on the default build (2026-09-29, `verify/d3_force_20260929/sa_b2.csv`):

| car | failing bands |
|---|---|
| 1 | `c0_distinct` 7 (floor 13), `c1_distinct` 106 (ceil 70), `steer_distinct` 112 (ceil 96), `c1_median` 48.0 (band `[0,0]`), `abs_steer_median` 48.0 (ceil 23) |
| 2 | `c1_distinct` 93, `steer_distinct` 121, `c1_median` 42.0, `abs_steer_median` 58.0 |
| 3 | `c1_distinct` 98, `steer_distinct` 116, `c1_median` 46.5, `abs_steer_median` 46.5, `accel_distinct` 1 (floor 2), `brake_distinct` 1 (needs 2) |

**Read the `c1_median` / `abs_steer_median` rows first — they are the NEW information and the
biggest ones.** The band is `[0,0]` for `c1_median`, i.e. the original's AI issues its steer
on the `c0` byte with `c1` at zero for at least half the window, and the port issues 42-48 on
`c1`. That is a sign/channel asymmetry, not a magnitude tuning problem: `c0` and `c1` are the
mutually exclusive steer pair (`+steer -> input[0]`, `-steer -> input[1]`,
`VehiclePhysicsRun.cpp` WS-A8-STEER block), so the port is steering one way far more than the
original. `c0_distinct` 7 on car 1 has been the standing symptom since 2026-09-26 and is
diagnosed in `D3_AI_RESIDUE_2026-09-27.md` (the `DAT_0089a368` spline-bank roll → curvature →
the `curv>20` multiplier at `0x0041665c`). Check whether the same chain explains the
`c1`-side pile-up before opening a new hypothesis.

Two things that must NOT be used as an excuse:

- These numbers are **worse** than the 2026-09-28 ones, and that is not a regression from the
  boost port: `MASHED_NO_START_BOOST=1` reproduces the 2026-09-28 (b) table exactly. What
  changed is that the cars now go the right speed, so the steer bands are for the first time
  scored on a car whose lookahead/curvature inputs are in the right regime. The older (b)
  numbers were not a measurement of the AI law.
- `accel_distinct` / `brake_distinct` = 1 is the D3-R1 story (unported behaviour modes 3 and 7,
  `FUN_00414c30` / `FUN_00484c70`), i.e. the port genuinely never lifts or brakes. It is a
  real band failure and it is *not* fixable inside the steer chain.

### 2. U-D3-BOOST-ARM — find the writer that arms the boost

The A6a boost FORCE law is transcribed and RVA-cited. The **arming** is a measured seed:
`+0xbf8 = 1`, `+0xbf4 = 1300`, once per AI slot, at its first real post-settle step
(`VehiclePhysicsRun.cpp`, the START BOOST ARM block). `MASHED_NO_START_BOOST=1` reverts.

`py -3.12 re/tools/findoffset.py --writes 0xbf8 0xbf4 0xbf0` puts **every** `.text` access to
those fields inside `FUN_00467650`, so the real arming store uses a base the displacement
sweep cannot see. Next commands, in order:

```
# 1. Frida WRITE watchpoint on &rec[car]+0xbf8, armed during the COUNTDOWN. A6a's two
#    +0xbf8 stores are both inside `bf8 == 1` / `== 2` arms, so while bf8 == 0 nothing in
#    A6a writes it and the first fault IS the arming instruction. Report the faulting EIP,
#    then decomp its containing function.
# 2. If the watchpoint API is unavailable: a Ghidra script walking stores whose base is
#    DAT_008815a0 + k and whose displacement is 0xbf8 - k.
```

Two things the writer would settle: whether 1300 is a constant or a value the countdown
computes from the throttle timing (`+0xbf4` rises +150/frame net through the last five
countdown frames and stops at the green), and therefore whether a human player who does not
jump the lights is boosted — the port currently does not boost slot 0, because no original
capture shows an armed player. Also open, and cheap once the writer is known:
`[UNCERTAIN] U-D3-BOOST-ORDER`, the per-car race-progress float at `0x008a96e8 + car*0x30c`
has no writer in the standalone (`FUN_00408a70` unported), so the 8e6 pair is pinned to the
grid order `{2,3}` instead of tracking race order. No effect on (e).

### 3. U-9141 / U-9145 — **RESOLVED 2026-09-29b.** Everything below this line is HISTORY.

Do **not** run the bisect the old item 3 asks for; it was run, and its premise was wrong.
Authoritative record: `re/analysis/PLAYER_REGRESSION_2026-09-29.md` (plan pre-registered at
`235e964a`, verdict at `2348614e`) and ROADMAP §D2's second amendment. In one paragraph:

- The arm was **asymmetric** — the knob was applied at HEAD and not at `56ad3806`. Applied at
  both ends, `647a5e24` (the *same commit*, both ways) moves `1931.36 → 692.07`, so the
  `-64%` is 0% a commit.
- The D2 reference is a **SOLO race**: `orig_steerR.msd.provenance.json` carries no `--cars`
  and `scenario_launch.py:1739` defaults it to 1; re-run live, `cars=1`, `SegmentCheck`
  1448/1448 with **segment-end 0** and `m1Max = -1`.
- With **`MASHED_MEASURE_SOLO=1`** at both ends: original `0.1913 / 0.2498 / 1940.59`,
  `56ad3806` `0.1374 / 0.2181 / 1760.49` (3/3), HEAD `0.1332 / 0.2179 / 1818.47` (4/5) —
  **HEAD vs `56ad3806` = `-3.1% / -0.1% / +3.3%`. No regression.**
- U-9145's coupling was two channels, **neither a shared physics global**: (A) the harness's
  steer-hold onset was on the real clock while the sim runs on a real-time accumulator —
  **fixed**, it now counts sim steps; (B) the opponents get the PLAYER eliminated at
  `race_time_` 2.1-2.6 s, which freezes `race_[0].gate` into the player's own off-mesh
  re-aim. `g_torqueRingPhase`, `g_suspScratch` and the pickup field are all REFUTED.

**What is newly owed, and where it sits in the order.** Item 1 (AI (b)) is still first, and
the closure path is now decision 3 above — port behaviour modes 3 and 7. Then, in this order:

- **U-9147** — on the matched (solo) arm the port is **~28% short on `slip 1500-2000` at BOTH
  commits** (0.1374 / 0.1332 against 0.1913). Not a regression; a D2-era gap the recipe's
  scenario mismatch concealed. Next command: `MASHED_MEASURE_SOLO=1` + `MASHED_COUPLING_DIAG=1`
  and a per-frame per-wheel lateral-force diff against `orig_steerR.msd`, the way
  `A8_velocity_vector_motion_20260825.md` follow-up 27 does. Re-baselining the ROADMAP §D2 row
  on this arm is a **user decision** — do not do it unasked.
- **U-9146** — does the ORIGINAL also eliminate a stationary player at ~2.5 s with three
  opponents? The reference is solo, so it cannot say. Next command:
  `py -3.12 re/frida/scenario_launch.py --oracle --rule 0 --cars 4 --poke-ctrl-slots --statediff-drive --statediff-drive-late --statediff-steer 1 --hold 38`,
  then read `segment-end` / `deadMax` from `log/rules_oracle_rule0.json`. Regardless of the
  answer, `race_[0].alive → race_[0].gate → the off-mesh re-aim` (`TrackRenderer.cpp:2808`)
  has no original counterpart.
- **U-9148** — one HEAD solo run in five lands in a second attractor, so a real-time-keyed
  input survives the sim-clock fix on the HEAD side. Next command: two
  `MASHED_PLAYERTRACE=1 MASHED_MEASURE_SOLO=1` runs until both attractors are sampled, then
  diff the two `player_trace.log` files for the first differing FIELD.
- **U-9149 — take this one WITH U-9147; it is the live lead for it.** A4 loads A6b's ESI from
  `[esp+0x3c]` (`0x0047093b`) two instructions after loading A6a's from EDI (`0x00470934`),
  so A6b's context is never null and the exe's `nullptr` at `VehicleControl.cpp:195` makes
  the whole rotation-apply dead — airborne auto-level and the velocity-align rotation never
  run. The `.asi` C4 forwarder's `ESI = record` assumption is **also** unsupported by that
  instruction. Next command: decompile `FUN_00470670` and read the third argument of its
  `FUN_00468980` call at `0x00470943`. **Do not invent a matrix to pass.**

### 3b. The dual-copy leads (`aa4795af`) — four are DONE, one is open

Judged against the original 2026-09-29b, `PLAYER_REGRESSION_2026-09-29.md` §7.3, commit
`3e4fba77`. **Do not re-do these four**; the fifth is U-9149 above.

- **A5 `0x0046ddb0` constants — FIXED, and there were EIGHT, not the four the audit named.**
  All were 6-significant-digit truncations of exact round numbers (1/3000, 1/300, -1/30000,
  5e-6, 1/3, 0.99, 1e-4, 2^-31), now `asFb(bits)`. Re-audit with
  `audit_consts.py`-style bit comparison against `original/MASHED.exe`: **30 exact, 0
  mismatch**. Worth running the same check on any other header that annotates `_DAT_`
  addresses — this class of slip is invisible to review and trivial to detect.
- **A3 `0x0046b540` output stride — FIXED to `0x40`**, settled from `add ebx, 0x40` in all
  three loops, not from symmetry. Measured inert on this recipe.
- **A6a `0x00467650` gear clamp — FIXED to bound 5** (the original declares `local_54[5]` and
  reads it unclamped). Measured currently inert: `gear` took only 0..4 over 79,356 frames.
- **`CarCarContacts.cpp:192-195` — REFUTED as the U-9145 coupling.** `0x00469df0` has **zero
  call sites** in the whole tree. The empty `Rw_MatrixDerive` is latent dead-code damage and
  belongs to the audit's own §8.2 item 6 pass.
- **None of the four moves U-9147's ~28% slip gap**, though the constants are demonstrably
  live ((e) moved 0.1 on two cars). That is a useful negative — do not re-search there.

---

**HISTORY (2026-09-29, first pass). Superseded — kept for the audit trail.**

### 3-old. U-9141 — the controlled arm EXISTS and it found something. Redo the bisect on it.

**Read `re/analysis/D2_CONTROLLED_ARM_2026-09-29.md` §3 before touching this.** The arm and
its pass rule were pre-registered at `1d0ca916` before any run; the results are at `4938adba`.
Do not re-derive either, and do not move the bounds.

**The arm works.** `MASHED_MEASURE_NOOPP=1` (opponents not updated) plus `--max-lines 1080`
gives a run-to-run spread on `slip 1500-2000` of **exactly 0** on 3/3 runs at both HEAD and
`56ad3806`, where the uncontrolled arm still spreads 0.0088. The knob is proved inert when
unset (a default run on the build containing it reproduces the pre-knob build to every
printed digit). So the instrument question is settled.

**What it found, and it inverts the 2026-09-28 conclusion.** With the opponents absent at
BOTH ends, HEAD is `-16.0% / -14.0% / -64.2%` off `56ad3806`
(slip 1500-2000 0.1609 vs 0.1916, slip 2000-2600 0.2296 vs 0.2669, driving-median 691.0 vs
1932.1). The 2026-09-28 bisect concluded *"no commit in `56ad3806..HEAD` edits the player's
solver, therefore the drift is the instrument"* — **on a controlled instrument that does not
hold.** There is a real player-side difference, and the uncontrolled recipe could not have
seen it: its 0.128..0.177 spread brackets both 0.1609 and 0.1916.

`56ad3806`'s controlled arm reproduces the ROADMAP §D2 row on 2 of 3 gated statistics plus
`av.y` to four decimals (slip 0.00%, slip +0.04%, av.y exact) and misses the driving-median
by +2.39% against a ±2% bound. **D2 is not reopened** — that overshoot is already in the
record at §3.1 of the 2026-09-28 note, so the row's `1887` is itself ~2% low at its own
commit. Re-baselining that figure is a **user decision**; do not do it unasked.

**What is owed, in order.**

1. **Resolve U-9145 first — it may be the whole of U-9141.** The opponents move the PLAYER's
   driving-median 691 → 2538 (3.7x) on the same build and recipe, and `VehicleCarCarContact`
   (`0x00469df0`) has **zero callers** in the port, so no car-car path exists to do it with.
   The coupling is shared mutable state. Per-global A/B on the controlled arm, one temporary
   env-gated diag at a time, removed afterwards; candidates and citations in §3.5 of the note
   (`g_torqueRingPhase` `DAT_007f101c` and A4's steer ring `+0x1ac`/`+0x270` at `0x00470670`
   is the first one to try). **One run per configuration decides**, at a spread of 0.
2. **Then re-bisect `56ad3806..HEAD` on the controlled arm**, one run per commit, over the 17
   `mashedmod/`-touching commits of `D3_DRIVE_2026-09-28.md` §1.2:
   ```
   py -3.12 re/tools/statediff/a8_run_port.py verify/<tag> 50 -MASHED_REAL_PHYSICS \
       MASHED_MEASURE_NOOPP=1 MASHED_TITLE="U-9141 controlled bisect <sha>"
   py -3.12 re/tools/statediff/a8_slip_axis.py --orig verify/a8_steer_20260824/orig_steerR.msd \
       --port verify/<tag>/motion_diag.log --max-lines 1080
   ```
   Classification fixed in §3.5: `slip 1500-2000` `>= 0.185` GOOD, `<= 0.170` BAD, between =
   INDETERMINATE and gets a second run. Commits before `09a73dc6` have no opponent loop, so
   the knob is a no-op there.

**Two premises to carry, both already paid for.** The §1.3 one-run-decides rule was refuted on
the UNCONTROLLED arm (0.1840 and 0.1283 from one build) and is sound on this one — do not
re-litigate it in either direction without citing which arm you mean. And one asymmetry
remains in the arm: at `56ad3806` the opponents are moved by the pre-D3 kinematic Option B
model, whereas the knob leaves them PARKED; if U-9145 is real, the HEAD end should use the
Option B treatment instead (§3.4's `NOAIPHYS`, measured 0.1736 / 0.1561 / 0.1774).

## Added 2026-09-29b (the player-regression session)

- **`MASHED_MEASURE_SOLO=1`** (`D3d9Render/TrackRenderer.cpp`, **both** spawn sites — the
  car-load spawn AND `StartRound`'s `ai_cars_.assign`) — MEASUREMENT HARNESS ONLY, default-OFF:
  spawns **no** opponents, so `ai_cars_` stays empty and `UpdateRace`, the `RaceCamera`
  framing, `ParticipantCount()` and the rule engine all see a one-car race. **This is the arm
  the D2 gate should use**, because the original-side reference was captured that way. Verify
  it took: `mashed_re.log` must log `MATCH-SEED … participants=1`. Gating only one of the two
  sites leaves it silently inert — that mistake cost a whole bisect's worth of mislabelled
  runs this session.
- **The steer-hold onset is counted in SIM STEPS, not real seconds** (`exe_main.cpp`,
  `steerHoldApply()`). It used to be decided once per RENDER frame from a real clock while the
  car sim runs on a real-time fixed-timestep accumulator, so the sim step at which the held
  lock began tracked CPU load. Same 4 s threshold. This is what made the default `a8` arm
  deterministic — four consecutive commits now reduce bit-equal.
- **`MASHED_PLAYERTRACE=1`** (`D3d9Render/TrackRenderer.cpp`) — default-OFF per-sim-step
  `%.17g` dump to `./player_trace.log`: world position, `in.dt`,
  `race_[0].gate/laps/progress/alive`, and record floats `+0xb14` / `+0xb1c` / `+0x9e4`, one
  line before and one after `UpdateRace`. **Diff two of these and read the first differing
  FIELD** — that single recipe found both U-9145 channels and refuted four named suspects. It
  is far cheaper than a per-global A/B and it cannot be fooled by a knob that is inert.
- **A `git checkout HEAD -- mashedmod/` restore inside a bisect script will silently delete
  your uncommitted edits.** It ate two of them this session. Commit before probing.
- `a8_run_port.py` at 50 s discarded two boots that stalled in the frontend (`NAV_DEMO
  phase=0 00_challengeselect` in `mashed_re.log`); at **90 s** the exe exits on its own with a
  complete race. Use 90 and re-run once on an empty log.

## Added 2026-09-29 (U-D3-DRIVE-FORCE + the D2 controlled arm)

- `MASHED_MEASURE_NOOPP=1` (`D3d9Render/TrackRenderer.cpp`) — MEASUREMENT HARNESS ONLY, on the
  `MASHED_STEER_HOLD` precedent: sets the per-opponent update loop's bound to 0 and does nothing else.
  Default-OFF and proved inert when unset. **Only legitimate on the D2 controlled arm, where BOTH ends
  of the comparison run it.** Do not use it to make any other number look better.
- `--max-lines N` on `a8_slip_axis.py` and `a8_momentum.py` — truncates the PORT side to the first N
  logged frames, before the regime filter and before the spike median. The ORIGINAL side is
  deliberately never truncated. N = 1080 is the D2 controlled arm's fixed 18.0 s window.
- Frame count == simulated time in the standalone: the chain dt is pinned at `frameMs = 50`, measured as
  a single distinct `linTerm=1.66667e-05` over all 3596 samples of
  `verify/d3_force_20260929/cad1/friction_diag.log`. Frame COUNT varies with machine load (25 vs 30 fps
  gave 1268 vs 1497 frames in the same 50 s wall clock), which is why pinning it is the right control.

- `MASHED_NO_START_BOOST=1` — revert arm for the A6a start boost. Reproduces the 2026-09-28
  criterion (e) and (b) numbers exactly, which is what makes any before/after here legitimate.
- `MASHED_GAMEMODE_STUB=0` — revert arm for `Fi_GameMode()` 6 → 0. Its only live consequence
  is A6a's `+0xbf4` timer site; audited call-site by call-site in `ForceIntegratorStubs.cpp`.
- `Fi_UpdateBoostOrder()` (`ForceIntegratorStubs.cpp`) — ported `FUN_00470c70`
  `0x00470e2e..0x00470f0a`: seeds `DAT_0088e660..66c` with 0,1,2,3 and sorts descending by
  the per-car progress float. Pinned to `{2,3}` in the standalone because that float has no
  writer (see U-D3-BOOST-ORDER above).
- `VehicleControlIntegrate` gained a `car` argument, so A6a's `param_1` is the real car index
  instead of a hardcoded 0. That resolves the `[UNCERTAIN]` that was on `VehicleControl.cpp:187`.
- `re/tools/findoffset.py` is the right tool for "who writes struct field +0xNN" and it was
  what proved the boost arming writer is NOT reachable by a displacement sweep. Read its two
  CAVEATS before citing a hit.
- **`MASHED_AI_STEPDUMP` needs `MASHED_TRACK_VIEW=Training`.** Without it the standalone sits
  in the frontend, never calls `AiStepDump()`, and you get three screenshots and no CSV with
  no error. Cost this session one capture. The full recipe is in
  `verify/d3_force_20260929/PROVENANCE.txt`.
- `a8_run_port.py` moves `motion_diag.log` but **not** `friction_diag.log`. If you run with
  `MASHED_COUPLING_DIAG=1`, delete `friction_diag.log` first and move it yourself afterwards.

## Tools added in the 2026-09-28 session

- `re/tools/ai_speed_env.py` — the criterion (e) scorer. Holds the band as `REFERENCE` /
  `BAND_PCT` / `GATED`. `--check <csv>`, `--envelope`, `--json`.
- `re/tools/statediff/msd_fields.py` — print arbitrary vehicle-record fields per frame out
  of an MSD1 capture. `<msd> 0x490:i 0x494:i 0x498:f ... [--every N] [--first N]
  [--distinct]`. This is what read the original's gearbox and suspension state.
- `MASHED_MOTION_DIAG` now also prints `b0c` / `gb498` / `gb49c`.
- `MASHED_MOTION_DIAG_AI=1` logs the gearbox and launch fields for the **opponent** slots to
  `motion_diag_ai.log` — the widening `D3_SPEED_GAP` §6.3 asked for. Separate file on
  purpose: the `a8` reducers key on `reseed=` … `wax=[…]` and assume slot 0.
- **`MASHED_TITLE` (from `a9da810a`, another session) — use it on every run.** The standalone
  window title is now `Mashed RE | <label> | <state>`, where the label is `MASHED_TITLE` if
  set and otherwise the run's `MASHED_*` env vars. Both harnesses forward bare `KEY=VAL`
  arguments into the child env, so it needs no code change:
  ```
  py -3.12 re/tools/statediff/a8_run_port.py verify/<tag> 50 -MASHED_REAL_PHYSICS \
      MASHED_D3_NOOPP=1 MASHED_TITLE="U-9141 bisect <sha>"
  py -3.12 re/tools/sa_capture.py verify/<tag> 8,65 MASHED_MUTE=1 ... \
      MASHED_TITLE="U-9140 cadence check"
  ```
  This session ran without it and had several near-identical windows open at once while
  bisecting; label them.

## Standing gotchas this session paid for

- The criterion (e) window and the criterion (b) window are the same span
  (`ai_ctrl_window.py`, 220 calls from the first `c4 != 0`), so the two cannot disagree
  about which calls are the race. Keep it that way.
- `flag_a368` (`DAT_0089a368`) is **not binary** on the speed-gap recipe — it takes 0, 1 and
  2. The reference regime is 0 on every window call. A capture with `regime0=0` on a car is
  re-taken, not scored. Two of four originals taken this session landed off-regime.
- The original is **deterministic** on this recipe: 10 regime-0 captures agree to the printed
  0.1 on `launch`, `ft_median_m0` and `ft_median`; only `start_frame` varies (802..891).
  A zero-width envelope is why criterion (e)'s band had to be inherited (2%) rather than
  measured, and that is stated in the note rather than hidden.
- The standalone is deterministic too on the AI capture: `sa_ctl` reproduced the 2026-09-27
  `sa_d1` exactly, which is what makes a one-capture before/after legitimate here.
- Four of this session's `a8` runs produced 38-, 79-, 302- and 361-line logs, i.e. races
  that ended in under a second. Those are truncated boots, not samples; the shortest complete
  race observed is 1023 lines. Discard below ~900 and say so.
