# D2 attempt 20 — STEP 3 (the fix) and STEP 4 (re-measure)

Pre-registration `PREREG_STEP34.md`, committed **unrun** at `35a4c7b4`. Build HEAD at
capture time `35a4c7b4` + the fix. Every figure carries `n`, median speed and `d`
(`L = 0`; ORIG `R = 886` for `orig_solo3.msd`, PORT `R = 1`).

---

## 1 The fix, as landed

`ProduceTerrainBatch` (`mashedmod/src/mashed_re/Collision/ContactProducer.cpp:67`): the
admission test is now **spatial** — the triangle's AABB grown by the query `radius` must
contain the query centre — and that is the **default**. The old plane-distance test is kept
only as the explicit diagnostic arm `MASHED_D2_BATCHMODE=plane` so the paired collateral has
a pre-fix side.

**No new numeric constant**: same `center`, same `radius` (the record's own `+0x4a4`).
No knob on the shipping path, no clamp, no fitted value, no threshold chosen to close a
branch. The justification is geometric and was registered before the change landed: a plane
is unbounded, so a plane-distance test is not a locality test, and `ProduceTerrainBatch`
stands in for `FUN_00538c80`, a broadphase whose output is the triangles **near** the query
sphere. The AABB form is a conservative superset of sphere-vs-triangle, so it cannot drop a
triangle the narrow phase would have used.

**Guards.** `scripts/lint_rva_bodies.py`: `allowlisted=122 NEW=0` — **PASS**.
`mashedmod\build.bat`: `[asi] all 422 objects up to date` on the final build, and all five
edited TUs (`ContactProducer.cpp`, `ContactStubs.cpp`, `CarWorldContacts.cpp`,
`WheelContactSolver.cpp`, `D2SinkProbe.cpp`) are **exe-only**.

**PROMOTION, stated plainly: no C-level moves and none is requested.**
`ProduceTerrainBatch` is port-only scaffolding with **no RVA of its own**, so `run_diff`
path1 and `run_verify_hook` path2 have nothing to install and nothing to call — there is no
export, and manufacturing a diff here would be a false claim. Levels before -> after:
`0x0046f6c0` **C2 -> C2**, `0x0046cc40` **C2 -> C2**, `0x00468d80` **C2 -> C2**,
`0x00538c80` **C1 -> C1**, `0x00470c70` **C2 -> C2**. The evidence offered is behavioural
and cross-side: the ORIGINAL's own per-wheel measurement at matched `d` (`K = 4`, `S1 = 4`,
state word `(1,1,1,1)` on 58/58 calls) reproduced by the port, plus gate KA-O at 99.9787 %.

## 2 The pre-fix arm reproduces attempt 19 exactly — the A/B is a real control

`MASHED_D2_BATCHMODE=plane`, runs `pre1` and `pre2`:

| metric | pre1 | pre2 | attempt 19's published figure |
|---|---|---|---|
| slip 1500-2000 | **0.1983** (n=19) | **0.1983** (n=19) | **0.1983** (n=19) |
| slip 2000-2600 | **UNSCORABLE** (n=0) | **UNSCORABLE** (n=0) | **UNSCORABLE** (n=0) |
| driving-median | **1019.77** (n=76) | **1019.77** (n=76) | **1019.77** (n=76) |
| median `d` | 85 / — / 79 | 85 / — / 79 | 85 / — / 79 |

Bit-for-bit. So the legacy arm is the pre-fix build and every post-fix number below is
measured against a verified baseline, not against a remembered one.

## 3 STEP 4 verdicts

### (a) `wcs_drift` at `d = 222..250`, and `T_post` — **PASS**

Run `a1` (default build + the default-OFF sink, `MASHED_D2SINK` without `_SM`), 1627
motion-diag lines, `participants=1` confirmed from the game's own
`MATCH-SEED rule=0 participants=1 teams=0 seed=6 engine=1`.
`a19_split.py` gates: CV **PASS** (1626 frames, `miss`/`dup` 0 for all five once-per-frame
tags, orphans 0/0, `nsub {3: 1626}`, 0 bad lines), KA1 **100.0000 %**, KA2 **100.0000 %**
(worst 0.000e+00), KA3 **52.7060 % FAIL** (the same `%g` six-digit channel limit attempt 19
measured and did not re-threshold; it forms no producer delta), EV **PASS**
(n = 29, gnd4 100.00 %, ctrl>0 100.00 %, median speed 737.3, median frame 237).

