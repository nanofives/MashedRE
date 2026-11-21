# RESULT BIND-TOOL (run 53, 2026-11-21)

Verdict: **GREEN**. Row kind: top-down tooling (no exe source changed). Branch `orch/BIND-TOOL` on `orch/integration` b24bba46c.

## Deliverable

- `re/tools/bind/porttop_bind.py`: one binder with subcommands `plan`, `write`, `externs` (extern-guard step), `sync`
  (rsp + allowlist), `unclean` (cpp-unclean list from a VS2022 log), `bindings` (BINDINGS.tsv), `samesrc`, `prune`,
  `index`, `selftest`. It replaces `verify/ro_20261116/rltools/rl_bind_gen.py`, `verify/portorigtop_20261113/tools/audiotick_bind_gen.py`,
  `verify/portorigtop_20261113/tools/bindings_update.py` and their helpers (`exe_body_index.py`, `sync_rsp_allowlist.py`,
  `rl_sync.py`, `guard_externs.py`, `unclean_from_log.py`, `same_source_exe.py`, `prune_allowlist.py`). The old scripts are
  left in place as the history of their rows and are not to be run any more.
- Two profiles carry the old rule tables unchanged: `raceload` (race-load closure of 0x0040d270) and `audio`.
- State moved to `re/tools/bind/state/<profile>/` (`plan.json`, `newfiles.txt`, `cpp_unclean.txt`, copied from the verify folders).
- Usage: `.claude/skills/roadmap-loop/SKILL.md`, section "Binding checkpoints", a 9-step checkpoint recipe for RACE-BIND-C4-BULK.

Fixes against the old scripts, each found by the self-test or the reproduction:
1. **Index of own wrappers.** `rl_bind_gen.py` read `rltools/exeidx.json`, which was not committed. A fresh index of the
   current tree lists the race-load wrappers as exe bodies, and the old generator then planned 37 reuse / 6 stop / 4 bind
   instead of 92 / 45 / 33 (and its writer would have deleted the 88 wrappers no longer planned as bind). The tool leaves the
   profile's own wrappers (header `// PORT-OT-RACE (2026-11-16): exe-only port body`) out of the index.
