# RESULT HB-TRAINFIX (Harness, run 5)

Verdict: **GREEN**. Branch `orch/HB-TRAINFIX` off `orch/integration` @ 8bbf48638.

## 1. merge_train.ps1: temp-worktree guard without a dot-sourced body

**Cause, verified in the code.** HB-TRAINCLEAN (653f632cb) wrapped steps 2 and 3 in
`function Invoke-Guarded([scriptblock]$Body) { try { . $Body } finally { ... } }`. Dot-sourcing
inside a function runs the block in that function's scope. Two failures follow:

- The INTEGRATION_LOG update did `$body = @(Get-Content $il)`. PowerShell variable names are
  case-insensitive, so this assigned an `Object[]` to the `[scriptblock]`-typed `$Body` parameter:
  "Cannot convert System.Object[] to ScriptBlock". That line only runs once something merged, so the
  crash came after all gates passed (the D3-CF-U9779 flip train).
- `$res`, `$merged`, `$bounced`, `$verdict` were function-local, so the REPORT after the wrapper
  would have read empty values even without the crash.

The selftest reproduces both with a copy of the old wrapper (controls below).

**Fix (d18ab6c62).**
- Steps 2 and 3 now run inside `try { ... } finally { Clear-Temps }` directly at script scope. No
  function wrapper, no scriptblock parameter.
- `New-Temp` registers each temp in `$script:Temps`. `Remove-Temp` unregisters it only when
  `diag.py wt-remove` returns 0. `Clear-Temps` removes whatever is still registered. Removal goes
  only through `diag.py wt-remove`, never `--force`, and `-KeepTemp` is still respected.
- `$body` is renamed to `$ilLines`.
- Restores HB-TRAINCLEAN's other rule, which b94c480a0 also reverted: `Cleanup-Step` removes only
  worktrees under `.worktrees\`.

**Selftest (47301914f).** `pwsh -NoProfile -File re/orchestrator/merge_train.ps1 -SelfTest` gives
`SELFTEST PASS (0 failure(s))`: 19 checks, 10 of them new.

| new check | result |
|---|---|
| guard: body ends without removing its temp (foreign-tip skip shape), finally removed it | PASS |
| guard: forced throw in the body, exception propagates AND finally removed the temp | PASS |
| guard: body that cleans up after itself, nothing left, registry empty | PASS |
| guard: two temps, one removed by the body, finally removed only the other | PASS |
| row worktree outside `.worktrees\`: skipped, worktree and branch kept | PASS |
| control: the old dot-sourcing wrapper reproduces the crash ("System.Object[]" to "System.Management.Automation.ScriptBlock") | PASS |
| control: the old wrapper loses the body's `$merged` (function-local) | PASS |
| new shape: `$body` assignment works and `$merged`/`$bounced` reach the code after the guard | PASS |
| AST: one script-scope try holds Try-Batch through Cleanup-Step, finally calls Clear-Temps | PASS |
| AST: no function wraps the run body, no dot-sourced variable outside the selftest control | PASS |

Negative check on the AST test: the same AST logic run on the reverted 653f632cb file finds
`top-level run-body tries=0 dot-sourced vars=1 (. $Body)`, so both AST checks would FAIL on the
old shape.

Full selftest log (6.5 KB, committed): `verify/harness_20261025/mtrain_selftest_HB-TRAINFIX.txt`.
Source run log: `%TEMP%\mtrain_selftest\20261025_135417\merge_train.log`, SHA-256
`61b862bdf20aee06002fda0bd82b69302f60f608f41e9d74339b4d159a0e8b88`.

Not run: the real train against `orch/integration`, which the task forbids. The first live train
after merge is the end-to-end check of the GREEN fast-forward path.

## 2. roadmap.ps1 snapshot: git EOL warnings

**Cause, verified.** Live `log/orch/DECISIONS.md` has 72 CRLF line endings out of 72 lines, and
`events.jsonl` has 141 out of 219. `roadmap.json` and `merge_queue.json` have 0. The old lines
predate HB-EOL. `snapshot` used `Copy-Item` verbatim into `eol=lf` paths, so `git add` warned.

**Fix (714204e6e).** New `Copy-Lf` (reads the source, writes via the existing `Write-Lf`).
`snapshot` uses it for all four files. The live files are not rewritten, because rewriting them in
place could drop an append from a concurrent session.

**Proof, with orch/integration untouched.** I made a scratch repo with `core.autocrlf=true`, the
branch's `.gitattributes`, and the tracked ledger files. I pointed `MASHED_ORCH_WT` at that repo and
`MASHED_ORCH_STATE` at a copy of `log/orch`.

```
=== OLD snapshot (8bbf48638 roadmap.ps1) ===
warning: in the working copy of 're/orchestrator/DECISIONS.md', CRLF will be replaced by LF the next time Git touches it
warning: in the working copy of 're/orchestrator/ledger/events.jsonl', CRLF will be replaced by LF the next time Git touches it
warning: in the working copy of 're/orchestrator/ledger/events.jsonl', CRLF will be replaced by LF the next time Git touches it
snapshot committed e03aed3
=== NEW snapshot (HB-TRAINFIX roadmap.ps1) ===
snapshot committed 0f9b16b
```

Both snapshot commits have the same tree, `2511439c01e2`, so the committed content is unchanged.
Only the warnings are gone. After the new snapshot, `git ls-files --eol` shows `i/lf w/lf` for all
four files.

## Gates

Not run: the change is harness-only (`re/orchestrator/*.ps1`). No build input changed and no exe
was built.
