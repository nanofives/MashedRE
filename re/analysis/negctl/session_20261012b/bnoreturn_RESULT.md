# B-NORETURN: no-return import stand-in (0x004a31b1, 0x004a332b) — RESULT 2026-10-12

Worktree `bnoreturn`, branch `work/bnoreturn`. Triage source: `re/analysis/negctl/reearn_20261011/TRIAGE_2026-10-12.md`
§B (rows 0x004a31b1 / 0x004a332b, "no-return import stand-in that unwinds to the harness and records args").

## Verdict per row

| RVA | key | now | proposed | why |
|---|---|---|---|---|
| 0x004a31b1 | `crt_exit_process_j5` | C2 | **C3** | Port had a REAL defect the old channel could not see (ABI of the CorExitProcess pointer). Re-ported from the bytes; the new channel reads RED 3/9 on the old port, GREEN 9/9 on the re-port; self-baseline GREEN 9/9; negctl null RED 9/9, field NA (the body writes no memory), field_none CTRL-GREEN. |
| 0x004a332b | `crt_exit_no_return_j5` | C2 | **C3** | New channel GREEN 8/8, self-baseline GREEN 8/8; negctl null RED 8/8, field_first RED 6/8, field_last RED 6/8, field_none CTRL-GREEN; injected `skip_atexit` defect RED 5/8 exactly as predicted. |

