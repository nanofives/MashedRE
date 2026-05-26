---
rva: 0x004dc1f0
name_in_ghidra: FUN_004dc1f0
size_bytes: 128
confidence: C2
subsystem: render
session: batch-render-6-s6
date: 2026-05-26
pool_slot: Mashed_pool14
prior_plate: re/analysis/bucket_004d7ac0/0x004dc1f0.md
---

## Shape

- Signature: `float* FUN_004dc1f0(float* param_1, const float* param_2, int param_3, const float* param_4)`
- Return: `param_1` (output buffer pointer)
- Subsystem: rw-d3d9-matrix-batch

## Mechanical description

Vec3 array × 3x3 matrix transform (no translation row). Same shape as FUN_004dc160 but with translation values (m12, m13, m14) excluded — only the upper-left 3x3 of the 4x4 is used.

Per input vertex (`param_3` count): reads 3 floats from `param_2`, writes 3 transformed floats into packed output. Output stride 0xc per vertex.

## Constants

| Address | Value | Note |
|---------|-------|------|
| 0x004dc200 | `0xf` | alignment mask |
| 0x004dc24a | `0xc` | output stride (3 floats packed) |

## Callees (depth-1)

None (leaf function).

## Callers noted

None observed.

## Uncertainties

None.

## Stubs encountered

None.