```
wcs_drift firings:  in the window 0    in the WHOLE capture 0     (was 47 in the window)
SolveWheelContacts  0x0046f6c0   n=29  median +0.000000   share -0.00 %   (was -23.194031 / 84.33 %)
grip-clamp #6       0x004687f0..0x0046897b  n=29  median -0.612549  share 100.23 %
every other site                           n=29  median +0.000000   share -0.00 %
D_sub[0] / D_sub[1] / D_sub[2]             n=29  median +0.000000 each
```

| | ORIG | PORT pre-fix | PORT post-fix | bars |
|---|---:|---:|---:|---:|
| `T_post` at `d = 222..250` | **-0.00464** | -27.50393 | **-0.61116** | — |
| `\|delta\|` vs ORIG | — | 27.49929 | **0.60652** | 5.5560 / 8.6373 |

**`T_post` is inside both registered bars** — the first time in the D2 re-open — and the
carrier site is exactly 0. The remaining term is grip-clamp #6 at -0.6125, which is now
100 % of a term that is itself 45x smaller than it was.

### (b) launch — **PASS**

`a8_launch.py`, `orig_solo3.msd` vs `s1`:

```
ORIGINAL  frames 2333  R=886  peak 1832.40 at d=95  trough 85.45 at d=101  +0xb14 engages d=15
PORT      frames 1625  R=  1  peak 1835.50 at d=95  trough 83.43 at d=106  +0xb14 engages d=15
best-fit lag over d=16..95 (n=80):  L=0: 0.19 %   L=14: 49.03 %   L=15: 51.27 %   L=16: 53.43 %
BEST L = 0 at 0.19 %
```

Identical to attempt 19's launch figure (0.19 %, peak 1835.50 at `d` = 95). **No regression.**

### (c) recovery under H1 — **PASS, both legs, having failed before**

H1 = `>= 50 %` of the 400 post-trough frames `>= 100` **AND** median `>= 900`.

| | fraction `>= 100` | median | max |
|---|---:|---:|---:|
| ORIGINAL | 398/400 = **99.5 %** | **1333.9** | 2478.3 |
| PORT pre-fix (attempt 19) | 243/400 = 60.8 % | **132.8** | 737.9 |
| **PORT post-fix** | **398/400 = 99.5 %** | **1362.8** | 2374.0 |

The port now reproduces the original's recovery fraction **exactly** and its median to
2.2 %. The median leg went from **132.8** to **1362.8**.

### (d) the three D2 metrics, 3 runs, against the UNCHANGED `d81a8df6` bounds — **2 of 3 PASS**

`s1`, `s2`, `s3` are identical on all three metrics (the port is deterministic on this arm).
ORIG reference `verify/d2_reopen_20260929/orig_solo3.msd`.

| metric | bound | PORT post-fix | n | median speed | median `d` | PORT pre-fix | ORIG | ORIG n | ORIG median `d` | verdict |
|---|---|---:|---:|---:|---:|---:|---:|---:|---:|---|
| slip 1500-2000 | 0.18855 .. 0.19635 | **0.1914** | 355 | 1777.40 | **605** | 0.1983 (n=19) | 0.1937 | 314 | 720 | **PASS** |
| slip 2000-2600 | 0.24488 .. 0.25487 | **0.2521** | 571 | 2215.78 | **720** | UNSCORABLE (n=0) | 0.2498 | 540 | 909 | **PASS** |
| driving-median | 1904.70 .. 1982.44 | **1879.59** | 1335 | 1879.59 | **637** | 1019.77 (n=76) | 1937.89 | 1154 | 824 | **FAIL** by **25.11** (**1.3 %** low) |

**§26.10's median-frame guard, applied before any magnitude is read.** Pre-fix the port's
scored populations sat at median `d` **79-85** against the original's **720-909** — three
bands that were not the same regime at all, which is why attempt 19 declared its magnitudes
unreadable. Post-fix the port's medians are **605 / 720 / 637** against **720 / 909 / 824**:
the same ordering, the same regime, and within 15-25 %. The guard no longer disqualifies
the readings, and the two slip metrics land inside bounds that were set from the original.

Populations also stopped being degenerate: `n` 19 -> **355**, **0 -> 571**, 76 -> **1335**.
The 2000-2600 band was **unscorable on the port for the whole D2 re-open** and is now
scored, in bounds.

