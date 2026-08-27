# Frontend feedback — user review, 2026-08-27

Source: human review of the 17 side-by-side composites from
`re/frida/frontend_parity.py` (run `verify/parity_run_20260827_005410/`). The harness
scored **12/17 FAITHFUL**; the reviewer found defects on essentially all of them. That
gap is the point of this note — see "Why the harness missed all of this" at the end.

Triaged into three classes. **B and C are not port defects and must not be worked as
if they were.**

---

## A — real port defects

### A1. Font rendering looks low-resolution / wrong. REOPENED, and it has history.

Reviewer: *"font rendering is different, it looks poor resolution"*, and notes they
raised this before and it was not found at the time.

**It was found before.** Four rounds on 2026-06-12/13 root-caused and fixed real bugs:
anisotropy + anchor + alpha tiering (`714ec7cc`), the FGDC20 atlas pixel base off by
4 bytes so every glyph sampled its neighbour's edge column (`c81ed136`), and a
supersample whose negative lobes eroded thin strokes (`eb920fbd`, reverted).

A fifth round on 2026-06-15 tested two more hypotheses and **disproved both**, leaving
the defect open. Its conclusion lives at `MashedFont.cpp:24-30`:

> "2x supersample (kAtlasSS=2) tested … did NOT smooth the edges (nor did removing the
> gamma), so the jaggies are NOT atlas-resolution or coverage-curve … **The font
> jaggedness vs the original's smooth edges is still OPEN** — likely the actual
> draw-time sampler state or a different font the original uses for some text; next
> step is to instrument the real sampler filter at the glyph draw."

**That next step was never executed.** `git log -1 -- MashedFont.cpp` = `3836926f`,
2026-06-15. And **no tracker row was ever filed** — the source comment is the only
record, which is exactly why a later tracker-driven look did not find it. Filed now
(see "Tracker actions").

Three concrete, unexamined contradictions sit on that named next step:

1. `RwIm2DBridge_SetTexturePointFilter` (`RwIm2DBridge.cpp:284`) has **zero call sites
   in the tree**, and `RwIm2DBridge_RegisterTexture` defaults `point_filter = false`
   (`:279`), so glyphs draw `D3DTEXF_LINEAR` (`:182-185`) — while `RwIm2DBridge.h:47-49`
   and `RwIm2DBridge.cpp:178-181` both still assert the original is POINT-sampled.
2. The verbatim quad port faithfully emits the original's `rw_set_state(9, 2)`
   (`DrawQuadPrimitives.cpp:495`) and the bridge **throws it away**:
   `RwIm2DBridge.cpp:106` `default: break; // 6/8/9 etc. — not needed for the bridge`.
3. The raster-native filter value `*(texture+0x50)&0xff` is still `[UNCERTAIN]` —
   residual 5 of `re/analysis/font_fgdc20_text_law.md:158-160`, "runtime read would
   settle it".

**Rule out first, it is cheap:** a GDI/Arial fallback exists
(`D3d9Render/TextRenderer.cpp:14-25`) and fires whenever `g_font.ready()` is false
(`exe_main.cpp:3823-3827`). If the FGDC20 atlas fails to load for any reason the entire
menu silently renders in Arial. Confirm the real atlas is live before chasing samplers.

**Trap from the earlier rounds, do not repeat it** (`frontend_feedback_20260612.md:96`):
the 2026-06-12 complaints persisted across backbuffer-verified fixes because DWM was
bitmap-stretching the non-DPI-aware window *after* Present — invisible to BBDUMP,
obvious in `capture_window.ps1`. **Presentation-affecting fixes must be verified with
the presented-window capture, not BBDUMP alone.**

### A2. Selection icon should be a green fill with a black border. It is not.

Reviewer: *"select icon should be filled green with black border, it's not that right now"*.

The reviewer is describing a **decoded original law we have not ported**.
`re/analysis/promote_c2_render_lowrva/00403fa0.md:29-30`: `FUN_00403fa0` draws each
element **twice** — an opaque black copy at x=322.0 (`0xff000000`) and a coloured fill
at x=320.0 (`0xc800d805`, annotated "colored green-fill"). That is a 2-virtual-px
offset black pass behind a green fill, i.e. a border.

Ours is a **1px down-right drop shadow** with a different green:
`exe_main.cpp:3599-3603`, `ob = 1.0f * kVScale`, green `0xff10ec00`. The 2026-06-14
comment at `:3592-3597` says this shape was chosen from cached screenshots
("image-cache 1.png/2.png"), **not** from a draw-list diff.

