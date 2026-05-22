---
band_start: 0x004ee461
band_end:   0x004fc9e0
library:    D3DX9 (Microsoft D3DX9_xx.dll PSGP path, statically linked)
session:    batch-aa-s2
session_date: 2026-05-20
opened_in_slot: Mashed_pool6
rva_count_skipped: 56
---

# Library-residue determination — 0x004ee461 .. 0x004fc9e0

## Verdict

The whole 56-RVA assignment is **D3DX9 PSGP library residue** (Pentium-Specific
Geometry Pipeline runtime — the math/vector helper code that ships with
D3DX9_xx.dll and was statically linked into MASHED.exe at build time). No game
code calls these wrappers; the linker pulled the .lib in because some other
D3DX9 helper drew on it. C0->C1 plates are not authored — they would be
unrelated to Mashed gameplay.

This determination matches the brief's elevated library-residue warning for the
0x004ee/0x004ef range and is consistent with the s1 low-half bucket if the
same call returned the same conclusion.

## Evidence anchors (5 independent signals)

### 1. PSGP dispatcher — `FUN_004fbe7a` (0x004fbe7a)

The defining D3DX9 PSGP fingerprint. Decompilation literally shows:

- Reads registry key `HKLM\Software\Microsoft\Direct3D` via `FUN_004fbd78`
  (0x004fbd78 calls `RegOpenKeyA` + `RegQueryValueExA`).
- Queries `"DisablePSGP"` and `"DisableD3DXPSGP"` value names (the literal
  D3DX9 opt-out env keys).
- Calls `IsProcessorFeaturePresent(7)` -> SSE2 path.
- Calls `IsProcessorFeaturePresent(6)` -> SSE path.
- Reads CPUID via `FUN_004fbdd5` (0x004fbdd5) and compares vs string
  `"GenuineIntel"` at `s_GenuineIntel_005daf70`.
- Copies 71 entries (`iVar3 = 0x47`) from a default table at
  `&PTR_FUN_006188c8` into a live dispatch table at `&PTR_FUN_006187a8`.
- Selects one of three installers depending on the result:
  - `FUN_004fc467(&PTR_FUN_006187a8)`  -> 3DNow! / 3DNow! Extended table
    (gated on bits 0x20/0x40 of the CPUID feature byte)
  - `FUN_004fc21f(&PTR_FUN_006187a8)`  -> SSE2 table
  - `FUN_004fc0b1(&PTR_FUN_006187a8)`  -> SSE table
- Caches result in `DAT_00619ab4` (sentinel = `0xffff` before init; final
  value 0/1/2/3 indicates which platform path is active).

The string `"DisableD3DXPSGP"` is published in the D3DX9 SDK documentation; the
registry-keyed scalar/SSE/SSE2/3DNow selector is unique to D3DX9 PSGP.

### 2. Three CPU-feature install tables

- `FUN_004fc0b1` (0x004fc0b1) installs 44 distinct function pointers into the
  71-slot dispatch table at `&PTR_FUN_006187a8`, all pointing into the
  0x00504..0x00508 range (the SSE implementations of D3DXVec/D3DXMatrix ops).
- `FUN_004fc21f` (0x004fc21f) installs 44 distinct function pointers, same
  slot layout but pointing to a *different* set of bodies in
  0x00504..0x00508 (the SSE2 implementations).
- `FUN_004fc467` (0x004fc467) installs ~62 function pointers into the
  0x0050..0x00510 range, gated on `bVar1 & 0x20` (3DNow!) and
  `bVar1 & 0x40` (3DNow! Extended). This is the AMD-specific PSGP path.

The three install tables sharing slot indices, gated by CPUID feature bits,
in a 71-entry layout is the textbook D3DX9 PSGP dispatch shape.

### 3. Thin dispatcher-then-indirect wrappers (0x004ee... half)

Most RVAs in the 0x004ee range are 5-10 byte wrappers of the form:

