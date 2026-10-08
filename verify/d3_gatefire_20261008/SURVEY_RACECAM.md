# SURVEY: does the standalone carry clear site (a)? (D-11073 open read)

Date 2026-10-08. Account2 worker (claude-opus-5, read-only), prompt below. Orchestrator spot-checked: RaceCamera.h:106-107 collapse, zero hits for 00442600/004481c9 in mashedmod/src, .rsp membership (CameraEntryDispatch, L2_00442a20 asi-only; RaceCamera, UtilLeaves_ab6, VehicleSeed in exe_sources.rsp), no 0x00445aa0 body anywhere. All confirmed.

## Prompt

    Mashed RE, read-only source survey. NO-GUESSING: cite file:line for every claim; if absent, say "not found" and list what you searched.
    
    Context: in the original MASHED.exe, FUN_00446520 (camera director, 7,412 B) is called with param_1 = &DAT_00897fe0 (a struct; [0] is the D-11073 phase-3 exit flag). When sub-mode != 6 (call site 0x004468e2..0x004468f9) it calls FUN_004464c0(&DAT_00897fe0), which dispatches 7 camera entries (base 0x008964c0, stride 0xd8, count DAT_00898994 written by FUN_00442600) to FUN_00445aa0 (type 0), FUN_00441d40 (type 1), FUN_00442440 (type 2). At 0x004481c9..0x004481e2 FUN_00446520 copies struct+0x40..+0x48 into struct+4..+0xc (param_1[1..3] = param_1[0x10..0x12]) then param_1[4..6] = locals, and calls FUN_00442a20 if *param_1 != 0. FUN_00445aa0's flag-set branch clears the flag when an entry's horizontal distance to struct[1]/[3] < 0.02f.
    
    Questions, answer each separately:
    Q1. In mashedmod/src/mashed_re/Race/RaceCamera.cpp (+ .h): which parts of FUN_00446520 are ported? Specifically: is the sub-mode != 6 path ported, and does the port contain the struct+0x40 -> struct+4..+0xc copy (0x004481c9) and the param_1[4..6] write? Cite lines and the RVA comments.
    Q2. Is there any standalone (non-hook) port of FUN_004464c0, FUN_00445aa0, FUN_00441d40, FUN_00442440, FUN_00442600, FUN_00442a20, FUN_00448700? Search all of mashedmod/src (grep both "00445aa0" and "0x445aa0" style spellings). For each: file:line, whether it is standalone body or a hook calling the original address (e.g. reinterpret_cast<...>(0x00445aa0u)), and whether it is compiled into mashed_re.exe (check mashedmod/*.rsp or build file lists for membership; hooks.csv exe_file is NOT ground truth).
    Q3. Does the port model the camera-entry array (0x008964c0 / DAT_00898994 / per-entry fields +0x3c,+0x44,+0x8c..+0xc8,+0xa8) anywhere? Cite.
    Q4. Where does the port read/represent DAT_00897fe0 (TrackRenderer.cpp ~6124-6352 mentions it)? Is there a flag field equivalent at offset 0?
    Q5. One-paragraph verdict: what of {target copy, entry setup FUN_00442600, entry dispatch, FUN_00445aa0 flag-set branch} already exists in the standalone build, and what would have to be ported for clear site (a) to work standalone. Mark anything not settled as [UNCERTAIN] with what evidence is missing.
    Keep the answer under ~1,200 words.

## Worker answer


**Q1 — What of FUN_00446520 is in `Race/RaceCamera.{h,cpp}`**

Ported: only the *race branch body* — the pose/zoom math from ~`0x00446b30` to `0x00447f50`. `RaceCamera.cpp:238` is the section header `// ---- 0x00446520 race branch ----`, and `RaceCamera.h:4` declares the same: `0x00446520 FUN_00446520(cam, force_reset) race branch (camera director)`. `RaceCamera.h:57-63` defines `Update(cars, track_type, dt_blend, time_ticks, jitter_amp, force_reset, overhead)` — it takes a `RaceCamCar cars[4]` adapter array, **not** `param_1 = &DAT_00897fe0`.

- **Sub-mode != 6 path: not ported.** No sub-mode/mode-6 test exists anywhere in the file. The only mode text is `RaceCamera.cpp:245` ("mode-7/4/8/9 overrides not ported — standard path only, matching live mode 0"), `RaceCamera.cpp:514` ("mode-1 … not ported"), and `RaceCamera.h:21-22` ("Modes 4/5/7/8/9/10 … are NOT ported"). The call to `FUN_004464c0` at `0x004468e2..0x004468f9` has **no counterpart** — searched `RaceCamera.cpp`/`.h` and all of `mashedmod/src` for `004464c0`/`0x4464c0`; the only hits are `Util/CameraEntryDispatch.cpp` and `Vehicle/VehicleSeed.cpp` (Q2).
- **struct+0x40 → struct+4..+0xc copy (`0x004481c9`): not found.** Searched `mashedmod/src` for `004481c9`/`4481c9` — zero hits. The port has no separate `[1..3]`/`[4..6]` fields: `RaceCamera.h:106-107` declares `pos_out_[3] // +0x40 (and [1..3])` and `tgt_out_[3] // +0x4c (and [4..6])`, i.e. the two destinations are *collapsed into one pair of members*, and `RaceCamera.cpp:484-489` writes both in a single loop (`pos_out_[i] = cam_pos[i]; … tgt_out_[i] = tgt[i];`). So there is no copy, and no `param_1[4..6]` write distinct from `+0x4c`.
- **`FUN_00442a20` call gated on `*param_1 != 0`: not ported.** No `00442a20` reference in `RaceCamera.*`; nothing in the file reads a flag at struct offset 0.
- `RaceCamera.cpp:490-495` explicitly notes the tail (`cam[0xd]/[0xe]` Eulers + `Camera::Apply 0x00441760`) is deferred to a host LookAt.

**Q2 — Standalone vs hook bodies, and `.rsp` membership**

Searched `mashedmod/src` for both spellings of each RVA. Ground truth for membership = `mashedmod/exe_sources.rsp` (exe) vs `mashedmod/asi_sources.rsp` (dev .asi).

| RVA | Where | Kind | In `exe_sources.rsp`? |
|---|---|---|---|
| `FUN_004464c0` | `Util/CameraEntryDispatch.cpp:28-89` | **Standalone body** (reads absolute `0x00898994` at :35, base `0x008964c0` at :42), but its three dispatch arms are **calls to the original**: `reinterpret_cast<tEntryFn>(0x00445aa0u)` :55, `0x00441d40u` :64, `0x00442440u` :73. `RH_ScopedInstall(…, 0x004464c0)` :89 | **No** — `asi_sources.rsp:306` only |
| `FUN_00445aa0` | `Util/CameraEntryDispatch.cpp:55` | call to original address only. (`Util/PromoLoop_round8.cpp:41` is a comment.) | No body anywhere |
| `FUN_00441d40` | `Util/CameraEntryDispatch.cpp:64` | call to original address only | No body anywhere |
| `FUN_00442440` | `Util/UtilMid.cpp:504-635` **standalone body** (`TransformMatrixUpdate`), install **commented out** at :635 | **No** — `asi_sources.rsp:110` only |
| `FUN_00442600` | **not found** — zero hits for `00442600`/`0x442600` in all of `mashedmod/src` | — | — |
| `FUN_00442a20` | `Lane2/L2_00442a20.cpp:30-59` — generated standalone body, shadow-A/B wrapped, `RH_ScopedInstall(L2_FUN_00442a20, 0x00442a20)` :59 | **No** — `asi_sources.rsp:394` only | |
| `FUN_00448700` | `Vehicle/VehicleSeed.cpp:50-69` — **naked asm standalone body**, but its inner `call dword ptr [s_call_004464c0]` (:57) targets original `0x004464c0` (:26). Also `Race/ScoringHooks.cpp:99-100` declares a thunk `reinterpret_cast<…>(0x00448700)` used at :603/:608 | **`VehicleSeed.cpp` YES — `exe_sources.rsp:168`** (also `asi_sources.rsp:16`). `ScoringHooks.cpp` is `asi_sources.rsp:176` only | |

Note the near-miss: `FUN_00442a**6**0` (not `a20`) *does* have a standalone body in `Race/SpectatorDistances.cpp:148` and **is** in `exe_sources.rsp:164`. `FUN_00442a20` is a different function and is asi-only.

**Q3 — Camera-entry array (`0x008964c0` / `DAT_00898994` / per-entry fields)**

Two places, both absolute-address, **not** a modeled structure:
- `Util/CameraEntryDispatch.cpp:34-77` — count from `0x00898994` (:35, re-read :77), base `0x008964c0` (:42), stride `0xd8` iteration. Header at :8-14 documents it. **asi-only.**
- `Util/UtilLeaves_ab6.cpp:53-61` — `UtilZeroTable8964c0` walks `[0x008964c0, 0x00897fc0)` at stride `0xD8` zeroing elem[0] (:55-57), then `0x0089898c` (:58). This file **is** in `exe_sources.rsp:158`, but the body dereferences original-image absolute addresses and is only registered via `RH_ScopedInstall` (:61).

Per-entry fields `+0x3c`, `+0x44`, `+0x8c..+0xc8`, `+0xa8`: **not found** — no struct definition or named accessors for them anywhere in `mashedmod/src`. Only elem[0] (`UtilLeaves_ab6.cpp:56`) and the type selector read inside `CameraEntryDispatch.cpp:34-77` exist.

**Q4 — `DAT_00897fe0` in the port**

No struct type and **no flag field at offset 0** in the standalone. Occurrences:
- `D3d9Render/TrackRenderer.cpp:6124`, `:6141`, `:6352` — **comments only**, describing the Frida-side producer (`race_draw_burst.py` reads `+0x40` eye / `+0x4c` at) and `+0x84 → +0x6c` viewWindow. `:6141-6145` explicitly says the port does **not** source the camera from that struct (it uses the RwCamera frame basis instead).
- `Race/RaceCamera.h:95` — `// camera struct state (offsets cite 0x00897fe0 fields)`, then members annotated `+0x964…+0x9a0` (:96-103) and `+0x40`/`+0x4c`/`+0x58` (:106-108). These are **C++ members on `RaceCamera`**, not a mapped struct, and there is no member for offset 0.
- Raw-address touches, all **asi-only**: `Util/PromoLoop_round8.cpp:49` (write dword `0x00897fe0`, `0x004430a0`) and `:72` (read, `0x004430b0`) — these *are* the offset-0 dword, but neither file is in `exe_sources.rsp` (`asi_sources.rsp:236`). `Vehicle/VehicleSeed.cpp:20,57` passes `0x00897fe0` as the dispatch arg. `qol_asi/mashed_qol.cpp:344,359` is a separate QoL .asi target.

**Q5 — Verdict**

In the standalone `mashed_re.exe` build, of the four pieces you named: **the target copy does not exist** (no `0x004481c9` transcription; `pos_out_`/`tgt_out_` in `RaceCamera.h:106-107` collapse `+0x40`/`[1..3]` and `+0x4c`/`[4..6]` into one pair each, written together at `RaceCamera.cpp:484-489`); **entry setup `FUN_00442600` does not exist at all** (zero hits in `mashedmod/src`); **entry dispatch exists only as `Util/CameraEntryDispatch.cpp`, which is asi-only (`asi_sources.rsp:306`) and in any case trampolines to the original `0x00445aa0`/`0x00441d40`/`0x00442440` addresses** (`:55/:64/:73`); and **`FUN_00445aa0`'s flag-set branch has no port anywhere** — no file contains a body for `0x00445aa0`, and no code reads or clears a flag at `DAT_00897fe0[0]` in the exe build. What the exe *does* have is the pose math (`RaceCamera.cpp:238-496`, `exe_sources.rsp:75`), driven from an adapter array at `TrackRenderer.cpp:5530-5557`, plus two absolute-address stragglers compiled in but original-image-dependent (`UtilLeaves_ab6.cpp` / `VehicleSeed.cpp`, `exe_sources.rsp:158/168`). For clear site (a) to work standalone you would have to port, in order: `FUN_00442600` (entry population + `DAT_00898994` count), a real camera-entry record (stride `0xd8`, fields `+0x3c/+0x44/+0x8c..+0xc8/+0xa8`) to replace the absolute-address walk, `FUN_00445aa0` including its `< 0.02f` horizontal-distance flag-clear branch, the `0x004481c9` target copy so `struct[1]/[3]` carries the value `FUN_00445aa0` compares against, the sub-mode `!= 6` gate with its `FUN_004464c0` call, and a real offset-0 flag field on `RaceCamera`. **[UNCERTAIN]**: whether `UtilLeaves_ab6.cpp:54` / `VehicleSeed.cpp:50` are actually *reachable* in the exe (they are only reachable through `RH_ScopedInstall`, `HookSystem.h:37-43`, which registers into a patcher registry) — I did not find a standalone caller, and resolving it needs a build/link or runtime check I cannot run read-only. **[UNCERTAIN]**: whether `0x00441d40`/`0x00442440` types ever occur in the entry array during a race — settling that needs a live count of `DAT_00898994` per entry type, which is Frida work.
