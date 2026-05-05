# SESSION_END — split_screen-20260505

**Session ID**: split_screen-20260505  
**Bucket**: split_screen  
**Date**: 2026-05-05  
**Slot**: Mashed_pool3 (pre-assigned; .lock~ only, unlocked)  
**Binary anchor**: MASHED.exe SHA-256 = `BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E` ✓

---

## Anchor outcome

**FOUND via Strategy 3 (render_frame call-graph).**  
Prior session (`render_frame_d3-20260503`) had already noted `InGameRenderDispatcher` as having a "4-player loop". This session confirmed via decompilation and extended the analysis.

All Strategy 1 string searches (splitscreen, SplitScreen, viewport, numPlayers, player2, 2P/3P/4P) returned **zero results**. No debug strings naming the split-screen system exist in the binary.

---

## Anchor — SPLIT_FN

**`InGameRenderDispatcher` (0x00410b30)** — strategy: render_frame call-graph (prior session note + decompile confirmation).

Body: 0x00410b30..0x00410d03 (~0x1D3 bytes).

Two 4-player loops inside:

```c
// Loop 1 — per-player scene render (conditional, guarded by DAT_0063ba98 == 0)
if (DAT_0063ba98 == 0) {
    iVar2 = 0;
    do {
        FUN_00420050(iVar2, *DAT_007d3ff8);   // PerPlayerViewportRender
        iVar2++;
    } while (iVar2 < 4);
}

// Loop 2 — per-player overlay/HUD (unconditional)
iVar2 = 0;
do {
    FUN_0045b350(iVar2, *DAT_007d3ff8);       // STUB — empty body
    iVar2++;
} while (iVar2 < 4);
```

The second 4-player loop target `FUN_0045b350` (0x0045b350) has body_start == body_end — it is a **stub** (already C1/mapped from rw_engine_init session).

---

## D3D9 SetViewport call-site count

**0 explicit sites** — D3D9 methods are all COM vtable dispatch; `IDirect3DDevice9::SetViewport` would not appear in the IAT. Not findable via imports list.

The per-player viewport positioning mechanism uses **RenderWare camera rasters** (each player's camera renders to a different screen subrect via `RwCamera` rasters at offsets +0x60/+0x64). The actual screen-region assignment per player is **not yet located** — U-1908 opened.

---

## Player-count strings observed

- `"MultiPlayer"` at 0x005cd7cc — referenced DATA from 0x0042eef0 (inside `FUN_0042ee40`)
- `"?player %d points : %d\n"` at 0x005ccdf7 — debug scoring format
- No `"2P"`, `"3P"`, `"4P"`, `"splitscreen"`, `"viewport"`, `"numPlayers"` strings found

The `"MultiPlayer"` reference from 0x0042eef0 is in `FUN_0042ee40`, which switches on `DAT_0067e9fc` (game mode) and returns `FUN_0040bb90()` for valid mode+player combos. Mode 2 appears to be multiplayer.

---

## Functions covered

| RVA | Name | Status |
|-----|------|--------|
| 0x00410b30 | InGameRenderDispatcher (SPLIT_FN) | C1/unmapped (prior); notes extended |
| 0x00420050 | PerPlayerViewportRender | **C0→C1** |
| 0x0042f530 | ViewportSetup | C1/unmapped (prior); U-1716 resolved |
| 0x0042f660 | DefaultViewportCameraInit | **C0→C1** |
| 0x0041faf0 | VehicleShadowRender | **C0→C1** |
| 0x0041fcc0 | TireMarkRender | **C0→C1** |
| 0x0042c960 | CameraTransitionStateMachine | **C0→C1** |
| 0x004c1b40 | FrustumSphereTest | C1/unmapped (camera_follow session); cross-confirmed |
| 0x00492e90 | FUN_00492e90 (top-level render machine) | C1/mapped (prior); cross-confirmed |
| 0x0042ee40 | FUN_0042ee40 (mode+player selector) | C0/deferred (hud_frontend_d2); notes conflict observed |
| 0x0045b350 | FUN_0045b350 (stub) | C1/mapped (prior); confirmed empty |

**5 new C0→C1 promotions.**

---

## Tracker mutations

| Tracker | Change |
|---------|--------|
| hooks.csv | 1 row updated C0→C1 (PerPlayerViewportRender); 9 new rows appended |
| DEFERRED.md | D-5038 CLEARED; D-5620..D-5624 appended (5 entries) |
| UNCERTAINTIES.md | U-1716 RESOLVED; U-1907..U-1912 appended (6 entries) |
| STUBS.md | S-1642 CLEARED (CameraTransitionStateMachine identified) |
| re/analysis/CHANGELOG.md | 6 entries (5 individual + 1 BATCH) |

---

## Deferred entries filed

| ID | RVA | Description |
|----|-----|-------------|
| D-5620 | 0x0041f8f0 | depth-2 callee of PerPlayerViewportRender; unknown pre-render step |
| D-5621 | 0x004228f0 | per-player mirror/reflection pass |
| D-5622 | 0x00426060 | 5-byte getter; world render target |
| D-5623 | 0x004260c0 | 5-byte getter; render condition flag |
| D-5624 | 0x004e4900 | fog/scale matrix transform |

Bucket: `split_screen-cont1`

---

## Uncertainties opened / resolved

- **U-1716 RESOLVED**: `DAT_0067ed68` is a transition animation slider, not per-player viewport coord
- **U-1907**: per-player struct at `DAT_0063dc38` stride 0x2AC — full layout unknown
- **U-1908**: per-player camera raster screen-quadrant assignment — mechanism unknown
- **U-1909**: `DAT_0067e9fc` game-mode enum full mapping
- **U-1910**: `DAT_0063ba98` role (guards PerPlayerViewportRender loop)
- **U-1911**: `FUN_0040bb90` type/return value
- **U-1912**: per-player struct flag bits at +0x3C full enum

---

## ID ranges consumed

- U-1907..U-1912 of U-1907..U-1926 pre-assigned (6 of 20 used)
- D-5620..D-5624 of D-5620..D-5679 pre-assigned (5 of 60 used)
- S: none new; S-1642 cleared
- cap_count: 0 (split-screen confirmed found)

---

## Pool / MCP

- Slot Mashed_pool3 used (pre-assigned; was unlocked with .lock~ only)
- MCP session `797b2164b3ab4634ae1ce58ffacd46d7`

---

## Key finding for greenfield port

**Mashed PC build RETAINS 4-player split-screen.** The `InGameRenderDispatcher` renders each player's viewport in sequence (player 0 through 3) via `PerPlayerViewportRender`. Each player has a dedicated camera and per-player struct (stride 0x2AC). The HUD overlay loop is present but stubbed. The actual per-player viewport screen-position assignment happens via RenderWare camera rasters — U-1908 to resolve in `split_screen-cont1`.