```
FUN_004ee461:  call FUN_004fbe7a(1); jmp [PTR_FUN_006188xx](args)
FUN_004ee48f:  call FUN_004fbe7a(1); jmp [PTR_FUN_006188xx]
FUN_004ee4a0:  call FUN_004fbe7a(1); jmp [PTR_FUN_006188xx](args)
FUN_004ee74a:  call FUN_004fbe7a(1); jmp [PTR_FUN_006188xx](args)
FUN_004ee7c6:  call FUN_004fbe7a(1); jmp [PTR_FUN_006188xx](args)
FUN_004ee8d3:  call FUN_004fbe7a(1); jmp [_DAT_006187f4]
FUN_004ee989:  call FUN_004fbe7a(1); jmp [_DAT_00618858]
FUN_004eea4d:  call FUN_004fbe7a(1); jmp [_DAT_00618884]
FUN_004eeadb:  call FUN_004fbe7a(1); jmp [PTR_FUN_00618888]
FUN_004eeb86:  call FUN_004fbe7a(1); jmp [PTR_FUN_00618834](args)
FUN_004eec93:  call FUN_004fbe7a(1); jmp [PTR_FUN_00618854](args)
FUN_004eed14:  call FUN_004fbe7a(1); jmp [_DAT_0061885c]
FUN_004eeddc:  call FUN_004fbe7a(1); jmp [PTR_FUN_006187fc]
FUN_004ef0df:  call FUN_004fbe7a(1); jmp [_DAT_006188b8]
FUN_004ef1af:  call FUN_004fbe7a(1); jmp [_DAT_006188bc]
```

This is the classic "exported D3DX9 API function" shape: trampoline that
ensures the dispatch table is initialized, then jumps through it. The
indirect targets `PTR_FUN_00618xxx` all live in the dispatch-table band
(0x006187a8 + N*4, N <= 0x47).

`thunk_FUN_004ee7c6` (0x004ee7f4), `thunk_FUN_004ee8d3` (0x004ee8e0),
`thunk_FUN_004ee989` (0x004ee996), `thunk_FUN_004eea4d` (0x004eea5a),
`thunk_FUN_004eeadb` (0x004eeae8), `thunk_FUN_004eeb86` (0x004eebaa),
`thunk_FUN_004eeddc` (0x004eede9) are all 5-byte Ghidra-detected `jmp rel32`
thunks pointing back to their respective non-thunk dispatcher wrappers —
the standard MSVC-emitted "import-equivalent" jump stubs that D3DX9 ships.

### 4. Verbatim D3DXMath bodies (0x004ef... range)

- `FUN_004ef25b` (0x004ef25b) - D3DXVec2TransformCoord (2-component vector
  by 4x4 matrix, row-major, hard-coded `0xc`/`0xd` translation offsets).
- `FUN_004ef2c6` (0x004ef2c6) - D3DXVec2TransformCoordArray (same body in
  a stride loop over `param_6`).
- `FUN_004ef349` (0x004ef349) - D3DXVec2TransformCoordW (divides by W if
  `FUN_004ec760(w, 1.0f) == 0`).
- `FUN_004ef3c6` (0x004ef3c6) - array variant of the W-divide flavor.
- `FUN_004ef45e` (0x004ef45e) - D3DXVec2TransformNormal (no translation).
- `FUN_004ef49a` (0x004ef49a) - array variant.
- `FUN_004ef4ee` (0x004ef4ee) - D3DXVec3Normalize (epsilon-clamp via
  `PTR_DAT_005ceabc`, divide by sqrt, scale).
- `FUN_004ef58b` (0x004ef58b) - D3DXVec3TransformCoord.
- `FUN_004ef647` (0x004ef647) - D3DXVec3TransformCoordArray.
- `FUN_004ef726` (0x004ef726) - D3DXMatrixAffineTransformation: 8-way switch
  on `((p8!=0)<<2)|((p7!=0)<<1)|(p6!=0)` selecting which of
  scaling/rotation/translation inputs are present, then applies them.
- `FUN_004ef8b3` (0x004ef8b3) - vector variant of the same switch.
- `FUN_004efab3` (0x004efab3) - matrix variant with viewport divide
  (`(p2[0]-fVar1)/fVar2*2 - 1.0`, NDC unprojection).
- `FUN_004efc59` (0x004efc59) - perspective projection setup with
  `_DAT_005cc574/aspect` etc.
- `FUN_004efe8a` (0x004efe8a) - D3DXVec4Normalize (4-component, same
  epsilon clamp as the Vec3 variant).
- `FUN_004eff40` (0x004eff40) - D3DXQuaternionRotationMatrix wrapper.
- `FUN_004eff79` (0x004eff79) - D3DXMatrixAffineTransformation2D variant
  (vector + quaternion + center + translation, 7 args).
