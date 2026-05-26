# FUN_004c9cd0 — D3D9 device-lost / Reset handler (RESET_FN)

**RVA:** 0x004c9cd0  
**Body:** 0x004c9cd0 – 0x004c9e8a  
**Status promoted:** C1 → C2 (batch-render-4-s5)  
**Confidence rubric:** C2 — decompilation read; callee/caller graph confirmed; semantics documented.

---

## Signature

```c
int FUN_004c9cd0(void);
```

Returns: 1 if device is ready/recovered; 0 if device lost or reset failed.

---

## Callers

- `FUN_004d07b0` @ 0x004d07b0 — per-frame render gate.

## Callees

- `FUN_004c9ad0` @ 0x004c9ad0 — pre-reset resource release
- `FUN_004d1e30` @ 0x004d1e30 — subsystem re-init (post-reset)
- `FUN_004dc970` @ 0x004dc970 — subsystem re-init
- `FUN_004d6200` @ 0x004d6200 — subsystem re-init
- `FUN_004db3e0` @ 0x004db3e0 — subsystem re-init
- `FUN_004e0920` @ 0x004e0920 — subsystem re-init

---

## Mechanics

Standard D3D9 cooperative-level / device-lost loop:

```
TestCooperativeLevel() [vtable+0x0C]
  == D3DERR_DEVICELOST (-0x7789f797 = 0x88760009): return 0 (still lost)
  == D3DERR_DEVICENOTRESET (-0x7789f794 = ...): try Reset
  else: proceed to BeginScene
```

### Step 1 — TestCooperativeLevel
`(*(*DAT_007d4110 + 0x0C))()`.  
- If not D3DERR_DEVICELOST and not D3DERR_DEVICENOTRESET: jump to BeginScene check.

### Step 2 — Pre-reset
`FUN_004c9ad0()` — releases all D3D references, invalidates RS cache.

### Step 3 — Reset (D3DERR_DEVICENOTRESET path)
If `DAT_00912100 == 0` (windowed?): set `DAT_007d4140 = 0`; return 0.  
Otherwise: re-query current adapter display mode via `IDirect3D9::EnumAdapterModes` (vtable+0x1c) → `DAT_009120e0` (D3DPRESENT_PARAMS); translate D3DFORMAT to bit-depth → `DAT_00911f94`.  
Call `IDirect3DDevice9::Reset(DAT_007d4110, &DAT_009120e0)` (vtable+0x40).

### Step 4 — Post-reset re-init (if Reset succeeded)
Re-acquire surface pointers:
- `GetRenderTarget(0, &DAT_007d4118)` (vtable+0x98); `Release(DAT_007d4118)` (vtable+0x08)
- `GetDepthStencilSurface(&DAT_007d4114)` (vtable+0xA0); `Release(DAT_007d4114)`

Re-init subsystems:
```
FUN_004d1e30() && FUN_004dc970() → FUN_004d6200()
Restore lights: loop i=0..DAT_007d456c-1: SetLight(i, ...) (vtable+0xCC), LightEnable(i, ...) (vtable+0xD4)
FUN_004db3e0() → FUN_004e0920()
```

If optional callback `DAT_007d459c != NULL`: call it.

### Step 5 — BeginScene
If `DAT_007d4140 == 0`:  
`IDirect3DDevice9::BeginScene()` (vtable+0xA4); if succeeded, `DAT_007d4140 = 1`; return 1.  
Return `DAT_007d4140`.

### Key globals

| Address | Role |
|---------|------|
| `DAT_007d4110` | IDirect3DDevice9* |
| `DAT_007d4108` | IDirect3D9* |
| `DAT_007d410c` | Adapter index |
| `DAT_009120e0` | D3DPRESENT_PARAMS |
| `DAT_00911f90` | Current D3DFORMAT |
| `DAT_00911f94` | Bit depth derived from format |
| `DAT_00912100` | Windowed flag |
| `DAT_007d4140` | BeginScene-active flag |
| `DAT_007d4118` | Back-buffer render target |
| `DAT_007d4114` | Depth-stencil surface |
| `DAT_007d456c` | Light count |
| `DAT_007d4570` | Light array ptr |
| `DAT_007d459c` | Optional post-reset callback |

---

## Notes

- Previously had C1 plate at `re/analysis/render_d3d_reset/004c9cd0.md` (U-0707 S-0700 D-2020).
- No new UNCERTAINTIEs raised.
