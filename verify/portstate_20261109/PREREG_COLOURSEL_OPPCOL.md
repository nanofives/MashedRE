# PREREG PORT-COLOURSEL-OPPCOL (2026-11-10, U-9672)

Branch `orch/PORT-COLOURSEL-OPPCOL` = `orch/PORT-COLOURSEL-COMMIT` 502277adf (queued, not merged). This PREREG is committed
alone, before the code and the exe arms.

Disclosure: the original-side slot-colour values for N = 1, 3, 6, 5 are already known from PORT-COLOURSEL-COMMIT arm 1
(`D:/MashedWT/PORT-COLOURSEL-COMMIT_data/race_n*.peek.json`, RESULT_PORT-COLOURSEL-COMMIT section 4). They are reused,
not re-run. The challenge-select peeks of arm 1b (section 3) have not run yet. No exe arm of this step has run.

## 1. Static reading (bytes of `original/MASHED.exe.unpatched`, `re/tools/disasm_va.py`; decomp `re/tools/decomp_pc.py`)

### 1a. Colour confirm tail (action 0xff1d0000, cup flow)

After the DAT_007f1a1c commit (ported by PORT-COLOURSEL-COMMIT, 0x0043e56c..0x0043e57e) and the FUN_0043d2a0(6,0) push
(0x0043e586): `call 0x00431d00` (0x0043e58e), then `mov [0x0067ea78],ebx` with ebx = 0 (0x0043e593).

FUN_00431d00 (98 B, C4 by match, exe body `Match/MatchS07_00431d00.cpp` = `re/match/s07/00431b80.c`, exported as
`CarSelectReset`): DAT_0067ea74 = 1, DAT_0067ea90 = 1, DAT_0067ea94 = 0, DAT_0067ea98/9c/a0 = 1/2/3, DAT_0067eaac = 0,
then FUN_00431b80(0,1,0), (1,1,0), (2,1,0), which move each of 0x0067ea98/9c/a0 off DAT_007f1a1c + 1 and off each other.
No exe caller reaches CarSelectReset today (MatchS07_00431d00.cpp header).

### 1b. Challenge launch opponent colours (action 0xff260000, DAT_0067e9fc in 3/4/5)

The event type is DAT_005f65c8[(tier + DAT_0067f17c*3)] & 0xffff0000 (tier 0/1/2 for mode 3/4/5), already baked in
`Race/RaceModes.cpp` kCupEvent. Five of its arms run the same slot loop. The table pointer is FUN_00413f90 =
`return &DAT_005f2b10` (0x00413f90). Row = DAT_0067f17c * 0x30, tier offset +0x10 (mode 4) or +0x20 (mode 5). Entry =
{count, c1, c2, c3} (dwords).

| arm (event) | loop start `mov ebp,0x7f1a2c` | player-colour read | colour store `mov [ebp],ecx` | slot +4 store |
|---|---|---|---|---|
| 0x80000 | 0x0043eb4a | 0x0043eb76 | 0x0043eba4 | 1 (0x0043ebab) |
| 0x50000 | 0x0043ec6d | 0x0043ec98 | 0x0043eccc | 1 |
| 0x20000 / 0x30000 / 0xb0000 / 0x60000 | 0x0043edfa | 0x0043ee26 | 0x0043ee60 | 1 |
| 0x00000 / 0x10000 / 0xc0000 | 0x0043ef4f | 0x0043ef7a | 0x0043efac | 1 (0x0043efb3) |
| 0x90000 / 0xa0000 | 0x0043f080 | 0x0043f0ab | 0x0043f0dc | 0 (0x0043f0e3) |

Events 0x70000 and 0x40000 run no loop. Loop body (first arm, 0x0043eb50..0x0043ebb9): for i = 1..count, c = c_i
(0x0043eb74 `mov ecx,[edi]`); if c == DAT_007f1a1c (0x0043eb76..0x0043eb84), then k = 0..5 (0x0043eb86..0x0043eb9e): the
first k with k != DAT_007f1a1c, k != c1 ([ebx]), k != c2 ([ebx+4]), k != c3 ([ebx+8]) replaces c (0x0043eba2); none
keeps c. Store c at 0x007f1a2c + (i-1)*0x10 (0x0043eba4). FUN_00430670 (0x00430670) computes the same law into a local.

