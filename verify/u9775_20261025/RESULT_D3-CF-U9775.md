# RESULT D3-CF-U9775: the original's run-to-run split is the A5 race-timer onset (2026-10-25)

Branch `orch/D3-CF-U9775` from `orch/integration` `3613b185b`. PREREG `PREREG_D3-CF-U9775.md` (`979fee4d0`, committed alone before
the knob arms). Knob + probes `4d7be2f10`. Exe `0732a032` (`MASHED_NO_DEPLOY=1`), root `D:\MashedWT\D3-CF-U9775_root`, 13 tracks on
`D:\MashedWT\D3-CF-U9775_root13`, host LIVING-PC, solo lock held for every run. No default changed.

## 0. Verdict

1. **U-9775 RESOLVED.** What varies run to run in the original is the player's first-moving call a0 (877..896 over 13
   captures) against the race timer `DAT_007f0ff8`, which runs 50 per call from the race start, countdown included. A5
   `FUN_0046ddb0` reads it (`FILD [0x007f0ff8]` at `0x0046e0a9`) for its grip ramp: `60000 - t` (`_DAT_005cea7c`) going negative
   starts `1 + (60000 - t) * (-1/30000)` (`_DAT_005cea74`, floor `_DAT_005cea78` -30000). So the ramp starts at k = 1203 - a0
   (record k counted from each side's first moving call), and that is the first call where two originals with different a0
   differ.
2. **The port's one realisation is outside the original's distribution.** The port's timer reads 0 at its first moving call
   (`MASHED_A5_RACE_TIMER_LOG`: 0 / 29900 / 59900 at slot-0 steps 0 / 600 / 1200), so its ramp starts at k 1200
   (`MASHED_NO_A5_RACE_TIMER=1` control first changes the record there). The originals start it at k 307..326.
3. **With the onset matched, the faithful player path is bit-exact against the original for the whole capture and passes
   D2 3/3.** `CLSF4` (default + `MASHED_COLL_LUT` + the five u9774 knobs) + `MASHED_A5_RACE_TIMER_ADD=44200` equals
   orig_solo3 / orig_solo4 / o_w2 (a0 887) over all ~1446 calls. D2: slip1500 0.1942, slip2000 0.2492, drvmed 1896.74 x3
   (floor 1893.63), H1 398/400. n 43700 equals o_w0 (a0 877) for its whole capture, and n 44650 equals o_ww9b4 (a0 896).
4. New **U-9790** (the port's race timer does not run through the countdown). The faithful start and the `COLL_LUT` default
   are decision rows, not taken here.

## 1. Measurements

| what | result |
|---|---|
| x87 CW / MXCSR at entry and leave of `0x00470c70` (native probe, 3 runs, 2346 / 2403 / 2396 calls) | `0x007f` (PC_24) / `0x9fb0` (FTZ) on every call: not the source |
| frame gap per record call, k 300..318 | 1 on every call: no skipped / doubled step |
| pairwise first split, 13 originals (`splitmatrix_1.txt`, `a0table_1.txt`) | equal a0 means bit-identical over the whole capture (887: solo3 / solo4 / o_w2, two hosts; 893: o_w3 / o_sn1; 885: o_cw1 / o_sn2 / o_ww9b0). Split k = 1203 - max(a0_A, a0_B), 12/12 against the trunk, 318 / 320 between side branches |
| `.data` images at dispatcher entry, o_sn1 (a0 893) vs o_sn2 (a0 885), k 296..315 (`snapdiff_sn1_sn2.txt`) | at entry of o_sn1's split call (k 309) the first new differences are A5-ramp-time state. `DAT_007f0ff8` = 50 (a0 - 1) + 50 k in both runs (60050 vs 59650 at k 309). The AI bank-switch words `0x0089a368` / `0x0089a370` (20.0166664) also change at that call |
| port timer, exe default | t = 0 at the first moving call, onset k 1200 |

| arm | D2 (score_d2_v2 port) | record vs originals |
|---|---|---|
| `D2_RT_1` default (+ dump / log) | 0.1900 / 0.2550 / 1965.67 PASS | onset k 1200 |
| `D2_CLSF4RT_1` CLSF4 | 0.1939 / 0.2537 / **1891.51 OUT** | = every original until that original's own onset |
| `D2_CLSF4A_1..3` CLSF4 + n 44200 | 0.1942 / 0.2492 / 1896.74 x3, H1 398/400 **PASS 3/3** | = orig_solo3, orig_solo4, o_w2 for the whole capture |
| `D2_CLSF4L_1` n 43700 | 0.1899 / 0.2506 / 1924.76 | = o_w0 for the whole capture |
| `D2_CLSF4H_1` n 44650 | 0.1935 / 0.2514 / 1928.03 | = o_ww9b4 for the whole capture |
| `D2_DEFA_1..3` default + n 44200 | 0.1874 / 0.2488 / 1928.43, 1928.43, 1930.04 (reported) | |
| `D2_NORT_1` timer opt-out (control) | 0.1901 / 0.2573 OUT / 1936.78 | |

The comparison word set is d2first.py's (0x928..0x9f0, 0xb00..0xb30 minus the matrix flag / pad and +0x9d4..dc lag words), with
`+0xb2c` excluded on port-vs-original rows (a sign-only 3.75e-08 difference from k 14, standing since u9774).

## 2. Gates (knob unset, exe `0732a032`)

| gate | measured | ref | verdict |
|---|---|---|---|
| G-CTRL `H_DEF` | `5943c5f1 / b493262b / 09551e6a` | merge_train.json | PASS |
| B `B_DEF_K0 / K1 / K26` | `3a03670c / 2516d903 / 53778664` | merge_train.json | PASS |
| D2 `D2_DEF_1..3` | 0.1900 x3 / 0.2550 x3 / 1965.35, 1964.25, 1965.67, H1 398/400, 1629..1632 lines | band | PASS 3/3 |
| 13 tracks | 39 rows, identical to `verify/rpct_20261022/tracks_flip_score.txt`, INSTRUMENT GATES PASS | | PASS |

## 3. Tools (this folder)

`splitprobe.py`, `firstwords.py`, `pads.py`, `splitmatrix.py`, `a0table.py` (`U9775_SKIP_B2C=1` drops `+0xb2c`), `trunkview.py`,
`snapdiff.py`, `snaptimer.py`, `run_arms.py` (savelink runner, paths changed, stacks RT / NORT / CLSF4A / CLSF4L / CLSF4H / DEFA).
`re/frida/scenario_launch.py` gained `--fpucw-probe N` and `--snap-probe n0:ns`, both native stubs with no JS in the hooked path.

## 4. Raw data (off git, `D:\MashedWT\D3-CF-U9775_data`)

SHA-256 of every cited raw file in `raw_sha256.txt`: o_cw1 / o_sn1 / o_sn2 (`.msd`, `.fpucw.csv`, `.snap.bin` 93 MB each),
`arms\D2_*` dumps / logs / motion_diag, `tracks\`. The archived originals are at `verify/d2_reopen_20260929/orig_solo{3,4}.msd`
(main checkout) and `D:\MashedWT\u9774_data\arms\o_*.msd`.

## 5. Next (decision rows, none taken)

- Faithful race timer: the original runs `DAT_007f0ff8` through the countdown (a0 - 1 calls before the first move, a QPC-length
  intro). The port's D2 arm has no such countdown. Options: (a) port the pre-move timer accumulation in the race flow,
  (b) fix the D2 arm's start offset to one original's a0 (887 = the trunk), (c) keep it as a named residual. U-9790.
- `COLL_LUT` default-ON: blocked only by the onset on this evidence. With n 44200 the faithful path is bit-exact and passes D2,
  but at 1896.74 it sits 3.1 above the floor. The other two onsets give 1924.76 / 1928.03.
