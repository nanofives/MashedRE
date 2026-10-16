# PREREG d2lateral: the post-impact lateral hold (U-9156 / U-9180), term by term through A6a

Branch `work/d2lateral` from `race/first-frame-parity` `10c18c59`. Base exe built from that commit, unmodified source:
sha8 `4d4a457a` (`exe_base\mashed_re.exe` in the session scratchpad). Model claude-opus-5-5, account3.
Registered before the first measuring run of this lane.

## 0. Facts read before any run (existing captures and the bytes only)

0.1 **The symptom** (`verify/d2engage_20261016/CAUSE2.txt`, `RESULT_d2engage.md` section 3). From the first wall impact
(snapshot after the drop = +0), lateral retention per frame is close on both sides for +1..+6 (orig 0.819 0.857 0.862
0.867 0.874 0.880, port 0.819 0.824 0.860 0.865 0.871 0.878). At **+7..+10 the original holds lateral at 81.7..81.9
(retention 0.998 0.999 1.001 1.003)**, while every port branch (B0X / DX / BX) keeps losing it (0.885, then 0.900 0.757
0.495 0.265 on the LOW arm with k 0.194 -> 0.799). Forward speed grows ~+15.5 per frame on both sides through +7..+11.

0.2 **What A6a `0x00467650` does to velocity, from the bytes** (`re/tools/disasm_va.py 0x467650 0x132c` on
`MASHED.exe.unpatched`, 1236 instructions, this lane):
- writes `+0x9b0..+0x9b8` at `0x00467aee..0x00467b08` (surface-id scale for `+0x1f0` = -1 / `0xffc8c8c8`), at
  `0x00468688..0x0046869e` (W1, `v += linTerm*(ctrl + accum)`), and in clamp #6 (`0x00468840..`, `0x004688d4..`);
- writes `+0x9bc..+0x9c4` at `0x0046858c..0x004685ac` (before W1) and in clamp #6;
- writes **none** of `+0x9d4..+0x9dc` (forward axis) and **none** of `+0x9e0`. So the forward axis and the grounded
  count read at A6a entry are the ones clamp #6 uses (`0x00468761 cmp [esi+0x9e0], 0x40800000`).
- The W1 speed is `call 0x004c3ac0` (RwV3dLength) at `0x004686a4` with EDI = ESI+0x9b0 (`0x00468686 fadd [edi]`),
  return `0x004686a9`. Its argument IS the post-W1, pre-clamp velocity.
- Per wheel, `ld4` is RwV3dLength at `0x0046820a` (return `0x0046820f`), with EDI = the wheel block
  (`0x004681d0 fmul [edi+0x80]`). It runs only inside the gate `kSpeedMin < le4 && +0x9e0 == 4.0`
  (port `Integrate2.cpp:440`). So `+0x9e0 != 4.0` at A6a entry removes BOTH the per-wheel lateral force and clamp #6
  in that frame.

0.3 **Existing instruments** (no new hook site is added by this lane):
- `scenario_launch.py --lat-bracket`: entry hooks A6a `0x00467650` (ESI-filtered), A6b `0x00468980`, substep
  `0x004709a0`, rows `0,2,1,1` per frame.
- `scenario_launch.py --mag-probe <sites>`: entry hook RwV3dLength `0x004c3ac0`, tagged by return address, hard
  row limit with auto-detach. D2_REOPEN section 21.8/21.9 ran 9 A6a sites for a 38 s race (`err null`, KA 1424/1424).
