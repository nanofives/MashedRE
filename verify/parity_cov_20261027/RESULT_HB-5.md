# HB-5 RESULT: parity coverage upkeep (run 14)

**Verdict: GREEN.** `nav_coverage.py` on `orch/integration` @ `1c2f681e9` gives output byte-identical to
the last coverage record (HB-ARCTIC, `f2a710899`, same SHA-256 `fcfaea0e...`). Since that record the only
frontend commits are menu-timing changes (`48798c3ae`, `be3429252`, `1bc83c9b2`, `7998eb8f4`). None of them
edits a `kT*` descriptor table, `kScreenTables`, `ActionToScreen`, or adds a `sid == N` branch. **Zero newly
wired screens, so zero new recipes were owed.** No source changed in this step.

Evidence pointer: `re/HARNESS_BACKLOG.md:57-59` (item 5). Asymmetries read first:
`re/analysis/parity_tooling.md:81-106`.

## 1. Commands

```
git -C C:\Users\maria\Desktop\Proyectos\Mashed worktree add .worktrees\HB-5 -b orch/HB-5 orch/integration
cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"          # === Build OK ===, asi NOT deployed
py -3.12 re/tools/nav_coverage.py                               # default --src mashedmod/src/mashed_re
git log --oneline f2a710899..HEAD -- mashedmod/src/mashed_re/Frontend mashedmod/src/mashed_re/exe_main.cpp
```

`nav_coverage.py` is a static source walker (`re/tools/nav_coverage.py:1-20`): it parses
`Frontend/MenuNavSM.cpp` and `exe_main.cpp`, not the binary. The build pins which tree the output
describes: `mashed_re.exe` SHA-256 below was built from the same sources the walker read.

## 2. nav_coverage result (copy: `verify/parity_cov_20261027/nav_coverage.txt`)

- 34 screen ids, 33 with tables, 11 reachable from root 1.
- Table hole: screen 27 (unchanged since the first run, `re/analysis/parity_tooling.md:138`).
- No dead-end actions.
- Kind consistency: kind 2 handled [4, 6, 8] / not [2, 3]. Kind 4 handled [19, 32] / not [31, 33].
  Same rows as HB-ARCTIC (`verify/hbarctic_20261027/RESULT_HB-ARCTIC.md:104-108`) and the first run.
- No static push path (table exists, reached only through the 0xff260000 mode-tile handler or modals):
  [0, 5, 7, 9-18, 20-26, 28, 29]. Unchanged.

## 3. Coverage table (statically reachable screens)

Recipe column = committed original-side artifact a parity diff can run against.

| scr | kind | sid-ref | original-side recipe on disk | gap |
|---:|---:|:-:|---|---|
| 1 | 1 | - | drawlist: `verify/parity_20260819/scr1_orig_burst.json`, `verify/menuspeed_20261027/run_parity.py` | none |
| 2 | 2 | - | imgdiff pair only: `verify/parity_caps/pair_scr2.png` | no draw-list capture |
| 3 | 2 | - | imgdiff pair only: `verify/parity_caps/pair_scr3.png` | no draw-list capture |
| 4 | 2 | yes | imgdiff pair only: `verify/parity_caps/pair_scr4.png` | no draw-list capture |
| 6 | 2 | yes | standalone shots only: `verify/pal4_20260910/re_s6_*.png` | no original-side capture |
| 8 | 2 | yes | imgdiff pairs: `verify/parity_caps/pair_scr8.png`, `verify/frontend_parity2/pair_scr8_options*.png` | no draw-list capture |
| 19 | 4 | yes | drawlist: `verify/scr19_parity_20261012/scr32_texty_recheck/dl_orig19a.json` (+ fe_residuals diffs) | none |
| 30 | 6 | yes | drawlist: `verify/scr19_parity_20261012/after_fix/orig_scr30.json` | none |
| 31 | 4 | - | text trace: `verify/scr19_parity_20261012/scr32_texty_recheck/orig_frame31.json` ("screen": 31) | no draw-list capture |
| 32 | 4 | yes | drawlist: `verify/scr19_parity_20261012/after_fix/orig_scr32.json` | none |
| 33 | 4 | - | text trace: `verify/scr19_parity_20261012/scr32_texty_recheck/orig_frame33.json` | no draw-list capture |

Not statically reachable but captured: 15, 16 (`log/burst_s15_*.json`, `log/burst_s16_*.json`), 18, 24
(`verify/scr19_parity_20261012/text4/dl_orig{18,24}.json`). Every id except 27 has a forced-push
standalone render in `verify/screen_id/` (`verify/screen_id/README.md:8-12`).

## 4. Standing gaps (not newly wired, filed, no recipe work started)

These predate this step and none was introduced by a commit since the last record, so the brief's
"newly wired" trigger does not fire. Listed so a later slice can pick them up:

| screen | reached by | missing |
|---|---|---|
| 2, 3, 4, 8 | static BFS from root 1 (2, 3 are literal actions on screen 1) | original-side `menu_draw_burst.py --screen N` capture + drawlist_diff run (only imgdiff pairs today) |
| 6 | screen 4 confirm pushes 6 (`38a9e0f30`) | any original-side capture |
| 31, 33 | kind-4 family next to 19/32 | draw-list capture (text trace only), plus the kind-4 sid-specific handling nav_coverage flags |
| 27 | none (NULL entry in `PTR_DAT_005f7638`, `verify/screen_id/README.md:9`) | no table to cover, nothing owed |

## 5. Gates

Not run. No source file changed (the commit holds this RESULT and the nav_coverage text only), so
the default G-CTRL / B hashes cannot move. `re/orchestrator/merge_train.json` refs stand.

## 6. Files changed

- `verify/parity_cov_20261027/RESULT_HB-5.md` (this file)
- `verify/parity_cov_20261027/nav_coverage.txt` (2 KB walker output)

## 7. Hashes

```
bb7a0cb93e9a0064362d05f3ae9afe609def0f6b6b80526b5caa88847372829a *mashedmod/build/mashed_re.exe
9d3092ce87d0c621ce15ec59b6cc097ea8635a81cd9c3abbeec9c4f38aaa3933 *mashedmod/build/mashed_re_dev.asi
fcfaea0efd814704cfc25b41abd9c3b94e26e2ee1e5a30a67c79cda4d22b5824 *D:/MashedWT/HB-5_data/nav_coverage.txt
500584744dad129e272894d926574b5018bd69d6fd895800069bc813631a1bdc *D:/MashedWT/HB-5_data/build.log
fcfaea0efd814704cfc25b41abd9c3b94e26e2ee1e5a30a67c79cda4d22b5824 *D:/MashedWT/HB-ARCTIC_data/nav_coverage.txt  (previous record)
```