2. **CRLF-only rewrites.** Files are written only when the content differs modulo CRLF (the KNOB RESULT's "revert CR-only rewrites" step is no longer needed).
3. **prune** keeps file-local functions of bound sources (PORT-OT-RACE ck3 put 3 pruned lines back by hand).
4. **externs** reads sources byte-preserving (some are cp1252) and skips declarations already inside a `MATCH_PORT` /
   `MASHED_RE_PORT` conditional.

## Self-test (`py -3.12 re/tools/bind/porttop_bind.py selftest`)

```
[selftest] raceload roots 2: plan IDENTICAL (bind 92 reuse 45 stop 33), wrappers+stops IDENTICAL (93 checked)
[selftest] audio roots 63: plan IDENTICAL (bind 384 reuse 71 stop 88), wrappers+stops IDENTICAL (385 checked)
[selftest] BINDINGS.tsv from the audio plan: IDENTICAL (updated 0 added 0)
[selftest] PASS
```

Rebind of PORT-OT-RACE checkpoint 3 from scratch: the 92 race-load wrappers and `Match/PortTop_RaceLoadStops.cpp` were
deleted, then `write --profile raceload --roots 00426e10,004264d0` regenerated them. `git diff` (content) empty for all 93
files. `state/raceload/plan.json` byte-identical to `verify/ro_20261116/rltools/plan_rl.json`, `newfiles.txt` byte-identical
to `newfiles_rl.txt`. The listing matches `rltools/plan_64d0.txt` line for line, apart from the codepage of the 4 non-ASCII
compiler messages in the cpp-unclean reasons (old listing was a cp850 stdout redirect). `sync --dry`: rsp + 0, allowlist + 0.
`samesrc`: the same 7 rvas as RESULT_PORT-OT-RACE 7.1. `prune` (dry): 0 lines.

Extern-guard reproduction: the 8 sources guarded at PORT-OT-RACE ck2 (commit 4952d4280), taken at their parent revision and
guarded by the tool, came out byte-identical to the committed versions: **8/8 IDENTICAL**. `externs` on both current plans: 0
unguarded sources.

BINDINGS.tsv: the audio plan reproduces the committed file (0 updated, 0 added). `bindings --profile raceload --dry` would
update 1 and add 131 rows (the race-load closure was never written to BINDINGS.tsv). Not applied here: the first
RACE-BIND-C4-BULK checkpoint on the tool should apply it with its own `--step`.

Not exercised: `unclean` on a real build log (no compile error in this build). Its regex and file format are the old
`unclean_from_log.py`'s.

## Builds and ddkobj

Status printed before each step (`mashed_lock.py status`, `proyectos_test_slot.py --status`):

| step | time | mashed_lock | test slots | commit |
|---|---|---|---|---|
| VS2022 build | 01:52:37 | free | 2 of 8 held (Simcity) | 23.9 GB |
| object build | 01:55:44 | HELD by PROV-INSITU-REF1 (game run, not PRIORITY-HOLD / SOLO) | 3 held | 24.3 GB |
| ddkobj check | 02:00:55 | free | 2 held | 24.5 GB |
| knob boot | before launch | free | waited 420 s for memory, then ran | 20.5-25.3 GB |

Both builds started after another session's object build (PID 4152) ended.

| check | result |
|---|---|
| VS2022 build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, `mashed_re.exe` SHA-256 `c75426b6...` |
| object build (`MASHED_DDKOBJ=1`) | `=== Build OK ===`, `mashed_re_ddk.exe` SHA-256 `f11af0f8...` |
| `ddkobj.py check` | **IDENTICAL 1765/1812**, DIFFERENT 28, not-in-map 19, units linked 1759/1991, refusals 11 (externalized-static-register-convention 7, wrapper-redefines-globals 4). Same IDENTICAL count and DIFFERENT 28 as RESULT_PORT-TOP-AUDIO-5AEF30-KNOB (the tree's exe sources are unchanged by this row) |
| race-load bound units in `BYTECHECK.tsv` | 92 of 92 IDENTICAL |
| knob boot (`MASHED_ORIG_TOP=1 MASHED_OT_RACELOAD=1 MASHED_DET_FRAMES=3000`, root `D:/MashedWT/BIND-TOOL_root`) | rc 0 after 33.9 s, `WinMain 0x00492370 returned 0 after 3000 frames` |

No G-CTRL, no B, no 12x4 captures (top-down row). No menu code changed, so no menu walk. `original\mashed_re_dev.asi` not touched.
Only PID 1660 (knob boot) was launched by this row.

## Raw evidence (`D:/MashedWT/BIND-TOOL_data`)

| file | SHA-256 |
|---|---|
| build_vs2022.log | 5e2ff7f84ac99a539d9d5a886daf301a7c73cf792fa23b18ba4d5b2db4684db7 |
| build_ddkobj.log | ca0f4ba37770a754491b43c0b02eb7db7f38f7c34799f9f0f2bc98352d1780d5 |
| ddkobj_check.log | 28e089ace54dd99ead1e1053cc9ddc8c62960d9da87324404deaa620a9ad8aac |
| BYTECHECK.tsv | c1589114b7d80937ba0b4d9df0dd57e28efc0ea5d951fe4e77fd9d24269778e4 |
| knob_rl_on.log | 0a907ba81f6e3cad76a2f1498a5fa0fae5b459e128035d209242f0a0538b7877 |
| knob_boot.out | 02a5228360bd7c2a294e2175e45b9e11e2419b6488f55153aba2d279477f2bba |
| plan_raceload_ck3.txt | aadc2a523d48e2bd7a40f459fd55099094b7a133704bc3cb31b7042614966238 |

## Next

RACE-BIND-C4-BULK checkpoints 2 and 3: `porttop_bind.py plan/write --profile raceload --roots 00426e10,004264d0,<new C4 roots>`,
then the SKILL.md steps 4 to 9 (externs, sync with the row's `--step` / `--evidence`, build, unclean, prune, bindings, ddkobj check).
