# RESULT BIND-BULK-PROFILE (run 54, section BIND, tool row)

Branch `orch/BIND-BULK-PROFILE` from `orch/integration` @ 65ef9f74a, orch/integration 6bb42c68a merged in. Worker on
claude2, Opus 5.5 medium. Row kind: top-down tool row (binder change, KICKOFF_run54 rule 2).

## Verdict

__VERDICT__

The RACE-BIND-C4-BULK checkpoint 1 generator (`verify/bindbulk_20261121/tools/bulk_bind_gen.py` + `regen.py` +
`refs_gen.py`, left untouched as history) is now the profile `bulk` of `re/tools/bind/porttop_bind.py`
(`re/tools/bind/state/bulk/`). On cp1's own tree the profile reproduces cp1 byte for byte. On the head, 36 plan lines
move from cp1's `last_plan.txt`, each named with its cause in `state/bulk/moved.tsv`. The selftest checks that file.
`write --profile bulk` on the head changes 3 generated files. The 0x005c91f0 wrapper now calls the audio profile's
`PortTop_Bound_005aba20` instead of `PortTop_BulkStop_005aba20` (the RO-AUDIOINIT-45D460-REACH-2 blocker).

## 1. What moved where

| Was (bulk_bind_gen.py and the audio generator prefix it exec'd) | Now |
|---|---|
| roots: `bulk_bind_gen.py <wt> <roots,comma>` from `log/orch/run53_bindlists/bulk_cp1.tsv` | `state/bulk/roots.txt` (362 rvas, list order) |
| `FORCE = set()` (a root with an exe body is reused) | `profile.json` `force_roots: false` |
| `if s and rva not in ROOTS: stop 'outside the bulk cp1 list ...'` | `roots_only_reason` (the same text, `%s` = source) |
| pass 1: source already compiled by an exe TU -> reuse, REFUSED | `samesrc_reuse: true`. On the head the profile's own wrappers are left out (cp1 deleted them before planning, regen.py step 1) |
| pass 2: lowest rva carries a shared source, members reuse | `group_same_source: true` + `group_primary: lowest` |
| pass 3: file-local copy of another bind -> stop | `drop_local_copies: true` |
| `existing_stop()`: a stop's callers name an existing `PortTop_*Stop*_<rva>(` of another file | `stop_sym_foreign: true` (the profile's own stop file left out, as cp1 deleted it before planning) |
| wrapper text (thunk name, group members, `PortTop_Keep_<rva>` anchor per bind and member) | `wrapper_style: bulk` (`Binder.wrapper_text_bulk`), header lines as `hdr` data |
| stops text | the binder's count style + `stops_subst` (6 string pairs), `stop_prefix` `PortTop_BulkStop_` |
| ALIAS (7), CALLEE_OVERRIDE (1), NOT_LOCAL (2) | `state/bulk/rules.tsv` |
| audio prefix tables (PROV_BIND, EXTRA_PRE, EXTRA_IN, LOCAL_SHARED, LOCAL_AS_BOUND, ALIAS) | `state/_shared/rules.tsv`, unchanged: cp1's PROV_BIND (audiotick_bind_gen.py at d4fda5c93, lines 25-39) equals the shared prov_bind set |
| `rltools/cpp_unclean.txt` copy | `state/bulk/cpp_unclean.txt` (byte copy of `tools/cpp_unclean.txt`) |
| `plan_cp.json`, `disposition_cp.json`, `newfiles_cp.txt` | `state/bulk/plan.json`, `disposition.json`, `newfiles.txt` |
| `refs_gen.py` | none needed: the raceload profile's `refs` step already owns `Match/PortTop_BulkRefs.cpp` and lists the bulk anchors through `foreign_keep_marks` |
| allowlist seeding in regen.py | `sync --profile bulk` with `allow_text` / `allow_default` |

