# RESULT BIND-PROFILE-DATA (run 54, section BIND, tool row)

Branch `orch/BIND-PROFILE-DATA` from `orch/integration` @ 77dedfece. Worker cmwqeaxvt723pnz1c8hlicd7i (claude2, Opus 5.5 medium).

## Verdict

GREEN. Every per-profile block of `re/tools/bind/porttop_bind.py` is now data under `re/tools/bind/state/`. The binder
reproduces all four profiles byte for byte before and after (raceload, audio on this tree; raceframe, renderframe on
their own branch trees), the selftest prints the same lines before and after, and no generated source changed. A binding
row now adds lines to data files only (KICKOFF_run54 rule 1).

## 1. What moved where

| Was in code (porttop_bind.py) | Now |
|---|---|
| `PROV_BIND`, `EXTRA_PRE`, `EXTRA_IN`, `LOCAL_SHARED`, `LOCAL_AS_BOUND`, `ALIAS` (shared tables, incl. PORT-TOP-06's `00401570` lines and BIND-CUNCLEAN-AS-C's three C-unit `extra_pre` lines) | `state/_shared/rules.tsv`, one rule per line (`<kind>\t<fields>`), comments kept |
| `PROFILES['audio']`, `PROFILES['raceload']` dicts | `state/<p>/profile.json` (settings) + `state/<p>/rules.tsv` (per-rva rules) |
| `PROFILES['raceframe']` (orch/PORT-TOP-06), `PROFILES['renderframe']` (orch/PORT-TOP-RENDERFRAME-492E90) | `state/raceframe/` and `state/renderframe/` `profile.json` + `rules.tsv` + `roots.txt` (their plan.json / stop_reasons.tsv / cpp_unclean.txt come with their branches) |
| `SELFTEST`, `CUNCLEAN_ROOTS`, `SELFTEST_ROOTS_ONLY` (BIND-CUNCLEAN-AS-C) | `state/raceload/roots.txt` + `profile.json` `selftest_roots_only`; `selftest_order`, `selftest_bindings` |
| `if self.name == 'raceload'` header, `RL_MARK` | `profile.json` `hdr` template (`%(mark)s %(rva)s %(src)s %(ev)s`) |
| audio header logic (three hard-coded root rvas, three text blocks) | `profile.json` `header` (style keep: `keep_roots`, `keep_roots_add`, `new`, `missing`). The old generator's third branch (`rva not in old_plan`) could not be reached (`table_new` covers it) and is dropped |
| `if self.name == 'audio'` stop text, `'PortTop_AStop_'` | `profile.json` `stops` (style `exit` + `head` lines), name from `stop_prefix` |
| `if self.name == 'raceload'` listing format | `profile.json` `listing` (`full` / `short`) |
| audio `stop_force` note text | `profile.json` `stop_force_note` |
| `'PORT-OT-RACE' not in rd(fn)` write refusal | `profile.json` `own_tag` |
| BINDINGS.tsv selftest hard-wired to audio | profiles with `selftest_bindings` |

The generic code the two unmerged branches added to the binder is folded in, driven by profile keys: `stop_reasons`
(file), `stop_why` (rule), `static_stop`, `group_same_source`, `held_external`, `thunk_names`,
`bindings_keep_missing`, `stops_subst`, `hdr`, `own_tag`, `root_external` + `write_selectors` (selector names and the
closure word from the profile: `sel_prefix` -> `PortTop_RRSelOn`, `closure`), and the wider `build_index` include
pattern (nested folders, `h2_<rva>.c`). New key `c_emit` (default true): raceframe and renderframe set it false, because
both were written before BIND-CUNCLEAN-AS-C and their plans keep their cpp-unclean rvas as named stops. Their owners flip
it when they want C units.

CLI: `plan` / `write` take `--roots` as before, and use `state/<p>/roots.txt` when it is left out. Profiles are the
folders under `state/` with a `profile.json`. An unknown rule kind or a wrong field count stops the tool with file:line.

## 2. Byte-for-byte check

Harness `verify/bindprofile_20261121/tools/gen_dump.py` dumps, without writing any file: the plan as built, the
listing, every bound wrapper's text, the stop file text, the refs text, the root_external selector writes (captured in
memory), BINDINGS.tsv after `bindings` (restored), the allow text, and the `sync --dry`, `samesrc`, `prune` output.

