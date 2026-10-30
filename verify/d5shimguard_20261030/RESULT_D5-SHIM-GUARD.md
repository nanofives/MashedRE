# RESULT D5-SHIM-GUARD (run 25, 2026-10-30)

VERDICT: GREEN. `mashed_re.exe` no longer binds a `d3d9.dll` or `dinput8.dll` placed beside it. Both now resolve from System32 only. Default gates are unchanged.

Branch `orch/D5-SHIM-GUARD` from `orch/integration` 929c1c633 (D5-SHIM-CONSUMERS merged, L-65: both shims stay as dev harness, so the guard is required). Source commit ab60b8bf6.

## 1. Method and why

The task's two suggested forms (pre-load from System32, or `SetDefaultDllDirectories` early in startup) both assume the DLLs are loaded at runtime. They are not. The pre-guard exe imports `d3d9.dll` (`Direct3DCreate9`) and `DINPUT8.dll` (`DirectInput8Create`) **statically** (`dumpbin /imports` on the Oct 24 build, sha256 `01fa6bdd...`). The loader binds static imports before any user code runs, including the B17 TLS callback (`exe_main.cpp`, `B17_TlsReserve`). The exe's own directory is searched first, so a guard at WinMain entry would run after the proxy was already loaded and initialized.

Chosen form: **delay-load plus a delay-load notify hook that loads only these two names with `LOAD_LIBRARY_SEARCH_SYSTEM32`.**

- `mashedmod/build.bat`, exe link line: `/DELAYLOAD:d3d9.dll /DELAYLOAD:dinput8.dll`. `exe_main.cpp` adds `#pragma comment(lib, "delayimp.lib")`. The `.asi` link line is untouched.
- `mashedmod/src/mashed_re/exe_main.cpp`: `DllGuardDelayHook`, installed as `__pfnDliNotifyHook2`. On `dliNotePreLoadLibrary` for `d3d9.dll` or `dinput8.dll` (case-insensitive) it calls `LoadLibraryExA(name, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32)` and writes `dllguard: <dll> -> <resolved path>` to `mashed_re.log`. Every other delay-load returns `nullptr` (default behaviour).
- It fails closed. If the System32 load fails, the hook raises the standard delay-load `VcppException(ERROR_SEVERITY_ERROR, ERROR_MOD_NOT_FOUND)` instead of returning `nullptr`, because a `nullptr` would let the helper fall back to the default search (exe dir first).

Why this rather than `SetDefaultDllDirectories`:
1. It does not depend on call order. The hook runs on the first call of either DLL, wherever that call is, including librw's `Direct3DCreate9` (`deps/librw/src/d3d/d3ddevice.cpp`).
2. It changes nothing else in the process. `SetDefaultDllDirectories` is process-wide and would also govern the relative `LoadLibraryExA("original\\MASHED.exe", ...)` resource load (`exe_main.cpp:8571` pre-edit, R6). The docs leave relative paths under `LOAD_LIBRARY_SEARCH_*` undefined.
3. It is standalone-only by construction. The hook and the delay-load flags exist only in the exe target (`exe_main.cpp` is not in `asi_sources.rsp`, and the `.asi` link line is unchanged). The dev flow, which injects `mashed_re_dev.asi` into `MASHED.exe` through the shims in `original\`, does not change.

Post-build import table (`dumpbin /imports`, guard exe sha256 `3ee9c6f4...`): static imports are USER32, DSOUND, GDI32, ole32, KERNEL32. The section "delay load imports" lists `d3d9.dll` and `DINPUT8.dll`.

## 2. Dummy-proxy test

Driver: `verify/d5shimguard_20261030/dummy_dll_test.py`. Dummy source: `verify/d5shimguard_20261030/dummy_proxy.c`. Each dummy exports undecorated `Direct3DCreate9` and `DirectInput8Create`. On `DLL_PROCESS_ATTACH` it writes `<dll>.LOADED.txt` beside itself and returns FALSE. The driver copies the exe to `D:\MashedWT\D5-SHIM-GUARD_root\mashed_re.exe`, puts the dummies beside it, and boots with `MASHED_ROOT` = that root on the G-CTRL base env for 1200 deterministic frames, under `mashed_lock.py` and `test-slot`. It waits on its own PID only.

| Arm | Exe | Dummies beside exe | Exit | Marker | dllguard lines |
|---|---|---|---|---|---|
| control_preguard | pre-guard `01fa6bdd` | d3d9 + dinput8 | `0xC0000142` (STATUS_DLL_INIT_FAILED), 1 s | d3d9 BOUND (dinput8 not reached: load aborted at d3d9 init) | log absent |
| control_preguard_dinput8 | pre-guard `01fa6bdd` | dinput8 only | `0xC0000142`, 1 s | dinput8 BOUND | log absent |
| guard | guard `3ee9c6f4` | d3d9 + dinput8 | 0, ran its 1200 frames (45 s) | neither bound | `d3d9.dll -> C:\WINDOWS\SYSTEM32\d3d9.dll`, `DINPUT8.dll -> C:\WINDOWS\SYSTEM32\DINPUT8.dll` |
| guard_dinput8 | guard `3ee9c6f4` | dinput8 only | 0 (44 s) | not bound | same two lines |

A first pass used dummies with `__stdcall`-decorated exports (`_Direct3DCreate9@4`). There the control failed with `0xC0000139` (entry point not found) before `DllMain`, so it left no marker. That pass was superseded by the rows above. Its output is kept as `dummy_test.txt`.

## 3. Default gates (merge_train.json refs: G-CTRL 8284be46/ba407a16/31fe043e, B 1d5bf4f9 x3)

`run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, `A4_ROOT=D:/MashedWT/D5-SHIM-GUARD_root`, guard exe `3ee9c6f4`:

