# RESULT MATCH-FIX-46C5C0 (+ MATCH-S11-FU-STRIDE), 2026-11-06

**Verdict: GREEN. Port defect fixed, byte-matched bodies swapped in, default gates unchanged.** Branch `orch/MATCH-FIX-46C5C0`, cut from orch/integration ac2e9ed63. Code commit 128cdcce4.
Both ledger rows (MATCH-FIX-46C5C0, MATCH-S11-FU-STRIDE) are the same defect and close together.

## The defect, confirmed against the disassembly

Read from `original/MASHED.exe.unpatched` (anchor SHA-256 BDCAE093...), capstone, 2026-11-06:

```
0046c5c0  8b442404        mov eax, dword ptr [esp + 4]
0046c5c4  83f810          cmp eax, 0x10
0046c5c7  7203            jb 0x46c5cc
0046c5c9  33c0            xor eax, eax
0046c5cb  c3              ret
0046c5cc  8b0d30107f00    mov ecx, dword ptr [0x7f1030]
0046c5d2  69c0040d0000    imul eax, eax, 0xd04
0046c5d8  05a0158800      add eax, 0x8815a0
0046c5dd  c7400400000000  mov dword ptr [eax + 4], 0
0046c5e4  8988100b0000    mov dword ptr [eax + 0xb10], ecx
0046c5ea  b801000000      mov eax, 1
0046c5ef  c3              ret

0046c790  8b442404        mov eax, dword ptr [esp + 4]
0046c794  83f810          cmp eax, 0x10
0046c797  7204            jb 0x46c79d
0046c799  83c8ff          or eax, 0xffffffff
0046c79c  c3              ret
0046c79d  8b4c2408        mov ecx, dword ptr [esp + 8]
0046c7a1  69c0040d0000    imul eax, eax, 0xd04
0046c7a7  8988b0158800    mov dword ptr [eax + 0x8815b0], ecx
0046c7ad  33c0            xor eax, eax
0046c7af  c3              ret
```

| rva | old port body (Frontend/SmallLeaves_n2.cpp) | original |
|---|---|---|
| 0x0046c5c0 | BYTE 0 at 0x008815a4 + i*0x341 | DWORD 0 at 0x008815a4 + i*0xd04 (0x0046c5d2, 0x0046c5dd) |
| 0x0046c5c0 | DAT_007f1030 to 0x008820b0 + i*0xd04 | same (0x0046c5e4: 0x008815a0 + 0xb10 = 0x008820b0) |
| 0x0046c790 | param_2 to 0x008815b0 + i*0x341 | param_2 to 0x008815b0 + i*0xd04 (0x0046c7a1, 0x0046c7a7) |

The two agree only at i = 0 (and 0x0046c5c0 then still wrote 1 byte where the original writes 4). Bounds checks and
return values matched, which is why the old Frida C3 vectors (`vehicle_slot_init` / `vehicle_slot_field_set`,
return value only) were GREEN: they never observed the stores.

## Fix

Two wrapper TUs in the MATCH-S11 pattern (`Match/MatchS11_<rva>.cpp`: `#define FUN_<rva> <export>`, `/EXPORT`
pragma, `extern "C" { #include re/match/s11/<rva>.c }`, `RH_ScopedInstall`):

- `Match/MatchS11_0046c5c0.cpp` -> `VehicleSlotInit`
- `Match/MatchS11_0046c790.cpp` -> `VehicleSlotFieldSet`

The old bodies and their installs are removed from `Frontend/SmallLeaves_n2.cpp` (a pointer note stays). Both
wrappers are added to `exe_sources.rsp` and `asi_sources.rsp` right after `"Frontend\SmallLeaves_n2.cpp"`, the
same two targets that compiled the old bodies. No x87 entry (old TU was SSE; the bodies have no float math).

**Bound targets.** Both sources are leaves: no callees to bind. Globals bind to their original addresses through
the preamble `MGLOBAL`/`MGLOBALA`: `DAT_008815a0` (`Car[16]`, 0xd04 each), `DAT_007f1030`. Exports keep their names
(`VehicleSlotInit`, `VehicleSlotFieldSet`), so `hooks_registry.py` entries resolve unchanged.

## Static evidence

| check | result |
|---|---|
| `match.py re/match/s11/0046c5c0.c --flags "/O2 /GS /Ob1" --toolchain ddk --bytes --only 0x0046c5c0` | 100.0% (regs 100, ops 100) 12/12 ins, 48 B whole symbol BYTE-IDENTICAL |
| `match.py re/match/s11/0046c790.c --flags "/O2 /GS /Ob1" --toolchain ddk --bytes --only 0x0046c790` | 100.0% (regs 100, ops 100) 10/10 ins, 32 B whole symbol BYTE-IDENTICAL |
| `relocheck.py re/match/s11/0046c5c0.c` | OK checked=2 unchecked=0 |
| `relocheck.py re/match/s11/0046c790.c` | OK checked=1 unchecked=0 |
| source sha256 0046c5c0.c | 70e3406f129a79685172b6b07038a9fad438746eb659943742d53614944be376 |
| source sha256 0046c790.c | e47999c89a1d119761416dd50f0fb8b40b306f198b71b5dbbd4da55d2f54091f |
| `scripts/lint_rva_bodies.py` | OK, NEW=0 (1 known CROSS-TARGET, allowlisted, not these RVAs) |
| build (`MASHED_NO_DEPLOY=1 mashedmod\build.bat`) | OK, exe + asi, `original\mashed_re_dev.asi` not deployed |

