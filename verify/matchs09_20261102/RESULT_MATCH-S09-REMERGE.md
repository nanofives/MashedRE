# RESULT MATCH-S09-REMERGE (2026-11-02)

**Verdict: GREEN.** `orch/MATCH-S09` re-merged onto the current `orch/integration` tip 9d208ac35 (train 20261102_063056 had
bounced it on one conflict). History is not rewritten: old head 4079c670d, new merge commit dab53e17f, and a later text-only
commit adds this RESULT and `raw_sha256_remerge.txt` (final sha in the lead's REPORT).

(The merge commit message says "419a9abe9.." for the tip. That was a typo: the tip merged is 9d208ac35, shown by
`git rev-parse orch/integration` at merge time.)

## Conflict

One file, `re/console/pc_funcs.csv` (tool data read by `match.py`). Integration added a row for 0x00451680 (15 B,
`FUN_00451680`, from MATCH-S08's tool-data fix) and S09 added a row for 0x00451690 (49 B, `FUN_00451690`) at the same spot
between 0x00451060 and 0x004516d0. They are two different functions. Resolution: keep both, address order
(0x00451680 then 0x00451690). Check: 5817 data rows, 5817 unique entries, sorted. No other file conflicted
(`asi_sources.rsp`, `exe_sources.rsp`, `x87_tus.txt`, `PromoLoop_round34.cpp`, `PromoLoop_sessionB.cpp` auto-merged).
No matched source and no forwarder changed.

## Checks on the merged tree

| check | result |
|---|---|
| build `set MASHED_NO_DEPLOY=1&& mashedmod\build.bat` | Build OK, .asi not deployed |
| `lint_rva_bodies.py` | 2 findings (CROSS-TARGET=1, DUP-IN-TARGET=1), allowlisted=2, NEW=0 |
| `scripts/lan_release/archive_lint.py --ref HEAD` | OK, clean git archive of dab53e17f (asm-lint OK, 1150 files) |
| `relocheck.py` on the 19 swapped S09 sources (`swaps.tsv`) | 19/19 OK |

exe sha256 `c8d0c53cb6c7da42dd9c0e8220e0bea61c6f0865831d56b96ff7d2a835efe365`, asi
`4cb5904acf5ccb4a42c5eba23af9dfa83365ae84b51d0e8cfdb2388140c27321`.

## Gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, exe c8d0c53c, private root
`D:/MashedWT/MATCH-S09-REMERGE_root` (robocopy of the MATCH-REMERGE root, `MASHED_ROOT` set), under
`mashed_lock.py run --solo --label MATCH-S09-REMERGE-gates` (the open waited about 28 min behind the merge train's
priority claim, then ran 08:06 to 08:17). Every arm exit 0 and self-exited, one run each, no reruns.

| arm | file | sha8 | ref (`merge_train.json`) |
|---|---|---|---|
| H_DEF | step.csv / gates.csv / ptrace.log | 8284be46 / ba407a16 / 31fe043e | 8284be46 / ba407a16 / 31fe043e |
| B_DEF_K0, K1, K26 | step.csv | 1d5bf4f9 x3 | 1d5bf4f9 x3 |

Equal to the refs. The merge train's `-Full` pass (D2 x3, 13 tracks) was not run here.

## Raw files (not committed, `D:/MashedWT/MATCH-S09-REMERGE_data/arms`)

SHA-256 of each cited file: `raw_sha256_remerge.txt` (this folder). Build log `D:/MashedWT/s09_remerge_build.log`
(sha256 aa77c6f2f6b4d16ae7e6f070c55ad38ab3dff10a16756d9575e4982f45581d51), gates log
`D:/MashedWT/MATCH-S09-REMERGE_data/gates.log`.
