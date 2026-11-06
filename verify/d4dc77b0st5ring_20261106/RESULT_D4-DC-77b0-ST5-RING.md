# RESULT D4-DC-77b0-ST5-RING (2026-11-06)

**Verdict: GREEN, decision filed (no default flipped).** The original's ring call index at FUN_00414180 is measured:
k = 8 draws after the ring open, in 3 of 3 boots. All 8 are FUN_00463640 (engine audio) draws from the audio pass that runs
before the state-2 game tick. With those 8 draws ported behind `MASHED_ST5_LEG2`, the exe rolls cd = 3,3,3,3 without
`MASHED_RING_SKIP`, and knob-ON W-C4, W-HOLD and W-GO match the original. Default gates are unchanged.

Branch `orch/D4-DC-77b0-ST5-RING` (from main `adf5ff69b`). Commits: PREREG `642aa6ad0` (after the original boots and
the code, before any exe arm), probe + port `2d19a3424`, PREREG amendment 1 `9d4088914` (alone, before its re-run), gate
fix + scorers `cfc4537bd`, then this RESULT. Exe (worktree, `MASHED_NO_DEPLOY=1`): `mashed_re.exe` sha256
`e9201c99b58452ea...` (first try `0cde9a6d602d57da...`). Private root `D:/MashedWT/D4-DC-77b0-ST5-RING_root`.

## (1) Original ring index (3 boots, `--ring-probe`)

`scenario_launch.py --track 0 --mode 10 --cars 4 --rule 0 --poke-ctrl-slots --statediff-car 1 --statediff-aictrl
--ring-probe 4` (the ST5-HOLD recipe). The probe reads the FUN_00534870 block `S = [0x007dc578] + [0x007d3ff8]` (base,
p, q, end, and the 31 ring dwords) at arm time on the menu and at FUN_00414180 entry and leave. `ringmatch.py` matches each
state against the port ring (FUN_00534990 verbatim). `SCORE_ringmatch_20261106.txt`:

| boot | menu (arm) | FUN_00414180 entry (ret 0x0041025e, sub-state 2) | leave | cd from k=8 |
|---|---|---|---|---|
| r1, r2, r3 | k = 0 (p 3, q 0) | k = 8 (p 11, q 8) | k = 12 | 3, 3, 3, 3 |

The ring contents match the port ring exactly at every row, so seed, LCG and discards are confirmed live.

Callers (r2/r3, first 64 FUN_00534870 calls). FUN_00472650 is `push ecx / call 0x00534870` (0x00472650..0x00472651),
so `[esp+8]` at FUN_00534870 entry is its caller and `[esp+0xc]` / `[esp+0x10]` are lo / hi (`orig_r3.ring.csv`):

- Draws 0..7, sub-state 2, same frame as the roll: 0x004636f9 (lo 0, hi 250) and 0x0046386e (lo 0, hi 100), alternating,
  4 pairs. Draws 8..11: 0x004141da (lo 1, hi 5) = FUN_00414180's own 4 cd rolls.
