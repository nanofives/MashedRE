# hud area — round 2: Rt2d library-attest calibration (2026-09-01)

Bus item **B-0001** (hud->render): is the `0x00553000-0x00557fff` font-vector band vendored
RenderWare Rt2d? Parent directed a library-attest calibration + a reclass-OUT txn if confirmed.

## Verdict: **REFUTE** — the font-vector band is FIRST-PARTY, not vendored RenderWare. No reclass-OUT.

The calibration (reading the bodies + classifying the callees + checking the port) shows these
are Mashed's **own vector-font subsystem** (loads the game asset `FGDC20.RWF`, parses PostScript
glyph outlines, stroke-renders via the project's Im2D bridge). Their RenderWare *appearance* is
entirely in their callees, which are already correctly classified. This overturns the round-1 F1
doubt-note (which trusted the plates' unverified "module-vendor-doubt"); the doubt note itself
said "MAY be vendored ... if a calibration confirms" — it does not confirm.

## Two groups (must not be conflated — the ~15-row list mixed them)

- **Group A — hand-plated, named, some C3-implemented first-party font code:** `FontCanvas_Init`
  (005540d0), `FontGlyph_UploadData` (00553f40), `FontSys_InitFontPool/InitDataPools/InitBuffers`,
  `FontCtx_LoadMetrics_Met/Atlas`, `FontSys_Shutdown*`, `FontCtx_BuildExtTable`, `SetDat00912a20`
  (00556cc0, **C3-impl**), `GetDat00912a20` (00556cd0, **C3-impl**), `00557110` (**C3-impl**).
  Recovered semantic names + implementations = deliberately first-party. **Never reclass-OUT.**
- **Group B — anonymous `FUN_`/`LAB_` rows carrying the doubt note** (`bucket_gameplay_004e4800_00558030`):
  `00554010`, `00554150`, `00554200`, `00554390`, `00555830`, `00556780`, `00556e40`, `005572b0`,
  `005572c0`. These were the only legitimate reclass-OUT candidates. The calibration below is on them.

## Evidence (Group B) — decisive facts, each cited

1. **The callees that make the band "look RW" are first-party render/boot, not third-party:**
   | callee | name | subsystem | conf |
   |---|---|---|---|
   | 004cd070 | `RwRenderPrimitiveSubmit` | **render** | C2 |
   | 004cd140 | `RwRenderCommandBufferReset` | **render** | C3 |
   | 004cd170 | FUN_004cd170 | **render** | C2 |
   | 005c4c60 | FUN_005c4c60 (handle-grow) | **boot** | C2 |
   | 005c4d30 | `CondGet5c4d30` (handle-resolve) | **boot** | C3 |
   | 005c4da0 | FUN_005c4da0 (handle-value) | **boot** | C2 |
   Only genuinely third-party callees: `00550a20` (RW line-reader, C1) and `0055deb0`
   (`RwpWorldSolverHandle_RegAbi`, RW-Physics-3.7 C3 — a generic handle-resolve reused here).
   A first-party function calling RW/boot/render APIs is not itself vendored (CONFIDENCE anti-island
   rationale, L23-26): the RW-ness is the *callees*, already correctly C1/C2/C3-tagged.
2. **The asset is Mashed-specific.** `FGDC20.RWF` lives in `original/TOASTART/Common/Font36.piz`
   (a game `.piz`). `00554390` is the FGDC20.RWF binary loader; `00556780` is the PostScript-outline
   text loader. Loading a game asset is first-party integration, not stock RW.
3. **The port already reimplements it first-party.** `mashedmod/src/mashed_re/D3d9Render/MashedFont.cpp`,
   `MenuStringTable.cpp`, `TextRenderer.h`, `RwIm2DBridge.cpp`. The `00554390` plate itself: "the
   function whose behavior the standalone B19 faithful-font work reimplemented for FGDC20.RWF."
   Reclassing to library-skip C1 would contradict active first-party port work.
4. **No FidDB match** (per the doubt note) cuts *against* vendored: stock RW leaves usually FidDB-match
   (cf. the qhull precedent, `library_residue/qhull.md`, all FidDB-attested). The RpPatch precedent
   was stock RW *plugin internals*; this band is Mashed glue that *calls* RW, structurally different.

## Actions

- **Do NOT queue a reclass-OUT.** (Reported to parent; the parent owns the apply and the premise was
  overturned.)
- **Recommended tracker hygiene (parent, via re-classify):** resolve the standing `module-vendor-doubt`
  on the Group B plates to **REFUTED — first-party FGDC20.RWF vector-font subsystem** so the doubt does
  not resurface. This converts ~9 rows from "in-doubt C2" to "confirmed first-party C2" — real progress
  on the *understanding* axis even though residue count is unchanged.
- Residue does NOT shrink via this lever. hud's real path forward is authoring/verifying the first-party
  font + HUD-compose code, not a reclass.
