# FUN_004cafd0 — D3D9 SetTexture with change-check (stream slot)

**RVA:** 0x004cafd0  
**Body:** 0x004cafd0 – 0x004cb063  
**Status promoted:** C1 → C2 (batch-render-4-s5)  
**Confidence rubric:** C2 — decompilation read; callee/caller graph confirmed; semantics documented.

---

## Signature

```c
bool FUN_004cafd0(int param_1, int *param_2);
```

param_1 = texture stage / stream index (0-based)  
param_2 = pointer to a RW texture/raster object (may be NULL → unbind)  
Returns: true if call succeeded or texture was already bound.

---

## Callers

- `FUN_004d07b0` @ 0x004d07b0 — render frame gate / material setup.

## Callees

None (all calls through vtable pointers).

---

## Mechanics

Resolves a RW texture object to a D3D9 texture surface, then calls `SetTexture` with change-check:

1. If `param_2 != NULL`:
   - Read byte flag: `bVar1 = *(*(param_2) + DAT_00911ae4 + 9)`.
   - Get D3D surface: if `(bVar1 & 0xf) == 0` call vtable+0x48 with args `(obj, 0, &local_4)`; else call vtable+0x48 with `(obj, bVar1>>4, 0, &local_4)`.
2. Compare: if `DAT_007d4578[param_1] == local_4` → already bound, return true.
3. Otherwise: update cache `DAT_007d4578[param_1] = local_4`.
4. Call `IDirect3DDevice9::SetTexture(DAT_007d4110, param_1, local_4)` (vtable+0x94).
5. If `local_4 != NULL`: call `Release(local_4)` (vtable+0x08).
6. Return `iVar3 >= 0` (HRESULT success check).

### Change-check cache

`DAT_007d4578` is an array of up to 4 bound texture pointers (stages 0..3), also used by `FUN_004c9ad0` reset path.

### Key globals

| Address | Role |
|---------|------|
| `DAT_007d4110` | IDirect3DDevice9* |
| `DAT_007d4578` | Texture stage cache array |
| `DAT_00911ae4` | RW object vtable offset base |

---

## Notes

- Previously had C1 plate at `re/analysis/bucket_004c4270/0x004cafd0.md` (batch-x-s3).
- No new UNCERTAINTIEs raised.