- `FUN_004fb9aa` (0x004fb9aa) - reflection across plane defined by p2/p3
  (3D reflection matrix application).
- `FUN_004fc916` (0x004fc916) - fast 3D normalize with reciprocal-sqrt LUT
  (reads `&DAT_00619ab8` and `&DAT_00619abc` indexed by upper bits of the
  squared length — the D3DX9 PSGP scalar rsqrt approximation table).
- `FUN_004fc9e0` (0x004fc9e0) - 4x4 matrix multiply with alias-safe local
  buffer copy (`local_44[16]`).
- `FUN_004ee4c7` (0x004ee4c7) - quaternion-rotation-around-pivot composer
  (uses `thunk_FUN_004ee36c` for axis-angle quaternion build and
  `thunk_FUN_004ed6ad` for quaternion multiply).
- `FUN_004ee76a` (0x004ee76a) - axis-angle to quaternion builder
  (`q.w = cos(theta*kDegToRad/2)`, `q.xyz = axis * sin(...)` —
  `_DAT_005cc32c` is the deg-to-rad constant 0.01745329).
- `FUN_004eecbd` (0x004eecbd) - quaternion squad-style interpolation
  (calls `thunk_FUN_004eeb86` three times with weight
  `(1.0 - t) * t * 2.0` -> Bezier evaluation).
- `FUN_004eed54` (0x004eed54) - quaternion slerp with degenerate-case
  fallback (`if FUN_004ec760(w1+w2, 0)` -> copy through).

The hard-coded floating-point constants — `_DAT_005cc320` (1.0),
`_DAT_005cc32c` (pi/180), `_DAT_005cc574` (2.0), `_DAT_005cc94c`
(4294967296.0, the unsigned-int-to-float bias), `PTR_DAT_005ceabc`
(the normalize-epsilon threshold) — and the rsqrt LUT at `&DAT_00619ab8`
are the D3DX9 PSGP shared rodata.

### 5. Zero in-game callers on sampled wrappers

- `FUN_004ee4c7` `function_callers` -> 0 entries.
- `FUN_004ee76a` `function_callers` -> 0 entries.

If these were Mashed game code, callers would exist. They have none because
the .lib was linked but the symbols are not referenced by game code paths —
classic dead-strip-resistant static library residue.

## Non-PSGP cross-references found inside

- `FUN_004ec760` (0x004ec760) - generic float-near-zero test; called by most
  bodies above with `0x3f800000` (1.0) as the comparand.
- `thunk_FUN_004ed6ad`, `thunk_FUN_004ed8f6`, `thunk_FUN_004edbbf`,
  `thunk_FUN_004ee36c`, `thunk_FUN_004ecb5a`, `thunk_FUN_004ecf1c`,
  `thunk_FUN_004ecf2f` - all live in the 0x004ec/0x004ed range (s1's low
  half). These are the SAME library's quaternion-multiply / matrix-multiply
  / identity-build helpers; s1's bucket presumably contains them.

These cross-references confirm s1's half is the same library, not a coincidental
math cluster.

## Confirmation cross-check (string evidence)

The string `"GenuineIntel"` lives at `s_GenuineIntel_005daf70` and is read
literally by `FUN_004fbdd5` (the CPUID branch dispatcher). The strings
`"DisablePSGP"` and `"DisableD3DXPSGP"` are passed as the `lpValueName`
argument to `RegQueryValueExA` inside `FUN_004fbe7a` — these are the
D3DX9 SDK's documented opt-out registry values.

## RVA-by-RVA classification (all 56)

All 56 RVAs in this bucket are D3DX9 library residue. No plates authored.

