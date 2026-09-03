# Next session — kickoff prompt

Written at the end of the 2026-09-03 **standings camera / crown / promotions** lane
(tip `b03e610c` on `race/first-frame-parity`, tree clean, pushed). Paste the block below.

---

Resume the Mashed in-race UI lane. Branch `race/first-frame-parity` @ `b03e610c`,
tree clean, pushed, no children running, no worktrees or pool slots held.

Read `re/analysis/race_hud_capture_20260902.md` — start with **"State of the standings
port"**, then **Finding 20** (the newest and by far the most load-bearing). Do NOT read
the file top to bottom. Several early Findings are overturned by later Findings in the
SAME file; they are struck in place, but reading them cold will mislead you. The struck
ones are: guard value 7 "never observed", the Im3D arg decode, the `0x004c1be0` frame
anchor, "the car icon is a flat colour swatch", the **a20–a23 "4-piece bar frame"**, the
claim that the **slot index is baked into the DFF**, and the attribution of the layout
residual to **per-child scales**. All four of those last ones were killed by Finding 20.

**Headline, so you do not re-derive it: there is NO driving HUD.** `DAT_0063ba8c == 3`
is mid-race driving and draws exactly one fully transparent quad per frame with the font
pipe silent. `5/6/7` are the between-round **standings** screen, precisely
`HudIngameDispatch 0x0040dfc0`'s `{5,6,7}` guard. "In-race UI" means the standings
overlay. State 7 needs a ~28 s settle; state 5 draws no chrome.

## The standings overlay is now fully reversed — do not re-derive any of this

The complete model, all cited in Finding 20:

```
camera   FUN_0040de30 @0x0040de30, every frame: camera frame forced to IDENTITY,
         view window {0.6, 0.45} (0x3f19999a / 0x3ee66666 at 0x0040de44 / 0x0040de4c),
         projection type 2 = parallel/orthographic.
map      screen_x = (1 - world_x / 0.6 ) * 320      <- X is MIRRORED, same as Y.
         screen_y = (1 - world_y / 0.45) * 240         (the (w/vw + 1) form is WRONG)
row      FUN_0041c410 @0x0041c410, dispatched per frame by FUN_0041cc50 @0x0041cc50:
         RwMatrixTranslate from DAT_005f337c (4 x vec3, stride 12, .data file 0x1f337c,
         populated at runtime by FUN_0041cbc0 @0x0041cbc0) = {0.48, y, 1.0},
         y in {0.25, 0.15, 0.05, -0.05}, REPLACEd onto group+0x104,
         then RwFrameScale by _DAT_005cd118 = 1.125 (applied to LOCAL coords, not to
         the row translation — which is why row centres are k-independent).
rows     row 0 = highest scorer. FUN_0040b540 bubble-sorts scores DESCENDING.
crown    every car with score >= win threshold, NOT "the leader".
         threshold = 7 if race rule in {1,2}, or DAT_0067ea64 != 0, or <4 players; else 10.
         pulse = sin(tick*0.2)*0.15 + 1.0, tick = DAT_0063d270, about the quad centre.
```

Those four row-Y values project to 106.7 / 160.0 / 213.3 / 266.7 against measured
107 / 160 / 213 / 267. The crown rule **predicts the reference**: `orig_stand7.bmp` has
scores 8/7/5/4 against a threshold of 10, which is why no capture ever showed a crown.

## DONE and verified — do not redo

| element | evidence |
|---|---|
| letterbox chrome + white rules | draw-list diff `matched 4 / 0`, two scenarios, through the entry animation |
| chrome entry slide (5 frames, `B` −65→0, alpha constant) | byte-identical both sides, guards 6 + 7 |
| text `MASHED` / `Current Standings` / `\x81 Continue` | measured fracs, diff-matched (y is BOTTOM-origin, UTF-16, prompt glyph in ctrl green) |
| per-car badges bound to Player Colour `DAT_007f1a1c` | non-degenerate, distinct colours → distinct badges |
| point circles, bar cells, max 8/12 | non-degenerate |
| **score-bar frame (U-9077 RESOLVED)** | the 4-piece premise was FALSE. a21/a22 are coincident full-bar quads with complementary half-UVs; a20 a thin strip; a23 a separate element entirely RIGHT of the bar. Shipped single-UV a22 is DFF-exact, 93.1 px vs 93 measured |
| **crown (U-9071 RESOLVED)** | position + pulse + trigger all reversed and ported; verified over two score scenarios (8,7,5,4 → no crown; 4,10,5,7 → crown row 0 only) |
| **row order** | was a real defect — the port drew row `r` for car `r`. Fixed. Original's own output for 4,10,5,7 is `[1,3,2,0]`, matching the fixed standalone |
| three C3 promotions | `0x0040b930` PlayerScoreThresholdMask, `0x0040b540` PlayerScoreRankOrder, `0x0041cc50` HudSlotUpdateCc50 — all path1 GREEN + non-degenerate, all path2 install-verified |

Test pokes available (display-only, not set in normal play): `MASHED_ROUND_SCORES`,
`MASHED_ROUND_COLOURS`, `MASHED_ROUND_RULE`, and **`MASHED_CROWN_TEST=<car>`** (new).

## Still open

