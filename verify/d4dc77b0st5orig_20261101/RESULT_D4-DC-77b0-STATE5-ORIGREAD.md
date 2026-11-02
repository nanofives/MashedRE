# RESULT D4-DC-77b0-STATE5-ORIGREAD (run 28, 2026-11-01)

**Verdict: GREEN.** The original takes the second FUN_004177b0 leg on **every** sub-state-5 dispatcher call. Two
original boots (TRAINING, Egypt) both show 112 of 112 case-5 calls with FUN_0040fc00 -> FUN_00425a40 (at 0x0040fc6c),
FUN_00425a40 returning 1 (not the early return), and FUN_00418860 / FUN_004177b0 running twice per call. The default exe
runs that pair once per step (STATE5CALLS RESULT), so the gap is real in the canonical scenario. As the task asked, the
port is now a separate ledger step, **D4-DC-77b0-STATE5-LEG2** (default-OFF knob, proposed `MASHED_ST5_LEG2`, PREREG
first). This step ports nothing. No `mashedmod/` change, no tracker change, no flip.

Base: `orch/integration` @ `ca02bde58`. Builds on `verify/d4dc77b0state5_20261030/` (call tree),
`verify/d4dc77b0writers_20261030/` (0x005f29c0 = 1 in the image), `verify/d4dc77b0decode_20261030/`.

## Method

- Harness: `re/frida/scenario_launch.py` (existing), extended with one read-only probe `--st5-probe N` (this branch).
  Entry hooks on FUN_0040fc00 (C), FUN_00418860 (S), FUN_004177b0 (B), and entry + leave on FUN_00425a40 (A / R, the
  leave logs EAX). Each row holds the return-address RVA and the dwords 0x0063ba8c, 0x005f29c0, 0x007f1a50, 0x007f1a54,
  0x007f1a70, 0x007f1a74, 0x007f1a78, 0x00801ca0, plus the tick 0x007f0ff4. Every call with 0x0063ba8c == 5 is logged.
  For any other sub-state, only the first 8 calls per (function, sub-state) are logged. All four functions run a few
  times per frame (C 1, A 1, S 2, B 2 in state 5), so none is a hot path. Writes nothing to the game.
- Recipe: `--mode 10 --cars 4 --rule 0 --poke-ctrl-slots --hold 30 --statediff-out <msd> --st5-probe 8`, with
  `--track 0` (TRAINING, run 1) and `--track 1` (Egypt, run 2).
