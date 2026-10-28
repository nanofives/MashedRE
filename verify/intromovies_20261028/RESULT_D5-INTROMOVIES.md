# RESULT D5-INTROMOVIES (D-11062, L-45): boot movies through DirectShow

Branch `orch/D5-INTROMOVIES`. Code `9c07e7b08`, PREREG `36a6fa990` (committed alone, before
the arms). Exe `mashed_re.exe` sha256 `265732d019c9cbc9...`, built from the worktree with
`MASHED_NO_DEPLOY=1`. Every arm ran on that exe.

## Verdict

**RED on one PREREG row, every other row PASS.** The default gates are unchanged
(G-CTRL `5943c5f1/364350ae/52052890`, B `3a03670c/9fcdbd3a/53778664`). Knob OFF runs no intro
code (I0). The skip path (I2), the near-empty skip files (I3), the sequence, small.mpg and the
handoff to the frontend all pass. I1 missed its time band on movie 1 only: empire took 34.7 s for
a 7.78 s file, while movies 2-4 in the same run were inside their bands. The cause is not
measured. The two re-runs never reached the game in run 18 (see I1 below). **Run 19** ran
them with machine load sampled: I1b PASS at 35% CPU, I1c FAIL on movie 1 at 97-100% CPU (see
"Run 19 re-runs" below).

## Run 19 re-runs with machine load measured (2026-10-28, appended)

**Verdict stays RED on PREREG row I1** (the PREREG says report, do not re-score). The two re-runs
split: I1b passes every I1 check, I1c misses movie 1 again. The load sampler shows the machine
at 35% CPU through I1b's movie 1 and at 97-100% through I1c's movie 1. Movies 2-4 are in band in
all three I1 runs.

