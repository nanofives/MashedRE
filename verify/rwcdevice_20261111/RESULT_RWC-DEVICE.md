# RESULT RWC-DEVICE: in-exe IDirect3D9 / IDirect3DDevice9 wrapper for the top-down path (L-124 (C) step 1)

Verdict: **GREEN.** Branch `orch/RWC-DEVICE` off orch/integration `778b8b91d`. Knob `MASHED_RWC_DEVICE=1`, default
OFF, read only on the `MASHED_ORIG_TOP` path. No new librw binding. No tracker edit. G-CTRL / B not run (parent brief
and WORKER_RULES top-down row). One caveat for the merge train in section 6.

## 1. What landed

| file | lines | what |
|---|---|---|
| `mashedmod/src/mashed_re/Compat/D3d9Wrap.cpp` | 491 | `WrapD3D9` (IDirect3D9) and `WrapDevice9` (IDirect3DDevice9) COM wrapper objects, the four behaviours below, a first-Present self-test |
| `mashedmod/src/mashed_re/Compat/D3d9Wrap.h` | 25 | `MashedD3D9Wrap_Enabled`, `_Direct3DCreate9`, `_RealDevice`, `_PresentCount` |
| `Compat/D3d9Wrap_IDirect3D9.inc`, `D3d9Wrap_IDirect3DDevice9.inc` | 19 + 121 | generated forwarding members, 17 + 119 methods in vtable order |
| `verify/rwcdevice_20261111/gen_d3d9_wrap.py` | | the generator (Windows SDK 10.0.26100 `shared/d3d9.h`) |
| `mashedmod/exe_sources.rsp` | +1 | the new TU, exe only |
| `mashedmod/src/mashed_re/exe_main.cpp` | +6 / -1 | `g_rwc_wrap` (set only in `PortTop_HandCreateDevice`), and `InitD3D9` takes its IDirect3D9 from the wrapper when it is set |

The shim's behaviours and where each one went (`mashedmod/src/d3d9_shim/d3d9_shim.cpp`):

| shim | wrapper |
|---|---|
| `CreateDevice_ForceWindowed` on IDirect3D9 vtbl[16]: `Windowed=TRUE`, backbuffer 640x480 (`MASHED_HIRES` 1280x960, `MASHED_RES` WxH), `FullScreen_RefreshRateInHz=0` | `WrapD3D9::CreateDevice`, same env, same fields; the returned device is a `WrapDevice9` |
| `ApplyWindowBorders` after CreateDevice (titled fixed border, client = backbuffer, pin at 64,64; `MASHED_WIN_POS`, `MASHED_RE_BORDERLESS`, `MASHED_QOL_BORDERLESS`, `MASHED_RE_NO_SCREEN1_PIN`) | same function, same env. When CreateDevice does not run on the window's thread it is applied at the first Present: the exe's `CreateDeviceWatched` runs CreateDevice on a worker while the window thread waits, so a SetWindowPos from the worker would block on it |
| `Present_BBDump` -> `FrameLimit` on IDirect3DDevice9 vtbl[17] (`MASHED_FPS_CAP` default 60, `MASHED_FPS_CAP_RACE` on DAT_00771968 phases 3 / 6, `MASHED_FPS_LOG`) | `WrapDevice9::Present` -> `FrameLimit`, same pacing and env; the log goes to `log\rwc_fps_limiter.txt` beside the exe |
| `patch_mashed_fix_camera_res` (getters 0x00498bc0 / 0x00498bd0 rewritten to return 640 / 480) | `AssertRenderDims`: the getters' cells 0x00616028 / 0x0061602c are set to the backbuffer size after CreateDevice and Reset and before every Present. The byte-matched getter bodies (`re/match/s16/00498bc0.c`, `00498bd0.c`) are untouched |

Not carried over, all dev instruments of the shim: `MASHED_ORIG_BBDUMP` / `_REQ` dumps, draw-call counters, parity
Clear colour, FPS OSD. `MashedD3D9Wrap_PresentCount` replaces `MashedShim_PresentCounter`.

Added beyond the shim: `WrapDevice9::Reset` forces the same parameters (the shim does not hook Reset; a Reset with
fullscreen parameters from the RW driver would undo the forced mode). `GetDirect3D` returns the IDirect3D9 wrapper,
`QueryInterface(IID_IDirect3DDevice9 / IID_IUnknown)` returns the wrapper, other IIDs go to the real object.

### Why writing the cells equals the getter patch

`scan_dim_refs.py` over `MASHED.exe.unpatched` `.text` (`DIM_REFS.txt`): the 4 address bytes of 0x00616028 /
0x0061602c occur 4 times:

| target | operand at | opcode | |
|---|---|---|---|
| 0x00616028 | 0x00498bc1 | `a1` mov eax,[0x00616028] | getter 0x00498bc0 (read) |
| 0x00616028 | 0x00499604 | `89 15` mov [0x00616028],edx | FUN_00499400 (write) |
| 0x0061602c | 0x00498bd1 | `a1` mov eax,[0x0061602c] | getter 0x00498bd0 (read) |
| 0x0061602c | 0x0049961c | `a3` mov [0x0061602c],eax | FUN_00499400 (write) |

The getters are the only readers, so cells = backbuffer gives every caller the patched value. The one window where it
does not: a getter call between a FUN_00499400 write and the next Present. [UNCERTAIN] whether the RW-core path calls
a getter in that window; the wrapper logs every re-assert (first 8), and none appeared after CreateDevice in the runs.

## 2. Builds

