# HB-TEXANIM-REFS RESULT (2026-10-27): Training and Sands move toward the original, Warzone does not move

**Verdict: DECISION. `MASHED_TEXANIM` stays default OFF.** On pinned water-view poses (PREREG2),
the knob takes the water gap to the original from 14.0 / 15.3 luma to 3.3 / 3.6 on Training and
from 99.1 to 3.9 on Sands. On Warzone it moves 0.2 luma away on both poses (16.5 -> 16.7,
10.1 -> 10.3). PREREG2 section 5 needs all three tracks TOWARD, so no flip. Warzone ships
`water00`, the ring's own node 0, so the knob changes only which ring frame is drawn there.
PREREG2 predicted a Warzone shift within about 1.5 luma before the arms ran. The 10 to 16 luma
Warzone residual is present with the knob OFF and ON.

- PREREG: [`PREREG.md`](PREREG.md), committed alone as `e33c83731` (round 1).
  [`PREREG2.md`](PREREG2.md), committed alone as `dcfea927a` (round 2) after round 1 and the
  standalone-only pose scan, before any round-2 capture or arm. Neither amended.
- Code: `13ca1b0aa` adds dev-only `MASHED_TEXANIM_MASKTEX=<name>` (`LibRw/RwRaceSubmit.cpp`,
  `RaceSubmit_TexAnimCreate`): with `MASHED_TEXANIM=1` the ring becomes that one texture, so
  `|MASK - OFF|` marks the visible water. With the knob unset there is no effect. No default changed.
- Exes: round 1 `acfa80318ab2641a...` (PREREG commit, no source change). Round 2 and gates
  `9f2afe9246c59abb...` (`13ca1b0aa`).

## 1. References (original side)

`re/frida/race_draw_burst.py` copied to a private root `D:/MashedWT/HB-TEXANIM-REFS_root` (its own
`original/` copy). That root's save was swapped to an unlocked copy
(`gamesave_edit.py --rows 0..12 --set c1=1,c3=2`, sha `b46217c0`) only for the original captures,
and restored to the stock `bd18788182b2` before every standalone run and the gates. The shared
`original/gamesave.bin` was never touched. Challenge index Training 0, Sands 5, Warzone 11.
Every capture's `ORIGINAL TRACK =` line names the expected track (15/15).

- Round 1: grid vantages, `--settle 8 / 14 / 20`, 9/9 PASS.
- Round 2: `--pose=<basis> --settle 8`, 6/6 PASS. The training pair first failed because a basis
  starting with `-` was read as a flag (`--pose <basis>`). They were re-captured with `--pose=<basis>`.
  Every echoed `orig_cambasis.txt` equals the requested basis.

## 2. Round 1 (PREREG.md): grid vantages, mask = knob footprint `|ANIM - OFF| > 6`

Full table: [`measure_r1.txt`](measure_r1.txt).

| vantage | % frame | original L | OFF L | ANIM L | dOFF | dANIM |
|---|---|---|---|---|---|---|
| training_s20 | 0.57 | 36.2 | 53.6 | 39.1 | 17.4 | 2.9 |
| training_s8 / s14 | 0.11 / 0.30 | not scored (< 0.5%) | | | | |
| sands_s8 / s14 / s20 | 0.00 | not scored | | | | |
| warzone_s8 / s14 / s20 | 0.00 | not scored | | | | |

R0 (OFF == OFF2, s8) bit-identical on all three. R2 0.001%. Sands and Warzone not scored, so round
1 alone could not decide. Two causes: Sands `WATER.DFF` is one plane at y = -1.23 spanning x, z in
[-169, 169], and the grid camera never sees it. Warzone `RIVER.DFF` ships `water00`, the ring's
node 0, so the knob footprint mask stays empty until the ring advances. (`measure_r1.txt` prints
Training as "TIE": a numpy-bool test in the first `measure.py` (`x is True`), fixed in `13ca1b0aa`.
The numbers are unchanged, 2.9 < 17.4.)

## 3. Round 2 (PREREG2.md): pinned poses, mask = `|MASK - OFF| > 6`

Pose pick from the standalone-only scan (`scan_table.txt`, 28 bases in `poses_scan.json`): the top two
by mask per track (`poses_r2.json`). Mask texture Sands / Warzone `BarrelRed`, Training `Brick`.
Full table: [`measure_r2.txt`](measure_r2.txt).

