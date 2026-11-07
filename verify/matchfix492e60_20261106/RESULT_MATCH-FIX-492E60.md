# RESULT MATCH-FIX-492E60 (2026-11-06)

**Verdict: GREEN.** The byte-matched source `re/match/s15/00492e60.c` is now the only body of 0x00492e60
SetDefaultViewWindow in both targets, and the default gates equal `re/orchestrator/merge_train.json` on exe `564e940d`:
G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3. No C-level moves (C3 stays: exe-callers=0, L-68).

Branch `orch/MATCH-FIX-492E60`, cut from orch/integration `db86c66b5`.

## The port defect (from verify/matchs15_20261102/RESULT_MATCH-S15.md)

| | old port body (Boot/SubsystemInit.cpp) | original / matched source |
|---|---|---|
| FUN_004671a0 | `FUN_004671a0(0xffffffff, &v)` (2 args) | `FUN_004671a0(-1)` (1 arg, `add esp,4`) |
| FUN_004c1c80 | `FUN_004c1c80(handle)` (1 arg) | `FUN_004c1c80(handle, v)` (int, float*) |

`v` = `{0.8f, 0.8f}` (`0x3f4ccccd`). The old body handed the pair to the wrong callee and gave FUN_004c1c80 no
second argument.

## Swap

- `mashedmod/src/mashed_re/Match/MatchS15_00492e60.cpp` (new): `#define FUN_00492e60 SetDefaultViewWindow`, includes
  the matched source under `MATCH_PORT`, exports `SetDefaultViewWindow`. The two callees (0x004671a0, 0x004c1c80) bind
  to raw-address forwarders, as the old body called them (MATCH-FIX-4926C0 precedent).
- Install stays mass-disabled (the old TU carried only the `MASS-DISABLED 2026-05-24` comment), so the wrapper has no
  install. x87: no (two float stores, no arithmetic).
- `Boot/SubsystemInit.cpp`: old body and its two static callee pointers (`s_FUN_004671a0_2`, `s_FUN_004c1c80`, no
  other user in the TU) removed, replaced by a pointer comment. Header line updated.
- `mashedmod/asi_sources.rsp` (after `MatchS15_00492d10.cpp`), `mashedmod/exe_sources.rsp` (after
  `MatchS15_00492770.cpp`): wrapper added.

## Checks

| check | result |
|---|---|
| `MSYS_NO_PATHCONV=1 py -3.12 re/tools/match/match.py re/match/s15/00492e60.c` | `0x00492e60 _FUN_00492e60: 100.0% (regs 100.0%, ops 100.0%) 12/12 ins flags=/O2 /GS /Ob1 tc=ddk` |
| `py -3.12 re/tools/match/relocheck.py re/match/s15/00492e60.c` | `0x00492e60 OK checked=2 unchecked=0` |
| matched source sha256 | `410a032f04b0c0db1441ba86221ac0dde6779a7bf5a0eb9fe0164a805705d1f6` (unchanged from S15) |
| `py -3.12 scripts/lint_rva_bodies.py` | `1 finding(s): CROSS-TARGET=1 \| allowlisted=1 NEW=0` |
| `py -3.12 scripts/lint_inline_asm_abs.py` | OK (1426 files) |
| build (`MASHED_NO_DEPLOY=1 mashedmod\build.bat`) | Build OK, exe + asi, `original\mashed_re_dev.asi` not deployed, exe sha8 `564e940d` |
| port callers of `SetDefaultViewWindow` | 0 (grep of `mashedmod/src`: comments only) |
| raw-address callers of 0x00492e60 | `Frontend/MenuModal.cpp:221` (`s_FUN_00492e60`), asi only (not in exe_sources.rsp), reaches the original since the install is disabled |

## Exe callers (L-68)

`py -3.12 verify/matchs11_20261102/tools/exe_callsites.py mashed_re.exe mashed_re.map SetDefaultViewWindow
MatchS15_00492e60_Call004671a0 MatchS15_00492e60_Call004c1c80` (`exe_callsites.txt`, committed next to this file):
`SetDefaultViewWindow 0x001655a0 0`; both forwarders `NOT-IN-MAP` (inlined into the body). **exe-callers=0**, so not C4.

## Default gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on a frozen copy of this build
(`A4_EXE` = `D:/MashedWT/MATCH-FIX-492E60_data/exe_gate/mashed_re.exe`, sha256
`564e940d189bbff57149f6b30e10576f1d04e70adf38922cd2705403dbbcd051`), private root `D:/MashedWT/MATCH-FIX-492E60_root`
(robocopy of `original\`), `A4_OUT` created before the run, `test-slot` outside, `mashed_lock.py run --label
MATCH-FIX-492E60 --wait 7000` inside. Every arm self-exited 0. Log: `D:/MashedWT/MATCH-FIX-492E60_data/gates.log`.

| arm file | sha256 | ref (merge_train.json on orch/integration `db86c66b5`) |
|---|---|---|
| D:/MashedWT/MATCH-FIX-492E60_data/arms/H_DEF.step.csv (13962 lines) | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 | 8284be46 |
| D:/MashedWT/MATCH-FIX-492E60_data/arms/H_DEF.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c | 67931834 |
| D:/MashedWT/MATCH-FIX-492E60_data/arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 | 31fe043e |
| D:/MashedWT/MATCH-FIX-492E60_data/arms/B_DEF_K0.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 | 1d5bf4f9 |
| D:/MashedWT/MATCH-FIX-492E60_data/arms/B_DEF_K1.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 | 1d5bf4f9 |
| D:/MashedWT/MATCH-FIX-492E60_data/arms/B_DEF_K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 | 1d5bf4f9 |

Gate hashes unchanged, as expected: the body has no exe caller and no install, so the default build never runs it.
No L-n flip decision is needed.

## Trackers (re-classify)

0x00492e60 sub_00492e60 C3->C3, no change (L-68): match evidence identical, swapped, exe-callers=0. `file` and
`exe_file` move to `Match/MatchS15_00492e60.cpp` (the `file` cell held the analysis note path, now cited in notes).
U-3922 (meaning of the `-1` argument) stays open: the match fixes the arity, not the meaning.

**Flag for the lead:** the row is C3 from the 2026-05-23 implicit-survival sweep, but the install has been
mass-disabled since 2026-05-24, which fails the C3 rubric item "hooked via RH_ScopedInstall". The sibling 0x004926c0
was demoted C4->C2 for the same reason on 2026-06-05; this row was missed. L-68 says leave the level, so this step did
not demote. A demotion to C2 is a separate re-classify call if the lead wants it.