- Both sites are in FUN_00463640. FUN_00466b50's race case loops v = 0..3 (0x00466cf4..0x00466d3d), calling FUN_00463640(v)
  (0x00466d04) when FUN_0040e370(v) != 0 (0x00466cf7). Main loop 0x004922a0: 0x004922b3 `call 0x00492d30` (the game tick,
  where FUN_00414180's chain returns to 0x004922b8), then 0x004922bd `call 0x004926c0`, whose 0x0049270c `call 0x00466b50`
  runs the audio. So the 8 draws come from the audio pass of the iteration before the state-2 game tick.
- FUN_00463640: no exit before 0x004636f4 `call 0x00472650` (push 0x437a0000 / push 0 at 0x004636ed / 0x004636f2). The rpm
  state switch at 0x004637c7 `jmp [eax*4 + 0x463c64]` (eax = `[0x0069054c + v*4]`, table 0x004637ce / 0x0046389d /
  0x004638f9 / 0x00463943 / 0x0046399d) sends state 0 to 0x004637ce. Every path from there reaches 0x00463869
  `call 0x00472650` (push 0x42c80000 / push 0 at 0x00463862 / 0x00463867). States 1..4 and > 4 (0x004639d5) do not draw.
  Two draws per car in this pass = every car in rpm state 0.
- Later draws (sub-state 3, draws 12..63) come from the audio pass again (4 to 8 per frame) and from 0x00442749 (0..7),
  0x00447f02 / 0x00447f22 / 0x00447f43 (0..0), 0x00421318 (0..1), and 0x0048a097..0x0048a114 (±0.0075, 0.015..0.04,
  ±0.001). None of them comes before FUN_00414180.

Why the exe drew 3,1,4,2: its engine voice (`Audio/AudioWire.cpp`) uses a private LCG (`s_rng`, line 91), and the exe's
first sub-state tick calls FUN_00414180 with no audio pass before it. So the roll was the ring's first consumer
(`draws_before=0`).

## (2) Port (knob ON only)

`TrackRenderer.cpp` `RingAudioPassDraws`: at the exe's state-2 entry, before `SubStateRollCountdown414180`, for each
car `FUN_00472650(0, 250)` then `FUN_00472650(0, 100)` on the shared ring (`Ai::Ai_RandRange`). The values are discarded.
It runs when `MASHED_ST5_LEG2` is on. `MASHED_NO_RING_AUDIO_PASS=1` opts out, and `MASHED_RING_AUDIO_PASS=1` turns it on
alone. With the knobs unset, no code path changes.

DEVIATION (amendment 1): the per-car gate is the exe's car set, `v < 1 + ai_cars_.size()`, not FUN_0040e370. The original
B captures read the slot table as 1,2,2,2 (scenario_launch.py FUN_0040e480 pokes). The exe's table is 1,0,0,0 at this
point (`Ai_SetCarSlotState` at race load, `TrackRenderer.cpp` H4 block; cars 1..3 become 2 only in the AI tick,
`AiController.cpp` FUN_00418860). The first try gated on `IsCarSlotActive` and drew for car 0 only
(`draws_before=312` = 310 + 2, cd 4,2,1,4; arms kept in `D:/MashedWT/D4-DC-77b0-ST5-RING_data/arms_try1`).

Not ported: the rpm state machine (`0x0069054c` / `0x0069052c` / `0x006904b4`) and every later audio pass, plus the
other later consumers listed above. After FUN_00414180 the exe ring and the original ring still diverge from sub-state 3 on.

Harness: `scenario_launch.py --ring-probe N` (read-only; FUN_00414180 entry + leave, and the first 64 FUN_00534870 calls,
then detached). `run_arms.py` stack `ST5L2NRA`.

## (3) Arms (exe e9201c99, one lock hold) — `SCORE_arms_20261106.csv`, `SCORE_cd_20261106.txt`

| arm | sha8 | FUN_00414180 line |
|---|---|---|
| H_DEF step / gates / ptrace | 8284be46 / ba407a16 / 31fe043e | draws_before=0 cd=3,1,4,2 |
| B_DEF_K0 / K1 / K26 | 1d5bf4f9 x3 | draws_before=0 cd=3,1,4,2 (K0) |
| B_ST5L2D_K0, B_ST5L2RD1_K0, B_ST5L2RD3_K0, B_ST5L2_K0 | ebc6afd1 | draws_before=318 cd=3,3,3,3 |
| B_ST5L2NRA_K0 | dda413d7 | draws_before=0 cd=3,1,4,2 |
| H_ST5L2 step / gates / ptrace | a6ec3b06 / 2adf0572 / 85b28ac8 | draws_before=318 cd=3,3,3,3 |
| B_ST5L2_K1 / K26 | 3acd6e97 / a0903356 | 319 cd=3,3,3,2 / 344 cd=4,3,1,4 (K = MASHED_RING_SKIP, off-phase samples) |

| witness | prediction | measured |
|---|---|---|
| G0 | merge_train refs | 8284be46 / ba407a16 / 31fe043e, 1d5bf4f9 x3: **GREEN** |
| R-CD | draws_before=318, cd=3,3,3,3 | all K0 knob-ON arms and H_ST5L2: **GREEN** |
| R-EQ | ebc6afd1 (= ST5-C4ACCEL amendment-1 `MASHED_RING_SKIP=8`) | ebc6afd1 on all four K0 knob-ON arms: **holds** |
| R-OFF | B_ST5L2NRA_K0 dda413d7 | dda413d7: **GREEN** |
| W-C4 | score_c4 exit 0, 5 non-exit 0xff calls per car, bf4 1300 at exit | 0 mismatches, 5 / 5 / 5, 1300 x3; W-HOLD 0 mismatches: **GREEN** (`SCORE_c4_20261106.txt`) |
| W-GO | frames 0..5 = 900/1 .. 100/1, 0/0 (original 877..882, 837..842) | cars 1 and 3 MATCH: **GREEN** (`SCORE_wgo_20261106.txt`) |

## (4) Decision filed

`roadmap.ps1 decision D4-DC-77b0-ST5-RING` (L-n in `log/orch/DECISIONS.md`). Flip `MASHED_ST5_LEG2` (which now carries the
audio-pass ring draws) to default ON. Expected refs with the knob ON: G-CTRL a6ec3b06 / 2adf0572 / 85b28ac8,
B ebc6afd1 / 3acd6e97 / a0903356 (K0 / K1 / K26). D2 and run13 are owed in the flip pass, as in L-90.

## Files

Committed (this folder): PREREG (with amendment 1), this RESULT, `ringmatch.py`, `score_c4.py` (copy from ST5-C4ACCEL),
`score_wgo.py`, `orig_r1/r2/r3.ring.csv`, `SCORE_ringmatch_20261106.txt`, `SCORE_arms_20261106.csv`,
`SCORE_cd_20261106.txt`, `SCORE_c4_20261106.txt`, `SCORE_wgo_20261106.txt`, `SHA256_raw_20261106.txt`.
Not committed (`D:/MashedWT/D4-DC-77b0-ST5-RING_data`): `arms/` (12 arms), `arms_try1/` (first try), `orig/` (r1..r3
logs and msd), `boot_ring.sh`, `arms.sh`.
