# RESULT: D2-BAND-42S (L-57, run 22)

Date 2026-10-29, LIVING-PC. Branch `orch/D2-BAND-42S` from `orch/integration` @ f6033d936. PREREG
`PREREG_D2-BAND-42S.md` committed alone at 46889ba71, before any run. Amendment 1 (capture-frames validity rule) committed
alone at 10ae8b17c, after o42_r1..r7 ran and with only o42_r1 scored. No source change, no default change, no .ps1 edit,
no tracker touched. `verify/d2rebase_20261019/score_d2_v2.py` and its `band.json` are not edited.

## 0. Verdict first

> **GREEN. The 42 s band is built from 8 valid original runs: `verify/d2band42_20261029/band.json`. D2_SUBH is IN on all
> three metrics, 3 of 3. D2_DEF under 42 s (D2_DEFH) is IN, 3 of 3, and gives the same values as D2_DEF at 30 s.**

| metric | 30 s band (d2rebase, `--hold 38`) | **42 s band (O42, `--hold 42`)** | O50 report (`--hold 50`, 7 runs) |
|---|---|---|---|
| slip1500 | 0.18396 .. 0.20586 (mean 0.194912, 3h) | **0.17665 .. 0.20605** (mean 0.19135, 3h) | 0.16964 .. 0.20924 (3h) |
| slip2000 | 0.24555 .. 0.25557 (mean 0.250563, 2%) | **0.24493 .. 0.25602** (mean 0.250475, 3h) | 0.24362 .. 0.25832 (3h) |
| drvmed | 1893.63 .. 1997.46 (mean 1945.5425, 3h) | **1886.07 .. 2042.76** (mean 1964.4125, 3h) | 1860.28 .. 2066.74 (3h) |

| port arm | slip1500 | slip2000 | drvmed | O42 band | 30 s band |
|---|---|---|---|---|---|
| D2_SUBH_1 / _2 (exe 58c21e05) | 0.1934 | 0.2515 | 1919.62 | in / in / in | in / in / in |
| D2_SUBH_3 | 0.1933 | 0.2515 | 1917.10 | in / in / in | in / in / in |
| D2_DEFH_1..3 (exe 7eee09a2, `MASHED_DRIVE_HOLD_MS=42000`) | 0.1942 | 0.2492 | 1896.74 | in / in / in | in / in / in |
| D2_DEF_1..3 (30 s, exe 58c21e05, reference) | 0.1942 | 0.2492 | 1896.74 | in / in / in | in / in / in |

Margins on the O42 band: SUBH drvmed 1919.62 is 33.55 above the floor (1886.07). DEF / DEFH drvmed 1896.74 is 10.67 above
it (3.11 above the 30 s floor). Every original and port run here scores H1 398/400, median 1333.9. Against the O50 report
band every port value above is also in.

## 1. The knob on the original side (PREREG section 1)

`MASHED_DRIVE_HOLD_MS` is standalone-only (one reader, `mashedmod/src/mashed_re/exe_main.cpp:1784`). The original-side
window is `re/frida/scenario_launch.py --hold <s>`, a wall-clock hold after race spawn. No .ps1 is involved and no .ps1
edit is needed. The 30 s band was built with `--hold 38` (u9763 provenance), so "30 s -> 42 s" has two readings: `--hold 42`
(O42, the literal 42 s, the band delivered) and `--hold 50` (O50, +12 s on 38, report only). The window witness (section 3)
puts O42 closer to SUBH. **Which window the flip uses is the lead's call.** This step delivers O42 as `band.json` and
O50 as `band_o50.json`.

## 2. Original runs (scenario_launch.py, MASHED_ROOT = `D:\MashedWT\D2-BAND-42S_root`)

Argv per run: `--statediff-out <msd> --statediff-drive --statediff-drive-late --statediff-steer 1 --hold <42|50>
--poke-ctrl-slots`, inside `test-slot` and `mashed_lock.py run --solo --label D2-BAND-42S` (`run_orig.py`). Audio endpoint
"Philips FTV (NVIDIA High Definition Audio)" status OK before every launch. No crash and no U-9677 collision in 18 launches.
Full lines: `RUNLOG_orig.txt`. Scores: `o42_scores.txt`, `o50_scores.txt`.