| Profile | Tree | Before (old binder) sha256 | After (data binder) sha256 | |
|---|---|---|---|---|
| raceload (36 roots, --roots-only) | this branch | 349af334cfa70f5ca579692b29dc4426dd8ad61068d02103fa3f30bdbac7f846 | 349af334cfa70f5ca579692b29dc4426dd8ad61068d02103fa3f30bdbac7f846 | IDENTICAL |
| audio (63 plan roots) | this branch | 551dd8e9fe420dc9556fdf19324dca515e933c290c7901da06fe4509643e9068 | 551dd8e9fe420dc9556fdf19324dca515e933c290c7901da06fe4509643e9068 | IDENTICAL |
| raceframe (004111c0) | `git archive orch/PORT-TOP-06` | 782338be08e11f88b7f0aaf406bfac12439cabcb1b9b0c7e982b4860e7702415 | 782338be08e11f88b7f0aaf406bfac12439cabcb1b9b0c7e982b4860e7702415 | IDENTICAL |
| renderframe (00492e90) | `git archive orch/PORT-TOP-RENDERFRAME-492E90` | cea766f6c0a3d2c48af98c16cd7f74fe80cb78451938794c3237f31b3e52f72f | cea766f6c0a3d2c48af98c16cd7f74fe80cb78451938794c3237f31b3e52f72f | IDENTICAL |

For the branch trees, "after" = the branch tree with this branch's `porttop_bind.py` and `state/*/profile.json`,
`rules.tsv`, `roots.txt`, `state/_shared/rules.tsv` copied over it. Dumps: `D:/MashedWT/BIND-PROFILE-DATA_data/{before,after}/`.

Tables: `verify/bindprofile_20261121/tools/cmp_tables.py` loads the old code tables and the new data. Against the base
binder every profile key and shared table is equal, except `EXTRA_PRE` / `EXTRA_IN` gaining exactly the `00401570` lines
of PORT-TOP-06. Against the two branch binders the raceframe / renderframe keys are equal (renderframe's
`root_external` gains the documentation key `_doc` and `closure`, which the code reads with the old literal as default).
Log: `cmp_tables_base.txt`.

## 3. Selftest

`py -3.12 re/tools/bind/porttop_bind.py selftest` before and after: the same lines, plus two new ones
(`raceframe: no plan.json, skipped`, `renderframe: no plan.json, skipped`, their plans live on their branches).
Before and after both end **FAIL 19**: the committed raceload and audio `plan.json` are stale against this head, not
something this row changed. raceload: 8 plan rows differ (00478660 / 0047a020 now `replay` not `provisional`, 5 stops
gain the `--roots-only` reason text, 005a7b60 is now a reuse of `Match/PortTop_005a7b60.cpp`) and 4 files; audio: 14
plan rows (005a6ea0 / 005a9f10 now replay, 12 stops now reuses of exe bodies another row bound) and 13 files.
BulkRefs IDENTICAL, C emit 34 units + fixture + 25 held units compile, BINDINGS.tsv IDENTICAL. The next raceload / audio
binding row re-runs `write` and commits the refreshed plans; this row leaves generated files alone.
Files: `selftest_before.txt`, `selftest_after.txt`.

## 4. merge=union or regenerate

