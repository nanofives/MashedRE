# 0x004c4270 / 0x004c42d0 — x87 matrix orthonormality residual leaves

Subsystem: render
Date: 2026-07-27
Source: Ghidra `Mashed_pool0`, `read_only=true`, session asserted via `program_list_open` path match.
Anchor: `original/MASHED.exe.unpatched` SHA-256 `BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`.

## Label correction (supersedes two prior documents)

`re/analysis/plans/frontier_shape_refinement_2026-07-24.md` lines 27–29 label
`0x004c4270` / `0x004c42d0` / `0x004c4360` as **"RwV3d bbox Y / X / Z accessors"**.
That label is **wrong** and is retracted here.

`re/PROMOTION_QUEUE.md:285` (batch `frida-sweep-20260608-ah`, 2026-06-08 — 48 days
*earlier* than the plan) had already recorded the byte-level disproof for `0x004c4270`:
a `vec3_ptr` MISFIT reading offsets beyond `0x08`. The 2026-07-24 plan reasserts the
disproven label without citing or reconciling that finding. The plan's Cluster-A framing
("needs only the ST0 handler + a manual x87 reimpl") is therefore also wrong: no existing
`arg_type` seeds offsets `0x10`–`0x28`.

Both functions take **one pointer arg at `[ESP+4]`** and return a scalar in **ST0**.
Neither is a single-field accessor.

The prior per-RVA plates (`re/analysis/bucket_004c4270/0x004c4270.md`,
`re/analysis/render_4_c1_to_c2_s1/FUN_004c4270.md`) record U-4901 "decompiler-blank" —
the Ghidra lift is false-empty for `0x004c4270`, which is why the shape was hypothesised
rather than read. This note is derived from raw bytes only (`memory_read`).

## Operand layout

Both read nine dwords from the pointer arg at
`{0x00, 0x04, 0x08, 0x10, 0x14, 0x18, 0x20, 0x24, 0x28}` — three 3-float rows at stride
`0x10`. The three dwords at `0x0c`, `0x1c`, `0x2c` are **never read**. This is the RwMatrix
right/up/at layout with its pad words.

Note: `PROMOTION_QUEUE.md:285` listed the offset set as
`{0x0,0x4,0x10,0x14,0x18,0x20,0x24,0x28}` — it omits `0x08`. The full set is the nine
above (`0x08` is read by `FMUL dword ptr [EAX+0x8]`, encoding `d84808`).

## 0x004c4270 — off-diagonal (orthogonality) residual

Bytes at `0x004c4270` (96 read, function ends `c3` then `90` padding):

```
8b442404      MOV   EAX,[ESP+4]
d94024 d84814 d94020 d84810 dec1 d94028 d84818 dec1
d94024 d84804 d94020 d808   dec1 d94028 d84808 dec1
d94014 d84804 d94010 d808   dec1 d94018 d84808 dec1
d9c1 d8ca d9c3 d8cc dec1 d9c1 d8ca dec1
dddb ddd8 ddd8 c3
```

Literal semantics:

```
A = ((m[0x24]*m[0x14]) + (m[0x20]*m[0x10])) + (m[0x28]*m[0x18])   ; dot(row1, row2)
B = ((m[0x24]*m[0x04]) + (m[0x20]*m[0x00])) + (m[0x28]*m[0x08])   ; dot(row0, row2)
C = ((m[0x14]*m[0x04]) + (m[0x10]*m[0x00])) + (m[0x18]*m[0x08])   ; dot(row0, row1)
return ((B*B) + (A*A)) + (C*C)
```

Sum of squares of the three pairwise row dot products — zero exactly when the three rows
are mutually orthogonal.

The tail `dddb ddd8 ddd8` is `FSTP ST(3); FSTP ST(0); FSTP ST(0)` — the MSVC idiom that
writes the result into the deepest live slot and pops the two temporaries, leaving the
result in ST0 for `RET`. (`DD D8+i` is FSTP ST(i); `DD C0+i` would be FFREE.)

