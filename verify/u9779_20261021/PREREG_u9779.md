# PREREG u9779: the round end at call 220 on the B recipe, K0 (2026-10-21)

Branch `work/u9779` from `d7afd53e4` (work/u9778 merged). Model claude-opus-5-5, account2. Exe `MASHED_NO_DEPLOY=1`, private root
`D:\MashedWT\u9779_root`. Evidence `D:\MashedWT\u9779_data` (never committed, `MOVED_OFF_GIT.txt`). Runner `run_arms.py` (copy of
u9778's, stacks `ET` / `ILET` added: `MASHED_ELIM_TRACE`).

## Step 1 (measurement, no knob) - done before this PREREG

### The original's 6 -> 7 writer and its trigger

Writers of `DAT_0063ba8c` in `MASHED.exe.unpatched` (byte search for the absolute address, `absref.py`): 19 immediate stores
(`C7 05 8c ba 63 00 imm32`) plus the setter FUN_0040e360 (`0x0040e364 mov [0x63ba8c],eax`). The only store of 7 is
`0x004111a6 mov dword ptr [0x63ba8c], 7` in FUN_00411170:

```
0041118f  call 0x410d10        ; FUN_00410d10 (segment check)
00411194  test eax, eax / je 0x4111b5 (ret)
00411198  call 0x410510        ; FUN_00410510 (result)
0041119d  test eax, eax / je 0x4111a6
004111a1  jmp 0x430290         ; match end
004111a6  mov dword ptr [0x63ba8c], 7
004111b0  jmp 0x418990         ; -> FUN_004194f0
```

New original captures, same recipe as `o_cc3_1` (`--track 0 --mode 10 --cars 4 --car 0 --poke-ctrl-slots --statediff-car 3
--statediff-aistep --hold 25`):
- `o_al_1` (+ `--alive-probe`): FUN_00410d10 runs 220 times (sub-state 6 only). On its 220th call (seq 219) the zoom `DAT_00898980`
  is exactly 10.0 (the `0x005cc55c` compare in the elimination block), the alive vector goes 1/1/1/1 -> 0/0/0/1 inside the one
  call, and it returns 1. `DAT_007f0fd0` reads 0 on every row.
- `o_ep_1` (+ `--alive-probe --elim-probe`, new probe: entry hooks on FUN_00422fd0, FUN_0040eee0, FUN_0040d590, FUN_0040b290,
  the tails 0x00418990 / 0x00430290): at that call the chain is `kill(0)`, `FUN_0040eee0(0,1)`, `FUN_0040b290(0,-2)`,
  `FUN_0040d590(3,1,2,1)`, `kill(1)`, `(1,-1)`, `kill(2)`, `(2,+1)`, `(3,+2)`, then the 0x00418990 tail with `DAT_0063ba8c` = 7.
  Scores 6,6,6,6 -> 4,5,7,8. Gate inputs: `DAT_0067e9fc` (FUN_0042f6a0) 10, `DAT_007f0fd0` 0, `DAT_008a94d0` 4, `DAT_0067ea64`
  (FUN_0042f500) 0, slot types `FUN_0040e470` 1,2,2,2.

So: the elimination block of FUN_00410d10 kills the player (FUN_00422fd0 zeroes the alive word through FUN_0046c5c0), FUN_0040eee0
counts 3 remaining, takes the `DAT_008a94d0 == 4`, no-team, 3-remaining arm, awards -2, and because the mode is in {3,4,5,10},
there are no teams and `DAT_007f0fd0 == 0`, walks the alive slots of type 2 and calls FUN_0040d590 on the three AI cars. That
kills two (by the wrap-adjusted FUN_00408ad0 race_pct compare) and awards +-1 / +2. FUN_00410d10's alive-count tail then sees 1
alive and returns 1, FUN_00410510 returns 0, and 0x004111a6 writes 7. In sub-state 7 FUN_0040fc00 skips the clock add
(0x0040fe46 cmp [0x0063ba8c],7 / 0x0040fe4d je over 0x0040fe4f..0x0040fe64).

### What the port does

New diagnostic `MASHED_ELIM_TRACE=<path>` (default OFF, one row per UpdateRace camera update; K0 step dumps `7ab70c5f` / IL
`9bd86ffe` = unchanged):
- The port kills the player at the same simulation point: its trace row 220 reads positions P220, the original's seq 219 reads
  the same positions (port row k+1 camera positions = the original's record k, bit-equal on car 3 at k 0..219), zoom 10.0 on
  both. The port's required zoom is exactly the pair term |sep| * 0.8; the original's is 0.03..0.045 higher from k~85 on (pairs
  with the player). It does not move the crossing frame here. Not pursued in this lane.
- `ScoreOnElimination` (TrackRenderer.cpp) awards -2 and stops. Its comment says the FUN_0040e470 type-2 arms are not ported. So
  cars 1, 2, 3 keep racing, the next eliminations are at port k 345 / 755 / 935, and the AI host keeps returning sub-state 6
  (`aib_game_sub_mode`, the stand-in for FUN_0040e350).
- Inputs the port holds for the gate: live `DAT_007f0fd0` 0 (= original), slot types 1,2,2,2 (= original), live `DAT_0067e9fc` 0
  (original 10, no exe writer exists), the AI host's FUN_0042f6a0 stand-in `aib_round_type()` 3, the rule engine's `rule_` 4
  (from the recipe's `MASHED_ROUND_RULE=4`; the original's `DAT_007f0fd0` is 0). The camera's progress inputs are the stand-in
  `race_[].progress`; the live race_pct `0x008a96ec` (PATHPROG) equals the original's bit for bit at k 0..219 (IL).

## Knobs (both default OFF)

- `MASHED_ELIM_RESOLVE`: in `ScoreOnElimination`'s no-team path, after the -2 / -1 award, the FUN_0040eee0 type-2 walk (3 remaining
  -> FUN_0040d590 on the three AI cars, 2 remaining -> the pairwise kill with +2 / +1), gated on `aib_round_type()` in {3,4,5,10},
  no team play and live `DAT_007f0fd0 == 0`. FUN_0040d590 runs through its one body (`Frontend::RaceRankThree`,
  MenuLeaves_af4.cpp, the existing .asi transcription refactored to take its callees; its .rdata wrap bounds are literals now
  because the exe reads them as 0) with the live race_pct, KillCar and ScoreAward.
- `MASHED_ROUND_END7`: when the rule engine ends a round with r == 0, the AI host's FUN_0040e350 returns 7 until StartRound, and
  the frame-end clock add is skipped.

## Predictions

- P0 (knobs unset): G-CTRL `15594ec8 / 7285d708 / 26a6e4ad`; B K0/K1/K26 `7ab70c5f / 15262e4a / d1b4bc64`; with
  `MASHED_SUBSTEP_INTERLEAVE=1` `9bd86ffe / 433abcd2 / 1a1ff727`. (The 0x0040d590 refactor is not on any default path.)
- P1 (`MASHED_ELIM_RESOLVE`, IL, K0): at the player's elimination the port also kills cars 1 and 2 and the scores become 4,5,7,8,
  as `o_ep_1`. The round ends in the same UpdateRace (RULE-EVAL at that row).
- P2 (+ `MASHED_ROUND_END7`, IL, K0): car 3's aistep rows from k=220 read substate 7 and c4 0 / c5 0, clk_0ff4 stays constant;
  cars 1 and 2 have no rows past 219. Car 3's first difference against `o_cc3_1` / `o_al_1` / `o_ep_1` moves past k=220. No value
  is predicted for where it lands.
- P3 (both knobs, interleave off, K0): the same three kills at the same row (player, car 1 and car 3 are bit-exact through 219
  without the interleave too; car 2's race_pct differs from 204 on).
- P4: (b) with both knobs: the (b) window is read from the step dump. If the dump is shorter (dead cars stop producing rows), a
  car whose window no longer exists is reported NOT SCORED, not PASS. Reported for K0 / K1 / K26 with the interleave off and on.
- P5: D2 has one car (MEASURE_SOLO): neither knob can fire, D2 values equal the default's. 13 tracks: reported, no prediction
  (the run13 recipe is one deterministic round; whether it reaches an elimination with the player first is not known).

## Gates

| gate | arms |
|---|---|
| G-CTRL knob-off | `H_DEF` x1 (must equal `15594ec8 / 7285d708 / 26a6e4ad`) |
| B dumps | `B_DEF_K0/K1/K26`, `B_IL_K*`, `B_R7_K*`, `B_ILR7_K*` (R7 = both knobs) |
| (b) | `vsum.py` on the B dumps, interleave off and on |
| D2 | `D2_R7_1..3` alone, `score_d2_v2.py` |
| 13 tracks | `run13.py run` with both knobs, `score` against `verify/rres_20261016/cites_rres.json` |
| G-MECH | `MASHED_ELIM_RESOLVE` alone and `MASHED_ROUND_END7` alone on K0 IL, to attribute |

Nothing is flipped. Default changes are Mariano's decision.

## Amendment 1 (after the first K0 knob arms, before any arm with the new knob)

Measured with `ER` / `E7` / `R7` on exe `0b275a85` (P0 held: K0 `7ab70c5f`, IL `9bd86ffe`):
- P1 holds: `ELIM-RESOLVE remaining=3 ... alive=0,0,0,1 scores=4,5,7,8 pct=8.164267,16.720179,18.977242,20.087664` (the
  original's o_ep_1 pct, bit-equal) and `RULE-EVAL ... r=0` in the same UpdateRace.
- P2 fails by one frame: car 3's row 220 still reads substate 6 / c4 255 in the port, 7 / 0 in the original; from row 221 the port
  reads 7 / c4 0 / c5 0 and the clock stays (11000 relative, = the original).

Cause, measured: in the original FUN_004111c0's sub-state-6 case calls FUN_00411170 (0x004112cd) BEFORE FUN_0040fc00
(0x004112d5), so the round end is decided on every car's position after the previous frame's physics, and the same frame's AI
tick reads it. The port's UpdateRace runs after the AI tick and the player's physics and before the opponent loop commits. Its
camera input is a mixed-time snapshot. `MASHED_ELIM_TRACE` row k holds the player at its record-dump frame k (`B_ILC0_K0.rec.msd`,
195/195 at offset 0) and cars 1 / 3 at frame k-1 (195/195 at offset -1, `B_ILC1/3_K0.rec.msd`).

New knob `MASHED_RACE_TAIL_LATE` (default OFF): UpdateRace moves to the end of UpdateCar (after the opponent commit, the late
path-progress tick and the pickup / registry ticks). Stack `TL`; `R7` is redefined as ER + E7 + TL; `R7m` = ER + E7 (the
amendment-0 R7).

- P6 (TL alone, knob-off for the other two, IL, K0): the step dump changes (the camera and the rules read one consistent
  snapshot). The port's required zoom at the new row k equals |sep| * 0.8 of a single-time snapshot; the 0.03 deficit on player
  pairs against `o_al_1` should vanish. Reported, not gated.
- P7 (R7 = ER + E7 + TL, IL, K0): car 3's row 220 reads substate 7, c4 0, c5 0; cars 1 / 2 have no row 220. First car-3 difference
  past k=220.