| Arm | Exit | Result | Ref |
|---|---|---|---|
| H_DEF | 0, self-exited, 477 s | step 8284be46 (13962 lines), gates ba407a16 (54085), ptrace 31fe043e (27042) | match |
| B_DEF_K0 | 0, 40 s | 1d5bf4f9 (762) | match |
| B_DEF_K1 | 0, 39 s | 1d5bf4f9 (762) | match |
| B_DEF_K26 | 0, 39 s | 1d5bf4f9 (762) | match |

The H_DEF `mashed_re.log` carries both `dllguard:` System32 lines. The `-Full` extras (D2_DEFH arms, run13) were not run here. The change touches no `full_set_paths` (Vehicle/Ai/Collision), and the merge train runs its own gate set.

## 4. Notes for the lead

- `.ps1` edits needed: none. `scripts/lan_release/publish.sh` builds through `build.bat`, so LAN releases pick up the guard with no change.
- The `build.bat` loop that renames stray `d3d9.dll`/`dinput8.dll` out of `%OUT%` (R1) is now redundant for the exe. I kept it and updated the RUNTIME NOTE to say so. It can be deleted in a later cleanup step.
- Residual, out of scope: `DSOUND.dll` is still a static import, so the same hazard applies to a `dsound.dll` proxy beside the exe (a common mod-loader name). The same two-line fix (`/DELAYLOAD:dsound.dll` plus adding the name to the hook) would cover it. That would need its own step and gates.

## 5. Raw files (D:/MashedWT/D5-SHIM-GUARD_data)

| File | SHA-256 |
|---|---|
| build.log | b60f85579a2f168e2be2b7de9c0cce21f85292df20e5017f489ca4a1260377e4 |
| dummy_test2.txt (both-dummy control + guard) | 15bc0567a83a9f6360ca0b2f21d0ec81260adf8f4587b334a248a31f8f8cc5f7 |
| dummy_test3.txt (dinput8-only control + guard) | 6abbfdd3b6f31f7d43bd42c49984191416ba71b73bfe2fd072ed8a6ce4238792 |
| dummy/guard.log | d4b2225c57cded7aacf4d193a8a0e177571bdf56ddb2410ada4a5590b381ff35 |
| dummy/guard_dinput8.log | f58f5edf164251e403b1faadbb494e37ccfe807ce3f85da9acc0e623e59ef10e |
| dummy_bin/d3d9.dll | 12f08646a249bbd61ec1b4f975a71df38fd46cd23b3c8f141fcef32276782db2 |
| dummy_bin/dinput8.dll | dd15b3118a1988353d503cf479530fc6753f3000ab1e5126b8dbc0ebdcce897e |
| gates.txt | c5d09f72f62a023650b6293db35aa6f79c8d07da0a912a3324c4220e1387ec65 |
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K{0,1,26}.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 (all three) |
| guard exe mashed_re.exe | 3ee9c6f44ba6cfd6f4fe75c920412d5ab5b668808fe55a31b3b52c28da5511c2 |
| pre-guard control exe (main checkout mashedmod/build, Oct 24) | 01fa6bddca4702c7a94beb4c13dbd478eadab482401a970f72c4e210663f2560 |
