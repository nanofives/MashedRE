// RWC-DEVICE (L-124 (C) step 1): the in-exe IDirect3D9 / IDirect3DDevice9 wrapper for the top-down path.
// EXE ONLY (exe_sources.rsp). Knob MASHED_RWC_DEVICE=1, default OFF, consulted only on the MASHED_ORIG_TOP path.
//
// It carries into the exe what the dev d3d9 proxy (mashedmod/src/d3d9_shim/d3d9_shim.cpp) does for MASHED.exe on this
// machine, so that the matched RW D3D9 driver can create and present its device here the way the original does there:
//   - IDirect3D9::CreateDevice (vtbl slot 16) forced windowed, backbuffer 640x480 (MASHED_HIRES / MASHED_RES as the shim)
//   - IDirect3DDevice9::Present (vtbl slot 17) runs the frame limiter (MASHED_FPS_CAP, default 60; MASHED_FPS_CAP_RACE)
//   - the device window gets the shim's titled border, client area = backbuffer (MASHED_RE_BORDERLESS=1 opts out)
//   - the fix_camera_res coupling: the screen-dim getters 0x00498bc0 / 0x00498bd0 return the backbuffer size.
// See verify/rwcdevice_20261111/RESULT_RWC-DEVICE.md.
#pragma once

#include <windows.h>
#include <d3d9.h>

extern "C" {
// MASHED_RWC_DEVICE (read once). Default OFF.
bool MashedD3D9Wrap_Enabled(void);
// The wrapper's Direct3DCreate9: the real IDirect3D9 inside the wrapper, or null when the real call fails.
IDirect3D9* MashedD3D9Wrap_Direct3DCreate9(UINT sdkVersion);
// The real device inside a wrapped device (the pointer itself when it is not a wrapper).
IDirect3DDevice9* MashedD3D9Wrap_RealDevice(IDirect3DDevice9* dev);
// Present calls seen by the wrapper.
LONG MashedD3D9Wrap_PresentCount(void);
}