- Port: `MASHED_A6ADUMP` (act.* at A6a entry, clamp #6 `l60 / grip / kvel / arm / clamp`, per-wheel `fired`),
  `MASHED_D2SINK` (`w1`, `a6a_out`, `a6b_out`, `a4_out`, `sub_top` sub 0/1, `sub_end`, `snap`).

0.4 **Edits this lane makes to the instrument (diagnostic only, no game code):** `scenario_launch.py` appends columns
(older reducers' indices do not move): to every lat-bracket row `+0x1f0 +0xd00 +0x2c +0x34 +0xb20 +0x9ec +0x9f0`, the
four wheel states `+0x198` and `+0x1a0` (stride `0xc4`); to every mag-probe row the lat-bracket sequence counter
`lbseq` (the frame marker shared by both probes: A6a's entry row is `lbseq-1`) and the caller's EDI/EBP/EBX.

## 1. Question

At which term does the original's first-impact lateral hold (+7..+10) part from the port: (a) clamp #6's gate
(`+0x9e0` at A6a entry) and with it the per-wheel lateral force, (b) clamp #6's `k` (via `l_60` / G), (c) the
per-wheel force integration before W1, or (d) a stage after A6a (A6b, the two contact substeps, the next frame's head)?

## 2. Runs

| run | what | role |
|---|---|---|
| `orig_lx1` (`_lx2`, `_lx3` only if needed, see 3 EV) | original, argv of `orig_solo3` (`--statediff-drive --statediff-drive-late --statediff-steer 1 --hold 38 --poke-ctrl-slots`) + `--lat-bracket` + `--mag-probe 00467673,00467685,004680fb,0046820f,00468343,004684c0,004684dc,004685bc,004686a9 --mag-probe-limit 40000`, `MASHED_MUTE=1`, own PID only | the measurement |
| `D2_DLX_1` | port D2 recipe (d2engage's `D2BASE`) + A6ADUMP + D2SINK + world-contact log, default build | port side, default |
| `D2_BLX_1` | the same with fix B (`MASHED_POS_MODEL`) | port side, B |
| `BASE_H_DEF`, `H_DEF`, `H_DEF-2` | G-CTRL on the base exe and on this branch's build (no knob) | baseline / no-op |

The original runs alone (no port run of this lane in parallel). A foreign MASHED loading a piz at the same moment
can crash it (U-9677): a crash with either documented signature is re-run, not reported.

## 3. Gates on the measurement (all must pass before section 4 is read)

- **CV (coverage):** lat-bracket `err null`, the `0,2,1,1` pattern holds on every step from j0-2 to j0+16
  (`d2lat.py` prints `!pattern`); mag-probe `err null`, exactly one `004686a9` row per step through j0+16 (the limit
  of 40000 rows must not detach before j0+16).
- **KA-1 (known answer):** on every step, the `004686a9` argument equals the record's own `+0x9b0` / `+0x9b8` read at
  that call (bit-exact). Failure voids the run.
- **KA-2 (frame marker):** every `004686a9` row's `lbseq` is its step's A6a-entry `seq + 1`. Failure voids the run.
- **KA-3 (wheel tag):** `ld4` rows resolve to a wheel 0..3 from EDI on >= 99 % of rows; the rest are counted.
- **EV (the event reproduces):** the probed run's impact step j0 (first step whose next A6a-entry horizontal speed is
  below half of a > 500 speed) is followed by lateral retention >= 0.98 on at least three of steps j0+7..j0+10 (the
  orig_solo3 signature). If not, the run is a different trajectory: re-run up to three times in total, and report
  every run. If none shows it, section 4 is read on the run whose retention series is closest to orig_solo3's and the
  difference is stated.
- **Port KA:** `d2lat.py port` pairs A6ADUMP rows with D2SINK frames by `act.speed == |w1|` (both are A6a's own post-W1
  |v|); `ka_miss` must be 0 over the window.

## 4. Decision rules (fixed now)

Per step j from j0+1 to j0+12 and per stage in the order gate (`gnd`, `nld`) -> `dInt` (E->W1) -> `dClamp` (W1->B,
with `k_meas`) -> `dPost` (A6b + A4 tail) -> `dS1` -> `dS2` (+ next head):

4.1 **First diverging term** = the first (step, stage) where the two sides differ categorically (gate value 4.0 vs
not, clamp ran vs not, arm, number of `ld4` wheels) or numerically by more than `max(0.05*|lat_E(orig)|, 1.0)` in
that stage's lateral change. Steps j0+1..j0+6 also print their differences so the drift before the hold is visible.

4.2 **Branches.**
- **H-GATE**: at the first diverging step the original's `gnd != 4.0` (then `nld = 0` and `k_meas = 0` exactly) while
  the port's is 4.0 -> the term is A5's grounded count (`0x0046ddb0` zeroes `+0x9e0` at `0x0046ddd1` and rebuilds it
  per wheel). Next: the per-wheel states at A6a entry name the wheel(s); their writer is read from the bytes and the
  port's transcription compared there.
- **H-K**: gate open on both, `k_meas(orig)` and the port's `k` differ by more than 0.05 -> the term is `l_60` / G;
  per-wheel `ld4` / `le4` are compared next.
- **H-INT**: gate and `k` agree, `dInt` differs -> the per-wheel force (block #4/#5) before W1.
- **H-POST**: everything inside A6a agrees within 4.1's tolerance, `dPost`, `dS1` or `dS2` differs -> a stage after
  A6a (A6b, contact substeps). The substep stages contain the terrain producer the sibling lane d2bsp is porting
  (`FUN_00538c80`): if the term is there, no fix is authored here and it is reported.

4.3 **Fix rule.** A fix is authored only when the diverging term's law is read from the bytes AND the port differs from
the bytes at that law (a transcription defect or a missing input). It goes behind one knob (`MASHED_D2LAT_*`,
default OFF), with an amendment to this file naming it and its predicted effect before its first run.

## 5. Gates for a fix (scored as d2engage section 3, same tools)

- **no-op:** `H_DEF` = `H_DEF-2` = `BASE_H_DEF` (step / gates / ptrace sha8; predicted `716cb2fd / 2a2afda1 /
  ba90ed88`).
- **knob-took:** the diag arm with the knob shows the changed term at the step 4.1 names.
- **determinism:** `H_X` = `H_X-2`.
- **D2 three metrics** (`verify/d2settle_20261016/score_d2.py`, bounds `d81a8df6` unchanged: slip 1500-2000
  0.18855..0.19635, slip 2000-2600 0.24488..0.25487, driving-median 1904.70..1982.44) on `D2_X_1..3`, 3/3.
- **recovery H1** (`a8_launch.py`): >= 50 % of the 400 post-trough frames >= 100 and median >= 900, 3/3.
- **D3 (b)** per regime (`d3b_score.py`, `re/tools/b_envelopes.json`) on `B_X_K{0,1,26}`: not below the base's count
  in any regime (base reference A 1/3, B 2/3, C 0/3, re-measured on `BASE_B_DEF_K*`).
- **collateral** (`collateral_run.py`): `H_X` (floor `H_X-2`) at most 2 fields above `H_DEF` (floor `H_DEF-2`).

**Default rule:** the knob goes default ON (opt-out `=0`) only if every gate of this section passes. Otherwise it
stays OFF and the RESULT names the failed gate. If driving-median enters its bound in any arm, that is reported first.

## 6. Predictions (registered now)

6.1 CV, KA-1, KA-2 pass (the same hooks ran for 38 s at D2_REOPEN 21.9).
6.2 The port arms reproduce CAUSE2: clamp on the LOW arm with k rising 0.19 -> 0.8 across j0+8..j0+11, `gnd` 4.0.
6.3 No direction is predicted for the original. The bytes allow both H-GATE (the `ld4` and clamp gates share
`+0x9e0`, and a retention of exactly ~1.0 is what removing both would give) and H-POST (a wall contact restoring
lateral in the substeps). [UNCERTAIN] which; that is the measurement.

## 7. D2 watch list (DEFERRED D-11071)

This lane reads grip-clamp #6 and A6a's per-wheel force; it changes no game code before an amendment names a fix.
Every touched watch item is reported in the RESULT.

## Amendment 1 (2026-10-16, after `orig_lx1`, `D2_DLX_1`, `D2_BLX_1`; before `orig_fx1` and `D2_DFX_1`)

A1.1 **Section 3 gates on `orig_lx1`:** CV pass (pattern 0 bad of 2292 steps, `err null` on both probes, mag-probe
covered 2265 steps), KA-1 2265/2265, KA-2 by construction of the pairing (one `004686a9` row per step on every step
it covers), KA-3 0 unresolved `ld4` rows, EV pass (retention 0.998 / 0.999 / 1.001 / 1.003 at j0+7..j0+10, the
orig_solo3 series). Port KA: `ka_miss` 0 on both arms.

A1.2 **Section 4 result.** H-GATE refuted: the original's `+0x9e0` is 4.0, states `1111`, four `ld4` wheels on every
step j0-2..j0+14. Rule 4.1 names **step j0+7, clamp #6 arm** (categorical): original G 31227.9 < 32768 (LOW, k on its
0.1 floor), port G 33555.7 (HIGH, k 0.1993). |W1| is equal (97.78 vs 97.59), so the factor is `l_60` (319.36 vs
343.85). H-K fires. The reconstructed original `k` equals the inverted `k_meas` to 4 decimals on every step (resid 0).

A1.3 **Per-wheel split (`d2lat_wheels.py`).** j0-1 and j0: `ld4` and `le4` equal to 4-5 digits on all four wheels.
j0+1: `le4` is 6-11 % higher on every port wheel, `ld4` equal except w2 (0.750 vs 0.777). So the `l_60` excess from
j0+1 on is carried by the post-impact speed. Upstream of it: S2 of step j0 (the last pre-contact sample) is equal
(|v| 1832.29 vs 1827.91), and the next A6a entry is (175.14, -71.75, 130.20) vs (187.56, -77.07, 141.63), a near
uniform x1.077 at the same direction. The stage that creates the difference is **step j0's contact substep**, which
4.1's window (from j0+1) did not include. The trace is extended upstream into step j0 here, before measuring it.

A1.4 **What the bytes say about that stage.** `FUN_004709a0` runs `0x0046e9e0` -> `0x0046f6c0` -> `0x00469aa0` ->
(if `+0x9ec` != 0) `0x0046ef70` and re-runs the chain while `0x00469aa0` returns nonzero (`0x00470ab0..0x00470b0b`).
`0x0046ef70` ends with `vel = (vel + local_54) * min(3 * local_74, 0.9)`, `local_74 = 1 - min(1, |m| / +0x9e4)` from
the slot loop (port `ContactFixup.cpp:133-137, 292-315`). On the port's impact (`DLX.wc.log` line 1: slots s5/s9,
n (1,0,0), m -1671.8, depth -0.0269) |m| / +0x9e4 is about 0.915, so the damp is about 0.26 and `d(damp) / d(|m|/sp)
= -3`: a 0.7 % difference in |m| / +0x9e4 is a 7.7 % difference in output speed.

A1.5 **Measurement.** `orig_fx1`: the original, `orig_solo3`'s argv + `--fixup-probe` (existing entry hooks
`0x0046ef70` / `0x004709a0` / `0x00467650` / `0x0046f6c0` / `0x00469aa0`, one sequence, live 18-slot set), with this
amendment's appended columns (`+0xcfc +0x5c +0x9c8..+0x9d0`, slot 0's raw key, four wheel states, angular velocity,
position, fwd.y). Port `D2_DFX_1`: D2 recipe + `MASHED_FIXUP_LOG` + `MASHED_D2SINK` (`fx_bounce`, `fx_damp`, `fx_slide`,
`sub_orient`, `sub_wheel`) + the world-contact log + A6ADUMP. Gates: `err null`; the impact fixup (first fixup with
|vel| > 500) is present on both sides; known answer: on the original's non-contact substeps the velocity at the
`0x0046f6c0` entry equals the substep entry's (A9 writes no velocity), so a retry's `0x0046f6c0` row after a fixup is
that fixup's output.

A1.6 **Decision (fixed now), on the impact fixup.**
- Slot sets differ (live count, slot indices, normals to 1e-3, depth sign) -> the contact producer
  (`0x00469aa0` / the BSP walk). Report, no fix here (sibling d2bsp's lane).
- Slot sets agree, the slot magnitudes `m` or `+0x9e4` differ by more than 0.1 % -> trace that input (`m` is written
  by the producer from the contact-point velocity; it is compared against `v + w x arm` on both sides).
- Slot sets, `m` and `+0x9e4` agree to 0.1 %, the output differs -> `0x0046ef70`'s transcription is the term; a fix
  goes behind `MASHED_D2LAT_FIXUP` (default OFF) after a further amendment.

## Amendment 2 (2026-10-16, after `orig_fx1` and `D2_DFX_1`; before any build or run with a code change)

A2.1 **A1.6 result.** Gates: fixup-probe `err null`, 55 fixups, self-check rows ESI = EDI = record. Known answer:
on every non-contact substep around the impact the `0x0046f6c0` row's velocity equals the substep entry's (A9 writes
no velocity), so the retry row after the impact fixup is its output. The impact fixup has the same slot set on both
sides (2 live, slots 5 and 9, normals within 1e-3 of (1,0,0), depth sign equal), so the producer's slot set is not the
term. **The port's transcription of `0x0046ef70` replayed on the original's own inputs (`fixup_replay.py`) gives
|v| 231.477 against the measured 231.471 (max component error 5.4e-5 relative): the law is not the term.** The inputs
differ: `+0x9e4` 1835.50244 (port, `DFX.fixup.log` `sp=`) vs 1832.40173 (original), m -1671.8 vs -1681.2. Damp
0.26763 vs 0.24754 (x1.081), which is the whole post-impact gap (247.34 / 229.73 = x1.077).

A2.2 **The first diverging term, from the bytes.** `+0x9e4` at the fixup. Its writer on the original is the per-frame
refresh in `FUN_00470c70` after the A4 loop: `0x004710b0..0x004710ba` gate (`[rec+0x4] != 0 || [rec+0x10] == 1`),
`0x004710c3 call 0x004c3ac0(rec+0x9b0)` / `0x004710c8 fstp [rec+0x9e4]`, `0x004710d5 call 0x004c3ac0(rec+0x9bc)` /
`0x004710da fstp [rec+0x9e8]`. `0x004c3ac0` forms `(x*x + y*y) + z*z` and takes the RW LUT sqrt
(`0x004c3af2 mov eax,[0x7d3ff8]`, `0x004c3af7 mov ecx,[0x7d3ffc]`, index `((b + 0x800) >> 12) & 0xfff`).
`Math::LutSqrt` on the PC_24 float sum reproduces the original's `+0x9e4` **bit-exactly** on both rows checked
(1832.4017 from (-1704.885, 0, 671.309) and 190.662 from (147.532, -57.480, 106.182); the exact sqrt gives 1832.2907
and 190.6417). The port's default has no per-frame refresh (d2settle's fix C, `MASHED_SPEED_REFRESH`, default OFF,
uses the exact sqrt), so its fixup reads A6a's pre-clamp |W1|.

The residual after the refresh is `m` (-1671.8 vs -1681.2). Its inputs are the contact-point velocity (velocity +
the angular term) and the slot normal. The pre-impact velocity differs by 0.37 deg of heading and 0.23 % of speed
(port (-1696.48, 0, 680.60) vs original (-1704.89, 0, 671.31)), and the arm by 0.012 in z. That is the steady-state
trajectory drift before the impact, outside this lane's term. Predicted share of the post-impact gap left after the
refresh: about 3 % (the damp's `-3 * d(|m|/sp)` on the remaining 0.3 % of |m|/sp).

A2.3 **The fix (one knob, default OFF): `MASHED_D2LAT_REFRESH=1`.** It sits at fix C's site
(`VehiclePhysicsRun.cpp`, after `VehicleControlIntegrate`, same gate, the A4-skipped frame's refresh included), and
writes `+0x9e4 = |+0x9b0|` and `+0x9e8 = |+0x9bc|` with `0x004c3ac0`'s arithmetic (`Math::LutLength`: float
`(x*x + y*y) + z*z`, then `LutSqrt`, in the SSE2 TU `RwLutTables.cpp`). With the knob OFF the code path is the
current default. `MASHED_SPEED_REFRESH=1` (exact sqrt) stays as it is and is run as a report arm.

A2.4 **Arms.** No-op: `BASE_H_DEF`, `H_DEF`, `H_DEF-2`. Knob: `H_L`, `H_L-2` (determinism, collateral), `D2_L_1..3`,
`B_L_K{0,1,26}` against `BASE_B_DEF_K{0,1,26}`, and `D2_LFX_1` (diag + knob: knob-took and the impact chain).
Report: `D2_DEF_1` (same-build control), `D2_C_1` (fix C exact on today's base), `D2_BL_1` (fix B + knob).

A2.5 **Predictions.**
- No-op: `H_DEF` = `H_DEF-2` = `BASE_H_DEF` (`716cb2fd / 2a2afda1 / ba90ed88`).
- Knob-took (`D2_LFX_1`): the impact fixup's `sp=` equals `LutLength` of that substep's entry velocity bit-exactly,
  and differs from |W1|.
- Impact: post-impact |v| at the next A6a entry within 4 % of the original's 229.73 (from 7.7 %).
- The +7 arm and the D2 metrics: no direction predicted. G at +7 lands about 2 % under the knee if the residual is
  as predicted, so the arm can go either way. [UNCERTAIN] whether the hold reproduces.
- G-CTRL with the knob: changes (`+0x9e4` feeds A6a's next-frame reads and every fixup), no hash predicted.

A2.6 **Default rule.** Section 5 unchanged: default ON only if every section-5 gate passes on `L`. Otherwise OFF, with
the failed gate named.

## Amendment 3 (2026-10-16, after `D2_LFX_1`, `D2_L_1..3`, `D2_DEF_1`, `D2_C_1`, `D2_BL_1`; before any spawn-yaw run)

A3.1 **A2.5 results.** Knob-took: the impact fixup's `sp=` 1832.12231 equals `LutLength` of that substep's entry
velocity (-1700.98755, 3.36854e-05, 680.836365) = 1832.122314 bit-exactly, and differs from |W1|. Impact: post-impact
speed at the next A6a entry 224.5 against the original's 218.2 (+2.9 %, from +7.7 %), inside A2.5's 4 %. The +7 arm
still flips: G 32788.6, 0.06 % over the 32768 knee (original 31227.9), then the LOW-arm collapse at +9..+11 as before.
D2 (`score_arms.py`): `L` x3 slip 0.2038 OUT / 0.2639 OUT / driving-median 1736.20..1742.09 OUT (n 464..467),
recovery 9/400 x3 = the wall-pin signature. `C` (exact sqrt) the same signature (0.2008 / 0.2627 / 1798.12, 9/400).
`BL` 0.1967 OUT / 0.2523 in / 1862.13 OUT, 387/400. `DEF` 0.1927 in / 0.2489 in / 1863.55 OUT, 398/400. **The knob
fails section 5 (D2 three metrics, H1) and stays default OFF** whatever the remaining gates give.

A3.2 **Where the remaining impact difference comes from (`orig_lx1` vs `DLX` / `LFX`, aligned at the first step with
speed > 0.5).** The velocity heading at the fixup differs by 0.32 deg at equal speed (21.82 vs 21.49 deg from -x), so
n . v_c and with it |m| / +0x9e4 differ by 0.27 %, which is the damp's remaining x1.03. The earliest step where the
two sides differ is the spawn itself: **body yaw differs by 0.2997 deg from launch step 0** (original fwd
(-8.74e-8, 0, -1), port -89.6992 deg in the same convention), identically on `DLX` and `LFX`. The port's slot-0 yaw is
`atan2(g1 - g0)` (the gate0 -> gate1 direction) at both spawn sites (`TrackRenderer.cpp` R6 :3533 and StartRound's
`place`), a stand-in for the original's `L = FUN_00426cc0()` (StartRound comment, `verify/grid_orig_ts/`). This is the
grid-pose question d2posmodel 5.2 left to the user. Later the port also lags (speed -11 % at launch +24) and turns
about 2 deg less by +72..+84. Those are not attributed here.

A3.3 **Diagnostic only (never a fix), default OFF: `MASHED_D2LAT_SPAWN_YAW=<rad>`.** At both slot-0 spawn sites,
after the stand-in computes `car_yaw_`, it is replaced by the given value and the body basis re-seeded
(`VehiclePhysics_ResetOrientation(0, car_yaw_)`). Value used: -1.5707964 (port convention `atan2(fz, fx)` of the
original's spawn fwd (-8.742278e-08, 0, -1)). It answers one question: with the original's spawn heading, does the
impact heading close, and do the +7 arm and the pin follow?

A3.4 **Arms (report only).** `D2_YLFX_1` (knob L + yaw + the diag logs), `D2_Y_1` (yaw alone), `D2_YL_1..3`
(yaw + knob L).

A3.5 **Predictions.** Knob-took: body heading at launch step 0 equals the original's (-89.9989 deg) within 0.001 deg.
No direction is predicted for the impact heading, the +7 arm or the pin. [UNCERTAIN] the launch lag and the turn
deficit are independent of the spawn yaw, and may keep the impact heading apart.

A3.6 **Decision.** None on defaults: the spawn yaw's faithful source (`FUN_00426cc0`) is the grid lane's port and
d2posmodel 5.2's user decision. The result is reported with `n` and the per-step table.
