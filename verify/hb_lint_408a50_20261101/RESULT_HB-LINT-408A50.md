# RESULT HB-LINT-408A50 (+ D4-DC-INC-00408a50)

Branch `orch/HB-LINT-408A50`, base `orch/integration` = `race/first-frame-parity` = `0e3d71a75`.
Fix commit `e03f1bdc3`. Ready for the train as **SOURCE -Full** (Ai/ is in `full_set_paths`).

Verdict: **GREEN**. Lint passes on a clean git archive of the branch and in the worktree, the clean-archive build compiles, default gates unchanged.

## 1. Why the train passed and publish.sh failed

The two sides lint **different trees**. The train does not skip a step.

| side | tree | re/match/ | lint on 0e3d71a75 |
|---|---|---|---|
| merge train | full worktree (`merge_train.ps1:265` builds, `build.bat:35` and `merge_train.ps1:270` lint) | present (462 tracked files) | 48 findings, allowlisted 48, NEW 0 |
| publish.sh | `git archive "$REF" mashedmod scripts re/tools/dual_copy_allowlist.txt` (`publish.sh:46` at 0e3d71a75) | **absent** | 47 findings, allowlisted 46, **NEW 1: DUP-IN-TARGET 0x00408a50** |

Mechanism, cited at 0e3d71a75:
- `Match/MatchS01.cpp:104` has `#include "../../../../re/match/s01/00407a60.c"`, whose
  `// MATCH: 0x00408a50 FUN_00408a50` (`re/match/s01/00407a60.c:33`, `FUN_00408a50` #defined to
  `PerCarRaceProgressGet` at `MatchS01.cpp:55`) is the third body of 0x00408a50.
- `scripts/rva_body_scan.py:132` (`if not inc.is_file(): continue`) skipped the unresolved include
  in silence (same shape for `.inc` at `:157`). The scanner saw 2 of 3 bodies.
- The allowlist key is the sorted file SET (`lint_rva_bodies.py:43-44`):
  `DUP-IN-TARGET:00408a50:Ai/AiLeafTargets.cpp,Ai/AiTargetClosest.cpp,Match/MatchS01.cpp`
  (`re/tools/dual_copy_allowlist.txt:82`). The 2-file finding produced a different key, so it was NEW,
  and the 3-file key went stale.
- The archive could not have built anyway: 84 `#include "../../../../re/match/..."` lines in the port
  TUs do not resolve in it (list in `lint_runs.txt`). The lint just failed first. publish.sh's path
  list predates MATCH-S01 (`b5dd53ed8` 2026-10-29 vs `c73f918c3` 2026-10-31).

Ruled out:
- **CRLF**: the allowlist is CRLF in the working tree and in the extracted archive. An archive WITH
  re/match/ and the CRLF allowlist passes (48/48), and the same with the allowlist converted to LF passes
  too. `load_allowlist` uses `splitlines()` + `strip()` (`lint_rva_bodies.py:51-52`), so either ending
  matches. The `\r` in D4-DC-INC-00408a50's ledger title is copied text, not part of the key.
- **Untracked files**: every re/match/ file the build includes is tracked (`git ls-files re/match` = 462).

Second gap found on the way: `merge_train.json` `text_only_globs` contains `*.txt`, so a batch that only
changes `re/tools/dual_copy_allowlist.txt` (a build input) skips build and lint
(`merge_train.ps1:260`).

## 2. One body for 0x00408a50 (D4-DC-INC-00408a50)

Kept body: `PerCarRaceProgressGet`, the C4-by-match source `re/match/s01/00407a60.c:32-37` compiled by
`Match/MatchS01.cpp` (both targets). Removed: the verbatim naked copies `AL_408a50`
(`AiLeafTargets_naked.inc`) and `TG_408a50` (`AiTargetClosest_naked.inc`).

How: both naked specs drop the func entry and bind `"0x00408a50": "PerCarRaceProgressGet"`
(`re/tools/naked_specs/aileaf_g1.json`, `target_p3.json`), the .inc files are regenerated with
`re/tools/gen_naked_asm.py` (the generator reproduced both pre-fix files byte for byte first), and each
TU declares `extern "C" float __cdecl PerCarRaceProgressGet(int i);`. The 6 call sites become
`call PerCarRaceProgressGet` (0x00414626, 0x0041467f, 0x0041594a, 0x004159ae, 0x00414ab6, 0x00414ae2).
No edit in `Match/MatchS01.cpp` or `re/match/` (MATCH-S01-RC works there).

Equivalence of the call target, disassembled from the built binaries:

| binary | bytes |
|---|---|
| original 0x00408a50 | `mov eax,[esp+4]` / `imul eax,eax,0x30c` / `fld dword ptr [eax+0x8a96e8]` / `ret` |
| mashed_re.exe `PerCarRaceProgressGet` | `imul eax,[esp+4],0x30c` / `fld dword ptr [eax+0x8a96e8]` / `ret` |
| mashed_re_dev.asi `PerCarRaceProgressGet` | same as the exe |

