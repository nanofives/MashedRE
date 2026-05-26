# FUN_004cb0b0 — D3D9 SetPixelShader with change-check

**RVA:** 0x004cb0b0  
**Body:** 0x004cb0b0 – 0x004cb0e2  
**Status promoted:** C1 → C2 (batch-render-4-s5)  
**Confidence rubric:** C2 — decompilation read; callee/caller graph confirmed; semantics documented.

---

## Signature

```c
void FUN_004cb0b0(int param_1);
```

param_1 = pixel shader handle / state value.

---

## Callers

- `FUN_0049a750` @ 0x0049a750
- `FUN_004db770` @ 0x004db770

## Callees

None (call through IDirect3DDevice9 vtable).

---

## Mechanics

Change-checked `SetPixelShader` wrapper:

```c
if (DAT_006181d0 != param_1) {
    iVar1 = IDirect3DDevice9_vtable[0x170/4](DAT_007d4110, param_1);  // SetPixelShader
    DAT_006181d0 = -1;
    if (iVar1 >= 0) DAT_006181d0 = param_1;
}
```

`IDirect3DDevice9::SetPixelShader` is at vtable offset 0x170 (index 92).  
No secondary cache invalidation (unlike `FUN_004cb070`).

### Key globals

| Address | Role |
|---------|------|
| `DAT_007d4110` | IDirect3DDevice9* |
| `DAT_006181d0` | Pixel-shader handle cache |

---

## Notes

- Previously had C1 plate at `re/analysis/bucket_004c4270/0x004cb0b0.md` (batch-x-s3).
- No new UNCERTAINTIEs raised.
