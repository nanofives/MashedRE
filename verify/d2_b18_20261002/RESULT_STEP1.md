# RESULT — D2 attempt 16, STEP 1: `+0xb18` is a STRUCTURAL zero on the original, a float-epsilon on the port

Measured against [`PREREG_STEP1.md`](PREREG_STEP1.md), committed at `7e9cbf7c` before the
first run. **No gate was amended.** The verdict follows the pre-committed decision rule.

Branch `race/first-frame-parity`. HEAD at the runs `7e9cbf7c`. **No `mashedmod/src` change,
no build** — only the read-only `--axis-probe` and these docs. The port `mashed_re.exe` is
byte-identical to attempt 15, so STEP 3's numbers reproduce attempt 15 to every digit.

---

## 1 The live test — gates, all PASS

ORIGINAL arm: `scenario_launch.py --statediff-out orig_ax.msd --statediff-drive
--statediff-drive-late --statediff-steer 1 --hold 38 --poke-ctrl-slots --axis-probe`
(PID 31420, muted, `MASHED_WIN_POS=primary-bl`). 2179 A6a frames, 1275 active-drive.

| gate | required | measured | |
|---|---|---|---|
| **SC** self-check | `fwdConst == [0,0,1]`, `rec == 0x8815a0` | `fwdConst=[0,0,1]`, `rec=0x8815a0` | **PASS** |
| **COV** coverage | `a6a >= 400`, `drive >= 50`, `err == null` | `a6a=2179`, `drive=1275`, `err=null`, `skipped=0` | **PASS** |
| **PRE** reset | `b18nzPre == 0` | `b18nzPre=0` (A4's 0x0047072c reset confirmed) | **PASS** |

Agent counters, verbatim:
`{"a6a":2179,"a6b":2179,"skipped":0,"drive":1275,"fwdYnz":0,"axYnz":0,"b18nzPre":0,
"b18nzPost":0,"chk":{"fwdConst":[0,0,1],"rec":"0x8815a0"},"err":null}`

## 2 VERDICT: H-struct CONFIRMED

On the ORIGINAL, over all 2179 A6a frames (1275 active-drive):

- body forward-Y `+0x9d8` has **exactly one distinct value: `0`** (bit-exact `0.0`),
- all four wheel forward-axis Ys `+0x224/2e8/3ac/470` are `0.0` (`axYnz=0`),
- A6a leaves `+0xb18 == 0.0` at A6b entry (`b18nzPost=0`).

`+0xb18 = Σ_wheels axisY * force` (A6a `0x00467cc5/ccb` active-drive, `0x00467d97/d9d`
boost). With `axisY == 0`, every term is `0`, so `+0xb18 == 0` **regardless of the force
magnitude**. The original's zero is **STRUCTURAL** — the producer feeds `0.0`; no later step
is involved. H-later is refuted (A6a's own output is already `0`).

### The two writers, by RVA (confirmed)

| quantity | writer RVA | mechanism |
|---|---|---|
| `+0xb18` accumulator | A6a `FUN_00467650` `0x00467cc5`/`0x00467ccb`, `0x00467d97`/`0x00467d9d` | `+0xb18 += axisY * (drive\|ff)` |
| wheel axis-Y `+0x224..` | A5 `FUN_0046ddb0` `0x0046de74` (steer==0) | `[esi+0xb8] = [edi+0x9d8]` (body forward-Y) |
| body forward `+0x9d4/d8/dc` | A5 `FUN_0046ddb0` `0x0046ddc9` | `xform*(0,0,1)` = the car matrix at-row |
| per-frame reset | A4 `FUN_00470670` `0x0047072c` | `+0xb14/18/1c = 0` before A5/A6a |

## 3 The port, measured — the divergence is a float-epsilon in the angular chain

From `port_a6adump.log` (slot 0, 1633 frames; the port's own A6a block dump), §16.7 arm
(`MASHED_MEASURE_SOLO=1 MASHED_TRACK_SEL=12 MASHED_STEER_HOLD_AFTER=0`, PID 38148, rc 0):

| quantity | port | original |
|---|---:|---:|
| body forward-Y `act.bf[1]` (+0x9d8) | nonzero 1632/1633, median **3.05e-08**, max **3.83e-08** | **0.0** (all 2179) |
| wheel axis-Y `a.ax[1]` | nonzero 1631/1633, median **3.06e-08**, max 3.83e-08 | **0.0** |
| `+0xb18` snapshot (motion_diag `b14[1]`) | nonzero 1618/1633, median **0.0282**, bulk 1e-3..1.0, one frame-0 transient 1.3e7 | **0.0** |
| angular vel X `act.av[0]` (+0x9bc) | nonzero 1630/1633, median **5.44e-10**, max 0.0134 | (implied **0.0**) |
| angular vel Z `act.av[2]` (+0x9c4) | nonzero 1630/1633, median **6.86e-10**, max 0.0076 | (implied **0.0**) |

**The chain, root-caused:** the car body matrix at-row is maintained by
`BodyOrientationIntegrate FUN_0046e9e0` as `at.y_new = (omega.z*at.x − omega.x*at.z) + at.y`
(disasm cited in `BodyOrientationIntegrate.cpp:25-27`). With `at.y` starting at `0`, it stays
`0` **iff** `omega.x == omega.z == 0` every step. The original's angular velocity has exactly
`0` X/Z in this flat grounded solo arm (no pitch/roll torque), so `at.y` stays bit-exact `0`,
forward-Y is `0`, axis-Y is `0`, and `+0xb18` is `0`. The port's `omega.x/z` carry a
**~5e-10 median (max 0.013) float-epsilon** from the contact/suspension-force torque sum,
which drifts `at.y` to ~`3e-08`. The explicit omega-X/Z zeroing the port already has
(`AeroStabilize.cpp:144`, `Integrate2.cpp:752`) is the `state != 0` (airborne/aligned) branch
and does not run during grounded driving.

**Why `+0xb18`'s snapshot is 0.028, not 3e-08:** it is `axisY * force` summed, and the drive
multipliers are large (the boost arm's `ff = 5e6`, the per-gear `local_cc`), so a `3e-08`
axis-Y yields a non-negligible product. This confirms the logic — a zero axis-Y zeroes
`+0xb18` at any force — rather than contradicting it.

## 4 STEP 2 is NOT entered — per the PREREG decision rule

`PREREG_STEP1.md` §5: STEP 2 is entered only if the cause is **a single faithfully-portable
producer** diverging from a structural zero. It is not:

- The divergence is **diffuse float-epsilon** accumulated across the entire contact-force
  torque chain into `omega.x/z` (median 5e-10). There is no single producer whose faithful
  port makes `omega.x/z` bit-exactly `0`; the per-wheel force sum is already transcribed, and
  the epsilon is an FP-order artifact that survives any faithful reimplementation.
- The only way to force `at.y == 0` is an explicit clamp (a **forbidden single-constant /
  knob fix**, kickoff STEP 2) or a full bit-identical re-port of the angular/torque chain
  (infeasible, and the epsilon would likely persist).
- **The magnitude is physically negligible.** Forward-Y `3e-08` -> `+0xb18` consumed into
  velocity-Y at `Integrate2.cpp:640` as `linTerm * +0xb18` with `linTerm = dt * +0x54(0.001)
  * kDt` ~ `1.7e-5`, i.e. ~`5e-7`/frame of velocity-Y. That **cannot** be the recovery defect
  (port median 132.8 vs original 1333.9, a ~10x speed deficit in the **X/Z** plane).

So **U-9175's `+0xb18` is a confirmed binary divergence of zero physical consequence** — real
in kind (`0.0` vs epsilon), negligible in effect. It is a downstream symptom of FP noise in
the angular chain, **not** the recovery root. No fix is authored; no value is changed; no knob
is added.

## 5 STEP 3 — the four pre-registered verdicts (build byte-identical to attempt 15)

Reduced from this session's port run (`port_ax/motion_diag.log`) vs `orig_solo3.msd`,
`a8_launch.py` + `a8_medframe.py`. Numbers reproduce attempt 15's STEP 3 to every digit,
confirming determinism.

| gate | rule | result |
|---|---|---|
| **a** `+0xb18 == 0` every post-release port frame | pass iff all 0 | **FAIL** — nonzero 1618/1633, median 0.0282 (no fix applied, §4) |
| **b** launch still PASSES | L=0, b14 at d=15, peak at d=95 | **PASS** — L=0 (0.19%), b14 d=15, peak 1835.50 at d=95 vs 1832.40 |
| **c** recovery H1 | ≥50% of 400 ≥100 AND median ≥900 | **INCONCLUSIVE** — 243/400 = 60.8% (fraction passes), median 132.8 (median fails) |
| **d** three metrics in `d81a8df6` bounds, 3 runs | — | **FAIL 3/3** (below) |

`participants=1` confirmed. Every row carries n, median speed, median frame.

| metric | bound | PORT | n | median speed | median frame (`d`) | verdict |
|---|---|---:|---:|---:|---:|---|
| slip 1500-2000 | 0.18855 .. 0.19635 | 0.1983 | 19 | 1665.05 | 86 (d=85) | **FAIL** +0.00195 |
| slip 2000-2600 | 0.24488 .. 0.25487 | UNSCORABLE | 0 | — | — | **FAIL** |
| driving-median | 1904.70 .. 1982.44 | 1019.77 | 76 | 1019.77 | 80 (d=79) | **FAIL** −47.5% |

ORIG reference (same reducers): slip 1500-2000 n=314 median frame 1606 (d=720); slip
2000-2600 n=540 median frame 1795 (d=909); driving-median 1937.89 n=1154 median frame 1710
(d=824). §26.10's median-frame guard fires on every row.

## 6 Collateral

**No behavioral change is possible** — `git diff 29bd7619..HEAD -- mashedmod/src` and
`git status mashedmod/src` are both empty, and the port run matches attempt 15's `r1` launch
numbers to every digit. The cross-side banded collateral is therefore unchanged from attempt
15 `RESULT_STEP4.md` §2 (one readable band 70-100; `+0xb0c` top-ranked by gap/floor; `+0xb18`
the binary row, now resolved here). **No outside-scope rows are introduced** — the only diff
is the read-only `--axis-probe` and these docs.

## 7 Verdict

**D2 does NOT close.** The launch is still faithful (gate b PASS). U-9175's `+0xb18` lead is
**resolved** — structural zero on the original, physically-negligible float-epsilon on the
port, no faithful single-producer fix, not the recovery defect. The recovery gap (gate c
INCONCLUSIVE, metrics FAIL 3/3) lies in the **X/Z-plane** dynamics, not the Y channel. The
next first-diverging term frame-aligned at the same `d` remains `+0xb0c` (`RESULT_STEP4.md`
§3a: errs in both directions, so **no single-constant fix**), not re-opened here.