**`av.y` cross-check**, same filter as the slip metric: band 1500-2000 ORIG **+1.142** /
PORT **+1.124**; band 2000-2600 ORIG **+1.466** / PORT **+1.467**. The yaw channel is
matched where the slip is matched.

## 4 Collateral

### Leg 1 — paired same-side, pre-fix vs post-fix, with floors

`collateral.py --a kv:pre1 --b kv:s1 --floor-a kv:pre2 --floor-b kv:s2 --speed sp`:
76 fields, 1626 aligned frames, **A-only 0, B-only 0**, noise floor from 1625 A-pair and
1626 B-pair frames. **53 of 75 paired fields diverge, first past the floor at frame 239
(`d` = 238)** — the carrier window itself.

**Those 53 are all inside the fix's write set, and the honest statement is that this leg
cannot produce an outside-scope row by construction.** The fix changes which triangles the
classifier sees, so its transitive write set is the whole vehicle-dynamics state, and
`motion_diag.log` contains **only** vehicle-dynamics fields. Reporting "53 outside-scope
rows" would be an artefact of the channel, not a finding. The measured facts worth keeping
are the shape and the onset:
- the divergence **starts at `d` = 238**, inside `d = 222..250`, and not earlier — so the
  fix does not perturb the launch phase, which is what (b) independently confirms at 0.19 %;
- both same-arm floors are **0** on every field (`pre1`/`pre2` and `s1`/`s2` are
  bit-identical), so the port is deterministic on this arm and nothing here is noise.

### Leg 2 — cross-side banded at matched `d`

`--mode banded`, A `orig_solo3.msd`, B `s1`, floor-A `orig_solo4.msd`, floor-B `s2`, anchor
`msd+0x9e0:ge:4.0`, 18 field maps. The anchor is satisfied on both arms (A frame 1, B frame
2) and 1625 frames align — attempt 19 could not read a single band here.

**3 of 7 bands are now ON-regime** (`500-1000` medFrm 1514/796, `1000-1500` 1530/822,
`2000-2600` 1794/940); `100-150`, `150-260`, `260-500` and `1500-2000` are marked `!!`
OFF-REGIME and **no row in them is read**, per §26.10. Attempt 19 had **all six** bands
off-regime.

**OUTSIDE-SCOPE ROW, reported and NOT introduced by this fix:** `msd+0x4a4` (the query
radius / `susp`) reads **0.67804** on the original and **692.302** on the port in every
band — a factor of ~1021. It is **identical in the pre-fix and post-fix arms** (`susp=692.302`
on line 1 of both `pre1` and `s1`), so it is a standing cross-side divergence this attempt
inherited, not caused. It is the field the fix's own `radius` argument comes from, which
makes it the next thing to look at: the port's spatial admission test is being handed a
radius three orders of magnitude larger than the original's.

**Floors are 0 on the cross-side leg too**, so `gap/floor` prints `0-floor` on every row and
the ranking carries no information; the band **medians** are what is read above. Reported
rather than re-thresholded.

### AI slots 1+ — mechanically in the blast radius, magnitude NOT measured

`ProduceTerrainBatch` is called from `SolveWheelContacts` (`VehiclePhysicsRun.cpp:1007`) for
**whichever car is being stepped**, so the fix changes the contact batch for AI cars as well
as the player. This arm is `MASHED_MEASURE_SOLO=1` with `participants=1` confirmed, so **no
AI car existed in any run above** and the magnitude is **[UNCERTAIN]**. Nothing in the fix
touches `VehiclePhysicsRun.cpp:702`'s fitted seed, and **nothing was tuned**. This is D3's
to measure and is recorded for it.

## 5 What is still open after STEP 3/4

- **driving-median is 1.3 % below its lower bound** (1879.59 vs 1904.70). D2 does not fully
  close on the three-metric gate.
- **grip-clamp #6** is now the whole of `T_post` (-0.6125 of -0.6112), at 1/45th of the old
  magnitude.
- **The substep loop is still 3 vs the original's 2** (`nsub {3: 1626}`). It is fully
  transcribed (attempt 19 `RESULT.md` §2) and `PREREG_STEP34.md` §4 permits it as a
  separately pre-registered step now that (a) has passed.
- **`msd+0x4a4`** — the cross-side radius row above.
