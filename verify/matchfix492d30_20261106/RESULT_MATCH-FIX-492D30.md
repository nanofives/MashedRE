# RESULT MATCH-FIX-492D30 (2026-11-06)

**Verdict: GREEN.** The byte-matched source `re/match/s15/00492d30.c` is now the only body of 0x00492d30
GameTickStateMachine7 (in the .asi, the only target that compiled the old body). No C-level moves (C2 stays:
exe-callers=0, L-68). The default gates equal `re/orchestrator/merge_train.json` on exe `284090b6`:
G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3.

Branch `orch/MATCH-FIX-492D30`, cut from orch/integration `db86c66b5`.

## The port defect (from verify/matchs15_20261102/RESULT_MATCH-S15.md)

After the entry calls (0x004950b0, 0x0045b350, 0x005c9d00) the original switches on `DAT_00771968` and reaches one of
three join points: `A` = call 0x0043dfd0 then fall into `B`, `B` = call 0x0043d7c0 then fall into `T`, `T` = the tail
(0x00496920(5) gate, 0x004671a0/0x00498860, `DAT_007719cc`, `DAT_007719a8`, return 1).

| case | old port body (Util/UtilMid.cpp) | original / matched source |
|---|---|---|
| 1 | 0x0043dfd0, 0x0043d7c0, tail | same (break to `A`) |
| 2 | 0x0043dfd0, 0x0043d7c0, tail | tail only (`T`) |
| 3 | loop 0x004111c0(0x32), 0x0043dfd0, 0x0043d7c0, tail | loop 0x004111c0(0x32), tail only (`T`) |
| 4 | 0x0043dfd0, 0x0042b930 test, tail | 0x0043dfd0, 0x0042b930 test, 0x0043d7c0, tail (`B`) |
| 5 | tail only | 0x0043dfd0, 0x0043d7c0, tail (break to `A`) |
| 6 | loop 0x004111c0(0x32), tail only | loop, 0x0043dfd0, 0x0043d7c0, tail (break to `A`) |
| 7 | tail only | 0x0043d7c0, tail (`B`) |
| default | 0x0043dfd0, 0x0043d7c0, tail | tail only (`T`) |

Only case 1 agreed. The tail itself was the same.

## Swap

- `re/match/s15/00492d30.c`: the two callee prototypes g2s wrote as `(void)` now take the one argument every call
  passes: `void FUN_004111c0(uint p0);` (the t01g matched source declares `void FUN_004111c0(uint param_1)`) and
  `void FUN_00498860(undefined4 p0);`. C accepted the old form with a warning, C++ rejects it. Body unchanged. match.py
  re-run below: still 100%.
- `mashedmod/src/mashed_re/Match/MatchS15_00492d30.cpp` (new): `#define FUN_00492d30 GameTickStateMachine7`, includes
  the matched source under `MATCH_PORT`, exports `GameTickStateMachine7`. The eleven callees (0x004030d0, 0x004111c0,
  0x0042b930, 0x0043d7c0, 0x0043dfd0, 0x0045b350, 0x004671a0, 0x004950b0, 0x00496920, 0x00498860, 0x005c9d00) bind to
  raw-address forwarders, as the old body called them (precedent MATCH-FIX-4926C0).
- Install stays mass-disabled (the old TU had only the `MASS-DISABLED 2026-05-24` comment), so the wrapper has no
  install. x87: no (integer only).
- `Util/UtilMid.cpp`: old body and its anonymous-namespace RVA constants removed, pointer comment left.
- `mashedmod/asi_sources.rsp`: wrapper added after `Match\MatchS15_00492d20.cpp`. Not in `exe_sources.rsp`:
  `Util/UtilMid.cpp` was never in the exe, so the exe gets no body for this RVA (same as before).

## Checks

