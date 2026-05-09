# Mashed RE Roadmap

High-level plan to take Mashed from "imported binary" to "fully reimplemented executable." Phases are gates: do not advance until the **Definition of Done (DoD)** for the current phase is satisfied. Each phase has measurable exit criteria — fuzzy criteria are not allowed (a phase is *done* or it is not).

## Definition of Done — three scopes

The project enforces three nested DoDs. Every reverse-engineered function must satisfy F-DoD; every subsystem rolls up to S-DoD; the project ships when P-DoD is satisfied.

### F-DoD — function level
A function is **DONE** only when ALL of these hold:
1. **RVA pinned** in `hooks.csv` against the version anchor SHA-256.
2. **Confidence ≥ C3** per the rubric (`re/CONFIDENCE.md`). C4 means "verified by Frida diff."
3. **No `[UNCERTAIN]` markers** remain in `re/analysis/<subsystem>/<function>.md`. Outstanding uncertainties either:
   - Resolved with cited evidence, or
   - Promoted to `UNCERTAINTIES.md` with an owner and a path-to-resolution.
4. **No stubs** in the implementation. Every callee resolves to a real reversed function or an explicitly-allowed passthrough hook to the original (recorded in `STUBS.md`).
5. **Frida diff clean** (the `diff-original` skill produced an identical CSV between original and modded run for at least one canonical scenario).
6. **Hook registered** via `RH_ScopedInstall(Method, 0xRVA)` and **runtime-toggleable**.
7. **Inline RVA comments** on every cross-reference (`// 0x00xxxxxx`).

### S-DoD — subsystem level (e.g. AI, Audio, Vehicle, HUD, Frontend)
A subsystem is **DONE** when:
1. Every function reachable from the subsystem entry points is at C3 or higher (≥ 90% at C4).
2. The subsystem can run **with all original-fallback hooks disabled** for one full canonical scenario without crash, hang, or visible regression.
3. All shared structs touched by the subsystem are documented in `re/analysis/structs/` with **field-by-field** RVA citations for first read/write of each field.
4. Any custom asset format (e.g., `.piz`, `gamesave.bin` for the save subsystem) has a round-trip parser+writer in `re/tools/` with a byte-identical re-encode test.
5. The subsystem section of `STUBS.md` is empty.

### P-DoD — project level
The project is **DONE** (v1.0) when:
1. Every subsystem is S-DONE.
2. A clean playthrough of every track + every vehicle + every game mode runs with the modded DLL alone (no original fallbacks).
3. `MASHED.exe` is reduced to a thin shim or the build produces a standalone `mashed_re.exe` (TD5RE-style source port).
4. All four trackers (`hooks.csv`, `STUBS.md`, `UNCERTAINTIES.md`, `DEFERRED.md`) have either zero rows or only rows tagged `wontfix` with rationale.

## Phases

### Phase 0 — Bootstrap (DONE on 2026-05-02)
**Goal:** infrastructure exists, tools wired, prior art archived.
**Exit criteria:**
- ✅ `original/` immutable; binary anchors pinned (SHA-256 + size).
- ✅ Ghidra master `Mashed.gpr` analyzed and saved.
- ✅ MCP wired (`.mcp.json`); pool script + skills functional.
- ✅ SciLor repos cloned read-only into `re/prior_art/`.
- ✅ `piz_extract.py` smoke-tested.
- ✅ CLAUDE.md, AGENTS.md, ROADMAP.md, four trackers seeded.

### Phase 1 — Surface mapping (target: 1–2 weeks)
**Goal:** know what every named function in the binary roughly does. No reimplementation yet.
**Activities:**
- Auto-name imports/exports; identify `WinMain`, `RwEngineOpen`, RW APIs, MSVCRT calls, DirectX entry points.
- Sweep all top-level functions; assign C0/C1 in `hooks.csv` based on call-graph and string xrefs.
- Identify subsystem boundaries by string clustering (audio/AI/render/UI/network/save).
- Produce a one-page subsystem map in `re/analysis/subsystem_map.md` with RVAs of each entry point.
**Exit criteria:**
- Every function in the binary has a row in `hooks.csv` (status `mapped` minimum) — total count from Ghidra `function_list`.
- ≥ 80% of those rows have a confidence ≥ C1 (purpose hinted by name/strings/xrefs).
- The subsystem map names each major area and lists ≤ 5 entry-point RVAs per area.
- A "skeleton call tree" markdown for `WinMain → main loop → frame tick` exists.

### Phase 2 — Asset formats (target: 1 week, parallelizable with Phase 1)
**Goal:** every binary asset format Mashed touches has a Python parser.
**Activities:**
- `.piz` — verify offset semantics on every file, not just `AI.piz`. Implement repacker with byte-identical round-trip.
- `.rws` — confirm Mashed's flavor matches RenderWare 3.7 standard or document the deviation.
- `pc.txd` — TXD reader producing PNGs.
- `gamesave.bin` — DEADBEEF struct layout.
- `videocfg.bin` — 512-byte struct layout.
- `.AI` files inside `AI.piz` — internal AI script format (cross-ref MashedFileExtractor for hints).
**Exit criteria:**
- Each format has a tool under `re/tools/` with `list` and `extract` (or `parse`) modes.
- Each tool has at least one round-trip test on a real archive.
- `re/analysis/formats/<name>.md` documents the layout with byte-offset table.