| run | frames | slip1500 | slip2000 | drvmed | driving n | valid (amend. 1) |
|---|---:|---:|---:|---:|---:|---|
| o42_r1 | 2547 | 0.1903 | 0.2505 | 1981.75 | 1366 | yes |
| o42_r2 | 2527 | 0.1937 | 0.2498 | 1937.89 | 1154 | yes |
| o42_r3 | 2085 | 0.1931 | 0.2474 | 1885.60 | 947 | **no** (-18% frames, replaced by r3b) |
| o42_r3b | 2522 | 0.1903 | 0.2505 | 1990.12 | 1350 | yes |
| o42_r4 | 2518 | 0.1917 | 0.2511 | 1969.40 | 1353 | yes |
| o42_r5 | 2536 | 0.1964 | 0.2531 | 1984.45 | 1354 | yes |
| o42_r6 | 2543 | 0.1881 | 0.2496 | 1957.58 | 1377 | yes |
| o42_r7 | 2552 | 0.1866 | 0.2494 | 1956.22 | 1395 | yes |
| o42_r8 | 2542 | 0.1937 | 0.2498 | 1937.89 | 1154 | yes |
| o50_r1 | 3027 | 0.1975 | 0.2504 | 1941.29 | 1150 | yes |
| o50_r2 | 3068 | 0.1871 | 0.2504 | 1945.41 | 1846 | yes |
| o50_r3 | 3039 | 0.1937 | 0.2498 | 1937.89 | 1154 | yes |
| o50_r4 | 3048 | 0.1852 | 0.2504 | 1954.73 | 1826 | yes |
| o50_r5 | 3048 | 0.1843 | 0.2507 | 2005.03 | 1889 | yes |
| o50_r6 | 3053 | 0.1929 | 0.2547 | 2006.71 | 1879 | yes |
| o50_r7 | 3054 | 0.1854 | 0.2504 | 1953.54 | 1832 | yes |
| o50_r8 | 2795 | 0.1853 | 0.2508 | 1957.66 | 1633 | **no** (-8.3%) |
| o50_r8b | 2861 | 0.1932 | 0.2521 | 1989.90 | 1699 | **no** (-6.1%), second failure for N=8 |

O42 median frames 2539, valid range 2412..2666. O50 median 3048, valid range 2896..3200. o42_r3's lock line reads
"draining the current holder" at boot and its car first moves at +23 s against +18 s on the other runs (`o42_r3.txt`).
The O50 runs queued behind other sessions' solo windows (secs 110-494) while other sessions' runs held 4 of the 6 test
slots. The PREREG's RED rule covers O42 only. O50 is short one valid run and is built from 7. It is report only.

**Why o42_r2 / o42_r8 / o50_r3 equal the 30 s band's solo3 / solo4 to four decimals:** they follow the same deterministic
trajectory, in which the car stops. On orig_solo3 (2333 frames), o42_r2 (2527) and o42_r8 (2542) the frames with horiz > 100
number exactly 1391, and the last frame's horiz is 16.9 / 9.8 / 9.9. Frames after the stop fail the reducer's speed filter,
so a longer hold adds no driving samples on that trajectory. Runs that keep driving (o42_r1, r3b, r4..r7) have 1350-1395
driving samples against 1150-1185 on the 38 s runs, and their drvmed is 1956-1990. That is the 42 s band's upward shift (mean drvmed
1945.54 -> 1964.41).

## 3. Window witness (report only)

| side | window | rows / frames | driving n |
|---|---|---:|---:|
| original, 30 s band (`--hold 38`) | 38 s after spawn | 2333-2336 (solo3, solo4, u9763 r1..r4) | 1150-1185 |
| original O42 (`--hold 42`), moving runs | 42 s | 2518-2552 | 1350-1395 |
| original O50 (`--hold 50`), moving runs | 50 s | 3048-3068 | 1826-1889 |
| port D2_SUBH (42 s, SUBSTATE) | 42 s after race entry | 1637-1639 rows | 1429-1431 |
| port D2_DEF (30 s) | 30 s | 1629 rows | 1208 |
| port D2_DEFH (42 s) | 42 s | 2344-2355 rows | 1208 |

