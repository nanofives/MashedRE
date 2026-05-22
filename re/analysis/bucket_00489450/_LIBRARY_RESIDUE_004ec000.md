---
halt_marker: LIBRARY_RESIDUE
band: D3DX9_PSGP_MATH
band_span: 0x004ec000..0x004ee400  (approx; entire 0x004exxxxx low-half)
dispatcher_init: FUN_004fbe7a  @ 0x004fbe7a
dispatcher_table: PTR_FUN_006187a8 .. PTR_FUN_006188c8  (0x47 = 71 pointers, copied from PTR_FUN_006188c8 .. PTR_FUN_0061899c)
plate_prefix_date: 2026-05-20
opened_in_slot: Mashed_pool11
session: batch_aa_s1
---

## Identification

The address range 0x004ec000..0x004ee400 is **Microsoft D3DX9 inlined math library** statically linked into `MASHED.exe`. The cluster has two distinct sub-patterns, both of them library residue:

### Pattern A: PSGP dispatch trampolines

Tiny (≤0x18-byte) functions of the canonical shape:
```
push 1
call FUN_004fbe7a          ; PSGP CPU-feature init (idempotent)
add  esp, 4
jmp  [PTR_FUN_00618xxx]    ; tail-call through dispatch table
```
Each branches through a function pointer in the dispatch table at `PTR_FUN_006187a8 .. PTR_FUN_006188c8`.

`FUN_004fbe7a` (verified at 0x004fbe7a) is the **D3DX PSGP (Processor-Specific Graphics Pipeline) initializer**:
- Reads global flag `DAT_00619ab4` (0xFFFF = "not yet initialized")
- Copies the default function-pointer table from `PTR_FUN_006188c8` into `PTR_FUN_006187a8` (0x47=71 pointers via a `for (i=0x47; i; --i)` loop)
- Calls `FUN_004fcb51` to scrub/init the table
- Reads registry keys `"DisablePSGP"` and `"DisableD3DXPSGP"` via `FUN_004fbd78`
- Calls `IsProcessorFeaturePresent(7)` (SSE2) and `IsProcessorFeaturePresent(6)` (SSE)
- Reads CPU feature mask via `FUN_004fbdd5` (checks 3DNow! at bit 0x08)
- Dispatches to one of three CPU-specific table populators:
  - `FUN_004fc467` → SSE2 path  (DAT_00619ab4 = 1)
  - `FUN_004fc21f` → 3DNow! path (DAT_00619ab4 = 2)
  - `FUN_004fc0b1` → SSE path    (DAT_00619ab4 = 3)

This is the canonical Microsoft D3DX9 PSGP init flow (see `d3dx9_*.lib`/`d3dx9math.h` from the DirectX 9 SDK).

### Pattern B: Scalar-fallback inlined math

Slightly larger (~0x60-0xC0-byte) functions that perform PURE math, with no dispatch indirection. These are the "scalar branch" that the optimizer kept as a fixed reference body the PSGP table can point at. Identifiable by their D3DX9 mathematical shape and reuse of the static-data constant pool at `_DAT_005cc31c..005cc574` (shared with other D3DX-inlined math in this binary).

## Evidence per affected RVA (37 total)

