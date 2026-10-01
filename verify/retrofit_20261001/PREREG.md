# PRE-REGISTRATION — retrofit two exe-only fixes into real ports at their RVAs

Written **before any edit under `mashedmod/src`** for this effort. Base HEAD
`e8140023`, branch `race/first-frame-parity`.

This is the first use of the standing workflow "a fix that is a real port must
also produce promotion evidence". Two fixes landed as exe-only code; each is
retrofitted into a single shared body at its RVA, listed in BOTH
`mashedmod/exe_sources.rsp` and `mashedmod/asi_sources.rsp`, with the exe-only
copy replaced by a call into it.

---

## 0. Version anchor

```
original\MASHED.exe.unpatched   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
```
Re-verified at the start of this effort (see RESULT.md). All RVAs below were
read from that file (capstone) and from a **read-only** Ghidra pool clone
(`Mashed_pool0`, `analyzeHeadless -readOnly`; the Ghidra MCP is not wired into
this session — `ToolSearch` finds no `mcp__ghidra__*`). The master project
`Mashed.gpr` is never opened.

---

## 1. Target A — `0x00458e00` (pickup-pool spawn)

### Level before
`hooks.csv:3625` — `00458e00,FUN_00458e00,gameplay,C2,mapped`,
plate `re/analysis/bucket_gameplay_00458a40_0045ac40/0x00458e00.md`.

### What exists today (the thing being retrofitted)
`c9615225` re-implemented this function's **normal-race arm** inline inside
`PickupField::InitReal` (`mashedmod/src/mashed_re/D3d9Render/PickupField.cpp:192`),
over `std::vector<Orb>` instead of the original's pool, and in the exe build
only (`PickupField.cpp` is in `exe_sources.rsp` only). So the RVA has no body.

### Rubric path and the evidence each step needs (`re/CONFIDENCE.md`)
C2 → C3 requires, and this effort must produce, ALL of:

| # | requirement (CONFIDENCE.md L23, L32-36) | how it is met |
|---|---|---|
| A1 | a real name | `PickupPoolSpawn` |
| A2 | every field named, or an `[UNCERTAIN]` recorded in `UNCERTAINTIES.md` | entry fields `+0x18 +0x1c +0x20 +0x24 +0x28 +0x2c +0x38`; the stubbed callees and the un-exercised rank-2 arm get `[UNCERTAIN]` rows |
| A3 | one caller at C2+ | `FUN_00405730` C2 (`hooks.csv`), also `FUN_00458fd0` C2 |
| A4 | one callee at C2+ | `FUN_0042fe30` `RaceEndFlagIfEndMode` **C4**; `FUN_00458dd0` C2, `FUN_00458d00` C2 |
| A5 | purpose in plain prose with citations at the top of the plate | plate updated |
| A6 | a reimplementation in `mashedmod/src/mashed_re/` | `Gameplay/PickupPoolSpawn.cpp`, in BOTH `.rsp` lists |
| A7 | hooked through `RH_ScopedInstall`, runtime-toggleable | `RH_ScopedInstall(PickupPoolSpawn, 0x00458e00)` |
| A8 | clean build | `mashedmod\build.bat` from PowerShell |

Project practice adds (and this effort will run):

| # | requirement | how |
|---|---|---|
| A9 | path1 bit-identity A/B vs the original | `py -3.12 re\frida\run_diff.py pickup_pool_spawn` |
| A10 | path2 inline-JMP install proof | `py -3.12 re\frida\run_verify_hook.py pickup_pool_spawn` |
| A11 | a zero-hook control run FIRST, so the verifier has a passing baseline | memory `verifier-needs-passing-baseline` |
| A12 | vectors reach the BODY, not an early-out guard | at least one vector must take the accept path and write an entry; non-degenerate fingerprints across vectors |

### C4 is NOT claimed
C4 needs a canonical-scenario run with the `.asi` hook live inside `MASHED.exe`,
and `no stubs in the implementation` (CONFIDENCE.md L37). The standalone arm of
this port stubs `FUN_00458dd0` / `FUN_004c15c0`, so C4 is out by construction here.

### Declared PRE-REGISTERED blockers (promotion stops here if hit)
- **A-B1** If `FUN_0042fe30` cannot be made to read a stable rank in the A/B
  scenario, the rank-2 arm is not covered. That arm calls `FUN_00458d00`, which
  is a **random** selector (`FUN_00472690(0,8)` at `0x00458d47`), so it can never
  be part of a bit-identity A/B. It will be recorded `[UNCERTAIN]`, NOT claimed.
- **A-B2** If path1 is degenerate (every vector produces the same fingerprint),
  that is a FALSE GREEN and the promotion stops at C2.

### In-game no-regression acceptance (re-run with the ported body)
- **A-P1** `verify/pickups_fix_20261001` P1 reproduced exactly: TRAINING 5/5 and
  ARCTIC 7/7, **bit-identical float32** positions and types in pool order, against
  the already-captured original reads
  (`verify/pickups_20260929/orig_qb.bmp.pickuprecs.json`,
  `verify/pickups_fix_20261001/orig_arctic_ctrl.bmp.pickuprecs.json`).
  Checker: `verify/pickups_fix_20261001/pu_placement_check.py`.
- **A-P2** filter parity on all 13 tracks (same checker, keep set AND drop set).
- **A-P3** 0 differing pixels vs the pre-retrofit HEAD `e8140023` build at the
  same deterministic pose, two boots. Regions, where any are used, are built from
  **projected geometry**, never from a colour class
  (memory `achromatic-selector-breaks-on-tinted-light`,
  `control-box-outside-the-defect-needs-proving`).

---

## 2. Target B — `0x00448940` (the "arctic" per-track node, slot 0)

