# FUN_004c0ed0 — C1→C2 plate (batch-render-3-s6)

**RVA:** 0x004c0ed0  
**Name:** FUN_004c0ed0  
**Subsystem:** render  
**Promoted:** 2026-05-26 by batch-render-3-s6  

## Decompilation (0x004c0ed0)

```c
int FUN_004c0ed0(int param_1)
{
  if ((*(byte *)(*(int *)(param_1 + 0xa0) + 3) & 1) != 0) {
    FUN_004d8350(*(int *)(param_1 + 0xa0));  // update dirty frame
  }
  return param_1 + 0x50;
}
```

Body: 0x004c0ed0 – 0x004c0eee (30 bytes).

## Mechanical description

- One parameter: `param_1` (pointer to a RW object, likely `RwObject*` or `RwCamera*`).
- Tests bit 0 of flag byte at `*(param_1+0xa0)+3`.
- If bit 0 is set (dirty): calls `FUN_004d8350` (0x004d8350) with the sub-object pointer to update/sync the frame matrix.
- Returns `param_1 + 0x50` — the LTM (Local-To-Model/World matrix) start within the object layout, at raw offset 0x50 (80 decimal).
- 38 callers across the codebase — heavily used accessor.

## Callers / callees

- Callers: 38 (includes FUN_00405ab0, FUN_00406160, FUN_0041f1e0, RpAtomicGetWorldBoundingSphere, FUN_00533ec0, etc.)
- Callees: `FUN_004d8350` (0x004d8350, frame matrix sync)

## Evidence for C2

- Full body decompiles cleanly; lazy-update + accessor pattern is unambiguous.
- Corroborates C1 note: "camera matrix accessor; returns RwCamera+0x50 (LTM start); lazy-updates dirty camera via FUN_004d8350 if flag at *(cam+0xa0)+3 bit0."
- Key offsets: sub-object at +0xa0, dirty bit at sub+3 bit0, LTM at param_1+0x50 (0x50).
- Pattern matches `RwFrameGetLTM` from RenderWare 3.x SDK.

## U-ids

None required.
