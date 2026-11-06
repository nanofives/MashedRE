# RESULT MATCH-FIX-4926C0 (2026-11-06)

**Verdict: GREEN.** The byte-matched source `re/match/s15/004926c0.c` is now the only body of 0x004926c0
AudioTickAndAvg in both targets, and the default gates equal `re/orchestrator/merge_train.json` on exe `7bb1d886`:
G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3. No C-level moves (C2 stays: exe-callers=0, L-68).

Branch `orch/MATCH-FIX-4926C0`, cut from orch/integration `ac2e9ed63`.

## The port defect (from verify/matchs15_20261102/RESULT_MATCH-S15.md)

| | old port body (Boot/FrameDispatch.cpp) | original / matched source |
|---|---|---|
| button 4 | `if FUN_00496930(4) { if DAT_006147b8 { FUN_0045d3f0(0); FUN_00466b50(0); FUN_0045d430() } else DAT_006147b8 = 0 }` | `if FUN_00496930(4) DAT_006147b8 = 0; else if DAT_006147b8 { FUN_0045d3f0(0); FUN_00466b50(0); FUN_0045d430() }` |
| 60-frame average | `DAT_0077197c = (uint32)DAT_00771994 / 60u` | `DAT_0077197c = (int)DAT_00771994 / 60` (signed) |

The old body never ran the three callees with button 4 up, and ran them with button 4 down.

## Swap

- `mashedmod/src/mashed_re/Match/MatchS15_004926c0.cpp` (new): `#define FUN_004926c0 AudioTickAndAvg`, includes the
  matched source under `MATCH_PORT`, exports `AudioTickAndAvg`. The five callees (0x004950b0, 0x00496930, 0x0045d3f0,
  0x00466b50, 0x0045d430) bind to raw-address forwarders, as the old body called them (S15 precedent for 0x00492770).
- Install stays mass-disabled (the old TU carried only the `MASS-DISABLED 2026-05-24` comment, no active
  `RH_ScopedInstall`), so the wrapper has no install. x87: no (integer only).
- `Boot/FrameDispatch.cpp`: old body, its five static callee pointers and four address constants removed (none had
  another user in the TU), replaced by a pointer comment.
- `mashedmod/asi_sources.rsp`, `mashedmod/exe_sources.rsp`: wrapper added after `Match\MatchS15_00492770.cpp`.

## Checks

| check | result |
|---|---|
| `py -3.12 re/tools/match/match.py re/match/s15/004926c0.c` | `0x004926c0 _FUN_004926c0: 100.0% (regs 100.0%, ops 100.0%) 47/47 ins flags=/O2 /GS /Ob1 tc=ddk` |
| `py -3.12 re/tools/match/relocheck.py re/match/s15/004926c0.c` | `0x004926c0 OK checked=17 unchecked=0` |
| matched source sha256 | `85c3534ca4888effea3809709960f988106f5c47eba5a887f318e2dc93432147` (unchanged from S15) |
| `rva_body_scan` bodies of 0x004926c0 | 1: `Match/MatchS15_004926c0.cpp`; active installs: none |
| `py -3.12 scripts/lint_rva_bodies.py` | `1 finding(s): CROSS-TARGET=1 \| allowlisted=1 NEW=0` |
| `py -3.12 scripts/lint_inline_asm_abs.py` | OK (1426 files) |
| build (`MASHED_NO_DEPLOY=1 mashedmod\build.bat`) | Build OK, exe sha8 `7bb1d886` |
| port callers of `AudioTickAndAvg` | 0 (grep of `mashedmod/src`: comments only) |

## Default gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, root `D:/MashedWT/MATCH-FIX-4926C0_root`,
under test-slot + `mashed_lock.py run --label MATCH-FIX-4926C0`. Every arm self-exited 0.

| arm file | sha256 | ref (merge_train.json) |
|---|---|---|
| D:/MashedWT/MATCH-FIX-4926C0_data/arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 | 8284be46 |
| D:/MashedWT/MATCH-FIX-4926C0_data/arms/H_DEF.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c | 67931834 |
| D:/MashedWT/MATCH-FIX-4926C0_data/arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 | 31fe043e |
| D:/MashedWT/MATCH-FIX-4926C0_data/arms/B_DEF_K0.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 | 1d5bf4f9 |
| D:/MashedWT/MATCH-FIX-4926C0_data/arms/B_DEF_K1.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 | 1d5bf4f9 |
| D:/MashedWT/MATCH-FIX-4926C0_data/arms/B_DEF_K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 | 1d5bf4f9 |

Gate hashes unchanged, as expected: the body has no exe caller and no install, so the default build never runs it.
No L-n flip decision is needed.

## Trackers (re-classify)

0x004926c0 AudioTickAndAvg C2->C2, no change (L-68): match evidence identical, swapped, exe-callers=0. `file` moves
to `Match/MatchS15_004926c0.cpp`. C4 when a caller lands and the install (or an exe call site) runs the body.
