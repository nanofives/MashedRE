# RESULT: a port ring phase in regime C, and an exact roll model

Date 2026-10-09. Pre-registration: `PREREG_BREGIME_C.md` (`5f8d3e37`, committed before the sweep).
Default build, `MASHED_RING_SKIP` K = 12..63 (52 runs, all exit 0), plus K = 0..11 from `RESULT_BSCORE.md`.

## 0. Verdict first

> - **C1 holds, exactly.** The offline roll model with **n0 = 0** labels **all 64 ring phases with 0
>   mismatches** (the other candidates miss 16 and 23). So in the default build the band-0 roll is the
>   **first ring draw of the race**, and the draw count before it does not depend on draw values. The
>   map from K to regime is now known, not sampled: B at K = 1, 4, 27, 36, 47, 48, 55, 56, 58, 59, 60; C at
>   K = 26, 54, 57; A elsewhere in 0..63.
> - **C2 holds:** regime C at K = **26, 54, 57**, as predicted. **C3 holds:** each regime gives exactly one
>   window set (A 50 phases, B 11, C 3).
> - **Port regime C: 0/3** against the C envelope (original run 2, car 3 only, n = 1).
> - **Main finding: the port's regime-C windows are identical to its regime-A windows.** In the default
>   port, flag value 2 changes nothing inside the window. In the original, run 2's car 3 does differ
>   from regime A (steer# 121 vs 96, abs~ 36.5 vs 23, c1~ 27.5 vs 0).

## 1. Why flag 2 is inert in the default port (from the bytes and the exe source)

The original reads `DAT_0089a368` at (capstone scan, `0x0089a368` operands):

| site | function | test | in the default exe |
|---|---|---|---|
| `0x004148b0` | `FUN_004148b0` (leader timer) | `cmp eax, 2` | body exists (`AiLeaderTimer.cpp`, both targets), but its ControlStep call is wired only behind `MASHED_WIRE_B2` (default OFF) |
| `0x004158a5` | `FUN_00415880` (ram-from-behind targeting) | `test eax, eax` | targeting chain stubbed (`AiStandalone.cpp:903`) |
| `0x004169e0`, `0x00417130` | ControlStep / M49 accel x0.4 | `== 1` | ported (`:1065`) |
| `0x004172b7..0x004172d3` | line-type reset | `== 0/1/2` | ported (`:1431-1433`; 2 resets like 0) |
| `0x00417af5` | difficulty timer | `== 2` | ported (`:1665`; 180,000 to 1) |
| `0x00414256` | `FUN_00414220` (race state case-8 helper) | read | not an AI-step path |

So the only flag-2 readers that could shape the window are unwired (`FUN_004148b0`) or stubbed
(`FUN_00415880`) in the default port. [UNCERTAIN U-9204] that these are what moved the original's car 3:
in original run 2, cars 1 and 2 also stopped being stepped at 196 of 220 calls, which by itself can
change car 3's targeting. One run cannot separate the two.

## 2. Owed

- **U-9204:** a second original regime-C run (with all three cars full-length), to separate flag 2
  from the run-2 drop-out; then test the port with `MASHED_WIRE_B2` at K = 26.
- **A labelled original regime-B run** (unchanged from `RESULT_BREGIME.md`).

## 3. Raw

`PREREG_BREGIME_C.md`, `predict_regime_k.py`, `SWEEP2_RUN.txt`, `K12..K63.step.csv`, `SWEEP2_SCORE.txt`,
`score_bregime_c.py`.
