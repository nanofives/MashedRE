# FUN_004cb2f0 — D3D9 DrawPrimitive wrapper (with FUN_004d53b0 pre-call)

**RVA:** 0x004cb2f0  
**Body:** 0x004cb2f0 – 0x004cb321  
**Status promoted:** C1 → C2 (batch-render-4-s5)  
**Confidence rubric:** C2 — decompilation read; callee/caller graph confirmed; semantics documented.

---

## Signature

```c
void FUN_004cb2f0(undefined4 param_1, undefined4 param_2, undefined4 param_3,
                  undefined4 param_4, undefined4 param_5, undefined4 param_6);
```

6 arguments — matches `IDirect3DDevice9::DrawIndexedPrimitive(primitiveType, baseVertexIndex, minVertexIndex, numVertices, startIndex, primCount)`.

---

## Callers

- `FUN_0049a750` @ 0x0049a750

## Callees

- `FUN_004d53b0` @ 0x004d53b0 — pre-draw flush / state commit

---

## Mechanics

Pre-draw setup then raw D3D9 draw call:

```c
FUN_004d53b0();   // flush pending state / commit render state
IDirect3DDevice9_vtable[0x148/4](DAT_007d4110, param_1, param_2, param_3, param_4, param_5, param_6);
```

`IDirect3DDevice9` vtable offset 0x148 (index 82) = `DrawIndexedPrimitive`.  
Pass-through of all 6 arguments unchanged.

### Key globals

| Address | Role |
|---------|------|
| `DAT_007d4110` | IDirect3DDevice9* |

---

## Notes

- Previously had C1 plate at `re/analysis/bucket_004c4270/0x004cb2f0.md` (batch-x-s3).
- No new UNCERTAINTIEs raised.