## 0x004c42d0 — diagonal (normality) residual

For each row `r` in `{0x00, 0x10, 0x20}`:

```
resid_r = ((m[r+4]*m[r+4] + m[r+0]*m[r+0]) + m[r+8]*m[r+8]) - *(float*)0x005cc320
return  = ((resid_1*resid_1) + (resid_0*resid_0)) + (resid_2*resid_2)
```

`*(float*)0x005cc320` = `0x3f800000` = **1.0f** (`memory_read` at `0x005cc320`, 4 bytes,
`data_hex 0000803f`, 2026-07-27).

So each term is `(|row|^2 - 1)`: zero exactly when the row is unit length.

Row 0 uses a different x87 register schedule in the original (`FMULP ST(3)` / `FADDP ST(3)`
/ `FXCH ST(2)` / `FSTP ST(2)`) than rows 1 and 2 (`FLD ST(1)` / `FADDP ST(1)` /
`FSTP ST(3)`). Both schedules are mirrored verbatim in the reimpl rather than normalised.

## Taken together

`0x004c4270` is the off-diagonal and `0x004c42d0` the diagonal half of an
`M * transpose(M) == I` orthonormality error metric.

[UNCERTAIN] What the caller does with the residual (tolerance compare, renormalise
trigger, assert) is not derivable from these leaves. Path to resolution: decompile the
shared caller `FUN_004c4530`.

[UNCERTAIN — RESOLVED 2026-07-27, see "Caller resolution" below] `0x004c4360` is **not** part
of this pair. Its bytes open `83ec18` (`SUB ESP,0x18`) with a stack frame and it reads
additional fields at `+0x30`, `+0x34`, `+0x38`, so it is a different shape. It is not ported
and its byte-level semantics remain undetermined; its *role* is now fixed by the caller.

## Caller resolution — `FUN_004c4530` is `RwMatrixOptimize` (2026-07-27)

Decompiling the shared caller closes **U-9021** and fixes the role of all three leaves.
`FUN_004c4530(int param_1, float *param_2)` (Ghidra, body `0x004c4530`–`0x004c45e2`):

- `param_2 == NULL` → defaults to `DAT_007d4028 + 0xc + DAT_007d3ff8` (an RW-globals default
  tolerance triple).
- calls `FUN_004c42d0(param_1)`, compares the `float10` result against `*param_2` → `bVar1`
- calls `FUN_004c4270(param_1)`, compares against `param_2[1]` → `bVar2`
- calls `FUN_004c4360(param_1)`, compares against `param_2[2]` (only when neither of the
  first two already failed) → together with `bVar1`/`bVar2` gives `bVar3`
- writes the flag word at `*(uint *)(param_1 + 0xc)`:
  `bVar1` clears/sets bit `0x1`; `bVar2` clears/sets bit `0x2`;
  `bVar3` sets `0x20000`, else clears it (`& 0xfffdffff`)
- **returns `param_1`** (the matrix itself)

Flag constants are already pinned on this binary by prior first-party work — no external
guessing: `0x20000` = `rwMATRIXINTERNALIDENTITY` and low bits `0x3` =
`rwMATRIXTYPEORTHONORMAL` (= NORMAL | ORTHOGONAL), per
`re/analysis/bucket_004c4270/0x004c4670.md:47-48` and
`mashedmod/src/mashed_re/Math/RwMatrixRotateInner.cpp:47`.

Signature, tolerance triple, flag writes at `+0xc`, and return-the-matrix together match
RenderWare's `RwMatrixOptimize(RwMatrix*, const RwMatrixTolerance*)`, whose tolerance struct
is `{normal, orthogonal, identity}` in that order. Resulting role assignment:

| RVA | tolerance slot | flag bit | metric |
|---|---|---|---|
| `0x004c42d0` | `param_2[0]` | `0x1` (NORMAL) | normality / diagonal residual |
| `0x004c4270` | `param_2[1]` | `0x2` (ORTHOGONAL) | orthogonality / off-diagonal residual |
| `0x004c4360` | `param_2[2]` | `0x20000` (IDENTITY) | identity-deviation residual |