Same address read (0x008a96e8 + i*0x30c, the block `DAT_008a9620` +0xc8 bound to the absolute VA at
`MatchS01.cpp:82`), same result in ST0, same clobbers (EAX, EFLAGS), cdecl `ret`. The naked callers
see no difference.

Allowlist key `DUP-IN-TARGET:00408a50` removed. Findings 48 -> 47 (DUP-IN-TARGET 15 -> 14).

## 3. Train gap closed

In the branch (.py / .sh / .txt):
- `scripts/rva_body_scan.py`: every unresolved `re/match/` or `.inc` include is recorded in
  `MISSING_INCLUDES`. `scripts/lint_rva_bodies.py` fails on any (`MISSING-INCLUDE`) before it counts
  findings, so a lint can no longer pass or fail on a tree that hides bodies.
  `scripts/test_rva_body_scan.py` has a case for it (9/9).
- `scripts/lan_release/archive_paths.txt`: the one list of what a clean archive build needs
  (adds `re/match`). `publish.sh` reads it at the ref (`git show "$REF:..."`, fallback to the same 4 paths).
- `scripts/lan_release/archive_lint.py`: `git archive` of a ref with that list, then
  [1/3] include-closure (every quoted include leaving mashedmod/ resolves), [2/3] lint_rva_bodies.py,
  [3/3] lint_inline_asm_abs.py, all inside the extracted tree. About 15 s.

For Mariano to apply (.ps1): `verify/hb_lint_408a50_20261101/merge_train_archive_lint.diff`
(`git apply` checked, PowerShell parser 0 errors). It runs `archive_lint.py --ref HEAD` on every batch
right after the merge, **before** the text-only shortcut, so a `*.txt`-only batch is linted too, and
writes `archive_lint.txt` into the batch data dir. If the script is missing it logs a WARN rather than
failing, so the diff can be applied before or after this branch merges.

## 4. Verification

Full output: `verify/hb_lint_408a50_20261101/lint_runs.txt`.

| run | result |
|---|---|
| `archive_lint.py --ref e03f1bdc3` (paths read from the ref) | include-closure 0 unresolved, rva-lint 47/47 NEW 0, asm-lint OK: **PASS** |
| worktree `lint_rva_bodies.py` | 47 findings, allowlisted 47, NEW 0: **PASS** |
| control: `--ref e03f1bdc3 --paths <publish.sh's old 3 paths>` | FAIL: 84 MISSING-INCLUDE (the new check names the real cause) |
| control: `--ref 0e3d71a75 --paths <old 3 paths>` | FAIL: NEW DUP-IN-TARGET 0x00408a50, the exact publish.sh error |
| `test_rva_body_scan.py` | 9/9 |
| worktree build (`MASHED_NO_DEPLOY=1 mashedmod\build.bat`) | Build OK, both targets |
| clean-archive build of e03f1bdc3 (publish.sh's archive + `build.bat`, `MASHED_NO_DEPLOY=1`, into the scratchpad) | rva-lint OK 47/47, asm-lint OK, Build OK. Its exe bytes differ from the worktree exe: each embeds its own build path (13 occurrences each) |

Default gates (`verify/savelink_20261023/run_arms.py`, refs from `re/orchestrator/merge_train.json`):

Exe gated: worktree build of `e03f1bdc3`, `mashedmod/build/mashed_re.exe` sha256 `d9d9b07a5b1721ebce9cbcec96fdd97f224d911bd53bfeeb2ac72b3441849b5e`
(run_arms prints exe=d9d9b07a). Private root `D:/MashedWT/HB-LINT-408A50_root`, wrapped in test-slot + mashed_lock.

| gate | got | ref (merge_train.json) | |
|---|---|---|---|
| G-CTRL H_DEF step/gates/ptrace | 8284be46 / ba407a16 / 31fe043e | 8284be46 / ba407a16 / 31fe043e | unchanged |
| B K0 / K1 / K26 | 1d5bf4f9 / 1d5bf4f9 / 1d5bf4f9 | 1d5bf4f9 x3 | unchanged |

Full set (D2, 13 tracks) not run here: Ai/ is in `full_set_paths`, so the train runs it on -Full.

Raw arm data stays in `D:/MashedWT/HB-LINT-408A50_data/arms` (not committed).

Raw files cited:
- `D:/MashedWT/HB-LINT-408A50_data/arms/H_DEF.step.csv` sha256 `8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198`
- `D:/MashedWT/HB-LINT-408A50_data/arms/H_DEF.gates.csv` sha256 `ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb`
- `D:/MashedWT/HB-LINT-408A50_data/arms/H_DEF.ptrace.log` sha256 `31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3`
- `D:/MashedWT/HB-LINT-408A50_data/arms/B_DEF_K0.step.csv` sha256 `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457`
- `D:/MashedWT/HB-LINT-408A50_data/arms/B_DEF_K1.step.csv` sha256 `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457`
- `D:/MashedWT/HB-LINT-408A50_data/arms/B_DEF_K26.step.csv` sha256 `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457`
