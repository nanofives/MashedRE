# HANDOFF MATCH-FIX-4950B0 (2026-11-06 20:19, paused for the LIVING-PC reboot)

Worktree `C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\MATCH-FIX-4950B0`, branch `orch/MATCH-FIX-4950B0`
(cut from orch/integration `22ffd015a`). Ledger row: `working`.

## Done (committed)
- `095fc874d` code swap: `mashedmod/src/mashed_re/Match/MatchS16_004950b0.cpp` (new, `#define FUN_004950b0
  QpcTimeScaledTo3Mhz`, includes `re/match/s16/004950b0.c` under MATCH_PORT, exports QpcTimeScaledTo3Mhz, no install),
  old body removed from `Util/UtilMid.cpp` (pointer comment left), wrapper added to `mashedmod/asi_sources.rsp`
  after MatchS16_00495080. Not in exe_sources.rsp (UtilMid was asi-only; the exe calls 0x004950b0 by raw address).
- `6aca125f5` re-classify: hooks.csv row 004950b0 C2->C2 (L-68), name QpcTimeScaledTo3Mhz, file -> wrapper,
  CHANGELOG line under `<!-- ENTRIES -->`.
- Checks run: match.py 100% 23/23 (/O2 /GS /Ob1, tc=ddk); relocheck OK checked=2 unchecked=3
  (__alldiv, __allmul, __imp__QueryPerformanceCounter@4); src sha256
  41f7ebf41440edec5a8eb2cf4dcd9e30756636642d03c12dae734bd6ccfd458b; rva-lint NEW=0; asm-lint OK (1427 files);
  rva_body_scan 0x004950b0: exe none, asi Match/MatchS16_004950b0.cpp only.
- Private root made: `D:\MashedWT\MATCH-FIX-4950B0_root\original` (robocopy), `D:\MashedWT\MATCH-FIX-4950B0_data\arms`.

## Next
1. `py -3.12 re/orchestrator/mashed_lock.py status`: no PRIORITY-HOLD, then build:
   `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"` > `D:/MashedWT/MATCH-FIX-4950B0_data/build.log`.
   Check no warning from the new TU (possible C++ clash: the matched source declares QueryPerformanceCounter itself
   inside extern "C"; the wrapper does not include windows.h), QpcTimeScaledTo3Mhz exported from the asi, exe/asi sha256.
2. Gates (test-slot outside, mashed_lock inside, `--wait` large): A4_OUT=D:/MashedWT/MATCH-FIX-4950B0_data/arms,
   A4_ROOT=D:/MashedWT/MATCH-FIX-4950B0_root, `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`.
   Compare with re/orchestrator/merge_train.json on orch/integration at gate time (was G-CTRL 8284be46/67931834/31fe043e,
   B 1d5bf4f9 x3). Expected unchanged: the exe has no body and no caller change.
3. Write `RESULT_MATCH-FIX-4950B0.md` here (mirror orch/MATCH-FIX-492D30's RESULT), commit, `roadmap.ps1 set ... review`.
4. REPORT verdict DECISION (the matched body drops the freq==0 guard). Options: (a) keep the matched original form,
   no guard (recommended: in the original every call with freq 0x00771e70 == 0 raises #DE, and the original boots and
   races, so no call it makes reaches freq 0; QPF is written once by 0x00495120 TimerQpfStore; only suspended-spawn
   Frida harnesses reach freq 0 and hooks_registry.py:706 already seeds it), (b) keep the guard as a port-safety
   deviation (wrapper-side check before the matched call, not byte-identical semantics).