| build | result |
|---|---|
| default (`MASHED_NO_DEPLOY=1`, VS2022) | Build OK, `mashed_re.exe` sha256 `f37c0db5...` |
| object build (`MASHED_DDKOBJ=1`) | link rc 0, `mashed_re_ddk.exe` sha256 `f02546f6...`; units 583, DDK objects 540, refusals 20 lines `fail:` as in the spike |
| `ddkobj.py check` | IDENTICAL 540, DIFFERENT 8, not-in-map 3. Row by row equal to `verify/ddkobj_20261111/BYTECHECK_all.tsv` (0 rows changed). The 8 DIFFERENT are the spike's 7 below-100% sources and 0x00414060. `BYTECHECK.tsv` committed |

## 3. Knob boots

Private root `D:/MashedWT/RWC-DEVICE_root` (junction `original` -> `D:/MashedWT/PORT-TOP-04J_rootK/original`), driver
`verify/porttop_20261108/knob_boot.py`, every launch in `mashed_lock.py run` inside `test-slot`,
`MASHED_ORIG_TOP=1 MASHED_WIN_POS=primary-bl`.

| arm | exe | env | result |
|---|---|---|---|
| vs_on_off | VS2022 | `MASHED_DET_FRAMES=3000` | rc 0, 125.9 s, `returned 0 after 3000 frames` |
| vs_on_rwc | VS2022 | + `MASHED_RWC_DEVICE=1` | rc 0, 59.8 s, 3000 frames, **self-test PASS** |
| ddk_on_off | object build | `MASHED_DET_FRAMES=3000` | rc 0, 49.8 s, 3000 frames |
| ddk_on_rwc | object build | + `MASHED_RWC_DEVICE=1` | rc 0, 76.0 s, 3000 frames, **self-test PASS** |
| vs_on_rwc_res800 | VS2022 | + `MASHED_RES=800x600`, 600 frames | rc -1: CreateDevice HUNG in the exe watchdog (HAL, then REF), InitD3D9 failed |
| vs_on_off_res800 | VS2022 | `MASHED_RES=800x600`, knob OFF, 600 frames | rc 0, 600 frames |
| vs_on_rwc_res800b | VS2022 | rerun of the hung arm | rc 0, 600 frames, coupling fired, **self-test PASS** |

Self-test line (vs_on_rwc and ddk_on_rwc, identical):
`selftest PASS (0 fail): backbuffer=640x480 windowed=1 getters=640x480 GetDirect3D=wrapper QI=wrapper client=640x480`.
In the 640x480 runs the cells already held 640x480, so the coupling wrote nothing. The 800x600 rerun exercises it:
`coupling at CreateDevice: getter cells 0x00616028/0x0061602c were 640x480, set to the backbuffer 800x600`, then
`getters=800x600 ... client=800x600`.

Log comparison (`logcmp.py`: `[ORIG_TOP]` / `[RW_BIND]` lines, heap pointers, HWNDs and timings masked, original
addresses kept):

| pair | lines | differing |
|---|---|---|
| vs_on_off vs vs_on_rwc | 1133 / 1133 | 0 |
| ddk_on_off vs ddk_on_rwc | 1133 / 1133 | 0 |
| vs_on_rwc vs ddk_on_rwc | 1133 / 1133 | 0 |

The hung arm: both the HAL and the REF CreateDevice timed out (6 s each) inside the real `CreateDevice` call (the
wrapper's post-call log line is absent). The rerun of the same arm passed, and so did the knob-OFF control. The exe's
watchdog exists for this intermittent driver hang (`exe_main.cpp` `CreateDeviceWatched`, 2026-07-14). [UNCERTAIN] 1 hang
in 5 wrapper-ON boots: the runs do not exclude a wrapper contribution. More ON boots would narrow it.

Menu walk and 12 captures not run: no menu code changed, and the device wrapper does not change what is drawn.

## 4. What this does not yet cover

- The matched RW D3D9 driver (`_rwDeviceSystemFn` 0x004c7a70 and its callees) does not run yet. Today the knob path's
  device comes from the exe's `InitD3D9` (librw adopts it, `MashedRw_EngineStart_004c2fb0`), so the wrapper sees the
  exe's windowed 640x480 request and forces nothing. The fullscreen-to-windowed forcing and Reset run first when
  RWC-CORE-RUN routes the RW driver's `Direct3DCreate9` to `MashedD3D9Wrap_Direct3DCreate9`. How a matched or DDK
  object reaches that name (the import `__imp__Direct3DCreate9@4` in the DDK object) is RWC-CORE-RUN's binding work.
- The frame limiter adds wall-clock pacing on top of `D3DPRESENT_INTERVAL_ONE`. The deterministic logs are equal ON
  against OFF, so it moves no computed value in these runs.

## 5. Raw data

`D:/MashedWT/RWC-DEVICE_data`, sha256 in `RAW_SHA256.txt`: both build logs, the 7 knob-boot logs, the two driver
transcripts.

## 6. For the merge train

The knob is read only on the top-down path, but `exe_main.cpp` `InitD3D9` is also the default path's device init: it
now evaluates `g_rwc_wrap` (always false there) before the same `Direct3DCreate9` call, and the exe links one more TU.
So the default exe's machine code changed. Per the parent brief the worker ran no G-CTRL / B. The batch's default gates
cover it. [UNCERTAIN] a code-layout effect on rouabout / Storm / Highway (memory "Sim trajectory is code-layout
sensitive"); TRAINING G-CTRL is not layout-sensitive by that note.