(Git Bash note: run match.py with `MSYS_NO_PATHCONV=1`, otherwise `/O2 /GS /Ob1` is path-converted and the compile
falls back to unoptimized frames, which reads as a mismatch.)

## Exe callers (L-68)

`verify/matchs11_20261102/tools/exe_callsites.py mashed_re.exe mashed_re.map VehicleSlotInit VehicleSlotFieldSet`:
`VehicleSlotInit 0x00160490 0`, `VehicleSlotFieldSet 0x001604d0 0`. **exe-callers=0** for both. The only port
caller of either RVA is `FUN_00422fd0` (`Match/MatchS05_00422fd0.cpp`, asi only), by raw address. The exe's
dead-coast path (`TrackRenderer.cpp:8168`, `VehiclePhysics_MarkKilled`) writes the record through its own port
record, not through these RVAs. So **not C4** (L-68): the rows keep C3 with the match evidence note.

## Gates

**Unchanged.** `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on a frozen copy of this
build (`A4_EXE` = `exe_gate/mashed_re.exe`, sha256 0ddd2fbe...), private root `D:/MashedWT/MATCH-FIX-46C5C0_root`
(`MASHED_ROOT`, robocopy of `original\`), test slot outside, `mashed_lock.py run --label MATCH-FIX-46C5C0` inside.

| arm | file | sha8 | ref (`merge_train.json` on orch/integration, 2026-11-06) |
|---|---|---|---|
| H_DEF | step.csv (13962 lines) | 8284be46 | 8284be46 |
| H_DEF | gates.csv | 67931834 | 67931834 |
| H_DEF | ptrace.log | 31fe043e | 31fe043e |
| B_DEF_K0 / K1 / K26 | step.csv (762 lines each) | 1d5bf4f9 x3 | 1d5bf4f9 x3 |

Expected: both swapped bodies have exe-callers=0, so the exe's executed code does not change. The first attempt
(`gates_run_try1_nooutdir.txt`) stopped in run_arms.py's copy-out because `A4_OUT` did not exist yet. It produced no
hashes. The rerun above is the gated result. `mashedmod/src` is in `full_set_paths`, so the merge train's `-Full` pass
(D2 x3, 13 tracks) still applies on landing.

## Tracker transaction (re-classify)

Applied on this branch (re-classify, no level change):

- `hooks.csv` 0046c5c0 / 0046c790: confidence stays **C3**. `file` and the last column now point at
  `Match/MatchS11_<rva>.cpp`. `notes` gain `2026-11-06 MATCH-FIX-46C5C0 (closes MATCH-S11-FU-STRIDE)`: the defect,
  `match evidence: identical, swapped, exe-callers=0; C4 when a caller lands (L-68 ruling)`, the match.py command and
  result, relocheck line, source sha256, gate hashes and this RESULT. No `c4=match` token.
- `re/analysis/CHANGELOG.md`: two `C3->C3` lines below `<!-- ENTRIES -->`.
- `UNCERTAINTIES.md`: U-2870 and U-2872 resolved through `re/tools/uncertainty_resolve.py` (their 0x341-stride and
  +0x0c statements are refuted by the disassembly above: the record is 0xd04 bytes at 0x008815a0, these leaves write
  +0x4, +0x10 and +0xb10). U-2871 (meaning of DAT_007f1030) stays open: nothing here answers it.

Not changed: `re/frida/hooks_registry.py` (`vehicle_slot_init` / `vehicle_slot_field_set` keep their export names
and return-value vectors; their comments still say 0x341, and the vectors never observed the stores, which is how the
defect passed C3). A store-observing fixture exists for the caller (`frontend_race_results_dispatch`, re-earn
2026-10-11, observes 0x008815a4/0x008815b0/0x008820b0 + p*0xd04).

## Raw files (`D:/MashedWT/MATCH-FIX-46C5C0_data`)

| file | sha256 |
|---|---|
| exe_gate/mashed_re.exe | 0ddd2fbecdb8bb26ebda40ee57f98f4d7fdf1a8552e37b39a47eb2deb4b5f622 |
| exe_gate/mashed_re.map | 8af82d2b39668113bf621c92e5af4c2060758250b4edb183e6bb1bbbb0fdc3ec |
| build.log | 7e62c81d6466e3b0c04b1453d0af70ea22b6d91ee84e2ad2631c7dcf8d1323b1 |
| match.txt | 82759900d7037ac17e308206dc0ac36076166f126adb331a062b8d5a8024f078 |
| exe_callsites.txt | 697dc3137799f95a7011ddca9efb4689145e567c4b0fb94cd157380e0f754968 |
| rva_lint.txt | 9d55eab622208dd47d82ba9b7510f0f9b66a19bab10ad22420a0bee9cde64307 |
| arms_gate/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms_gate/H_DEF.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c |
| arms_gate/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms_gate/B_DEF_K0.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| arms_gate/B_DEF_K1.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| arms_gate/B_DEF_K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| arms_gate/RUNLOG.txt | ce717d5edb4867cd773a312dcdf08510c63997680964b42563e6013ca50b6972 |
| gates_run.txt | 87adb17bfd2e0fcce63ff80bf2e2893968ef20ab63a6d6e40bca1d26f56f9b68 |
| gates_run_try1_nooutdir.txt | bd20c6c146555fef98f9009593b2ae15d8af26e22628933d46ba1bb6c7a57cf9 |
