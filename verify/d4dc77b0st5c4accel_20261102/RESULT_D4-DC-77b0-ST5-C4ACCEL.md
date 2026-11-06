# RESULT D4-DC-77b0-ST5-C4ACCEL (2026-11-02, arms 2026-11-06)

**Verdict: DECISION.** The c4 cause is found and fixed behind `MASHED_ST5_LEG2` (default stays OFF). The fix holds:
W-C4 0 mismatches, W-HOLD 0 mismatches, and the default gates are unchanged (G0). W-GO is red as preregistered:
knob ON gives car 1 700 / 1 and car 3 800 / 1 at frame 0, against 1100 / 1. The remaining difference is measured and
is not in the knob. The original draws the FUN_00414180 per-car value cd = 3 for all four cars, while the exe draws
3, 1, 4, 2 from an earlier phase of the same ring. With the exe's ring moved onto the original's draw (diagnostic
`MASHED_RING_SKIP=8`), every amendment-1 prediction holds and the knob-ON boost sequence equals the original's frame
for frame. No flip is recommended until the ring phase at FUN_00414180 is decided (decision row below).

Branch `orch/D4-DC-77b0-ST5-C4ACCEL`. Commits: PREREG `b28e1b609` (alone, before code and arms), code `bbe67dc6e`,
handoff RESULT `337e0d634`, merge of orch/integration `ed486d1ce` (ledger files only), arm scores `9c2ad1492`,
PREREG amendment 1 `3e8891262` (alone, before its arms), run_arms stacks `98a037a35`, original-boot summary `65976acc4`,
amendment scores `28d8f5ee8`, this RESULT.
Built exe (worktree, `MASHED_NO_DEPLOY=1`): `mashedmod/build/mashed_re.exe` sha256 `34530fab3126354e...`.
Display checked first: `scripts/diag.py probe-render` = `RENDER: OK`.

## Why the exe AI wrote c4 from sub-state-5 call 1 (static, MASHED.exe.unpatched)

FUN_00418560 (the per-car AI step FUN_00418860 calls):

| address | instruction | meaning |
|---|---|---|
| 0x0041857d..0x0041858b | `mov byte [edi+4/5/0/1/7/6], bl` (bl = 0) | ctrl block 0x007f1038 + slot*0x4c zeroed |
| 0x0041858e / 0x00418593 | `call 0x0040e350` / `cmp eax,5` | 0x0040e350 = `mov eax,[0x0063ba8c]; ret` |
| 0x00418596 | `jne 0x004185e5` | mode != 5: the control step (0x004185e7 `call 0x00417180` onward) |
| 0x00418598 / 0x0041859e | `mov [0x007f0ff8],ebx` / `call 0x00413fe0` | mode 5 |
| 0x004185a3 | `call 0x0040e4a0` | = `mov eax,[0x005f29b8]; ret` |
| 0x004185a8 / 0x004185ab | `imul ebp,ebp,0x74` / `mov ecx,[ebp + 0x0089a4f8]` | cd of car v (ebp = arg v, 0x00418562) |
| 0x004185b5..0x004185bd | `add ecx,0x40` / `imul ecx,ecx,0x64` / `cmp eax,ecx` / `jge 0x00418841` | cd >= 0 |
| 0x004185c3 | `mov byte [edi+4],0xff` then ret | c4 = 0xff only below the threshold |
| 0x004185cb..0x004185dd | `(0x4a - cd) * 0x64`, same compare, c4 = 0xff | cd < 0 |
| 0x00418841 | `pop edi / pop ebp / pop ebx / ret` | no control step, no replay-slot store |

FUN_00413fe0 (0x0041859e) zeroes 12 dwords per 0x74 record from 0x0089a4ec (0x00413ff2..0x00414012); none of them is
record + 0x38, so cd survives it. cd is written only by FUN_00414180 at 0x00414205 (`mov [esi+0x38],eax`,
esi = 0x0089a4c0 + v*0x74), from `_ftol2(FUN_00472650(lo, hi))` with lo / hi = row `_ftol2(DAT_0089a360)` of
0x005f2d80 (0x004141a0..0x004141d5). FUN_00472650 = `lo + (hi - lo) * (FUN_00534870() & 0x7fffffff) * 2^-31`
(0x00472650..0x00472681).

