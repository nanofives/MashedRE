# RESULT HB-TRAIN-MATCH-TEXTONLY: re/match/** text-only unless built, -Full only for decided flips

Verdict: GREEN (self-test 100 PASS, 0 FAIL; no game launched). Stacked on orch/HB-TRAIN-CONFLICT-SKIP.

## What changed
`re/orchestrator/merge_train.ps1`:
- `Get-BuildMatchRefs <tree>`: the set of `re/match/` files the build reads in the merged tree. Seeds are every
  `mashedmod/exe_sources.rsp` / `asi_sources.rsp` entry (relative to `mashedmod/src/mashed_re`) that resolves under
  `re/match/`, plus every `#include "..."` in a mashedmod `.cpp/.c/.h/.hpp/.inl` (build/ and deps/ excluded) that
  resolves there. Each referenced file is then read for its own `#include`s inside `re/match/` (transitive). If
  `mashedmod/build.bat` or `build_objs.ps1` names `re\match` / `re/match`, every file counts as a build input (`*`).
- `Test-TextOnlyPath`: a changed path is text-only by `text_only_globs` as before, or when it is under a
  `text_only_unless_built` root (`re/match/`) and not in that set. The set is computed only when a batch changes a
  `re/match/` file that the globs do not already free.
- `Resolve-FullFlag`: `-Full` counts only when the batch holds a decided-flip row (the HB-TRAIN-DRAIN `Test-FlipRow`
  rule: id `-FLIP`/`FLIP-`, or "decided flip" in the ledger title). Otherwise it is dropped with a log line;
  `full_set_paths` files still bring the full set in. New `-ForceFull` keeps the old `-Full` (the drain passes it on).
- Text-only now ignores a dropped `-Full`: a text-only batch skips build and gates. With an effective `-Full` (a flip
  pass, or `-ForceFull`) the batch is always built and gated, as before.
`re/orchestrator/merge_train.json`: `text_only_unless_built: ["re/match/"]` plus a note. Refs unchanged.
Docs: `re/orchestrator/MERGE_TRAIN.md` "Text-only and -Full", `.claude/skills/roadmap-loop/SKILL.md` ("When to pass -Full").

## Self-test
Summary lines: `verify/hb_train_20261110/selftest_match_textonly.txt`. Raw log (not committed):
`C:/Users/maria/AppData/Local/Temp/claude/2279e0b7f1f09563/2e04f018-b1e0-463e-8db5-d34153b1d580/scratchpad/st_mt.log`,
SHA-256 `8a41542feeb9aca0859788056d7cbe6eeb8ce850435dbe3b8907b97bb8c81ee6`.

New checks (real Try-Batch on the fake repo: a mashedmod TU including `re/match/t01g/004111c0.c`, which includes
`common.h`; an rsp entry `..\..\..\re\match\rsp1\a.c`; unreferenced `re/match/s99/*`):
- build refs = exactly `rsp1/a.c, t01g/004111c0.c, t01g/common.h`.
- a match-only change to unreferenced files = text-only, gates `not run: text-only`.
- the included file, the file it includes, the rsp-listed file, and an unreferenced file mixed with a mashedmod source
  = not text-only (the train goes on to the build, which fails in the fake repo by design).
- an effective -Full still builds a match-only batch.
- a build script naming `re\match` makes every file a build input.
- `Test-TextOnlyPath` frees `re/match/` only (not `re/matchx/`, not `re/tools/match/`).
- `Resolve-FullFlag`: -Full + no flip = off, -Full + flip = on, -ForceFull = on, nothing = off.
- live tree: `re/match/t01g/004111c0.c` (the row's example) and `re/match/s01/00401340.c` are build inputs, and 790 of
  4035 files under `re/match/` are (the other 3245 are text-only now). The live scan takes about 3 s.
All earlier checks (DRAIN, CONFLICT-SKIP, STACK, INFRA, WTADD-RETRY, AST shape) still pass. Live `-Plan` prints the same
batch as before.

## Saving
The row title's figure: about 17 of 20 min per match-only pass (build + gates), now skipped when no file of the batch
is read by the build. Not measured live.

## Watch
A decided flip whose row id and title carry neither `FLIP` nor "decided flip" would run without D2 and the 13 tracks
unless the lead passes `-ForceFull` (or it touches `full_set_paths`). The drain refuses no such row either. The default
gates still run on every non-text-only batch.