| check | result |
|---|---|
| `py -3.12 re/tools/match/match.py re/match/s15/00492d30.c` | `0x00492d30 _FUN_00492d30: 100.0% (regs 100.0%, ops 100.0%)  81/81 ins  flags=/O2 /GS /Ob1 tc=ddk` |
| `py -3.12 re/tools/match/relocheck.py re/match/s15/00492d30.c` | `0x00492d30 OK checked=25 unchecked=0` |
| matched source sha256 (working copy, LF) | `3d65ee4a8b9be3d5028c0a44c47cac6f6f8d39c416fdb5211a2fe0f662cfe535` (S15 copy before the prototype fix: `244657c162066a0f8aa2738b1e3c1ee197551c81d2e2200a1e7eb8257e673ca4`) |
| `rva_body_scan` bodies of 0x00492d30 | exe: none. asi: 1, `Match/MatchS15_00492d30.cpp` (`FUN_00492d30` line 62). Active installs: none |
| `py -3.12 scripts/lint_rva_bodies.py` | `1 finding(s): CROSS-TARGET=1 \| allowlisted=1 NEW=0` |
| `py -3.12 scripts/lint_inline_asm_abs.py` | OK (1426 files) |
| build (`MASHED_NO_DEPLOY=1 mashedmod\build.bat`) | Build OK, no warning from the new TU. exe sha256 `284090b6f3b0d0501382b5ae0464d8db7575593266e628f96fd8ceb1222870ce`, asi `a60b5c99499229c5c87e1a3c2203baef10e932b241fa829fb4ebafdea158192f`. Log: D:/MashedWT/MATCH-FIX-492D30_data/build.log |
| `GameTickStateMachine7` export in the asi | present |
| port callers of `GameTickStateMachine7` | 0 (grep of `mashedmod/src`: definitions and comments only) |

## Default gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, root `D:/MashedWT/MATCH-FIX-492D30_root`,
`A4_OUT` = `D:/MashedWT/MATCH-FIX-492D30_data/arms`, under test-slot + `mashed_lock.py run --label MATCH-FIX-492D30`.
Every arm self-exited 0 (H_DEF 632 s, B arms 42-44 s). Log: D:/MashedWT/MATCH-FIX-492D30_data/gates.log. The run
queued about 65 min (memory, all 4 test slots, then the merge-train 13-track solo window). Refs re-read from
orch/integration `22ffd015a` after the run: unchanged.

| arm file | sha256 | ref (merge_train.json) |
|---|---|---|
| D:/MashedWT/MATCH-FIX-492D30_data/arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 | 8284be46 |
| D:/MashedWT/MATCH-FIX-492D30_data/arms/H_DEF.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c | 67931834 |
| D:/MashedWT/MATCH-FIX-492D30_data/arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 | 31fe043e |
| D:/MashedWT/MATCH-FIX-492D30_data/arms/B_DEF_K0.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 | 1d5bf4f9 |
| D:/MashedWT/MATCH-FIX-492D30_data/arms/B_DEF_K1.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 | 1d5bf4f9 |
| D:/MashedWT/MATCH-FIX-492D30_data/arms/B_DEF_K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 | 1d5bf4f9 |

Gate hashes unchanged, as expected: `Util/UtilMid.cpp` and the new wrapper are .asi-only, the body has no exe
caller and no install, so the default build never runs it. No L-n flip decision is needed.

## Trackers (re-classify)

0x00492d30 C2->C2, no change (L-68): match evidence identical, swapped, exe-callers=0. Row name `FUN_00492d30` ->
`GameTickStateMachine7`, status `mapped` -> `impl`, `file` -> `Match/MatchS15_00492d30.cpp` (was the analysis note),
`exe_file` stays empty (no exe body). C4 when a caller lands and the install (or an exe call site) runs the body.
The analysis note `re/analysis/util_c0_promote/0x00492d30.md` keeps three unfiled `[UNCERTAIN]` lines (state-value
meaning, 0x0043dfd0 semantics, 0x005c9d00 purpose). Not filed here: no level moved, so the sweep gate did not run.