The exe's `Ai::VehicleStep` carried this branch only behind `MASHED_MODE5_RESET` (default OFF, set by no arm). On every
exe state-5 tick it ran `AiBankSwitch` + `ControlStep` (FUN_00416250, accel stores 0xff at 0x004167dc and 0x40 at
0x00416896). That was ST5-HOLD's 0xff x 112 (cars 1, 2) and 0xff then 0x40 x 111 (car 3).

## Change (knob ON only)

- `VehicleStep` takes the mode-5 branch when `MASHED_MODE5_RESET` is set or `MASHED_ST5_LEG2` is on. Knob unset: no
  code path changes. The stale 2026-10-08 DEVIATION note is replaced by the listing citations.
- `MASHED_ST5_HOLD_DIAG` (log only): each `[st5hold]` line also prints `cd=`, `c29b8=` and `bf4=`.
- `verify/savelink_20261023/run_arms.py`: amendment-1 stacks (ST5L2RS8D, ST5L2RS8RD1/3, RS8RD1/3). No code change.

## Main arms (exe 34530fab, private root D:/MashedWT/D4-DC-77b0-ST5-C4ACCEL_root, one lock hold)

Table: `SCORE_arms_20261106.csv`.

| arm | sha8 | ref |
|---|---|---|
| H_DEF step / gates / ptrace | 8284be46 / ba407a16 / 31fe043e | equal (G0) |
| B_DEF_K0 / K1 / K26 | 1d5bf4f9 x3 | equal (G0) |
| B_RD1_K0, B_RD3_K0 | 1d5bf4f9 | equal |
| H_ST5L2 step / gates / ptrace | 3b2e4652 / c292dab8 / fdfcb362 | knob ON |
| B_ST5L2_K0 / K1 / K26 | dda413d7 / 23a12e1d / dda413d7 | knob ON (K1 differs from K0 / K26) |
| B_ST5L2D_K0, B_ST5L2RD1_K0, B_ST5L2RD3_K0 | dda413d7 | knob ON |

- **G0: GREEN.** Default refs equal merge_train.json (`gctrl.ref` 8284be46 / ba407a16 / 31fe043e, `b.ref` 1d5bf4f9 x3).
- **W-C4: GREEN.** `score_c4.py B_ST5L2D_K0.log`: 0 mismatches over 111 non-exit calls x 3 cars (`SCORE_c4_20261106.txt`).
- **W-HOLD: GREEN.** 0 mismatches, held x / z equal ST5-HOLD's for cars 1..3.
- **W-GO: RED** (`SCORE_wgo_20261106.txt`).

| car | cd (diag) | non-exit 0xff calls | bf4 at exit (after release) | frame 0 knob ON | frame 0 default |
|---|---|---|---|---|---|
| 1 | 1 | 1 | 1100 | 700 / 1 | 1100 / 1 |
| 2 | 4 | 7 | 1400 | (not dumped) | |
| 3 | 2 | 3 | 1200 | 800 / 1 | 1100 / 1 |

## Original evidence: cd = 3 on every car

Two original boots (private root, `scenario_launch.py --track 0 --mode 10 --cars 4 --rule 0 --poke-ctrl-slots
--hold 30 --statediff-car 1|3 --statediff-aictrl --peek ...`), both rc 0, VB-Audio first. Summary:
`SUMMARY_orig_cd_20261106.txt`.

- Every peek of both boots (9 per boot, state 3 through 7): 0x0089a4f8 / 0x0089a56c / 0x0089a5e0 / 0x0089a654 =
  3 / 3 / 3 / 3, DAT_0089a360 = 2.5 (row 2 of 0x005f2d80 = lo 1, hi 5).
- Car 1: c4 = 255 from frame 871, +0xbf4 50, 100, 150, 200, 250 on 871..875, 1100 / 1 on 876, then 900, 700, 500, 300,
  100, 0 on 877..882. Car 3: the same on 831..842. This repeats ST5-HOLD's three captures.
- With 0x005f29b8 stepping 50 per call to 6450 on the last non-exit call (exe diag, `c29b8=`), T(3) = (3 + 0x40) * 100 =
  6700 gives exactly 5 non-exit 0xff calls (6650 .. 6450), the original's 5 frames.

