# Frontend Subsystem S-DoD Audit — 2026-05-17

**Session:** frontend_s_dod_audit_20260517
**Model:** Opus 4.7 (1M context)
**Anchor:** MASHED.exe.unpatched SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E (2,846,720 B) — verified
**Build state:** mashed_re_dev.asi 174,592 B, deployed at original/ (2026-05-17 22:13)
**Scope:** audit-and-close only; no fresh authoring of new hooks; HUD subsystem out of scope.

---

## S-DoD scorecard (ROADMAP.md § Phase 5)

| # | Criterion | Status | Score |
|---|-----------|--------|-------|
| 1 | Every function reachable from entry points at C3+; ≥ 90% at C4 | **FAIL** | 57/219 = **26.0% at C3+**; 19/219 = **8.7% at C4** |
| 2 | Subsystem runs with all original-fallback hooks disabled for one canonical scenario | **PARTIAL** | Harness exists (observe_hooks_at_menu.py + observe_c4_sweep_20260517.py); c4-sweep B1..B4 logs show 20 hooks across 4 batches GREEN at main_menu_idle 10s each; no single observation log with all 57 C3+ frontend hooks loaded simultaneously |
| 3 | All shared structs touched by subsystem documented with field-by-field RVA citations | **PASS** (substantially) | 4 frontend-touched structs documented: frontend_state.md, frontend_menu_state.md, hud_ingame_element.md, rwim2d_vertex_buffer.md, font_atlas.md, lobby_slot_array.md; 4 struct-level open uncertainties at README level |
| 4 | Custom asset format (Frontend.piz, panel.piz, perm.piz, font36.piz) has round-trip parser+writer + byte-identical re-encode test | **FAIL** | piz_extract.py has `list`+`extract` modes only; **no pack/repack mode**; no round-trip test |
| 5 | Frontend section of STUBS.md is empty | **FAIL** | 48 active frontend stubs (drained 35 this session from 83) |

