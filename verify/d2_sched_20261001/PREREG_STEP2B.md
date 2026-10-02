# D2 attempt 14, STEP 2B — PRE-REGISTRATION: the bounce lane, by state-matched response

Written and committed **BEFORE any reduction run and before any game launch**, after STEP 2A
closed ([`RESULT_STEP2.md`](RESULT_STEP2.md), commit `3fdabe3e`). HEAD at registration:
`30662a61`. **Not to be amended** — a failing gate stops the lane and is reported.

## 1. Why this step exists and what it may NOT do

`PREREG_STEP2.md` §1 registered a bounce-anchored fallback. Step 2A's result makes it the live
target: the launch is faithful to **1.44 %** and **D2's defect is the recovery** — both arms
peak within 1.3 %, both crash to the same trough, the ORIGINAL returns to a median of **1828.5**
and the PORT's ceiling is **91.4**.

**The hard constraint this step must respect.** After the first bounce the two trajectories are
genuinely different: different positions, different headings, different wall contacts. A
per-frame cross-side comparison of force or velocity fields in that regime reports the
**trajectory**, not the implementation — `collateral.py`'s own docstring says so and §26.10 is
the same lesson. **This step therefore does NOT run a per-frame field diff on the recovery.**
It runs a **state-matched response test**, which is the only cross-side instrument that stays
valid once two trajectories separate: *given the same state, do the two sides produce the same
per-frame change?*

## 2. Precondition, already verified and cited

**P1 — same track.** `mashed_re.log` reads
`R4 track load OK: original/TOASTART/TRACKS/training.piz - tris=11469 verts=12767 sectors=42
mats=24 textured=21 radius=265.32 props=12 (instances=33) gates=30`, and the reference capture's
`--track` defaults to `0 = Training` (`re/frida/scenario_launch.py:1773-1777`, confirmed by
`orig_solo3.launch.txt:2` `track=0 mode=10 cars=1`). Both arms are on **training.piz**.

## 3. The anchor, and its known-answer gate

> **`B` = the first frame, searched forward from each arm's own `+0x9e4` peak within
> `d` in `[0,140]`, at which `+0x9e4` falls by more than 25 % from the previous frame.**

- **GE (known answer).** `B` must reproduce the two bounce frames already on record **to within
  3 frames**: the ORIGINAL's first wall hit is its frame **980** (§16.6's twelve-hit list, also
  §20.14's table) and the PORT's is its frame **81** (§20.14). If `B` lands elsewhere, the
  anchor is not finding the bounce and nothing below is read.

## 4. The window, chosen to KILL the steer confounder

The steer angle `+0x1a8` ramps from each arm's own **release**, not from its bounce, and the two
arms bounce at different release-relative frames (ORIG `d = 94`, PORT `d = 80`). At their own
bounces their steer angles therefore differ:
`17.07471 + 0.141113 * 94 = 30.34` against `17.07471 + 0.141113 * 80 = 28.36`, **6.5 % apart**.
**A bounce-aligned comparison is confounded by the steer ramp** — the same trap as
`band-on-speed-compares-different-moments`, in a new guise.

> **The window is `d` in `[119, 300]`, release-relative, on both arms.** Both saturate `+0x1a8`
> at `33.86719` on their own 120th steering frame, i.e. at `d = 119`, so **inside this window
> both arms carry the identical, saturated steer angle** and the ramp cannot fake anything.
> Both are post-bounce (ORIG `B` at `d = 94`, PORT at `d = 80`) and both enter the window near
> 100 speed.

**C1/C2 — confounders reported, not assumed away.** The steer angle at `B` on each side; and
within the window, `+0x1a8`, `+0xb24` and the gearbox pair `+0x490`/`+0x494` per side. **Any
state bucket in which `+0x1a8` or `gear` differs across the sides is marked CONFOUNDED and its
row is not read.**

## 5. The channels

All are record dwords on **both** sides. ORIGINAL from
`verify/d2_reopen_20260929/orig_solo3.msd`; PORT from
`verify/d2_sched_20261001/ph1/a6a_dump.log` (`snap.vel` = `+0x9b0/+0x9b4/+0x9b8`, `snap.bf` =
`+0x9d4/+0x9d8/+0x9dc`, `snap.sp` = `+0x9e4`, `snap.av` = `+0x9bc/+0x9c0/+0x9c4`, `snap.gc` =
`+0x9e0`, `snap.steer` = `+0x1a8`, all one record read per frame at the `.msd`'s own phase per
`VehiclePhysicsRun.cpp:1284-1285`) and `ph1/motion_diag.log` for `gear`/`gtmr`/`b14`/`wf`.
`ph1` is bit-identical to attempt 13's `sc1` on all 1627 shared lines (STEP 2A §1).

**State** (what the car is in):
- `S1 = cos(fwd, vel)` = `dot(+0x9d4..+0x9dc, +0x9b0..+0x9b8) / (|fwd| * |vel|)` — §20.14's
  channel, and the quantity the reverse gate `kRevDot = -0.1` (`_DAT_005cd0fc` = `0xbdcccccd`,
  `BodyOrientationIntegrate.cpp:287`) tests.
- `S2 = +0x9e4` (speed).
- `S3 = +0x9e0` (grounded).