This **independently confirms** the retraction recorded above: the slot order assigns
`0x004c42d0` to normality and `0x004c4270` to orthogonality, exactly as the byte-level
derivation concluded, and is incompatible with the retracted "RwV3d bbox Y/X/Z accessor"
labels (an accessor has no tolerance argument and writes no flag word).

**U-9022** (`0x004c4360`) is resolved at the role level: it is the identity-deviation metric
feeding `rwMATRIXINTERNALIDENTITY`. [UNCERTAIN] Its *byte-level* formula is still
underived — the `SUB ESP,0x18` frame and the `+0x30/+0x34/+0x38` reads are not yet explained.
Path to resolution: full disasm of `0x004c4360` plus its own `arg_type` handler (its shape
differs from `st0_ret_mat3_ptr`, which is pointer-seeded with no stack frame).

## Call graph (Ghidra, 2026-07-27)

- `0x004c4270`: callees **0**; callers `FUN_004c4530`, `FUN_005c47e0`.
- `0x004c42d0`: callees **0**; callers `FUN_004c4530`, `FUN_005c47e0`.

Both are true leaves → callee leaf-exemption applies. `FUN_004c4530` is C2 in `hooks.csv`,
satisfying the "one caller at C2+" half of the C3 gate.

[UNCERTAIN] `FUN_005c47e0` falls inside the `0x5c0000`–`0x5c8000` range that memory
`feedback_library_skip_bands` marks as the MSVC CRT library band, yet it calls these
first-party render leaves. Either the band bound is imprecise or this is a mislabelled
region. Not resolved here; flagged so the band is not trusted blindly at `0x005c47e0`.
Path to resolution: decompile `FUN_005c47e0` and check for CRT signatures.

## Reimplementation and evidence

- Reimpl: `mashedmod/src/mashed_re/Math/MatrixOrthoResidual.cpp`
  (`MatrixOrthoResidual4c4270`, `MatrixNormResidual4c42d0`), verbatim inline `__asm`,
  registered via `RH_ScopedInstall`. Built into both targets (`build.bat` +
  `asi_sources.rsp`). Declared `double`, never `void` — a void-declared forward leaks the
  x87 stack (memory `x87_st0_float10_return_fnptr`).
- New `arg_type` **`st0_ret_mat3_ptr`** authored in `re/frida/diff_template.js`
  (pointer-seeded 0x30 scratch, nine f32 at the offsets above, pads zeroed, ST0 captured
  as a 64-bit double fingerprint). Registry entries `mat3_ortho_residual_4c4270` /
  `mat3_norm_residual_4c42d0`.
- Diffs: `log/diff_mat3_ortho_residual_4c4270.csv` — 10/10 bit-identical, 0 mismatches.
  `log/diff_mat3_norm_residual_4c42d0.csv` — 10/10 bit-identical, 0 mismatches.

Semantic cross-check from the fingerprints (not just bit-identity): the ortho residual is
exactly `0x0000000000000000` for every orthogonal input row-set (identity, uniform 0.5
scale, the `±0.1` skew-orthogonal pair, the 90° rotation, the 45° rotation) and non-zero
for the five non-orthogonal ones. The norm residual is `0` for the two unit-row inputs and
non-zero (`0x3e09791a79ab7365`) for the 0.7071 rotation, where `2*0.7071^2 = 0.99998 != 1`
— independently confirming the subtracted constant is `1.0f`.

## Confidence

**C3.** The A/B ran with the hook **BYPASSED** (synthetic `run_diff` path1), which is C3
evidence at best and never C4 (`re/CONFIDENCE.md`, memory
`feedback_no_overclaiming_c_levels`). C4 would require a canonical-scenario run with the
inline-JMP actually installed. Not claimed.
