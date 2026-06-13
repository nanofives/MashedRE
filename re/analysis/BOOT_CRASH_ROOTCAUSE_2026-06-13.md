# Boot crash root-cause + fix (2026-06-13): camera raster vs forced backbuffer

## Symptom
`original\MASHED.exe` AVs `0xC0000005` ~4 s into boot (read of address `0x1c`) at
`MASHED.exe+0xc7785`. Prior sessions mis-diagnosed this as a "no-display / D3D9
wedge requiring a reboot." It is neither — it is a deterministic null-pointer
deref whose trigger is the **display resolution**, so boot depended on the
monitor topology (worked at 3 monitors, crashed after dropping to 1).

## Evidence (probes, all committed under re/frida/)
- `d3d9_createdev_probe.py`: `IDirect3D9::CreateDevice` **SUCCEEDS** — adapterCount=1,
  HAL, bbW=640 bbH=480, `hr=0`, non-null device. So D3D9 device creation is fine.
- `boot_crash_probe.py`: fault at `0x004c7785`, `mov cx,[edi+0x1c]`, `edi=NULL`.
- `raster_create_probe.py`: `RwRasterCreate(w=2560,h=1440,flags=0x2) -> 0` (FAIL);
  same size `flags=0x1` (zBuffer) succeeds; `FUN_004670a0` (camera builder) returns 0.

## Chain (Ghidra, MASHED.exe)
1. `FUN_00499400` (video-mode setup; the selector silenced by
   `patch_mashed_skip_selector.py`) sets the screen-dim globals from the SELECTED
   video mode: `DAT_00616028 = mode.width`, `DAT_0061602c = mode.height`. With the
   selector skipped, MASHED auto-picks the desktop's best mode (here **2560x1440**).
2. `DisplayInit` (`FUN_004921d0`) reads them via `FUN_00498bc0`/`FUN_00498bd0`
   (the ONLY readers of those globals — Ghidra `reference_to`) → W=2560, H=1440.
3. `FUN_00467110` → `FUN_004670a0` builds the active camera `DAT_006905b0` and
   creates its frameBuffer raster: `RwRasterCreate(W,H,0,flags=2)` =
   `RwRasterCreate(2560,1440,0,2)`.
4. That render-target raster **fails to create against the d3d9-shim-forced
   640x480 backbuffer** → returns 0 → `FUN_004670a0` fails its `[0x60]&&[0x64]`
   check, calls cleanup `FUN_00467020`, returns 0 — but leaves `DAT_006905b0`
   pointing at a camera whose `[0x60]` (frameBuffer raster) is NULL, and the
   caller ignores the failure.
5. `FUN_0042d560` (derived-camera init) → `FUN_0042d4a0` inherits the viewport by
   copying `active->[0x60]`/`[0x64]` rasters via `FUN_004c7760`, which derefs the
   NULL source raster at `+0x1c` → **AV at 0x004c7785**.

## Root cause
The camera frameBuffer raster is created at the **selected video mode size**
(desktop best mode), but the d3d9 shim forces the device **backbuffer to 640x480**.
When the auto-picked mode is larger than the device can back a render target for,
the camera-raster create fails and boot derefs the resulting null raster. Hence
the dependency on the monitor configuration.

## Fix (display-independent)
`scripts/patch_mashed_fix_camera_res.py` patches the two screen-dim getters to
return the shim's forced backbuffer size instead of the selected-mode globals:

    FUN_00498bc0 (0x00498bc0):  mov eax,[0x616028];ret  ->  mov eax,640;ret
    FUN_00498bd0 (0x00498bd0):  mov eax,[0x61602c];ret  ->  mov eax,480;ret

The getters are the sole readers of `DAT_00616028/2c`, so 640x480 becomes the
effective internal resolution everywhere (camera, viewport, HUD), matching the
640x480 backbuffer the shim presents 1:1. The camera frameBuffer raster is then
always created at exactly the device backbuffer size → succeeds on ANY display.

## Verification
- `raster_create_probe.py`: `RwRasterCreate(640,480,0,2) -> non-null`; camera
  builder returns non-null.
- boot survival probe: MASHED alive 15 s, window `MASHED [D3D9] [Release]`.
- `run_diff.py get_771e78`: LUT ready, root populated, 10/10 GREEN — the booted-
  game diff lane is OPEN. This unblocks the C2->C3 run_diff grind regardless of
  display configuration.

## Coupling
640x480 must equal the shim's `kForceBackBufferWidth/Height`
(`mashedmod/src/d3d9_shim/d3d9_shim.cpp`). If the shim's forced backbuffer changes,
update `WIDTH/HEIGHT` in the patch script to match.
