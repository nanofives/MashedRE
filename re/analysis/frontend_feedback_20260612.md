# Frontend feedback RE program — user review 2026-06-12

User directive: iterate over ALL items until fixed; method = locate owning
functions in the binary, RE properly, no cosmetic patching.

Empirical base: `re/frida/menu_draw_dump.py` retaddr-augmented dumps (every
chrome quad's emitter pinned by return address) + Mashed_pool0 decomp session
2026-06-12. Standalone fix lands only after the owning function is RE'd
(C2 note or twin diff), per the no-inventing rule.

## Architecture discovered this session (corrects F3's framing)

**FUN_0042e5b0 (MenuChromeShellB) = the background composer.** Per-frame it
calls, in draw order:
- `FUN_00473c20(x,y,w,h | stack: tex_sel, alpha)` — UV-subregion textured quad:
  UVs = video dims (FUN_00493f80) / texture dims; SRCALPHA blend (states
  10=5/0xb=6); tex_sel 0/1 → video texture pair DAT_00771a50/DAT_00771a54
  (crossfade pair); other values = explicit texture handle (state 2=3 around
  the draw). Draws the menu VIDEO and (with a handle) the TITLE LOGO quad.
  [emitter retaddr 0x473ebd, dump draw 0]
- `FUN_00474890(int* tex, x,y,w,h, color, time_i, mode, grad_flag)` — TWO
  textured quads with TIME-ANIMATED UVs (mode switch 1..5 selects the UV
  pattern; time term = time_i × _DAT_005cead4; grad_flag fades corners 0/2):
  the right-side "white wash" is a SCROLLING TEXTURED overlay, not flat white.
  Callers: 0x0042e5b0 + 0x0042e8b0 (sibling composer). [retaddrs
  0x474bd3/0x474d0c, dump draws 1-2]
- `FUN_00473ee0(p1, p2, slide_f)` — NOT a logo drawer:
  1. two FUN_004733b0 band quads;
  2. fade pair step (DAT_0086ecc8/cc, 2 steps/frame);
  3. 15-strip CIRCULAR-ARC wash: strip x = slide − √(_DAT_005cead0 − s²) −
     _DAT_005cc9d0, s = 90,78,..,−78 step −12; strip h = _DAT_005ceacc
     (34.13); alpha = DAT_0086eccc×0x60/0xff → the strips are the SCREEN
     TRANSITION wash (alpha 0 when settled — why the dump showed alpha-0);
  4. the "checkers": 2 bands × 3 rows × 7 cols of UNTEXTURED grey quads
     (state 1=0!) ff808080, staggered (odd rows +0x15=21px, pitch 40px from
     x=360), corner-jittered by fsin(col+row(+k) + 2×DAT_007f1010) ×
     _DAT_005ce1f8, rows pinned at y=64/416, first cell of even rows
     transparent. The checkerboard look IS the stagger — there is NO checker
     texture. This kills the "checker texture identity" residual.

The standalone currently calls its port of FUN_00473ee0 as "the rainbow logo
drawer" with the MASHEDNEWLOGO texture — semantically wrong. The big title
logo is a FUN_00473c20 textured quad (handle from ShellB). The port itself is
bit-identical (C4) — only its standalone WIRING (texture binds, fade-driven
alpha, call sites) is wrong.

## Item → owning function map

| # | Feedback item | Owning function(s) | Status / route |
|---|---|---|---|
| 1 | out-of-focus freeze | window proc / WM_ACTIVATE path (WindowMsgPump 0x00499690 area) | confirm the original's activate handling; decide standalone behavior to match |
| 2 | loading screen: spinning disk + text | FUN_00403050 (sprite DAT_00771964 at 320,200,480,240 + FUN_00402fb0 body) | decomp FUN_00402fb0 next; port |
| 3 | "Press button to start" flashing | separate title handler (NOT FUN_0043c5b0 - title phase 1 only calls FUN_0042aae0); text via FUN_00554940 glyph pipe | ROUTE PINNED: confirmed drawn at title (live probe text='Press button to start'); print path = FUN_00428140->FUN_00554940 vtable+0x138; next: find the title-screen handler + flash timer |
| 4 | attract/demo race on title idle | title idle timer → demo launch | find the timer global + transition in the title handler |
| 5 | logo misplaced | FUN_00428760 sprite pipe (emitter 0x450c7a, titleframe dump) | LANDED: ONE static quad at virtual (80,80,480x240) full-white; standalone fixed from the invented 768px/96% placement; draw order corrected (previews before logo) |
| 6 | band opacity (start semi-transparent, gradient) | TextGradientV0V1/V2V3 (0x00472f40/0x004730b0) args from chrome; band alpha may be phase-driven | RE ShellA/ShellB band alpha inputs; verify dump colors ff→a0 vs title state |
| 7 | checkers not animated | FUN_00473ee0 checker pass (above) | LANDED cde0ee52: layer runs every frame per ShellB; animated checkers visible (verify/dbg_backbuffer.png) |
| 8 | no boot "Load Successful" modal | save-load boot flow + modal screens (descriptor tables; orig_gts.png shows the modal) | locate the modal screen table + draw path |
| 9 | font pixelated/wrong | FUN_00554940 glyph renderer (vtable ctx+0x138 @0x0067d838); height = param_5*0.0708 (_DAT_005cd5fc); text goes through device slot 0x30 from its OWN vert buf | height scale LANDED; residual = the glyph renderer's per-glyph UV/filter (RE FUN_00554940 vert layout) |
| 10 | buttons not centered with text | record y = row CENTER line (bit-verified plate spans rec.y-12..+14) | LANDED: glyph cell now centered on the plate center (was top-anchored at rec.y, overflowing below) |
| 11 | back indicator on main menu | back-row visibility rule in FUN_0043d2a0 (type/slide init) + draw gating | RE the exact gate (screen-kind? depth?) |
| 12 | select-arrow texture missing | FGDC20 ext glyph 0x81 (footer=' Select' live-confirmed) | ROUTE PINNED: original footer = glyph 0x81 + ' Select'; verify our FGDC20 ext-table renders 0x81; the prompt-strip records carry it |
| 13 | black semi-circle (button cap) | TextSpriteScaled 0x004739f0 'Button' sprite, color ffb45010 (dump draw 88) | port audit: badge texture content + modulate color |
| 14 | video not blending | FUN_00473c20 + FUN_00474890 (preview crossfade) | PARTIAL: two-quad corner-faded form + title gating removed (ShellB runs it every frame) + draw order fixed; residual = UV-pan modes 1..5 (preview atlas layout) |
| 15 | screen-transition fades wrong | arc-wash strips + slide DAT_008990e0 + fade pair DAT_0086ecc8/cc | LANDED (writers xref-confirmed): fade 0xff raised by nav RELOAD ops (0x0043d2c2; push/pop skip it) + selects (0x0043f9d7); slide raised +0x20/frame by FUN_0042e8b0 framed-preview body (caller FUN_004368e0) [slide residual lands with Wave-3 per-screen content]. The settled menu's permanent left haze (alpha 0x60 arc wash) is FAITHFUL and now renders |
| 16 | top/bottom text animations wrong | header/footer rows' slide/type anim (FUN_004325c0 classes) | port-side audit of row classes vs the verified tick |
| 17 | menu text always black | FUN_0043c5b0 text-color rule (pool0 decomp) | LANDED: list items ARE 0xff000000 black for both states (the original's behavior - plate is the cue); BACK/HEADER rows draw record color (white) + black shadow. Standalone was white-on-record-color for items + shadow on all; now matches (verify/frontend_parity2/re_scr8_text.png) |
| 18 | unselectable = transparent black | FUN_00428140 dim path (DAT_008990e4 -> 0x80000000, alpha clamp 0x60) | LANDED: greyed rows now draw half-alpha black (0x80000000) per the decomp's dim branch, not the invented 0x60606060 grey |
| 19 | options load/save greyed out in RE | FUN_00432800 case-8 gate on FUN_00492d10 save-ready | LANDED: live probe DAT_00771968==1 at the original's menu (blank save) -> Load/Save enabled; standalone's hardcoded disable removed |
| 20 | sound sliders all wrong + continuous steps | screen-19 value-widget drawer + input handler (NOT yet located) | find via retaddr dump ON screen 19 (sample was mid-transition; redo) + Ghidra |
| 21 | insults tri-state Off/Auto/Manual | sound settings model + its strings | locate the setting global + string ids |
| 22 | gamma screen has no slider | gamma screen widgets (different from sound) | retaddr dump on the gamma screen + RE |
| 23 | autosave on/off (no arrows) | autosave screen widget | same route as 22 |
| 24 | quick battle greyed in RE | FUN_00432800 case-2 gate on DAT_007f0ad4 | LANDED: live probe shows 2 at the menu with an ALL-ZERO gamesave (boot default, not save-derived); kFreshState.has_profiles 0 -> 2 (verify/frontend_parity2/re_scr2_gates.png) |
| 25 | player color select UI missing | post-car-select screen (unmapped) | locate its screen id + descriptor table + drawer |

## Iteration order (goal: all fixed)

Wave 1 (composer stack — biggest visual win, mostly RE'd today):
ShellB decomp → logo quad args + wash texture identity + band alphas →
re-wire the standalone composer (video/logo/wash/arc/checkers) faithfully.
Wave 2 (text pipe): RtCharset capture + font rendering quality + black text
semantics + ext glyphs (items 3, 9, 12, 17).
Wave 3 (widgets): sound/gamma/autosave screens (20-23) via retaddr dumps per
screen + Ghidra.
Wave 4 (flow): boot modal, back-row gating, grey-out engine, enables
(8, 11, 18, 19, 24).
Wave 5 (features): loading screen (2), attract demo (4), focus behavior (1),
player color select (25).

## Verification note (2026-06-12)

Window screenshots of the standalone are UNTRUSTWORTHY on this machine (multi-monitor Present issue renders agent-spawned windows white while the backbuffer is correct). Use MASHED_DBG_BBDUMP=1 (dumps verify/dbg_backbuffer.bmp at frame ~200) as the truth channel for all standalone visual verification.