**Response** (what it does next), strict tier order, upstream first:
- `R1 = d(+0x9e4)` per frame — does speed grow or decay.
- `R2 = d(S1)` per frame — does the nose come back into line with the velocity.
- `R3 = +0x9c0` (`av.y`, the yaw rate) — the quantity the inverted-torque gate acts on.

## 6. The buckets, and the occupancy gate

Frames in the window with `S3 >= 4` (grounded), bucketed on **(S1, S2)**:

- `S1`: `[-1,-0.5)`, `[-0.5,-0.1)`, `[-0.1,0.3)`, `[0.3,0.7)`, `[0.7,0.95)`, `[0.95,1]`
- `S2`: `[0,50)`, `[50,150)`, `[150,400)`, `[400,1000)`, `[1000,3000)`

> **GF — a bucket is READABLE only if `n >= 20` on BOTH sides.** Every bucket is reported with
> `n`, **median speed** and **median frame index** per side (memory
> `band-on-speed-compares-different-moments`), readable or not. **GF passes only if at least 3
> buckets are readable**; with fewer, the test has no power on this arm and that is the result.

## 7. The tolerance and the FIRST-DIVERGENCE rule

For response channel `R` in bucket `b`:

> `tol(R, b) = max( spread(R, b), 0.10 * |orig_median(R, b)| , eps(R) )`
>
> `spread(R, b)` = `|solo3_median(R,b) - solo4_median(R,b)|`, the ORIGINAL's own same-arm
> variation **in that bucket** (its two captures are the registered floor pair; STEP 2A measured
> their whole-record floor at exactly 0). `eps(R1) = 0.5` speed units/frame, `eps(R2) = 0.005`,
> `eps(R3) = 0.01` — each one tenth of the smallest cross-side difference §20.14 reported on
> that family, so a bit of rounding cannot name a term.

> **The first diverging term is the lowest-tier channel `R` that exceeds `tol` in at least one
> READABLE, NON-CONFOUNDED bucket, with every strictly-upstream channel inside `tol` in that
> same bucket.** Ties broken by the largest `|gap| / tol`.

Report: the bucket, `n` / median speed / median frame index on both sides, both medians, the
ratio, and **the RVA of the writer** of the channel. If the writer cannot be cited it is
reported `[UNCERTAIN]` and **without** a writer, never with a guessed one.

## 8. The gates. A gate that fails STOPS the lane; none may be amended.

- **GE** — the anchor reproduces frames 980 (ORIG) and 81 (PORT) to within 3. (§3)
- **GF** — at least 3 readable buckets. (§6)
- **GG — the verifier has a passing baseline** (memory `verifier-needs-passing-baseline`). The
  identical test is run with arm B = `orig_solo4.msd`.
  > **GG passes iff that control reports NO diverging response channel in ANY readable
  > bucket.** If the original's own repeat trips the rule, the rule is too tight and its output
  > on the real arms is meaningless.
- **GH** — within the window both arms' `+0x1a8` is `33.86719` on **every** frame. If either
  arm leaves saturation inside the window, the window is not what §4 claims and the lane stops.

## 9. The two verdicts, both pre-committed

- **(A) A response channel DIVERGES in a readable, non-confounded bucket.** Then the defect is
  in the **per-frame physics of the stuck regime**. Name the term + RVA, **test the hypothesis
  on the running ORIGINAL before writing any code** (session directive; §22.2's pattern), and
  only if it survives author the fix with the **promotion leg** (memory
  `fix-briefs-carry-a-promotion-leg`): shared-TU body at the RVA, `run_diff` /
  `run_verify_hook` in the same child, `re-classify` with only what was earned, dual-copy guard
  **`NEW=0`**, then re-score 3x against the **unchanged** §3 bounds with `participants=1`.
- **(B) Every readable bucket AGREES within tolerance.** Then **the per-frame physics of the
  stuck regime is faithful and the divergence is in the STATE TRAJECTORY** — the two cars are
  on different lines into and out of the wall. That is a real, reportable result and it
  promotes the step-2A finding from "a 15-frame cosmetic latency" to "the proximate cause of a
  different line", because the latency makes the port carry **`15 * 0.141113 = 2.12` degrees
  less steer at matched speed** through the whole approach. In that case this step **measures
  that line difference and reports it**, authors **no** fix (the trigger is still unlocated,
  **U-9174**), and names the next lane.

**Pre-committed either way:** no fix is authored on a confounded or unreadable bucket, and the
§3 bounds (`slip 1500-2000` `0.18855..0.19635`, `slip 2000-2600` `0.24488..0.25487`,
`driving-median` `1904.70..1982.44`) are not renegotiable.

## 10. Collateral and process

The attempt's collateral review is already recorded (`RESULT_STEP2.md` §5) and is **extended**,
not replaced, if this step produces new arms. Muted launches, `MASHED_TITLE` on every run,
`MASHED_WIN_POS=primary-bl`, `--poke-ctrl-slots` on any race capture, never `MASHED_NAV_DEMO`,
Frida **entry hooks only**, own PIDs tracked and only those killed, build only via
`mashedmod\build.bat` from PowerShell, trackers only through `re-classify`, CRLF preserved,
commits as `nanofives` with explicit pathspecs, nothing pushed.
