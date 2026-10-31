# RESULT D5-KNOBS-AI-TICK-FIRST (2026-10-31)

Verdict: **GREEN**. `MASHED_AI_TICK_FIRST` had no reader left, and it is deleted. The exe builds, rva-lint reports 0 NEW,
and G-CTRL and B are unchanged. (b) is 3/3, (e) is 3/3 with launch unchanged, and D2 DEFH is 3/3 in the 42 s band. The
13-track table is identical to the reference (39/39 rows, INSTRUMENT GATES PASS). No default flipped.

Step: delete the dead `MASHED_AI_TICK_FIRST` knob (ledger, roadmap loop run 27, section "D5 before ship"). Branch
`orch/D5-KNOBS-AI-TICK-FIRST` from `orch/integration` @ `702354fc5`, change commit `4a913b367`, refreshed onto
`orch/integration` @ `f6d48c932` by merges `74ec3b5f4` (onto `363ebc434`) and `2dc32e97e` (onto `f6d48c932`, after
D5-KNOBS-E2-PHYS-A6FIX merged).

## 1. Audit

`S` = `mashedmod/src/mashed_re/`. Before this step, `git grep -E "AI_TICK_FIRST|TickFirst|tick_first"` over `mashedmod/`,
`scripts/`, `re/`, `.claude/`, `ROADMAP.md` and `CLAUDE.md` found:

- `S/Core/FaithDefaults.cpp:34`, the `kFaith` seed `{ "AI_TICK_FIRST", "1" }`. That TU only seeds the environment, so the
  entry is not a reader.
- Comments only: `S/D3d9Render/TrackRenderer.cpp:5418`, `:5464`, `:5559`, `:5627`, `:5993` and
  `S/Vehicle/VehiclePhysicsRun.cpp:825`.
- No `getenv` / `GetEnvironmentVariable` of the name and no helper that builds it, in either target.
- `re/analysis/CHANGELOG.md:412`, `:415` (history, left as written).

The read site went with D5-KNOBS-E1-PHYS-A4A5. That step deleted `MASHED_A4_ORDER` and made the AI tick phase
unconditional (`TrackRenderer.cpp:5627`, `if (g_aib.loaded && (Ai::I32(Ai::kSplineRaceCnt) > 3)) ai_tick_phase();`),
and `MASHED_AI_TICK_FIRST` was the other side of that OR. `Race::PathProg_FrameTick()` on countdown frames is
unconditional too (`TrackRenderer.cpp:5470`).

Live setter: `verify/savelink_20261023/run_arms.py`, the gate runner, set the name in report-only stacks `S` (via `_S`),
`T`, `CCET`, `TE`, `TPPT`, `TCS` and `KT`. `MASHED_NO_AI_TICK_FIRST`: 0 hits. The gate stacks `DEF` / `DEFH` never set it,
so every gate arm already ran with it unset, and the exe ignored it anyway.

## 2. Change (commit `4a913b367`, 4 files, 25 insertions, 19 deletions)

- `Core/FaithDefaults.cpp`: the `AI_TICK_FIRST` entry removed from `kFaith`, plus a note naming this RESULT.
- `D3d9Render/TrackRenderer.cpp`: the five comments that described the knob as live (one said "default OFF") now say
  the phase runs unconditionally and the knob is deleted. No code changed.
- `Vehicle/VehiclePhysicsRun.cpp:825`: comment only.
- `verify/savelink_20261023/run_arms.py`: the name is removed from every stack, and a header comment names this RESULT.
  `T` is empty (= DEF), `SmT` = `S`, `CCET` = `CCE`, `TE` = `E`, `TCS` = `CS`, `KT` = `TGT`, and `TPPT` keeps `PATHPROG` and
  its traces. No tag is removed, so every derived stack still resolves. `ast.parse` OK. Afterwards the grep returns 0 live
  setters (the 2 remaining hits are history comments at `:66` and `:227`).

Not touched: closed copies of `run_arms.py` under other `verify/*` folders (frozen evidence), `CHANGELOG.md`, and trackers
(no RVA changes C-level).

## 3. Gates

Build: `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"` = `=== Build OK ===` (shared `.asi` not deployed).
rva-lint: `scripts/lint_rva_bodies.py` rc 0, `16 finding(s): CROSS-TARGET=16 | allowlisted=16 NEW=0`.