### Level before
**C0 — there is no `hooks.csv` row for `0x00448940`.** It must climb C0 → C1 →
C2 → C3 honestly, each step through the `re-classify` skill.

### What exists today (the thing being retrofitted)
`a0f3004c` put a 5x5 loop inline in `TrackRenderer.cpp:1792-1801`, gated on
`course_id_ == 0 && c.idx == 2`. Exe-only; the RVA has no body.

### Facts already established and re-verified from the binary for this effort
Read from `original\MASHED.exe.unpatched`:

- per-track node table `0x005f33f8`, stride `0x48`,
  `{char name[0x10]; u32 Course_Id; void* slot[13]}`.
- record 2 = `"arctic"` at `0x005f3488`, `Course_Id` = 0 at `0x005f3498`,
  **slot 0 at `0x005f349c` = `0x00448940`**.
- `0x0041e8b0` = `8b0d e4d76300` `mov ecx,[0x0063d7e4]` / `ff61 14`
  `jmp dword ptr [ecx+0x14]` — i.e. `TrackNodeDispatch14` dispatches **slot 0**.
- `_DAT_005cc728` = `00007042` = **60.0**; the `-4.1` immediate is `0xc0833333`.
- `0x00448940` = `83ec24 53 55 56 57 e8 9417 0500` — `sub esp,0x24`, 4 pushes,
  `call 0x0049a0e0`. Confirms the function entry.

### Rubric path
C0 → C1: subsystem + tentative purpose from the table xref and the dispatcher.
C1 → C2: decomp read end-to-end, plate written under `re/analysis/`.
C2 → C3 requires:

| # | requirement | how it is met |
|---|---|---|
| B1 | a real name | `ArcticTrackNodeSlot0` — purely mechanical and fully defensible: the `"arctic"` record of the per-track node table, slot 0. It deliberately claims **nothing** about intent (memory `a-name-may-not-claim-more-than-its-comment`). |
| B2 | fields named or `[UNCERTAIN]` | the course-object offsets `+0x10004 +0x10008 +0x1000c +0x10118 +0x105d4 +0x105f8 +0x105fc +0x1030 +0x2030 +0x3030` and the tile array `0x008963e0..0x00896440` are named where the plate can cite them and `[UNCERTAIN]` otherwise |
| B3 | one caller at C2+ | `0x0041e8b0` `TrackNodeDispatch14` **C3** — and the dispatch is `jmp [ecx+0x14]`, slot 0, verified from the bytes above, not assumed |
| B4 | one callee at C2+ | `FUN_0040bb30` **C3**, `Set6147b4Triple` `0x004924c0` **C3**, plus 16 more at C2 |
| B5 | prose + reimpl + clean build + `RH_ScopedInstall` | `Render/ArcticTrackNodeSlot0.cpp`, in BOTH `.rsp` lists |

### Declared PRE-REGISTERED blockers (promotion stops at the level earned)
- **B-B1 (expected to fire).** A path1 A/B on this RVA calls the function TWICE in
  the live process. The body clones 24 `RpClump`s (`0x00448a47`, 24x
  `FUN_004e6ab0`), re-parents frames (`RwFrameRemoveChild` `0x004e45b0`), pushes
  entries into `0x008962e0` under the global cursor `DAT_0068324c`, and registers
  a particle emitter. It is a once-per-track-load initialiser with unbounded,
  non-idempotent side effects and **no save/restore is possible for an RW
  allocation**. If no safe A/B can be built, path1 is reported **BLOCKED with the
  reason**, and the promotion stops at whatever `re/CONFIDENCE.md` alone supports.
  It is NOT papered over, and a path2-only result is NOT reported as path1.
- **B-B2** `re/CONFIDENCE.md` does not itself require a Frida diff for C3 (that is
  the C4 gate, L37). If B-B1 fires, the C3 decision is made on the rubric text and
  the gap is stated explicitly in `hooks.csv` notes and in RESULT.md.

### In-game no-regression acceptance (re-run with the ported body)
- **B-P1** all 25 sea-tile positions reproduced **exactly**: X,Z in
  {-150,-90,-30,30,90} and Y = -4.1 on all 25, per-instance (a count is not
  enough), from the standalone's own `SEA-TILE[nn]` log lines.
- **B-P2** the negative: `SEA-TILE-OFF` still printed for clump 2 on every
  non-Arctic track, so "did not fire" stays an observed line
  (memory `absent-log-proves-nothing-run-a-control`).
- **B-P3** 0 differing pixels vs the pre-retrofit HEAD `e8140023` build at the
  same deterministic pose, two boots.

---

## 3. Rules binding this effort

- NO-GUESSING. Every constant and offset cites the address it was read from.
  Hypotheses are tested against the running original before being coded.
- One body per RVA. `scripts/lint_rva_bodies.py` must report **NEW = 0**, and a
  manual check for an *unanchored* duplicate is run as well — the lint only sees a
  body whose comment's FIRST token is the RVA (memory `rva-lint-misses-unanchored-bodies`).
- Trackers (`hooks.csv`, `STUBS.md`, `UNCERTAINTIES.md`, `DEFERRED.md`,
  `re/analysis/CHANGELOG.md`) are mutated ONLY through the `re-classify` skill.
- Ghidra: read-only pool slots only; never `Mashed.gpr`.
- Every game launch is muted, carries `MASHED_TITLE`, uses `MASHED_WIN_POS=left-bl`,
  and never uses `MASHED_NAV_DEMO`. PIDs spawned are tracked and only those are killed.
- `unlock_*` patches are never applied to `original/MASHED.exe`. `orig*` captures
  are never pruned.
- Commits are explicit-pathspec, authored `nanofives`, never pushed.
