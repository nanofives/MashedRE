# Next session — kickoff prompt

Written at the end of the 2026-09-02 **standings-port** lane (tip `b75fc0d1` on
`race/first-frame-parity`, tree clean, pushed). Paste the block below.

---

Resume the Mashed in-race UI lane. Branch `race/first-frame-parity` @ `b75fc0d1`,
tree clean, no children running, no worktrees or pool slots held.

Read `re/analysis/race_hud_capture_20260902.md` — start with **"State of the
standings port"**, then Findings 13-19. Do NOT re-read the whole file top to
bottom; the early Findings contain conclusions that later Findings in the SAME
file overturn (they are struck in place, but reading them cold will mislead you).
The four struck ones are: guard value 7 "never observed", the Im3D arg decode,
the `0x004c1be0` frame anchor, and "the car icon is a flat colour swatch".

**The headline finding, so you do not re-derive it: there is NO driving HUD.**
`DAT_0063ba8c == 3` is mid-race driving and draws exactly one fully transparent
quad per frame with the font pipe silent. `5/6/7` are the between-round
**standings** screen, which is precisely `HudIngameDispatch 0x0040dfc0`'s
`{5,6,7}` guard. "In-race UI" means the standings overlay. State 7 needs a ~28 s
settle to reach; state 5 draws no chrome at all.

## What is DONE and verified — do not redo

| element | evidence |
|---|---|
| letterbox chrome + white rules | draw-list diff `matched 4 / mismatched 0`, across two scenarios AND through the entry animation |
| chrome entry **slide** (5 frames, `B` −65→0 in 640-space, alpha constant) | both sides byte-identical, guard 6 + 7 |
| text: `MASHED` / `Current Standings` / `\x81 Continue` | normalised coords (y is BOTTOM-origin), UTF-16, prompt glyph `0x81` in ctrl green |
| per-car badges bound to real Player Colour (`DAT_007f1a1c`) | non-degenerate: distinct colours → distinct correct badges |
| point circles, bar cells, `max` 8/12 binding | non-degenerate: distinct scores, and rule 1 vs 0 move as predicted |

## What is still approximate

- **U-9077** — the bar frame is a single-UV (a22) approximation of the original's
  4-piece `a20`-`a23` composition. **The 4-piece was tried and FALSIFIED**: the
  UVs are known but their *screen placement is not in the DFF* (only relative
  model-X; the transform lives in the RW camera), and the only DFF-derivable
  mapping made imgdiff worse on every row (58→79, 58→82, 65→91, 70→94). Do not
  retry it the same way — it needs the per-atom RW frame transforms first.
- **U-9070** colour 5 (SHADOW) has no dedicated badge atomic; **U-9071** crown
  trigger/position; **U-9074** which pipeline slot carries the sprites (moot);
  **U-9073** guard values 8/9/0xa/0xb; **U-9076** row rects are visible-ink
  extents, not emitter args.
- `DAT_0067ea64` / participant-count determinants of `max` are unmodelled.
- Cross-renderer bar pixel parity: imgdiff ~58-70 per bar, established as a
  **uniform** librw-vs-RW render difference (region grid is flat across the bar),
  NOT dash phase — that attribution was tested and corrected.

## The obvious next slice

Reverse the **endpointpanel per-atom RW frame transforms** to get true screen
placement. That single piece unblocks U-9077 (4-piece frame), would replace the
pixel-measured row layout (U-9076) with emitter-derived rects, and is the only
thing standing between this screen and a fully derived port. Needs Ghidra.

## Tooling built this lane

- `re/frida/race_hud_burst.py` — original-side in-race draw capture. Three
  channels (Im2D quads / text at the font thunk AND glyph renderer / Im3D),
  coverage counters, `--guard-eq` to pin the race state, `--free-run`,
  `--bbdump`, `--driver warp|nav`.
- `re/tools/`: `hudburst_to_drawlist.py`, `hud_rows_check.py`,
  `hud_text_check.py`, `hud_text_fracs.py`, `band_slide_check.py`.
- d3d9 shim exports `MashedShim_PresentCounter` — **Present is the only true
  frame boundary in MASHED**; every captured draw is tagged with its real frame.

## Standing rules that cost real time this lane

- **`MASHED_RES=800x600` is mandatory for standalone captures.**
  `Standalone_ScreenWidth()` is a hardcoded 800 while `kWidth` defaults to 640, so
  `ChromeBaseDraw` always emits 800-space; at 640x480 the chrome lands off-screen
  AND `--scale-b 0.8` reports it as matched anyway. The diff will hide the defect.
  Do NOT "fix" that thunk — it underpins the frontend's GREEN 118/118 scr1 baseline.
- **Read raw values, never the formatted preview.** A rounded `xy_f=(0.10,0.09)`
  baked a 2px error; `str_utf16` hid the `0x81` nav glyph as a "leading space".
  Both shipped before being caught.
- **No in-game function is once-per-frame.** `0x004c1be0` fires 5-10x per frame;
  `DAT_0063ba8c` is mutated many times per frame so it cannot gate frames. Use
  the shim Present counter.
- **Never synthesise input during a capture window** — pulsing confirm carries the
  game out of the race into standings and silently changes what you measure.
- **The degenerate trap bit three times, differently each time**: all-same
  characters, settled-only frames, all-same scores. Before believing any binding,
  force DISTINCT known inputs and check the outputs differ AND match.
- **The RW device vtable is a STRUCT** — past ~slot `0x120` it holds data, not
  code. Attaching an Interceptor there crashes the game. Range-check.
- Sweep `[UNCERTAIN]` markers at END of session: in a long single-session note the
  early markers get answered by later findings, so filing them as you go creates
  rows that are already resolved.
- Kill only PIDs you spawned. Never `git worktree remove --force`.

## If you need Ghidra or any MCP — delegation policy

**Ghidra MCP is hard-blocked on this account (account2).** When a task needs
Ghidra decompilation, or any other MCP this account cannot reach:

- Spawn a child on **account3** with `mcp__happy__spawn_child`, passing
  `account: "claude3"` and **`model: "sonnet"`** — not opus. The child exists to
  be an **MCP proxy**, not a second engineer.
- **Do the heavy lifting in the parent session.** Keep the judgment work here:
  what to ask, what counts as evidence, designing the verification, deciding
  whether a result is degenerate, the port itself, and the tracker transaction.
  Send the child narrow, well-specified questions and have it return
  decompilation plus RVA-cited facts.
- Give it the context it cannot infer: the measured facts it must NOT re-derive,
  the traps below, and the working-tree state. A cheap model with a precise brief
  outperforms an expensive one guessing at scope.
- **Verify what it returns before acting on it.** Every claim relayed from a
  child this lane was re-checked here first, and that caught real errors in both
  directions (a false "no Resolved section exists" from the child, and an
  unfounded wrong-file suspicion from me).
- Stand it down with `mcp__happy__close_child` when the MCP work is done.

Ask before spawning a fleet or anything needing a human at the keyboard.
