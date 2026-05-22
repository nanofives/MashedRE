# Phase 6 prep — standalone `mashed_re.exe` to main menu

**Goal:** make `mashed_re.exe` boot from cold start to a working main menu, with no `MASHED.exe` injection. This is the first measurable Phase 6 milestone — "standalone proves the source-port shape."

**Not** the full P-DoD ("every track + every vehicle + every game mode") — that's months further out and gated on `FUN_004b6940` past-menu unblock. This doc is the *menu-only* intermediate goal.

## What we have today (2026-05-22)

- **Save subsystem: first S-DONE** (5/5 criteria, 28 C4 + 29 C3+). 28 reimpls ready to link into `mashed_re.exe` directly.
- **`mashed_re.exe` build target exists** in `mashedmod/build.bat` but currently links only stub asset parsers (`exe_main.cpp`, `Piz/PizReader.cpp`, `Rws/RwsChunkWalker.cpp`, `Txd/TxdDecoder.cpp`, `D3d9Render/QuadRenderer.cpp`). It compiles but doesn't attempt Mashed bootstrap.
- **343 hooks landed in dev DLL** (`mashed_re_dev.asi`) across all subsystems, verified via Frida diff. These need to be lifted from "hook into MASHED.exe" status to "linked directly into mashed_re.exe."

## The skeleton path

`re/analysis/skeleton_call_tree.md` (Phase 1 deliverable) documents the boot-to-frame-tick call path with 84 strategic RVAs:

- Boot path (PE entry → CRT init → WinMain → AppInitialize): ~36 functions
- Main loop: ~10 functions
- Frame tick (state dispatch for frontend menu / world render / HUD): ~38 functions

**Coverage today** (per skeleton § Confidence):
- 23 nodes at C3+ (boot CRT, window pair, frame-dispatch leaves — verified bit-identical via Frida)
- 32 nodes at C2 (WinMain wrapper chain, main loop, frame switch, RW engine init — mechanical Ghidra reads, no runtime diff)
- 17 nodes at C1 (CRT plumbing post-`__heap_init`, world-render sub-passes for state 3/7, state-1/4 frontend tail)
- 1 node `[UNCERTAIN]` (state-4 sub-mode 0x20 — U-0642)
- 13 RVAs cited in skeleton but not in `hooks.csv` (probably compiler stubs / Phase-1 work-in-progress)

**Per-subsystem breakdown of the skeleton's 84 functions:**

| Subsystem | C0 | C1 | C2 | C3 | C4 | total | C3+% |
|---|---:|---:|---:|---:|---:|---:|---:|
| boot | 0 | 0 | 24 | 12 | 0 | 36 | 33.3% |
| frontend | 0 | 0 | 2 | 4 | 1 | 7 | 71.4% |
| hud | 0 | 0 | 0 | 1 | 0 | 1 | 100% |
| render | 0 | 3 | 13 | 5 | 0 | 21 | 23.8% |
| util | 0 | 0 | 4 | 2 | 0 | 6 | 33.3% |

Note: skeleton's frame-tick section is intentionally thin (~38 nodes total). The actual menu state machine, sprite dispatch, font/glyph rendering call many more functions that aren't in the skeleton — those land via the frontend/hud/render subsystem sweeps already in progress.

## Approach

**Step 1 — Promote the skeleton's C2/C1/UNCERTAIN nodes to C3+.** ~60 functions, parallel-fanout-friendly. Estimated **3-4 sessions** at the today-calibrated rate of ~25 C3 promotions per parallel-fanout day = **1-2 weeks of focused work**.

Skeleton-batch composition suggestion (single 6-session parallel fanout, 5-7 candidates each):
1. **boot_winmain_chain** — `0x00492370, 0x00492270, 0x00492290, 0x004924f0, 0x00492e90, 0x004921d0, 0x00428590, 0x00493600` (WinMain wrapper)
2. **boot_main_loop** — `0x00492770, 0x004929d0, 0x00492d20, 0x00492d30, 0x004926c0, 0x00493480` (loop init/tick callees)
3. **boot_appinit_destroy** — `0x00402750, 0x00402a40` + launch-handshake `0x00493540, 0x00493550, 0x00493560, 0x00493900, 0x004938c0`
4. **boot_rw_engine_init** — `0x00493710` (gate) + sub-callees `0x004c2c90, 0x004c2ed0, 0x004c2f00, 0x004c2fb0`
5. **boot_crt_post_init** — `0x004aa3fe, 0x004a8a04, 0x004ac04a` + file-handle plumbing (C1 cluster)
6. **frame_state_sub_passes** — `0x00426030, 0x00426670, 0x004266b0, 0x00410b30, 0x0040de30, 0x0040df20, 0x00404320` (state 3/7 world-render) + `0x00433f40, 0x0043c5b0, 0x00492e60` (state-1/4 frontend tail) + `0x00430b90` (U-0642 [UNCERTAIN] state-4 sub-mode 0x20)

Plus resolve U-0642 in a dedicated Ghidra session.

The 13 unmapped skeleton RVAs need triage — likely add to `hooks.csv` with appropriate subsystem labels first.

**Step 2 — Build mashed_re.exe linkage.**

Update `mashedmod/build.bat` `mashed_re.exe` section to link every `.cpp` reachable from the boot→menu skeleton:
- All `mashedmod/src/mashed_re/Boot/*.cpp`
- All `mashedmod/src/mashed_re/Save/*.cpp` (DONE — already S-DONE)
- The boot/menu-reachable subset of `Frontend/`, `Hud/`, `Render/`, `Input/`, `Util/`

Author `exe_main.cpp` (currently stub) to:
1. Call our reimplemented `WinMain` chain (instead of MASHED.exe's)
2. Hand off to `AppInitialiseOnBootup`
3. Run the main loop until close

**Step 3 — Boot test.**

Run `mashed_re.exe` from cold start. Expected: window opens, splash/intro plays, main menu reachable, navigable via keyboard, idle survives 60+ seconds without crash.

Failures observed during this step feed back into Step 1 (more functions need promotion or fixing).

**Step 4 — Standalone-only attestation.**

Once boot test passes: delete `mashed_re_dev.asi` from the build artifacts directory; re-run `mashed_re.exe`. If it still works, the standalone is no longer dependent on the hook DLL for the menu scenario. This is the Phase 6 sub-milestone.

## What's deliberately out of scope here

- Past-menu work (track/vehicle/AI/physics/audio in race): blocked on `FUN_004b6940` per `project_runtime_blocked`. Separate workstream.
- Full Phase 6 P-DoD (every track/vehicle/mode): months-multi-quarter.
- Full S-DONE for boot/frontend/hud/render/input subsystems: also separate, multi-month. This doc only chases the boot→menu-reachable subset to C3+, not every function in every subsystem.

## First concrete batch

**`skeleton_to_menu_batch.txt`** — to be authored. 6 sessions × 5-7 candidates each. Predicted yield 60-80% (skeleton candidates are mostly well-understood C2s with clean signatures). Should clear 30-40 of the 60 needed in one fanout.

Subsequent batches drain the remainder + the 13 unmapped + U-0642 resolution.

## References

- `re/analysis/skeleton_call_tree.md` — the source-of-truth path
- `re/analysis/subsystem_map.md` — subsystem ownership boundaries
- `ROADMAP.md` § Phase 6 — the formal exit criteria
- `re/CONFIDENCE.md` — C3/C4 rubric
- `memory: project_phase1_closed` — context on why skeleton exists
- `memory: project_runtime_blocked` — past-menu deferred work
