# In-race HUD: original-side capture harness + first measurements

Date: 2026-09-02. Branch `race/first-frame-parity`. Session goal: open the
"port the UI elements into the race" slice with a parity capture, per the
harness rule that composition work is accepted on a draw-list diff rather than
screenshots (`re/analysis/parity_tooling.md`).

All numbers below are measured from stock `original/MASHED.exe`
(`MASHED_RE_NO_AUTO_HOOK=1`, no `.asi`). NO-GUESSING: every claim is either a
counter reading or a captured record, and the places where the evidence runs out
are marked `[UNCERTAIN]`.

## Tool added: `re/frida/race_hud_burst.py`

Nothing in the tree captured an in-race 2D draw list before this.
`menu_draw_burst.py` is frontend-gated (its `collecting` flag is armed only by
the ShellA frame delimiter `FUN_0042e3a0`, a frontend function, so in a race it
never arms). `race_draw_burst.py` is **not** the sibling tool: it captures 3D
draw-call *totals* from the d3d9 shim's slot counters plus a backbuffer BMP, not
a draw list.

The new tool captures **three channels**, bracketed by a frame anchor, with
coverage counters armed before the first run:

| channel | hook | why |
|---|---|---|
| quads | device vtable `*(0x007d3ff8)+0x30` (`RwIm2DRenderPrimitive`) | `HudIm2DQuad 0x00450b10` ends in `rw_draw_4verts` -> this slot (`DrawQuadPrimitives.cpp:506-507`). Same `v`/`r` schema as `menu_draw_burst.py`, so `drawlist_diff.py` decodes both sides with one decoder. |
| text | `FUN_00556ca0` (thunk) **and** `0x00554940` (glyph renderer) | Glyphs go `0x00554940 -> FUN_004cd070` (Im3D) and do **not** pass the `+0x30` draw (`MenuDrawLoopTwin.cpp:114-117`). A quad-only diff would score GREEN while every number on screen was wrong. |
| Im3D | `0x004cd070` `RwRenderPrimitiveSubmit` | The remaining candidate pipe once both of the above measured silent while driving. |