- Private root `D:\MashedWT\D4-DC-77b0-STATE5-ORIGREAD_root` (robocopy of `original\`, patched MASHED.exe sha256
  `b9977dab...fe87`, `.unpatched` present), `MASHED_ROOT` set to it. Both boots ran under
  `mashed_lock.py run --label D4-DC-77b0-STATE5-ORIGREAD`. Before each boot, `Get-PnpDevice -Class AudioEndpoint -PresentOnly`
  listed the VB-Audio endpoints. Both boots reached phase 3 and exited cleanly. There was no crash and no piz collision.
- Run 1 note: in run 1 the C/S/B hooks used Frida's bare-function (probe) form, which has no valid `returnAddress`, so
  run 1's `ret` column for C/S/B holds the hooked function's own address. Run 1's counts, gate dwords, A/R rows and
  EAX are valid. The probe was switched to the `{onEnter}` form before run 2, and run 2's return addresses are valid.

## Gate dwords, every logged row of both runs (sub-states 3, 4, 5, 6, 7)

| address | run 1 (TRAINING) | run 2 (Egypt) |
|---|---|---|
| 0x005f29c0 | 0x1 (1) | 0x1 (1) |
| 0x007f1a50 | 0x0 (0) | 0x0 (0) |
| 0x007f1a54 | 0x0 (0) | 0x0 (0) |
| 0x007f1a70 | 0x0 (0) | 0x0 (0) |
| 0x007f1a74 | 0x0 (0) | 0x0 (0) |
| 0x007f1a78 | 0x0 (0) | 0x0 (0) |
| 0x00801ca0 | 0xc (12) | 0x14 (20) |

None of the eight dwords changes value within a run. The STATE5CALLS decode gives these consequences:
- FUN_0040fc00's `if (DAT_005f29c0 != 0) FUN_00425a40()` is taken.
- FUN_00425a40's early return requires `DAT_007f1a50 == 1`, and 0x007f1a50 is 0, so the early return is not taken. The
  `DAT_007f1a78` increment arm is not taken either (0x007f1a78 stays 0).
- FUN_00418860's `DAT_00801ca0 > 3` holds (12 and 20).

## Counts in sub-state 5 (0x0063ba8c == 5 at entry)

| row | run 1 TRAINING | run 2 Egypt | run 2 return address |
|---|---|---|---|
| C FUN_0040fc00 | 112 | 112 (1 from 0x004112b2 on the first state-5 tick, 111 from 0x004112c5) | |
| A FUN_00425a40 entry | 112 | 112 | 0x0040fc71 (call at 0x0040fc6c) |
| R FUN_00425a40 leave, EAX | 112, all 0x1 | 112, all 0x1 | |
| S FUN_00418860 | 224 | 224 = 112 from 0x00410413 (FUN_004103a0's call at 0x0041040e) + 112 from 0x00425a78 (inside FUN_00425a40) | |
| B FUN_004177b0 | 224 | 224, all from 0x00418905 (inside FUN_00418860) | |
| state-5 ticks | 35650..41250 (step 0x32 = 50) | 29150..34750 | |

So every A row has a matching R row with EAX 1, and every S call reaches B. That gives two FUN_004177b0 calls per
dispatcher call, one per caller site.

Edges (run 2, `EXTRACT_o_t1_r2_edges.csv`):
- First state-5 tick (29150): C enters from 0x004112b2 with 0x0063ba8c already 5. A, S(0x00425a78), B and R follow.
  There is no S from 0x00410413 on that tick. 0x00410413 appears from tick 29200 on, together with C from 0x004112c5.
- Exit tick (34750): S(0x00410413) and its B read 0x0063ba8c = 5. Then C (from 0x004112c5), A, S(0x00425a78), B and R
  read 6. This is what the WRITERS RESULT predicts: 6 is stored at 0x004104e3 between the two legs.
- Run 1's exit tick (41250) shows the same 5-then-6 split.

[UNCERTAIN] Which FUN_004111c0 arm 0x004112b2 belongs to (the first state-5 C call). The return address is measured,
the case label is not. That needs a disassembly read of 0x004111c0's switch around 0x004112ad. It does not change the
verdict: on that tick the second leg also runs (A/R/S(0x00425a78)/B present, EAX 1).

## Consequence for the exe

On each sub-state-5 step the default exe (MASHED_SUBSTATE=1) runs `AiPreTickRubberBand` once
(STATE5CALLS RESULT table, TrackRenderer.cpp:5270-5405). The original runs FUN_004177b0 twice, and it also runs the
whole FUN_00425a40 body (FUN_00423b00, the second FUN_00418860 with its FUN_00418560 loop, FUN_00424eb0, FUN_00470c70,
FUN_00422ba0, FUN_004252c0) 112 times per countdown on both tracks. Porting that body changes behaviour, so it is filed
as ledger step **D4-DC-77b0-STATE5-LEG2** (section "D4 allowlist", wave 4): default-OFF knob (proposed
`MASHED_ST5_LEG2`), PREREG committed alone first, default gates unchanged, knob-ON arms, then the flip as a decision row.

## Files

Committed (this folder):
- `SUMMARY_o_t0_r1.csv`, `SUMMARY_o_t1_r2.csv`: every probe row grouped by (kind, sub-state, return, EAX, gate dwords),
  with counts and the first and last tick.
- `EXTRACT_o_t0_r1_edges.csv`, `EXTRACT_o_t1_r2_edges.csv`: raw rows within one tick of each sub-state change.
- `re/frida/scenario_launch.py`: the `--st5-probe` option.

Raw (not committed), `D:/MashedWT/D4-DC-77b0-STATE5-ORIGREAD_data/`:

| file | SHA-256 |
|---|---|
| o_t0_r1.msd.st5.csv | 7c4f30197439f0ba0ace668b169f698eb669c799dcfd17e8388419c0b304a668 |
| o_t0_r1.log | cc96cbf5a0351e78cc9b6705d7bf8fd8346edc98983dd98ba658fbc90f56f4e7 |
| o_t0_r1.msd | e87aab10761fc7ae39699c70bb0834f33e0a7a0883c1451d30599d19c4bdfa90 |
| o_t0_r1.msd.provenance.json | 9ee11f3c416b2cdd9beaa542e495296703705cbe01d3cb47530f599f6e0c0f55 |
| o_t1_r2.msd.st5.csv | c18352b2650482b8589de7a6370ea7aa02f7d30a6f4afb595406ab534d03824d |
| o_t1_r2.log | a2d3fa22b2048ef486f374e10e4333d14b3b6e816881bc72714e322c08472baa |
| o_t1_r2.msd | 8bac3a3d554a8d63ff5e5e267daab04acf4e5e1022de576cfd246eb96b87a7c0 |
| o_t1_r2.msd.provenance.json | 923014decdc9b4a143242f8e53754096218ffde08b88bdce15325f58e8a7359f |

## Gates

Not run: this step changes no build input. The only code change is `re/frida/scenario_launch.py`, a Python/Frida
harness that is not compiled into `mashed_re.exe` or `mashed_re_dev.asi`. The default exe is byte-identical to
`orch/integration` @ `ca02bde58`, so G-CTRL and B stay at the `merge_train.json` refs.
