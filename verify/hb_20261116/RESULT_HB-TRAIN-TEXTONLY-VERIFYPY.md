# RESULT HB-TRAIN-TEXTONLY-VERIFYPY

Verification: `merge_train.ps1 -SelfTest` = 0 failures, no game build.

## Finding
`merge_train.json` `text_only_globs` held `verify/*`, and PowerShell `-like` lets `*` cross `/`, so every file under `verify/` was already text-only, scorers included (`verify/savelink_20261023/run_arms.py`, `vsum.py`, `verify/d2band42_20261029/score_d2_v2.py`, `verify/rsky_20261016/run13.py`). The SKILL.md known gap described the opposite. `re/frida/**` and non-scorer `re/tools/*.py` were build inputs.

## Change
- `re/orchestrator/merge_train.json`: `re/frida/*` added to `text_only_globs`; new `gate_input_dirs` (arm stacks) and `gate_scorer_extra` (lint scripts the train runs).
- `re/orchestrator/merge_train.ps1`: `Get-GateInputs` derives the gate-input set from the scorers' own imports (roots from `runner`, `full.*`, `gate_scorer_extra`; closure over `import`, `from X import` and quoted `*.py` tokens, resolved by basename among tracked files, over-approximating). `Test-TextOnlyPath` returns false for those files before the globs apply, so a scorer or scorer import under `verify/` or `re/tools/` is a build input. Computed only when the batch touches `verify/` or `re/frida/`. Match tables stay build inputs (`Test-MatchTablePath` first).
- `.claude/skills/roadmap-loop/SKILL.md`: known-gap text replaced.
- Remaining gap, stated in SKILL.md: `.py` under `re/tools/` that no scorer imports is still a build input (re/tools not freed, `re/tools/match/match.py` stays one by the existing check).
- Control line updated: `re/frida/hooks_registry.py` removed from the "stays non-text-only" list (no train script, build script or lint reads it, grep of `mashedmod/*.bat|ps1`, `scripts/lint*.py`, `scripts/lan_release`, `re/orchestrator/*.py`, `re/tools/ddkobj`).

## New self-test cases (all PASS)
Real `Try-Batch` on the fake repo: `v-evid` (verify evidence .py + RESULT), `v-frida` (re/frida js + hooks_registry.py), `v-mix` = text-only, no build; `v-runner` (scorer), `v-import` (scorer-imported .py under verify/), `v-retool` and `v-quoted` (scorer-imported .py under re/tools via from-import and via quoted path), `v-stack`, `v-tool` (unrelated re/tools .py), `v-mixsc` (evidence + scorer), `v-table` (match table + evidence, object build path) = go on to the build. Plus the closure unit check.

## SelfTest tail
```
[mtrain 03:59:20]   SELFTEST PASS: ddkobj 13: AST: every pass that builds sets MASHED_DDKOBJ=1 for build.bat, reads the step before the build-failed check and the gates, and clears the env (offsets 3453<3820<3889<4185<4248<4487<5100)
[mtrain 03:59:20]   SELFTEST PASS: ddkobj 13: build.bat fails on a failed object build and on a failed byte check (C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\HB-TRAIN-TEXTONLY-VERIFYPY\mashedmod\build.bat)
[mtrain 03:59:20] SELFTEST done: 0 failure(s)
SELFTEST PASS (0 failure(s)) log C:\Users\maria\AppData\Local\Temp\mtrain_selftest\20261116_035157\merge_train.log
```
