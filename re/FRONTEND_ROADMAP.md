# Roadmap: standalone `mashed_re.exe` running MASHED's frontend

Authored 2026-05-24. Target: `mashed_re.exe` launches without `MASHED.exe`,
shows the MASHED main menu, navigates correctly via keyboard/joystick.

This is the plan to take the project from **dev-time hook DLL** (current
state) to **standalone exe rendering the frontend** (target).

## Current position

- `mashed_re_dev.asi` injected into `MASHED.exe`: **294 hooks active and
  diff-original GREEN** under the dinput8 ASI loader.
- `mashed_re.exe` builds but only as an asset-tooling stub (5 .cpp files).
  None of the reimpl code links into it.
- ~99 hooks still mass-disabled (refused diff-original / boot-crashers /
  untouched-since-loader-broken-9d audit).

## Gap

```
   we are here ↓                                          ship here ↓
   .asi w/ hooks ──→ asi w/ everything ──→ standalone exe (frontend) ──→ standalone full game
   294 hooks GREEN                          (this roadmap target)            (Phase 6+)
```

**What we don't have for standalone:**
1. A `WinMain` that boots without `MASHED.exe`.
2. A working D3D9 device + window in standalone (we have the shim; it
   only proxies MASHED's).
3. The RenderWare 3 runtime. MASHED links RW3 statically; we have no
   RW3 SDK; every RW call needs a replacement, stub, or proxy.
4. C++ implementations of asset readers in the standalone target.
5. Standalone DirectInput init + per-frame state read.
6. Standalone intro player path (or skip).

## Phases

### Phase A — Finish hook validation for the frontend call graph
*~3 weeks*

Goal: every RVA reached by the frontend boot+menu chain is C3+ with
diff-original GREEN under the dinput8 loader.

- **A1.** Build per-cluster `arg_type` scaffolds (TimerSlot, FontCtx,
  sprite_ptr_table, AudioSubStruct). Recovers ~20 currently-refused hooks.
- **A2.** Decomp the 67 untouched-since-mass-disable hooks, run
  diff-original on safe ones, scaffold the rest. ~30–40 more.
- **A3.** Fix the ~6 real-RED reimpls (HudSpinCoinAnim, audio_list_drain2,
  hw_exit_dispatch) by re-reading decomp.
- **A4.** Re-instate the 6 boot-crashers carefully (CrtPreInit /
  CrtExitCore / WinMainEntry / CarSelectReset / data_zero_fill /
  engine_stop_dispatch) — these need scaffolded-call tests rather than
  install-and-pray.
- **Gate:** all RVAs in `re/analysis/skeleton_call_tree.md` for the
  frontend subgraph are C3+.

### Phase B — External-dependency audit
*~1 week*

Goal: enumerate every symbol the frontend code calls that's NOT in our
reimpl set.

- **B1.** `dumpbin /imports MASHED.exe` + intersect with the .asi's call
  graph → list of all DLL imports MASHED uses (DirectShow, RW3
  internals, DirectInput, MSVBVM60, etc.).
- **B2.** Decomp every "external" `sub_xxx` in MASHED that the frontend
  hooks call → classify as RW-runtime / DirectShow / OS / MASHED-internal.
- **B3.** For each: pick `proxy` (link Microsoft DLL), `stub` (return 0 /
  no-op), or `reimplement` (write our own). Most RW3 calls will be
  `reimplement`.
- **Gate:** `re/STANDALONE_DEPS.md` written with one line per external
  symbol + chosen disposition.

### Phase C — Make standalone exe link without `mashed_re_dev.asi`
*~2 weeks*

Goal: `mashed_re.exe` compiles and links with the full frontend `.cpp`
set, no unresolved externals.

- **C1.** Extend `mashedmod/build.bat` exe target to include the same
  `.cpp` set the `.asi` does (minus `Core/HookSystem.cpp` and
  `dll_main.cpp` which are dev-only).
- **C2.** Provide stubs in `mashedmod/src/mashed_re/Stubs/` for every
  external Phase B identified — start them all as `return 0;` no-ops.
- **C3.** Link. Fix unresolved-symbol errors one by one; each fix is
  either "wire to real DLL" or "add to stubs".
- **Gate:** `mashed_re.exe` builds cleanly with full frontend source set.

### Phase D — `WinMain` + window + D3D9 device
*~1 week*

Goal: launching `mashed_re.exe` opens a window and clears it to a color.
No menu yet.

- **D1.** Write our own `WinMain` in `exe_main.cpp` — adapt RVA 0x004a4bb7
  reimpl logic (`WinMainEntry`) but standalone-safe (no SEH frame magic).
- **D2.** Direct `Direct3DCreate9` + `CreateDevice` (forced windowed
  800×600 — reuse the d3d9 shim logic).
- **D3.** Per-frame clear + `Present`. Message pump.
- **Gate:** `mashed_re.exe` runs, shows an 800×600 blue window. Quits via
  close button.

### Phase E — Asset path
*~2 weeks*

Goal: standalone exe loads `original/TOASTART/COMMON/FONT36.PIZ` and can
read entries.

- **E1.** Port `re/tools/piz_extract.py` → C++ as
  `mashedmod/src/mashed_re/Piz/PizReader.cpp` (already started — verify
  it handles real files).
- **E2.** RWS chunk walker C++ port (already started; verify).
- **E3.** TXD decoder C++ port (already started; verify).
- **E4.** Wire `FsOpen.cpp` / `VfsStream.cpp` reimpls to actually read
  from `original/` filesystem (not MASHED's running state).
- **Gate:** unit-test C++ piz reader on every
  `original/TOASTART/**/*.piz`; bit-identical to Python output.

### Phase F — RenderWare-replacement layer
*~6 weeks (biggest unknown)*

Goal: the RW3 functions the frontend calls have working implementations
(real-renderer or stub-that-doesn't-crash).

- **F1.** Categorize RW3 calls into: engine-init (RwEngineInit,
  RwEngineOpen, RwEngineStart), per-frame (RwCameraBegin/End,
  RwIm2DRenderPrimitive), asset (RwStreamRead, RtCharsetOpen, etc.), and
  state (RwSetRenderState).
- **F2.** Pick approach: (a) **stub renderer** that just clears +
  immediate-mode rectangles + textured quads — enough for menu; or
  (b) **full RW-replacement** layered on D3D11 like TD5RE. Recommendation:
  start with (a); upgrade later.
- **F3.** Implement chosen approach in `mashedmod/src/mashed_re/RwReplace/`.
- **F4.** Test each frontend sprite/font draw against a screenshot of the
  same operation in MASHED via .asi diff-original.
- **Gate:** standalone exe can draw a textured 2D sprite at correct UVs.

### Phase G — Frontend boot chain in standalone
*~2 weeks*

Goal: `mashed_re.exe` boots through the same call chain MASHED's
frontend takes.

- **G1.** Call the C2/C3 reimpls in `Boot/*.cpp` in the same order
  MASHED's `WinMainEntry` does — load videocfg, init audio (or stub),
  init input, load FONT36.PIZ, init frontend state.
- **G2.** Detect divergence by comparing standalone's mashed.log-equivalent
  against MASHED's actual mashed.log line-by-line.
- **G3.** Skip intro player entirely (no DirectShow in standalone — go
  straight to menu init).
- **Gate:** standalone reaches `Frontend/MenuInit` without crashing.

### Phase H — Menu navigation in standalone
*~4 weeks*

Goal: arrow keys move the cursor on screen, Enter selects, Esc backs out.

- **H1.** Per-frame call `frontend_mode_dispatch`. Provide it with the
  standalone equivalent of MASHED's frontend state globals.
- **H2.** Wire DirectInput keyboard polling → the input globals frontend
  reads.
- **H3.** Render the sprite/font draw lists the frontend hooks emit,
  through the Phase F replacement layer.
- **H4.** Visual diff: side-by-side MASHED main menu vs. standalone main
  menu. Pixel-equality is the bar.
- **Gate:** **standalone shows MASHED's main menu and lets you navigate
  to "Start Race".**

## Milestones

| milestone | name | est weeks | dep |
|---|---|---|---|
| M1 | All frontend hooks C3+ via diff-original | 3 | — |
| M2 | External-dep audit written | 1 | M1 |
| M3 | Standalone exe links | 2 | M2 |
| M4 | Standalone shows blue window | 1 | M3 |
| M5 | Standalone reads PIZ assets in C++ | 2 | M3 |
| M6 | RW replacement layer (stub-tier) renders 1 textured sprite | 6 | M4, M5 |
| M7 | Standalone boot chain reaches MenuInit | 2 | M6 |
| M8 | Standalone main menu displays & navigates | 4 | M7 |
| **total** | | **~21 weeks** (calendar) | |

## Risks + decisions to flag

- **Phase F is dominant risk.** If RW3 turns out to need 100+ functions
  implemented before frontend renders correctly, F balloons. Mitigation:
  ruthlessly stub everything except `RwIm2DRenderPrimitive` + texture
  upload + a 2D camera. Defer 3D until Phase 6+.
- **Phase A could shortcut.** If "frontend visible" is OK before
  "frontend bit-identical", we can skip A1–A4 and go straight to C–H,
  accepting weird-looking standalone until A is done.
- **C4 promotions** aren't strictly needed. C3 is enough for standalone
  to behave like MASHED. C4 is a separate gate.
- **Wave 3** is sideways. Its 41 hooks were authored for race state —
  Phase 6+ (track loading), not frontend. Stay on origin until then.
- **DirectShow** isn't needed in standalone — replace
  `HardwareShowIntroVideo` with an immediate `return;`. Frontend loads
  in <1 s.
- **Audio** can be silent-stubbed through the entire frontend path.
  Saves us a whole subsystem.

## Smallest viable demo

If "frontend running" can be relaxed to **"standalone exe shows the
MASHED title screen, no input"**:

M1 → M2 → M3 → M4 → minimal-M6 (clear + draw one textured quad) →
minimal-M7 (load LOGO sprite + display) → **~6 weeks** instead of 21.

That's the natural first deliverable.

## Cross-references

- `ROADMAP.md` — project-wide phases (we are mid-Phase 5).
- `re/CONFIDENCE.md` — C0..C4 rubric.
- `re/INJECTION.md` — why the hook .asi exists during dev.
- `re/analysis/subsystem_map.md` — call graph by subsystem.
- `re/analysis/skeleton_call_tree.md` — boot chain.
- Memory: `[[project-loader-split-dinput8]]`, `[[project-loader-broken-9d-audit]]`.
