# RESULT d2lateral: the post-impact lateral hold, term by term (2026-10-16)

Branch `work/d2lateral` from `race/first-frame-parity` `10c18c59`. PREREG `PREREG_d2lateral.md` + amendments 1-3, each
committed before the runs it governs (`21e2a258`, `bc495ea2`, `4dfe0e72`, `009f5a75`). Exes: base `4d4a457a` (from
`10c18c59`), knob build `6fb65de1`, final build `81df3bb3` (knob + the diagnostic spawn yaw). Model claude-opus-5-5,
account3. Original runs: `orig_lx1` (lat-bracket + 9-site mag-probe), `orig_fx1` (fixup-probe), own PIDs only, the
shared `original\` read-only. Port runs: private `MASHED_ROOT` copies `root` / `root2` containing `original\`,
`MASHED_NO_DEPLOY=1`, every PID killed was one `run_arms.py` spawned (`RUNLOG.txt`).

## 0. Verdict

1. **H-GATE is refuted.** The original's clamp #6 runs at +7..+10. `+0x9e0` = 4.0, the states are `1111`, and four
   `ld4` wheels are present. The hold is a balance: the LOW arm sits on its 0.1 floor (G 31228..32438, just under the
   32768 knee) and removes 9.1 of lateral per step, while the W1 force integration adds 9.0.
2. **The port parts at +7 on the clamp arm** (PREREG 4.1). Its G is 33556, over the knee: HIGH arm, k 0.1993. The arm
   flip starts the LOW-arm collapse at +9..+11. The factor is `l_60` (+7.7 %), and that excess is carried by the
   post-impact wheel-point speed (`le4` +6..+11 % on every wheel at +1).
3. **The first diverging term, from the bytes, is `+0x9e4` at the impact fixup `0x0046ef70`.**
   - On the original it is the per-frame refresh `0x004710c3 / 0x004710c8` (RwV3dLength `0x004c3ac0`, RW LUT sqrt) of
     the post-A4 velocity. This is U-9212.
   - The port's default carries A6a's pre-clamp |W1|: 1835.50 against the original's refreshed 1832.40.
   - The resulting damp `min(3 * (1 - |m| / +0x9e4), 0.9)` is x1.081, and so is the post-impact car (x1.077).
   - The fixup law itself is faithful: the port's transcription replayed on the original's own inputs gives 231.477
     against the measured 231.471 (5.4e-5).
4. **The fix is ported behind `MASHED_D2LAT_REFRESH` (default OFF)**, with `0x004c3ac0`'s arithmetic, bit-exact
   (1832.4017 and 190.662 on the original's rows, 1832.122314 on the port). It closes the impact gap from +7.7 % to
   +2.9 % but **fails section 5**:
   - D2 0/3 (wall pin, recovery 9/400 on all three runs);
   - H1 0/3;
   - D3 (b) regime A 0/3 against the base's 1/3.

   No-op, knob-took, determinism and collateral pass. **The default build is unchanged:** G-CTRL `716cb2fd /
   2a2afda1 / ba90ed88` on `BASE_H_DEF`, `H_DEF-2`, `H_DEF-3` and the final exe's `H_DEF-4`.
5. **The remaining +2.9 % is the impact heading** (0.32 deg). Its earliest term is the spawn: the port's slot-0 yaw is
   the gate0 -> gate1 stand-in, 0.2997 deg off the original's grid direction `FUN_00426cc0` (fwd (-8.74e-8, 0, -1)).
6. **Diagnostic, not a fix: the original's spawn heading plus the knob (`YL`).**
   - The car tracks the original through the first two wall contacts. The pre-impact speed is within 0.002 %, and
     post-impact retention is 0.825 0.857 0.862 0.867 0.873 0.880 **0.998 0.999 1.000 1.003** 0.894 (original 0.825
     0.857 0.862 0.867 0.874 0.880 0.998 0.999 1.001 1.003 0.894).
   - Both D2 slips are **in** (0.1938 / 0.2544..0.2546) and H1 is 398/400 on 3/3.
   - **Driving-median 1985.10 / 1987.02 / 1985.10 is OUT on the high side** (bound 1904.70..1982.44, original 1937.89).
   - It does not enter its bound in any arm.

## 1. Gates of the measurement (PREREG section 3, amendment 1)

| gate | measured | verdict |
|---|---|---|
| CV, `orig_lx1` | lat-bracket `err null`, `0,2,1,1` on all 2292 steps, mag-probe `err null`, steps 0..2264 covered (limit 40000 reached and detached at j0+1276) | PASS |
| KA-1 | `004686a9` argument == record `+0x9b0 / +0x9b8` on 2265 / 2265 | PASS |
| KA-2 | one `004686a9` row per covered step, `lbseq` = A6a row + 1 | PASS |
| KA-3 | 0 unresolved `ld4` / `le4` wheel rows (EDI = wheel block) | PASS |
| EV | retention at j0+7..+10 0.998 / 0.999 / 1.001 / 1.003 (`orig_solo3` signature) | PASS |
| Port KA | A6ADUMP `act.speed` == D2SINK `w1` on every step, `ka_miss` 0 (DLX, BLX, DFX, LFX, YLFX) | PASS |
| fixup-probe, `orig_fx1` | `err null`, 55 fixups, self-check ESI = EDI = record; known answer: velocity at `0x0046f6c0` entry == substep entry on non-contact substeps | PASS |
| reconstruction | original `k` from `l_60` (sum `ld4 * min(le4,1024)`) through the bytes' arms == `k` inverted from W1 -> A6b entry, to 4 decimals, residual 0, every step j0-2..j0+14 | PASS |

## 2. The term chain (`TABLE_*.txt`, `WHEELS_DLX.txt`, `FIXUP_REPLAY.txt`)

| step | original | port default (`DLX`) | note |
|---|---|---|---|
| j0, pre-contact S2 | (-1704.89, 0, 671.31), 1832.29 | (-1696.48, 0, 680.60), 1827.91 | heading 21.49 vs 21.86 deg |
| j0 fixup inputs | slots 5, 9; n (1.00006/0.99994, 0, 0); m -1681.41 / -1681.20; `+0x9e4` **1832.40** | slots 5, 9; n (1, 0, 0); m -1671.8; `+0x9e4` **1835.50** | the law replays the original to 5.4e-5 |
| j0 damp | l74 0.08251, damp 0.24754 | l74 0.08921, damp 0.26763 | x1.081 |
| j0+1 A6a entry | 218.2, lat 201.41 | 235.0, lat 218.17 | x1.077 |
| j0+1 `le4` w0..w3 | 297.0 277.0 205.4 157.4 | 314.4 294.8 220.8 175.2 | `ld4` equal except w2 |
| j0+7 clamp #6 | `l_60` 319.36, G 31227.9, LOW, k 0.1000 | `l_60` 343.85, G 33555.7, **HIGH**, k 0.1993 | first categorical divergence (4.1) |
| j0+8..+11 | k 0.1 floor, retention ~1.0 | LOW, k 0.197 0.335 0.586 0.803, retention 0.897 0.751 0.483 0.261 | |

Fix B (`BLX`) follows the default within 1 % through the window (+1 234.1 vs 235.0, +7 G 33319.6 vs 33555.7, same
arms and the same collapse).

## 3. The knob `MASHED_D2LAT_REFRESH` (amendment 2)

`VehiclePhysicsRun.cpp`, at d2settle fix C's site (after `VehicleControlIntegrate`, gate `[rec+4] != 0 || [rec+0x10]
== 1`, the A4-skipped frame's refresh included): `+0x9e4 = LutLength(+0x9b0)`, `+0x9e8 = LutLength(+0x9bc)`.
`Math::LutLength` (`RwLutTables.cpp`, SSE2 TU) is the float `(x*x + y*y) + z*z` of `0x004c3ac4..0x004c3ada` and the LUT
of `0x004c3af2..0x004c3b17`. Knob OFF = the previous code path.

| gate (section 5) | arm(s) | measured | verdict |
|---|---|---|---|
| no-op | `BASE_H_DEF`, `H_DEF-2`, `H_DEF-3`, `H_DEF-4` (final exe) | `716cb2fd / 2a2afda1 / ba90ed88` x4 | PASS |
| knob-took | `D2_LFX_1` | fixup `sp=` 1832.12231 == `LutLength`(-1700.98755, 3.3685e-05, 680.836365) bit-exact | PASS |
| determinism | `H_L`, `H_L-3` | `9882827c / f5ca9cd5 / 1717f80a` x2 | PASS |
| D2 three metrics | `D2_L_1..3` | 0.2038 OUT / 0.2639 OUT / 1736.20, 1739.37, 1742.09 OUT (n 467 / 466 / 464) | **FAIL** 0/3 |
| recovery H1 | `D2_L_1..3` | 9/400 x3, median 21.6 (wall pin) | **FAIL** |
| D3 (b) | `B_L_K{0,1,26}` vs `BASE_B_DEF_K*` | A **0/3** (base 1/3), B 2/3 (2/3), C 0/3 (0/3) | **FAIL** (regime A) |
| collateral | `H_L` (floor `H_L-3`) vs `H_DEF-2` (floor `H_DEF-3`) | 39 vs 39 of 94 fields above floor | PASS (+0) |

Impact under the knob (`TABLE_LFX.txt`): post-impact 224.5 vs the original's 218.2 (+2.9 %, prediction <= 4 %). The +7
arm still flips, now by 0.06 % (G 32788.6 against 32768), and the slower post-impact car is then held on the x = -2.04
wall (the `d2-arm-wall-pin-signature`). Report arms: `D2_C_1` (fix C, exact sqrt) gives the same pin (0.2008 / 0.2627
/ 1798.12, 9/400). `D2_BL_1` (B + knob) 0.1967 OUT / 0.2523 in / 1862.13 OUT, 387/400. `D2_DEF_1` 0.1927 / 0.2489 /
1863.55, 398/400.

Two G-CTRL runs (`H_DEF`, `H_L-2`) on separate roots both exited at 14:31:00 with truncated dumps and the same game
outcome as their full twins. That is an external event: re-run as `H_DEF-3` / `H_L-3`, whose hashes are the ones read.

## 4. The impact-heading residual and the spawn (amendment 3, diagnostic only)

Aligned at the first moving step, the port's body yaw differs from the original's by **0.2997 deg from launch step 0**
(`DLX` and `LFX` alike). Slot 0 is placed by `atan2(g1 - g0)` at both spawn sites (`TrackRenderer.cpp` R6 and
StartRound's `place`). The original's grid direction is `L = FUN_00426cc0()`, measured as fwd (-8.742278e-08, 0, -1)
on Training. `MASHED_D2LAT_SPAWN_YAW=-1.5707964` (default OFF, measurement only) replaces it and re-seeds the basis.

| arm | slip 1500-2000 | slip 2000-2600 | driving-median (n) | H1 |
|---|---|---|---|---|
| `D2_Y_1` (yaw alone) | 0.2007 OUT | 0.2627 OUT | 1759.10 OUT (457) | 399/400 |
| `D2_YL_1..3` (yaw + knob) | 0.1938 in x3 | 0.2546 / 0.2544 / 0.2546 in | **1985.10 / 1987.02 / 1985.10 OUT, above 1982.44** (1347 / 1351 / 1346) | 398/400 x3 |
| `D2_YLFX_1` (same + diag logs) | 0.1938 in | 0.2544 in | 1986.59 OUT (1348) | 398/400 |

`YL` per-step speed against `orig_lx1`, aligned at the impact: within 0.06 % from 60 steps before it, 0.002 % at it, <=
0.53 % through +21 (second contact at +19, as the original). Then it grows to 1.4 % at +24 and 3 % at +30, comes back
to 0.17 % at +50, and the two runs part after about +300. The clamp-#6 series at +7..+12 takes the original's arms
with G within 0.5 % (`TABLE_YLFX.txt`).

## 5. D2 watch list (DEFERRED D-11071)

- **Grip-clamp #6 code:** read, not changed. Transcription confirmed against the original's live inputs (section 1
  reconstruction).
- **The speed tail `0x004710b0..0x004710da` (U-9212):** ported LUT-exact behind the knob, default OFF.
- **Player-car speed on Training:** driving-median 1860 -> 1736..1742 (knob, pinned) and -> 1985..1987 (knob +
  diagnostic yaw).
- `FUN_00538c80`, the chunk loop, `+0x4a4` and `ReassertContacts`: not touched.

## 6. User decisions needed

1. **The spawn heading.** The slot-0 grid direction comes from `FUN_00426cc0` (original) or the gate0 -> gate1 stand-in
   (port). This is d2posmodel 5.2's grid question and new U-9720. With the original's heading and the knob, D2's two slips
   pass and the trajectory matches through two wall contacts. Porting `FUN_00426cc0`'s direction for the initial grid is
   the grid lane's change and needs its own PREREG and G-CTRL re-baseline.
2. **`MASHED_D2LAT_REFRESH`** stays OFF by the section-5 rule. It is the faithful arithmetic, and alone it pins D2 the way
   fix C did. With the faithful spawn it is the arm whose slips enter. Whether to pair the two is the decision.

## 7. Follow-ups

- The next diverging term under `YL` is at the second contact (+19..+30, 0.5 % -> 3 %). Run the same fixup replay
  there.
- The driving-median overshoot under `YL` (1985 vs 1937.89) belongs to the trajectory after about +300. Bisect with
  per-step speed, not bands (memory `band-on-speed-compares-different-moments`).
- G-CTRL and D3 (b) under the diagnostic yaw are not measured: it touches every slot-0 spawn (StartRound included).
- `scenario_launch.py` now carries the shared frame marker (`lbseq`) and the clamp / fixup inputs. The reducers
  `d2lat.py`, `d2lat_wheels.py` and `fixup_replay.py` reproduce every number above.

## 8. Provenance

`RUNLOG.txt` (one line per port arm: exe sha8, knobs, pid, exit, output sha8). Original captures
`orig_lx1.msd(.latbracket.csv/.magprobe.csv)` and `orig_fx1.msd(.fixupprobe.csv)` with their `.provenance.json`
(argv = `orig_solo3`'s + the probe flags). Not committed (over 100 KB, or `*.log`): `.msd`, the probe CSVs, `*.a6a.log`,
`*.sink.log`, `*.wc.log`, `*.fixup.log`, step / gates / ptrace and motion_diag logs. Their sha8 are in `RUNLOG.txt`
(port) and in section 1 (original row counts).
