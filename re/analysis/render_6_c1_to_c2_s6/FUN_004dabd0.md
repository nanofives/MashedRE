---
rva: 0x004dabd0
name_in_ghidra: FUN_004dabd0
size_bytes: 1044
confidence: C2
subsystem: render
session: batch-render-6-s6
date: 2026-05-26
pool_slot: Mashed_pool14
prior_plate: re/analysis/bucket_004d7ac0/0x004dabd0.md
---

## Shape

- Signature: `void* FUN_004dabd0(void* param_1, void* param_2)`
- Return: `param_1` (destination descriptor pointer)
- Subsystem: rw-image-resample

## Mechanical description

32-bit-RGBA image resampler from descriptor `param_2` to descriptor `param_1` using bilinear-area weighted resampling.

Reads source/dest dimensions from descriptor offsets `+0x4` (width), `+0x8` (height); strides `+0x10`; pixel ptr `+0x14`. Output stride `param_1[4]`, output ptr base `param_1[5]`.

Computes per-axis source-step-per-dst-pixel as Q16.16 fixed point:
- `iVar2 = ROUND(src_width/dst_width * _DAT_005cea64)` — `_DAT_005cea64 = 0x10000` (65536)
- `iVar3 = ROUND(src_height/dst_height * _DAT_005cea64)`

For each dst row and column: integrates the source samples spanning the dst pixel's footprint via FUN_004daff0 (returns area-weighted average RGBA in `local_20`).

Scales by 1/footprint, clamps via `_DAT_005cd04c` and offset `_DAT_005cc32c` (0.5 — rounding bias), writes 4 bytes (RGBA) to output pixel.

Returns `param_1`.

## Constants

| Address | Value | Note |
|---------|-------|------|
| 0x004dabd5 | `+0x4`, `+0x8` | width/height offsets |
| 0x004dabd9 | `+0x10` | stride |
| 0x004dabdd | `+0x14` | pixel ptr |
| 0x004dabe6 | `_DAT_005cea64` | Q16.16 `1.0` (= 65536) |
| 0x004dad88 | `_DAT_005cd04c` | output scaling (float 255.0 — 8-bit channel max) |
| 0x004dad8e | `_DAT_005cc32c` | rounding bias (0.5) |
| 0x004daccd | `_DAT_005d8d88` | per-step inverse |
| 0x004dac28 | `_DAT_005cc320` | constant 1.0 |

## Callees (depth-1)

- `FUN_004daff0` — inner column-integrator for bilinear area sample

## Callers noted

- `FUN_004db2e0` (0x004db2e0)

## Uncertainties

None.

## Stubs encountered

None.
