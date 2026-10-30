# RESULT D5-SHIM-GUARD-DSOUND (2026-10-30)

VERDICT: GREEN. `mashed_re.exe` no longer imports `DSOUND.dll`, and a `dsound.dll` beside it is not bound. dsound now loads from System32 only, before the boot movies, with a `dllguard:` line. Default gates are unchanged.

Branch `orch/D5-SHIM-GUARD-DSOUND` from `orch/integration` e23f260b7. Source commit 049eb27bd. Follow-up of
D5-SHIM-GUARD (`verify/d5shimguard_20261030`, §4 residual): the static `DSOUND.dll` import of `mashed_re.exe` bound a
`dsound.dll` placed beside the exe.

## 1. Method, and why not `/DELAYLOAD:dsound.dll`

The step title asks for `/DELAYLOAD:dsound.dll` plus the existing `__pfnDliNotifyHook2`. That form needs an edit to
`mashedmod/build.bat` (the exe link line), and this step may not edit `.bat` files. The pragma route does not work:
`#pragma comment(linker, "/DELAYLOAD:dsound.dll")` is rejected by the linker with `LNK4229: invalid directive
'/DELAYLOAD:dsound.dll' encountered; ignored`, and the probe exe still imports `DSOUND.dll` statically (probe run
2026-10-30 in the session scratchpad, cl 19.44).

Second finding that rules out delay-load alone: the boot movies run before `Audio::Init`
(`exe_main.cpp` WinMain, `RunBootIntroMovies()` then `Audio::Init`), and `MpegVideoTexture::Open` calls
`graph_->Render()` on the unconnected audio pin (`D3d9Render/MpegVideoTexture.cpp:186`). DirectShow's audio renderer
loads `dsound.dll` by name, and a by-name load searches the exe directory first. With dsound delay-loaded and no other
change, the first `dsound.dll` in the process would come from that DirectShow load, not from the hook. So the guard has
to load dsound from System32 **before the first movie**. After that, every by-name load returns the module already
loaded (documented loader rule: a module with the same name already in memory is used).

Chosen form (source only, no `.bat` change):

- `mashedmod/src/mashed_re/DllGuard.h` (new): `mashed_re::DllGuardLoadSystem32(name)` =
  `LoadLibraryExA(name, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32)` plus the `dllguard: <dll> -> <path>` line in
  `mashed_re.log`. The D5-SHIM-GUARD delay-load hook now calls it, so its d3d9 / dinput8 behaviour and log line are
  unchanged.
- `Audio/AudioEngine.cpp` `Audio::Init`: `DirectSoundCreate8` is resolved with `GetProcAddress` on
  `DllGuardLoadSystem32("dsound.dll")`. That was the exe's only reference into `dsound.lib`, so `DSOUND.dll` drops out
  of the import table (dsound.lib stays on the link line, unreferenced). On failure, audio stays off and nothing falls
  back to the default search.
- `exe_main.cpp` WinMain: `DllGuardLoadSystem32("dsound.dll")` right after `InitDirectInput` and before
  `RunBootIntroMovies()`. Fail closed: if the System32 copy does not load, WinMain returns 5 before any movie or audio
  code runs. This matches the old behaviour, where the loader refused to start the exe without `DSOUND.dll`.
  Placement is after the B17 / B7 address-space work and InitD3D9, the same window where dinput8 already loads.
- `mashed_re_dev.asi` is untouched. `AudioEngine.cpp` is exe-only (`exe_sources.rsp:99`, not in `asi_sources.rsp`),
  and so is `exe_main.cpp`.

Import tables (`dumpbin /imports`):

| Exe | sha256 | Static imports | Delay-load imports |
|---|---|---|---|
| base (e23f260b7) | `53f208f4...` | USER32, **DSOUND (ordinal 11)**, GDI32, ole32, KERNEL32 | d3d9, DINPUT8 |
| guard (049eb27bd) | `e1c75289...` | USER32, GDI32, ole32, KERNEL32 | d3d9, DINPUT8 |

## 2. Dummy-proxy test

Driver: `dummy_dll_test.py` (the D5-SHIM-GUARD driver, with `dsound.dll` as the dummy, audio ON by default and
`GUARD_MUTE=1` for a muted arm). Dummy source: `dummy_dsound.c`. On `DLL_PROCESS_ATTACH` the dummy writes
`dsound.dll.LOADED.txt` beside itself and returns FALSE. Root `D:\MashedWT\D5-SHIM-GUARD-DSOUND_root`, G-CTRL base env,
1200 deterministic frames, intro movies at their default (ON), under `mashed_lock.py` and `test-slot`, waits on its own
PID only.