Probe: `verify/bindprofile_20261121/tools/merge_probe.py` (output `merge_probe.txt`) builds synthetic ours / theirs from
the committed files: different appends, the same append, edit vs append, edit vs edit; `git merge-file --union` (what
`merge=union` does) against `tracker_merge3.py allow` (the train's keyed merge).

| File | Verdict | Why |
|---|---|---|
| `re/tools/bind/state/*/rules.tsv`, `roots.txt`, `stop_reasons.tsv`, `c_hold.txt`, `cpp_unclean.txt` | **merge=union** (added) | one record per line, read into sets / dicts; a duplicate line is harmless (same define twice, same set member) |
| `mashedmod/exe_sources.rsp` | **merge=union** (added) | the train already unions it (`merge_train.json` `conflict_union`); the attribute makes restacks do the same. Residual risk, same as today: a TU one side pruned next to a line the other side appended comes back, the build then fails on the missing file |
| `re/tools/dual_copy_allowlist.txt` | **merge=union** (added) | `lint_rva_bodies.py` `parse_allowlist` reads it into a dict by entry, every entry line is unique (0 duplicates). The train's keyed merge does **not** work on it today: `tracker_merge3.py allow` keys a line by its first token, and 463 lines start `ORIGTOP-ONLY`, so it exits 2 (`duplicate key ORIGTOP-ONLY`) on the committed file. A stale entry that union keeps is a WARN-free extra key, not a failure |
| `re/match/PROVISIONAL.tsv` | **not union: key-merge** | union keeps an edited row twice when the edit sits next to the other side's append (probe case 3: `0x00495870` twice, exactly the run-53 hand merge). `tracker_merge3.py allow` keys it right (first token = `0x<rva>`, header `rva`): case 3 clean, case 4 conflict as it should. Train change (lead / L0): add `"re/match/PROVISIONAL.tsv": "allow"` to `merge_train.json` `conflict_keyed` |
| `Match/PortTop_BulkRefs.cpp` | **regenerate** | the `/include` list must match the `PortTop_Keep_<rva>` anchors the wrappers define on the merged head (an rva that gained a bound caller in the other branch must drop out of both); union can leave an `/include` with no anchor (link error). Step: `py -3.12 re/tools/bind/porttop_bind.py refs --profile raceload` on the merged head (rewrites the anchors and the file from plan.json), then the object build |
| `Match/PortTop_*Stops.cpp` | **regenerate** | one function per stop with the callers in its comment; union of two plans gives two definitions of one stop or a stop that is now bound. Step: `py -3.12 re/tools/bind/porttop_bind.py write --profile <p>` on the merged head (roots from the union-merged `roots.txt`) |
| `state/*/plan.json`, `newfiles.txt`, the `Match/PortTop_<rva>.cpp` wrappers | **regenerate** | outputs of `write`; same step as the stops file |
| `verify/porttop_20261108/BINDINGS.tsv` | unchanged | already has the `genbindings` driver (HB-GENFILES-NOCONFLICT) |

Since each profile has its own plan / stop file, two binding rows on **different** profiles meet only in
`exe_sources.rsp`, the allowlist, BINDINGS.tsv, PROVISIONAL.tsv and `_shared/rules.tsv`, all covered above. Two rows on
the **same** profile also meet in its plan.json, stop file and (raceload) BulkRefs: run them one at a time, or let the
train run `write` + `refs` for that profile on the merged head and then both builds and `ddkobj.py check`. Both are
train / ledger rules for the lead and L0, not code here (merge_train.ps1 is a .ps1 and outside a worker's edit rights).

## 5. Builds, object check, knob boot

No generated source changed (`git status` after every step: only the tool, its data, `.gitattributes`, the skill
text and this folder), so both builds compile the same sources as the run 54 head. Every build / check / boot started
after `py -3.12 re/orchestrator/mashed_lock.py status` showed HELD by another row's game run, never PRIORITY-HOLD or SOLO.

| Step | Result |
|---|---|
| VS2022 build `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"` | Build OK, rva-lint 465 findings all allowlisted, NEW=0; `mashed_re.exe` sha256 cee9785623fb6877b5b2727f0aef6aee98e6076a16810b27f85f58eafdea3596 |
| Object build (`set MASHED_DDKOBJ=1`) | Build OK; `mashed_re_ddk.exe` sha256 a31c8ed1803f1843c50fd25aec6eec5dae6f807598d8fcbe4c791090bf7c42f0 |
| `py -3.12 re/tools/ddkobj/ddkobj.py check` | IDENTICAL 1976/2035, DIFFERENT 32, not-in-map 27, units linked 1966/2231, refusals 12 (externalized-static-register-convention 8, wrapper-redefines-globals 4) |
| against `log\orch\run54_headcheck\` | `ddkobj_summary.json` equal as JSON to the head's; `BYTECHECK.tsv` equal (sorted, CR stripped, md5 eaf7fca55dd287c255266002149a0510 both). **No newly DIFFERENT unit**, no listed unit fell back to its VS2022 body that did not already at the head |
| Knob boot, VS2022 exe, `MASHED_ORIG_TOP=1 MASHED_OT_RACELOAD=1 MASHED_DET_FRAMES=3000`, root `D:/MashedWT/BIND-PROFILE-DATA_root`, test-slot outside, mashed_lock inside, `verify/porttop_20261108/knob_boot.py` | rc 0 after 33.0 s, `WinMain 0x00492370 returned 0 after 3000 frames`. Only PID 20460 launched by this row. Ran locally (test slot waited 600 s for memory, then started under the cap). Out: `knob_boot.out`, log `D:/MashedWT/BIND-PROFILE-DATA_data/knob_vs.log` |
| Selftest | section 3 (same output before and after) |

G-CTRL, B and the 12x4 captures not run: tool row of the top-down kind, and the exe sources are unchanged.

## 6. Lead questions answered during the run

(1) Files: `porttop_bind.py`, `state/_shared/rules.tsv`, `state/{audio,raceload,raceframe,renderframe}/profile.json` +
`rules.tsv` (+ `roots.txt` for raceload, raceframe, renderframe), `.gitattributes`, `.claude/skills/roadmap-loop/SKILL.md`
(binder section). No generated file (BulkRefs, stop files, BINDINGS.tsv, exe_sources.rsp, allowlist) changed.
(2) The 0x005a7b60 (audio) and 0x0040cea0 (raceload, HANDLER) replay-C4 binds need no rule line: `msrc()` picks a
`c4=frida-replay` row from hooks.csv. They can run before this row merges if they touch no `porttop_bind.py` line. Note
from the selftest: 0x005a7b60 already plans as a reuse of `Match/PortTop_005a7b60.cpp` in both profiles on this head.

## 7. Merging the two unmerged branches after this row

orch/PORT-TOP-06 and orch/PORT-TOP-RENDERFRAME-492E90 each edit `porttop_bind.py`. When they restack onto an integration
that has this row, they take this branch's `porttop_bind.py` and `state/_shared/rules.tsv` whole (their code is in it,
section 2 proves the same output), and keep their own `state/<p>/plan.json`, `newfiles.txt`, `stop_reasons.tsv`,
`cpp_unclean.txt` and generated files. That restack is a git merge on their branch (lead).

## Files

| File | sha256 |
|---|---|
| `cmp_tables_all.txt` | f44e66626af8ef328b23fd457cdf470214b3c4f714847ac583e31106b335b66f |
| `cmp_tables_base.txt` | 00d1545b00692a22858eb98aa39768394985491f05bbe0f98550c7880e539251 |
| `ddkobj_check.log` | a02ee76c23b57331669c4c70cad1b0530fa896b219c4754d6ec736dce3519793 |
| `ddkobj_summary.json` | 430eac07fa11d0ebe23311ae60bec60ff8cfece0427e8ce7f1a45943cebdac05 |
| `knob_boot.out` | 17ee6bf64596ac6958ef95ff7d37ba8550957bc849f72d7c18fd161222d92a83 |
| `merge_probe.txt` | 4990472d3e5844633bb9ad6edbf848617be6d90939bdefed5cc18e4f944b37fa |
| `selftest_after.txt` | 539faaa5889659b23c54c432b590b768d2d69688d7aa363d0d1973cc24e16ff1 |
| `selftest_before.txt` | f57226e2410582455c6595d396e0325b6f9039b6cc916370e0e0a9c32f3dc52c |
| `tools/cmp_tables.py` | 61cbb7d60ba617928bf879e83e2d768aeeeb67e5f7aec8767982e5f4719b509a |
| `tools/gen_dump.py` | a05f87c2d95ca3d5c8705160b7f47b1ab0eac6c4315605bbb163664f91828817 |
| `tools/merge_probe.py` | 03629e02036a5ea8f7de76600a269a56d63e9429fbf95780d8686915778bd106 |
| `D:/MashedWT/BIND-PROFILE-DATA_data/before/raceload.txt` (raw, not committed) | 349af334cfa70f5ca579692b29dc4426dd8ad61068d02103fa3f30bdbac7f846 |
| `D:/MashedWT/BIND-PROFILE-DATA_data/before/audio.txt` (raw, not committed) | 551dd8e9fe420dc9556fdf19324dca515e933c290c7901da06fe4509643e9068 |
| `D:/MashedWT/BIND-PROFILE-DATA_data/before/raceframe.txt` (raw, not committed) | 782338be08e11f88b7f0aaf406bfac12439cabcb1b9b0c7e982b4860e7702415 |
| `D:/MashedWT/BIND-PROFILE-DATA_data/before/renderframe.txt` (raw, not committed) | cea766f6c0a3d2c48af98c16cd7f74fe80cb78451938794c3237f31b3e52f72f |
| `D:/MashedWT/BIND-PROFILE-DATA_data/after/raceload.txt` (raw, not committed) | 349af334cfa70f5ca579692b29dc4426dd8ad61068d02103fa3f30bdbac7f846 |
| `D:/MashedWT/BIND-PROFILE-DATA_data/after/audio.txt` (raw, not committed) | 551dd8e9fe420dc9556fdf19324dca515e933c290c7901da06fe4509643e9068 |
| `D:/MashedWT/BIND-PROFILE-DATA_data/after/raceframe.txt` (raw, not committed) | 782338be08e11f88b7f0aaf406bfac12439cabcb1b9b0c7e982b4860e7702415 |
| `D:/MashedWT/BIND-PROFILE-DATA_data/after/renderframe.txt` (raw, not committed) | cea766f6c0a3d2c48af98c16cd7f74fe80cb78451938794c3237f31b3e52f72f |
| `D:/MashedWT/BIND-PROFILE-DATA_data/knob_vs.log` (raw, not committed) | e74011402ad4204352d2200af936d6fe5c7eb884e75000e05aa06d0b524a8277 |
| `D:/MashedWT/BIND-PROFILE-DATA_data/build_vs2022.log` (raw, not committed) | 93d3816e9cc62132a69d8d2ca3efbcb0e62989b438d8ce5419b98e0798693da5 |
| `D:/MashedWT/BIND-PROFILE-DATA_data/build_ddkobj.log` (raw, not committed) | 14aba4d419182cea5de2c7e552d84d56e179fb9e6726dfddcb78f58d0270b83a |
