# FUN_004c9fb0 — D3D9 viewport / render-target reassign on window change

**RVA:** 0x004c9fb0  
**Body:** 0x004c9fb0 – 0x004ca157  
**Status promoted:** C1 → C2 (batch-render-4-s5)  
**Confidence rubric:** C2 — decompilation read; callee/caller graph confirmed; semantics documented.

---

## Signature

```c
undefined4 FUN_004c9fb0(int param_1, HWND param_2);
```

param_1 = viewport context struct ptr  
param_2 = new HWND  
Returns: 1 if reassignment performed; 0 if no change needed.

---

## Callers

None detected — called via function pointer.

## Callees

- `GetWindowRect` (Win32 external)
- `FUN_004c7650` @ 0x004c7650 — surface/viewport release
- `FUN_004c77c0` @ 0x004c77c0 — surface/viewport allocate

---

## Mechanics

Guards on `DAT_00912100 != 0` (multi-viewport mode?) AND `DAT_007d4104 != param_2` (window actually changed).

If both true:
1. `GetWindowRect(param_2, rect)` to get new window dimensions.
2. Release existing viewports: `*(param_1 + 0x60)` and `*(param_1 + 100)` via `FUN_004c7650` if non-zero.
3. Copy current `D3DPRESENT_PARAMS` (0xe DWORDs from `DAT_009120e0`) to local `local_38`.
4. Set `local_38[0] = (rect.right == rect.left)` (zero-width flag), `local_38[1] = (rect.bottom == rect.top)` (zero-height flag).
5. Set `local_1c = param_2` (new HWND); `local_14 = NULL`.
6. Call `IDirect3DDevice9::ResetEx` or `CreateAdditionalSwapChain` via vtable+0x34 with modified present params.
7. If that returns 0:
   - Release and clear via vtable+0x14, vtable+0x30.
   - `FUN_004c77c0(rect.top, rect.right, 0, 0x82)` → viewport object. If non-null: fill in top/right/depth/color at offsets +0xc/+0x10/+0x14/+0x23; write context offsets via `DAT_00911ae4`.
   - `FUN_004c77c0(rect.top, rect.right, 0, 1)` → depth object. If non-null: store at `*(param_1 + 100)`.
   - Call vtable+0x08 on `local_1c` (AddRef/update).
   - Return 1.
8. Else return 0.

### [UNCERTAIN U-5000]
The exact semantics of vtable+0x34 call are unclear from decompilation — it may be `CreateAdditionalSwapChain` or a custom device operation. Evidence needed: cross-reference with D3D9 vtable layout + callee context.

### Key globals

| Address | Role |
|---------|------|
| `DAT_00912100` | Multi-viewport / windowed flag |
| `DAT_007d4104` | Primary HWND |
| `DAT_009120e0` | D3DPRESENT_PARAMS |
| `DAT_007d4110` | IDirect3DDevice9* |
| `DAT_00911ae4` | Viewport struct base offset |
| `DAT_00911b00` | Color/type table |

---

## Notes

- Previously had C1 plate at `re/analysis/bucket_004c4270/0x004c9fb0.md` (batch-x-s3).
- U-5000 raised for vtable+0x34 identity.