SUBH's driving n (1429) sits nearest O42's moving runs (1350-1395). O50's moving runs have about 400 more.

**The port default stops the car.** D2_DEFH_1: 2353 motion rows, horiz > 100 on 1391 rows, last row with horiz > 100 is
1427, last horiz 13.66. D2_DEF_1 (30 s): 1627 rows, the same 1391 and 1427. So the extra 12 s adds rows after the stop and
no driving samples. That is why DEFH = DEF on every metric, and why 1391 is the same count as on the original's stopping
trajectory (section 2). D2_SUBH_1 keeps moving to its last row (1610 of 1636 rows with horiz > 100, last horiz 1797.03).
**[UNCERTAIN]** why SUBH does not take the stopping trajectory that both DEF and solo3 take. Evidence missing: a
position-and-contact trace of SUBH and DEF from row 315 (the D3-SUBSTATE-ROW315 split) to row 1427.

## 4. Files (verify/d2band42_20261029/)

- `band.json`: **the 42 s band (O42)**, same schema as `verify/d2rebase_20261019/band.json` plus a `label` field.
- `score_d2_v2.py`: byte-identical copy of `verify/d2rebase_20261019/score_d2_v2.py` (sha256 1c48e9fe...6c20 on both).
  The scorer reads `band.json` from its own folder, so `py -3.12 verify/d2band42_20261029/score_d2_v2.py port <log>`
  scores against the 42 s band. Its `BAND_PROVENANCE` string still names the d2rebase file. That constant is never read.
- `band_o50.json`: the O50 report band (7 runs).
- `build_band.py`: the band rule. Run on `verify/d2rebase_20261019/orig8_scores.txt`, it reproduces the 30 s `band.json`
  values exactly.
- `run_orig.py`: the original-side runner. `RUNLOG_orig.txt`, `RUNLOG_port.txt`: one line per run.
- `o42_scores.txt`, `o50_scores.txt`, `SCORE_PORT.txt`: scorer output.
- `raw_sha256.txt`: 64-hex sha256 of every raw file cited (18 captures + their run logs, the DEFH logs, the SUBH / DEF logs,
  build.log, the exe).
- `verify/savelink_20261023/run_arms.py`: one added line, stack `DEFH` = `MASHED_DRIVE_HOLD_MS=42000`.

## 5. Raw (D:/MashedWT/D2-BAND-42S_data)

`orig/o42_r*.msd`, `orig/o50_r*.msd` (sha8: o42_r1 16f1b634, r2 91d6682f, r3 98384f0f, r3b 6ce62e80, r4 e5ee7952,
r5 17322051, r6 aa900a50, r7 7753e444, r8 dbfb8c35. o50_r1 1b598bf1, r2 5b910e1d, r3 5bf2fbc9, r4 d9306d5c, r5 3763a5f1,
r6 c40d2fc2, r7 d89eeb33, r8 d40b2497, r8b ffdcba37), plus `.provenance.json` and `.txt` per run. `arms/D2_DEFH_1..3.motion_diag.log`
(sha8 a9b63ab2 / 4ace5ea2 / ca446e21). SUBH / DEF logs stay in `D:/MashedWT/D3-SUBSTATE-SETTLE-ORDER_data/final/`.
Exe `mashedmod/build/mashed_re.exe` 7eee09a2 (`MASHED_NO_DEPLOY=1` build of f6033d936). Full hashes: `raw_sha256.txt`.

## 6. Process

Every MASHED.exe and mashed_re.exe launch ran inside `test-slot` and `mashed_lock.py run --solo --label D2-BAND-42S`, alone.
scenario_launch.py spawned and ended its own PIDs. run_arms.py killed only its own PIDs at the 110 s wall. Nothing else
was killed. No .asi deploy. `original\` untouched (the private root is a robocopy, `MASHED.exe.unpatched` = the anchor).
Gates: not run. No source changed. The exe is integration HEAD's.
