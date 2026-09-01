# area-render r8 — FUN_0054fd60 texture/raster callee neighbourhood classification

Date: 2026-09-01. Scope: classify the 12 C2 direct callees of FUN_0054fd60 (the RW
texture-stream reader plated to C2 in r7) as **child-synthetic-safe leaf** (my lane) vs
**allocator / live-state / dispatcher** (parent's booted lane). Excluded per parent:
0x004c5a00 RwTextureCreate (DEMOTED allocator, parent's). Already C3 (not re-examined):
0x004cbd30 RwStreamRead, 0x004c5bc0.

## Verdict: CLEAN NEGATIVE — no child-synthetic C3 in this neighbourhood.

The key distinction from the r3–r6 string wins: those dispatched through the RW **string**
vtable (`DAT_007d3ff8` slots +0xc4..+0x104 = strncpy/strlen/stricmp/strupr/…), which are
**pure CPU functions** registered at boot and live at menu-attach, so a synthetic path1 on a
fabricated buffer is bit-identical. Every promising leaf here instead dispatches through the
RW **device/raster** vtable (different slots: +0x64/+0x6c/+0x84/+0x88/+0xb8 = raster
lock/unlock/copy/destroy), which are **D3D9-backed device operations**. On a fabricated raster
with no real D3D9 texture behind it they fail (return 0 → degenerate green, exactly the
[[scratch-field-false-green]] trap) or fault — so they are not synthetic-safe. They ARE
promotable, but only via the parent's booted canonical-observation lane during a real texture
load.

## The 5 Ghidra-leaves (callees_depth1 empty) — ALL device-vtable wrappers, parent's lane

| RVA | size | shape | dispatch | why not child-synthetic |
|---|---|---|---|---|
| 0x004d5340 | 107 | `f(raster, mode, *w,*h,*d,*fmt)` locks raster, reads back w/h/d + byte-swapped stride into out-params | vtable **+0x6c** (raster lock) | needs a live locked raster; fake buf → lock returns 0 → out-params untouched (degenerate) |
| 0x004c76f0 | 54 | `int f(raster)` returns 1 if flag +0x23 high-bit clear, else calls device | vtable **+0xb8** | discriminating path is the device call; the no-call path returns constant 1 (degenerate) |
| 0x004c7860 | 54 | `uint f(raster, level, flags)` lock mip level, returns level or 0 | vtable **+0x84** (mip lock) | always calls device lock; fake raster → 0 (degenerate) |
| 0x004d5310 | 48 | `int f(raster, image)` device copy, sets raster flag +0x22 bit0 | vtable **+0x64** (copy/destroy) | device copy on a fake image → 0 (degenerate) |
| 0x004c7600 | 28 | `f(raster)` unlock (void-ish, returns raster) | vtable **+0x88** (unlock) | pure side-effect on the device; no scalar observable |

## The 7 non-leaves — allocators / stream-readers / dispatchers, parent's lane

| RVA | callees | role (from FUN_0054fd60 plate) |
|---|---|---|
| 0x004c77c0 | 1 | **RasterCreate** — allocator (EXCLUDED-adjacent; live D3D9 raster alloc) |
| 0x004cc5e0 | 4 | sub-chunk header stream read |
| 0x004cee90 | 5 | level-image stream read (allocates image) |
| 0x004cefd0 | 2 | gamma/flag fixup on the read image |
| 0x004cdd00 | 1 | image destroy (frees) |
| 0x004c7650 | 1 | raster pre-resize helper |
| 0x004db2e0 | 6 | per-level image→raster mip convert (allocates converted image) |

## Handoff

All 12 → parent's booted canonical-observation lane (device-vtable + allocators). No row
authored. Depth-2 not walked: the non-leaves recurse into RW stream/raster/image internals
(more device + allocator code), so the yield stays parent-lane; stopped rather than force it.
Recommend the parent, if promoting these, do it as a cluster under one real texture-load
capture (all 12 fire in FUN_0054fd60's own execution) rather than one scenario per row.