Setup: same exe (`265732d019c9cbc9`), private root, shared `original\` only read. The job ran in
the machine lock (`mashed_lock.py run --label D5-INTROMOVIES`, queued behind
HB-WARZONE-RIVER-CAP, held from 15:29:16 local) inside a test slot (`proyectos_test_slot.py`).
5 of 6 slots were free at start, and no other MASHED.exe or mashed_re.exe ran during either arm
(`other_games_max` 0 in every phase). Log: `D:/MashedWT/D5-INTROMOVIES_data/run_i1_r19.txt`.

`run_intro.py` gained `sample_load()`, which replaces the plain `sleep(wall)`. About every 1.4 s
(the 1 s sleep plus the process walk) it records system CPU %, the busiest core, the game's own
CPU, everyone else's CPU (in cores x100, so 1200 = all 12 cores), the count of other game
processes and the top 3 other processes. It also tags each sample with the movie playing, taken
from the count of `INTRO movie` lines already in the log. The game's priority class reads
`16384` = `BELOW_NORMAL_PRIORITY_CLASS`, which it inherits from the test slot.

| arm | movie 1 ms (band 6613..10503) | loop frames | ms/frame | movie 1 load: sys mean / p95 | others (cores x100) | samples in movie 1 | row |
|---|---|---|---|---|---|---|---|
| I1 (run 18) | 34672 | 223 | 155 | not measured | not measured | - | FAIL |
| I1b | 7984 | 206 | 38.8 | 34.6% / 47.5% | 400.6 | 5 | **PASS (all I1 checks)** |
| I1c | 33547 | 167 | 200.9 | 98.3% / 96.9-99.8% | 1175.7 | 2 | FAIL (idx1 only) |

Movies 2-4: I1b 9625 / 8125 / 63875 ms, I1c 9703 / 8141 / 63937 ms, all in band. Both arms:
small.mpg BUILT not run, `F1 frontend.mpg: RUNNING` after the INTRO lines, 4 shots with teal
fraction 0.0. During movies 2-4 the machine sat at 24.7-43.4% mean system CPU.

**What the I1c load data shows.** The sampler's first row is at t=31.1 s, so the sampler (a
BelowNormal Python process in the same slot) got no CPU for the first 31 s of the arm. Over that
window the machine averaged 96.9% CPU, and the next window (to t=34.2 s) 99.8%, with the busiest
core at 99.4-100%. The game itself used 0.02-0.07 of a core. The heaviest long-lived other process was
`msmpeng.exe` (Defender) at 43% then 99%. The top 3 named processes add up to 0.7-1.7 cores,
which leaves about 10 cores' worth of load in processes not in the top 3 or not alive across two
samples (short-lived processes such as a parallel build fit that, but nothing identified them).
The load fell to 27% by t=38.8 s, during movie 2. I1b's movie 1 window averaged 34.6% with no
core above 74%.

So movie 1 is in band when the machine is at about 35% and takes 4.3x the file length when the
machine is saturated and the game runs at BelowNormal. The run-18 I1 miss has no load record, so that one
link is still not measured. Its loop rate (155 ms/frame) sits with I1c (201) and not with I1b
(38.8). Why the saturation landed on movie 1 in 2 of 3 runs (the first ~35 s after launch) is not
measured. The game's own CPU in that window is too small to cause it.

Movie 1 is the only one whose loop starts while the process is still loading. No per-iteration
split (Update / Present / input poll) was taken, because I1b shows the loop at 38.8 ms/frame on a
35%-loaded machine, the same rate as movies 2-4.

Raw data (not committed), `D:/MashedWT/D5-INTROMOVIES_data/`:

| file | sha256 |
|---|---|
| run_i1_r19.txt | 8f6581fc9f26c161f5fa6af436bcfddd1906e74e2cd65bcd8e0fad998ea35790 |
| arms/I1b.log | 93a22eb55aa7259950f45ffa27204d35727ddf459a55753fa6c9d9a51f4859cf |
| arms/I1c.log | 5145ba19f35c85e43b3dcf4651901e1d13434fc2121ca69ac4c2b40e472519e6 |
| arms/I1b_load.csv | e71b9651ef52e8fd3b5820fdeb5ecb9d53b409ff8907dc9b82fb3da359896817 |
| arms/I1c_load.csv | 01ba01cf0b7e28099a18325fb5f8c9f74f82819cd6b9a1474f8aafd3a7eff9f4 |
| arms/I1b_shot_1..4.bmp | 3e4ab829.. / 10ff2e75.. / cce38c28.. / d78abee8.. |
| arms/I1c_shot_1..4.bmp | 962ccea3.. / 3d7c03d4.. / ab98a289.. / d78abee8.. |
| arms/intro_score.txt (6 rows) | 7b814e0f76a391a58a4b5c31380644ef718a5113c01367b9c130ec1bd0a545c2 |

The I1b and I1c rows are appended to the committed `intro_score.txt` (each carries a `load`
summary per movie phase). The shared `original\toastart\pc\movies\*.mpg` were re-hashed at
2026-10-28T18:35:18Z: the five skip files are still `a0e085d46d222763` (167,414 B) and frontend
is still `6d12b398284bd3d6`. `--restore` was not run, so no announcement was needed.

**For the lead (decision):** (a) accept I1 on I1b plus the load record, treating the movie-1 miss
as CPU starvation of a BelowNormal process on a saturated machine, or (b) PREREG one more I1 arm
that only counts when the movie-1 load window stays under a stated CPU ceiling. Recommendation:
(a). I1b is a clean pass on the same exe, and the one failing arm with a load record had every
core saturated. Either way the knob stays default OFF. Turning it ON and striking D-11062 remain
a decision row.

## What was ported (knob `MASHED_INTRO_MOVIES=1`, default OFF)

`RunBootIntroMovies()` in `exe_main.cpp` runs after `InitDirectInput` and before the menu music
starts. Source comment carries every cite. Facts read this session:

- **Sequence.** `FUN_00495350` sets `local_18 = {1,2,3,4}` and plays each index through
  `FUN_00494a80(0, idx, 0)` -> `FUN_004944c0(idx)`, which `lstrcpyA`s `[0x006147c4 + idx*4]`.
  The table, read from `MASHED.exe.unpatched`: `0x006147c4` frontend, `0x006147c8` empire,
  `0x006147cc` supersonic, `0x006147d0` renderware, `0x006147d4` intro. So the order is
  **empire, supersonic, renderware, intro**.
- **End of movie.** `FUN_004942b0` polls `GetEvent` (vtbl +0x20) on `DAT_00771a28`. On code 1
  (`EC_COMPLETE`) with `DAT_00771a08 == 0` it clears `DAT_00771a04`, and the loop closes the
  movie (`FUN_00494460`) and opens the next one.
- **Skip.** For 4 slots, held byte `0x007f1042 + k*0x4c` non-zero and previous-frame byte
  `0x007f1502 + k*0x4c` zero (bound `0x007f1632`), meaning a rising edge of the cooked action-4 byte
  (keyboard 0x1c, pad button 0). Ported with the standalone's existing cook inputs
  (`g_keys`, `s_joy_btn`, `g_input_slot_type`).
- **Draw.** `FUN_004c1bb0` clears the camera image to RwRGBA `{0x00,0x80,0x80,0xff}`, and
  `FUN_00493fd0` draws one Im2D quad over the whole camera raster in default white. Disassembly
  `0x0049545a push eax / 0x0049545b push ecx / call 0x00493fc0 / ... / call 0x0049546f`: the two
  ratio floats pushed for `FUN_00493fc0` are never popped and become `FUN_00493fd0`'s
  `param_6` / `param_7`. Those are the quad's max U and max V, `DAT_007719ec/DAT_007719f4` and
  `DAT_007719f0/DAT_007719f8`. `FUN_00494820` writes video width/height to `0x007719ec/f0` and
  texture width/height to `0x007719f4/f8`, padded to powers of two only when
  `D3DCAPS9.TextureCaps & D3DPTEXTURECAPS_POW2` (`0x00494898`). The port does the same (`pow2`
  option). This machine reports no POW2 cap, so the extent is 1.0. The loop ends each frame with
  `Sleep(5)`.
- **small.mpg.** `FUN_00494c80` builds the graph and never runs it. The only code references to
  its `IMediaControl` `DAT_00771a48` are `0x00494e57` (store), teardown `FUN_00494bc0`, and the
  CRT static init/exit thunks `0x005ca780` / `0x005ca890`. Ported as build-only, released at
  exit. **This corrects the task brief's "small.mpg attract loop":** no code plays small.mpg.
- **Sound.** The original renders the source pin (`IGraphBuilder::Render`, vtbl +0x30), which also
  builds the audio branch. The port wires video by hand and `Render()`s the remaining splitter
  output pins (`MpegOpenOptions.audio`).

Port choices, stated in the code: the standalone's -0.5 pixel-centre convention, and linear
filtering as on the frontend backdrop (the RW filter state at `0x00495350` entry is not read). A
movie that fails to open is logged and skipped. In the original that case shows a message box
and waits for the skip press, since the running flag never clears.

`MpegVideoTexture` gains `MpegOpenOptions {loop, audio, run, pow2}` and `Ended()`. The
frontend.mpg backdrop uses the defaults, so its path does not change. The event interface is
only queried when `loop == false`.

## Arms (`verify/intromovies_20261028/run_intro.py`, scored table `intro_score.txt`)

| arm | env | result | detail |
|---|---|---|---|
| G | knob OFF, `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` | **PASS** | G-CTRL `5943c5f1/364350ae/52052890`, B `3a03670c/9fcdbd3a/53778664`, equal to `merge_train.json` refs |
| I0 | knob OFF, 25 s | **PASS** | no INTRO line, `F1 frontend.mpg: RUNNING` |
| I1 | knob ON, real movies, shots at frame 90 | **FAIL (1 row)** | see below |
| I2 | knob ON, `MASHED_INTRO_PRESS=30` | **PASS** | 4 x `skip-test frames=30` (1172/1172/1140/1344 ms), small BUILT, frontend RUNNING after |
| I3 | knob ON, 167,414-byte skip files | **PASS** | 4 x `complete` (157/78/110/125 ms, 2-3 frames), small BUILT, frontend RUNNING after |

I1 per movie (band = [0.85, 1.35] x file duration):

| idx | movie | video | loop frames | ms | band | row |
|---|---|---|---|---|---|---|
| 1 | empire (233 pictures @ 30 = 7.78 s) | 720x480 | 223 | 34672 | 6613..10503 | **FAIL** |
| 2 | supersonic (9.48 s) | 720x576 | 252 | 9641 | 8058..12798 | pass |
| 3 | renderware (241 @ 29.97 = 8.04 s) | 640x480 | 217 | 8125 | 6834..10854 | pass |
| 4 | intro (63.73 s) | 640x480 | 1720 | 63703 | 54170..86035 | pass |

small.mpg BUILT (160x128, not run). `F1 frontend.mpg: RUNNING` is logged after the INTRO lines.
All 4 shots pass (teal fraction 0.0, per-channel sd 18.5..92.0). A 2x2 contact sheet viewed by
hand shows all four upright, with the RenderWare legal text readable.

**The I1 row 1 failure.** Movie 1 played to `EC_COMPLETE`, but the run took 34.7 s for a 7.78 s
file. The loop ran 223 iterations at about 155 ms each. Movies 2-4 in the same process ran at
about 37-38 ms per iteration, and the same movie in I2 ran 30 iterations in 1172 ms (about 39 ms).
Shot timestamps: shot 1 (frame 90 of movie 1) at 11:08:11, shot 2 (frame 90 of movie 2) at
11:08:37. The process started after 11:07:47, so frames 1-90 of movie 1 took about 19 s.

During the run, all 4 test slots were held by other steps (D5-ICONCUBE-SPIN,
D3-CF-SNAP-RETIRE x2), and test runs sit at BelowNormal priority. Starvation of the decoder and
render threads would produce exactly this pattern, but nothing measured it, so it stays a
hypothesis. Per the PREREG the row is reported, not re-scored. Two extra I1 runs (I1b, I1c, not
PREREG rows) were run on the same exe:

**Not run.** The job (`run_intro.py I1b I1c`, `D:/MashedWT/D5-INTROMOVIES_data/run_i1.txt`)
waited 66 min for a test slot (acquired 12:32 local), then queued on the machine lock behind
merge-train solo/priority windows (`merge-train13:20261028_114545`, `merge-train:20261028_123923`)
and D4-WSB-RAND-FLIP. It hit the 2 h background limit at 13:22 local with no `mashed_re.exe`
launched. I1b/I1c have no rows in `intro_score.txt`.

To close the row: re-run `py -3.12 verify/intromovies_20261028/run_intro.py I1b I1c` inside the
lock on an idle machine. If movie 1 is still out of band, the next step is to log per-iteration
time split by Update / Present / input poll.

## Not covered

- **Movie audio:** `audio_hr = 0x80040256` on empire, supersonic and intro, and
  `0x00000001` (S_FALSE: no unconnected pin) on renderware, which is a video-only elementary
  stream. This machine has no audio endpoint, the same cause that keeps the original from booting
  now. The audio branch is built but untested here.
- **No original-side capture** was made or needed. The comparison is static (decomp +
  disassembly above) plus the file facts. No tracker row changed. No hooks.csv confidence moved,
  because the standalone loop is not an installed hook.
- The default stays OFF. Turning it ON, and striking D-11062, is a decision row for the lead.

## Movie files and original\

`patch_mashed_skip_intro.py --restore` was **not** run, so no multi-session announcement was
needed and nothing was re-applied. The real movies came from the private root's copy of the
skip_intro backup (`original\toastart\pc\movies\backup\`), copied into
`D:\MashedWT\D5-INTROMOVIES_root\original\toastart\pc\movies\`. The shared `original\` was only
read.

Shared `original\toastart\pc\movies\*.mpg` sha256 (prefix 16):

| file | before (2026-10-28T13:16:59Z) | after (2026-10-28T16:22:59Z) |
|---|---|---|
| empire / supersonic / renderware / intro / small | `a0e085d46d222763` each, 167,414 bytes | `a0e085d46d222763` each, 167,414 bytes (unchanged) |
| frontend | `6d12b398284bd3d6` | `6d12b398284bd3d6` (unchanged) |

Real files used (private root): empire `F89F7D92D51A4184` 4,751,364 B, supersonic
`33A6DE9296B65F87` 6,445,060 B, renderware `177DDB4D65223B09` 4,463,728 B, intro
`DBB08D35C970109C` 37,072,900 B, small `E709EF15FDED167C` 24,580 B.

## Raw data (not committed), `D:/MashedWT/D5-INTROMOVIES_data/`

| file | sha256 |
|---|---|
| arms/I0.log | f09371b5026bde44747e758e50ea2e29ef869a52716b5ad839b4ab2bf7db3e4c |
| arms/I1.log | 227204e0cee30ea4676ee2d9042b725e6004e26225dc3e2c01d6273848ace306 |
| arms/I2.log | 050d7a0ff60e28d470f2dff50d977eb10c2517036462ad95585370063aadfb40 |
| arms/I3.log | 71e2cd5adaf90e11bdf4cad6de213426b6034c70de7a9ea23b2242dd461b2de7 |
| arms/I1_shot_1.bmp | baf7272a94b2ba8ca9e2aa14febffd15f68df03d91e0668b693776df01066c7c |
| arms/I1_shot_2.bmp | 63a8936865e687a0a88e5028893fbada7b3ef78e3597df5297e1689bd694c33f |
| arms/I1_shot_3.bmp | 57a25d83cf3176f1d5cd5e46cc4477b64cd54f0c1d43d7de79d27a6cb9e7acbe |
| arms/I1_shot_4.bmp | 1ed4ab5b2e0b2ba037b3e613eabc5f1862390fd7c12214a914b1b5d0ab6df357 |
| gates/H_DEF.step.csv | 5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 |
| gates/H_DEF.gates.csv | 364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4 |
| gates/H_DEF.ptrace.log | 520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757 |
| gates/B_DEF_K0.step.csv | 3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f |
| gates/B_DEF_K1.step.csv | 9fcdbd3a6901025544e377132c575910aa5cd001faba802c1e7e8a9ac0d2886f |
| gates/B_DEF_K26.step.csv | 5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d |
| arms/intro_score.txt | 28a402e085f523db323ff8464331fec6763247a42d7337ce26b6a3a95908c27a |

The scored table (one JSON line per arm) is committed next to this file as `intro_score.txt`.
