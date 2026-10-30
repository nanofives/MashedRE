# RESULT D5-KNOBS-B2-TRACKRENDER (2026-10-30)

Verdict: **GREEN**. 10 of the 13 named knobs deleted (track render / sky / props), exe builds, rva-lint 0 NEW,
G-CTRL and B unchanged, 13-track table byte-identical to the reference. 3 knobs left out (section 1): the gate runner still sets them.

Step: second batch of class (b) knob deletions from `verify/d5knobsinv_20261030/RESULT_D5-KNOBS-INVENTORY.md`.
Branch `orch/D5-KNOBS-B2-TRACKRENDER` from `orch/integration` @ `145895e24`. No default flipped: every deleted read
was an opt-out of a path that runs when the variable is unset, so the default build executes the same code before
and after.

## 1. Audit, one line per knob (read site at `145895e24`)

Every deleted read is `std::getenv("MASHED_NO_<x>") != nullptr` (any value opts out, unset = ON), except
`MASHED_NO_TEXANIM`, which opted out only on the exact value `"1"` (`TexAnimOn`, `TrackRenderer.cpp:1600-1606`,
unset = ON). `Core/FaithDefaults.cpp` seeds none of the 13 names (grep for every name stem: 0 hits), and no
source file sets any of them (`grep -rn` over `mashedmod/src`: read sites and comments only). `TR` =
`mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp`.
Script check = `grep -rnE "MASHED_NO_<name>\b"` over `scripts/`, `re/tools/`, `re/frida/`, `verify/`,
`re/orchestrator/` (`.py .ps1 .sh .bat .js .json`).

| Knob | Read site(s) (inv. n) | Default | Set by a live script | In batch |
|---|---|---|---|---|
| MASHED_NO_PONTOON_HIDE | `TR:2640` (n 1) | ON | none (prose only: `verify/pontoon_20261016/apply_changelog.py:17`) | yes |
| MASHED_NO_SEA_TILE | `TR:2909` (n 1) | ON | none (print text only: closed `verify/sea_fix_20260930/check_rules.py:195`) | yes |
| MASHED_NO_SKYFIX | `TR:2595` load (bake_frames arg), `TR:9882` draw (tests at 9893, 9909) (n 2) | ON | only the closed rprops control `verify/rprops_20261016/rnoop.py` (R-NOOP, 2026-10-16) | yes |
| MASHED_NO_SKYSLOTS | `TR:1230` (n 1) | ON | only closed rsky / rskyverify runners (`verify/rsky_20261016/run_final{,2}.ps1:14`, `verify/rskyverify_20261016/run_verify{,2}.ps1`) | yes |
| MASHED_NO_UVSCROLL | `TR:9454`, consumers `TR:9859` (librw anim time) and `TR:9924` (D3D9 sky scroll) (n 1) | ON | none | yes |
| MASHED_NO_TEXANIM | `TR:1602` (n 1) | ON (L-42) | only the closed L-42 runners `verify/hbtexanimrefs_20261027/run_render.py:28` (`NO` arm) / `measure_flip.py` | yes |
| MASHED_NO_CLUMPANIM | `TR:3210`, log text `TR:3232` (n 2) | ON | only closed `verify/rskyverify_20261016/run_verify{,2}.ps1` (prose: `verify/rhooks_20261016/apply_trackers.py:109`) | yes |
| MASHED_NO_TXDSCOPE | `TR:1827` + log `TR:1861` (track), `TR:4053` (vehicle), `LibRw/RwRaceFx.cpp:359` (SFX) (n 4) | ON | only the closed txdscope control `verify/txdscope_20261016/run_txdscope.ps1:18` | yes |
| MASHED_NO_OBJBSP | `TR:2839` (Forest tree clumps), `TR:3540` (object world load) (n 2) | ON | only closed rsky / rskyverify runners (as SKYSLOTS) | yes |
| MASHED_NO_HOOKSLOTS | `Render/TrackHookSlots.cpp:138` (n 1) | ON | only closed `verify/rskyverify_20261016/run_verify{,2}.ps1` | yes |
| MASHED_NO_PONTOON_ANIM | `Track/WarzonePontoon.cpp:711` (n 1) | ON | **yes**: `verify/savelink_20261023/run_arms.py:19` `OPTOUT` (= `STACK['OPT']`) and `:23` `STACK['NOP']` (the gate runner), plus 90 copies of that runner under `verify/` | **no** |
| MASHED_NO_PONTOON_STATE | `Track/WarzonePontoon.cpp:712` (n 1) | ON | **yes**: `verify/savelink_20261023/run_arms.py:19` `OPTOUT` / `STACK['OPT']` | **no** |
| MASHED_NO_PONTOON_CARRY | `Track/WarzonePontoon.cpp:713` (n 1) | ON | **yes**: `verify/savelink_20261023/run_arms.py:19` `OPTOUT` / `STACK['OPT']` and `:23` `STACK['NOPC']` | **no** |

