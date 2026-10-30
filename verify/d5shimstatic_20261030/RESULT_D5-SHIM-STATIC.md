# RESULT D5-SHIM-STATIC (static, text-only, 2026-10-30)

Method: grep and read on branch `orch/D5-SHIM-STATIC` (from `orch/integration` c36bbf631). No build, no run, no Ghidra, no source change.
Scope: exe target (`mashedmod/exe_sources.rsp`, `build.bat` exe leg, `exe_main.cpp`) and `scripts/lan_release`.
Paths are repo-relative. Line numbers are from this worktree.

## Verdict

**Exe needs the dev harness: NO** (static read). The exe links the real System32 d3d9 and dinput8 (`build.bat:238` `d3d9.lib`, `exe_main.cpp:228` `dinput8.lib`), has its own frame pacing and focus handling, and `exe_sources.rsp` has no shim, ASI or hook-install TU. Its only runtime read of `MASHED.exe` is as a data file for one icon texture.

Residual risks: see the [UNCERTAIN] rows (R11, R12). Both need a run or a PE import dump, which the quiet hold forbids.

## Table

| # | file:line | What it references | Standalone runs without it? | Removal / replacement step |
|---|---|---|---|---|
| R1 | `mashedmod/build.bat:199-207, 279-290` | RUNTIME NOTE + loop that renames stray `d3d9.dll`/`dinput8.dll` proxies out of `%OUT%` | Yes. Guards against a dev proxy sitting beside `mashed_re.exe` (Windows would bind it and fail with "missing dinput8_real.DLL"). Pure dev hygiene | D5-SHIM: delete the loop once the shims are removed from `mashedmod\build\` and no builder writes them there |
| R2 | `mashedmod/src/mashed_re/LibRw/RwBridge.cpp:237-241` | Comment: standalone "deliberately binds System32's d3d9, NOT the dev d3d9 shim that hosts MASHED_FPS_CAP" | Yes. States the exe path does not use the shim. Comment only | Reword when shims go. No code change |
| R3 | `mashedmod/src/mashed_re/exe_main.cpp:3081, 3121` | `PresentationInterval = D3DPRESENT_INTERVAL_ONE`, `MASHED_D3D_NOVSYNC` env opt-out | Yes. Exe paces by vsync plus its own QPC accumulator (`exe_main.cpp:3669-3677`, `3866-3868`), not by `MASHED_FPS_CAP`. Comment at `3669-3671` cites the "shim-capped at 60" original only as the target cadence | None. Note: exe has no reader of `MASHED_FPS_CAP`, `MASHED_FPS_LOG`, `MASHED_FPS_CAP_RACE` (grep of `mashedmod/src` outside `d3d9_shim`/`qol_asi`: zero code hits) |
| R4 | `mashedmod/src/mashed_re/exe_main.cpp:115, 3077-3078` | Backbuffer 800x600 (`kWidth`/`kHeight`), windowed | Yes. Exe sets its own size. The 640x480 coupling (`fix_camera_res` patch with the d3d9 shim backbuffer) is original-side only | None for the exe. Keep the coupling documented as dev-only when shims are retired |
| R5 | `mashedmod/src/mashed_re/exe_main.cpp:1018-1026` (and `:1665` per CLAUDE.md) | Focus handling. Exe never calls `WaitMessage()`, throttles with `Sleep(15)` unfocused | Yes. This replaces the `no_focus_pause` patch (`0x004996d3`) in the exe. No dependency on the patched image | None |
| R6 | `mashedmod/src/mashed_re/exe_main.cpp:8564-8583` (`LoadLoadIconFromExe`) | `LoadLibraryExA("original\\MASHED.exe", LOAD_LIBRARY_AS_DATAFILE)`, resource `RWTEXDICTIONARY`/404 | **Needs the file, not the patches.** Data-file load runs no code and reads a resource section the ten patches do not touch (patch addresses in CLAUDE.md are all `.text`/cave, none in resources). Failure path logs and returns false (`:8573-8576`), so a missing file degrades the loading disc only | Replacement: extract RWTEXDICTIONARY/404 once into a project asset and load that, then drop `MASHED.exe` from the LAN stage. Decision for D5-SHIM (shipping the original binary is a licensing and size question) |
| R7 | `scripts/lan_release/stage_lan_release.py:11-12, 99-102` | Stages `MASHED.exe.unpatched` (falls back to `MASHED.exe`) as `original\MASHED.exe`, for R6 only | Yes for runtime logic. It is the only artifact link between the release and the original binary. Prefers the pristine copy, so the release never carries patched bytes | Removed together with R6's replacement |
| R8 | `scripts/lan_release/stage_lan_release.py:21` | Exclusion list: d3d9/dinput8 proxies, `mashed_re_dev.asi*`, `MASHED.exe.pre*` are NOT staged | Yes. Confirms the release excludes the dev harness | None. If the shims are deleted, this exclusion list shrinks |
| R9 | `scripts/lan_release/publish.sh:13` | `MASHED_NO_DEPLOY=1` so the shared `original\mashed_re_dev.asi` is untouched | Yes. Build-time guard only | None. Simplifies once the asi leg is split from the exe leg |
| R10 | `mashedmod/build.bat:209-238, 242-276` | One script builds both targets. The exe leg (`:229`, `/DMASHED_STANDALONE`, `exe_sources.rsp`) and the asi leg (`:258`, `asi_sources.rsp`). `:264-276` copies `mashed_re_dev.asi` to `original\` unless `MASHED_NO_DEPLOY` | Yes for the exe. The exe leg does not depend on the asi leg outputs. A plain `build.bat` run still redeploys the dev asi (WORKER_RULES forbid it) | D5-SHIM: split the asi leg or make deploy opt-in. `.bat` edit |
| R11 | `mashedmod/exe_sources.rsp:132` `Compat\StandaloneRvaThunks.cpp` | Only `Compat` TU in the exe list. Grep of the rsp for `dll_main`, `dinput8_shim`, `d3d9_shim`, `qol_asi`, `InjectHooks` found no match. Name suggests thunks for RVA-addressed code | Statically no harness hook in the list. [UNCERTAIN] whether the thunks read memory that exists only inside `MASHED.exe`. Missing evidence: read `StandaloneRvaThunks.cpp` (not opened, out of the grep scope) | Open that file in a follow-up |
| R12 | `mashedmod/src/mashed_re/exe_main.cpp:542-` (B7 "MASHED data-section emulation"), `:102-104` | Exe emulates `.data` of `MASHED.exe` at the original absolute addresses. Comments state those addresses "don't exist in mashed_re.exe's address space" without it | Statically self-contained (it builds its own mapping). [UNCERTAIN] whether the mapping includes `.rdata` constants copied from the original image: missing evidence is a read of `:542-780` | Follow-up read. Not a shim dependency, but it is the closest thing to "assumes the original image" |
| R13 | `mashedmod/src/mashed_re/exe_main.cpp:9290-9320`, `:9298-9305` | Asset root search for `original\TOASTART\Common\Frontend.piz`, `MASHED_ROOT` env override | Needs the `original\` data folder, not the exe or patches. Not harness | None. Folder layout is the shipping layout (`stage_lan_release.py`) |
| R14 | `mashedmod/src/mashed_re/exe_main.cpp:9278` | `DirectInput8Create(GetModuleHandleA(nullptr)...)`, linked via `dinput8.lib` (`:228`) | Yes. Real System32 `dinput8.dll`. It would bind a proxy only if one sits beside the exe (R1) | None. R1 covers the hazard |
| R15 | `mashedmod/src/mashed_re/exe_main.cpp:9396, 9642` | Comments naming `setup_mashed_compat.ps1` (HIGHDPIAWARE on the original) and ENGLISH.DAT copy choice "MASHED.exe we diff against" | Comments only. No run-time read | None |
| R16 | `mashedmod/src/mashed_re/**` comments naming `MASHED.exe.unpatched` / SHA anchor (Boot/*, Math/*, Ai/*, Camera/*; about 40 files) | Provenance notes: bytes were read off the unpatched binary | Yes. Comments, no run-time effect | None. Optional: keep as provenance |
| R17 | `mashedmod/src/dinput8_shim/dinput8_shim.cpp:1-30, 83-96`; `build_dinput8_shim.bat`; `mashedmod/src/d3d9_shim/d3d9_shim.cpp:8, 344-374, 732-750`; `build_d3d9_shim.bat:9, 43-50` | The shims themselves: `d3d9_real.dll`/`dinput8_real.dll` forwarding, `MASHED_FPS_CAP`/`_RACE`/`_LOG`, asi autoload of `mashed_re_dev.asi` (next to dll, then `..\mashedmod\build\mashed_re_dev.asi`) | Not in `exe_sources.rsp`, not linked into the exe. Dev harness only | D5-SHIM removal target: these files, the two `.bat`, `original\d3d9.dll`, `original\dinput8.dll`, `original\*_real.dll`, `original\mashed_re_dev.asi`. Must come after the original-side Frida diff lane no longer needs them (they host the original-side frame cap and asi autoload) |
| R18 | `mashedmod/src/qol_asi/mashed_qol.cpp:14-20, 52`, `build_qol_asi.bat:4` | QoL asi loaded by the dinput8 proxy, conflicts with `mashed_re_dev.asi` | Not exe. Original-side only | Goes with R17 |
| R19 | `mashedmod/src/mashed_re/dll_main.cpp:1-2` | `mashed_re_dev.asi` entry point (Ultimate-ASI-Loader style) | In `asi_sources.rsp` only | Goes with R17 |

## Not found (stated so the table reads as complete)

- No exe-side reference to `d3d9_real.dll`, `dinput8_real.dll`, `mashed_re_dev.asi`, `MASHED_FPS_*` (code hits only in `d3d9_shim.cpp`, `dinput8_shim.cpp`, `mashed_qol.cpp`).
- No exe-side check of the `MASHED.exe` SHA-256 or of any patch byte. The only `MASHED.exe` open is R6, as a data file.
- `scripts/lan_release/*.bat|*.ps1|*.sh`: only `publish.sh:13` matched the harness patterns. `stage_lan_release.py` rows R7/R8.

## Limits of this audit

- Grep over `.cpp .h .hpp .c .rsp .bat .txt .inc` under `mashedmod`, with `head_limit 150` on the first sweep, then targeted greps on the exe TUs. Hits beyond line 150 of the first sweep were not read. They are Boot/Math/Ai/Camera comment hits of the R16 kind by the sort order shown, but this is [UNCERTAIN] for the unread tail.
- Import table of the built `mashed_re.exe` not dumped (needs a build/tool run). A PE import dump would settle that no `d3d9_real`/`dinput8_real` import exists. Static source grep says none.
- R11 and R12 need a read of the TUs named there.
- No tracker, `.ps1` or source edit was made.
