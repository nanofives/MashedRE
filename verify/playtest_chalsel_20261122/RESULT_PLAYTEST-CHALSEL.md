# PLAYTEST-CHALSEL: challenge select arrows + versus strip (2026-11-22)

Branch `fix/playtest-challenge-select`. Two playtest bugs on screen 6/7 of the default standalone build.
Stock save throughout (`original/gamesave.bin`, the seed the LAN release ships, sha256 `bd187881...`).

## Original-side evidence (MASHED.exe, cup flow: confirm x4 from the main menu)

| file | what |
|---|---|
| `o_s6_m3.json` / `.text.json` / `.peek.json` | Bronze Cup row 0: mode `0x0067e9fc`=3, `0x0067f184`=1, one player (slot 0 colour 0, slots 1-3 profile -1). Text: `0x115` Bronze Cup, `0xec` Battle Game, `0x118` battle against 1 opponent at (326, 316/334/350), one list row `0x49` at y 178. No `0x22` "Multi Player" checklist. |
| `o_s6_m3_right.*` | after RIGHT: mode 4, `f184`=2. Text `0x116` Silver Cup, `0x11f`, `0x120`, `0x121`. Strip: player 112 px vs three opponents at 64 px, power-up row. |
| `o_s6_m3_down.*` | after DOWN: mode 3, row `0x0067f17c` still 0 (only row 0 has `0x007f0a44` != 0 on the stock save). |

Ghidra (Mashed_pool10, analyzeHeadless DecompPC) for `0x00439210`, `0x004368e0`, `0x004391b0`, `0x0042ee40`,
`0x00430a10/60/b0`, `0x0042ef40`, `0x00472dc0`; capstone for the `0x004391b0` EAX argument (`0x0043a184`,
`0x0043a521`) and the `0x0042ee40` sprite names.

## Standalone evidence

- `navkeys_sp_cup.txt`: cup flow, before the fix. RIGHT already moved mode 3->4 / sel 1->2, DOWN walked it back
  (RowMoveFwd). The state matched the original, but the screen showed nothing of it.
- `navkeys_mp6.txt`: Top Dog (mode 6), rows 0->1->2->3 move.
- `diff_bronze_m3_before.txt`: before the fix, 52 EXTRA / 53 MISSING.
- `diff_bronze_m3.txt` (Bronze, mode 3) and `diff_silver_m4.txt` (Silver, mode 4), after the fix. Every row the
  screen-6 block owns matches, except these:
  - Star `0x00439c0e` colour and selected-tier sprite `0x0043a153` / `0x0043a315` size: animated (sin of
    `DAT_0067ed30`). The two sides' clocks are not synchronised.
  - Logo band `0x473f6c` / `0x473f87` / `0x4733a2..`: slide position of the shell's logo animation. Not in this block.
  - Preview `0x42eafa` (two 135x180 halves + 2 px frame) against the standalone's single 290x160 preview quad: the
    shell's preview, left as it was.
- `re_s6_m4.png` (Silver), `re_s6_m6.png` (Top Dog row 1), `inter_s6_m4.png` (after commit 1 only).

## Gates (A4_ROOT private copy, `gates_RUNLOG.txt`)

G-CTRL `8284be46 / 67931834 / 31fe043e`, B `1d5bf4f9` x3. Equal to `re/orchestrator/merge_train.json`.

## Not ported (open)

- Mode 10 (Quick Race) panel and strip (`DAT_0067ea94..eaac` not tied to the frontend's copies).
- Mode 2 best-time string (two ftol operands the decomp drops).
- Events 0xb0000 / 0xc0000 power-up row (animated, random id).
- Rotated "Challenge Completed / Locked" stamp (`FUN_0042ed70`).
- The mode-6 strip has no original-side capture: the harness `down` step does not move the original's main menu.
  It is ported from the decomp only.