| RVA | Bytes | Pattern | Identification |
|---|---|---|---|
| 0x004ecd40 | 0x2e | A | tail-jumps `PTR_FUN_00618874`; 6 args, likely `D3DXMatrix(Decompose|...)` |
| 0x004ecd6f | 0x5c | B | `D3DXVec3BaryCentric` (vec3 barycentric: `v + f*(v1-v) + g*(v2-v)`) |
| 0x004ece64 | 0x10 | A | tail-jumps `_DAT_006188a0`; 0 args, likely a scalar accessor |
| 0x004ecf1c | 0x0c | A | tail-jumps `PTR_FUN_006187d0` |
| 0x004ecf29 | 0x05 | A | thunk → FUN_004ecf1c |
| 0x004ecf2f | 0x10 | A | tail-jumps `_DAT_006188a4` |
| 0x004ecf40 | 0x05 | A | thunk → FUN_004ecf2f |
| 0x004ecf46 | 0x0c | A | tail-jumps `_DAT_006187c0` |
| 0x004ecf53 | 0x05 | A | thunk → FUN_004ecf46 |
| 0x004ecfbf | 0x10 | A | tail-jumps `_DAT_006188a8` |
| 0x004ed04e | 0x10 | A | tail-jumps `PTR_FUN_006187e0` |
| 0x004ed1c6 | 0xd0 | B | `D3DXVec4CatmullRom` (cubic CR using _DAT_005cc31c, _DAT_005cc320) |
| 0x004ed297 | 0x29 | A | tail-jumps `PTR_FUN_0061887c`; 6 args |
| 0x004ed2c1 | 0xf2 | B | `D3DXVec4Hermite` (cubic Hermite using _DAT_005cc31c, _DAT_005cc358, _DAT_005cc35c, _DAT_005cc574) |
| 0x004ed3b4 | 0x2e | A | tail-jumps `PTR_FUN_00618880`; 6 args |
| 0x004ed3e3 | 0x76 | B | `D3DXVec4BaryCentric` (vec4 barycentric — same shape as 0x004ecd6f but 4-wide) |
| 0x004ed506 | 0x10 | A | tail-jumps `_DAT_006188ac` |
| 0x004ed6ad | 0x0c | A | tail-jumps `_DAT_006187b4` |
| 0x004ed6ba | 0x05 | A | thunk → FUN_004ed6ad |
| 0x004ed8f6 | 0x0c | A | tail-jumps `_DAT_0061883c` |
| 0x004ed903 | 0x05 | A | thunk → FUN_004ed8f6 |
| 0x004ed976 | 0x0c | A | tail-jumps `_DAT_006187b8` |
| 0x004ed983 | 0x05 | A | thunk → FUN_004ed976 |
| 0x004edbbf | 0x0c | A | tail-jumps `_DAT_006187f0` |
| 0x004edbcc | 0x05 | A | thunk → FUN_004edbbf |
| 0x004edf71 | 0x2d | A | tail-jumps `PTR_FUN_0061882c`; 4 args, likely `D3DXMatrixMultiply` |
| 0x004edffb | 0x2d | A | tail-jumps `PTR_FUN_00618810`; 4 args |
| 0x004ee085 | 0x1b | A | tail-jumps `PTR_FUN_00618820`; 2 args |
| 0x004ee0a1 | 0x79 | B | **`D3DXMatrixRotationX`** (writes 4x4 with [1,0,0; 0,cos,sin; 0,-sin,cos]) |
| 0x004ee11b | 0x1b | A | tail-jumps `PTR_FUN_00618824`; 2 args |
| 0x004ee137 | 0x7a | B | **`D3DXMatrixRotationY`** (writes 4x4 with [cos,0,-sin; 0,1,0; sin,0,cos]) |
| 0x004ee1b2 | 0x1b | A | tail-jumps `PTR_FUN_00618828`; 2 args |
| 0x004ee1ce | 0x7c | B | **`D3DXMatrixRotationZ`** (writes 4x4 with [cos,sin,0; -sin,cos,0; 0,0,1]) |
| 0x004ee24a | 0x1f | A | tail-jumps `PTR_FUN_00618850`; 3 args |
| 0x004ee26a | 0x102 | B | **`D3DXMatrixRotationAxis`** (Rodrigues' formula — calls `fcos`/`fsin`, normalizes axis via `thunk_FUN_004ecb5a`, builds 4x4 rotation) |
| 0x004ee36c | 0x0c | A | tail-jumps `_DAT_00618840` |
| 0x004ee379 | 0x05 | A | thunk → FUN_004ee36c |

## Why this is NOT game code

1. **Mechanical**: every Pattern-A function calls `FUN_004fbe7a(1)` as a one-shot init, then tail-jumps through an entry in the contiguous pointer table at `PTR_FUN_006187a8`. This is the literal Microsoft D3DX9 PSGP dispatch idiom — there is no equivalent pattern in real game code.

2. **Algorithmic**: the Pattern-B functions are **byte-for-byte the D3DX9 inlined math** shipped in `d3dx9*.lib`. The rotation-matrix layouts (`D3DXMatrixRotationX/Y/Z`), barycentric/Catmull-Rom/Hermite splines, and Rodrigues axis-rotation are all 1:1 with the public D3DX9 documentation and headers.

3. **Constant pool**: the static-data constants `_DAT_005cc31c, _DAT_005cc320, _DAT_005cc32c, _DAT_005cc358, _DAT_005cc35c, _DAT_005cc574` are the standard D3DX9 cubic-spline coefficient pool (2.0f, 3.0f, 0.5f, 1.0f, …). The fact that real game code at 0x00489500 ALSO uses these constants is because the game inlined a Catmull-Rom evaluator with the same coefficients — not because 0x00489500 is library code.

4. **No game-state references**: none of the 37 functions touch any of the game's globals (no `_DAT_007xxxxx`, no `DAT_00703110`-style runtime singletons, no engine vtables).

## What we DO NOT do

Per the STOP-AND-ASK clause in the batch-aa-s1 prompt, **do NOT plate these 37 RVAs as C1 game code**. They are stock Microsoft D3DX9 math statically linked into the binary. If/when the project ever needs a behavioral reference for these functions, the canonical implementation is the public D3DX9 SDK documentation, not reverse engineering. They should be marked as `library-residue/d3dx9` in any subsequent triage.

## Affected RVAs from the batch_aa_s1 list (skipped — no plate written)

```
0x004ecd40, 0x004ecd6f, 0x004ece64, 0x004ecf1c, 0x004ecf29, 0x004ecf2f,
0x004ecf40, 0x004ecf46, 0x004ecf53, 0x004ecfbf, 0x004ed04e, 0x004ed1c6,
0x004ed297, 0x004ed2c1, 0x004ed3b4, 0x004ed3e3, 0x004ed506, 0x004ed6ad,
0x004ed6ba, 0x004ed8f6, 0x004ed903, 0x004ed976, 0x004ed983, 0x004edbbf,
0x004edbcc, 0x004edf71, 0x004edffb, 0x004ee085, 0x004ee0a1, 0x004ee11b,
0x004ee137, 0x004ee1b2, 0x004ee1ce, 0x004ee24a, 0x004ee26a, 0x004ee36c,
0x004ee379
```

(37 RVAs)
