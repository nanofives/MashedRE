---
rva: 0x004daff0
name_in_ghidra: FUN_004daff0
size_bytes: 740
confidence: C2
subsystem: render
session: batch-render-6-s6
date: 2026-05-26
pool_slot: Mashed_pool14
prior_plate: re/analysis/bucket_004d7ac0/0x004daff0.md
---

## Shape

- Signature: `void FUN_004daff0(void* param_1_descriptor, uint32_t param_2, uint32_t param_3, uint32_t param_4, float* param_5)`
- Return: void (accumulates into `param_5[0..3]`)
- Subsystem: rw-image-resample

## Mechanical description

Inner loop for the 32-bit RGBA bilinear-area resampler: integrates source pixels spanning the column range `[param_2 >> 16, param_3 >> 16]` (Q16.16) at source row index `param_4 >> 16`, into the 4-float accumulator at `param_5[0..3]`.

Computes `fVar1 = (param_3 - param_2) * _DAT_005d8d88` (1/footprint as float — used to normalize at end).

Source pixel pointer: `pbVar7 = src_pixels + row_idx * src_stride + ((param_2 >> 16) << 2)`.

Case A — single-column footprint (`param_2 >> 16 == param_3 >> 16`): reads single pixel, multiplies each channel by fVar1.

Case B — multi-column: pre-multiplies first sub-pixel by `(integer_boundary - param_2) / 65536`, accumulates full intermediate columns at unit weight, then adds the last partial column by `(param_3 - last_boundary)/65536`.

Final: divides each channel by `fVar1` (normalize back).

## Constants

| Address | Value | Note |
|---------|-------|------|
| 0x004daff7 | `_DAT_005d8d88` | `1.0 / 65536.0` (Q16.16 → float) |
| 0x004db014 | `+0x10` | source stride |
| 0x004db017 | `+0x14` | source pixel base |
| 0x004db062 | `_DAT_005ceb90` | per-byte normalizer |
| 0x004db28a | `0x10000` | Q16.16 unit (boundary step) |

## Callees (depth-1)

None (leaf function — no callees).

## Callers noted

- `FUN_004dabd0` (0x004dabd0)

## Uncertainties

None.

## Stubs encountered

None.