### Phase 3 — Engine boot path (target: 2–3 weeks)
**Goal:** reimplement enough of the boot flow that the modded DLL gets to a known-good state inside `WinMain`. We hook nothing yet, just observe.
**Activities:**
- Reverse `WinMain`, command-line parsing (cross-ref MashedRunner), language selection, video config load (`videocfg.bin`), Renderware engine init.
- Identify the global game-state struct(s) and document their fields.
- Frida-trace the full boot sequence on the original binary; produce a "ground truth CSV" for diffing.
**Exit criteria:**
- `WinMain` is C3 or higher in `hooks.csv`.
- Every global accessed during boot has a documented type.
- Frida boot trace runs and is checked into `verify/boot_trace_baseline.csv`.

### Phase 4 — Dev harness + standalone skeleton (infrastructure done 2026-05-09; verification blocked-on-runtime)
**Goal:** stand up *both* build targets (greenfield exe + dev-mode hook DLL) and prove the verification loop on one small function.
**Activities:**
- ✅ MSVC Build Tools 2022 (x86); `mashedmod/build.bat` calls `vcvars32.bat`. (2026-05-08)
- ✅ Scaffold `mashed_re.exe` standalone: `WinMain`, empty event loop, exits cleanly. (2026-05-08)
- ✅ Scaffold `mashed_re_dev.asi` with `DllMain` + `InjectHooks()` bootstrap. (2026-05-08)
- ✅ Hand-rolled inline-JMP hook system (`Core/HookSystem.{h,cpp}`) with `RH_ScopedInstall(Method, RVA)` macro and runtime-toggleable registry. (2026-05-08)
- ✅ Pick one leaf function: `0x004c3ac0` (Vec3Magnitude, 3-vec sqrt LUT). Reimpl at `Math/Vec3.cpp` registered via `RH_ScopedInstall`. (2026-05-08)
- ✅ Ultimate-ASI-Loader v latest deployed as `original\d3d9.dll`. (2026-05-08)
- 🚫 Live verification blocked: MASHED.exe crashes mid-asset-load (RW3 TXD/piz close handler) on this Win11 + RTX 5070 Ti. Crash predates any of our code (reproduced with no ASI loader, no .asi, with dgvoodoo2 wrapper). See `memory/project_runtime_blocked.md`.

**Exit criteria:**
- ✅ `mashed_re.exe` builds and exits 0. (verified 2026-05-08: x86, Subsystem Windows GUI, exit code 0)
- ✅ `mashed_re_dev.asi` builds with `InjectHooks` exported. (verified 2026-05-08: 88,576 bytes, x86 DLL)
- 🚫 One function C4 in `hooks.csv` with green Frida diff — **blocked on runtime**.
- 🚫 `hook-author` + `diff-original` skills exercised end-to-end — **blocked on runtime**.

**Path forward when unblocked:** restore `original\d3d9.dll` (currently `d3d9_loader_disabled`) and `original\mashed_re_dev.asi` (currently `.bak`), launch game, run `diff-original` skill against `Vec3Magnitude` over a canonical scenario, promote `0x004c3ac0` to C4 via `re-classify`.

### Phase 5 — Subsystem sweeps (target: 4–8 weeks each, parallelizable)
**Goal:** progressively reverse each subsystem and land its functions into the **standalone exe**, using the dev harness for verification.
**Order (suggested, by isolation):**
1. **Frontend / HUD** — most isolated, lowest blast radius.
2. **Save / Load** — pure data, no rendering.
3. **Audio** — RWS-driven, mostly leaf calls.
4. **Input** — Lua remap is already exposed.
5. **Vehicle / Physics** — core gameplay; depends on Save and Input.
6. **AI** — depends on Vehicle, Track.
7. **Track / World** — depends on Asset Phase 2.
8. **Render** — touches everything; do last.
**Per subsystem:** functions land in `mashed_re/<Subsystem>/`, hooks register them in the dev target only, the standalone exe links them directly. Hook-mode is gradually removed for any subsystem the standalone can run unaided.
**Exit criteria per subsystem:** S-DoD as defined above; the standalone exe runs the canonical scenario for that subsystem with no hooks active.

### Phase 6 — Standalone v1 (target: when last subsystem is S-DONE)
**Goal:** the standalone exe runs every track / vehicle / mode without `MASHED.exe`.
**Activities:**
- Decide renderer endgame: keep RenderWare 3 (if a build is licensable / available), or replace with a D3D11 backend like TD5RE's `ddraw_wrapper`.
- Strip the dev harness from the build matrix.
- Final pass on `STUBS.md` / `UNCERTAINTIES.md` — every row resolved or `wontfix`.
**Exit criteria:** P-DoD.

## Per-phase rituals

- **Start of phase:** `git worktree add` a new branch via the `worktree` skill. Update `ROADMAP.md` to mark phase `in-progress`.
- **End of session:** every newly-touched function gets its row in `hooks.csv` updated; uncertainties promoted to `UNCERTAINTIES.md`; stubs to `STUBS.md`.
- **End of phase:** all four trackers reviewed; any row not advanceable in this phase moves to `DEFERRED.md` with a phase tag for re-pickup.
- **Cross-phase:** the `re-classify` skill is invoked on every function as it's touched; the rubric is the only thing that updates a hook's status.

## Sizing reality check

`MASHED.exe` has **5,800 functions** identified after auto-analysis (full inventory at `re/analysis/function_inventory.csv`). At a sustainable 5–15 functions to C4 per week, full C4 coverage is multi-year. **C4 is not the goal for every function** — the goal is **S-DoD** per subsystem, which typically needs 20–40% of a subsystem's functions at C4 (the public/cross-subsystem ones), with the rest at C2/C3 (correct-by-construction leaf utilities).

Realistic horizon for a P-DoD shipping `mashed_re.exe`: 12–24 months solo, depending on how much of the renderer is rebuilt (RenderWare passthrough vs D3D11 replacement). Don't promise dates — promise phase exits.