- **U-9070** colour-5 badge (port maps colour 5 → `NFLShadow`).
- **U-9073/74** `DAT_0063ba8c` values 8/9/0xa/0xb; RW pipeline device slot attribution.
- **U-9076** narrowed — the layout is now emitter-corroborated; only a ~0–4 px
  ink-vs-quad reconciliation is left. Close it by diffing emitted quad rects, not ink.
- **U-9078** `DAT_0067ea64` — one flag gating BOTH the bar max (8/12) and the crown
  threshold (7/10). Decompile the car-select writers `FUN_0043dfd0` / `FUN_0043f*`.
- **U-9079** what populates `FUN_004b5190`'s atomic→slot index at runtime. Clean
  negative on file: `ENDPOINTPANEL.DFF` carries NO frame or atomic plugin data at all.
- **U-9080** row-order mode overrides (rules 4/7 via `FUN_00417740`; rule 9 via
  `DAT_007f0fcc`). Unreachable in the port's modes.
- **S-0491** `FUN_00417740` passthrough stub — blocks C4 on `0x0040b540`.

## Candidate next slices

1. **`FUN_0041c410` @0x0041c410 → C3.** The single highest-value remaining row: it is
   the standings row update and the last C2 in the chain. HARD: 1293 bytes, x87 `fsin`,
   writes live RW frame matrices. Needs a scenario harness with real standings state, and
   mind the `x87-st0-float10-fnptr-void-leak` memory.
2. **`0x0040de30` / `0x0041c9a0` / `0x0041cb10` → C3.** Each is a session. `0040de30`
   mutates the live camera and calls `RwCameraBeginUpdate`; `0041c9a0` draws through
   `vtable+0x48`; `0041cb10` loads TXD/DFF and clones clumps. None is menu-diffable.
3. **U-9078** — cheap Ghidra round, closes a determinant used by two shipped bindings.
4. **Leave the lane.** The standings overlay is visually faithful and its determinants
   are reversed. R7 has other subsystems.

## Traps that cost real time (carry these forward)

- `MASHED_RES=800x600` is **mandatory** for standalone captures: `Standalone_ScreenWidth()`
  is hardcoded 800 while `kWidth` defaults 640, so chrome lands off-screen at 640 and
  `--scale-b 0.8` still reports it matched. Do NOT "fix" that thunk — it underpins the
  frontend's GREEN 118/118 baseline.
- `sa_capture.py`'s `PrintWindow` path does **not** honour `MASHED_RES` (client stays
  640×480). For resolution-correct standalone shots use the game's own
  `DumpBackbufferBMP` outputs under `verify/run_<pid>/`. `MASHED_DBG_BBDUMP_REQ` is
  **frontend-gated** and never fires in-race.
- Read **raw values**, never formatted previews.
- No in-game function is once-per-frame — use the shim's `MashedShim_PresentCounter`.
- Never synthesise input during a capture window.
- **The degenerate trap has now hit four times in four disguises.** Force DISTINCT known
  inputs before believing any binding. The row==car defect survived precisely because
  the reference scores (8,7,5,4) make the sort an identity.
- **`hooks.csv` does NOT tell you whether an implementation exists.** A deferred batch
  leaves a hooked function reading `C2 / plate-only`. Before writing any hook, grep
  `mashedmod/src/` for `RH_ScopedInstall(..., 0x<rva>)`. I wasted a full implementation
  of `0x0040b930` this way; it built, exported and went path1 GREEN while path2 showed
  rel32 `0x70` bytes off — the U-9065 failure mode.
- **path1 GREEN does not prove install.** path2 is mandatory for any C3. A pointer-arg
  function needs its `path2_tests` in the `{'scalars': [...]}` shape or call-through dies
  `bad argument count` *before the function is entered*. Re-run path2 after **any**
  rebuild — the reported `reimpl addr` is build-specific.
- **`Path.write_text` flips LF→CRLF on Windows.** It rewrote all 3187 lines of
  `UNCERTAINTIES.md` for a five-line edit. Always check `git diff --numstat` after
  scripted edits and normalise back before staging.
- CHANGELOG inserts need an **exact-line** match on `<!-- ENTRIES -->` (the header quotes
  it in prose). Verify insert-only afterwards.
- The RW device vtable is a struct; hooking past ~slot `0x120` crashes the game.
- Sweep `[UNCERTAIN]` markers at end of session, not as you go.
- Kill only PIDs you spawned; never `git worktree remove --force`.
- Ghidra/MCP is hard-blocked on this account. Spawn a child with
  `mcp__happy__spawn_child`, `account: "claude3"`, `model: "sonnet"`. Treat it as an MCP
  proxy, not a second engineer: send narrow questions, demand verbatim decomp plus
  RVA-cited facts, and **verify what it returns**. It produced a circular
  "verification" twice this session by inverting measurements it had been handed.

## One open risk to know about

`HudSlotUpdateCc50` (`0x0041cc50`) is installed in the dev `.asi`, which auto-loads into
`MASHED.exe`, and it runs every frame in `Race::Tick`. Its ESI asm thunk is verified only
on the guard-CLEAR path — the diff deliberately seeds the four `group+0x110` guards to 0
so the `FUN_0041c410` arm never fires. A 24 s boot smoke test was clean, but the
guard-SET arm is **unexercised in production**. The next race-side standings capture is
its real test; if MASHED starts AV'ing in a race, look there first.