New rule, `bound_wins: true`: before any stop rule (`stop_reason`, cpp-unclean, roots-only), an rva that has a
`Match/PortTop_*` match-include body in the exe index (other profiles' wrappers, own ones left out) reuses it.
`stop_force` and `alias` still come first. This is what makes a bound body of any profile win over a bulk stop:
0x005aba20 (audio, `Match/PortTop_005aba20.cpp`, provisional) and 0x00543bb0 (raceload) by the index reuse, and the 33
cpp-unclean rvas by the raceload C units of BIND-CUNCLEAN-AS-C.

Other code: `history_moves` / `selftest_history` (selftest compares the listing with a frozen listing and requires every
moved line in `moved.tsv`, no unnamed and no stale row), `plan --index <json>` (plan against a frozen exe index, debug
and history checks only, refused with `write`), `keep_list`, `foreign_stops`. Every new key defaults to the old
behaviour, so the other profiles take the same code path as before.

## 2. Byte-for-byte checks

### 2a. cp1's own tree

Tree: `git archive d4fda5c93` (the cp1 commit that wrote `last_plan.txt`) of `hooks.csv`, `re/match`, `re/promotions`,
`mashedmod/exe_sources.rsp`, `mashedmod/src/mashed_re`, `verify/bindbulk_20261121/tools`, with this branch's
`porttop_bind.py` and `state/` copied in. Harness `verify/bindbulk_20261121/profile/tools/cp1_repro.py`, output
`profile/cp1_repro_cp1tree.txt`:

| Check | Frozen index (`tools/exeidx.json`) | Live exe index |
|---|---|---|
| plan vs `plan_cp.json` (values and key order, 505 entries) | IDENTICAL | IDENTICAL |
| listing vs `last_plan.txt` (506 lines; its trailing `wrote 98 bind files + 227 stops` print line skipped) | IDENTICAL | IDENTICAL |
| 97 wrapper texts vs `Match/PortTop_<rva>.cpp` | IDENTICAL | IDENTICAL |
| stops text vs `Match/PortTop_BulkStops.cpp` | IDENTICAL | IDENTICAL |
| anchors, groups, refused roots vs `disposition_cp.json` | IDENTICAL | IDENTICAL |

### 2b. The head: 36 moved lines, all named

`porttop_bind.py plan --profile bulk` on the head: `reuse 216, stop 192, bind 97` (cp1: `stop 227, reuse 181, bind 97`).
The 97 binds and their sources are the same. `state/bulk/moved.tsv` (generated by `profile/tools/moved_tsv.py`, one
cause rule per class, no per-line text) lists:

| Count | cp1 line | head line | Cause |
|---|---|---|---|
| 33 | `stop cpp-unclean: C2632 ...` | `reuse PortTop_Bound_<rva>` | bound_wins: raceload's C unit `Match/PortTop_<rva>.c` (BIND-CUNCLEAN-AS-C) |
| 1 | 0x00543bb0 `stop outside the bulk cp1 list ...` | `reuse PortTop_Bound_00543bb0` | exe body `Match/PortTop_00543bb0.cpp` (raceload, PORT-OT-RACE-BIND-PROV4 c74c6b11c, merged after cp1's base) |
| 1 | 0x005aba20 `stop` (C2) | `reuse PortTop_Bound_005aba20` | exe body `Match/PortTop_005aba20.cpp` (audio, RO-AUDIOINIT-45D460-REACH checkpoint 3, bb9990520, 2026-11-21 04:16, after cp1's 03:24 commit) |
| 1 | 0x0040cea0 `stop` (C2) | `stop outside the bulk cp1 list ..., source re/match/s02/0040cea0.c` | text only: hooks.csv now carries the row C4 `c4=frida-replay` with one re/match source |
| 1 | totals line | totals line | the counts after the 36 moves |

The lead's estimate of about 60 BIND-lane lines came from RO-AUDIOINIT-45D460-REACH-2's dry run of the old generator on
the head (`bulk_dry_newidx.txt`). That generator did not leave its own wrappers out of pass 1 and of `existing_stop`,
so on a tree that already holds them it turns its own binds into reuses (278 reuse / 226 stop / 1 bind). The profile
leaves them out, as cp1 did by deleting them first.

`write --profile bulk` on the head (only 3 files change, `git diff` on the branch):
- `Match/PortTop_005c91f0.cpp`: `#define FUN_005aba20 PortTop_Bound_005aba20   // exe body (Match/PortTop_005aba20.cpp)` (was `PortTop_BulkStop_005aba20`).
- `Match/PortTop_00543b90.cpp`: `FUN_00543bb0` -> `PortTop_Bound_00543bb0` (was `PortTop_BulkStop_00543bb0`).
- `Match/PortTop_BulkStops.cpp`: the 35 stops that became reuses are gone (no other exe TU names any of them, grep over
  `mashedmod/src/mashed_re`), 0x0040cea0's comment gains the source.
- `refs --profile raceload`: 0 files changed (132 anchors, BulkRefs IDENTICAL). `sync --profile bulk`: rsp + 0,
  allowlist + 0 (no new TU, no new rva-lint finding).

### 2c. The other profiles

`verify/bindprofile_20261121/tools/gen_dump.py` (plan, listing, every wrapper, stops, refs, selectors, BINDINGS.tsv,
`sync --dry`, `samesrc`, `prune` output), head binder against this branch's binder, on this tree:

| Profile | Before (head binder) | After (this binder) | |
|---|---|---|---|
| raceload (36 roots, --roots-only) | 349af334cfa70f5ca579692b29dc4426dd8ad61068d02103fa3f30bdbac7f846 | 349af334cfa70f5ca579692b29dc4426dd8ad61068d02103fa3f30bdbac7f846 | IDENTICAL |
| audio (63 plan roots) | 551dd8e9fe420dc9556fdf19324dca515e933c290c7901da06fe4509643e9068 | 551dd8e9fe420dc9556fdf19324dca515e933c290c7901da06fe4509643e9068 | IDENTICAL |

Both equal the BIND-PROFILE-DATA dumps (same sha256). raceframe / renderframe have no plan on this tree (their plans
live on their branches, as in BIND-PROFILE-DATA); their keys take the defaults of every new key, so their code path is
unchanged.

## 3. Selftest

`py -3.12 re/tools/bind/porttop_bind.py selftest`, head (`profile/selftest_head.txt`, sha256 equal to BIND-PROFILE-DATA's
`selftest_after.txt`) and this branch (`profile/selftest_after.txt`):

- bulk: `plan IDENTICAL (bind 97 reuse 216 stop 192), wrappers+stops IDENTICAL (98 checked)`;
  `listing vs verify/bindbulk_20261121/tools/last_plan.txt: 37 lines moved, all named in state/bulk/moved.tsv`.
- raceload, audio, BINDINGS.tsv, C emit, refs: the same lines as on the head.
- Total **FAIL 19** on both, the same 19 lines: the committed raceload and audio `plan.json` are stale against the
  head (8 + 14 plan rows, 4 + 13 files, listed in BIND-PROFILE-DATA section 3), not something this row changes. The
  binder writes the same files for them as before (2c). Refreshing those two plans is a `write` of their profiles, which
  rewrites 17 wrappers of the raceload and audio lineages: left to their next binding row, as BIND-PROFILE-DATA did.

## 4. Builds, object check, knob boot

__BUILDS__

G-CTRL, B and the 12x4 captures not run: top-down tool row (WORKER_RULES "Row kinds").

## 5. Notes

- The audio lineage has two generators on the head: `porttop_bind.py --profile audio` and
  `verify/portorigtop_20261113/tools/audiotick_bind_gen.py`, which RO-AUDIOINIT-45D460-REACH(-2) still runs. The latter's
  PROV_BIND gained 28 rvas (lines 40-51) that `state/_shared/rules.tsv` does not carry, which is one reason the audio
  `plan.json` is stale against the binder. Not changed here (an audio binding row's data step: add `prov_bind` lines to
  `state/audio/rules.tsv`).
- `bulk_bind_gen.py`, `regen.py`, `refs_gen.py` and the rest of `verify/bindbulk_20261121/tools/` are untouched history.

## STATE

Handoff for the LIVING-PC reboot (2026-11-21 11:50). Row not yet in review; verdict and section 4 pending.

- **Done and committed** (b105472f7, merge of orch/integration 6bb42c68a on top): binder change, `state/bulk/`, the 3
  regenerated files, cp1-tree and head checks (sections 1-3), skill text. `refs --profile raceload` and
  `sync --profile bulk` change nothing.
- **VS2022 build**: Build OK, rva-lint 465 allowlisted NEW=0, `mashed_re.exe` sha256
  e95c6397c6a6f8610ba18c8b9fc08010fa32e8c7d9e44d31838183a24c4b742b (log `D:/MashedWT/BIND-BULK-PROFILE_data/build_vs2022.log`).
- **Running at the warning**: the object build (`MASHED_DDKOBJ=1`, log `D:/MashedWT/BIND-BULK-PROFILE_data/build_ddkobj.log`),
  left to finish. The VS2022 knob boot was queued in the test slot memory wait and was stopped before it started (no
  game process launched). Private root ready: `D:/MashedWT/BIND-BULK-PROFILE_root`.
- **Next**: (1) if `build_ddkobj.log` does not end in `Build OK`, rerun the object build
  (`py -3.12 re/orchestrator/mashed_lock.py status` first; `cmd /c "set MASHED_NO_DEPLOY=1&& set MASHED_DDKOBJ=1&& set PYTHONUTF8=1&& mashedmod\build.bat"`);
  (2) `py -3.12 re/tools/ddkobj/ddkobj.py check`, compare `mashedmod/build/ddkobj/ddkobj_summary.json` and `BYTECHECK.tsv`
  with `C:\Users\maria\Desktop\Proyectos\Mashed\log\orch\run54_headcheck\`; expected changes only in units
  0x005c91f0 / 0x00543b90 (callee symbol) and none newly DIFFERENT; (3) knob boot:
  `PORTTOP_OUT=D:/MashedWT/BIND-BULK-PROFILE_data test-slot py -3.12 re/orchestrator/mashed_lock.py run --label BIND-BULK-PROFILE -- py -3.12 verify/porttop_20261108/knob_boot.py knob_vs D:/MashedWT/BIND-BULK-PROFILE_root 300 MASHED_ORIG_TOP=1 MASHED_OT_RACELOAD=1 MASHED_DET_FRAMES=3000`
  (background, 7200000 ms); (4) re-merge orch/integration, rerun `write --profile bulk` + `refs --profile raceload` +
  selftest, fill section 4 and the verdict, `roadmap.ps1 set BIND-BULK-PROFILE review -Evidence verify/bindbulk_20261121/RESULT_BIND-BULK-PROFILE.md`.

## Files

| File | sha256 |
|---|---|
| `profile/cp1_repro_cp1tree.txt` | 9d84e6f0a585b6bb4d8ba5c61766175d879197c22df5c183943e03a567367789 |
| `profile/cp1_repro_head.txt` | 442cda9ae274d6022c83d2e26010b3a19c9b3b5c38c93242b95f68ab0c56b5cd |
| `profile/selftest_head.txt` | 539faaa5889659b23c54c432b590b768d2d69688d7aa363d0d1973cc24e16ff1 |
| `profile/selftest_after.txt` | 5db1c3f64d160844db4ce00392b412f6de472c270c52ec75fffc116915acece9 |
| `profile/tools/cp1_repro.py` | e1a9040f580b5f0efeb22dc69f08e7deff320ef81aa3a491115f7487e15a24bb |
| `profile/tools/moved_tsv.py` | a36e3f73ece619665c1a88338a738f5f3169ec650344fb280d086883689308f5 |
| `re/tools/bind/state/bulk/moved.tsv` | 1d67999f3d1abe38c15ceb5271990d08c96179979fe55bc21f295b110e35bb0d |
| `re/tools/bind/state/bulk/plan.json` | 01560f59483b4db4e73193261b3ff2135ba6a7a9114e02c22293b017cb95f0d3 |
| `D:/MashedWT/BIND-BULK-PROFILE_data/{before,after}/raceload.txt` (raw) | 349af334cfa70f5ca579692b29dc4426dd8ad61068d02103fa3f30bdbac7f846 |
| `D:/MashedWT/BIND-BULK-PROFILE_data/{before,after}/audio.txt` (raw) | 551dd8e9fe420dc9556fdf19324dca515e933c290c7901da06fe4509643e9068 |
| `D:/MashedWT/BIND-BULK-PROFILE_data/build_vs2022.log` (raw) | c6456eaea0a3483b00626a83df97d0634ef62bdce025ffc0c4c6bb141da89508 |