`FUN_00403fa0` is C2 and unported (`hooks.csv:1441`); its draw callee `0x00427e00` is
C2 too. So this is a port gap with a decoded law available, not an unknown.

### A3. s15 / s16 — layout and icon defects

Reviewer: *"font size is smaller, player icon is smaller, joystick icon is not colored
according to the player, and the number 1 is right into it"* (s16 same feedback).

- font size smaller, player icon smaller → sizing law, related to A1's scale chain
  (cell height `scale * 0.0708 * 480 * kVScale`, `exe_main.cpp:3641`; cell width =
  height * 4/3, `:2397`). Note `exe_main.cpp:2399-2403` carries a **non-derived
  `+0.055` fudge** admitted in-source as a calibration against a baseline, because the
  law's `_DAT_0067d834` camera factor "was assumed 1.0". A fudged scale term is a prime
  suspect for "smaller than it should be".
- joystick icon not coloured per player, "1" colliding with it → the input-device
  sprite is a **placeholder box**. `exe_main.cpp:4025`: *"the keyboard/controller sprite
  isn't in INTERFACE.TXD — its source TXD is unconfirmed, so a placeholder box stands
  in."* The real assets exist on disk:
  `original/TOASTART/PC/{Joypad,Joypadi,Keyboard,Keyboardi}.png` + `pc.txd`.

### A4. s4 — controller selection screen

Reviewer: no orange-line background behind controller selection; there should be an
"unselected zone" to the left of all icons for controllers that have not picked a
player yet; we show 3 players on the left but only 1 on the right, "maybe there's
rewiring needed to show more players".

Corroborated in-source. `exe_main.cpp:4007`: *"No 6-swatch palette, no orange player
bars, no selection square — those were invented and matched nothing."* So the orange
bars are **known-missing and previously admitted**, and the current layout is invented.
The asymmetric player count is unexplained and is the part worth investigating.

### A5. s6 / s7 — missing textures and stretched icons

Reviewer: *"the 'vs' texture is not loaded, player icons are stretched"*.

Missing texture + non-uniform scale on the icon draw. Distinct from the game-state
issues on the same screen, which are class B below.

### A6. s24 / s18 — arrows, power-up textures, missing fade

Reviewer: *"arrows on the text are wrongly placed and wrong size … there's no loaded
textures for power ups available in this map … there's no fading of the menus
background boxes to the right"*.

- arrow placement/size ties to A2 (the same `exe_main.cpp:3592-3603` arrow law).
- power-up icons not loaded — asset path unresolved, same class as A3's placeholder.
- missing right-hand fade on the background boxes — the alpha-fade sprite draw is
  `0x00428140` (C2, `hooks.csv:248`, "sprite draw w/ alpha-fade, sets render-state 9=2")
  and is **unported**. Note it sets render-state 9, which A1(2) shows the bridge
  discards. Possibly one root cause with A1.

---

## B — harness artifacts, NOT port defects

### B1. The original has no map preview on s6. That is our doing.

Reviewer: *"renderer of the original game looks wrong here because there's no preview
of the map"*.

`frontend_parity.py` deliberately patches three original-side backdrop composers to
`RET` so the comparison is deterministic — including **`FUN_00474890`, the preview
crossfade wash** (agent comment, `frontend_parity.py`). The missing preview is the
harness neutralising the original, not a fault in the original or in our port. Any
review of preview rendering must use a run without `nop_backdrop()`.

### B2. Wrong track names (Arctic / Egypt), and stars showing.

Reviewer: *"track names are not loaded, arctic and egypt are wrong names"*, *"stars
should not appear if everything is unlocked"*.

Already recorded: `verify/parity_20260819/RESULT.md:32-38` — the parity walk pushes a
screen **without cup/championship state**, so it lists `Arctic`/`Egypt` where a
real-boot original lists `Angel Peak`/`Kharga Temple`/`Neustein`/`Timgidski`. That note
states plainly it is **not a data defect**. The stars are the same root: no unlock state.

---

## C — test-setup requests

### C1. Capture with everything unlocked (s2, and it fixes B2 as a side effect)

Reviewer: *"we need to test next time with everything unlocked by default"*.

Correct, and it is the single highest-leverage harness change here: real unlock + cup
state would fix the track names, remove the stars, and make s6 / s7 / s18 / s24
genuinely comparable instead of noise.