Exe side: the `[substate] FUN_00414180` line reads `draws_before=0 idx=2 cd=3,1,4,2`. FUN_00414180 is the exe's first
FUN_00534870 consumer. `ringsim.py` (offline copy of the port ring: FUN_00534990 seed 0x9a319039, LCG fill, 310
discards) gives 3, 1, 4, 2 at skip 0 and 3, 3, 3, 3 first at skip 8 (then 247, 248, 249, 445, ...).

## Amendment 1 arms (diagnostic, `MASHED_RING_SKIP=8`)

`SCORE_a1_20261106.txt`. All four preregistered predictions hold.

| witness | prediction | measured |
|---|---|---|
| A1-CD | cd = 3,3,3,3 in all four arms | 3,3,3,3 in all four |
| A1-C4 | score_c4 exit 0, 5 non-exit 0xff calls per car, bf4 1300 on the exit line | exit 0, 5 / 5 / 5, 1300 / 1300 / 1300 |
| A1-GO knob ON | frame 0 = 900 / 1, cars 1 and 3 | 900 / 1 both, then 700, 500, 300, 100, 0 (frames 1..5) |
| A1-GO default | B_RS8RD1_K0 frame 0 = 1100 / 1 | 1100 / 1, step sha8 1d5bf4f9 (= default) |

Step sha8: B_ST5L2RS8D_K0 = B_ST5L2RS8RD1_K0 = B_ST5L2RS8RD3_K0 = ebc6afd1.

Reading: once cd matches, the knob-ON exe charges 300, releases at 1300 (0x0046d7c7 `add ecx,0x3e8`), steps to 1100
on the exit call's leg 2 (not in the dump), and dumps 900 / 700 / 500 / 300 / 100 / 0 on race frames 0..5. That is the
original's sequence 1100 (876, its exit frame), 900 .. 0 (877..882). W-GO's 1100 / 1 bar came from the default exe,
which runs no physics on the exit call, so for a knob that adds the exit call's physics it is one frame off. W-GO
stays red as preregistered. The default B gate does not move with the ring skip in this window.

## Decision filed (no default flipped)

`roadmap.ps1 decision D4-DC-77b0-ST5-C4ACCEL` (L-n in the decisions file). Options:

- (a) Flip `MASHED_ST5_LEG2` now. Expected refs with the knob ON, ring as today: G-CTRL 3b2e4652 / c292dab8 /
  fdfcb362, B dda413d7 / 23a12e1d / dda413d7 (K0 / K1 / K26). Cost: AI start charges differ from the original
  (cd 1 / 4 / 2 against 3 / 3 / 3, car 2 releases at 1400). D2 + run13 would be owed in the flip pass.
- (b) **Recommended.** First a step that measures the original ring's call index at 0x00414180 (peek the FUN_00534870
  state `[0x007dc578] + [0x007d3ff8]`, ring base and cursors, at state 2, and match it in `ringsim.py`), then port the
  earlier consumers or decide the phase. Refile the flip with the hashes from that build.
- (c) Keep it OFF and park the row.

## Files

Committed (this folder): PREREG (with amendment 1), this RESULT, `score_c4.py`, `ringsim.py`,
`SCORE_arms_20261106.csv`, `SCORE_c4_20261106.txt`, `SCORE_wgo_20261106.txt`, `SCORE_a1_20261106.txt`,
`SUMMARY_orig_cd_20261106.txt`, `SHA256_raw_20261106.txt`, `SHA256_orig_20261106.txt`. Source:
`mashedmod/src/mashed_re/Ai/AiStandalone.cpp`, `mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp`,
`verify/savelink_20261023/run_arms.py`.

Not committed (D:/MashedWT/D4-DC-77b0-ST5-C4ACCEL_data): `arms/` (13 arms), `arms_a1/` (4 arms), `orig/` (p_c1, p_c3),
`arms.sh`, `arms_a1.sh`, `boots.sh`, outer logs. SHA-256 of every cited raw file: `SHA256_raw_20261106.txt`
(e.g. `B_ST5L2D_K0.log` b6d05bd9...) and `SHA256_orig_20261106.txt`.
