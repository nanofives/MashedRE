# RESULT MATCH-FIX-4950B0 (2026-11-06)

**Verdict: GREEN.** The byte-matched source `re/match/s16/004950b0.c` is now the only body of 0x004950b0
QpcTimeScaledTo3Mhz. It is compiled into the .asi only, the one target that compiled the old body. No C-level moves:
C2 stays (exe-callers=0, L-68). The default gates equal `re/orchestrator/merge_train.json` (orch/integration
`51e8a7a30`, read after the run) on exe `8b04acc8`: G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3.

Branch `orch/MATCH-FIX-4950B0`, cut from orch/integration `22ffd015a`. The PC rebooted mid-step (handoff:
`HANDOFF_MATCH-FIX-4950B0.md`). Nothing was running when it went down.

## The port defect (from verify/matchs16_20261102/RESULT_MATCH-S16.md)

| | old port body (Util/UtilMid.cpp) | original / matched source |
|---|---|---|
| product | `uint64 qpc * 3000000u` | signed `__int64` `__allmul` |
| divide | `uint64 / uint64` | signed `__alldiv` by `__int64` at 0x00771e70 |
| freq == 0 | returns 0 | no guard (`__alldiv` divide by zero raises #DE) |
| return | low 32 bits | low 32 bits (`t.s.LowPart`) |

Signed and unsigned results differ once `qpc * 3000000` passes 2^63. At a 10 MHz QPC that is about 85 h of uptime.

## Zero-freq guard: dropped (matches the original). Recommendation: keep it dropped.

The matched body has no guard. I kept it that way, so the port has no deviation. The question for the record:

- Option (a), **recommended and implemented**: no guard, the original's form. The QPF frequency at 0x00771e70/74 is
  written by 0x00495120 TimerQpfStore. The original divides by it with no check, so any call the original makes with
  freq == 0 raises #DE, and the original boots and races. So the calls the original makes never reach freq == 0, and
  the port body only runs in place of those calls. The only known freq == 0 context is a suspended-spawn Frida
  harness (before 0x00495120 runs). `re/frida/hooks_registry.py:706` already seeds 0x00771e70 non-zero for it.
- Option (b): keep a freq == 0 -> 0 guard as a port-safety deviation, with a check in the wrapper before the matched
  call. This is not the original's behaviour, and nothing that runs needs it.

## Swap

- `mashedmod/src/mashed_re/Match/MatchS16_004950b0.cpp` (new): `#define FUN_004950b0 QpcTimeScaledTo3Mhz`, includes
  the matched source under `MATCH_PORT` inside `extern "C"`, exports `QpcTimeScaledTo3Mhz`. No windows.h: the
  matched source declares `QueryPerformanceCounter` itself (same kernel32 import, `LargeInt*` argument). No callees
  besides `QueryPerformanceCounter` and the CRT `__allmul` / `__alldiv`. The install stays mass-disabled, as in the old
  TU (`MASS-DISABLED 2026-05-24 phase-a2-no-registry-deferred`). Integer only, no x87.
- `Util/UtilMid.cpp`: old body and its two RVA constants removed. A pointer comment is left there.
- `mashedmod/asi_sources.rsp`: wrapper added after `Match\MatchS16_00495080.cpp`. Not in `exe_sources.rsp`, because
  `Util/UtilMid.cpp` was never in the exe. The exe's `Match/MatchS06_00428a30.cpp` keeps its raw-address call to
  0x004950b0. In the .asi, `MatchS06_00428a30.cpp` binds `FUN_004950b0` to `QpcTimeScaledTo3Mhz`, now the matched body.

## Checks

| check | result |
|---|---|
| `py -3.12 re/tools/match/match.py re/match/s16/004950b0.c` | `0x004950b0 _FUN_004950b0: 100.0% (regs 100.0%, ops 100.0%)  23/23 ins  flags=/O2 /GS /Ob1 tc=ddk` |
| `py -3.12 re/tools/match/relocheck.py re/match/s16/004950b0.c` | `0x004950b0 OK checked=2 unchecked=3` (`__alldiv`, `__allmul`, `__imp__QueryPerformanceCounter@4`) |
| matched source sha256 (unchanged from S16) | `41f7ebf41440edec5a8eb2cf4dcd9e30756636642d03c12dae734bd6ccfd458b` |
| `rva_body_scan` bodies of 0x004950b0 | exe: none. asi: 1, `Match/MatchS16_004950b0.cpp`. Active installs: none |
| `py -3.12 scripts/lint_rva_bodies.py` | `1 finding(s): CROSS-TARGET=1 \| allowlisted=1 NEW=0` |
| `py -3.12 scripts/lint_inline_asm_abs.py` | OK (1427 files) |
| build (`MASHED_NO_DEPLOY=1 mashedmod\build.bat`) | Build OK, no warning from the new TU. exe sha256 `8b04acc87d3591c48929090080bb5c522345214ad5499989c17a756a23dc2310`, asi `506b7b8b0fce8a1c040dcb11355b799bccb3cee6407aaa1117c8b9851c862d88`. Log: D:/MashedWT/MATCH-FIX-4950B0_data/build.log |
| `QpcTimeScaledTo3Mhz` export in the asi | present (pefile) |

## Default gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, root `D:/MashedWT/MATCH-FIX-4950B0_root`,
`A4_OUT` = `D:/MashedWT/MATCH-FIX-4950B0_data/arms`, under test-slot + `mashed_lock.py run --label MATCH-FIX-4950B0`.
Every arm self-exited 0 (H_DEF 246 s, B arms 22 s each). Log: D:/MashedWT/MATCH-FIX-4950B0_data/gates.log.

| arm file | sha256 | ref |
|---|---|---|
| D:/MashedWT/MATCH-FIX-4950B0_data/arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 | 8284be46 |
| D:/MashedWT/MATCH-FIX-4950B0_data/arms/H_DEF.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c | 67931834 |
| D:/MashedWT/MATCH-FIX-4950B0_data/arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 | 31fe043e |
| D:/MashedWT/MATCH-FIX-4950B0_data/arms/B_DEF_K0.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 | 1d5bf4f9 |
| D:/MashedWT/MATCH-FIX-4950B0_data/arms/B_DEF_K1.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 | 1d5bf4f9 |
| D:/MashedWT/MATCH-FIX-4950B0_data/arms/B_DEF_K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 | 1d5bf4f9 |

The gate hashes did not change, as expected: the new wrapper is .asi-only, and the exe has no body and no caller change.

## Trackers (re-classify)

0x004950b0 C2->C2, no change (L-68). Match evidence: identical, swapped, exe-callers=0. Row name `FUN_004950b0` ->
`QpcTimeScaledTo3Mhz`, status stays `impl`, `file` -> `Match/MatchS16_004950b0.cpp` (was the analysis note),
`exe_file` stays empty. The analysis note `re/analysis/timer/0x004950b0.md` has only filed markers (U-0668, U-0669),
and the body has no stubs. Expect a tracker/rsp conflict with MATCH-FIX-492D30 on landing (same UtilMid.cpp,
neighbouring rsp and hooks.csv lines). It is a text conflict only, no code overlap: 492D30 removes the 0x00492d30
block of UtilMid.cpp, and this branch removes the 0x004950b0 block.
