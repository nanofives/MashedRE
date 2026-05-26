# FUN_004cb2c0 — D3D9 SetMaterial with change-check

**RVA:** 0x004cb2c0  
**Body:** 0x004cb2c0 – 0x004cb2e3  
**Status promoted:** C1 → C2 (batch-render-4-s5)  
**Confidence rubric:** C2 — decompilation read; callee/caller graph confirmed; semantics documented.

---

## Signature

```c
void FUN_004cb2c0(int param_1);
```

param_1 = material pointer or state handle.

---

## Callers

- `FUN_0049a750` @ 0x0049a750

## Callees

None (call through IDirect3DDevice9 vtable).

---

## Mechanics

Change-checked `SetMaterial` wrapper:

```c
if (DAT_006181d8 != param_1) {
    DAT_006181d8 = param_1;
    IDirect3DDevice9_vtable[0x1a0/4](DAT_007d4110, param_1);  // SetMaterial
}
```

`IDirect3DDevice9::SetMaterial` is at vtable offset 0x1A0 (index 104). Single-arg call (just the material pointer — no change of return value needed). Cache value written unconditionally before the call (optimistic update).

### Key globals

| Address | Role |
|---------|------|
| `DAT_007d4110` | IDirect3DDevice9* |
| `DAT_006181d8` | Material handle cache |

---

## Notes

- Previously had C1 plate at `re/analysis/bucket_004c4270/0x004cb2c0.md` (batch-x-s3).
- No new UNCERTAINTIEs raised.
