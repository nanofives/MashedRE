# RESULT HB-TRAIN-MATCHTABLES-BUILD

Change (re/orchestrator/merge_train.ps1): new `Test-MatchTablePath` (leaf name ADMITTED / PROVISIONAL / PROVISIONAL_RWC / REFUSED_UNITS .tsv, any folder). `Test-TextOnlyPath` returns false for it before the text-only globs, so a batch changing one is not text-only: Try-Batch goes on to the existing build path, which already sets MASHED_DDKOBJ=1 (object build plus ddkobj.py check) and logs `object build: <line>` and the `Object build:` line in the pass log. No change to merge_train.json (verify/* glob is overridden by the function).

Run: `pwsh -NoProfile -File re/orchestrator/merge_train.ps1 -SelfTest` -> `SELFTEST done: 0 failure(s)`, SELFTEST PASS (0 failure(s)).

New self-test cases (real Try-Batch on the fake repo, plus one function check):
- matchtables-build: m-tbl-adm / m-tbl-prov / m-tbl-ref / m-tbl-rwc (the four tables, RWC under verify/) are not text-only and proceed to the build (fake repo has no build.bat, so 'build failed').
- matchtables-build: Test-TextOnlyPath false for the four tables in any folder; other .tsv under verify/ stay text-only.

Not run: a real object build (harness row; the existing ddkobj 13 AST check already covers that every building pass sets MASHED_DDKOBJ=1 and logs the step).