| pose | % frame | original L | OFF L | ANIM L | dOFF | dANIM | toward | original RGB | OFF RGB | ANIM RGB | MAE OFF / ANIM |
|---|---|---|---|---|---|---|---|---|---|---|---|
| training_p3 | 52.99 | 38.8 | 52.8 | 42.1 | 14.0 | **3.3** | yes | 32.5/41.7/40.4 | 46.5/54.6/59.8 | 37.6/43.7/45.6 | 14.9 / 5.8 |
| training_p4 | 51.09 | 38.8 | 54.1 | 42.3 | 15.3 | **3.6** | yes | 32.1/41.8/40.4 | 47.4/56.1/61.3 | 37.6/44.1/45.8 | 15.5 / 5.2 |
| sands_p11 | 61.87 | 72.9 | 172.0 | 76.8 | 99.1 | **3.9** | yes | 60.3/78.5/77.1 | 155.6/177.5/186.9 | 67.1/80.7/81.9 | 99.1 / 5.6 |
| sands_p8 | 61.07 | 72.8 | 171.9 | 76.8 | 99.1 | **3.9** | yes | 60.1/78.5/77.2 | 155.2/177.5/187.0 | 67.0/80.7/82.0 | 99.1 / 5.6 |
| warzone_p3 | 44.38 | 54.9 | 71.4 | 71.6 | 16.5 | 16.7 | **no** | 53.4/55.9/53.7 | 70.3/72.1/71.0 | 70.5/72.3/71.2 | 26.4 / 26.6 |
| warzone_p2 | 43.51 | 62.4 | 72.5 | 72.7 | 10.1 | 10.3 | **no** | 60.8/63.4/61.3 | 71.3/73.1/72.1 | 71.5/73.3/72.2 | 26.6 / 26.7 |

| rule | measured | verdict |
|---|---|---|
| R0 OFF vs OFF2, first pose per track | bit-identical on training_p3, sands_p11, warzone_p3 | PASS |
| R1 Training | dANIM < dOFF on both poses | TOWARD |
| R1 Sands | dANIM < dOFF on both poses | TOWARD |
| R1 Warzone | dANIM > dOFF on both poses (+0.2, +0.2) | AWAY |
| R2 ANIM != OFF outside W dilated 3 px | 0.000 / 0.000 / 0.000 / 0.000 / 0.052 / 0.105 % (limit 0.5) | PASS |
| Flip | needs all three TOWARD | **NO FLIP** |

Per-track luma over the water mask (original / knob OFF / knob ON):

| track | original | OFF | ON (`MASHED_TEXANIM=1`) |
|---|---|---|---|
| Forest (HB-FOREST-WATER, forest_s8) | 21.5 | 48.1 | 24.5 |
| Training (p3 / p4) | 38.8 / 38.8 | 52.8 / 54.1 | 42.1 / 42.3 |
| Sands (p11 / p8) | 72.9 / 72.8 | 172.0 / 171.9 | 76.8 / 76.8 |
| Warzone (p3 / p2) | 54.9 / 62.4 | 71.4 / 72.5 | 71.6 / 72.7 |

## 4. Gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on exe `9f2afe92`
(`A4_ROOT` = the private root, stock save), through test-slot and `mashed_lock.py`.

| gate | this exe (`9f2afe92`) | `merge_train.json` ref (orch/integration `96c032d0b`) | |
|---|---|---|---|
| G-CTRL (step / gates / ptrace) | `5943c5f1 / 364350ae / 52052890` | `5943c5f1 / 364350ae / 52052890` | identical |
| B (K0 / K1 / K26) | `3a03670c / e85ea6fb / 53778664` | `3a03670c / e85ea6fb / 53778664` | identical |

All four gate processes self-exited rc 0 (H_DEF 126 s, B 11-12 s). Across the step: 15 original
captures, each ended by `race_draw_burst.py` killing its own spawned PID after the dump
(`dev.kill(proc.pid)`, line 678). The 4 failed training `--pose` attempts were argparse exits that
never booted the game. 21 + 56 + 21 standalone renders and 4 gate runs all self-exited, none killed.

## 5. Decision for the user (new L-n row)

Flip `MASHED_TEXANIM` default ON although Warzone moves 0.2 luma away?

- (a) **Flip** (recommended). Forest 26.6 -> 2.9, Training 14.0 / 15.3 -> 3.3 / 3.6, Sands
  99.1 -> 3.9. On Warzone the knob only changes which of the 32 ring frames is on the river,
  because the shipped material is already `water00`. The 0.2 luma shift is inside the 1.5 luma
  spread among the ring textures, and the frame drawn depends on the ring phase, which the capture
  does not pin. The Warzone 10 to 16 luma residual is the same with the knob OFF and ON, so it is
  a separate defect.
