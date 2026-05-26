---
rva: 0x004da4f0
name_in_ghidra: FUN_004da4f0
size_bytes: 1288
confidence: C2
subsystem: render
session: batch-render-6-s6
date: 2026-05-26
pool_slot: Mashed_pool14
prior_plate: re/analysis/bucket_004d7ac0/0x004da4f0.md
---

## Shape

- Signature: `void FUN_004da4f0(void* param_1, int param_2, void* param_3, int param_4, void* param_5, void* param_6)`
- Return type: void (no return value used at callsite)
- Subsystem: rw-palette-quantizer

## Mechanical description

Per-pixel paletted-image emission: given an input bpp configuration in descriptor `param_6` plus quantizer state in `param_5`, writes palette indices into the output buffer `param_1` (stride `param_2`).

Descriptor `param_6` shape: width `+0x4`, height `+0x8`, bpp `+0xc`, src stride `+0x10`, pixel buffer `+0x14`, palette `+0x18`.

Four code paths, gated by `(param_3==4, param_4==1)` toggle and `bpp ∈ {4, 8, 0x20}`:
- 4-bpp output + 4/8-bpp input: packs two indices per output byte (low nibble first, then high nibble).
- 4-bpp output + 32-bpp input: same packing, pixels read directly.
- 8-bpp output + 4/8-bpp input: one byte per pixel.
- 8-bpp output + 32-bpp input: same.

Per pixel: builds the 16-way octree key from each channel's high bits via `(&DAT_007d6c98)[byte >> shift]`, then either returns directly from the quantizer root leaf (`DAT_00618430 == 0` → palette index byte at `*(int*)(param_5+0x4000)+0x18`) or descends via FUN_004daa00 (key `>> 4`, depth `DAT_00618430-1`) to fetch leaf palette index.

## Constants

| Address | Value | Note |
|---------|-------|------|
| 0x004da4ff | `+0x10` / `+0x14` | input stride / pixel ptr |
| 0x004da506 | `4`, `1` | 4bpp-output gate (param_3, param_4) |
| 0x004da50f | `4`, `8` | paletted-input bpp guard |
| 0x004da6f5 | `0x20` | 32bpp-input gate |
| 0x004da52e | `+0x18` | leaf palette-index field |
| 0x004da538 | `+0x4000` / `+0x4004` | quantizer state pointers |
| 0x004da548 | `&DAT_007d6c98` | bit-extract LUT |
| 0x004da4f5 | `DAT_00618430` | octree depth |

## Callees (depth-1)

- `FUN_004daa00` — octree descend-to-leaf, returns palette index byte

## Callers noted

None observed.

## Uncertainties

None.

## Stubs encountered

None.