The race livery of slot i is DAT_007f1a1c[i*4] (FUN_0040d110, 0x0040d199..0x0040d1a6). The exe never writes
cfg.cars[1..3].colour, so those slots draw livery 0 (RaceSession.cpp SetCarColour).

Table DAT_005f2b10 (.data, file-backed, 13 rows x 3 tiers, only xref FUN_00413f90), row 0 = `1,0,-1,-1 | 3,2,3,4 |
2,5,1,2`. Full dump in the RESULT. The exe image pad does not hold it (not in `Compat/PortStateSeed.inc`), so it is baked.

## 2. Change (behind `MASHED_COLOURSEL_ICON`, default OFF, no new knob)

- `Frontend/MenuNavSM.cpp` ApplyActionGameMode, in the existing ICON + 0xff1d0000 + Q_ModeIsCupFlow block, after the
  DAT_007f1a1c store: `CarSelectReset()` (FUN_00431d00, 0x0043e58e), then DAT_0067ea78 = 0 (0x0043e593).
- `exe_main.cpp` MenuLaunchRace: ICON on, launch mode 3/4/5, event type of (mode, Nav_ChallengeRow()) not 0x7 / 0x4 and
  not -1: for i = 1..count of the baked DAT_005f2b10 entry, the loop law above, stored at 0x007f1a2c + (i-1)*0x10 and
  copied to cfg.cars[i].colour (range guard 0..5, as slot 0). Slots past count unchanged.

Not in this step: the launch arms' other stores (slot +0 / +4, DAT_0067ea64 / ea70 / ea74 / ea78 / ecdc, DAT_007f0fd0),
and FUN_0040e480(i,2).

## 3. Arms and predictions

Row 0, mode 3 (the reachable cup challenge on the baseline save): count 1, c1 = 0. Player colour p = N - 1.

| arm | what | prediction |
|---|---|---|
| 1 original (reused) | COMMIT arm 1 race peeks, 0x007f1a2c | N=1: 1. N=3, 6, 5: 0 (already observed) |
| 1b original | `menu_draw_burst.py --nav` confirm x3, moves, confirm, wait 3 s (challenge select, no launch); peek 0x0067ea74, ea78, ea90, ea94, ea98, ea9c, eaa0, eaac, 0x007f1a1c, depth, mode. N = 1, 3, plus a pre-confirm peek for N = 1 | ea74 1, ea78 0, ea90 1, ea94 0, eaac 0; ea98/9c/a0 = 4/2/3 (N=1), 1/2/4 (N=3) (hand run of re/match/s07/00431b80.c) |
| 2 exe ICON ON | MASHED_GOTO=1 + MASHED_NAV_KEYS as COMMIT arm 2 (N = 1, 3, 6, 5), new poller recording the arm-1b addresses + 0x007f1a2c/3c/4c, and the `[librwrest] livery: slot 1` line | slot 1 colour = arm 1 for every N. The arm-1b globals at challenge select = arm 1b |
| 3 exe ICON OFF | same keys, N = 1, 3 | slot 1 colour 0 (unchanged law), arm-1b globals not written by the confirm |
| 4 gates | `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` + `H_CSIX B_CSIX_K0` | G-CTRL 8284be46 / 67931834 / 31fe043e, B 1d5bf4f9 x3, both ways |

12-capture check: not run, no default path is touched (both changes are inside `ColourSelIcon_On()`).

GREEN = arm 2 equals arms 1 and 1b for every N, arm 3 unchanged, arm 4 unchanged both ways.

## 4. Default path (ICON OFF)

With ICON OFF the exe gives slot 1 colour 0 for every N, so N = 1 differs from the original (1). The ICON-OFF player
colour itself is not the original's either (COMMIT section 4: cursor 0x0067ea98 law), and the opponent law reads the
player colour, so a separate default-path opponent knob would sit on a wrong input. This step ships no default-path
change. The RESULT states the default-path gap and leaves it to the ICON flip decision.
