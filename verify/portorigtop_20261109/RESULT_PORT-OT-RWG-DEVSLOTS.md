# RESULT PORT-OT-RWG-DEVSLOTS: RwGlobals device slots +0x24 / +0x34 on MASHED_ORIG_TOP_RWMOD, 2 more rows released

Branch `orch/PORT-OT-RWG-DEVSLOTS`, stacked on orch/PORT-OT-RWG-STRINGS `dbea74e6e` (orch/integration was still at
`6144429be`, so no merge). PREREG `5a734e8af` (committed alone), code `5f62a3456`. Worker cmwajfohv25e9le1crcu5f7x6.

## Verdict: GREEN

- With `MASHED_ORIG_TOP=1 MASHED_ORIG_TOP_RWMOD=1`, RwGlobals rows: **OK 67, needs binding 5**. PORT-OT-RWG-STRINGS had
  65 / 7. This matches the PREREG prediction.
- Released (`RWG_DEVSLOTS_RELEASED.tsv`): 0x00413a40 (+0x24 +0x34), 0x004430e0 (+0x34).
- No row went from OK to needs binding. The seed columns are unchanged (111 / 13), row for row.
- 0x004d52d0 stays at needs binding on +0x60, by design (below).
- Default path unchanged: G-CTRL `8284be46 / 67931834 / 31fe043e`, B `1d5bf4f9` x3.

## The change

Knob: same as MODULES / STRINGS, `MASHED_ORIG_TOP_RWMOD=1`, set in `RegisterRwModules` (`LibRw/RwApiBind.cpp`). It runs
only on the MASHED_ORIG_TOP path. MASHED.exe values are from the original-side probe:
`D:/MashedWT/PORT-OT-RWG-MODULES_data/orig/orig_probe.json`, RwGlobals block at 33.02 s, the `original:` column of
`OT_MEASURE.tsv`. The PORT-OT-RWG-SLOTS probe (`ORIG_RWG.json`) shows the same values.

| slot | original | binding | exe probe (22.0 s) |
|---|---|---|---|
| +0x24 | 0x004d6910 (hooks.csv D3d9State_Get, C3) | the exe body `D3d9State_Get` (Render/D3d9StateCache.cpp) | `0x00191540` |
| +0x34 | 0x004dbc90 (no hooks.csv row) | `RwIm2DBridge_RenderIndexedPrimitive` (D3d9Render/RwIm2DBridge.cpp, new) | `0x00152e50` |
| +0x60 | 0x004de1b0 (hooks.csv C2) | not bound, stays 0 | `0x0` |

**+0x24.** `Render/D3d9StateCache.cpp` was compiled into the .asi only. It is now in `mashedmod/exe_sources.rsp` too, so
it is shared by both targets. rva-lint OK, with no new finding.
- The body reads the exe's bytes at the input caches 0x007d6ae8..0x007d6bf8 / 0x007d58b8.
- The bridge's set-state (+0x20, `Bridge_SetRenderState`) does not write those caches. The original's +0x20
  (0x004d7480) is a different function.
- The one ORIG_TOP reader, 0x00413a40, gets states 0x14 and 0xe (`re/match/s03/perm_00413a40.c:69/73`) and hands each
  value back to +0x20 afterwards (`:91/93`). `Bridge_SetRenderState` ignores both states (its `default:` case).
- [UNCERTAIN] What values the getter returns on the knob path. Evidence missing: a read of 0x007d6b18 / 0x007d6b1c in
  the exe while 0x00413a40 runs. It does not change the draw, because both values only go back to states the bridge
  ignores.

**+0x34**, from the disassembly of 0x004dbc90 (`original/MASHED.exe.unpatched`, `re/tools/disasm_va.py`):

Arguments: primType `[esp+0xc]` (0x004dbc98), vertices, numVertices (0x004dbcfc), 16-bit indices (0x004dbdc3), and
numIndices (0x004dbca0).

The primitive count comes from numIndices, through jump table 0x004dc144 = {0x004dbcbc, 0x004dbcc9, 0x004dbcd2,
0x004dbce6, 0x004dbcef}:

| primType | primitives | D3D type (table 0x005d8d8c = {0, 2, 3, 4, 5, 6}) |
|---|---|---|
| 1 | n / 2 | 2 |
| 2 | n - 1 | 3 |
| 3 | n / 3 | 4 |
| 4 | n - 2 | 5 |
| 5 | n - 2 | 6 |
| other | 0 (0x004dbcf8) | |

Vertices are 7 dwords each, copied raw (0x004dbddf..0x004dbde9). The original has two paths:
- expand through the indices, then DrawPrimitive at vtable +0x144 (0x004dbec2)
- DrawIndexedPrimitive at vtable +0x148 (0x004dc10e)

Both draw the same primitives. The function returns `hr >= 0` (0x004dbedb).

The binding:
- calls `DrawIndexedPrimitiveUP` with the same type, count and stride 0x1c, after the bridge's state setup
- returns 1 / 0 the same way
- returns 0 without a draw for primType outside 1..5. [UNCERTAIN] 0x004dbc90 submits a 0-primitive draw there. No
  ORIG_TOP row passes such a type: 0x00413a40 and 0x004430e0 pass 3.
- does not add the camera raster offset. Both 0x004dbc90 (0x004dbda2..0x004dbe8e) and 0x004dba60 (0x004dbb5b..0x004dbbcf)
  add `[[0x00911d60] + 0x60] + 0x1c / + 0x1e`, but the bridge's existing +0x30 does not, so the two bridge draws stay
  consistent.
