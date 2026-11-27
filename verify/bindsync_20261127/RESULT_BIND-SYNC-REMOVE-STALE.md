Met: from REACH-6's base state the new binder produces both REACH-6 hand edits byte for byte, 2/2 cases (A rebased, B pre-rebase), git diff vs fdee19028 0 bytes, raw blob hashes SAME.

# RESULT BIND-SYNC-REMOVE-STALE (run 57, section BIND)

Branch `orch/BIND-SYNC-REMOVE-STALE` on orch/integration 287c10369. `git log 287c10369..HEAD` lists only this row's
commits: tool commit `ae2828c60`, then the RESULT commits. Tool row (RULE 2): no game arm, no G-CTRL / B, no build.
Worker on claude2, Opus 5.5 medium.

**Re-base note (lead check).** The first two commits (f8190d9e6, ec9fd5a38) sat on REACH-6's fdee19028, not on 287c10369.
The cause: a stray `git checkout fdee19028 --` in this worktree (meant as a read) detached HEAD at REACH-6's tip. They
were cherry-picked onto the branch at 287c10369 (ae2828c60 + 19a37c5e8). Every tree-dependent check below was re-run on
287c10369 + the tool: sync and bindings dry counts, old vs new written BINDINGS, unit tests, and the selftest. They gave
the same numbers. The reach6_compare A/B runs use fdee19028 only as the comparison target, in the scratch checkout, so
they stand as run.

## What changed (re/tools/bind/porttop_bind.py)

1. **`sync` drops stale TUs.** Before it adds lines, `sync` lists the `Match/PortTop_<rva>.cpp|.c` TUs in
   `mashedmod/exe_sources.rsp` that the profile no longer writes (`stale_tus`). For each one it removes the rsp line, every
   `re/tools/dual_copy_allowlist.txt` line whose key names the TU, and the generated file. A TU counts as stale only when
   all four hold:
   - the profile's plan.json has its rva with a kind other than `bind`;
   - no profile's newfiles.txt lists it;
   - no profile's plan reuses it as an existing exe body;
   - the file is gone, or its first line is this profile's own wrapper header (`own_wrapper_line`: own_mark, or the first
     line of header.new / header.missing).

   The ownership guard is needed. A first version without it flagged `Match/PortTop_00458c70.cpp` for bulk and raceload.
   That file is RACE-RWGLOBALS-FRAMELIST's hand-written bound TU, called through the raceload stand-in. It has a generated
   shape but another row's tag. With the guard every profile is at `rsp - 0` on the head.
   New output lines: `STALE <tu>`, `rsp - N`, `DROP <line>`, `allowlist - N`.
2. **`bindings` rewrites a row whose exe TU is gone.** A non-`missing` row whose note names a `Match/PortTop_<rva>` TU
   that no longer exists under `mashedmod/src/mashed_re` is rewritten whole from the plan (`row_tu_gone`). Before this
   change, only `missing` rows were updated.
3. **`bindings` writes `near-match` for a provisional reuse.** A `reuse` of an exe body that re/match/PROVISIONAL.tsv
   lists under the same symbol with status `provisional` is written as `near-match`. The note reads
   `existing exe body <file> (hand body|provisional body, L-126 provisional, re/match/PROVISIONAL.tsv <sym>; NOT matched,
   knob path only; one body for the rva)`. It says "hand body" when best_pct is `n/a (hand)` or the diagnosis starts
   with "hand body". A reuse not listed there still writes `matched`.
4. **New `bindings --rva <rva> ...` option.** It writes only the plan rvas named, so one row can carry its own `--step`
   tag. It is needed for case 3: `--step` alone would retag the 74 audio `missing` rows (they carry
   PORT-TOP-AUDIOTICK-STOPS on fdee19028), so REACH-6's tag on the 0x004ccef0 row needs `--rva`.

REACH-6's two edits, as tool commands (run from its tree, in this order):
```
py -3.12 re/tools/bind/porttop_bind.py sync --profile audio
py -3.12 re/tools/bind/porttop_bind.py bindings --profile audio --step RO-AUDIOINIT-45D460-REACH-6 --rva 004ccef0
```

## Checks

