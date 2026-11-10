# PREREG PORT-OT-RWG-DEVSLOTS: RwGlobals device slots +0x24 / +0x34 on MASHED_ORIG_TOP_RWMOD

Committed alone, before any arm. Branch `orch/PORT-OT-RWG-DEVSLOTS`, stacked on orch/PORT-OT-RWG-STRINGS `dbea74e6e`
(PREREG_PORT-OT-RWG-MODULES.md defines the knob, PREREG_PORT-OT-RWG-STRINGS.md the string slots).

## Change

Same knob, `MASHED_ORIG_TOP_RWMOD=1` (default OFF, read only on the MASHED_ORIG_TOP path). `RegisterRwModules`
(`LibRw/RwApiBind.cpp`) also fills two RwGlobals device slots. MASHED.exe values: the original-side probe of
PORT-OT-RWG-SLOTS (`ORIG_RWG.json`, RwGlobals at `0x02b499a8`, 8 samples 0.51 s .. 33.39 s), re-read by the
PORT-OT-RWG-MODULES probe (`D:/MashedWT/PORT-OT-RWG-MODULES_data/orig/orig_probe.json`, 33.02 s).

| slot | original | binding |
|---|---|---|
| +0x24 | 0x004d6910 (hooks.csv D3d9State_Get, C3) | the exe body `D3d9State_Get` (Render/D3d9StateCache.cpp) |
| +0x34 | 0x004dbc90 (no hooks.csv row) | new `RwIm2DBridge_RenderIndexedPrimitive` in the B15 bridge (D3d9Render/RwIm2DBridge.cpp), from the disassembly below |

**0x004dbc90** (capstone over `original/MASHED.exe.unpatched`). Arguments: primType `[esp+0xc]` at 0x004dbc98,
vertices `[esp+0x20]`, numVertices `[esp+0x24]` (0x004dbcfc), indices `[esp+0x28]` (16-bit, `mov ax, word ptr
[ecx + edx*2]` at 0x004dbdc3), numIndices `[esp+0x2c]` (0x004dbca0).
- Primitive count from numIndices, jump table 0x004dc144 on primType - 1 (0x004dbcad..0x004dbcb5):
  1 -> n / 2 (0x004dbcbc), 2 -> n - 1 (0x004dbcc9), 3 -> n / 3 (0x004dbcd2), 4 -> n - 2 (0x004dbce6),
  5 -> n - 2 (0x004dbcef), any other -> 0 (0x004dbcf8).
- D3D primitive type = dword table 0x005d8d8c[primType] = {0, 2, 3, 4, 5, 6} (read from the binary): LINELIST,
  LINESTRIP, TRIANGLELIST, TRIANGLESTRIP, TRIANGLEFAN for 1..5.
- numIndices <= numVertices (0x004dbd05): the vertices are expanded through the indices into the ring VB, 7 dwords
  (0x1c bytes) per vertex (`rep movsd`, ecx = 7, 0x004dbddf..0x004dbde9), then DrawPrimitive at device vtable +0x144
  (0x004dbec2). Otherwise (0x004dbee7): vertices and indices go into the ring buffers and DrawIndexedPrimitive at vtable
  +0x148 (0x004dc10e). Both draw the same primitives.
- Both paths add the words at `[[0x00911d60] + 0x60] + 0x1c / + 0x1e` to vertex x / y when either is non-zero
  (0x004dbda2..0x004dbe8e). 0x004dba60, the original +0x30, does the same (0x004dbb5b..0x004dbbcf). The bridge's +0x30
  (`Bridge_DrawPrimitive`, the binding already on the exe path) does not, and neither does the new +0x34.
- Returns `hr >= 0` (`setge dl`, 0x004dbedb).

Binding: `DrawIndexedPrimitiveUP(table[primType], 0, numVertices, count, indices, D3DFMT_INDEX16, vertices, 0x1c)`
after the same state setup as `Bridge_DrawPrimitive` (FVF, texture, stage and blend states, factored into one helper
both functions call; `Bridge_DrawPrimitive`'s behaviour does not change). Vertices go to the device raw, as 0x004dbc90
copies them. primType outside 1..5 returns 0 without a draw.

**Not in this step: +0x60** (original 0x004de1b0, hooks.csv C2, RW image <- D3D9 raster copy). Its one row,
0x004d52d0 (`re/match/s21/004d52d0.c`), reads its raster argument at `+0x22` (RW 3.x raster layout), and its callers
read `raster->w / ->h` (0x00498810, `re/match/s16/00498810.c`) or pass a texture's raster (0x00555ff0). On the knob path
textures and rasters are librw objects (`MashedRw_TextureRead_004c5cb0` builds an `rw::Raster`), where `+0x22` is
inside `stride` (librw `rwobjects.h` struct Raster). A non-null +0x60 would release the row in the verdict without the
row working, so +0x60 stays 0 and 0x004d52d0 stays at needs binding.

## Arms and pass criteria

| arm | what | pass |
|---|---|---|
| A0 default gates | `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` | G-CTRL 8284be46 / 67931834 / 31fe043e, B 1d5bf4f9 x3 |
| A1 knob boot | MASHED_ORIG_TOP=1 MASHED_ORIG_TOP_RWMOD=1 MASHED_DET_FRAMES=3000 (`ot_measure.py probe`) | exit 0; the registration log line names the device slots +0x24 +0x34 |
| A2 measure | `ot_measure.py verdict --orig` | Prediction: RwGlobals OK 67 / needs binding 5 (was 65 / 7). Released: 0x00413a40, 0x004430e0. 0x004d52d0 stays (+0x60). No row goes from OK to needs binding. |
| A3 captures | `capflake_runs.py` OFF@0 ON@0 OFF@0 ON@0, RWMOD=1 in every run's environment | all 12 hashes equal across OFF and ON, and equal to the PORT-OT-RWG-STRINGS runs (walk_04 = 4d53d93f87f0332d) |
| A4 lint | `py -3.12 scripts/lint_rva_bodies.py` | `[rva-lint] OK` |
