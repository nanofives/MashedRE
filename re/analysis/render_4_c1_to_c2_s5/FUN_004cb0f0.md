# FUN_004cb0f0 — D3D9 SetPixelShaderConstant with change-check

**RVA:** 0x004cb0f0  
**Body:** 0x004cb0f0 – 0x004cb122  
**Status promoted:** C1 → C2 (batch-render-4-s5)  
**Confidence rubric:** C2 — decompilation read; callee/caller graph confirmed; semantics documented.

---

## Signature

```c
void FUN_004cb0f0(int param_1);
```

param_1 = pixel shader constant state / index.

---

## Callers

- `FUN_0049a750` @ 0x0049a750
- `FUN_004db770` @ 0x004db770

## Callees

None (call through IDirect3DDevice9 vtable).

---

## Mechanics

Change-checked `SetPixelShaderConstantF` (or similar) wrapper:

```c
if (DAT_006181d4 != param_1) {
    iVar1 = IDirect3DDevice9_vtable[0x1ac/4](DAT_007d4110, param_1);  // SetIndices/SetPSConst
    DAT_006181d4 = -1;
    if (iVar1 >= 0) DAT_006181d4 = param_1;
}
```

`IDirect3DDevice9` vtable offset 0x1AC (index 107) corresponds to `SetIndices` in standard D3D9 vtable layout.

### [UNCERTAIN U-5001]
Vtable+0x1AC: D3D9 standard layout maps this to `SetIndices`. However the context (called alongside SetPixelShader/SetClipPlane, cache at `DAT_006181d4`) suggests a different operation. Evidence needed: call-site register args in the material-setup caller `FUN_0049a750`.

### Key globals

| Address | Role |
|---------|------|
| `DAT_007d4110` | IDirect3DDevice9* |
| `DAT_006181d4` | PS-const / indices cache |

---

## Notes

- Previously had C1 plate at `re/analysis/bucket_004c4270/0x004cb0f0.md` (batch-x-s3).
- U-5001 raised for vtable+0x1AC identity.
