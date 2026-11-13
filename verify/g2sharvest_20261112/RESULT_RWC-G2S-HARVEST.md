# RESULT RWC-G2S-HARVEST (match-only / text row, no build, no gates)

Branch orch/RWC-G2S-HARVEST from orch/integration a616fcc3b. Source review: log/orch/RW_CLOSURE_REVIEW_20261112.md.

## What was done
1. **g2s census rerun** (MSYS_NO_PATHCONV=1, `/O2 /GS`, toolchains ddk and rw9044, two passes each: `decompKeep.json` with a
   function entry, `decompCreated.json` transient-created). Input decompile JSON: the review session's (not redone in Ghidra).
   Per-pass scores: `results_{all,cr}_{ddk,rw9044}.tsv`. Merged table `g2s_census_rerun.tsv` (best of both toolchains), made by
   `strict.py` (the review's script, `_9044` dir suffix changed to `_rw9044`). Result: 121 at 100%, **115 strict (byte-identical
   `match.py --bytes` + relocheck OK)**, 6 bytes-only. The review counted 114: the extra one is **0x00472330** (ddk 100.0% now,
   the review's log has 93.3%); checked by hand: BYTE-IDENTICAL 40 B, relocheck OK.
2. **Sources into the tree:** 114 files `re/match/g2sh/<rva>.c` (raw g2s output, header `// FLAGS` / `// TOOLCHAIN` kept;
   0x004d8090 stays at its existing `re/match/s21/004d8090.c`).
3. **12 hooks.csv rows** (11 of the review + 0x00472330) with `re/tools/c1_add.py` (C1, CHANGELOG `(none)->C1`):
   0x00472330, 0x004f08e0, 0x004f08f0, 0x004f0990, 0x004f09c0, 0x004f4540, 0x004f8620, 0x004f8630, 0x005142d0, 0x005142f0,
   0x0052d950, 0x0052fd90. Subsystem is `render` for the 0x004f.. / 0x0052fd90 RW rows and `unknown` for the four ambiguous ones
   (0x00472330, 0x005142d0, 0x005142f0, 0x0052d950). [UNCERTAIN] the subsystem of all 12 (band-based, no caller scan).
4. **Promotions TSV** `re/promotions/RWC-G2S-HARVEST.tsv`: 115 rows, route `match`. Dry run (no --apply) on this branch:
   `c4_reclass.py batch --tsv re/promotions/RWC-G2S-HARVEST.tsv --result <this file> --strict --jobs 4`
   -> **102 PASS, 0 REFUSED, 13 SKIP already C4** (`c4_dryrun.log`). The review said 101 not C4 today; the 102nd is 0x00472330.
   The 12 rows added in step 3 pass because they exist on this branch; the merge train key-merges hooks.csv, so the TSV applies
   on the head after the rows. Not applied here (HB-TRACKER-AT-MERGE). All 102 are C4 **by match** (RE axis), not port.
5. **match_permute.py** writes to `<checkout holding the file>/log/permute/` (nearest parent with `.git`), `--out-dir` overrides.
   Checked: a run on re/match/s19/004c3dc0.c in this worktree wrote `.worktrees/RWC-G2S-HARVEST/log/permute/004c3dc0.c`.
6. **Generator fix** `gen_rwcore_c_tu.patch` (applies with `git apply -p1` to orch/RWC-PLUGIN-ATTACH, checked with
   `git apply --check`; `gen_rwcore_tus.py` is not on orch/integration, so it is a patch, not an edit). A LIFT_INSTEAD source the ledger scores 100%
   (0x004c3d90, 0x004c3dc0, 0x004d8090, 0x004e89a0: all relocheck OK, all already C4) is kept instead of lifted and emitted as
   `RwCore_<rva>.c` (same `#define` bindings, no MATCH_PORT, no `extern "C"` wrapper). `build_objs.ps1` names the dependency
   json `<base>.<ext>.json` so `.c` TUs are tracked. **Not run**: no build (match-only row). The lead of RWC-PLUGIN-ATTACH must apply
   it, run `gen_rwcore_tus.py`, and build; the first build decides if VS2022 compiles the four sources as C. [UNCERTAIN]
   `RwCore_Knob`/rsp consumers of `.cpp` names.

## Raw data
`log/orch/g2s_harvest/` (gitignored, g2s output dirs). Committed: tables, logs, strict.py.