Arms: `verify/savelink_20261023/run_arms.py` (this branch's copy), private root `D:/MashedWT/D5-KNOBS-AI-TICK-FIRST_root`,
under `mashed_lock.py run` (D2: `run --solo`) and `test-slot`. 13 tracks: `verify/rsky_20261016/run13.py run`, root
`D:/MashedWT/D5-KNOBS-AI-TICK-FIRST_root13`, under `run --solo`, scored with `--cite verify/rres_20261016/cites_rres.json`.
Refs from `re/orchestrator/merge_train.json`.

Pass 1, change commit `4a913b367` (exe sha256 `fce42eaf`):

| Gate | This exe | Ref | |
|---|---|---|---|
| G-CTRL H_DEF step / gates / ptrace | `8284be46` / `ba407a16` / `31fe043e` | `8284be46` / `ba407a16` / `31fe043e` | same |
| B_DEF_K0 / K1 / K26 step | `1d5bf4f9` / `1d5bf4f9` / `1d5bf4f9` | `1d5bf4f9` x3 | same |
| (b) `vsum.py` on the 3 B arms | 3/3 arms 3/3 | 3/3 | same |
| (e) `ai_speed_env.py --check B_DEF_K0` | 3 `(e) PASS`, launch 1425.7 / 2052.5 / 2055.0 | 3, launch 1425.7 / 2052.5 / 2055.0 | same |
| D2 DEFH_1..3, `verify/d2band42_20261029/score_d2_v2.py port` | drvmed 1919.62 / 1917.10 / 1917.10, slip1500 0.1934 / 0.1933 / 0.1930, all `in`, H1 398/400; motion_diag 1637 / 1639 / 1640 lines | 3/3 in band | same (3/3) |
| 13 tracks, `run13.py score` stdout | 39 `^\d\d_` rows identical to `verify/run13p3_20261029/tracks_score_sub_p3.txt`, INSTRUMENT GATES: PASS, stdout sha256 `2431afb1` | `2431afb1` | same |

D2 re-runs: the first two D2 passes came back short (1209 / 706 / 1407, then 1524 / 714 / 1553 lines, min 1600). Each arm
ran its full 110 s and exited 0, and the log tails are normal driving rows. These are wall-clock short runs (the machine
also had a SimGolf `--jobs 3` suite and two other Mashed workers in test slots), not REDs. The third pass (rerun 2 of the
allowed 2) gave full-length logs, scored above. DEFH_3's motion_diag is byte-identical to D5-KNOBS-D-SEEDED6's
(`dea61fb2`).

Pass 2 runs on the refreshed branch. See section 3b.

## 3b. Post-merge re-confirmation

`orch/integration` moved twice during the pass. First came `514fb0a60` (`mashedmod/build.bat` drops dsound.lib from the
exe link line), merged as `74ec3b5f4`. Then D5-KNOBS-E2-PHYS-A6FIX landed (`62b3f2515`, 10 kFaith-seeded A6a / fixup /
terrain knobs deleted, `Core/FaithDefaults.cpp` and `Vehicle/`), merged as `2dc32e97e`. The merge had one conflict, in the
`kFaith` block of `Core/FaithDefaults.cpp`. Both sides were deletions, and the resolution keeps both:
`{ "REASSERT_KEEP_NORMAL", "1" },` then `{ "RW_SSE_POINTS", "1" },`, with no `AI_TICK_FIRST`, `FIX_5CEA60` or
`UP_TRINORMAL`. `verify/savelink_20261023/run_arms.py` auto-merged: `ast.parse` OK, 0 live setters of the name.

The final exe was rebuilt at `2dc32e97e` (`=== Build OK ===`, exe sha256 `a4ff2143`, .asi `d396a072` not deployed), and
rva-lint gives the same 16 CROSS-TARGET, all allowlisted, 0 NEW.

| Gate (exe `a4ff2143`) | This exe | Ref | |
|---|---|---|---|
| G-CTRL H_DEF step / gates / ptrace | `8284be46` / `ba407a16` / `31fe043e` | `8284be46` / `ba407a16` / `31fe043e` | same |
| B_DEF_K0 / K1 / K26 step | `1d5bf4f9` / `1d5bf4f9` / `1d5bf4f9` | `1d5bf4f9` x3 | same |
| (b) `vsum.py` | 3/3 arms 3/3 | 3/3 | same |
| (e) `ai_speed_env.py --check B_DEF_K0` | 3 `(e) PASS`, launch 1425.7 / 2052.5 / 2055.0 | same | same |

D2 and the 13 tracks were not re-run on `a4ff2143`. They passed on pass 1. The two merged changes are E2's own step,
which passed its full gate set in its merge train, and a link-line edit. The merge train runs the full set on this branch
anyway (`Vehicle/` is in `full_set_paths`).

Committed copies: `vsum_b_postmerge.txt`, `e_check_postmerge.txt`. Raw (sha256): `arms_postmerge/` step / gates / ptrace
files (the hashes in the table), `arms_postmerge.log` 10c04f258034efd76e91f126a351cfbb901db2d942794c6ba482a80383803ec1,
`build_postmerge.log` 8a4870f01fc4ec12a48aeada1ad1ff7f0ef55182903491b6539c96b537f771de, `lint_postmerge.txt`
e9dd7a2547772d2722eb778f62fb83ad83e8923d9a9fe732eaf3f772c2aa42c2.

One stopped attempt: the first post-merge arm request (exe `97c5d148`) was still queued for the lock when its shell hit the
tool time limit. That chain was stopped (my PIDs only), and the request was cancelled again once E2 merged, before any arm
ran. No arm output exists from it.

## 4. Raw files (not committed)

Folder `D:/MashedWT/D5-KNOBS-AI-TICK-FIRST_data/` (sha256):

- `arms/H_DEF.step.csv` 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198
- `arms/H_DEF.gates.csv` ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb
- `arms/H_DEF.ptrace.log` 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3
- `arms/B_DEF_K0.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/B_DEF_K1.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/B_DEF_K26.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/D2_DEFH_1.motion_diag.log` 8449ab3ba707ad4e29a68fb806c862fbfb389c0d4a4aea72b5d30126a161c1fd
- `arms/D2_DEFH_2.motion_diag.log` e1029fa7ee9a07e8036f2762b9b24e69bc31635c636fe6aef85cceac268e907a
- `arms/D2_DEFH_3.motion_diag.log` dea61fb24a373c2cb7500257d669a628bf9f29f123cfeb0296f6c92a1398d1e9
- `score_d2.txt` 4172ac8a46e8adac82fcfcb2975d737d760dcc358dbac607c17375bf4dafa68a
- `vsum_b.txt` 6b0c8feada572b7600d357e6cc3ee1302fa5bb25d59b34fbb6d4f01a9ee2956a
- `e_check.txt` f35e06dc8d79edca07ec3b6d45a908f64e8ca0e023f32a734fa2f943faeca0b2
- `score13.log` (= committed `tracks_score.txt`) 2431afb1da35f95db9bb01535f071da5400d53fb2cc2550210465c0404be27c3
- `t13/scores_diff.txt` 52c658bc05ee708b09851413bd258c09e0bd813d4d619187621c072b2e22946c
- `t13/gates_diff.txt` c35d0262ae508004c14c3ed1979e5ece7da2fc8ed84dd5bff637f04a33f1a536
- `build.log` 09940bb90288bd470f792214d9ffc93126b0863547de674def237cf2b439157e
- `lint.txt` e9dd7a2547772d2722eb778f62fb83ad83e8923d9a9fe732eaf3f772c2aa42c2
- `arms_run.log` ea4d9bb8c0df8830360407fe12756502ba685f137e8745e0a2be3d8c39f2d85a
- `d2_run.log` b4a8f814f3ed704fc273dc7ae8f78761f3b84b744c22c08a81186222e554f279 (pass 1, short)
- `d2_rerun1.log` e5813f210b6803c714354dc1832295393989937e4506a87e2d3f7a5d18d579f2 (short)
- `d2_rerun2.log` aeb2f38bc7b9bde5ac562377225e86d25fac9cc489a32ab107a870775752864b (scored)
- `run13.log` 330adc69e0e1b046989e2158ce3b4c69f62f73ca71e16d0d2b971727663419c4
- exe pass 1 (`mashed_re_premerge.exe`) fce42eaf52d88f734dda48709e00d7f6ad11bd7a01978f97e9a079fe9307864e
- .asi pass 1 `mashed_re_dev.asi` 55e87d66f69374dec006824d6300d8d03e2d427aff4db68c337f421bf8188c47 (built, not deployed)

Committed copies in this folder: `vsum_b.txt`, `e_check.txt`, `score_d2.txt`, `tracks_score.txt` (the scorer's stdout).

## 5. For the lead

- No `.ps1` / `.bat` edit needed. The closed `verify/*` runners that set `MASHED_AI_TICK_FIRST` are frozen evidence and
  were left unedited, as in SEEDED6.
- `MASHED_NO_FAITH` no longer has an AI_TICK_FIRST effect. It had none before either, since nothing read the name.
- The knob inventory row for `MASHED_AI_TICK_FIRST` can be marked deleted by whoever owns the inventory text.