The first dummy exported `DirectSoundCreate8` by name only. `dsound.lib` imports it by **ordinal 11**
(System32 `dsound.dll`: `DirectSoundCreate` = 1, `DirectSoundCreate8` = 11), so the base exe failed in the loader with
`0xC0000138` (STATUS_ORDINAL_NOT_FOUND) before `DllMain`. That still shows the loader resolved `DSOUND.dll` to the dummy
(System32's copy has ordinal 11), but it leaves no marker. The second dummy (`_ord` rows) exports the System32 ordinals.

| Arm | Exe | Dummy | Exit | Marker | dllguard dsound lines |
|---|---|---|---|---|---|
| control_base | base | by-name | `0xC0000138`, 1 s | none (loader failed before DllMain) | log absent |
| control_base_ord | base | ordinals | `0xC0000142` (STATUS_DLL_INIT_FAILED), 1 s | **BOUND** | log absent |
| guard | guard | by-name | 0, 45 s | not bound | 2: WinMain preload + `Audio::Init` |
| guard_mute (`MASHED_MUTE=1`) | guard | by-name | 0, 40 s | not bound | 1: WinMain preload only |
| guard_ord | guard | ordinals | 0, 41 s | not bound | 2: WinMain preload + `Audio::Init` |

All guard rows resolve to `C:\WINDOWS\SYSTEM32\dsound.dll`, next to the unchanged
`d3d9.dll -> C:\WINDOWS\SYSTEM32\d3d9.dll` and `DINPUT8.dll -> C:\WINDOWS\SYSTEM32\DINPUT8.dll` lines.

### DirectSound is not reached before the guard (no audio endpoint on LIVING-PC)

The `guard` arm's `mashed_re.log` (lines 90-101) shows the order:

```
dllguard: DINPUT8.dll -> C:\WINDOWS\SYSTEM32\DINPUT8.dll
...
Milestone B11: DirectInput8 + keyboard device acquired (cooperative = background+nonexclusive)
dllguard: dsound.dll -> C:\WINDOWS\SYSTEM32\dsound.dll          <- WinMain preload
INTRO movie idx=1 original/toastart/pc/movies/empire.mpg: complete frames=3 ... audio_hr=0x80040256
INTRO movie idx=2 .../supersonic.mpg ... audio_hr=0x80040256
INTRO movie idx=3 .../renderware.mpg ... audio_hr=0x80040256
INTRO movie idx=4 .../intro.mpg ... audio_hr=0x80040256
INTRO small.mpg: BUILT (graph built, not run) video=720x576
dllguard: dsound.dll -> C:\WINDOWS\SYSTEM32\dsound.dll          <- Audio::Init
```

- The only DirectSound consumers in the exe are the movie graphs (audio-pin `Render`) and `Audio::Init`. All movie opens
  are after the preload: `g_small_mpg.Open` inside `RunBootIntroMovies` and `g_menu_video.Open` later in WinMain. No
  other `CoCreateInstance(CLSID_FilterGraph...)` site exists.
- `audio_hr=0x80040256` is VFW_E_NO_AUDIO_HARDWARE: DirectShow found no audio endpoint on this PC, after the System32
  dsound was already loaded. The dummy stayed unbound through all four movies.
- `Audio::Init` (audio ON, guard_ord) logs `[audio] Init: DirectSoundCreate8 failed hr=0x88780078` (DSERR_NODRIVER) in `log/mashed_re.log`: `GetProcAddress` reached the System32 `DirectSoundCreate8`, which reports no device. `Audio::Init` already treats that as non-fatal, so the exe ran its 1200 frames with audio off, the same as before on this PC.
- Under `MASHED_MUTE=1` (every gate arm) `Audio::Init` returns before DirectSound, so only the preload line appears.

## 3. Default gates (merge_train.json refs: G-CTRL 8284be46/ba407a16/31fe043e, B 1d5bf4f9 x3)

`run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, `A4_ROOT=D:/MashedWT/D5-SHIM-GUARD-DSOUND_root`,
`A4_EXE` = guard exe `e1c75289`:

| Arm | Exit | Result | Ref |
|---|---|---|---|
| H_DEF | 0, self-exited, 474 s | step 8284be46 (13962 lines), gates ba407a16 (54085), ptrace 31fe043e (27042) | match |
| B_DEF_K0 | 0, 40 s | 1d5bf4f9 (762) | match |
| B_DEF_K1 | 0, 40 s | 1d5bf4f9 (762) | match |
| B_DEF_K26 | 0, 41 s | 1d5bf4f9 (762) | match |

The H_DEF `mashed_re.log` carries all three `dllguard:` System32 lines (d3d9, DINPUT8, dsound). A first gate attempt died after H_DEF finished because `A4_OUT` did not exist (`gates_try1_nooutdir.txt`, FileNotFoundError in `run_arms.py` copy). It produced no hashes and was re-run in full.

The `-Full` extras (D2_DEFH arms, run13) were not run. The change touches no `full_set_paths`
(Vehicle/Ai/Collision); the merge train runs its own gate set.

## 4. Notes for the lead

- `.ps1` / `.bat` edits needed: none. LAN releases (`scripts/lan_release/publish.sh` through `build.bat`) pick up the
  change as is.
- Optional cleanup, not needed for the guard: `dsound.lib` on the exe link line in `mashedmod/build.bat` is now
  unreferenced, and the RUNTIME NOTE names only d3d9/dinput8. Proposed diff for a step that may edit `.bat`:
  ```
  -REM so the standalone no longer binds a proxy beside it. The rename loop below
  -REM stays as belt and braces for the build dir.
  +REM so the standalone no longer binds a proxy beside it. dsound.dll is no longer
  +REM imported (D5-SHIM-GUARD-DSOUND: DllGuard.h, loaded from System32 in WinMain).
  +REM The rename loop below stays as belt and braces for the build dir.
  ...
  -    user32.lib d3d9.lib dsound.lib gdi32.lib "%QHULL_LIB%" "%LIBRW_LIB%"
  +    user32.lib d3d9.lib gdi32.lib "%QHULL_LIB%" "%LIBRW_LIB%"
  ```
  If the lead prefers the literal `/DELAYLOAD` form instead, it is `/DELAYLOAD:d3d9.dll /DELAYLOAD:dinput8.dll
  /DELAYLOAD:dsound.dll` on the same line plus `dsound.dll` in the hook's name test. The WinMain preload before the
  movies would still be required (§1).
- Return code 5 is new in WinMain (System32 `dsound.dll` missing). 1 to 4 were already taken.

## 5. Raw files (D:/MashedWT/D5-SHIM-GUARD-DSOUND_data)

| File | SHA-256 |
|---|---|
| build_base.log | a129a6c675944b0822901c9a8f236ceb3d2d64d0a1906fc9f40f08262f09eb27 |
| build.log (guard) | 5bfa04bb4b88389e460c0f96b490e22937f8a8745d987651373903ccb4f4e026 |
| dummy_control.txt | b212b43d0b25d18eaae285e2dadfca10d5f969e8390f9d4c9ece71ad04cef389 |
| dummy_guard.txt | 9efe0f8d595deff0a64eee82caca11ce838487d3d408f958501fe8ebb58ed728 |
| dummy_guard_mute.txt | e501b1c665d8b077f4d14c9d267a1a23ded5e5de8303fe1a59868a699f319963 |
| dummy/guard.log | b7736005ae6bc7e86f0b405d18fcccce6a51123f91da837c9de63ee5e0b1d6e4 |
| dummy/guard_mute.log | a9b09c446f451d0f44d4d24915d912940aab3a73a2b8a65f4ba7f1a5cd822249 |
| dummy_bin/dsound.dll (by-name) | 4d271fda470e90f1dcb7d12ecd03a764cb14a90fe977967a07eb383b9a80e2d1 |
| dummy_bin2/dsound.dll (ordinals) | cf286ce2195997710bd35194109c51c3bf1eb13a88edb04f800879739d98f8b5 |
| bin/mashed_re_base.exe | 53f208f45e0d25908359ef694ffa83c1a7063f3697d1358be70487b872e19311 |
| bin/mashed_re_guard.exe | e1c752899ec74f635f140bbef93bf6827df16d50997aefacfd6fef5685e48d03 |
| dummy_control2.txt (control_base_ord) | efdb78baacf8a18ff9a5adf3ec8e708794d7887f142538a19ed34841109f3f69 |
| dummy_guard2.txt (guard_ord) | 4c6b3127a5d3b2a1d75a729e565c2144c8ca0ad493e5b9eeaa79702e8fa50bc1 |
| dummy/guard_ord.log | 4ae853a748c86b27a3bf8106d25b67d57caf92ba860fc487339c87cb0d2e22c0 |
| audio_log_after_guard_ord.txt (root `log/mashed_re.log`: guard_mute, gate try 1, guard_ord) | 9560bc4898bba36347a1fec8bd45fa0cb1fc1a3a0eba1f2acf411c44a7b45c0c |
| gates_try1_nooutdir.txt | 77574f3792a7801641d1b221ad939afabee6b3b01cf6a28bb199bf657cf2813c |
| gates.txt | 83c9dade6735337e998a2e37dd0ab5e9da0727b64399446e5926fc93413627a9 |
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K{0,1,26}.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 (all three) |
| arms/H_DEF.log | 8c83e90e10cac118a693771e9817060882e705511246be3991c37332b43b8121 |