**Constraint that must not be violated.** `CLAUDE.md`: *"NEVER apply `unlock_all` /
`unlock_tracks` / `unlock_restore` to `original/MASHED.exe`"* — that binary is the
diffing reference, and patching it would make every future comparison modded-vs-modded
and still report GREEN. So the unlock must be applied **at runtime under Frida** (the
harness already drives the original under Frida) or against a separate copy — never as
an on-disk patch to the reference binary.

---

## Why the harness missed all of this

`frontend_parity.py`'s metric is an **ink-map XOR**: it thresholds both images at
brightness < 90 and compares *where* the dark pixels are. It is deliberately blind to
the backdrop — and consequently blind to glyph **shape**, stroke weight, colour, fill,
border, and texture content. A screen can be 1.0% "faithful" on that metric with every
glyph the wrong weight and every icon the wrong colour.

Worse, the acceptance harness this project mandates for visual work **cannot see text
at all on either side**: the original's RtCharset glyphs never reach the hooked draw
vtbl (`re/analysis/parity_tooling.md:83-87`), and the standalone's glyphs are texture
handle 9, explicitly `--exclude-tex 9`'d out of chrome comparisons. So
`drawlist_diff.py` structurally does not cover the font pipe.

**Human review is currently the only instrument that covers fonts and icons.** This
round produced more actionable frontend findings than the automated pass did, and it
should be repeated whenever the frontend changes.

---

## Tracker actions

- **A1 filed as an uncertainty** — the font defect had no tracker row for ten weeks,
  only a source comment, which is why a later look did not surface it. Same failure mode
  as the two camera defects found 2026-08-26 (both documented in comments, both shipped).
- A2..A6 are port gaps against decoded or partially-decoded laws, not knowledge holes;
  they belong in the frontend work queue rather than `UNCERTAINTIES.md`.
- B1/B2 need no tracker row; they need the harness changes in C1 plus a no-nop run.

---

## Addendum — corrections from a deeper history sweep (same day)

Two things in A1 above were wrong and are corrected here rather than edited away.

**1. The POINT-vs-LINEAR "contradiction" is not a lead. It is a stale comment.**
`re/analysis/font_fgdc20_text_law.md:106-115` already settles it: the original samples
**LINEAR**, and the point-sampling claim was an *intermediate compensation for the
4-column atlas shift*, reverted once the real cause was found. So
`RwIm2DBridge.cpp:178-181` asserting POINT is stale prose, and its zero-caller setter
(`:284`) is dead **correctly**. Do not "fix" the sampler to POINT — that would
re-introduce a known-wrong compensation.

**2. There is a much stronger lead: a stale RESOLUTION premise.**
`MashedFont.cpp:17-22` reasons in 800x600 terms ("~34 device px"), but the default
backbuffer is now **640x480** (`exe_main.cpp:344-346`). A sizing law calibrated at
800x600 and then run at 640x480 draws every glyph across fewer device pixels — which
produces **both halves of this review's report at once**: "looks poor resolution" *and*
"s15 font size is smaller". Independently corroborated by
`DIVERGENCE_LEDGER_3D.md:176-180`: *"glyph rendering still visibly differs (original
heavier/larger)"*. That should be checked before anything else.

**3. The stall had a specific, now-cleared cause.** The 2026-06-15 work was filed as
task A2 in `goal_scaffolding_plan_20260615.md:22-26`, explicitly blocked on *"needs a
fresh user side-by-side screenshot to pin the exact defect"*. The project pivoted to
physics the following day (`git log 2026-06-16..2026-06-19` is entirely WS-PHYS/WS-G) and
A2 was never resumed. **This review is that screenshot.** The blocker is cleared.

**4. There was an even earlier false all-clear.** Commit `da4ce514` (2026-06-12 11:11),
*"#9 RESOLVED: font render path verified verbatim-faithful (no boldness knob)"*, searched
for a font defect and concluded there was none. It was later declared wrong on four
counts, and the actual cause that day turned out to be the 4-byte atlas offset. So this
defect has now survived **two** "we looked and found nothing" verdicts. Treat a clean
bill of health on the font with suspicion unless it comes with a measurement.

**5. librw I6 will not fix this.** `LIBRW_SIZING_2026-08.md:243-245` explicitly excludes
`MashedFont` (and `TextRenderer`) from that lane. Waiting for the renderer migration is
not a strategy for the font.

**6. Evidence rot.** `MashedFont.cpp:158-166` cites `verify/_cmp_s4_title.png` for the
gamma correction; that file does not exist on disk.