No decision is needed from the user. No forced input, no differently patched MASHED.exe, no new dependency, nothing under `original\`.

## The new capability: no-return import stand-in

`re/frida/js/standins.js` (shared by `diff_template.js` and `verify_hook_install_template.js`). The import stand-in
already swapped the IAT slots of MASHED.exe and of the loaded .asi for the A/B thread only (other threads forward to
the real API through the gate stub). New spec keys:

| key | where | effect |
|---|---|---|
| `noreturn: true` | `import_standin['DLL!Api']` / `standin_fakes[name]` | records the call (`imp:Api:args:NORETURN`), runs `opts.onNoReturn`, then jumps to a 2-instruction trampoline that reads a PAGE_NOACCESS page of its own. Frida's NativeFunction (`exceptions: 'steal'`) catches that AV, restores the call frame it saved at entry (gum_exceptor setjmp/longjmp) and throws in JS. The wrapper recognises the end by `SI.isNoReturnEnd(e)` (stand-in fired AND the message names the sentinel page) and returns normally; any other error still propagates. The real API never runs on the A/B thread. No Gum interceptor state is involved (IAT slots are swapped data), so nothing is left on the invocation stack by the unwind. |
| `strings: [argIdx]` | import / fake spec | logs that arg as the ANSI string it points to (`a"mscoree.dll"`), not as its address (.rdata 0x005d11e0 vs the .asi's own literal). |
| `standin_fakes` | registry entry (forwarded by `run_diff.STANDIN_KEYS`) | `{name: {args, abi, ret, noreturn}}`: callbacks with no IAT slot, for a function POINTER a stand-in returns. Logged `fake:<name>:<args>`. |
| `import_rets` | per test | per-arm return override `{api-or-fake: value | [per call] | {fake: name}}`; `{fake: name}` resolves to that fake's address, so each vector picks `GetProcAddress -> 0 / CorExitProcess / CorExitProcessNR`. |
| `NegCtl_NoReturn` | dev .asi export (`Harness/NegCtl.cpp`) | the forwarder's post-call step (diff, thaw, field flip) never runs when the port never returns; the stand-in calls this at the no-return call instead, only inside a forwarded call. Without it both field classes read NA on every no-return row. |

How to use: give a `scalars_to_scattered_globals` entry an `import_standin` dict with the no-return import(s) marked
`noreturn`, optionally `standin_fakes` and per-test `import_rets`. Each arm's stand-in log is folded into that vector's
fingerprint and printed after ` S[`, so a RED shows the divergent call and its arguments. Fixture:
`re/frida/negctl_fixtures/z_fix_bnoreturn.py`.

### Two harness defects found and fixed on the way

1. **Generic stand-in wrapper was dead code.** `diff_template.js` appended the `S<k>` rows by overriding `send`, which
   is read-only under `'use strict'`: any handler other than `standin_object_call` with `import_standin` /
   `heap_standin` died `TypeError: 'send' is read-only` before its first test. No registry entry used that path (all 11
   stand-in users are `standin_object_call`). Now `scalars_to_scattered_globals` folds the log per vector, and any other
   handler is refused with an explicit error instead of crashing.
2. **Freed gate stubs (affects every import stand-in, including `standin_object_call`).** Stubs lived in
   `Memory.alloc` pages, freed at script unload. The IAT swap is undone before that, but a game thread that loaded the
   patched slot into a register (MSVC hoists `mov esi,[__imp_X]` out of loops) kept calling the stub and died
   executing the page base: AV `read@0xe760000 == addr`, `read@0xeac0000 == addr` on 2 of 8 warm boots with
   EnterCriticalSection stood in. Found because run_negctl respawned its slot mid-run (session `process-terminated`,
   exit 0xC0000005). Gate stub, `armed` flag, forward slot and counter now live in never-freed `VirtualAlloc` memory
   (one 64 KB region per installing script), so a stale pointer forwards to the real API. After the fix: 8 boots x 3
   runs with no page-base AV (the one crash in that probe was `MASHED.exe+0xe66db read@0x8`, the U-9677 vehicle-piz
   collision, which the rules say to re-run), and the 14-job negctl run stayed on one PID.

## Row 0x004a31b1 CrtExitProcess_j5 (___crtExitProcess)

Bytes (`disasm_va.py 0x004a31b1 0x30 original/MASHED.exe.unpatched`):
`push 0x5d11e0; call [0x5cc038]` (GetModuleHandleA "mscoree.dll"); `je 0x4a31d6`; `push 0x5d11d0; push eax; call
[0x5cc0b4]` (GetProcAddress "CorExitProcess"); `je 0x4a31d6`; `0x004a31d0 push [esp+4]; 0x004a31d4 call eax`;
`0x004a31d6 push [esp+4]; call [0x5cc1b8]` (ExitProcess); `0x004a31e0 int3`.
No stack adjust follows `call eax`, and `[esp+4]` at 0x004a31d6 must again be the exit code, so the CorExitProcess
pointer is callee-cleans (stdcall).

**Port defect (found by the new channel):** the port cast it `__cdecl`, compiled to `call eax; add esp, 4`
(.asi 0x10017b75). When CorExitProcess returns, ESP ends 4 too high and the port's `push [esp+4]` passes the slot
ABOVE the exit code to ExitProcess. Witness: `witness_old_defect_crt_exit_process_j5_RED.csv`, RED 3/9, exactly the
three `corret` vectors predicted, e.g. idx 2 code 0xff: original `fake:CorExitProcess:0xff imp:ExitProcess:0xff`, port
`fake:CorExitProcess:0xff imp:ExitProcess:0xf9cc100`. The old channel (`int_scalar`, 3-arg signature, 1-arg tests)
never entered the body on either arm: `negctl_jobs_20261011.tsv` rows `crt_exit_process_j5` null BLIND-DEGENERATE
`calls=0`.
**Re-port:** `Boot.cpp` casts to `void(__stdcall*)(int)`. The re-port compiles to the original's instruction sequence:
`push [esp+4]; call eax; push [esp+4]; call [ExitProcess]; int3`.

Vectors (9): mscoree modes absent (GetModuleHandleA -> 0), nofn (-> 0x7e570000, GetProcAddress -> 0), corret (-> fake
CorExitProcess, stdcall, returns), cornr (-> fake CorExitProcessNR, never returns) x codes 0, 1, 0xff, 7, 0x80000000,
0xffffffff, 0x12345678, 0x7fffffff, 3. Every path's import sequence and arguments are compared.
Field classes are NA here by construction: the body stores to no memory (its only effects are the imports, which the
log compares, and null reads RED 9/9 on that log).

Callers (Ghidra boundary, `decomp_pc.py --callers`): CrtExitCore 0x004a3258 (C3), fast_error_exit 0x004a4b93 (C3).
Callees: externals only (GetModuleHandleA, GetProcAddress, ExitProcess).

## Row 0x004a332b CrtExitNoReturn_j5

Bytes: `push 0; push 0; push [esp+0xc]; call 0x4a3258; add esp, 0xc; ret`. The port matches the bytes. It calls
the original CrtExitCore with `(code, 0, 0)`.
Path through CrtExitCore 0x004a3258 with keep = 0: `__lock(8)` 0x004a325f (EnterCriticalSection [0x5cc068] on
`*(0x616408 + 8*8)`, see 0x004a7886/0x004a78a7); `[0x7739dc] == 1` -> GetCurrentProcess + TerminateProcess
(0x004a3270..0x004a327a); `[0x7739d8] = 1`, byte `[0x7739d4] = 0`; atexit list walk `*(0x8ab6cc)` down to
`*(0x8ab6d0)` (0x004a3294..0x004a32c5, rewrites 0x8ab6cc); slots 0x005ea05c..0x005ea060 then 0x005ea068..0x005ea06c
(indirect `call eax` 0x004a32dd / 0x004a32fc); `[0x7739dc] = 1` (0x004a3319); ___crtExitProcess(code) 0x004a3323.

**atexit / teardown:** the walk runs BEFORE ExitProcess, and it is compared. The slot tables hold real teardown in the
binary: `0x005ea060 = 0x004a5f07` (___endstdio) and `0x005ea06c = 0x004af32d` (read from `.data` of the unpatched
image). Ghidra lists both as callees of 0x004a3258 for that reason (no direct call exists in 0x004a3258..0x004a332a).
Neither runs in the harness: every vector seeds all four slots and the atexit list (pointed at a scratch table) with
the b11 recorders R1..R4 or 0, and the seeds are restored after each arm. Observed: the recorders' windows (pre-filled
0xA5), `0x007739d4` (8 bytes), `0x007739dc` (new: b11's `_EC_OBS` stopped at 0x007739db), `0x008ab6cc` (8 bytes).
**Lock 8:** EnterCriticalSection / LeaveCriticalSection are stand-ins (recorded, not performed). The no-return path
never reaches `_unlock` 0x004a77eb, so a real lock would stay owned by the A/B thread and deadlock the game's next
atexit registration. The log shows `imp:EnterCriticalSection:0x773a98` on both arms.

Vectors (8): codes 7, 0, 3, 0xff, 5, 0x80000000, 0xffffffff, 9; atexit lists of 0..4 entries and empty; slot
mixes; guard 0x7739dc = 1 (TerminateProcess path) twice; mscoree modes absent / corret / cornr / nofn.

Witness (injected, only the new channel sees it): `CrtExitCore(code, 1, 0)` (skip_atexit). Predicted RED at idx 0,
1, 2, 3, 6 and GREEN at 4 and 7 (TerminateProcess before the flag is read) and 5 (empty list, empty slots). Observed RED
5/8 at exactly those indices (`witness_injected_skip_atexit_crt_exit_no_return_j5_RED.csv`). The import logs match on
both arms, and the observed windows carry the difference. Restored with `git checkout`, `touch`ed, rebuild printed
`Boot\Boot.cpp  (source newer)` (`build_restore_log.txt`), then run_diff re-run for the committed CSV. The old channel:
`negctl_jobs_20261011.tsv` `crt_exit_no_return_j5` null BLIND-DEGENERATE `calls=0`.

Callers (Ghidra boundary): 0x0040bbb0 (C2), 0x0040e3a0 PlayerColorTableGet (C3), 0x00478200 (C2), 0x00485380 (C2),
0x00485bf0 StackPush485bf0 (C3), 0x00490e50 (C2), 0x004b7ba0 (C2, lua-4.0), 0x0058f520 / 0x0058f6a0 / 0x0059abe0 (C2,
qhull), entry 0x004a4bb7 (C2). Callee: CrtExitCore 0x004a3258 (C3).

## Evidence (all under `re/analysis/negctl/session_20261012b/bnoreturn_evidence/`)

| file | what | result |
|---|---|---|
| `crt_exit_process_j5.csv` | path1, final build | GREEN 9/9 |
| `selfbaseline_crt_exit_process_j5.csv` | original vs original, final harness | GREEN 9/9 |
| `witness_old_defect_crt_exit_process_j5_RED.csv` | path1 on the pre-fix port (__cdecl cast) | RED 3/9 (idx 2, 5, 8) |
| `crt_exit_no_return_j5.csv` | path1, final build | GREEN 8/8 |
| `selfbaseline_crt_exit_no_return_j5.csv` | original vs original, final harness | GREEN 8/8 |
| `witness_injected_skip_atexit_crt_exit_no_return_j5_RED.csv` | path1 with `CrtExitCore(code, 1, 0)` | RED 5/8 (idx 0, 1, 2, 3, 6) |
| `build_restore_log.txt` | rebuild after the witness restore | `Boot\Boot.cpp (source newer)` |
| `negctl/negctl_20261012_205950.tsv` | `run_negctl.py --handlers scalars_to_scattered_globals --hooks crt_exit_process_j5,crt_exit_no_return_j5 --any-conf --classes null,ret,field_first,field_last,field_none --slots 1` | 31b1: real GREEN, baseline GREEN, null RED 9/9, ret SKIP (void), field_first/last NA, field_none CTRL-GREEN. 332b: real GREEN, baseline GREEN, null RED 8/8, ret SKIP, field_first RED 6/8 (flip 0x00639d70), field_last RED 6/8 (flip 0x008ab6cc), field_none CTRL-GREEN. One PID for all 14 jobs. |

The 6/8 field result is predicted: the two guard = 1 vectors reach TerminateProcess before any store, so the port
changed no byte there and nothing is flipped.

## Commits on `work/bnoreturn`

| commit | content | keep/drop |
|---|---|---|
| 94f2c0a1 | standins.js no-return / strings / fakes / per-arm rets + persistent gate stubs; diff_template.js wrapper fix + per-vector stand-in log; verify template arms the stand-ins; NegCtl_NoReturn; run_diff forwards `standin_fakes`; Boot.cpp stdcall re-port; fixture `z_fix_bnoreturn.py`; first evidence | keep |
| (this commit) | final CSVs, re-run self-baselines, witness CSV for 0x004a332b, restore build log, ARG_TYPES.md regenerated (line numbers only), this note | keep |

## Path2 handler keys (orchestrator runs path2 after merge)

| row | key | arg_type | path2 vectors |
|---|---|---|---|
| 0x004a31b1 | `crt_exit_process_j5` | `scalars_to_scattered_globals` + `import_standin` / `standin_fakes` | code 0xff corret, code 7 cornr |
| 0x004a332b | `crt_exit_no_return_j5` | `scalars_to_scattered_globals` + `import_standin` / `standin_fakes` | code 7, atexit [R1,0,R2], slots [R3,0] / [R4,0] |

`verify_hook_install_template.js` now installs the same stand-ins for every non-`standin_object_call` handler and treats
the no-return fault as the call's end. Without it, path2 calls the installed port, which calls the REAL ExitProcess
and ends the game. Not run here (per brief); both agent sources pass `node --check`. Expect `got` = `S[...]` per case.
For `crt_exit_no_return_j5` the installed port is entered from the patched 0x004a332b and calls the ORIGINAL
CrtExitCore: its own install is MASS-DISABLED (`Boot/CrtStartup.cpp:338`), while both rows here install
(`Boot/Boot.cpp` `RH_ScopedInstall(CrtExitProcess_j5, 0x004a31b1)`, `RH_ScopedInstall(CrtExitNoReturn_j5, 0x004a332b)`).

## Proposed re-classify transaction (do not hand-apply; run through `re-classify`)

- `004a31b1,CrtExitProcess_j5`: C2 -> **C3**. frida_diff ->
  `re/analysis/negctl/session_20261012b/bnoreturn_evidence/crt_exit_process_j5.csv`. Append: "bnoreturn 2026-10-12:
  C2->C3. Re-ported: CorExitProcess pointer is callee-cleans (0x004a31d4 call eax / 0x004a31d6 push [esp+4], no
  adjust); the __cdecl port passed ExitProcess the slot above exit_code (witness RED 3/9). No-return import stand-in
  (standins.js `noreturn`) records GetModuleHandleA / GetProcAddress / CorExitProcess fake / ExitProcess with args;
  GREEN 9/9, self-baseline 9/9; negctl null RED 9/9, field NA (no stores), field_none CTRL-GREEN. Callers 0x004a3258
  C3, 0x004a4b93 C3; callees externals only."
- `004a332b,CrtExitNoReturn_j5`: C2 -> **C3**. frida_diff ->
  `re/analysis/negctl/session_20261012b/bnoreturn_evidence/crt_exit_no_return_j5.csv`. Append: "bnoreturn 2026-10-12:
  C2->C3. No-return stand-in for ExitProcess / TerminateProcess, lock 8 Enter/Leave recorded not performed; atexit list
  + slot tables seeded with recorders (real ___endstdio 0x004a5f07 / 0x004af32d never run), 0x7739d4..0x7739df and
  0x8ab6cc observed. GREEN 8/8, self-baseline 8/8; negctl null RED 8/8, field_first/last RED 6/8, field_none
  CTRL-GREEN; injected skip_atexit RED 5/8 as predicted. Callee 0x004a3258 C3."
- `004a4b93,CrtFastErrorExit` (C3, unchanged): its note "callee ___crtExitProcess C2 (004a31b1)" becomes C3.
- CHANGELOG: one entry for both rows and the two harness fixes (generic stand-in wrapper `send` override; freed gate
  stubs in standins.js, which also hardens all 11 `standin_object_call` fixtures).
- No STUBS / UNCERTAINTIES / DEFERRED rows change.
