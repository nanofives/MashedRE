---
rva: 0x004db2e0
name_in_ghidra: FUN_004db2e0
size_bytes: 251
confidence: C2
subsystem: render
session: batch-render-6-s6
date: 2026-05-26
pool_slot: Mashed_pool14
prior_plate: re/analysis/bucket_004d7ac0/0x004db2e0.md
---

## Shape

- Signature: `void* FUN_004db2e0(void* param_1, int param_2, int param_3)`
- Return: destination image pointer on success, `0` on failure
- Subsystem: rw-image-resample

## Mechanical description

Outer-shell of the image resize call.

1. Allocates destination image at requested dimensions/bpp via FUN_004cdca0(`param_2`, `param_3`, `0x20`) — `0x20` = 32 bpp.
2. Performs internal-state init via FUN_004cdd60. On failure: cleans up (FUN_004cdd00) and returns 0.
3. If source bpp is already 32 (`*(int*)(param_1+0xc) == 0x20`): calls FUN_004dabd0(dst, src) directly.
4. Else: allocates an intermediate 32-bpp image at source dimensions (`param_1+4`, `param_1+8`, `0x20`), initializes via FUN_004cdd60, copies-converts source into it via FUN_004cecb0, resizes via FUN_004dabd0(dst, intermediate), then frees intermediate via FUN_004cde20 + FUN_004cdd00.
5. Returns dst on success, 0 on any failure (with cleanup).

## Constants

| Address | Value | Note |
|---------|-------|------|
| 0x004db2e9 | `0x20` | 32 bpp |
| 0x004db320 | `+0xc` | bpp field in source descriptor |

## Callees (depth-1)

- `FUN_004cdca0` — image allocator
- `FUN_004cdd60` — image pixel-buffer init
- `FUN_004cdd00` — image free
- `FUN_004dabd0` — bilinear resampler (this batch)
- `FUN_004cecb0` — image format-convert
- `FUN_004cde20` — image pixel-buffer destroy

## Callers noted

None observed.

## Uncertainties

- [UNCERTAIN] FUN_004cdca0 is image-allocator; FUN_004cdd60 sets up pixel buffer; FUN_004cdd00 destroys image; FUN_004cecb0 is converter — shapes match RtImageNew/RtImageCopy pattern but not verified against exported names. U-5425.

## Stubs encountered

- [STUB] FUN_004cdca0 (image alloc).
- [STUB] FUN_004cdd00 (image free).
- [STUB] FUN_004cdd60 (image pixel-buffer init).
- [STUB] FUN_004cde20 (image pixel-buffer destroy).
- [STUB] FUN_004cecb0 (image format-convert).