```
0x004ee461  D3DX9 wrapper (-> PTR_FUN_00618848)
0x004ee48f  D3DX9 wrapper (-> PTR_FUN_00618844, jumptable)
0x004ee4a0  D3DX9 wrapper (-> PTR_FUN_0061884c)
0x004ee4c7  D3DXQuaternionRotationAxis composer (zero callers)
0x004ee74a  D3DX9 wrapper (-> PTR_FUN_0061881c)
0x004ee76a  D3DXQuaternionRotationAxis (axis-angle quaternion build)
0x004ee7c6  D3DX9 wrapper (-> PTR_FUN_0061880c)
0x004ee7f4  thunk -> FUN_004ee7c6
0x004ee8d3  D3DX9 wrapper (-> _DAT_006187f4)
0x004ee8e0  thunk -> FUN_004ee8d3
0x004ee989  D3DX9 wrapper (-> _DAT_00618858)
0x004ee996  thunk -> FUN_004ee989
0x004eea4d  D3DX9 wrapper (-> _DAT_00618884)
0x004eea5a  thunk -> FUN_004eea4d
0x004eeadb  D3DX9 wrapper (-> PTR_FUN_00618888)
0x004eeae8  thunk -> FUN_004eeadb
0x004eeb86  D3DX9 wrapper (-> PTR_FUN_00618834)
0x004eebaa  thunk -> FUN_004eeb86
0x004eec93  D3DX9 wrapper (-> PTR_FUN_00618854)
0x004eecbd  D3DXQuaternionSquad helper (Bezier-form interp)
0x004eed14  D3DX9 wrapper (-> _DAT_0061885c)
0x004eed54  D3DXQuaternionSlerp with degenerate-case fallback
0x004eeddc  D3DX9 wrapper (-> PTR_FUN_006187fc)
0x004eede9  thunk -> FUN_004eeddc
0x004ef0df  D3DX9 wrapper (-> _DAT_006188b8)
0x004ef1af  D3DX9 wrapper (-> _DAT_006188bc)
0x004ef25b  D3DXVec2TransformCoord
0x004ef2c6  D3DXVec2TransformCoordArray
0x004ef349  D3DXVec2TransformCoordW (homogeneous divide)
0x004ef3c6  D3DXVec2TransformCoordWArray
0x004ef45e  D3DXVec2TransformNormal
0x004ef49a  D3DXVec2TransformNormalArray
0x004ef4ee  D3DXVec3Normalize (epsilon clamp)
0x004ef58b  D3DXVec3TransformCoord
0x004ef647  D3DXVec3TransformCoordArray (homogeneous divide)
0x004ef726  D3DXMatrixAffineTransformation (8-way switch)
0x004ef8b3  D3DXMatrixAffineTransformation (vector body, 9 args)
0x004efab3  D3DXMatrixUnproject (NDC) variant
0x004efc59  D3DXMatrixPerspectiveOffCenterRH
0x004efe8a  D3DXVec4Normalize (epsilon clamp)
0x004eff40  D3DXQuaternionRotationMatrix wrapper
0x004eff79  D3DXMatrixAffineTransformation2D (7 args)
0x004fb9aa  Plane-reflection apply (3D)
0x004fbd78  RegQueryValueExA wrapper for HKLM\Software\Microsoft\Direct3D
0x004fbdd5  CPUID + "GenuineIntel" probe
0x004fbe0f  Catch@004fbe0f (try/catch handler for the probe)
0x004fbe1b  CPUID-feature decoder (continuation of probe)
0x004fbe7a  PSGP dispatcher (DisableD3DXPSGP / SSE / SSE2 / 3DNow)
0x004fbf76  PSGP helper (continuation of dispatcher; not separately probed,
            inferred from address adjacency)
0x004fc015  PSGP helper (continuation; not separately probed)
0x004fc0b1  PSGP SSE-path table installer (44 entries)
0x004fc21f  PSGP SSE2-path table installer (44 entries)
0x004fc383  PSGP CPUID-feature query (returns bit mask)
0x004fc467  PSGP 3DNow!-path table installer (62 entries)
0x004fc916  D3DXVec3Normalize (PSGP rsqrt-LUT variant)
0x004fc9e0  D3DXMatrixMultiply (alias-safe)
```

(Two RVAs in the 0x004fbf/0x004fc01 band were not separately function_at'd
because they fall inside the obvious PSGP infrastructure window between the
known dispatcher 0x004fbe7a and the known table installer 0x004fc0b1; they
are statistically certain to be more PSGP helpers but a deeper probe would
be needed to give them a specific name. Confidence on the band-level
verdict is unaffected.)

## Action

- Plates: not authored (per protocol library-residue STOP).
- Trackers: untouched (read-only session, per brief).
- Recommend: queue this whole bucket as a single SCRIBE_QUEUE entry with
  level=library_residue / subsystem=d3dx9_psgp / note=this file, so it
  drops from the real-candidate pool. The sibling 0x004ee low-half bucket
  (batch_aa s1) should produce the same conclusion; both halves merge into
  a single ~79-RVA D3DX9-PSGP drop.
