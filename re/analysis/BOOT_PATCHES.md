# Boot patches, compat shim, and d3d9 proxy — full root-cause narratives

Extracted verbatim from CLAUDE.md "Runtime state" on 2026-07-01 (token-economy
Tier 3 trim). CLAUDE.md keeps the one-line summaries; the detail lives HERE.
One-command recovery after a wipe/restore: `py -3.12 scripts/repatch_original.py`.

## The binary patches (original/MASHED.exe)

All idempotent; each script self-checks; each reversible via `--restore`.
`original/MASHED.exe.unpatched` is the backup that matches the SHA-256 anchor — never delete it.

- ~~`scripts/patch_mashed_skip_powerups.py`~~ — **DO NOT APPLY.** Root-caused 2026-06-01 as boot crash #2 (the NOP at 0x40295d causes a downstream stack-imbalance ret-to-0). Removing it made baseline boot to the real main menu (verify/boot_powerups_removed.png). The script now refuses to apply and un-applies if found.
- `scripts/patch_mashed_show_windowed.py` — unhides `[Window]` entries in the video selector.
- `scripts/patch_mashed_skip_audio_com.py` — neutralizes `FUN_005bc750` (real null-deref fix).
- `scripts/patch_mashed_skip_selector.py` — silent video selector.
- `scripts/patch_mashed_skip_controller_dialog.py` — silent controller selector.
- `scripts/patch_mashed_fix_camera_res.py` — **display-independence fix (2026-06-13).** Forces the screen-dim getters `FUN_00498bc0`/`FUN_00498bd0` to return 640×480 (the shim's forced backbuffer) instead of the auto-selected desktop video mode. Without it, MASHED builds the active camera's frameBuffer raster at the desktop resolution (e.g. 2560×1440); that render-target raster fails to create against the shim's 640×480 backbuffer → null camera raster → AV at 0x004c7785 ~4 s into boot. This made boot depend on the monitor topology. Root cause + evidence: `re/analysis/BOOT_CRASH_ROOTCAUSE_2026-06-13.md`. COUPLED to the shim's 640×480 (keep in sync).
- `scripts/patch_mashed_disable_log.py` — **boot fix (2026-06-20).** Neutralizes the 3 boot-path file-log functions (`FUN_00496490` open→ret0, `FUN_00496400` printf→ret, `FUN_004963e0` write je→jmp). After a ~2026-06-10 Windows servicing update, stock boot AVs ~4 s in at `RtlEnterCriticalSection` (ntdll+0x542f0, ECX=0x5477): the log `FILE*` global `DAT_00772fbc` (PE-init 0) holds garbage `0x5453` at boot (layout-dependent wild write), so the log `fputs()` enters a bad critical section. **This is NOT heap** — the 06-10 ntdll layout shift made `+0x542f0` = `RtlEnterCriticalSection`, so the prior "RtlpHeap / EMULATEHEAP heap-inversion" reading (see compat-shim section) is WRONG for this crash; no compat shim (EMULATEHEAP / page heap / segment heap) fixes it. Logging to `mashed.log` is cosmetic; disabling it boots to the menu (`verify/_boot_fixed_menu.png`). Caveats: it MASKS the broken log (the wild write still happens, harmless once readers are no-ops, source not yet root-caused); Frida-**spawn** still crashes (perturbs layout) so record via **attach-after-boot**. Full backup `MASHED.exe.prelogpatch`. Root cause + evidence: memory `project-boot-crash-static-init-not-heap`. **Partial alone (~80% boot — a lottery over which fopen caller hit the garbage first); the reliable root fix is `patch_mashed_fix_fopen.py` below.**
- `scripts/patch_mashed_fix_fopen.py` — **ROOT boot fix (2026-06-20); makes boot reliable (8/8).** The same crash class generalized: the fopen wrapper `FUN_004a4541` (`_fsopen`) returns a garbage `FILE*` (`~0x5453`, i.e. `< 0x10000`) to **all 6 of its callers** on direct launch, so boot is a lottery over which caller runs first (the log was just the most common). Valid `FILE*`s are always `≥ 0x10000` (static `_iob` in .data + heap allocs are high; null page is `0..0x10000`), so this redirects `FUN_004a4541` (jmp) to a 0xCC code cave at RVA `0x50f6c4` that does the original `_fsopen` call then `if (result < 0x10000) result = 0` — every caller already guards `if (fp != NULL)` and skips cleanly. Verified: boots to menu 8/8, menu renders (`verify/_boot_fixed_fopen.png`). Frida-**spawn** still crashes (perturbs layout) → record via **attach-after-boot** (`re/frida/record_attach.py`). Backup `MASHED.exe.prefopenpatch`. See memory `project-boot-crash-static-init-not-heap`.
- `scripts/patch_mashed_fix_joypad.py` — **boot fix (2026-06-21); reliable boot WITH a game controller connected.** With a pad attached, stock boot AVs ~4 s in at `0x00495883` inside the per-device input poll `FUN_00495870` (`mov eax,[edi]`, `edi=[esi]`=garbage `0x3eb33333` = the float `0.35f` reinterpreted — one device-array slot's interface pointer is bad). The caller `FUN_00495fe0` loops `[0x772fac]` devices over the 4-slot array `0x771e88` (stride `0x448`) and derefs each slot's pointer unconditionally; keyboard-only boots fine (so the crash is "intermittent" = pad-dependent). Fix: redirect `FUN_00495870`'s entry (jmp) to a 39-byte 0xCC cave at RVA `0x508bd5` that re-runs the prologue, loads `[esi]`, and skips the whole poll (`add esp,0x6c; ret`, matching the real plain-`ret` epilogue at `0x495edd`) when the pointer is misaligned (`test al,3`) or `IsBadReadPtr` (IAT `0x5cc168`) — else jmps back to `0x495878`. The /GS cookie (in `eax`) is preserved across the probe. Verified **boots 8/8 with a controller connected** (was 0/6). Backup `MASHED.exe.prejoypadpatch`. See memory `project-replay-determinism-and-boot-patches`.
- `scripts/patch_mashed_skip_movies.py` — **OPTIONAL: skip small.mpg's DirectShow graph (2026-06-21; CORRECTED + verified 2026-06-22).** Boot-init `FUN_00402750` has two movie roots: `0x402838 call FUN_00495350` (intro loop — MUST keep running, it also pumps the D3D present/render loop later boot code needs; NOPping it AVs ~`0x402879`, 5/5) and `0x40283f call FUN_00494c80` (small.mpg). This NOPs **only the 5-byte `call` at `0x40283f`** and **KEEPS the `push 0` at `0x40283d`**. ⚠️ CALLING-CONVENTION CORRECTION: the original version NOP'd 7 B (incl. the `push 0`) believing `FUN_00494c80` is stdcall — it is NOT. `FUN_00494c80` ends in a plain `ret` (`c3` @ `0x494ed8`; cdecl/void, cleans nothing). The `push 0` is a **deferred-cleanup stack slot** popped later by the batch `add esp,0x24` @ `0x402888` (boot-init also uses esp-relative locals `lea [esp+0x24]`/`[esp+0x2c]` just before it). Removing the `push 0` shifted `esp` by 4 → over-pop → **deterministic 4-byte stack imbalance → `eip=0` crash ~menu+several-sec** (`esp=0x1afe50`/`ecx=0xff000000` — the long-chased signature; NOT joysticks, NOT the movies, NOT a "menu→race fps" crash). Proven 2026-06-22: with the `push 0` kept, small.mpg-skip is stable (menu +32 s, no crash). The script auto-detects/repairs the old broken 7-NOP form. **Redundant for the intro-skip goal** — `patch_mashed_skip_intro.py` (empties) is the preferred intro-skip and lets `FUN_00494c80` run normally. Backup `MASHED.exe.premoviepatch`.
- `scripts/patch_mashed_skip_intro.py` — **PREFERRED intro-skip (file-based; re-validated 2026-06-22).** Replaces the 5 intro `.mpg` files in `original/toastart/pc/movies/` with empty 1-frame MPEGs (originals backed up to `movies/backup/`). The empties play instantly (boot→menu <2 s) and are **stable** on the clean 7-boot baseline (menu +32 s, no crash, real menu renders — user-confirmed). The earlier claim that the empties crash the DirectShow path was WRONG: that crash was `patch_mashed_skip_movies.py`'s stack-imbalance bug (above), which had been applied alongside the empties. `FUN_00494c80` runs normally with the empties (small.mpg is a valid 1-frame movie). Approach borrowed from SciLor's MashedRunner (re/prior_art/MashedRunner/Intro.cs).

## Compat shim (per-machine)

One-time per-machine via `scripts/setup_mashed_compat.ps1`.
Must NOT include `DISABLEDXMAXIMIZEDWINDOWEDMODE` while the d3d9 shim is deployed
(the AcLayers d3d9 hooks deadlock against our proxy at process init).
`EMULATEHEAP` is **BUILD-DEPENDENT and the relationship INVERTS across Win11
builds** — the setup script toggles it on `[Environment]::OSVersion.Build`:

- **Build 26100** (EMULATEHEAP dropped 2026-06-16): `WIN98RTM`+`EMULATEHEAP`
  together heap-corrupt MASHED at CRT init → `0xC0000005` WRITE in
  `ntdll!RtlpHeap` (+0x542f0); boots clean WITHOUT EMULATEHEAP.
- **Build 26200+** (EMULATEHEAP re-added 2026-06-16, after an overnight feature
  update 26100→26200 + reboot re-broke boot): now the **native** heap is what
  MASHED's legacy CRT init corrupts (IDENTICAL signature: ntdll +0x542f0,
  `ECX=0x5477`, heap base 0x30000, CRT ret-chain
  0x4ac660/0x4a5f49/0x4a4274/0x49644c) **regardless of compat layer / apphelp /
  our .asi / our d3d9 proxy** (proven by isolation). EMULATEHEAP's legacy heap
  emulation side-steps it. Working layer on 26200+ = `~ RUNASINVOKER WIN98RTM
  HIGHDPIAWARE EMULATEHEAP` (boots + `run_diff` GREEN). See memory
  `project-emulateheap-boot-av`.

**If MASHED suddenly AVs at boot** (Frida attach fails `VirtualAllocEx 0x5`):
(a) parse the newest `%LOCALAPPDATA%\CrashDumps\MASHED.exe.*.dmp` with
`scripts/parse_minidump.py` — identical `ECX=0x5477` / CRT ret-chain ==
the heap-inversion class above → re-run `setup_mashed_compat.ps1` (it picks
EMULATEHEAP by build) or flip `$useEmulateHeap` if the inversion point moved;
(b) else check the layer for stray `EMULATEHEAP`/`DISABLEDXMAXIMIZEDWINDOWEDMODE`;
(c) else **clear the PCA Store** (`AppCompatFlags\Compatibility Assistant\Store`):
after a crash storm the Program Compatibility Assistant applies its own
heap/FTH-style shim *outside* the Layers key — `setup_mashed_compat.ps1` clears
both, re-run it to recover.

## d3d9 shim (windowed launch + frame limiter)

Built and deployed via `mashedmod\build_d3d9_shim.bat`. This proxy intercepts
`IDirect3D9::CreateDevice` to force `Windowed=TRUE`+640×480 — without it, MASHED
runs fullscreen, and the resolution-mode switch between launches causes monitor
flicker. 640×480 matches the lowest fullscreen mode so HUD/sprite assets render
without stretching. The build script auto-copies `SysWOW64\d3d9.dll` to
`original\d3d9_real.dll` (one-time; loader-dedup workaround). The proxy also
hosts a **frame limiter** in its `Present` hook (installed unconditionally):
MASHED is frame-COUPLED with no native cap, so it runs ~360 FPS / ~6× too fast
on modern GPUs — the limiter paces `Present` to a target FPS. Tunable via env
`MASHED_FPS_CAP` (default **60**; lower e.g. 30 for slower play; 0 disables).
`MASHED_FPS_LOG=1` logs measured FPS to `log/fps_limiter.txt`. Rebuild the shim
after editing. See memory `project-frame-limiter-speed-fix`.
