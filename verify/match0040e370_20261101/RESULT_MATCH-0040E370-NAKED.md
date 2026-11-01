# RESULT MATCH-0040E370-NAKED (+ D4-DC-INC-0040e370), 2026-11-01

**Verdict: GREEN.** 0x0040e370 (`IsCarSlotActive`) now has one body in `mashed_re.exe`, the byte-matched source.
The naked `TG_40e370` is gone and `TG_414a70`'s call at 0x00414ad5 goes to the matched body. Allowlist key
`DUP-IN-TARGET:0040e370` removed. Gates unchanged. Re-classified C3 -> C4, route `match`. hooks.csv C4 rows 202 -> 203.

## Change

- `re/tools/naked_specs/target_p3.json`: drop the `TG_40e370` func entry, map call `0x0040e370` -> `IsCarSlotActive`.
- `mashedmod/src/mashed_re/Ai/AiTargetClosest_naked.inc`: regenerated with `py -3.12 re/tools/gen_naked_asm.py
  re/tools/naked_specs/target_p3.json <inc>` (run from the main checkout, which holds `original/MASHED.exe.unpatched`).
  Before the edit the generator reproduced the committed .inc byte for byte (line endings aside). Diff: the forward
  declaration and the 34-byte naked body of `TG_40e370` removed, `call TG_40e370` -> `call IsCarSlotActive` at 00414ad5.
- `mashedmod/src/mashed_re/Ai/AiTargetClosest.cpp`: `extern "C" int __cdecl IsCarSlotActive(int)` declared before the
  include, header comment updated (five generated leaves, 0x0040e370 bound to the matched body).
- `re/tools/dual_copy_allowlist.txt`: the one line `DUP-IN-TARGET:0040e370:...` removed. No other key touched.
- No .ps1 / .bat edits, `build_objs.ps1` unchanged, 0x00408a50 untouched. `Match/MatchS02_0040e370.cpp` wrapper kept.

ABI: the matched body is cdecl, argument on the stack, return in eax, clobbers eax/ecx/edx only. The original
0x0040e370 also clobbers ecx and edx (`0x0040e37c`, `0x0040e387`), so the naked caller relies on neither.

## Evidence

| leg | result |
|---|---|
| (a) match | `py -3.12 re/tools/match/match.py re/match/s02/0040e370.c` -> `0x0040e370 _FUN_0040e370: 100.0% (regs 100.0%, ops 100.0%) 14/14 ins flags=/O2 /GS /Ob1 tc=ddk` (XP DDK cl 13.00.9176 / c2 13.00.9178, C, x87) |
| (a) relocheck | `py -3.12 re/tools/match/relocheck.py re/match/s02/0040e370.c` -> `0x0040e370 OK checked=1 unchecked=0` (no unchecked names) |
| source | `re/match/s02/0040e370.c`, git blob sha256 `dc117d70255108a579303163ffeac603f175ff71745cb0ba32f9f34a6df14c63` (unchanged since MATCH-S02) |
| (b) one body | `py -3.12 scripts/lint_rva_bodies.py` -> 47 known (CROSS-TARGET 33, DUP-IN-TARGET 14), NEW 0, no 0x0040e370 finding. `Match\MatchS02_0040e370.cpp` is in exe_sources.rsp and asi_sources.rsp; `Ai\AiTargetClosest.cpp` is exe-only. Not a physics TU. asm-lint OK |
| (b) exe callers, static | `mashed_re.exe` (sha256 `ee4976957b7a91c7...`) exports `IsCarSlotActive` at 0x00151070 (map: `_IsCarSlotActive` MatchS02_0040e370.obj). 8 `call rel32` into it: `_TG_414a70+0x65` (= 0x00414a70+0x65 = 0x00414ad5), `CarParts_FrameTick+0x29`, `_RaceComputeDistancesTick+0x48`, `PathProg_FrameTick+0x112`, `+0x156`, `HeadingAssist_Frame+0x15`, `HeadingAssist_Player+0xc`, `CarsReg_FrameTick+0x11`. exe-callers=8 |
| (b) exe callers, run | G-P3-REPLAY (`verify/d3_modes37_20261009/run_p3_replay.ps1` recipe, PC_24, 5490 original FUN_00414a70 calls from `o_tg1.msd.tgt.csv` replayed through `TG_414a70`, which now calls the matched body for each car slot): `pairs=5490 exact=5490 ret_mismatch=0 cand_mismatch=0 cand_touched_on_ret!=1=0 orig_ret0=5391 orig_ret1=63 orig_ret2=36`, identical to the 2026-10-09 P3 result. The exe self-exited, exit 0 |
| (c) gates | `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, private root, test-slot + mashed_lock: G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9/1d5bf4f9/1d5bf4f9` = merge_train.json refs |
| build | `set MASHED_NO_DEPLOY=1&& mashedmod\build.bat` full rebuild, Build OK, .asi not deployed. exe `ee4976957b7a91c7`, asi `c9d79a9a8033e1f2` |

Write-set: not asked on the match route. The function has no store outside its frame (`push esi`/`pop esi` only).
Still attached to the row, Blocks=none: U-1611 (type of the object at `*0x005f2770`), U-0394 (meaning of the +0x34
values). Neither changes the body. No stubs (no callees).

The merge train's `-Full` pass (D2 x3, 13 tracks) was not run here: `mashedmod/src/mashed_re/Ai/` is in
`full_set_paths`, so the train runs it on enqueue.

## Raw files (not committed; `D:/MashedWT/MATCH-0040E370-NAKED_data`)

| file | sha256 |
|---|---|
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0.step.csv, B_DEF_K1.step.csv, B_DEF_K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 (all three) |
| p3_replay_pc24.txt | 9eb2ee48194bb6feb8635fd7fe772fcc2254bcffcc86f95addf1f2bc612baebb |

## Trackers (re-classify)

hooks.csv row 0040e370: C3 -> C4, notes `c4=match` with the fields above. CHANGELOG line prepended below
`<!-- ENTRIES -->`. No STUBS / UNCERTAINTIES / DEFERRED rows changed.
