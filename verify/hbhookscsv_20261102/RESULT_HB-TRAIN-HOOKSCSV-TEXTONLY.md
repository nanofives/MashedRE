# RESULT HB-TRAIN-HOOKSCSV-TEXTONLY (2026-11-02)

Verdict: GREEN. Nothing in the build or in the gate scorers reads `hooks.csv` or `UNCERTAINTIES.md`.
`hooks.csv` is now in `text_only_globs`, and a new tracker lint runs on every merge-train batch (text-only
included), so a malformed tracker row still bounces. `merge_train.ps1 -SelfTest`: PASS, 0 failures, 31 checks
(`selftest_after.txt`). Static work only, no game boot, no build.

Branch `orch/HB-TRAIN-HOOKSCSV-TEXTONLY`, cut from `orch/integration` @ 78be531e1. Reviews as SOURCE `-Full`
(it changes `merge_train.json` and `merge_train.ps1`).

## 1. Reader survey (git grep -i `hooks\.csv|hooks_csv|UNCERTAINTIES|STUBS\.md|DEFERRED\.md`, tree 78be531e1)

Build path, 0 reads:

| File | Hits | Note |
|---|---|---|
| `mashedmod/build.bat` | 0 | calls the scripts below |
| `mashedmod/build_objs.ps1` | 0 | |
| `scripts/lint_rva_bodies.py` (rva / dual-copy lint) | 0 | reads `re/tools/dual_copy_allowlist.txt` (line 40) |
| `scripts/rva_body_scan.py` (its helper) | 1 | docstring line 4, names `backfill_exe_file.py`; no file open |
| `scripts/lint_inline_asm_abs.py` | 0 | reads `mashedmod/src` (line 30) |
| `re/tools/extract_loadicon_asset.py` | 0 | |
| `mashedmod/deps/librw/build_librw.bat`, `deps/qhull-2002.1/build_qhull.bat` | 0 | |
| `mashedmod/src/**` | comments only | the only string-literal match is `Audio/RwsStream.cpp:10`, a `//` comment; no fopen of either file |
| `re/match/**` (C4-by-match TUs) | 0 | |
| `scripts/lan_release/archive_lint.py`, `publish.sh`, `stage_lan_release.py` | 0 | |

`scripts/lan_release/archive_paths.txt` lists what a clean-archive build needs: `mashedmod`, `scripts`,
`re/tools/dual_copy_allowlist.txt`, `re/match`. `hooks.csv` is not in it, so `publish.sh` already builds
`mashed_re.exe` from a tree without `hooks.csv`.

Gate path (`merge_train.json` runner and `full` set), 0 reads: `verify/savelink_20261023/run_arms.py`,
`verify/savelink_20261023/vsum.py` (+ `re/tools/ai_ctrl_window.py`, `re/tools/b_envelopes.json`),
`re/tools/ai_speed_env.py`, `verify/d2band42_20261029/score_d2_v2.py` (+ its `band.json`),
`verify/rsky_20261016/run13.py` (+ `re/tools/drawlist_diff.py`, `verify/d1r1_20261016/check_gates.py`),
`verify/rres_20261016/cites_rres.json`. `merge_train.ps1` itself reads neither file. `merge_train.json` names
them only in `conflict_halt_note`.

Readers that exist are tracker tools, run by hand or by skills, never by the build or a gate:
`scripts/add_exe_file_column.py`, `scripts/backfill_exe_file.py`, `scripts/bulk_*.py`,
`re/tools/merge_trackers.py`, read-fleet queue prompts under `re/orchestrator/read_fleet/`.

`UNCERTAINTIES.md` (and `STUBS.md`, `DEFERRED.md`) were already text-only through `*.md`. Only `hooks.csv`
forced the build + gates.

## 2. Change

- `re/orchestrator/merge_train.json`: `text_only_globs` gains `"hooks.csv"` (root path only; PowerShell `-like`
  with no wildcard). No other key changed. `gctrl.ref` / `b.ref` untouched.