- Refactor: `Bridge_DrawPrimitive`'s state setup (FVF, texture, stage, sampler and blend states) moved unchanged into
  `ApplyBridgeDrawState()`, and both draws call it. The captures and gates below show the default path unchanged.
- [UNCERTAIN] Whether the A1 boot ran 0x00413a40 or 0x004430e0 at all, so the new draw is not exercised by any run here.
  Evidence missing: a call counter or log line in `Bridge_DrawIndexedPrimitive`. The verdict is static plus slot values,
  as for MODULES / STRINGS. Each swap's own gates remain the authority.

**+0x60 not bound.**
- Its one row, 0x004d52d0 (`re/match/s21/004d52d0.c:38`), reads its raster argument at `+0x22`, which is the RW 3.x raster
  layout.
- Its callers read `raster->w / ->h` (0x00498810, `re/match/s16/00498810.c:51`) or pass a texture's raster (0x00555ff0).
- On the knob path, rasters are `rw::Raster` (`MashedRw_TextureRead_004c5cb0` creates one). There, `+0x22` falls inside
  `stride` (`deps/librw/src/rwobjects.h`, struct Raster).
- A non-null +0x60 would release the row in the verdict without the row working.

## Arms (PREREG)

| arm | result |
|---|---|
| A1 knob boot | PASS on the second try: rc 0, 7 samples, the RwGlobals record at `0x00ffb368` from 6.0 s. The exe log has `... string slots +0xc4 +0xd0 +0xdc +0xe8 +0xf4 +0xf8 +0xfc, device slots +0x24 +0x34, record 00FFB368 size 0x440`. First try: rc 1, RwGlobals 0 at all 8 samples. Its log stops at `dllguard: d3d9.dll` inside `rw::Engine::open` (RwEngineOpen), before piece 4 and before any code this step changes, with neither `open ok` nor `FAILED` printed. It ran right after a 10-minute test-slot memory wait. The retry passed with the same exe. |
| A2 measure | PASS. OK 67 / needs binding 5, seed 111 / 13 unchanged. |
| A0 gates | PASS, exe sha8 `abbd69ae`. H_DEF step `8284be46`, gates `67931834`, ptrace `31fe043e`. B_DEF_K0 / K1 / K26 `1d5bf4f9`. |
| A3 captures | PASS. OFF@0 ON@0 OFF@0 ON@0, RWMOD=1 in every run's environment: rc 0 x4, freeze `pinned pos=55 hash=28538c74`. All 12 hashes are equal across the four runs and equal to the PORT-OT-RWG-STRINGS runs (walk_04 `4d53d93f87f0332d`). |
| A4 lint | PASS. `[rva-lint] OK -- no NEW duplicate bodies (2 known)`, in build.bat and standalone. |

## Remaining 5 needs-binding rows

| row | blocker | original |
|---|---|---|
| 0x004ce790, 0x004ce8e0 | image +0x21c empty | heap list, filled through 0x00493600 (PORT-OT-IMAGE-FMT) |
| 0x004d52d0 | +0x60 | `0x004de1b0`. Needs RW-layout rasters on the knob path, or a 0x004d52d0 binding on `rw::Raster` |
| 0x0047ade0 | +0x0 | the original holds 0 here too |
| 0x004c1680 | data +0xbc | heap pointer in the original |

## Evidence (raw data in `D:/MashedWT/PORT-OT-RWG-DEVSLOTS_data`, not committed)

| file | SHA-256 |
|---|---|
| probe.json (exe, ORIG_TOP + RWMOD, rc 0, retry) | 0df45090f6d4cc4dd21779948d6b55ef2fb19ae360135f2fec0c464b8ba88ea5 |
| probe_mashed_re.log (registration line 322) | 93f4ebb143ea0f516a3145e6b0bfb41c8a7bffce1c270a5b8d2a919fca76e62e |
| probe_try1.json (first try, rc 1) | 2f9cde2012ea7b2b5357ba99fb17a72ce08f4438d393ae42dea6f11ceb3b0ec3 |
| probe_try1_mashed_re.log | d5db930909b8860645708ea81022d0caae2417125695487b16555f6c9a9538dd |
| probe_run.log (retry) | 67400e097c3f4e08c382265abe31dc07547110b67665d147b07a3dcbaffda7ee |
| OT_MEASURE_before.tsv (the STRINGS table) | 6c0bf9c7b10068b6315320ee8307bf3158aa5e365595d19f62619998ba8615d6 |
| arms_run.log (first A1, A0, A3) | ba9cb9c56838759cd84f55951dd549c45299361efe4b69739b52732a414e3d8a |
| cap/runs.tsv (A3) | efd1dd4ea7832bd9d622384c9547bd569426e2886e14b4d2cef558587bc1b64f |
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| mashed_re.exe (built from 5f62a3456) | abbd69ae8dd904643d13a7bba58d81f4005d839478de3f94da599a7df5fa1aa3 |
| ../PORT-OT-RWG-MODULES_data/orig/orig_probe.json (original values, --orig) | ae6d8a62f6ec97dc8f530f15796a8d4649864ab07cf7fa3ad6275a968b1cf2c2 |

## REPORT

```
REPORT PORT-OT-RWG-DEVSLOTS
verdict: GREEN
branch: orch/PORT-OT-RWG-DEVSLOTS @ <RESULT commit>
evidence: verify/portorigtop_20261109/RESULT_PORT-OT-RWG-DEVSLOTS.md
gates: G-CTRL 8284be46/67931834/31fe043e  B 1d5bf4f9/1d5bf4f9/1d5bf4f9
decision: none
next: PORT-OT-IMAGE-FMT (0x00493600 formats -> 0x004ce790/0x004ce8e0); +0x60 needs 0x004d52d0 on rw::Raster first
```