| check | result | evidence |
|---|---|---|
| REACH-6 base -> tool -> fdee19028, case A | **IDENTICAL byte for byte**: `git diff fdee19028` 0 bytes. Raw `hash-object --no-filters` of rsp / allowlist / BINDINGS.tsv equals the fdee19028 blobs (93b05b22f, d91d60301, 7003f1fe0). `Match/PortTop_004ccef0.cpp` is absent. Tool output: sync `rsp - 1`, `allowlist - 1`, `rsp + 1` (005c78c0). bindings --rva `updated 1`, then a plain bindings run 0/0. | reach6_compare_A.txt |
| case B (base row = 37aca6283^'s `near-match PortTop_Bound_004ccef0 ... Match/PortTop_004ccef0.cpp`, the row naming the deleted TU) | **IDENTICAL byte for byte**, same hashes | reach6_compare_B.txt |
| unit tests re/tools/bind/test_porttop_bind_lib.py | **15/15 PASS** (12 old + sync_stale, bindings_tu_gone, bindings_prov_reuse) | unit_test.txt |
| binder selftest on this tree (lock free; test-slot slot 2/8) | **PASS** (0 fail), every profile: raceload, audio, bulk, raceframe, renderframe, gates plan + wrappers IDENTICAL. BINDINGS.tsv from the audio and bulk plans IDENTICAL 0/0. | bind_selftest.txt |
| `bindings --profile <p> --dry`, old (287c10369) vs new | unchanged: audio 0/0, bulk 0/0, gates 2/9, raceframe 4/14, raceload 14/282, renderframe 0/0 | bindings_old_vs_new.txt |
| `sync --profile <p> --dry` on this tree | every profile `rsp - 0`, `allowlist - 0`, `rsp + 0`, `allowlist + 0` (the old tool: `rsp + 0`, `allowlist + 0`) | sync_dry.txt |
| written BINDINGS.tsv, old vs new, per profile | IDENTICAL for audio, **bulk**, gates, raceframe, renderframe. **raceload DIFFERENT in 5 rows** (counts unchanged), see below. | bindings_old_vs_new.txt |
| Frida temp folders | no harness run | |

The base state for cases A and B (reach6_compare.py docstring):
- exe_sources.rsp, dual_copy_allowlist.txt and `Match/PortTop_004ccef0.cpp` come from 6b7da6d8e. That is REACH-6's
  commit before 100ba0cc5, which hand-removed the rsp line and the file. 6cb4697a2 later pruned the allowlist line.
- BINDINGS.tsv is fdee19028's with the 0x004ccef0 row put back. A uses 100ba0cc5's row, which is the rebased branch's
  row before the hand edit. B uses 37aca6283^'s row, which is the pre-rebase row.
- The scratch checkout was D:\MashedWT\BIND-SYNC-REMOVE-STALE_scratch, detached at fdee19028, with this branch's
  porttop_bind.py copied in. It was removed with `scripts/diag.py wt-remove`. Nothing was committed to REACH-6's branch,
  and its worktrees were not touched.
- `git status` in the scratch showed ` M` on the three files. Under core.autocrlf=true that is line endings only: the
  raw-bytes hashes above equal the committed blobs.

### Generated output this change alters (raceload, pending adds only)

`bindings --profile raceload` (not run on the head: its dry is 14/282 before and after) now writes 5 of its 282 added
rows as `near-match`, where it wrote `matched` before. The rows: 0x004c4dc0 RwCore_004c4dc0, 0x004db3e0 RwCore_004db3e0,
0x004dc8a0 RwCore_004dc8a0, 0x004e0920 RwCore_004e0920, 0x0052daf0 RwCore_0052daf0.
- None of the 5 is in the committed BINDINGS.tsv.
- re/match/PROVISIONAL.tsv lists each one with status `provisional`, best_pct 91.2 / 95.2 / 58.3 / 20.8 / 42.5.
- The near-match label rests on a run, not only on the list (L0 merge bar). The table below was measured in this
  worktree (tools\vc7ddk found under .worktrees\, mashed_lock free), using each source's own // TOOLCHAIN and // FLAGS.
  Output is in `raceload5_scores.txt` and `raceload5_match.txt`.

| rva | source | toolchain / flags | match.py --bytes score | differing bytes | relocheck |
|---|---|---|---|---|---|
| 0x004c4dc0 | re/match/s19/004c4dc0.c | ddk, /O2 /GS /Ob1 /G5 | 91.2% (80/80 ins) | 19 of 225 B (first +0x85) | OK (checked 3) |
| 0x004db3e0 | re/match/rwl01/004db3e0.c | ddk, /O2 /Ob1 /G5 | 95.2% (105/105 ins) | 9 of 368 B (first +0x148) | BAD (checked 20) |
| 0x004dc8a0 | re/match/rwl01/004dc8a0.c | ddk, /O2 /Ob1 /G5 | 58.3% (12/12 ins) | 17 of 46 B (first +0x0) | BAD (checked 7) |
| 0x004e0920 | re/match/rwl01/004e0920.c | ddk, /O2 /Ob1 /G5 | 46.3% (141/149 ins) | 446 of 570 B (first +0x16) | BAD (checked 21) |
| 0x0052daf0 | re/match/rwl03/0052daf0.c | rw9044, /O2 /Ob1 /G5 | 44.0% (160/200 ins) | 355 of 442 B (first +0x2) | BAD (checked 16) |

- None of the 5 is byte-identical, so `near-match` is the right kind for all 5, and `matched` was the same mislabel as
  0x004ccef0's. There is no 100% + relocheck OK row to report as a finding.
- For 0x004c4dc0 relocheck is OK, but the 19 differing bytes still keep it below a match.
- Side note, not acted on: today's scores for 0x004e0920 (46.3%) and 0x0052daf0 (44.0%) differ from PROVISIONAL.tsv's
  best_pct (20.8 / 42.5). That column is not this row's to change.
- The committed BINDINGS.tsv does not change. Bulk's written output does not change, so BIND-0048AE00-CALLEES's
  generated output does not change.

## Unfinished scope

- None of the three cases is open.
- REACH-6 still has to run the two commands above on its own tree. It then has to do its own step 3 (builds, ddkobj,
  RO arm), which is its row, not this one.
- bindings_old_vs_new.py needs `git show 287c10369:re/tools/bind/porttop_bind.py > re/tools/bind/_old_pb.py` before a
  re-run. The copy was deleted, not committed.