- (b) Keep OFF until a Warzone run pins the ring phase on both sides (read the original's ring
  node at `DAT_00684c50` at capture time and start the port's ring on the same node), then rescore.
- (c) Keep OFF and file the Warzone residual first.

## 6. Follow-ups

- Warzone river residual: OFF and ON both 10 to 16 luma brighter than the original (MAE about 26)
  over the river mask. Not this knob. Unmeasured cause.
- The round-1 grid vantages are not water-dominant on any of the three tracks (Training s20 at
  0.57% is the largest). Use `poses_r2.json` + `--pose=` for any future water score.
- `race_draw_burst.py --pose` needs `--pose=<basis>` when the basis starts with `-`.

## 7. Raw data (not committed), SHA-256

Root `D:/MashedWT/HB-TEXANIM-REFS_data/`. Full list (53 files: 15 original captures, the round-1
s8 / s20 and every round-2 standalone capture, both exes, the run logs) in
`D:/MashedWT/HB-TEXANIM-REFS_data/SHA256SUMS.txt`. The scored references:

```
ef25fd96fced7ee533cc688effcc0384d9d3ad3aa363c76410ee2775ae91087a *orig/training_s20/orig.bmp
800b14a5ad8d08dce3ee6bb8a41b6a4e750c47d70c58df50d02a2cb850d085df *orig2/sands_p11/orig.bmp
529b1a5260fd82ef7eefeb38e27b11324e43988b2f524372698a11948e177181 *orig2/sands_p8/orig.bmp
9f8a64b8e0208cb13a9c056796acd8ee8b1e4f7a211bc1ec95e4ad40e2543b9f *orig2/training_p3/orig.bmp
34ccb812c3aa797e23ddcf878a9462e1624576a4cf6b031b992bebea9c7f88a5 *orig2/training_p4/orig.bmp
dbe3b7c1d97665ae99d671c607a8c867f9ed468c1406ef1a0f712044f13f6e53 *orig2/warzone_p2/orig.bmp
ea65c7abe658459501d4ff6d71100408342cf5de72d6e936624f28d54c853b34 *orig2/warzone_p3/orig.bmp
59c2a6c98e2b078d7c812a7e7fedae3864cdd4a45e8fd6b1af3c4c092c5c3193 *render2/sands_p11_ANIM/race1/01_grid.bmp
e6dc9d7c588298caeb073102d710c5a1c1673e9a37c742ed28183818ffd603cb *render2/sands_p11_OFF/race1/01_grid.bmp
0e456b0417cda8ddacc5e5f37fda611d4a0dc3eab0ad4a19bbdf63c97f056b02 *render2/sands_p8_ANIM/race1/01_grid.bmp
c82dfc99dce32a6a198a39dc122e8954bd160e70996856387e121c9bc0a89f1b *render2/sands_p8_OFF/race1/01_grid.bmp
26d91de88347669dc33eefbe10a3253fdb6773d69771ca0f998972c2396dd1ea *render2/training_p3_ANIM/race1/01_grid.bmp
5d5299edecd962c4d1992488787d6e54a32b4852ae5a66ba747e436403eb6ef7 *render2/training_p3_OFF/race1/01_grid.bmp
e49e79c9905280f701c56376a4be4ef086cbba6c4537f4349a0ea714e90bbc1d *render2/training_p4_ANIM/race1/01_grid.bmp
7919294255623f41a5b3d68f759daa1d3ce76872304feb4c2a63606db76db42b *render2/training_p4_OFF/race1/01_grid.bmp
f3e9b7ddd66bbd9df9a854c64a5992555bc76f1e408de6bfe8a00f0a13ca1b88 *render2/warzone_p2_ANIM/race1/01_grid.bmp
44329ba43e7774b17d3dd972c2562184475f7cc9670f4e15c09ca38d5a0fa15e *render2/warzone_p2_OFF/race1/01_grid.bmp
247a9c5bdc180e13af7792270b06839171cc4fbf39e769bcf2835419562bcbb9 *render2/warzone_p3_ANIM/race1/01_grid.bmp
02180a03469b0d54e7c61aef632bf52280cc3bacbbdf07116296d45720d95569 *render2/warzone_p3_OFF/race1/01_grid.bmp
acfa80318ab2641addca0576ba5f65728e4507846207e22befc2e29c8ec8e458 *exe_prereg/mashed_re.exe
9f2afe9246c59abb92b43118e30797cb2443d8600a8df2d5d84b7cb3043d919d *exe_mask/mashed_re.exe
```

---

# L-42 flip (2026-10-27): default ON, gates hold, 13-track rows move on the 4 ring tracks

**Verdict: DECISION.** Mariano decided L-42 = flip. `MASHED_TEXANIM` is now default ON, with
`MASHED_NO_TEXANIM=1` as the opt-out (`TexAnimOn`, `D3d9Render/TrackRenderer.cpp`). The branch first
merged `orch/integration` `2232ac2e5` (Arctic sky flip) as `f2a4db0ec`. Flipped exe `1a4c2e82bb3bf9b2...`.
G-CTRL and B are identical to the refs. The 13-track score moves on 8 rows, all on the four ring tracks
(Sands, Forest, Warzone, Training), all toward the original (more matched draws, fewer cited rows,
0 RED). The step said to stop on any 13-track move, so this is not GREEN. If accepted, the new
`run13_reference` is `verify/hbtexanimrefs_20261027/tracks_score.txt`.

## F1. Water on the flipped default build (no env var vs `MASHED_NO_TEXANIM=1`)

[`measure_flip.py`](measure_flip.py) -> [`measure_flip.txt`](measure_flip.txt). Same poses and
original references as sections 2-3. Mask: pose cases `|MASK - NO| > 6`, forest_s8 `|DEF - NO| > 6`
(HB-FOREST-WATER W2).

| track | case | % frame | original L | OFF (`MASHED_NO_TEXANIM=1`) | ON (default) | gap OFF | gap ON |
|---|---|---|---|---|---|---|---|
| Forest | forest_s8 | 5.17 | 21.6 | 48.4 | 24.5 | 26.9 | **2.9** |
| Training | training_p3 | 52.99 | 38.8 | 52.8 | 42.1 | 14.0 | **3.3** |
| Training | training_p4 | 51.09 | 38.8 | 54.1 | 42.3 | 15.3 | **3.6** |
| Sands | sands_p11 | 61.87 | 72.9 | 172.0 | 76.8 | 99.1 | **3.9** |
| Sands | sands_p8 | 61.07 | 72.8 | 171.9 | 76.8 | 99.1 | **3.9** |
| Warzone | warzone_p3 | 44.38 | 54.9 | 71.4 | 71.6 | 16.5 | 16.7 |
| Warzone | warzone_p2 | 43.51 | 62.4 | 72.5 | 72.7 | 10.1 | 10.3 |

The default build's frames are bit-identical to round 2's pre-flip `MASHED_TEXANIM=1` frames on all six
pose cases (0.000% pixels differ), so the merged integration code does not touch these frames.

## F2. Gates (flipped exe `1a4c2e82`, private root, test-slot + `mashed_lock.py`, merge queue empty)

| gate | this exe | `merge_train.json` ref (orch/integration `8f4e96366`) | |
|---|---|---|---|
| G-CTRL (step / gates / ptrace) | `5943c5f1 / 364350ae / 52052890` | `5943c5f1 / 364350ae / 52052890` | identical |
| B (K0 / K1 / K26) | `3a03670c / e85ea6fb / 53778664` | `3a03670c / e85ea6fb / 53778664` | identical |

## F3. 13-track (`run13.py run` + `score --cite verify/rres_20261016/cites_rres.json`)

New table: [`tracks_score.txt`](tracks_score.txt), `INSTRUMENT GATES: PASS`, 0 RED on every row. 31 of 39
rows are identical to `verify/hbarcticskyflip_20261027/tracks_score.txt`. The 8 that move:

| row | old matched / m.tri / m.tri% / rows cited | new matched / m.tri / m.tri% / rows cited | red |
|---|---|---|---|
| 05_Sands 1084 | 286 / 20354 / 88.8% / 21 | 303 / 20494 / 89.4% / 19 | 0 -> 0 |
| 05_Sands 1159 | 234 / 14152 / 80.6% / 22 | 251 / 14292 / 81.4% / 20 | 0 -> 0 |
| 09_Forest 820 | 157 / 17332 / 93.0% / 17 | 158 / 17916 / 96.2% / 15 | 0 -> 0 |
| 09_Forest 860 | 153 / 17302 / 93.0% / 17 | 154 / 17886 / 96.1% / 15 | 0 -> 0 |
| 09_Forest 895 | 92 / 9504 / 89.6% / 19 | 93 / 10088 / 95.1% / 17 | 0 -> 0 |
| 11_Warzone 624 | 326 / 23936 / 96.1% / 21 | 328 / 24036 / 96.5% / 19 | 0 -> 0 |
| 13_Training 884 | 215 / 17478 / 96.4% / 20 | 216 / 17598 / 97.1% / 18 | 0 -> 0 |
| 13_Training 924 | 211 / 17440 / 95.0% / 20 | 212 / 17560 / 95.7% / 18 | 0 -> 0 |

Unchanged on those tracks: Sands 1124, Warzone 584 / 659, Training 959. The draw-list diff keys on the
bound texture. With the ring on, the water draw binds a `waterNN` texture, and on a row it matches only
when the port's ring frame equals the original's at that frame (the ring phase is not pinned). That is
consistent with some rows moving and others not. This explanation is not separately measured.

Raw data: `D:/MashedWT/HB-TEXANIM-REFS_data/{render3,arms_flip,tracks_flip}`, hashes in
`D:/MashedWT/HB-TEXANIM-REFS_data/SHA256SUMS_flip.txt`.
