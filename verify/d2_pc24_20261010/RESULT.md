# RESULT: x87 PC_24 does not close the parked D2 gap

Pre-registration: `PREREG.md` (`0c784928`) and its addendum (arm C/D, registered before those runs).
D2 §16.7 arm, attempt-20 recipe (`a8_run_port.py`, 90 s, `participants=1` from `MATCH-SEED`), ORIG
reference `verify/d2_reopen_20260929/orig_solo3.msd`.

## 0. Verdict first

> **Not a D2 reopen candidate.** The arm closest to the original's precision (D) reads driving-median
> **1826.74**, the same as the control (1825.74) and ~78 below the 1904.70 bound. Precision does move the
> metric (C, +31), but the faithful direction does not close it.
> - **P1 PASS:** the exe's game thread runs at **PC_53** (`[cw] pc=0x10000`). It never inherits Direct3D's
>   PC_24, because the device is created on another thread.
> - **P2 PASS:** `MASHED_X87_PC24` sets PC_24 (`0x20000`).
> - **The x87 slice is inert:** A and B are byte-identical over all 1,607 common `motion_diag` rows.

| arm | build / precision | slip 1500-2000 | slip 2000-2600 | driving-median (n) |
|---|---|---|---|---|
| bounds | | 0.18855..0.19635 | 0.24488..0.25487 | 1904.70..1982.44 |
| A | default, PC_53 | 0.1943 | 0.2524 | 1825.74 (866) |
| B | default, PC_24 | 0.1943 | 0.2524 | 1825.74 (866), rows byte-identical to A |
| C | 25 physics TUs on x87, PC_53 | 0.1961 | 0.2541 | 1857.01 (877) |
| D | 25 physics TUs on x87, PC_24 | 0.1965 | 0.2536 | 1826.74 (866) |

C and D used a temporary build (`x87_tus.txt` + the 22 SSE2 physics TUs that carry `double`). The list
was restored and the default build re-verified at `ae67b13a / c8317ce2 / 31db9384`.

## 1. What it means

- The 1.3% driving-median residue (U-9180) is not a precision artefact of the "ST values are double"
  rule. Under the original's own precision model it reads the same as today.
- The control's driving-median (1825.74, n=866, `a8_medframe.py` defaults) is not the 1879.59 (n=1335)
  in attempt 20's table. The slips match the parked values (0.1943 / 0.2524). The difference in the
  driving-median population is [UNCERTAIN]: likely the scorer window (`--max-lines`) or D3-era commits
  since 2026-10-02. It is the same on every arm here, so it does not affect this verdict.
- The PC_24 finding still holds for the code that was measured: `FUN_00408610` (race loop) and the
  track-load path. It is a rule for verbatim transcriptions, not a D2 fix.

## 2. Kept

`exe_main.cpp` PumpOnce: `MASHED_LOG_CW` (logs the live precision bits) and `MASHED_X87_PC24`
(default OFF). Both are inert by default (G-CTRL above).

## 3. Raw

`A/ B/ C/ D/` (`motion_diag.log`, `PROVENANCE.txt`, `mashed_re.log` with the `[cw]` line), `*.run.txt`,
`SCORE.txt`.