Drivers: `--driver warp` (scenario_launch's `PHASE 0x00771968 = 2` poke) and
`--driver nav` (normal menu->race flow). `--guard-eq N` pins collection to one
`DAT_0063ba8c` value; `--no-press-settle` suppresses all synthetic input after
the race starts.

## Whole-run call counts (`MASHED_COUNT_RVAS`, mode 10, 4 cars, ~20 s / ~1250 frames)

```
0x0040dfc0 HudIngameDispatch       1254
0x00450b10 HudIm2DQuad             2003
0x00556ca0 font-ctx thunk          1352
0x00554940 charset glyph renderer  1352
0x004cd070 RwRenderPrimitiveSubmit 7158
0x0041db80 post-switch tail        1106
0x00427f00 / 0x00427ff0               0
0x0041e850 / 0x0041ded0               0
```

`0x00427f00` and `0x00427ff0` are the two documented draw-string wrappers and
both are **dead in-race**. Note these totals span boot + menu + load + race, so
they cannot be attributed to the race by themselves -- which is exactly the trap
the windowed capture below avoids.

## `DAT_0063ba8c` semantics (NEW — previously left open)

`scenario_launch.py` records an XrefRange scan of this global and states
explicitly that "NO SEMANTIC IS ASSIGNED" to any of its values. Two values are
now pinned by measurement:

| value | state | evidence |
|---|---|---|
| **3** | mid-race **driving** | With `--no-press-settle`, 320/321 `0x0040dfc0` entries read 3, and the captured frames show the race. Holds on TRAINING and on `--track 1`. |
| **5 / 6** | between-round **standings** screen | Reached only when confirm is pulsed. Captured draw list is letterbox bands plus the strings "MASHED" and "Current Standings". |

Values `0x2` and `0x4` were each seen once, at transitions. ~~`[UNCERTAIN]` 7 was
never observed~~ — **superseded by Finding 3**: 7 IS reached, but only after a ~28 s
settle, and it is then the dominant value (1927 of 2989 dispatcher entries). The
short-settle runs behind this paragraph simply never got there.
`[UNCERTAIN U-9073]` `0xa`, `0xb`, `8`, `9` appear in the static scan but in no run
here, so their states are still unidentified.

## Finding 1: `HudIngameDispatch 0x0040dfc0` is NOT the driving HUD

Its guard needs `DAT_0063ba8c` in `{5,6,7}` (`HudDispatch.cpp:133-141`). While
driving, `DAT_0063ba8c == 3`, so the guard passes **0 of 321** calls — every
call is an early return. The two sub-dispatchers `0x0041e850` / `0x0041ded0`
counting 0 for the whole run corroborates this independently.

Where it *does* run is the standings screen (guard 5/6), and there it emits:

| draw | emitter chain | geometry |
|---|---|---|
| letterbox band, top | `0x410cff -> 0x472db4` (`ChromeBaseDraw 0x00472c60`) | 640x80 @ (0,0) `ff000000` |
| letterbox band, bottom | same | 640x80 @ (0,416) `ff000000` |
| white rules | same | 640x1 @ y=80 and y=416 `ffffffff` |
| "MASHED" | `0x427ed2` | xy `(0.1000, 0.8758)` scale `0.0566` |
| "Current Standings" | `0x427ed2` | xy `(0.1000, 0.8433)` scale `0.0425` |

Text coordinates are **normalised** (0..1), not pixels. String buffers are
**UTF-16** (an ASCII read stops at the first NUL and yields only `"M"` / `"C"`),
consistent with `FontText_UTF16WidenCopy` existing in `HudBatch.cpp`. Both text
channels recorded the same 12 strings independently, which cross-validates the
listing-inferred arg offsets for `0x00554940` (U-1067): on entry
`[esp+4]=font_ctx`, `[esp+8]=str_buf`.

## Finding 2: the driving state emits almost nothing on either 2D pipe

With `--guard-eq 3 --no-press-settle`, per frame, stable across 6 frames and
across both TRAINING and `--track 1`:

- **quads: exactly 1**, a 512x512 quad at (0,0) with colour `00000000` (fully
  transparent), chain `0x00410b54 -> 0x004271ed -> 0x00450c7a`.
- **text: 0**, at both the thunk and the glyph renderer, inside *and* outside the
  bracket. The font pipe is entirely silent while driving.
- **Im3D: 4 submits**, all from a single retaddr `0x00422a50`.

So the lap counter / position / timer / speed that the game visibly draws while
driving are on **none** of: the Im2D device slot, the charset/font pipeline, or
`HudIngameDispatch`. `0x00422a50` is the one live emitter identified and is the
next thing to chase.

~~`[UNCERTAIN]`~~ **RESOLVED by Finding 4** (capstone disasm of both call sites gave
the real shape: `arg1 = verts_ptr`, `arg2 = count`, stride `0x24` proven by the
divide-by-36 reciprocal magic). The paragraph below records the wrong reading that
Finding 4 corrected — kept because it is why the first Im3D numbers looked like
garbage. The Im3D arg decode was **not** trustworthy. Using the offsets from
STUBS S-2120's cited call site (`0x00554afa`, `(base_ptr, ptr, count, prim=5)`)
against this caller yields `count=0 prim=25 base=0x1ae2d4`, i.e. `prim` is not 5
and `base` is not `DAT_00912a04`. Either this caller passes a different shape or
`0x004cd070` is not `__cdecl` with those offsets. Only the retaddr attribution
(4/frame from `0x00422a50`) should be relied on from that channel.

## Finding 3: there is NO driving HUD — the in-race UI is the standings screen

Settled by backbuffer dump through the d3d9 shim's `MASHED_ORIG_BBDUMP_REQ`
protocol, with `DAT_0063ba8c` verified at request time.

- `verify/race_hud/orig_drive.bmp` (guard **3**, 5 s in): the race, four cars at
  the start line, and **no 2D HUD whatsoever**. The only overlay-looking things
  are world-space markers (a circular powerup icon, a magenta dot upfield).
- `verify/race_hud/orig_drive_late.bmp` (guard **7**, 28 s in): the full in-race
  UI — letterbox bands, "MASHED" / "Current Standings", four car-icon rows with
  score bars, `+2 / +1 / -1 / -2` point circles, and "Continue".

`DAT_0063ba8c == 7` was not observed at all until a 28 s settle, and is then
dominant (1927 of 2989 dispatcher entries). Completed timeline: **3** (driving)
-> **5** -> **6** -> **7** (standings/results). This closes the guard question:
`HudIngameDispatch`'s `{5,6,7}` is **exactly the set of standings/results
states**, which is why it early-returns for the entire driving phase.

So "port the UI elements into the race" resolves to **port the standings /
results overlay**, not a driving HUD. The standalone's `[SCAFFOLD]` per-car
score-pip row (`exe_main.cpp:2944-2965`) is an approximation of this screen.

### State-7 reference draw list (`--guard-eq 7`, stable over 3 frames)

Per frame: 5 quads + 3 strings.

| draw | emitter | geometry |
|---|---|---|
| invisible quad | `0x410b54 -> 0x4271ed -> 0x450c7a` | 512x512 @ (0,0) `00000000` |
| letterbox band, top | `0x410cff -> 0x472db4` | 640x80 @ (0,0) `ff000000` |
| letterbox band, bottom | same | 640x80 @ (0,416) `ff000000` |
| white rules | same | 640x1 @ y=80 and y=416 `ffffffff` |
| "MASHED" | `0x427ed2` | xy `(0.10, 0.88)` scale `0.0566` |
| "Current Standings" | `0x427ed2` | xy `(0.10, 0.84)` scale `0.0425` |
| " Continue" | `0x427ed2` | xy `(0.10, 0.09)` scale `0.0425` |

**The car icons, score bars and point circles are NOT in the quad channel** —
only those 5 quads are emitted. See Finding 4 for where they are and are not.

## Finding 4: `0x004cd070` arg shape and vertex layout (ESTABLISHED)

Both call sites were disassembled directly out of `MASHED.exe` with capstone
(pefile-mapped image, no Ghidra needed):

```
0x00422a4b:  push 0x19 / push 0 / push edx(=3*esi) / push eax(=[esp+0x2c], stack buf)
0x00555182:  mov eax,0x38e38e39 / imul ecx / sar edx,3 / push edx / push esi
```

=> `FUN_004cd070(verts_ptr, count, a3, a4)`, `__cdecl`. The S-2120-derived
labels were **wrong** and produced `count=0 prim=25` nonsense; `arg1` is the
vertex pointer and `arg2` is the count.

The reciprocal magic `0x38e38e39` + `SAR 3` at `0x0055516f` is a divide-by-36,
which **confirms stride `0x24`** and matches the documented
`tag=0x24=sizeof(Im3DVertex)`. Decoding as RW3.x `RwIm3DVertex` —
`objVertex(12) + objNormal(12) + color(4) + u(4) + v(4) = 36` — reproduces exact
0/1 corner UVs and uniform per-submit colours, so the layout is confirmed by the
data as well as by the disassembly.

### What the Im3D pipe actually carries (free-run, ~36 frames)

| emitter | per frame | content |
|---|---|---|
| `0x00477ae1` | 12 | **tyre-track ribbons**: ground-constant `y ~= -0.21`, ~0.1-unit width, UV `v` stepping 0..6 along the strip, alpha `0x29`/`0x2a`. 12 = 4 cars x 3 ribbon groups. |
| `0x00555187` | 3 | **glyph geometry**, one submit per string, in text-local space (x 0..5.6, y 0..1, z 1, `ffffffff`). Counts confirm it: 24/4 = 6 = `len("MASHED")`, 68/4 = 17 = `len("Current Standings")`. |
| `0x00422a50` | 2 | ground skid decals, `y ~= -0.24`, tri-lists, alpha `0x2c`..`0x5a`. |

`[CORRECTED]` `0x00477ae1`'s 12/frame was first read as "4 icons + 4 bars + 4
circles". That was a coincidence — the geometry is tyre marks. An earlier
3-sample look at it came from a sub-frame slice that caught 3 of the 12.

### Where the icons/bars/circles are NOT, and where they are

A free-run capture (no bracketing, no gate, 0.6 s / ~36 frames) is the
**complete** inventory of both pipes: 5 quads/frame on Im2D and 17 submits/frame
on Im3D, all accounted for above. The icons, score bars and point circles are in
**none** of it, so they bypass both `+0x30` and `0x004cd070`.

A device-vtable scan (slots `0x00`..`0x11c`, counted while collecting) locates
them on the **standard RW render pipeline**, not the immediate-mode pipes:

| slot | fn | per frame | via |
|---|---|---|---|
| `0x118` | `0x004cca80` | 11 | `0x004e3d30` |
| `0x11c` | `0x004ccba0` | 11 | `0x004e3c89` |
| `0xe8` | `0x004af340` | 8 | `0x00484394` <- `0x004e568d` |
| `0x20` | `0x004d7480` | ~200 | render-state setter (`0x410b3d`, `0x42710f`) |
| `0x4c` / `0x70` | `0x004ca160` / `0x004cab30` | 10 each | begin/end-update wrappers, `(0, obj, 0)` — NOT draws (disassembled at `0x004c2b92`/`0x004c2bcf`) |

`[UNCERTAIN U-9074]` Which of `0x118`/`0x11c`/`0xe8` carries the UI sprites, and their
vertex buffers, are not established. Their retaddr chains run through RW
internals (`0x004e3xxx` pipeline exec) and then into system DLLs, so
backtrace attribution stops being useful there. **Largely moot as of Finding 14**: the
layer was identified structurally instead (the `endpointpanel` clump's 24 `RpAtomic`s,
which is *why* it lands on pipeline slots), and Findings 15/16 port it through
`HudIm2DQuad` without needing to capture those slots at all.

**Harness caution:** the vtable is a STRUCT — past roughly slot `0x120` it holds
data, not code (`0x3`, `0x200000`, `0x80000000`, heap pointers). Attaching an
Interceptor to those **crashed the game**. The scan now range-checks
`0x00401000..0x005e0000` before hooking.

## Hypotheses tested and DISPROVED

- **"The warp driver skips HUD setup."** Asserted mid-session, then disproved.
  The variable was never the driver: pulsing confirm (copied from
  `scenario_launch.py`'s hold loop) carries the game out of the race into the
  standings screen. With `--no-press-settle`, warp and nav agree — both sit at
  `DAT_0063ba8c == 3` and both show the same near-empty driving draw list.
- **"Quick Battle races TRAINING, which has no HUD."** Disproved: `--track 1`
  behaves identically.
- **"`0x0040dfc0` early-returns in-race, so it is not the HUD entry point"**
  (first pass) — was based on the static scan's "3 observed at phase 3" and was
  wrong as stated, since the guard *does* pass on the standings screen. The
  correct scoped claim is Finding 1.

## Harness bugs found and fixed

- Detaching the frame-delimiter hook on the k-th hit left the final frame open,
  so it swallowed every later draw (12 quads in `f5` vs ~1 in `f0..f4`). Frames
  are now closed by the next anchor hit.
- Preferring the ASCII string decode hid every real string behind its first
  letter. UTF-16 is now tried first; raw bytes are always stored.

~~`[UNCERTAIN]`~~ **RESOLVED by Finding 5** — and resolved as a NO: `0x004c1be0` fires
~5-10x per frame, so this anchor was wrong and every "frame" captured through it was a
sub-frame slice. Frame boundaries now come from the d3d9 shim's exported Present
counter; the anchor path is retained only for comparison. The original caveat read:
frame anchor `0x004c1be0` is used because `scenario_launch.py`
drives its statediff clock from it at ~60/s. Once-per-frame is **not**
established anywhere in the tree. Observed quads/frame is exactly 1.00, which is
consistent but not proof. `--anchor` overrides it.

## Finding 5: frame boundaries, RESOLVED via a shim-exported Present counter

MASHED has no verified once-per-frame function, and both anchors tried failed:
`0x004c1be0` fires ~5-10x per frame (so every bracketed "frame" was a sub-frame
slice — `quad_out:quad_in` ran ~9:1) and `0x00492e90` is no better.
`DAT_0063ba8c` cannot gate frames either: it is mutated many times per frame, so
gating on `== 7` skipped 344 of 347 anchor hits.

Present is the only true frame boundary, so the d3d9 shim now exports the
**address** of its existing `g_PresentCount`:

```cpp
// mashedmod/src/d3d9_shim/d3d9_shim.cpp
extern "C" __declspec(dllexport) const volatile LONG* MashedShim_PresentCounter()
{ return &g_PresentCount; }
```

plus one line in `d3d9_shim.def`. Returning the address (not the value) lets the
Frida agent read the counter inline in each draw hook with no extra call per
draw, so **every captured draw is tagged with its frame index** (`"f"` field) and
no frame anchor is needed at all. `--free-run` collects ungated; frames are
grouped offline by `f`.

**Verification** — 30 consecutive frames at standings state 7, all three
channels, zero variance:

| channel | per-frame histogram |
|---|---|
| quads | `{5: 30}` |
| text | `{6: 30}` (3 strings x 2 channels) |
| Im3D | `{10: 30}` |

A representative frame (2034) decomposes exactly as expected: `0x450c7a` x1 +
`0x472db4` x4 quads, `0x427ed2` x6 text, `0x422a50` x3 + `0x477ae1` x4 +
`0x555187` x3 Im3D. No anchor ever produced a uniform histogram; this is the
first frame-accurate in-race capture in the tree.

Two integration traps worth keeping:
- Resolve the export **lazily**. `init()` runs immediately after spawn, before
  the game has loaded `d3d9.dll`, so an eager lookup always fails and every draw
  silently tags `f=-1`.
- The Frida module API moved: top-level `Module.findExportByName` is gone in
  17.x (`TypeError: not a function`). The agent now tries
  `Module.findExportByName`, `Module.getGlobalExportByName`,
  `Process.getModuleByName().findExportByName`, and a module enumeration, in
  order. A missing counter is reported loudly rather than degrading to `f=-1`.

## Finding 6: FIRST DIFF RUN — RED, total divergence (this is the port target list)

Standalone-side blocker fixed first: `DrawStreamDump_OnFrameBegin()` had exactly
one call site, `exe_main.cpp:3145`, in the **frontend** loop, while the
race/results branch `return true`s before reaching it. So the frame counter never
advanced during a race and every in-race draw was dropped on the `Capturing()`
guard — the standalone's drawstream was frontend-only in practice, exactly
mirroring the original's frontend-gated capture. Fixed by adding the delimiter at
the **top** of the race branch (`exe_main.cpp:2609`, before the HUD draws, so
they arm the right bucket). After the fix: `DRAWSTREAM wrote 801 frames
(13586 draws)`.

Also settled a contradiction between two earlier read-only passes:
`DrawMashedString` **does** emit through `HudIm2DQuad`/`HudIm2DQuadCorners`
(`exe_main.cpp:2522-2525`) into the bridge chokepoint
(`RwIm2DBridge.cpp:131-132`), so standalone text **is** recordable. The empty
race captures were the frame delimiter, not a charset detour.

**Driver** (all env, `exe_main.cpp:6820-6870` + `2891-2918`):
`MASHED_TRACK_VIEW=1 MASHED_CAR=1 MASHED_ROUND=1 MASHED_RESULT_DEMO=1`.
**Scale:** the race HUD and results overlay do NOT apply `kVScale` — they pass raw
640x480 coordinates — so `--scale-b 1`, not the 0.8 default that suits the
800x600-authored frontend menu.

Comparison frame chosen on both sides by content: the original's standings state
7, and the standalone's 32-draw frames whose 17 glyphs at y=44.1 spell
`"CURRENT STANDINGS"`.

```
py -3.12 re/tools/hudburst_to_drawlist.py log/race_hud_burst.json -o log/race_hud_frames.json --frames 6
py -3.12 re/tools/drawlist_diff.py log/race_hud_frames.json log/drawstream_re_stand.json \
    --scale-b 1 --exclude-tex 9 --map mashedmod/build/mashed_re.map
```

**Result: exit 1, `matched 0  mismatched 0  missing 5  extra 7`, identical across
all 6 frame pairs** (full output: `verify/race_hud/diff_standings_20260902.txt`).
Nothing in the standalone's scaffold corresponds to anything the original draws.

### MISSING — the original draws these, the standalone does not

| draw | emitter | note |
|---|---|---|
| 512x512 @ (0,0) `00000000` | `0x450c7a` <- `0x4271ed` | fully transparent; visually a no-op, may be safe to skip |
| 640x80 @ (0,0) `ff000000` | `0x472db4` <- `0x42a2ad` | letterbox band, top |
| 640x80 @ (0,416) `ff000000` | `0x472db4` <- `0x42a2d9` | letterbox band, bottom |
| 640x1 @ (0,416) `ffffffff` | `0x472db4` <- `0x42a307` | white rule |
| 640x1 @ (0,80) `ffffffff` | `0x472db4` <- `0x42a32c` | white rule |

All four visible rows come from `ChromeBaseDraw 0x00472c60`, which is **already
C3 and ported** — so the bands and rules are a wiring job, not a reversing job.
The four distinct callers `0x42a2ad`/`0x42a2d9`/`0x42a307`/`0x42a32c` are
consecutive call sites in one function, i.e. a single chrome routine to port.

### EXTRA — the standalone invents these

| draw | count | note |
|---|---|---|
| 22x20 @ x=20, y=24/50/76/102 (red/blue/green/yellow) | 4 | `[SCAFFOLD]` per-car pips, `exe_main.cpp:2950` |
| 10x20 score-bar segments | 3 | `[SCAFFOLD]`, `exe_main.cpp:2954` |

### Text layer (compared separately; the differ cannot align it)

The original's Im2D stream contains no text at all (glyphs are on Im3D), so
`--exclude-tex 9` drops the standalone's glyphs and the text layer is compared as
strings instead:

| side | strings |
|---|---|
| original | `"MASHED"` @ `(0.10, 0.88)`, `"Current Standings"` @ `(0.10, 0.84)`, `" Continue"` @ `(0.10, 0.09)` — normalised coords, UTF-16 |
| standalone | `"CURRENT STANDINGS"` @ y=44 (all caps, 640x480 px), plus four per-car `"%+d"` deltas |

Divergent in case, count, position and coordinate space. Note the original's
`" Continue"` carries a leading space and sits at the BOTTOM (`y` frac 0.09)
under a `Continue` prompt glyph, whereas the standalone's `"[ESC] Continue"`
(`exe_main.cpp:3038`) is in the unreached results overlay, not this banner.

**Interpretation.** This is not a regression — it is the first measurement of a
scaffold that was never claimed to be faithful (`[SCAFFOLD]` at
`exe_main.cpp:2933-2936`). The value of the RED is that the target is now a
5-row list with emitters attached, and the acceptance gate is reproducible.

## Finding 7: standings chrome PORTED — the 4 targeted rows are now GREEN

The four MISSING chrome rows from Finding 6 are ported and matched. Args were
recovered by disassembling the original's four consecutive `ChromeBaseDraw`
`0x00472c60` call sites (cdecl, last push = arg1) — no guessing, every constant
read from the image:

| # | call site | args `(x, y, w, h, argb)` | y at `B=0` |
|---|---|---|---|
| 1 | `0x0042a2a8` | `(0, B, 640, 80, 0xff000000)` | 0 |
| 2 | `0x0042a2d4` | `(0, 480-B-64, 640, 80, 0xff000000)` | 416 |
| 3 | `0x0042a302` | `(0, 480-B-64, 640, 1, 0xffffffff)` | 416 |
| 4 | `0x0042a327` | `(0, B+80, 640, 1, 0xffffffff)` | 80 |

Constants: `_DAT_005cd6d8 = 480.0` (`0x0042a2ad fld`), `_DAT_005cd6d4 = 64.0`
(`0x0042a2c2 fsub`), `_DAT_005cc730 = 80.0` (`0x0042a30d fadd`), band height
`80.0` (`0x0042a27c push 0x42a00000`), width `640.0`
(`0x0042a289 push 0x44200000`), `DAT_008991b4 = 0.0` (band offset).

Implemented at `exe_main.cpp:2952+`, gated on the standings condition
(`round_winner() >= 0 || match_winner() >= 0`), emitted in the original's order.

**Result** (`verify/race_hud/diff_standings_after_chrome_20260902.txt`):

| | before | after |
|---|---|---|
| matched | 0 | **24** (4 rows x 6 frames) |
| mismatched | 0 | **0** |
| missing | 30 (5 x 6) | 6 (1 x 6) |

All four chrome rows match on position, size AND colour. Overall verdict is still
`RED (match=24 mismatch=0 missing=6 extra=66)`, which per the parity harness's
per-item rule is acceptance for this item: the targeted rows are gone and no new
divergence kind appeared. Residuals, both pre-existing:

- **missing 6** = the 512x512 `00000000` quad (`0x450c7a`), fully transparent, a
  visual no-op. Deliberately not ported.
- **extra 66** = the `[SCAFFOLD]` per-car pips and score-bar segments
  (`exe_main.cpp:2961`/`2965`). Same family as before, identified by identical
  retaddr (`HudIm2DQuad+0x188`) and colours; the count rose from 7 to 11 per
  frame only because the chosen comparison frame is later in the match and more
  score segments have accumulated, NOT because anything new was added.

### Scale: the standalone mixes two coordinate spaces

`ChromeBaseDraw` scales internally (`ScreenWidth * arg * kScaleX`), so the ported
chrome emits at 800x600 (`800x100` bands at y=0/520, rules `800x1.25`) — exactly
1.25x the original's 640x480, i.e. faithful. It therefore needs `--scale-b 0.8`.
The `[SCAFFOLD]` pips pass **raw unscaled** pixels via `HudIm2DQuad` and so sit
in 640-space inside an 800-wide backbuffer. **That is a real scaffold defect**: on
screen the pips are mispositioned relative to correctly-scaled chrome. It also
means one frame cannot be diffed at a single `--scale-b`; the earlier
`--scale-b 1` choice was right for the raw-pixel scaffold and wrong for anything
ported through the scaling path.

`[UNCERTAIN U-9075]` The band alpha comes from `BL`, set before `0x0042a240`; its
animated source is not established. `0xff` is the settled value in every captured
frame, so a fade-in is not modelled.

## Finding 8: standings TEXT ported (3 strings, normalised coords)

Replaces the invented centered all-caps `"CURRENT STANDINGS"` banner
(`exe_main.cpp`) with the original's three strings, measured via the text channel
of `race_hud_burst.py` (thunk `FUN_00556ca0`, emitter `0x00427ed2`).

**Coordinate convention decoded** from the captured fracs plus the reference shot:

```
x_px = frac_x * 640          (left origin)
y_px = (1 - frac_y) * 480    (BOTTOM origin)
```

`"MASHED"` `frac_y 0.8758` -> 59.6 px from the top (title sits at ~y=45-60 in
`verify/race_hud/orig_drive_late.bmp`); `" Continue"` `frac_y 0.09` -> 436.8 px
(prompt at ~y=428). A top-origin reading would put `" Continue"` at y=54, stacked
on the title, which the shot rules out.

**Sizes:** the thunk receives `p5 * 0.0708` (the same 0.0708 em factor already in
`DrawMashedString`'s `top_y` law), so the captured `0.0566 / 0.0425` are
`p5 = 0.80 / 0.60`. Cell height = captured frac x screen height.

| string | frac | scale |
|---|---|---|
| `"MASHED"` | (0.1000, 0.8758) | 0.0566 |
| `"Current Standings"` | (0.1000, 0.8433) | 0.0425 |
| `" Continue"` | (0.1000, 0.0900) | 0.0425 |

Strings are verbatim in the original's mixed case, drawn `anchor_left` (the
original is left-aligned, not centered).

`[CORRECTED by Finding 11]` This finding originally recorded the prompt line as
`" Continue"` with a **leading space** that "reserves room for the prompt glyph".
That was a decode error: the first code unit is `0x0081`, a nav glyph, not a
space. See Finding 11.

**Verification.** `drawlist_diff.py` cannot check text: `--exclude-tex 9` drops
the standalone's glyphs and the original's Im2D stream contains no text at all
(glyphs are on Im3D). So text was verified two other ways:

1. **Numeric**, from the standalone drawstream at 800x600 — glyph counts and
   geometry exactly as predicted:

| line | glyphs | x_left | cell_h | expected |
|---|---|---|---|---|
| `"MASHED"` | 6 | 80.00 | 33.96 | `0.1*800`, `0.0566*600` |
| `"Current Standings"` | 17 | 80.00 | 25.50 | `0.1*800`, `0.0425*600` |
| `" Continue"` | 9 | 80.00 | 25.50 | y=534.8, inside 600 |

2. **Pixel backstop** — `verify/race_hud/re_stand_t4.png` vs
   `verify/race_hud/orig_drive_late.png`: top band with "MASHED" +
   "Current Standings", white rules at both band edges, bottom band with
   "Continue". Structurally matches the original.

Chrome remains 4/4 matched after the text change
(`verify/race_hud/diff_standings_after_text_20260902.txt`, still
`match=24 mismatch=0 missing=6 extra=66`).

## Finding 9: `MASHED_RES=800x600` is REQUIRED, and why (thunk/backbuffer mismatch)

`Standalone_ScreenWidth()` returns a **hardcoded 800**
(`Compat/StandaloneRvaThunks.cpp:16`, installed over `0x0042b8b0`), while
`kWidth` defaults to **640** (`exe_main.cpp:367`). `ChromeBaseDraw` scales
internally by `ScreenWidthGet() * arg * kScaleX`, so it always emits **800-space**
regardless of the real backbuffer.

Consequence at the default 640x480: the ported bottom band lands at y=520 in a
480-tall buffer — **off-screen** — and the right ~20% is clipped. The
`--scale-b 0.8` diff mapped 800->640 and reported those rows as matched, so
**the diff hid a real visual defect**. Anything ported through the scaling path
must be captured with `MASHED_RES=800x600`, where thunk and backbuffer agree;
verified on-screen in `re_stand_t4.png`.

The thunk was deliberately NOT changed: it underpins the frontend menu's
GREEN 118/118 scr1 baseline, so making it track `kWidth` is a
whole-frontend-parity decision, not a HUD one. Filed as the open item below.

**Still divergent** (visible in `re_stand_t4.png`):
- The `[SCAFFOLD]` pips/bars/deltas pass raw unscaled pixels, so at 800x600 they
  sit in the upper-left and **overlap the title text**. In the original the
  per-car rows are icon+bar+circle rows over the 3D view below the band. Same
  root cause as Finding 7's scale note.
- The original's `" Continue"` is preceded by a green circular-arrow prompt
  glyph; the leading space is ported but the glyph is not.

## Finding 10: standings rows scaled and repositioned (layout measured from pixels)

The `[SCAFFOLD]` per-car rows passed **raw unscaled** pixels, so once the chrome
and text were ported at 800x600 the rows sat in the upper-left on top of the
title (visible in `verify/race_hud/re_stand_t4.png`). Fixed: rows now scale by
`kUiS = kWidth/640` and sit on the original's measured layout.

**This layer could not be measured from a draw list.** It is emitted on the RW
pipeline device slots (`0x118`/`0x11c`/`0xe8`, Finding 4), so it appears in no
capture on either side. The layout was therefore measured from the reference
backbuffer `verify/race_hud/orig_drive_late.png` (640x480) by pixel analysis of
the saturated UI ink:

| element | measured (640-space) |
|---|---|
| score-bar frame | 4 bands, `x 87..179` (w=93), `h=17` |
| row centres | `y = 107.0 / 160.0 / 213.0 / 267.0` |
| icon red-ink box | `x 33..74` (w=42), `h=48` |
| point circle | `x 185..246`, diameter ~40 |

The bar frame is the cleanest signal; the icon boxes and point circles
independently agree on the same four centres, which is what makes the row
positions trustworthy rather than a single reading.

`[UNCERTAIN U-9076]` These are pixel extents of **visible ink, not emitter arguments** —
a sprite with transparent margin measures smaller than its quad, so the rects may be
tighter than the original's actual quads. ~~The icon, bar frame and circle art is not
ported; the coloured rects remain placeholders.~~ **That half is superseded by Findings
14-16**: the real `endpointpanel` TXD art IS now ported and the placeholders are gone
(they survive only as an asset-missing fallback). The measurement caveat stands, because
the port still takes its LAYOUT from these pixel extents rather than from emitter args.

**Verification** (`re/tools/hud_rows_check.py`, emitted draws scaled x0.8 back
into the original's space):

| emitted, scaled to 640 | measured |
|---|---|
| icon box `x=33.00 w=42.00 h=48.00` | `x=33 w=42 h=48` |
| row centres `107.00 / 160.00 / 213.00 / 267.00` | `107 / 160 / 213 / 267` |
| bar segments `x=87.00 h=17.00` | `x=87 h=17` |

Exact to the pixel. A first attempt used a uniform pitch of `53.333`, but the
measured spacing is `53, 53, 54` — not constant — which drifted the two middle
rows by 0.67px; the four centres are now used verbatim.

Score segments also now subdivide the measured 93-wide frame (`93/12` per point)
instead of marching at a fixed 14px pitch, so a full 12-point bar ends where the
original's frame ends instead of running past it.

**The draw-list diff cannot reward this fix** — the original's equivalents are on
the pipeline slots, so these rows stay `EXTRA` no matter how faithfully they are
placed. `extra` did drop 66 -> 42 across 6 frames, but only because the chosen
frame has fewer accumulated score segments; that is not a parity signal. The real
evidence is the numeric table above plus
`verify/race_hud/re_rows_t3.png`, where the rows sit over the 3D view in four
bands and no longer collide with the title. Chrome remains 4/4 matched
(`verify/race_hud/diff_standings_after_rows_20260902.txt`:
`match=24 mismatch=0 missing=6 extra=42`).

## Finding 11: the prompt glyph is IN the string (0x81), not a separate draw

The original's prompt line is **not** `" Continue"`. Reading the captured
`str_raw` hex as UTF-16LE code units instead of a rendered string:

```
81 20 43 6f 6e 74 69 6e 75 65   ->   L"\x81 Continue"
```

`0x81` is the remapped **"Select" nav glyph** — the `0x7f..0x8f` control range
documented at `DrawMashedString`'s per-glyph colour switch
(`exe_main.cpp:2515-2516`), where `0x7f` is the red BACK arrow and everything
else in range takes the caller's `ctrl_glyph_argb`. That is the green
circular-arrow prompt visible in `verify/race_hud/orig_drive_late.png`.

Two things follow, and both correct earlier notes:

1. **The glyph needs no new emitter.** It is a codepoint in the existing string,
   which is consistent with the text channel capturing exactly 3 strings per
   frame — there was never a 4th draw to find.
2. **`str_utf16` rendering is not safe for control codepoints.** `0x81` renders
   as an invisible/space-like character, which is how it got mistaken for a
   leading space in Finding 8. Always read `str_raw` code units when the exact
   bytes matter; the raw hex was captured precisely so this is recoverable.

Ported by setting the string to `L"\x81 Continue"` and passing
`ctrl_glyph_argb = 0xff10ec00`, the port's established prompt green
(`exe_main.cpp:3816`, `5139-5145`). `DrawMashedString` applies it per-glyph to
codepoints `>= 0x7f` only, so the word stays white.

**Verification** (`re/tools/hud_text_check.py`):

| line | glyphs | x_left | cell_h | colours |
|---|---|---|---|---|
| `"MASHED"` | 6 | 80.00 | 33.96 | `ffffffff` x6 |
| `"Current Standings"` | 17 | 80.00 | 25.50 | `ffffffff` x17 |
| `L"\x81 Continue"` | **10** | 80.00 | 25.50 | **`ff00ec10` x1** + `ffffffff` x9 |

The prompt line went 9 -> 10 glyphs, exactly one glyph carries the ctrl colour,
and its width is 34.00 vs ~15 for a letter — consistent with a circular arrow.
`ff00ec10` is `0xff10ec00` after the cited R<->B swap, which also confirms the
colour travelled the intended conversion path. The other two lines are
unaffected. Screenshot: `verify/race_hud/re_prompt_t3.png`.

## Finding 12: second scenario — chrome and text are scenario-invariant (and it caught a 2px error)

Second original capture: `--track 3 --cars 2` (vs `--track 1 --cars 4`), same
standings state, `--free-run`. Shape is identical: 5 quads and 3 strings per
frame, 30 frames.

**Original vs original, across scenarios** (`--scale-b 1`):
`match=24 mismatch=0 missing=6 extra=6` — the **4 chrome rows are bit-identical**
across track and car count, so the fixed constants in the port are correct rather
than a fit to one scenario.

The only divergence is the transparent quad that was deliberately not ported, and
it differs BETWEEN the two originals:

| scenario | quad | colour | caller |
|---|---|---|---|
| track1 / 4 cars | 512x512 @ (0,0) | `00000000` | `0x4271ed` |
| track3 / 2 cars | 640x480 @ (0,0) | `00dcdcff` | `0x42888d` |

Different size, colour AND caller, but **alpha `0x00` in both**, i.e. invisible
either way. That is independent support for omitting it: it is a scenario-varying
invisible overlay, not a stable element the port is missing.

**Standalone vs the scen2 original:** `match=24 mismatch=0` — the ported chrome
matches the second scenario exactly as it matched the first.

**Text:** same three strings with the same leading `0x81` prompt glyph, and the
fracs/scales are bit-identical between scenarios:

| string | fx | fy | scale | p5 |
|---|---|---|---|---|
| `"MASHED"` | 0.1000 | 0.8758 | 0.0566 | 0.800 |
| `"Current Standings"` | 0.1000 | 0.8433 | 0.0425 | 0.600 |
| `L"\x81 Continue"` | 0.1000 | **0.0933334082** | 0.0425 | 0.600 |

`p5` landing on exactly 0.800 / 0.600 confirms the `scale = p5 * 0.0708`
derivation in Finding 8.

**Error this check caught.** The port had the prompt line at `fy = 0.0900`,
taken from the ROUNDED preview output (`xy_f=(0.10,0.09)`) rather than the raw
float. The true value is `0.0933334082` (bits `0x3dbf2596`), so the line sat
~2px low. Fixed and re-verified: the prompt glyph row moved 534.80 -> 532.80 at
800x600, exactly the 2.0px anchor delta.

**Lesson:** read the raw float bits from the capture, not the formatted preview.
Same class of mistake as reading `str_utf16` instead of `str_raw` in Finding 11 —
both times a convenience rendering lost the precision that got baked into code.
A second scenario is what surfaced it, because the invariance check forced a
digit-for-digit comparison the single-scenario port never did.

## Finding 13: the icon/bar/circle renderer located — HANDOFF BRIEF (needs Ghidra)

The art layer was chased as far as this account can go. It is **not** in the
standings case that draws the chrome, and it **is** reachable through
`HudIngameDispatch`. Established without Ghidra (capstone raw-disasm + counters):

### The standings case does NOT draw the rows

Disassembling the enclosing function of the four `ChromeBaseDraw` calls
(one big switch, jump table at `0x0042a44c`) shows the state case emits, in
order: the 4 chrome quads, then **text only**, via `FUN_00427e00` with
**string-table IDs**:

| call site | string id | notes |
|---|---|---|
| `0x0042a36c` | `0x2d` | y = `480 - B - _DAT_005cd6d0`, size `0x42800000` (64.0) |
| `0x0042a393` | `0x41` | y = `B + _DAT_005cd120`, align `0x3f4ccccd` (0.8) |
| `0x0042a3bf` | `[0x008991b0]` | **dynamic** id, y = `B + _DAT_005cd6cc` |
| `0x0042a414` | `0xd1` | x=320.0, y=400.0, align 2 |

So `FUN_00427e00` is a string-table text draw, which is why the font thunk
receives already-widened UTF-16 buffers (the table lookup resolves the ID). This
also means the ported literals `L"MASHED"` etc. are the *resolved* strings, not
the original's mechanism — a faithful port would resolve IDs `0x2d`/`0x41`/`0xd1`
through the string table instead. Filed, not done.

No icon, bar-frame or circle draw appears anywhere in this case.

### Which function does draw them

`MASHED_COUNT_RVAS` over a 45 s run covering all states:

| RVA | calls | hooks.csv note |
|---|---|---|
| `0x0041b630` | 2268 | "{5/6}-path branch A" |
| `0x0041db80` | 2268 | post-switch tail |
| `0x0041ccc0` | **217** | "{7}-path branch A" |
| `0x0041e850`, `0x0041ded0`, `0x0041c300`, `0x0041a3e0`, `0x0041c0c0`, `0x0041d870`, `0x00403160`, `0x0041d410`, `0x0041e630` | 0 | — |

This independently corroborates the guard semantics of Finding 1: the `{5,6}`
branch and the `{7}` branch fire exactly in the states measured there.

Both branch entries are **thin wrappers** (12 insns to the first `ret`, one
direct call each):

```
0x0041ccc0  (state 7, the captured screen)  ->  0x0041c9a0
0x0041b630  (states 5/6)                    ->  0x0041b340
```

### What the handoff needs

**Targets:** `0x0041c9a0` (state-7 standings renderer) and `0x0041b340`
(states 5/6). Both are `C2 mapped` in `hooks.csv`, neither is ported.

**Why Ghidra:** these are full renderers, not leaf math — porting them verbatim
needs decompilation plus resolution of the texture/atlas handles for the car
icon, bar frame and point circle. Capstone raw-disasm carried the 5-arg chrome
calls and the string-IDs above, but it is the wrong instrument for a function of
this size. **Ghidra MCP is hard-blocked on this account** (account2; memory
`account2-mashed-workflow`), so this is an account3 task.

**Already measured, so the handoff should NOT re-derive it:**
- Row layout in 640-space: centres `y = 107 / 160 / 213 / 267`, icon box
  `x 33..74` (w=42, h=48), bar frame `x 87..179` (w=93, h=17), circle
  `x 185..246` diameter ~40 (Finding 10).
- These draws bypass Im2D and Im3D entirely; they land on the RW **pipeline**
  device slots `0x118` (`0x004cca80`), `0x11c` (`0x004ccba0`) and `0xe8`
  (`0x004af340`), ~11/11/8 per frame (Finding 4). A capture harness for them
  needs the RW pipeline structs, not a `(ptr, count)` stack pair.
- Reference frames: `verify/race_hud/orig_drive_late.bmp` and `orig_stand7.bmp`.

**Fallback if Ghidra MCP is unavailable on account3 too:** memory
`ghidra-mcp-down-use-analyzeheadless` records getting real PC decomp via
`analyzeHeadless` + `DecompPC.java` against a read-only pool slot. Not attempted
here.

## Harness caveat found while dumping

`--bbdump` must resolve to an **absolute** path. The shim resolves the request
file against the GAME's cwd (`original\`), so a relative path silently looks for
`original\<path>`, the request is never consumed, and the run reports a shim
failure that is actually a caller bug. Fixed by `.resolve()`.

## Finding 14: the icon/bar/circle layer is a RenderWare DFF+TXD, not procedural quads (Ghidra, account3)

Decompiled on account3 (Ghidra MCP available), pool slot `Mashed_pool14`, from the
targets the Finding 13 handoff located. Every RVA below was confirmed present via
`function_at`; every constant is read from the decompilation.

### The two renderers are generic widget-tree walkers, not art

`FUN_0041ccc0` (state-7 wrapper) is `MOV EAX,0x63ce20` then a loop calling
`FUN_0041c9a0` on each of **4 group objects** — base `0x0063ce20`, stride `0x114`,
end `0x0063d270` (`(0x63d270-0x63ce20)/0x114 = 4`). Its only caller is
`HudIngameDispatch 0x0040dfc0`. The 4 groups are the four standings rows.

`FUN_0041c9a0(this=EAX)` draws one group by walking its child-widget pointers and
calling each child's virtual method at **vtable `+0x48`** (the widget `Draw`):

- children at `this+0xe4` (gated by flag `this+0x64`), `this+0x88` (flag `+0x08`),
  `this+0x94` (flag `+0x14`), then an array of 32 slots at `this+0x80..0xfc`
  (each gated by a parallel enable flag at `this+0x00..0x7c`, minus a skip-set
  `{0,3,5,6,7,8,9,10,0x17}`), then `this+0x9c/0xa0/0xa4/0xa8/0xac/0xb0`.
- **The only literal colour in the renderer:** children `this+0x88` and `this+0x94`
  each get `*(*(*(child+0x18)+0x20)+4) = 0xff323232` (opaque grey 50/50/50)
  written every frame before their draw — a runtime material-colour override on the
  bar-frame background.

So the renderer contains **no rects, no UVs, no texture handles**. Those live in the
child widget objects, which is exactly why Finding 4 found this layer on the RW
**pipeline** device slots (`0x118`/`0x11c`/`0xe8`) and never on Im2D/Im3D: each child
is an `RpAtomic` and `+0x48` runs the standard atomic render pipeline.

`FUN_0041b340` (states 5/6) is the same shape with a fixed child set gated by a
bitmask at `this[0x1a]`.

### Where the art comes from — RESOLVED to concrete assets

`FUN_0041cb10` is the standings **init** (sibling walker over the same 4-group array):

```
DAT_0063cdd0 = FUN_0042a6b0("endpointpanel.txd", 0, 0);   // 0x0042a6b0 = RW TXD loader
FUN_004c5c80(0);                                           // render-state flag write (device+0x10 = 0)
clump        = FUN_0042a5d0("endpointpanel.dff", 0, 0);   // 0x0042a5d0 = RW DFF/clump loader
for (i = 0; i < 4; i++) {                                  // one clone per row
    if (i != 0) FUN_004e6ab0(clump);                       // RW clump/atomic clone
    FUN_0041c320();                                        // build one group from the clump
    (&DAT_0063cf28)[i * 0x45] = i;                         // store row index
}
```

`FUN_0041c320` enumerates **24 atomics** from the clump (`0x18` iterations), and for
each computes a slot index via `FUN_004b5190(atomic,0,0)` and stores the atomic
pointer at `group+0x80 + slot*4` — populating the child array `FUN_0041c9a0` walks.

**Assets (all in `original/TOASTART/Common/PANEL/Panel.piz`):**

- `ENDPOINTPANEL.DFF` — 29 frames, **24 geometries / 24 atomics**, 98 verts / 50 tris
  total (each atomic ≈ a 4-vert / 2-tri textured quad). One row's worth of sprites.
- `ENDPOINTPANEL.TXD` — 5 textures, all **64×64 PAL8** (custom flat TXD, root chunk
  `0x23`): `NewZero`, `NewPlusOne`, `NewPlusTwo`, `NewMinusOne`, `NewMinusTwo` — the
  **point circles** (0 / +1 / +2 / -1 / -2).
- The DFF materials also reference `NewPanelCrown` (128×128) and `OrangeDisplay`
  (128×64), which live in the sibling **`PANEL.TXD`** (the leader crown and the
  score-bar-frame display), plus non-raster named materials `NFLShadow`, `NFLPink`,
  `NFLRed`, `NFLBluejay`, `NFLMelon`, `NFLGold` — absent from both TXDs, so these are
  **flat material colours** (the shadow and the per-car colour swatches), not bitmaps.
  Texture binding is by name against already-resident dictionaries (`FUN_004c5c80`
  is a render-state write, not a current-TXD setter). PANEL.TXD is loaded by the
  in-race panel init, not by `FUN_0041cb10`.

~~`[UNCERTAIN]`~~ **RESOLVED later in this same session — do not file, do not act on the
text below.** This marker asked whether a per-character portrait raster is bound at
runtime onto one of the `NFL*` materials, and concluded on the then-available evidence
that the "car icon" was a flat colour swatch. **Both halves are wrong.** Finding 14's
asset table (below) locates six real `NFL*` badge bitmaps (128x128 PAL8) in
`SFX.piz :: INTERFACE.TXD`, and Finding 16 shows the badge is selected per-car by
Player Colour index via `1 << (colour & 0x1f)`. So no runtime portrait binding exists or
is needed: the variety comes from six pre-authored textures. The stale conclusion is kept
visible rather than deleted because it briefly justified the coloured-rect placeholder,
which Finding 16 then removed.

The superseded reasoning read: `FUN_0041cb10` binds no vehicle/character TXD,
and neither Panel TXD holds a portrait, so on the evidence the "car icon" is a flat
colour swatch (which matches the standalone's existing coloured-rect scaffold). The
mapping of the 24 atomics to specific screen elements was not enumerated per-geometry.

### Port verdict

The art is **not blocked** — every asset is identified and extractable. But there is
no procedural-quad recipe to port: the original draws this layer by rendering an RW
clump (24 `RpAtomic`s) in screen space through the pipeline device. Two routes:

1. **Faithful** — load `ENDPOINTPANEL.DFF` + both TXDs via librw, clone 4×, drive the
   per-atomic enable flags (which circle per car, crown on leader, grey bar override),
   render in the standalone's screen-space camera. This is a new subsystem (RW clump
   render-in-2D), not a HUD tweak.
2. **Pragmatic** — decode the 7 rasters (`NewPlusTwo/PlusOne/Zero/MinusOne/MinusTwo`,
   `NewPanelCrown`, `OrangeDisplay`) to RGBA and draw them as textured `HudIm2DQuad`s
   at the Finding-10 layout, keeping the flat colour swatches as the current rects.

**Neither route is verifiable by the draw-list diff** — this layer is on pipeline
slots on both sides, invisible to `drawlist_diff.py` (Finding 10 already established
this). Acceptance would be pixel/screenshot only. Because the choice is an
architecture-level decision with material consequences and no harness signal to
arbitrate it, it is left for the owner rather than picked unilaterally.

### The 24-atomic map (extracted from ENDPOINTPANEL.DFF, one row)

Parsed with `re/tools/dff_dump.py`'s `parse_clump` + `world_matrix` (RW3.6 clump,
frame transforms baked). Model-space X/Y (small, coplanar z≈0.05-0.07), material +
UV-subrect, per atomic. Every value is read from the DFF bytes.

| atomic | material | model X | UV subrect | element |
|---|---|---|---|---|
| a00 | `NFLShadow` | 0.02..0.11 | full | car-icon drop shadow |
| a01-a05 | `NFLPink/Red/Bluejay/Melon/Gold` | 0.02..0.11 | full | **car icon**, one per car colour (enable-gated) |
| a06,a07 | `NewPanelCrown` | -0.31..-0.24 | full | leader crown (×2) |
| a08-a12 | `NewZero/PlusOne/PlusTwo/MinusOne/MinusTwo` | -0.24..-0.16 | full | **point circle** set 1 (enable-gated) |
| a13-a17 | same 5 | -0.24..-0.16 | full | point circle set 2 (overlay) |
| a18,a19 | *(none)* | -0.14..0.01 | — | **grey bar backing** (baked `0xCCCCCC`, runtime-overridden to `0xff323232` — Finding 14 colour write) |
| a20-a23 | `OrangeDisplay` | -0.15..0.25 | sub-rects | **score bar fill** (4 textured pieces of the 128×64 display) |

Note the DFF's left→right model order (crown, circles, bar, icon) is **not** the
pixel-measured screen order (icon, bar, circle — Finding 10). Absolute screen
placement is applied by the RW screen-space camera per row and is **not in the DFF**,
so a port must take placement from the Finding-10 measured rects and art from the
DFF/TXD. All `NFL*` materials are white `(255,255,255,255)` = they modulate a
texture, so the car icon is a bitmap, not a flat colour swatch.

### Complete asset resolution (all textures located, nothing blocked)

| element | texture(s) | dims | archive :: TXD |
|---|---|---|---|
| car icon | `NFLPink` `NFLRed` `NFLBluejay` `NFLMelon` `NFLGold` (+`NFLShadow`) | 128×128 PAL8 | `Common/SFX.piz :: INTERFACE.TXD` |
| point circles | `NewZero` `NewPlusOne` `NewPlusTwo` `NewMinusOne` `NewMinusTwo` | 64×64 PAL8 | `Common/PANEL/Panel.piz :: ENDPOINTPANEL.TXD` |
| score bar | `OrangeDisplay` (textured) + grey backing quads | 128×64 PAL8 | `Common/PANEL/Panel.piz :: PANEL.TXD` |
| leader crown | `NewPanelCrown` | 128×128 PAL8 | `Common/PANEL/Panel.piz :: PANEL.TXD` |

### Port plan (approach B, faithful-via-Im2D)

Owner selected the "faithful" route; the architecture-fit realisation is B (the port
has no ortho-clump renderer — survey via account2 worker, cited below). Steps:

1. Open `Panel.piz` (new — loaded nowhere yet) and `SFX.piz`; pull the 4 TXD blobs.
2. Decode each needed texture (`Txd::Dictionary::Decode` → BGRA, pattern
   `TrackRenderer.cpp:390-416`) → `QuadRenderer::UploadBGRAToSlot` →
   `RwIm2DBridge_RegisterTexture(handle, tex)` (recipe `exe_main.cpp:5856-5889`).
3. In the standings block (`exe_main.cpp:3026+`), per car row at the Finding-10 rects
   (`kUiS` scale): draw `NFLShadow`+`NFL<colour>` icon, the grey bar backing at
   `0xff323232` + `OrangeDisplay` fill scaled by `g_track.score(i)`, the one point
   circle for the car's delta (`New{PlusTwo,PlusOne,Zero,MinusOne,MinusTwo}`), and
   `NewPanelCrown` on the leader — each via `HudIm2DQuad(<handle>, x,y,w,h, argb, uv)`.
4. Build wiring: pure-Im2D TU (no `<rw.h>`) → add to BOTH `build.bat` and
   `asi_sources.rsp`. Acceptance is screenshot-only (this layer is invisible to the
   draw-list diff), vs `verify/race_hud/orig_drive_late.bmp` / `orig_stand7.bmp`.

Standalone infrastructure confirmed by an account2-worker read-only survey
(`Piz::Archive` `Piz/PizReader.h:55`; `Txd::Dictionary::Decode` `Txd/TxdDecoder.h:138`;
`HudIm2DQuad` `exe_main.cpp:188`; `LoadPngAssetToSlot` recipe `exe_main.cpp:5856`;
no ortho-clump path exists — screen-space is Im2D-only).

## Finding 15: standings art PORTED (approach B) and screenshot-verified

Approach B (owner-approved 2026-09-02): draw the real `endpointpanel` textures
through the existing Im2D screen-space primitive `HudIm2DQuad`, on the Finding-10
measured layout. No ortho-clump subsystem (the port has none, and 24 coplanar
quads make one pixel-pointless). All changes are additive, inside `exe_main.cpp`
+ a one-line `QuadRenderer.h` cap bump — no new TU, so no `build.bat` /
`asi_sources.rsp` change.

### What landed

- **`QuadRenderer.h`**: `kMaxSlots` 80 -> 96 (slots 80..92 for 13 standings
  textures; without the bump every `UploadFromTextureToSlot` silently returns
  false, the documented 64->80 overflow class).
- **`exe_main.cpp`** — `kSlotStand0=80` / `kHandleStand0=71` block + a
  `LoadStandTexList` helper (mirrors `LoadPowerupIcons`: `Piz::Archive` +
  `Txd::Dictionary::Decode` + `UploadFromTextureToSlot` + `RwIm2DBridge_
  RegisterTexture` — the decode->bridge path already existed, no new plumbing).
  `LoadStandingsAssets()` loads all 13, wired at frontend init after
  `LoadPowerupIcons()`. Log confirms `iface=6/6 epp=5/5 panel=2/2 ready=1`.
- The standings per-car loop now draws, per row on the measured rects: car badge
  (`NFLShadow` + `NFL<colour>`), grey `0xff323232` bar backing + `OrangeDisplay`
  fill scaled by score, and the `New{PlusTwo/PlusOne/Zero/MinusOne/MinusTwo}`
  circle for `score_delta`. Placeholder rects survive only as an asset-missing
  fallback (never a silent blank).

### Two bugs found and fixed during verification

1. **Rows drew every round-mode frame, including while driving** (inherited from
   the scaffold). The original renders the endpointpanel widget groups only in
   states {5,6,7} (`HudIngameDispatch`), never state 3. With real badges this
   over-drew the live driving view. Fixed by gating the row loop on the same
   `standings` flag as the chrome. Confirmed: `re_drive_finding14.png` shows the
   race with only the countdown, no rows.
2. **The crown drew every round**; the per-round reference
   (`orig_drive_late.bmp`) has none. Gated to the match-end screen
   (`match_winner() >= 0`) and flagged `[UNCERTAIN U-9071]` (no reference for that
   screen; the enable flag lives in the unreversed group update `FUN_0041c410`).

### Verification

- **Chrome regression (the only diffable layer) is intact.** Pinning matched
  standings frames (`drawlist_diff.py --label-a f2019.. --label-b f778,f1200,
  f1300 --scale-b 0.8 --exclude-tex 9`): **matched 4, mismatched 0** every frame.
  `missing 1` = the transparent 512^2 quad (deliberately unported); `extra 18` =
  the new textured rows, invisible to the original's Im2D stream as established
  in Findings 4/10. The default file-order pairing reports match=0 because it
  pairs the original's standings frames against the standalone's *driving*
  frames (f700..) — a harness frame-selection artifact, not a regression.
- **Screenshots** (800x600, `MASHED_TRACK_VIEW=1 MASHED_CAR=1 MASHED_ROUND=1
  MASHED_RESULT_DEMO=1`): `verify/race_hud/re_stand_finding14.png` (+ `_late`)
  vs `orig_drive_late.bmp` — real badges, orange point circles, grey+orange bars,
  letterbox, `Continue` prompt, all on the measured layout. `re_drive_finding14.
  png` confirms no HUD over the driving view.

### Known divergences (flagged in code, NOT silently papered over)

1. **Car-icon colour per row is a placeholder.** The original shows each car's
   character badge; in this scenario all four cars are the same (all red devil in
   `orig_drive_late.bmp`), while the port draws four different badges by row
   index. Root cause: the race scene carries **no per-car character/colour**
   (`RaceSceneState::RaceCar`), and the original's source is unreversed
   (`DAT_007f1a1c`, `TrackRenderer.h:104`). The **art is real**; only the
   which-badge-per-row binding is provisional. A faithful fix needs per-car
   character threaded into `RaceSceneState` (or `DAT_007f1a1c` reversed). Left as
   an obvious placeholder rather than fitted to one scenario (all-red), per the
   evidence-discipline rule.
2. **Score-bar fill** `[UNCERTAIN U-9072]` is a single `OrangeDisplay` quad stretched
   over the score fraction, not the original's exact 4-UV-sub-rect composition (atomics
   a20-a23). Real art, growing with score; not the exact tiling.
3. **Crown trigger/position** `[UNCERTAIN U-9071]` unreversed (see fix #2 above).

## Finding 16: DAT_007f1a1c reversed = per-car Player Colour; badge bound to it

Owner-approved faithful route: reverse the real source and bind to it rather than
thread a parallel character field. Done on account3 (Ghidra), pool slot
`Mashed_pool14`. Every RVA confirmed via MCP.

### What DAT_007f1a1c is

`DAT_007f1a1c` is one element of a **per-car array**: base `0x007f1a14`, **stride
`0x10`** (4 dwords/car), the value field at **+8**, four cars. Car *i*'s value is
`*(0x007f1a1c + i*0x10)`. Proven by two independent witnesses that iterate it as a
4-entry per-car array:

- **Writer** `FUN_0042b9e0` (default assignment, `0x0042baf8`): `piVar2 =
  &DAT_007f1a1c; ... *piVar2 = iVar4-1; piVar2 += 4 (0x10 bytes)`, 4 slots — writes
  a value `0..5`.
- **Reader** `FUN_0040d040` (`0x0040d0c0`): `piVar5 = &DAT_007f1a1c; ... piVar5 +=
  4; while (piVar5 <= 0x7f1a5b)` — 4 iterations.
- **Real car-select writer**: `FUN_0043dfd0` (`0x0043e575` / `0x0043e57e` /
  `0x0043f8d5`).

The value is the **Player Colour index 0..5**. Two badge emitters select the car
badge atomic directly from it, **per car**:

- `FUN_0041adb0` (`0x0041adcd`): `*(this+0x68) = 1 << ((&DAT_007f1a1c)[car*4] &
  0x1f)` — enables the badge atomic for the car's colour. (Also writes the
  `0xff323232` grey bar override via `FUN_004b5260`, matching Finding 14.)
- `FUN_0041cdb0` (`0x0041cdd6`): packs several cars' badges, `colour + i*6` per car
  (6-colour blocks).

`[car*4]` on a `stride-0x10` array is per-car, so the badge is **per-car colour,
not player-0** — and the scalar-`DAT_007f1a1c` draw `FUN_0041de80` is dead in-race
(its only caller `FUN_0041ded0` counted 0, Finding 13), so it is not the standings
path.

### The colour -> name map (already in-tree, cross-confirmed)

The port already reversed this table for the audio character banks (`DAT_006041f0`,
stride `0x80`, `AudioEngine.h:62`, `AudioCharacterBankPaths.cpp:54`):

```
0=RED  1=BLUEJAY  2=MELON  3=GOLD  4=PINK  5=SHADOW
```

Badge texture = `NFL<Name>` (all six strings present `0x005cd864..0x005cd898`).
This resolves the reference: `orig_drive_late.bmp`'s four red-devil badges are
colour 0 = RED = `NFLRed` — i.e. all four demo cars are colour 0, which is why they
are identical. The mapping is by colour **name**, not DFF-atomic order (which is
Pink,Red,Bluejay,Melon,Gold) — the all-RED reference confirms colour 0 -> Red.

### Binding implemented

- `RaceSceneState.h`: `int colour_[kRaceCars]` — mirrors the original's per-car
  `0x007f1a14` colour (what the original carries, not a derived duplicate); default
  0 = RED. `TrackRenderer::car_colour(i)` / `SetCarColour(i,c)`.
- `RaceSession::Begin`: `SetCarColour(i, m_cfg.cars[i].colour)` after `StartMatch` —
  the config's `cars[i].colour` IS the standalone's copy of `DAT_007f1a1c`.
- HUD badge: `badge = kColourBadge[car_colour(i)]`, `kColourBadge =
  {Red,Bluejay,Melon,Gold,Pink,Shadow}` — replaces the Finding-15 row-index
  placeholder.
- R6 demo (`MASHED_ROUND`) sets no colour -> all default 0 -> all RED (matches the
  reference). `MASHED_ROUND_COLOURS="a,b,c,d"` pokes distinct per-car colours for
  non-degenerate verification (the real source the original writes via car-select).

### Verification (non-degenerate, per the standing rule)

The degenerate trap (all-same input yields all-same badge under any mapping) is
avoided by poking distinct colours:

- **`MASHED_ROUND_COLOURS=0,1,2,3`** -> four **distinct, correct** badges: RED
  devil / BLUEJAY blue-J / MELON green / GOLD eagle
  (`verify/race_hud/re_stand_finding16_colours.png`). Distinct colours produce
  distinct badges matching the map — the binding is proven, not degenerate.
- **Default (no poke)** -> all four RED, reproducing `orig_drive_late.bmp`
  (`verify/race_hud/re_stand_finding16_default.png`).
- **Chrome regression intact**: `matched 4, mismatched 0` on every pinned standings
  frame after the change.

`[UNCERTAIN U-9070]` The endpointpanel DFF has 5 badge atomics (Pink,Red,Bluejay,Melon,
Gold), no Shadow badge, so colour 5 (SHADOW) has no dedicated atomic in that panel;
the port maps colour 5 -> `NFLShadow` (loaded), which the original endpointpanel may
render differently. Edge case (SHADOW character in a round), not exercised by the
demo. Also, the standalone R6 demo cannot itself produce distinct colours (its cars
have no colour source); distinct-colour verification is via the deliberate poke or
the nav car-select flow.

## Finding 17: standings chrome ANIMATES (5-frame slide, no fade) — U-9075 resolved

U-9075 asked what animates the standings chrome on entry (band alpha from `BL`,
band offset `B = DAT_008991b4`). Reversed on account3 (Ghidra, `Mashed_pool14`) and
verified against a state the diff had never covered.

### What animates: a POSITION slide, not an alpha fade

The chrome draw is `FUN_00429e10`. The band **alpha is a constant `0xff000000`
literal** in every `ChromeBaseDraw`/`FUN_00472c60` argument — the Finding-14/7
"[UNCERTAIN] alpha from BL" was **wrong**; there is no fade. The only variable is the
band offset `B = DAT_008991b4`, which positions all four elements (top band `y=B`,
bottom `480-B-64`, rules `B+80` and `480-B-64`):

- **Init** `FUN_00429b70` (once, `DAT_008991b0==0`, submode ∈ {3,4,5}):
  `DAT_008991b0=0xeb; DAT_008991b8=0; B = -(_DAT_005cd6c8 / tick)`, `_DAT_005cd6c8 =
  48064.0`, `tick = DAT_0067ea56` — B starts **negative** (bands off-screen).
- **Per-frame** `FUN_00429310`: `if (B != DAT_005d757c) { B += tick*_DAT_005cc9a4;
  if (DAT_005d757c <= B) B = 0.0; }`, `_DAT_005cc9a4 = 0.025`, `DAT_005d757c = 0.0`
  — B ramps **up to 0** and clamps, so the bands slide in from the edges. The
  "Continue" prompt (`0xd1`) is gated on `B == 0.0`, i.e. only after settling.
- **Teardown** `FUN_00429820`: `DAT_008991b0=0; B=0`.

### Measured (the degenerate trap avoided)

Settled frames show `B=0` in every state, so a settled-only sample would wrongly read
"constant". Captured the **transition** instead (`race_hud_burst.py --free-run` across
entry, every draw tagged with its true Present-counter frame index). Top-band y over
f1681..f1686 (800-space): **`-81.25, -62.5, -43.75, -25.0, -6.25, 0.0`**, alpha
`0xff` throughout — a 5-frame linear slide (+18.75/frame), then hold.

Per-state (guard-gated captures): **state 5 draws no chrome** (360 frames, 0 bands);
**states 6 and 7 both carry the identical slide** then settle (guard-6: slide
f1954..f1958 `-81.25..-6.25`, settled f1959+). So the slide is the entry into the
standings/results chrome, state 5 is the pre-chrome beat.

### Ported and verified

`exe_main.cpp` now ramps `band_off` from **-65 to 0 over 5 frames** (+15/frame in the
640-space input; `ChromeBaseDraw` scales ×1.25, so it emits the measured
`-81.25..0`), reset on the `standings` rising edge. Alpha stays `0xff` (unchanged).

- **Ramp is byte-identical to the original.** Standalone drawstream top-band y over
  f1141..f1146: `-81.25, -62.5, -43.75, -25.0, -6.25, 0.0`, alpha `0xff` — the same
  six values as the measured original.
- **Per-frame diff vs original guard-6 (slide + settled), matching 800-space scale:
  matched 4, mismatched 0 on all six frames** — the ported chrome matches the
  original through the animation, not just at rest (`verify/race_hud/
  orig_guard6_frames.json` is the new committed guard-6 reference).
- Settled diff vs the committed guard-7 reference: still **matched 4/0**.
- **Coverage extended from state 7 to states 5/6/7** — the gap U-9075 point 2
  flagged (5/6 never diffed) is closed: 5 = no chrome both sides, 6 = slide+settled
  4/0, 7 = settled 4/0.

`missing 1` = the transparent 512² quad (deliberately unported); `extra 18` = the
standings badge/circle/bar rows (the RW-pipeline layer, invisible to the original's
Im2D stream). Both pre-existing.

**Disposition: U-9075 RESOLVED.** The chrome animates as a 5-frame position slide
(no fade); modelled and verified per-frame against the original in states 6 and 7.

## Finding 18: score-bar fill is a DARK-cell fill over an OrangeDisplay frame — U-9072 resolved

U-9072 asked the score-bar tiling: the exact sub-rects, cells-per-point, and
whether the fill is discrete or continuous. Reversed (Ghidra `Mashed_pool14`) and
measured; the Finding-15 port had the layering **backwards**.

### The rule

The bar is an **OrangeDisplay orange frame (static, full width)** with **DARK
`0xff323232` fill cells drawn left-to-right, one per point**. The `0xff323232` is the
fill (the Finding-14 "grey override"), OrangeDisplay is the frame — Finding 15 drew a
grey backing + orange growing fill, which is inverted.

Mechanism (`FUN_0041c410`): the fill atoms `group+0x88`/`group+0x94` are X-scaled by
`score/max` — `FUN_004c13e0(atom, {score/max, 1, 1})` → `FUN_004c5010(atom+0x10, …)`,
an RwFrame matrix scale. `score = (&DAT_008a94e0)[car]` (`FUN_0040b6d0`); `max = 8 or
12` (`FUN_0040b890`, `0xc` default). It is a **continuous** X-scale of a
cell-patterned dark element; with an integer score over a 12-slot frame it yields
exactly `score` visible dark cells (so it reads as discrete). A second atom pair
`group+0x98`/`group+0x8c` gets a `fsin` pulse scale when bit `0x40` is set — a
recent-change highlight, separate from the fill.

`OrangeDisplay` (128×64, `PANEL.TXD`) holds **two stacked bar graphics** (metallic
frame + orange interior + a dashed empty-track line). The DFF maps sub-rects
(a22 top-half u 0.178..1.0 v 0..0.5; a23 the dark left cap u 0..0.178; a20 the thin
divider strip v 0.13..0.17). The port uses the a22 top-half sub-rect for the frame
(a single quad) rather than the exact 4-piece composition.

### Cell geometry — MEASURED from `orig_drive_late.bmp` (640-space)

First cell **x=90** (= bar-frame `kBarX`=87 + 3), **pitch 7**, dark width **6**, ~8 px
tall centred on the row. The reference's four rows read **8 / 7 / 5 / 4** dark cells
= their scores.

### Ported

`exe_main.cpp` draws the OrangeDisplay frame full-width (a22 UV sub-rect), then
`min(score,12)` dark `0xff323232` cells at x0=90, pitch 7, width 6, h 8 (×`kUiS`).
max hardcoded 12 (default; `FUN_0040b890` returns 8 in some modes — flagged residual).

### Verification (non-degenerate — the degenerate trap avoided a 3rd time)

A tiling difference is invisible at 0/full or when all rows are equal, so distinct
known mid-range scores were poked: **`MASHED_ROUND_SCORES=8,7,5,4`** (display-only,
analogous to `MASHED_ROUND_COLOURS`).

- **Numeric (standalone drawstream):** the emitted dark cells are **8 / 7 / 5 / 4**
  per row, all at **x0=90.00, pitch 7.0** (640-space) — exactly the measured original
  geometry. (Verified via `drawlist_diff.normalize` on the `0xff323232` quads.)
- **Colour:** standalone orange `(247,148,29)` ≈ reference `(248,152,40)`.
- **Pixel (imgdiff, per-bar crop):** mean-abs-diff ~58–70 all-channel. This is
  cross-renderer noise, **not** a structural mismatch: the orange colour matches and
  the cells align; the residual is the dashed-track pattern phase and sub-pixel
  alignment between librw and the original RW render (a whole-frame or even bar-crop
  imgdiff can't go to zero across two renderers). The structural gates (cell count +
  geometry + colour) are the acceptance; imgdiff is corroborating only.
- **Chrome regression:** `matched 4 / mismatched 0` on all slide + settled frames vs
  `orig_guard6_frames.json` — the bar change did not disturb the chrome.
- Screenshot: `verify/race_hud/re_stand_finding18_scores8754.png` — orange frames with
  8/7/5/4 dark cells filling from the left, matching `orig_drive_late.bmp`.

**Disposition: U-9072 RESOLVED.** Tiling rule reversed + measured + ported + verified
non-degenerately. Minor flagged residuals: the frame is a single-UV approximation of
the a20–a23 4-piece composition (only the empty-track dashes differ), and `max` is
hardcoded 12 (8 in some modes).

## Finding 19: U-9072 residuals — max binding done (B), 4-piece frame falsified (A)

### Residual B — max = 8 vs 12: RESOLVED and bound

`FUN_0040b890` (@`0x0040b890`) returns the bar's max: `max = 12` iff
`FUN_0040e340()==4` **and** `FUN_0042f500()==0` **and** `DAT_007f0fd0 ∉ {1,2}`; else
`8`. The leaves are simple getters: `FUN_0040e340 = DAT_008a94d0` = **participant
count** (`= 4` for the standalone round — `TrackRenderer.cpp:3501/3675`);
`FUN_0042f500 = DAT_0067ea64` = a setup flag written by the car-select family
(`FUN_0043dfd0`/`FUN_0043f*`); `DAT_007f0fd0` = race rule. So a 4-player race at rule
0 → max 12; rule 1/2 (or ≠4 players, or the flag) → max 8.

**Frame-slot finding (the user's question):** the `OrangeDisplay` frame is a **fixed
12-slot** dash pattern, and the dark fill scales *continuously* by `score/max`
(Finding 18). So the visible cells = `round(score/max · 12)` — **NOT 1 cell/point**.
The clean 1:1 seen at the default is a **max=12 coincidence**; at max=8, score 4 →
`round(4/8·12)=6` cells. Ported: `exe_main.cpp` binds `max` from `race_rule()`
(rule 1/2 → 8, else 12; participant count is 4 in the demo and `DAT_0067ea64` is
unmodelled) and draws `round(score/max·12)` cells.

**Observable verification** (`MASHED_ROUND_RULE` test hook added, like the score/colour
pokes), scores poked to 8,7,5,4:

| mode | max | emitted cells |
|---|---|---|
| rule 0 | 12 | **8 / 7 / 5 / 4** (unchanged; default preserved) |
| rule 1 | 8 | **12 / 11 / 8 / 6** (= `round(score/8·12)`) |

Cell counts differ in the predicted direction (more cells at max=8) — the mode→max
binding and the non-1:1 mapping are both confirmed.

### Residual A — 4-piece frame: ATTEMPTED, FALSIFIED, left open

Falsifiable gate (stated up front by the owner): if the imgdiff residue were dash
phase, composing the real 4 pieces should DROP the per-bar imgdiff. It did the
opposite. **Explicit before/after** (identical crop/scores 8,7,5,4/frame selection,
per-bar all-channel mean abs diff):

| row | single-UV (baseline) | 4-piece (linear map) |
|---|---|---|
| 0 | 58.62 | 79.25 |
| 1 | 58.50 | 81.80 |
| 2 | 64.61 | 90.71 |
| 3 | 70.12 | 93.69 |

The 4-piece **raised** the diff by ~20–24 per row. Cause: the a20–a23 UVs are known
(Finding 14) but their **screen placement is not in the DFF** — only relative model-X
extents are, and the model→screen transform is the RW camera (the same limit Finding
14 hit for the row layout). The only DFF-derivable mapping (linear: model span
−0.1533..0.2499 → bar rect) puts the a23 **dark end cap over the right 57%** of the
bar (verified visually — a large dark region, nothing like the mostly-orange
reference). So the composition is not cleanly portable, and the single-UV a22 frame
is retained (it reproduces the reference visual; the numeric cell gate is the
acceptance).

**Two attributions corrected by this:** (1) the residue is **not** dash phase — the
imgdiff region grid is roughly uniform across the whole bar (row0 columns
58/48/54/55/70/66) with 77% of pixels over threshold, i.e. a uniform
render/alignment difference between librw and the original RW, not a frame-UV
artefact; (2) a DFF-faithful change is only worth landing if the derivation is
sound — here the piece placement is a guess that the metric rejects, so it is **not**
landed. Residual A stays open (UVs known, per-piece screen placement unreversed) —
filed as U-9077.

Chrome regression after both changes: `matched 4 / mismatched 0` on all slide +
settled frames vs `orig_guard6_frames.json`.

## State of the standings port (end of this lane)

**Faithful and verified** (draw-list diff or measured/observable gate):
- Letterbox chrome — 4 rows, positions, colour, and the **5-frame entry slide** —
  `matched 4/0` vs the original across states 5/6/7 (Findings 7, 17).
- The three text strings + `\x81` prompt glyph — measured fracs, diff-matched
  (Findings 8, 11, 12).
- Car badge **per Player Colour** (`DAT_007f1a1c`) — non-degenerately verified with
  distinct colours → distinct correct badges (Finding 16).
- Point circles by `score_delta`; score-bar **dark cells = round(score/max·12)** over
  the OrangeDisplay frame — cell counts + geometry match the measured original, and
  the max=8/12 binding is observably correct (Findings 18, 19).

**Approximate (flagged):**
- Score-bar **frame** is a single OrangeDisplay UV sub-rect, not the real a20–a23
  4-piece composition (U-9077 — screen placement unreversed).
- `DAT_0067ea64` (a max=8 determinant) and the participant-count determinant are not
  modelled; the port binds only the reachable `race_rule` leaf.
- Cross-renderer pixel parity of the bar is ~58–70 mean abs diff (uniform
  render/alignment difference; not closable across librw vs original RW).

**What a future session would need to finish it:**
- Reverse the endpointpanel widget's per-atom **screen frame transforms** (the RW
  camera placement of a20–a23 and the badge/circle atoms) to render the frame as the
  true 4-piece composition and to derive absolute placement instead of the
  pixel-measured Finding-10 layout. This is the one remaining "needs the RW pipeline
  structs" item from Finding 4/13.
- Model `DAT_008a94d0` (participants) and `DAT_0067ea64` for the full max law.
- Optionally, verbatim-render the endpointpanel clump (approach A from the 2026-09-02
  handoff) if pixel-exact frame parity is ever required — but the Im2D path is
  visually faithful and is the shipped choice.

## Not done / next

1. Recover the icons / score bars / point circles. They are on the RW **pipeline**
   device slots (`0x118`/`0x11c`/`0xe8`), not the immediate-mode pipes, so this
   is materially harder than the Im2D/Im3D channels: the vertex data lives in
   RW-internal pipeline buffers rather than in a caller-supplied array, so there
   is no `(ptr, count)` pair to read off the stack. Needs the RW pipeline structs
   and identification of `0x004e3d30` / `0x004e3c89` / `0x00484394`.
   **Needs Ghidra**, unavailable on this account (memory
   `account2-mashed-workflow`: Ghidra MCP is the hard blocker).
2. ~~Frame boundaries.~~ **RESOLVED** — see Finding 5.
2. Standalone side: `MASHED_DBG_DRAWSTREAM` is not frontend-gated
   (`RwIm2DBridge.cpp:131-132`, frame tick `exe_main.cpp:3145`), so it can
   capture the in-race scaffold HUD. No standalone capture was taken this
   session, so **no diff has been run yet** — the original-side reference exists,
   its counterpart does not.
3. The standalone's text (`DrawMashedString`) shares the original's blind spot
   on the `+0x30` slot, so a text diff needs the standalone mirrored into the
   drawstream (precedent: the menu video quad's tex sentinel `-1`).

## Finding 20: the standings camera reversed; U-9077 RESOLVED (the 4-piece premise was false)

Ghidra work done via an account3 MCP-proxy child session (pool slot `Mashed_pool14`);
every RVA below was returned from decompilation. The DFF arithmetic was done locally
against the real bytes (`re/tools/dff_dump.py` `parse_clump` + `world_matrix` over
`Panel.piz :: ENDPOINTPANEL.DFF`).

### The camera (this is the piece Finding 4/13/14 kept hitting)

`FUN_00492e90` wraps the standings dispatch:

```
cam = FUN_004671c0()            // returns DAT_006905b4
FUN_0040de30(cam)               // camera setup, below
FUN_004c1a00(cam)               // begin update
  vtable(DAT_007d3ff8+0x20)(6,0); vtable(...)(8,0)
  FUN_0040dfc0()                // HudIngameDispatch
  vtable(...)(6,1); vtable(...)(8,1)
FUN_004c19f0(cam)               // end update
```

`FUN_0040de30` @`0x0040de30` saves the camera's existing frame matrix / view window /
projection, then every frame writes:

| what | via | value |
|---|---|---|
| camera frame matrix `frame+0x10` | `FUN_004c1480` @`0x004c1480` | **identity** (diagonals `0x3f800000` = 1.0f) |
| view window `cam+0x68` / `cam+0x6c` | `FUN_004c1c80` @`0x004c1c80` | `0x3f19999a` = **0.6f**, `0x3ee66666` = **0.45f** (consts at `0x0040de5a`/`0x0040de5e`) |
| projection `cam+0x14` | `FUN_004c1c10` @`0x004c1c10` | **2** = parallel/orthographic |

So the model→screen map for this layer, at 640×480, is exactly:

```
screen_x = (world_x / 0.6  + 1.0) * 320      // 533.33 px per world unit
screen_y = (1.0 - world_y / 0.45) * 240
```

**Independently confirmed, not fitted:** projecting the DFF's own baked world extents
gives the `OrangeDisplay` bar quad at **93.1 px** wide against a pixel-measured bar
width of **93 px** (0.1%), the point circle at 41.2 px against a measured ink diameter
of ~40, and the crown at 33.3 px. Sizes come out right without any free parameter.

### U-9077 RESOLVED — a20-a23 are not four pieces of one bar

Projected rects and UV sub-rects for the four `OrangeDisplay` atomics, one row:

| atomic | screen w × h | UV (u, v) | texels of the 128×64 | verts/tris |
|---|---|---|---|---|
| a20 | 90.8 × 21.2 | u 0.200..0.964, v 0.130..0.171 | 97.8 × 2.7 | 4 / 2 |
| a21 | 93.1 × 24.1 | u 0.178..1.000, v **0.500..1.000** | 105.2 × 32 | 4 / 2 |
| a22 | 93.1 × 24.1 | u 0.178..1.000, v **0.000..0.500** | 105.2 × 32 | 4 / 2 |
| a23 | 122.0 × 24.1 | u 0.000..0.178, v 0.000..0.500 | 22.8 × 32 | **6 / 4** |

- **a21 and a22 are coincident** — identical screen rects, complementary *half*-UVs of
  the same texture. They are two alternate states of one element, drawn one at a time
  under the enable flags, not two pieces of a composition.
- **a23 sits entirely to the right of the bar** (projected x 331.3..453.3 vs the bar's
  238.2..331.3), is 5.4 px per texel where a22 is 0.885, and is a 6-vert / 4-tri mesh,
  not a quad. It is not part of the bar.
- a20 is a thin inset strip.

This is the quantitative cause of the Finding-19 falsification: linear-mapping all four
across the bar rect forced a23 (an off-bar element) into the right 57%.

**Verdict: the shipped single-UV a22 frame is DFF-exact**, in UV and in width to 0.1%.
U-9077 is resolved as a false premise, not as an unreachable target. No code change is
needed; `exe_main.cpp` comment corrected.

### The runtime placement transform — REVERSED

The DFF's *authored* positions are not the screen positions, but the gap is fully
accounted for by two things.

**1. The X axis is mirrored, exactly like Y.** The correct map is

```
screen_x = (1.0 - world_x / 0.6 ) * 320
screen_y = (1.0 - world_y / 0.45) * 240
```

not `(wx/0.6 + 1)*320`. With the `+1` form every element lands off the artwork; with the
mirrored form the icon and bar land on it (below). This also explains the apparent
"DFF order is crown/circle/bar/icon, screen order is icon/bar/circle" reversal noted
earlier in this session — there is no reversal, the X axis was being read with the wrong
sign. The DFF carries **no rotation at all**: all 29 frames are identity rotation with
translation only.

**2. `FUN_0041c410` @`0x0041c410` places each row.** It is dispatched per frame, once
per enabled group, by `FUN_0041cc50` @`0x0041cc50` (`ESI` = group ptr; the group's own
`+0x108` player index drives the lookup via a linear search over `DAT_0063cdf8`). It:

```
FUN_004c51a0(&m, &DAT_005f337c + row*3, 0);   // RwMatrixTranslate
FUN_004c1480(*(group+0x104), &m, 0);          // RwFrameTransform(rootFrame, m, REPLACE)
scale = (1,1,1) * _DAT_005cd118;
FUN_004c13e0(*(group+0x104), &scale, 1);      // RwFrameScale(rootFrame, ..., PRECONCAT)
```

Because the transform **replaces** the root frame matrix, the DFF's authored root
translation (`f00.pos = 0.05, 0.0, 0.075`) is discarded at runtime.

**`DAT_005f337c` — 4 entries × 3 floats, stride 12, read from `.data` (file offset
`0x1f337c`), raw bytes:**

| row | bytes | X | Y | Z |
|---|---|---|---|---|
| 0 | `8f c2 f5 3e  00 00 80 3e  00 00 80 3f` | 0.48 | 0.25 | 1.0 |
| 1 | `8f c2 f5 3e  9a 99 19 3e  00 00 80 3f` | 0.48 | 0.15 | 1.0 |
| 2 | `8f c2 f5 3e  cd cc 4c 3d  00 00 80 3f` | 0.48 | 0.05 | 1.0 |
| 3 | `8f c2 f5 3e  cd cc 4c bd  00 00 80 3f` | 0.48 | −0.05 | 1.0 |

The table is **not a static constant** — it is populated at runtime by
`FUN_0041cbc0` @`0x0041cbc0` (body `0x0041cbc0..0x0041cc44`, sole caller
`FUN_004111c0` @`0x004111c0`), which builds the twelve dwords from inline immediates on
its stack and block-copies them into `DAT_005f337c`, then zeroes `DAT_0063d270` (the
crown-pulse tick). Verified in the binary at file offset `0x1cbc0`:

```
83 ec 30              SUB ESP,0x30
56 57                 PUSH ESI / PUSH EDI
b9 0c 00 00 00        MOV ECX,0xc              ; 12 dwords
8d 74 24 08           LEA ESI,[ESP+8]          ; src = stack array
bf 7c 33 5f 00        MOV EDI,0x005f337c       ; dst = the table
c7 44 24 08 8f c2 f5 3e   MOV [ESP+0x08],0.48
c7 44 24 0c 00 00 80 3e   MOV [ESP+0x0c],0.25
c7 44 24 10 00 00 80 3f   MOV [ESP+0x10],1.0
... (rows 1-3 identically) ...
f3 a5                 REP MOVSD
c7 05 70 d2 63 00 00 00 00 00   MOV [0x0063d270],0
```

So `FUN_0041cbc0` is the only known writer of the row positions, and
`FUN_0041c410` the only reader.

### Result: the row layout is derived, not measured — residual closed

The root scale is **`_DAT_005cd118` = 1.125** (`.rdata`, raw `00 00 90 3f`), read from the
binary. `FUN_0041c410` scales the group root frame by it uniformly *after* the row
translate, with the scale applying to the clump's local coordinates and **not** to the
row translation (the row centres are unaffected by `k`, which is why they matched
exactly before `k` was known). The complete map, at 640x480:

```
k        = 1.125                         // _DAT_005cd118
screen_x = (1 - (0.48  + k*local_x) / 0.6 ) * 320   =  64 - 600*local_x
screen_y = (1 - (rowY  + k*local_y) / 0.45) * 240
rowY     in {0.25, 0.15, 0.05, -0.05}     // DAT_005f337c
local_*  = DFF world coords minus the authored root translation f00.pos
```

Derived vs observed, row 0 (observed edges read off `orig_stand7.bmp` at 5x with a pixel
ruler, not colour thresholds):

| element | derived at k=1.125 | observed | note |
|---|---|---|---|
| row centres y | 106.7 / 160.0 / 213.3 / 266.7 | 107 / 160 / 213 / 267 | < 0.5 px |
| bar frame a22 | x 81.3 .. 186.0 (w 104.7) | ~84 .. 186 (w ~102) | right edge exact |
| bar backing a18 | x 87.9 .. 177.5 (w 89.6) | orange inner fill | matches |
| point circle a08 | x 189.8 .. 236.2 (w 46.4) | ~186 .. 232 (w ~46) | width exact, ~4 px right |
| car icon a00 | x 25.8 .. 84.3 | badge ink 33 .. 73 | quad encloses ink |

At `k = 1.0` the bar's right edge came out at 172.4 (inside the bar) and the circle was
41.2 px wide against ~46 observed. Both are corrected by `k = 1.125`, so the value is
confirmed by the image and not just read from `.rdata`. Remaining disagreement is
~0-4 px, at the level of transparent texture margin.

**Earlier hypothesis FALSIFIED.** This note previously attributed the residual to the
per-child `FUN_004c13e0` scales. That is wrong, and the decompilation already in hand
disproves it. The four calls take children at `group+0x88`, `+0x8c`, `+0x94`, `+0x98`,
i.e. slot indices `(offset - 0x80)/4` = **2, 3, 5, 6**:

- slots **2 and 5** get `vec = (score/max, 1, 1)` — an X-only scale from
  `FUN_0040b6d0` (score) over `FUN_0040b890` (max). These are the same two children
  `FUN_0041c9a0` writes `0xff323232` to, i.e. the grey bar backing (a18/a19).
- slots **3 and 6** get `vec = (pulse, pulse, 1)`, gated on flag `group+0x10c & 0x40`
  (else 1.0) — the crown pair (a06/a07).

None of the four touches the circle, the bar frame or the icon, so they cannot produce a
placement error in those.

The slot-2/5 identification is on firm ground: `FUN_0041c9a0`'s write is
`*(*(*(child+0x18)+0x20)+4) = 0xff323232`, a material-colour override, and a18/a19 are
the only two atomics in the clump with **no texture** and a baked flat `0xCCCCCC`
(Finding 14). Slots 3/6 as the crown pair rests on the leader gate (`0x40`) plus the
pulse being uniform in X and Y, which suits a badge and not a bar.

**The general slot map is NOT in the DFF — clean negative from the bytes.** The earlier
claim (carried from the Ghidra proxy) that `FUN_004b5190` returns "the slot index baked
into that atomic's DFF frame plugin data, set by the DFF authoring tool" is **false**.
Parsing `ENDPOINTPANEL.DFF`'s chunk tree shows **all 29 frame extension chunks and all
24 atomic extension chunks are zero-length** — there is no frame plugin data, no frame
names, and no atomic extension data in the file at all. Every atomic struct is
`flags=0x5, unused=0x0`, and geometry index simply equals enumeration order
(a00→geom0 … a23→geom23). Frame indices are 23,24,25,26,27,28,22,21,9..20,4,5,6,1, and
neither frame index nor geometry index reproduces the observed slots {2,3,5,6} for the
backing/crown pairs.

So `FUN_004b5190`'s value is produced at runtime by something outside the DFF.
`[UNCERTAIN U-9079]` — what populates it is not determined; it needs Ghidra on the
`FUN_00543d40` / `FUN_00543d70` / `FUN_00543df0` accessor family and on the clump loader
`FUN_0042a5d0`. This only matters if the enable-flag semantics are ever ported; the
shipped Im2D path drives visibility with its own logic and does not need the slot map.

### Crown pulse law (bears on U-9071)

For the crown pair, with `tick = DAT_0063d270` (incremented once per frame at the end of
`FUN_0041cc50`, zeroed by `FUN_0041cbc0`):

```
if (tick < 0) tick += 4294967296.0;          // _DAT_005cc94c, 0x4f800000
pulse = sin(tick * 0.2) * 0.15 + 1.0;        // _DAT_005cc9c0, _DAT_005cc8f0, _DAT_005cc320
scale = (pulse, pulse, 1.0)                  // applied only when group+0x10c & 0x40
```

All four constants read from `.rdata`: `0x005cc94c` = `00 00 80 4f` = 4294967296.0,
`0x005cc9c0` = `cd cc 4c 3e` = 0.2, `0x005cc8f0` = `9a 99 19 3e` = 0.15, `0x005cc320` =
`00 00 80 3f` = 1.0. So the crown breathes +/-15% about its size at 0.2 rad/frame.
Derived crown position is x 240.2 .. 277.7 (w 37.5), vertically centred on the row, to
the right of the point circle. The crown atomic's own frame origin sits at local
x -0.325 -- exactly the centre of its quad -- so the scale pulses it about its centre.

### U-9071 crown PORTED and numerically verified (position + pulse only)

Ported in `exe_main.cpp`: the crown now draws at the derived rect
(640-space centre x 259.0, row centre y, side `37.5 * pulse`) instead of the previous
placeholder at the icon's top-left corner, and it pulses by the law above using a
standings-frame tick that mirrors `DAT_0063d270`.

Verified by measuring the standalone's own output over nine independent standings frames
(`verify/run_32508/r6/*_result.bmp`, 800x600, `MASHED_ROUND=1`,
`MASHED_ROUND_SCORES=8,7,5,4`, `MASHED_CROWN_TEST=0`; crown ink isolated to
x 296..360 so the point circle, which ends at x=290 in 800-space, cannot contaminate it):

| quantity | derived | measured |
|---|---|---|
| centre x (800-space) | 323.7 | **323.3** (mean of 9) |
| row-0 centre y | 133.4 | **133.8** (mean of 9) |
| side, envelope | 39.9 .. 53.9 | **38 .. 52** |

Non-degenerate: the size varies across the nine frames (implied pulse 0.832 .. 1.109
against a predicted 0.85 .. 1.15) while the centre stays put to within 0.5 px, which is
the signature of a scale about the quad centre and not a reposition. Both measured ends
sit ~2.5% under the predicted envelope by the same factor, consistent with the crown ink
being slightly inset from its quad. Screenshot: `verify/race_hud/re_crown_u9071.png`.

**Scope of that claim — read it narrowly.** This measures the port against the
derivation, so it proves the port implements the DFF+`FUN_0041c410` law correctly. It is
**not** a parity check: no capture in `verify/race_hud` has ever shown the original's
crown, so there is no original-side reference to diff against. The derivation itself
rests on the DFF geometry, the camera constants and the decompiled scale call, all cited
above.

### U-9071 TRIGGER reversed — the crown is a win-threshold test, not a leader election

`FUN_0040b930` @`0x0040b930`, which fills the `0x40` source array, is:

```c
for (i = 0; i < 4; ++i)
    DAT_0063cde8[i] = (DAT_008a94e0[i] >= FUN_0040b8e0());   // score >= threshold
```

`FUN_0040b8e0` @`0x0040b8e0` returns the **win threshold**:

| condition | threshold |
|---|---|
| `DAT_007f0fd0` (race rule) == 1 or 2 | 7 |
| `DAT_0067ea64 != 0` (via `FUN_0042f500` @`0x0042f500`) | 7 |
| fewer than 4 participants (`FUN_0040e340`) | 7 |
| 4 participants, other rule, flag clear | **10** |

So the crown is **not** "the leader" — it is *every* car whose score has reached the
value that wins the match. Note the threshold arms line up with the score-bar max from
`FUN_0040b890` (U-9072 residual B): max 12 pairs with threshold 10, max 8 with 7, i.e.
`max = threshold + 2` in both arms, off the same three determinants.

**This rule predicts the reference.** `orig_stand7.bmp`'s four scores are 8/7/5/4 and the
4-player threshold is 10, so `0x40` is legitimately clear on all four rows — which is
exactly why no capture in `verify/race_hud` has ever shown a crown. The absence stops
being an unexplained gap. This is non-circular: the threshold was decompiled without
reference to those scores.

Ported: the guessed match-end/leader gate is **replaced** by `score >= threshold`, with
the threshold bound to the reachable determinant (`race_rule`), participants fixed at 4
and `DAT_0067ea64` unmodelled — the same binding U-9072 uses.

### Row order was WRONG: the original sorts by score (found in the same round)

`FUN_0040b540` @`0x0040b540` fills `DAT_0063cdf8`, and `FUN_0041c410` finds a group's row
by searching it for the group's player index. It is a descending bubble sort:

```c
for (i = 0; i < 4; ++i) {
    local[i] = DAT_008a94e0[i];                       // score
    if (DAT_007f1a14[i*4] == -1) local[i] = -100;     // absent slot sentinel
    out[i] = i;
}
// adjacent-swap sort, descending on local[], carrying out[] along
```
(Modes 4/7 replace the order via `FUN_00417740`; mode 9 uses a fixed two-entry order.
Neither is reachable here — `[UNCERTAIN U-9080]`, unmodelled.)

**The port drew row r for car r.** That is invisibly correct whenever the scores already
descend with car index — which is true in the demo *and* in `orig_stand7.bmp` (8,7,5,4),
so no capture could ever have caught it. Fourth appearance of the degenerate trap in this
lane. Fixed: rows are now ordered by the same descending sort.

### Verification (non-degenerate, two scenarios)

Distinct scores were poked with distinct per-car colours (`MASHED_ROUND_COLOURS=0,1,2,3`)
so the row order is readable off the badges. Crown presence measured as gold-ink pixel
count inside the derived crown box, isolated past the point circle's x=290 edge.

| run | scores | predicted row->car | measured badges | crown |
|---|---|---|---|---|
| A | 8,7,5,4 | 0,1,2,3 (identity) | Red, Bluejay, Melon, Gold = **0,1,2,3** | **none on any row** |
| B | 4,10,5,7 | **1,3,2,0** | Bluejay, Gold, Melon, Red = **1,3,2,0** | **row 0 only** (189 px) |

Run B is the non-degenerate case: its row order differs from car index, and the old code
would have drawn it wrong. Run A reproduces the reference arrangement and, correctly,
draws no crown — the same outcome as `orig_stand7.bmp`. Bar cells read 10/7/5/4 in run B
off the image; an automated cell count is unreliable here because the empty part of the
bar carries a dashed pattern, so the authoritative cell verification remains Finding 18's
drawstream one, which this change does not touch. Screenshot:
`verify/race_hud/re_crown_rowsort_u9071.png`.

Still `[UNCERTAIN U-9078]`: `DAT_0067ea64`'s meaning (`FUN_0042f500` is a bare getter), and the
mode-4/7/9 row-order overrides. Neither is reachable in the port's current modes. (`DAT_0067ea64` = U-9078; the row-order overrides = U-9080.)

Bits named in passing, all from the same accessor family and all reading the score array
`DAT_008a94e0`: **0x80** = tied for highest score (`FUN_0040b9a0` @`0x0040b9a0`),
**0x100** = lowest score (`FUN_0040ba00` @`0x0040ba00`), **0x200** = score is exactly zero
(`FUN_0040b970` @`0x0040b970`). `FUN_0040b420` @`0x0040b420` is a bare read of
`DAT_008a9500[car]` — the score delta feeding the point-circle switch.

### Port impact

None applied. The derived layout agrees with the shipped Finding-10 measured constants
to within ~4 px, and acceptance for this layer is screenshot-only (it is invisible to
`drawlist_diff.py`). Swapping verified constants for derived ones of equal accuracy would
be churn with no better acceptance signal. The value of this is that the measured layout
is now **independently corroborated by the emitter**, and that the crown pulse and the
`score/max` bar scaling are now laws rather than observations.

**Clean negatives** (Ghidra proxy, explicit): no frame or matrix write exists in
`FUN_0041cb10` @`0x0041cb10` (init), `FUN_0041c320` @`0x0041c320` (group build),
`FUN_0041c9a0` @`0x0041c9a0` (row draw) or `FUN_0041ccc0` @`0x0041ccc0` (row loop).
`FUN_0041c320` writes exactly `group+0x100` = clump ptr and `group+0x104` = clump root
frame; `FUN_0041cb10`'s only per-row write is `group+0x108` = player index;
`FUN_0041c9a0` has zero static callees. All placement happens in `FUN_0041c410`.

Also decoded on the way: `FUN_004b5190` @`0x004b5190` returns the slot index baked in
the atomic's **frame plugin data** (`*(atomic+0x18)` → plugin block at
`frame + DAT_007dc634`, entry `+0xc`, `[0]`), read via `FUN_00543d40` / `FUN_00543d70` /
`FUN_00543df0` @`0x00543df0`. The DFF atomic → slot map is in the DFF's frame plugin
extension, not in code. `[UNCERTAIN U-9079]` — not parsed; needed only if the enable-flag
semantics are ported.

### Method note

Two errors were caught by cross-checking, one on each side.

The proxy child's first pass reported "DFF baked world positions ARE the screen
positions", verified against the bar rect. That check was **circular** — it inverted the
screen measurement it had been handed to produce the world coordinate it then
confirmed. Caught by re-deriving the world coordinates from the DFF bytes locally. Its
follow-up "per-row Y is baked in the DFF" was wrong for the same reason (the DFF holds
one row, not four). The camera constants it found are correct and are now confirmed the
other way round (DFF → screen, sizes match with no free parameter).

In the other direction, this session briefly claimed `DAT_005f337c` was not a float
table but `.text` instructions, on the strength of a local raw dump. That dump used a
**broken RVA→file-offset mapping** (it read the section header's `VirtualSize` field
where `VirtualAddress` was intended, so every lookup landed in the wrong section). The
child's address was right: `0x005f337c` is in `.data`, file offset `0x1f337c`.

The instructive part is that **both readings were true at once**, and the bug hid it.
The bytes that dump surfaced were real `MOV [ESP+..], imm32` instructions — they are the
initialiser `FUN_0041cbc0` at file `0x1cbc0`. What the broken mapper corrupted was only
the *label*: it reported them at RVA `0x005e54b7` instead of `0x0041cbc0`. So "the 0.48
immediates are in code" and "`DAT_005f337c` is a `.data` table" were never in conflict —
the code writes the table. Chasing the mislabelled RVA cost a round; a plausible-looking
disassembly at a wrong address was what made the false claim credible enough to state to
the child as fact.
