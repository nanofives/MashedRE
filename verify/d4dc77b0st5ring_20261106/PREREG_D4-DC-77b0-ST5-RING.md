# PREREG D4-DC-77b0-ST5-RING (2026-11-06)

Ledger row D4-DC-77b0-ST5-RING (L-90 answered (b)). Written after the three original boots and after the code change,
before any exe arm of this step ran.

## Original measurement (done, boots r1..r3, before this file)

`scenario_launch.py --track 0 --mode 10 --cars 4 --rule 0 --poke-ctrl-slots --statediff-car 1 --statediff-aictrl
--ring-probe 4` on a private root (r1: ring state only, r2/r3: plus the first 64 FUN_00534870 calls with their caller).
`ringmatch.py` matches each captured ring state (base, p, q, 31 dwords at `[0x007dc578] + [0x007d3ff8]`) against the
port ring (FUN_00534990 verbatim):

- menu (arm): k = 0. FUN_00414180 entry (return 0x0041025e, sub-state 2): k = 8. Leave: k = 12. cd draws from k = 8:
  3, 3, 3, 3 (the peeked cd).
- The 8 draws: FUN_00472650 returning to 0x004636f9 (lo 0, hi 250) and 0x0046386e (lo 0, hi 100), 4 pairs, sub-state 2,
  same statediff frame as the FUN_00414180 call. Both sites are in FUN_00463640 (engine audio per car), called from
  FUN_00466b50's race case (0x00466d04) behind FUN_0040e370(v) (0x00466cf7). Full listing citations: the comment over
  `RingAudioPassDraws` in `mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp`.

## Change

`RingAudioPassDraws`: at the exe's state-2 entry, before `SubStateRollCountdown414180`, per active car
FUN_00472650(0, 250) then FUN_00472650(0, 100) on the shared ring, values discarded. On with `MASHED_ST5_LEG2`,
`MASHED_NO_RING_AUDIO_PASS=1` opts out, `MASHED_RING_AUDIO_PASS=1` turns it on alone. Knobs unset: no code path changes.
New run_arms stack: `ST5L2NRA` = ST5L2 + `MASHED_NO_RING_AUDIO_PASS=1`.

## Predictions (exe arms, one private root, one lock hold)

| id | arm(s) | prediction |
|---|---|---|
| G0 | H_DEF, B_DEF_K0/K1/K26 | merge_train refs: 8284be46 / ba407a16 / 31fe043e, 1d5bf4f9 x3 |
| R-CD | B_ST5L2D_K0, B_ST5L2RD1_K0, B_ST5L2RD3_K0 | log line `[substate] FUN_00414180 draws_before=8 ... cd=3,3,3,3` |
| R-EQ | B_ST5L2D_K0, B_ST5L2RD1_K0, B_ST5L2RD3_K0 | step sha8 ebc6afd1, the amendment-1 MASHED_RING_SKIP=8 value (the exe draws nothing before state-2 entry, so 8 draws there = 8 skips at the open) |
| R-OFF | B_ST5L2NRA_K0 | step sha8 dda413d7 (the ST5-C4ACCEL knob-ON value): the opt-out restores the old ring phase |
| W-C4 | B_ST5L2D_K0 | `score_c4.py` exit 0, 5 non-exit 0xff calls per car, bf4 1300 on the exit line |
| W-GO | B_ST5L2RD1_K0, B_ST5L2RD3_K0 | knob-ON +0xbf4/+0xbf8 on race frames 0..5 = 900/1, 700/1, 500/1, 300/1, 100/1, 0/0, the original's frames exit+1..exit+6 (`SUMMARY_orig_cd_20261106.txt` of ST5-C4ACCEL: 877..882 car 1, 837..842 car 3). The exit frame itself (1100/1) is not in the port dump (ST5-C4ACCEL amendment 1 reading) |
| M | H_ST5L2, B_ST5L2_K0/K1/K26 | measured only: the knob-ON gate hashes for the refiled flip |

GREEN = G0, R-CD, R-OFF, W-C4 and W-GO hold. R-EQ is a consistency witness: if it fails while R-CD holds, report the
difference, it does not by itself make the step red.

## Amendment 1 (2026-11-06, after the first arm set, before its re-run)

First arm set (exe 0cde9a6d): G0 held, R-OFF held (B_ST5L2NRA_K0 dda413d7), R-CD failed: knob-ON logs read
`draws_before=312 ... cd=4,2,1,4`, i.e. the ring open's 310 discards plus 2 pass draws. The per-car gate
`IsCarSlotActive(v)` reads the exe's slot table, 1,0,0,0 at state-2 entry (`Ai_SetCarSlotState` at race load; cars 1..3
are set to 2 only in the AI tick), where the original's B captures read 1,2,2,2 (scenario_launch.py FUN_0040e480 pokes),
so the pass drew for car 0 only. Change: the gate is the exe's car set, `v < 1 + ai_cars_.size()` (DEVIATION comment
at `RingAudioPassDraws`). Predictions unchanged, with R-CD read as `draws_before=318` (310 + 8) on the K0 arms and
`cd=3,3,3,3`. Every arm of the first set is re-run on the new exe; the first set is kept as evidence of the failure.