- `re/orchestrator/tracker_lint.py` (new): lints `hooks.csv` (UTF-8 decode, strict csv parse, header equal to
  the base's, column count, 8-lowercase-hex rva, confidence in C0..C4, duplicate rva) and `UNCERTAINTIES.md`
  (duplicate U-id on `| U-n |` rows). Findings are keyed without line numbers and compared with `--base`. Only
  NEW findings fail. On 78be531e1 it reports `hooks.csv 5969 rows, UNCERTAINTIES.md 3649 ids, pre-existing=4
  NEW=0` in 0.66 s. The 4 pre-existing are NCOL rows: `00404830` n=13, `0041f360` n=11, `00420870` n=12,
  `004464c0` n=9.
- `re/orchestrator/merge_train.ps1`:
  - `Tracker-Lint` runs the train's own copy (`$HERE/tracker_lint.py`, so a branch cannot drop its own lint) on
    the merged temp tree against the batch base. It runs after the archive lint and before the text-only return,
    so every batch gets it. A NEW finding fails the batch with `tracker_lint: ...`, which bounces the culprit
    through the existing one-at-a-time retry. Log: `<data>/<ts>/tracker_lint.txt`.
  - SelfTest section HB-TRAIN-HOOKSCSV-TEXTONLY: runs the real `Try-Batch` on the fake repo for 6 branches. The
    fake base carries one 9-column row, like the live file. `rc-ok` (hooks.csv C3->C4 + CHANGELOG) is text-only,
    gates `not run: text-only`, lint OK with `pre-existing=1 NEW=0`. `bad-ncol`, `bad-conf`, `bad-dup`,
    `bad-quote` (PARSE), `bad-udup` are each bounced with the expected NEW key. Two controls: `hooks.csv` was not
    text-only under the old globs, and 8 build-input / tool paths (`mashedmod/...`, `scripts/...`, `re/match/...`,
    `re/tools/dual_copy_allowlist.txt.bak`, `re/orchestrator/tracker_lint.py`, `re/frida/hooks_registry.py`,
    `re/tools/hooks.csv`) stay non-text-only.
  - Pre-existing SelfTest defect fixed: the HB-D2RERUN checks hard-coded `D2_DEF_1` / `D2_DEF_2`, which the
    SUBSTATE flip renamed `D2_DEFH_1..3` in `full.d2_arms`. Its 3 checks failed on every `-SelfTest` at
    78be531e1 (`selftest_base_78be531e1.txt`: `SELFTEST FAIL (3 failure(s))`). They now read the arm names
    from `full.d2_arms` and pass.

## 3. Replay of the run 31 pass 4 batch (20261102_101731)

That pass merged `orch/MATCH-S08FU-NOTES2` on base 78be531e1, logged `changed 3 file(s); text-only=False`,
built, and ran gates. Its 3 files (`git diff --name-only 78be531e1...orch/MATCH-S08FU-NOTES2`):

| File | old globs | new globs |
|---|---|---|
| `hooks.csv` | False | True |
| `re/analysis/CHANGELOG.md` | True | True |
| `verify/matchs08fu_notes_20261102/RESULT_MATCH-S08FU-NOTES.md` | True | True |

With this change that batch is text-only (no build, no gates). The tracker lint on its `hooks.csv` /
`UNCERTAINTIES.md` blobs against 78be531e1: `rows 5969 ids 3649 NEW [] pre 4`, so it passes.

## 4. Limits

- A re-classify row that also touches `re/frida/hooks_registry.py`, source, `scripts/`, or `re/tools/` is
  still non-text-only and still builds + gates. That is the intended split.
- A schema migration of `hooks.csv` (new column) fails the lint as HEADER + NCOL. It needs the lint updated in
  the same branch, which makes it a source change reviewed by hand. The train runs its own copy of the lint, so
  the updated lint takes effect only once it is on `orch/integration`.
- The lint does not judge row content (evidence, C-level legality). That stays with the re-classify skill.

## Evidence files

- `verify/hbhookscsv_20261102/selftest_after.txt`: SELFTEST lines, this branch, `SELFTEST PASS (0 failure(s))`.
- `verify/hbhookscsv_20261102/selftest_base_78be531e1.txt`: SELFTEST lines, unmodified orch/integration,
  `SELFTEST FAIL (3 failure(s))` (the D2RERUN arm names).