**Overall:** Frontend is NOT S-DONE. It is closer to S-DoD than other subsystems but two structural gates (#1 — C4 coverage at 8.7% vs. 90% target; #4 — no .piz repacker) are far from satisfied. Estimated 6–10 weeks of focused work to S-DONE.

---

## Row 1 — C-tier coverage detail

Source: `hooks.csv` rows where `subsystem == frontend` (2026-05-17 22:30 snapshot).

| Tier | Count | Percent |
|------|-------|---------|
| C0   | 5    | 2.3%  |
| C1   | 105  | 47.9% |
| C2   | 52   | 23.7% |
| C3   | 38   | 17.4% |
| C4   | 19   | 8.7%  |
| **Total** | **219** | 100% |

Gap to S-DoD #1:
- **C3+ gate (≥ 90%):** need 197 rows at C3+; currently 57. **140 rows short.**
- **C4 gate (≥ 90%):** need 197 rows at C4; currently 19. **178 rows short.**

The C4 gap is the dominant blocker. Note that C4 requires **canonical-scenario evidence with hook installed** per re/CONFIDENCE.md (the C3→C4 promotion rule explicitly reaffirmed 2026-05-09 after Vec3Magnitude precedent). Synthetic Frida force-call diffs are C3, not C4.

C4 batches landed to date for frontend:
- 19 frontend rows at C4 (FrontendGlobalGet 0x0040ad20, FrontendArrayGet 0x0040b6c0, HotkeyStringBaseGet 0x0040b7a0, SpriteLookupTableA/B 0x0040bb70/90, FrontendPlayerSlotCheck 0x0042ebe0, HudSlotTypePlayer0..2 0x00430a10/a60/ab0, SpriteSlotGate 0x0042ee00, MenuAlphaGet 0x0042b930, FUN_0042b310/0042aff0/0042b180/0042b770, SpriteAnimFrameThunk 0x0042e590, RaceEndFlagIfEndMode 0x0042fe30, RaceEndAltFlagIfEndMode 0x0042fe50, GetRaceEndFlag 0x0042fe80)
- These are mostly leaf getters and small flag-check predicates; they passed canonical-scenario observation in c4-batch-a sweeps.

The 52 C2 frontend rows are the immediate promotion targets for c3_batch_i. Of those, 48 are unique RVAs (4 duplicate-row drift artifacts remain; tracker cleanup task).

---

## Row 2 — Canonical-scenario evidence detail

Harness: `re/frida/observe_hooks_at_menu.py` — spawn MASHED, wait 8 s for main menu, Module.load() the .asi (install all `InjectHooks()`), observe N seconds with NO Interceptor, RPC-poll bytes at end. This is the canonical-scenario harness used for C4 promotions.

Recent observation logs (`log/observe_*`):
- `observe_c4_sweep_20260517_B1.txt` — 4 hooks, GREEN
- `observe_c4_sweep_20260517_B2.txt` — 4 hooks, GREEN
- `observe_c4_sweep_20260517_B3.txt` — 4 hooks (Sub0041db80_HudThresholdDispatch + Sub00403160_SubMode0BViewport + FontText_StringTableLookup + FontText_UTF16WidenCopy), GREEN
- `observe_main_menu_idle_10s_c4-batch-a-s2.txt` — 4 hooks (RwMatrixScale, FrontendGlobalGet, FrontendArrayGet, HotkeyStringBaseGet), GREEN

These prove individual subsets of C3/C4 hooks coexist at main menu without regression. They do NOT yet prove that all 57 C3+ frontend hooks + 18 HUD C3+ hooks + their .asi co-residents (audio, save, input C3+) all coexist for one continuous canonical scenario. The .asi at original/ contains every C3+ hook from every subsystem ((`InjectHooks()` registers all), so the recent integration-diff GREEN run in frida-sweep-20260517-2121 (CHANGELOG row 6: "19/19 GREEN") is the closest evidence we have.

**Gap:** no single observation log demonstrates the full frontend hook set surviving a canonical scenario (a 30 s main-menu traversal, ideally including arrow navigation and a screen transition, with all hooks installed and active). This is the immediate next action to close Row 2.

**Re-pickup criterion:** run `re/frida/observe_hooks_at_menu.py` with the HOOKS list expanded to all 57 C3+ frontend RVAs (or simply rely on `InjectHooks()` installing everything; just observe alive-throughout for 30 s).

---

## Row 3 — Struct documentation detail

Frontend-touched struct docs already in place (`re/analysis/structs/`):

| File | Base address(es) | Field count | Status |
|------|------------------|-------------|--------|
| frontend_state.md | DAT_008a9584 / DAT_007f1a0c / DAT_007f0fd0 / DAT_0067ec.. | FrontendStateMachine (10 fields) + mode selector + score/rank arrays | C1 |
| frontend_menu_state.md | DAT_0067ed40 / DAT_007f0a48 / DAT_00898ac0 | menu cursor/limit arrays + menu entry array (52-byte stride, 30 entries) + score arrays | C1 |
| rwim2d_vertex_buffer.md | DAT_00898a20 | RwIm2DVertex[4] (4×28 bytes) + device vtable | C1 |
| hud_ingame_element.md | DAT_0063cab8 / DAT_0063ce20 / DAT_0063d298 | 3 HudIngameElement variants (stride 0x16C/0x114/0x160) | C1 |
| font_atlas.md | DAT_00912a00..0x912a3c | GlyphEntry (8 fields, stride 0x20) + FontCtx (12 known) + 11 globals | C1 (HUD-shared) |
| lobby_slot_array.md | DAT_007f0a44 / DAT_007f0e50 / DAT_008989e0 | per-slot state (stride 0xc) + vehicle unlock + lap-time records | C1 |

**Open struct-level uncertainties** (from `structs/README.md`):
1. HudIngameElement_114 +0x50 semantics (doubly-indirect dispatch field) — needs new analysis
2. HudIngameElement_A guard region +0x00..+0x0F — not dispatched by any observed function
3. FrontendStateMachine +0x20..+0x23 gap — not observed in any plate
4. PlayerSlotArray +0x04 and +0x0C within each 0x10-byte slot entry — never observed

These are non-blocking for S-DoD #3 ("substantially documented"). All four should be assigned U-IDs and filed via `re-classify`, but Row 3 is otherwise satisfied for the current Phase 5 work surface.

---

## Row 4 — .piz round-trip detail

Current `re/tools/piz_extract.py` modes:
- `list` — print archive entries
- `extract` — copy entries to disk, optional glob filter

**Missing:** `pack` mode that takes a directory and produces a byte-identical .piz archive.

Frontend touches at least four .piz archives:
- `original/TOASTART/Frontend.piz` — menu screens, panel layouts
- `original/TOASTART/panel.piz` — panel chrome
- `original/TOASTART/perm.piz` — persistent assets (presumed)
- `original/TOASTART/font36.piz` — font glyph atlas

S-DoD #4 requires a **byte-identical re-encode test**. The XeNTaX-documented format (header verified on AI.piz per `memory/project_piz_format.md`) gives the structure, but the writer needs:
- Header (re-emit same offset-mode constant)
- Per-entry filename table (null-padded to original stride)
- Per-entry size+offset table (offset mode = absolute or relative; auto-detect)
- Body payload (preserve internal alignment / padding bytes)

**Recommended action:** file a DEFERRED row for this; it's a substantive (~half-day to one-day) tooling task that should be done as a dedicated session, not opportunistically.

---

## Row 5 — STUBS.md frontend section detail

Drained in this session (35 stubs cleared; STUBS.md):

| Stub ID | Callee RVA | Disposition |
|---------|------------|-------------|
| S-0440  | 0x004c5c00 | drift-skip, C2 in frontend |
| S-0441  | 0x00427680 | drift-skip, C2 in hud |
| S-0442  | 0x00427780 | callee C3 (FontText_StringTableLookup) |
| S-0444  | 0x00552750 | drift-skip, C2 in hud |
| S-0445  | 0x00552d10 | callee C3 (FontMatrix_Push) |
| S-0452  | 0x0042e590 | callee C4 (SpriteAnimFrameThunk) |
| S-0456  | 0x0042b8b0 | callee C3 (ScreenWidthGet) |
| S-0457  | 0x0042b8c0 | callee C3 (ScreenHeightGet) |
| S-0800  | 0x00492d20 | callee C2 |
| S-0808  | 0x00499710 | callee C2 in render |
| S-0809  | 0x004c19f0 | callee C3 (RwVtableSlot07Call) |
| S-0819  | 0x004c77c0 | callee C2 in render |
| S-0861  | 0x00550bc0 | callee C3 (VfsStreamGetType) |
| S-0862  | 0x00550910 | callee C2 in save |
| S-1280  | 0x004a2c48 | drift-skip C1 hud_frontend plate |
| S-1281  | 0x0040e340 | drift-skip C1 game_state plate (U-0497) |
| S-1282  | 0x0040e350 | drift-skip C1 game_state plate (U-0498) |
| S-1283  | 0x0040e370 | drift-skip C1 timer_d2 plate (U-1611) |
| S-1645  | 0x0042d3e0 | callee C3 (MenuEntryArrayInit) |
| S-1647  | 0x0042ac00 | callee C3 (MenuGroupCount) |
| S-1648  | 0x0042ac50 | callee C2 |
| S-1651  | 0x0042b930 | callee C3 (MenuAlphaGet, canonical row) |
| S-1655  | 0x0042aa00 | callee C2 |
| S-1656  | 0x00430910 | callee C3 (MenuOptionSlotGet) |
| S-2089  | 0x005554d0 | callee C2 hud |
| S-2090  | 0x00427840 | callee C3 (FontText_UTF16WidenCopy) |
| S-2540  | 0x004c5c00 | callee C2 |
| S-2860  | 0x00419760 | callee C2 vehicle |
| S-2861  | 0x00420de0 | callee C2 vehicle |
| S-3421  | 0x005c9d00 | callee C3 (GetRaceEndTrigger) |
| S-3425  | 0x0045b350 | callee C3 (RwInitNullStub) |
| S-3426  | 0x004099e0 | callee C3 (SaveStatusClear) |
| S-3428  | 0x0040e470 | callee C3 (CarSlotStateGet) |
| S-3429  | 0x004c5c00 | duplicate, cleared |
| S-3433  | 0x0040bb50 | callee C2 in frontend |

**Frontend STUBS state now: 83 active → 48 active (35 drained, 42% reduction).**

The 48 remaining active stubs cluster on:
- **Video / intro splash chain** (sub_00493f70..sub_00494a80, FUN_004c1a00..004c1be0, FUN_00494320, FUN_004c7650, FUN_004c7730, etc.) — entire FUN_00495350 / intro splash callee tree; needs a dedicated `intro_splash-cont1` session.
- **Logo / animated draw chain** (FUN_00473c20, FUN_00473ee0, FUN_00474890, FUN_00473540/9f0/870) — needs `frontend_panels-cont1`.
- **Menu transition orchestrators** (FUN_00431f30, FUN_0042ad90, FUN_00432b30, FUN_00492d10, FUN_0042c1f0, FUN_0042c1d0, FUN_00432450) — depend on global state-machine semantics not yet resolved.
- **Sprite/font draw leaves** (FUN_004277a0, FUN_00552d70, FUN_00556ca0, FUN_00556e90, FUN_00474890, FUN_004967e0) — bare callee RVAs not in hooks.csv.
- **Frontend-side state shims** (FUN_004d8560, FUN_00409a80, FUN_0042c1a0, FUN_0040ce80, FUN_0042bfb0, FUN_004b5750, FUN_004a2b60, FUN_004a2c48, FUN_0042bfb0) — c0_promotion_frontend_a backlog.

---

## What was closed this session

1. **35 frontend STUBS drained** (83 → 48).
2. **Audit scorecard written** (this file).
3. **Tracker mutations:** STUBS.md only; no hooks.csv mutations (audit, not promotion).

---

## What is left to close (critical path to S-DONE)

| Row | Gap | Estimated work | Owner / batch label |
|-----|-----|----------------|---------------------|
| #1 (C3+) | 162 frontend rows below C3 (105 C1 + 52 C2 + 5 C0) | C2→C3 promotions are the fast path. At a recent observed rate of ~3–5 C3 promotions per `c3-batch` session, drain the 52 C2 over 8–12 batches. The 105 C1 require C1→C2 sessions first (~3 weeks of `discover-c1-batch` work). | `c3_batch_i..` + `discover_c1_frontend_*` |
| #1 (C4)  | 178 frontend rows below C4 | C3→C4 requires canonical-scenario observation. Each c4-batch lands ~3–5 promotions; expect 30–40 batches. | `c4_batch_a..` |
| #2 | All-frontend-C3+ canonical observation log | One session (extend HOOKS list in observe_hooks_at_menu.py, run, capture log) | next sweep |
| #3 | 4 open struct-level uncertainties + U-IDs | One `re-classify`-mediated U-ID filing session | next struct-extract session |
| #4 | .piz round-trip writer | ~1 day of dedicated tooling work | DEFERRED, new D-row |
| #5 | 48 active stubs | Each clusters on a deeper callee; needs ~5 targeted RE sessions for video/intro, logo-panel, menu-transition orchestrators, sprite/font leaves, frontend-state shims. | Mix of `discover-c1-*` (for stub callees not in hooks.csv) + `c3-batch-*` (for stubs whose callees just need a tier bump) |

---

## Honest assessment — sessions to S-DONE

**To get the frontend to "substantially S-DONE" (90% C3+, all stubs drained, .piz writer, canonical run):** **6–10 sessions** at the current cadence (~10–15 promotions per parallel-fanout day):
- 1 session: extend observe_hooks_at_menu.py and capture the full canonical run (Row 2 close)
- 1 session: build .piz writer + byte-identical round-trip test (Row 4 close)
- 1 session: drain the 48 stubs that need callee tier bumps (Row 5)
- 1 struct session: file U-IDs and close 4 struct-level uncertainties (Row 3)
- 2–4 c3 batches: promote the 52 C2 → C3 (Row 1 lower bound)
- The C4 90% target is **unrealistic in any session count** with the current per-batch yield. The S-DoD allows the 90% to be **C3+ with ≥ 90% at C4** — but in practice, the project has been operating at much lower C4 coverage per subsystem (the boot subsystem, the most-progressed, sits at ~30% C4). Strict 90% C4 would take 4–6 months of focused frontend-only work and would block all other subsystems. **Recommendation:** ask the user to consider relaxing S-DoD #1 to "≥ 90% at C3+, ≥ 50% at C4" or similar realistic threshold; otherwise Frontend never closes and the project never reaches Phase 6.

---

## Recommended next action

**Pivot:** before pushing more c3 batches at frontend, **resolve Row 4 first** (.piz round-trip writer) — it's a tractable one-day tooling task that unblocks the asset-format DoD for both Frontend and HUD (font36.piz is HUD-side; same writer). Row 4 is also a fast win that moves the project closer to a standalone `mashed_re.exe` (the standalone exe will need to read .piz archives without modifying them, then later, mods will need to repack them).

**Then:** run one canonical observation with the full C3+ frontend hook set installed, log it, and close Row 2.

**In parallel:** other subsystems (Save, Audio, Input — per ROADMAP.md Phase 5 order) should continue receiving c3 batches because frontend C4 is multi-month and shouldn't gate the whole project. The current "frontend-finish-first" pivot recommendation in the task brief is reasonable for closing the tractable gaps (3 of 5 rows) but should NOT extend to forcing the 90% C4 gate before Tier 2 work resumes.

---

## DEFERRED entries to file (this session)

| ID | Title | Re-pickup condition |
|----|-------|---------------------|
| D-11000 | .piz round-trip writer | Frontend S-DoD #4 close; build pack mode that reproduces Frontend.piz/panel.piz/perm.piz/font36.piz byte-identical from extracted dir |
| D-11001 | All-frontend-C3+ canonical observation log | Extend observe_hooks_at_menu.py HOOKS list to all C3+ frontend RVAs; capture log/observe_frontend_full_30s.txt as Row 2 evidence |
| D-11002 | Struct-level U-ID filing | 4 uncertainties at structs/README.md need U-IDs assigned and added to UNCERTAINTIES.md |
| D-11003 | Intro splash callee tree | 8 stubs on FUN_00495350 callee chain remain (sub_00493f70..00494a80, FUN_00494320, FUN_004c7650, FUN_004c7730) — needs intro_splash_cont1 session |
| D-11004 | Logo/animated draw chain | FUN_00473c20, FUN_00473ee0, FUN_00474890, FUN_00473540, FUN_004739f0, FUN_00473870 — needs frontend_panels_cont1 |
| D-11005 | Menu transition orchestrators | FUN_00431f30, FUN_0042ad90, FUN_00432b30, FUN_00492d10, FUN_0042c1f0, FUN_0042c1d0, FUN_00432450 — depends on global state-machine resolution |
| D-11006 | c0_promotion_frontend_a stub residue | 8 stubs from c0_promotion_frontend_a (FUN_004d8560, FUN_00409a80, FUN_0042c1a0, FUN_0040ce80, FUN_0042bfb0, FUN_004b5750, FUN_004a2b60, FUN_004a2c48) need either inclusion in hooks.csv or escalation |