The closed runners named above are frozen evidence of lanes closed 2026-10-16 .. 2026-10-27. Nothing calls them and
no gate reads their output. Re-running one on this branch would make its opt-out arm identical to its default arm.
Left unedited (same treatment as B1's `verify/carlod_20261016/run_all.py`).

## 2. Change

4 files, 40 insertions, 78 deletions (commit `2323ac34d`).

- `TR` `parse_slot_name_cmd`: the `old_form` parameter (fed only by NO_SKYSLOTS; `Object_Bsp_Filename` always passed
  `false`) and its `has_idx` local are gone. Both callers lose the argument.
- `TR` `TexAnimOn()` deleted; the kTexAnim loop at the load tail runs unconditionally.
- `TR` TXDSCOPE: track load always narrows to the `Texture_Dictionary_Filename` dictionary when one is named (the
  "TXDSCOPE off" log line now prints only for the no-call case, same text as before for that case); vehicle load
  always takes only the path-less `.TXD`. `LibRw/RwRaceFx.cpp` `SfxTexture` always takes only `FX.TXD`.
- `TR` sky: load passes `bake_frames=false` (the default value of the deleted read); the draw keeps copy A
  (`!s_sky_rtt_a`, MASHED_NO_ARCTIC_SKY_RTT, untouched) and the librw route. UV scroll: `RaceSubmit_SetAnimTime(t)`
  and the D3D9 sky scroll test drop the kill switch.
- `TR` PONTOON_HIDE, SEA_TILE, OBJBSP (both sites), CLUMPANIM: the `!s_no_*` terms are gone.
- `Render/TrackHookSlots.cpp/.h`: `HookTrackFor` loses its early `nullptr` return.
- Comments that named a deleted knob as a live opt-out are trimmed. Two history notes still name one in past tense
  (`TR:1583` TEXANIM, `LibRw/RwRaceSubmit.h:176` UVSCROLL).
- Overlap with D5-KNOBS-B1-RENDERFX (`34a90a698`, not yet on `orch/integration`): both touch `RwRaceFx.cpp` and
  `TR`, in disjoint hunks (B1: RwRaceFx 190/992/1066/1072/2103, TR 4235/10260; this: RwRaceFx 353-366, TR
  elsewhere). `TR:4235` (B1's dirt load) is 180 lines from this step's nearest hunk (`TR:4034-4042`).

Built exe check (`grep -ac` on the new `mashed_re.exe`): the 10 deleted names occur 0 times, `MASHED_NO_PONTOON_ANIM`
/ `_STATE` / `_CARRY` once each.

## 3. Gates

Build: `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"` = `=== Build OK ===` (shared `.asi` not deployed).
rva-lint: `scripts/lint_rva_bodies.py` rc 0, `16 finding(s): CROSS-TARGET=16 | allowlisted=16 NEW=0`.

Arms: `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, private root
`D:/MashedWT/D5-KNOBS-B2-TRACKRENDER_root`, under `mashed_lock.py run` and `test-slot`.
Refs from `re/orchestrator/merge_train.json`: G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9 x3`.

| Gate | This exe (`mashed_re.exe` sha256 `7b5d0a99`) | Ref | |
|---|---|---|---|
| G-CTRL H_DEF step / gates / ptrace | `8284be46` / `ba407a16` / `31fe043e` | `8284be46` / `ba407a16` / `31fe043e` | same |
| B_DEF_K0 / K1 / K26 step | `1d5bf4f9` / `1d5bf4f9` / `1d5bf4f9` | `1d5bf4f9` x3 | same |
| 13-track table (`verify/rsky_20261016/run13.py run` + `score --cite verify/rres_20261016/cites_rres.json`) | 39 rows, file sha256 `2431afb1`, `INSTRUMENT GATES: PASS` | `verify/run13p3_20261029/tracks_score_sub_p3.txt` `2431afb1` | byte-identical |

All four arms and all 13 tracks ran on the first attempt (no re-run, no piz collision). The 13-track run used
`mashed_lock.py run --solo` and the stock budget (`RUN13_PRE_FRAMES` unset = 0, the run13.py default the reference
used). Committed copy of the score: `verify/d5knobsb2_20261030/tracks_score.txt`. D2 arms were not run: this step
touches no file under `full_set_paths` (Vehicle/, Ai/, Collision/); the merge train runs D2 under `-Full`.

## 4. Raw files (not committed)

Folder `D:/MashedWT/D5-KNOBS-B2-TRACKRENDER_data/` (sha256):

- `arms/H_DEF.step.csv` 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198
- `arms/H_DEF.gates.csv` ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb
- `arms/H_DEF.ptrace.log` 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3
- `arms/B_DEF_K0.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/B_DEF_K1.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/B_DEF_K26.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `tracks_score.txt` 2431afb1da35f95db9bb01535f071da5400d53fb2cc2550210465c0404be27c3
- `build.log` 8e6038b9dc03f1f24f290d60280d5c5012a4773f20b95f36bf0ae41cb37e7ce1
- `lint.txt` e9dd7a2547772d2722eb778f62fb83ad83e8923d9a9fe732eaf3f772c2aa42c2
- `arms_run.log` ae4f7c9c972e612e8e043ee893c2d64e0feecade0d7cc278d1e0bd1fe86b456d
- `run13.log` 62407a80d58419de37c5ceb34f139305468526c8f0812271e081bc38360a3b13
- exe `mashedmod/build/mashed_re.exe` 7b5d0a9987b6f1ab0ebd56f791822fcf88f4b39ca99fd8e589e8ca79b9d9f9de
- `tracks/` (13 track folders, run13 captures)

## 5. For the lead

- No `.ps1` edit needed.
- Left out: `MASHED_NO_PONTOON_ANIM`, `MASHED_NO_PONTOON_STATE`, `MASHED_NO_PONTOON_CARRY`. Setter: the gate runner
  `verify/savelink_20261023/run_arms.py:19` (`OPTOUT`, used as `STACK['OPT']`) and `:23` (`STACK['NOP']`,
  `STACK['NOPC']`). They can go once those report-only stacks are retired from the runner (a run_arms change the
  lead owns). When they go, the positive knobs `MASHED_PONTOON_ANIM` / `_CARRY` / `_STATE` at
  `Track/WarzonePontoon.cpp:715-717` (`Env(..) || !NoX()`) become dead too.
