# Library relabel of the stand-in census's out-of-zone rows (2026-10-11)

**Ruling (Mariano, relayed by the plan-#3 orchestrator, 2026-10-11):** the 108 census rows labelled first-party inside
the RenderWare core, libpng and CRT-tail ranges are relabelled after a byte check by the method of
`re/tools/dshow_band_check.py`; only rows that make no call into first-party game code and touch no game-owned data
move; failures stay as they are. Separately, 0x004522d0 (12-byte RW-free thunk) is library glue: library-skip, left
deferred.

**Result: 83 relabelled** (79 `third-party-library[renderware]`, 4 `third-party-library[libpng]`); 25 stay as they are.
Per-row data: `re/analysis/library_relabel_2026-10-11.tsv` (the 93 checked rows). Tool: `re/tools/lib_band_check.py`.

## Which rows were checked

The 108 are the stand-in census rows (`re/coverage/standin_census_d1.tsv` / `_d2.tsv`: C2, reached, alloc/free within
one call level, no other import or file IO within two) at or above 0x0049c5d0 that the tracker labels first-party.

| range | rows | treated as |
|---|---|---|
| 0x004c0700..0x004dc300 (RW core: RwEngineInit 0x004c32b0, RwFreeListDestroy 0x004cc9f0, rwD3D8* rasters) | 49 | renderware |
| 0x004dc300..0x004ec000 and 0x0052a000..0x00558c50 (interleaved with existing `renderware` rows) | 11 | renderware |
| 0x004f0810..0x004f46a0, between the d3dx9 bands; plates read "RenderWare 3.x D3D9-backend" | 23 | renderware |
| 0x00516000..0x0052a000 libpng/zlib (png_create_struct_2 0x005207c0, png_free 0x00520930) | 4 | libpng |
| 0x005c0000..0x005c8000 CRT tail | 6 | checked, not relabelled (below) |
| 0x005a5820..0x005c0000 audio run (14) and 0x004b48e0 (1) | 15 | not checked: outside the three named ranges |

## Method

Same two tests as `dshow_band_check.py`, with the library side defined by range as that tool's `inr()` does:
code in `[0x004c0700, 0x00558c50)` and `[0x005c0000, 0x005ca700)`, rows labelled `third-party-library[...]`, and CRT
helpers named `_` / `FID_` are the library side; everything else is first-party.

1. **Calls first-party code**: a `call` / `jmp rel32` leaving the row's body to a first-party function.
2. **Touches game-owned data**: an absolute data address in the body that first-party code WRITES. The DirectShow
   test was "shared with first-party code and not a constant"; RenderWare differs in that its own mutable state is read
   by the game (RwGlobals 0x007d3ff8, the RW module globals at 0x007d4xxx), so ownership is decided by who writes the
   address. Constants pass, and so does state only library code writes.

The candidate set is iterated to a fixed point (a failing row leaves the library side). Before the range rule, 90 of 93
failed only because their callees and writers (e.g. 0x004c3040, the writer of 0x007d3ff8) are RW functions that the
tracker also labels `render`; the ruling corrects exactly those labels, so they are not evidence against it.

## Rows that stay as they are (25)

Failed the byte check (4):

| rva | name | reason |
|---|---|---|
| 0x004c32b0 | RwEngineInit | calls 0x0045b350 RwInitNullStub, a function in the game-code zone |
| 0x004f0810 | FUN_004f0810 | calls 0x0045b350 RwInitNullStub |
| 0x004c9ad0 | FUN_004c9ad0 | reads 0x006181c8 / 0x006181cc / 0x006181d8, written by game code 0x00499d90 / 0x0049a150 / 0x0049a2e0 |
| 0x004caea0 | FUN_004caea0 | same three addresses as 0x004c9ad0 |

CRT-tail range, passed the byte check but not relabelled (6): 0x005c47b0, 0x005c47e0, 0x005c4ad0, 0x005c4bb0,
0x005c4c60, 0x005c4d50. They are not CRT code: they call RW frame / matrix functions (0x004c0e50, 0x004c4270, ...)
and the RW allocator, and U-9023 (2026-07-27, `scripts/promote_frontier.py`) already records 0x005c47e0 as game code
and released the non-CRT rows from 0x005c4000 up to the frontier. Labelling them `msvc-crt-tail` would be wrong, and
nothing in the bytes names another library.

Outside the named ranges (15): the 0x005a5820..0x005c0000 audio run (0x005a71f0, 0x005a9f10, 0x005aa200, 0x005aa560,
0x005aba20, 0x005aca40, 0x005ad0b0, 0x005ad420, 0x005ae470, 0x005aea00, 0x005aea50, 0x005b0a90, 0x005b0b10,
0x005b3b80) and 0x004b48e0 (a raster + z-buffer + camera + frame constructor). Not part of the ruling's ranges and no
library identity in the bytes.

## Side effects

- `scripts/promote_frontier.py` already excludes any `third-party` subsystem tag (line 159), and libpng is inside its
  `LIBRARY_BANDS`; no exclusion change was needed.
- The same script states RenderWare internals are first-party port work under D2 Option B ("RW-subset verbatim",
  `_RW_NAME_PREFIXES`). Relabelling 79 RW rows to `third-party-library[renderware]` removes them from the frontier,
  consistent with the 464 rows that already carry that label but not with that comment.
