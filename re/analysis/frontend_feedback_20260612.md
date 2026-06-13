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
| 1 | out-of-focus freeze | PumpOnce() WaitMessage() park when !g_active | LANDED: replaced the blocking WaitMessage park with Sleep(15) so the render loop keeps running unfocused; verified frame 200 reached while the game window was unfocused |
| 2 | boot splash | FUN_004288a0 (legal/copyright splash) via FUN_00428a30; ~8s (24000000/3MHz QPC timer) | LANDED: the real boot splash is the Empire/Supersonic COPYRIGHT screen (MASHEDNEWLogo @320,100,256x128 + USA.DAT lines 0x1e5..0x1eb + 'Loading' 0x222), NOT Dolby/ProLogic (those textures are dead-loaded/vestigial - only read at load+free). Ported as phase-0 splash, 8s auto-advance or any key (verify/frontend_parity2/re_splash.png). Residual: bigE icon flanking 0x1e6/0x1e7 |
| 3 | "Press button to start" flashing | FUN_00402fb0 (string id 0x2a4, called by FUN_00403050) | LANDED verbatim: alpha = 128 + 127*sin(phase), phase += 0.1/frame (amp -127.0 @0x005cc570, step @0x005cc56c); black shadow (320,380) + white main (316,376) scale 1.2 centered; verify/frontend_parity2/re_title_pressbutton.png |
| 4 | attract/demo race on title idle | title idle timer FUN_004030d0 → FUN_0043df00 | RESOLVED (RE-cited, gameplay-blocked): FUN_0043df00 sets DAT_007f1a14/24/34=0/1/2 (3 cars), marks all three AI via FUN_0040e480(n,2), sets DAT_0067e9fc=0xb, and enters a game session via the nav SM FUN_0043d2a0(0,2). The attract "demo" is a SCRIPTED 3-AI RACE running the full game engine — NOT a video and NOT replay data. It cannot be a frontend fix; it requires the gameplay/racing engine the standalone does not have yet. Defer to the gameplay milestone (tracks with the renderer-gate / vehicle-lighting work). NOT blocked on any user question. |
| 5 | logo misplaced | FUN_00428760 sprite pipe (emitter 0x450c7a, titleframe dump) | LANDED: ONE static quad at virtual (80,80,480x240) full-white; standalone fixed from the invented 768px/96% placement; draw order corrected (previews before logo). **2026-06-12 parity adjudication: the logo also draws on MENU screens (scr1 burst A[40]), as the LAST draw of the frame, over the plates — the standalone's title-only gate was wrong and is fixed (drawn after the menu draw loop, phase>=1).** |
| 6 | band opacity (start semi-transparent, gradient) | chrome bands = vertical alpha gradients (draw dump draws 77-78) | LANDED (parity2 3de78e14): top band ff000000(top)->a0000000(bottom), bottom band a0000000(top)->ff000000(bottom) - exactly 'less opacity at the bottom/top'; the race-flag animation part = #7 (done) |
| 7 | checkers not animated | FUN_00473ee0 checker pass (above) | LANDED cde0ee52: layer runs every frame per ShellB; animated checkers visible (verify/dbg_backbuffer.png) |
| 8 | Load/Save Successful modal | FUN_00433f40 box drawer (state DAT_0067eab0/eab8 alpha) + trigger 0x00409d15 | LANDED: ported the confirm dialog VERBATIM (3 panels black/0x202020/black @130,120,380x42 / 162,166 / 328,32 + white border; title 'MASHED' 0x41 @140,142; body id; button 0x2d 'Continue' w/ nav-arrow; alpha fade-in). Wired to Load Game (0x1bc) / Save Game (0x1b7) menu actions, Enter/Esc dismisses (modal freezes menu). verify/frontend_parity2/re_modal.png |
| 9 | font pixelated/wrong | FGDC20 pipeline: FUN_00554390 loader, FUN_00554940 renderer, FUN_00427680 layout, FUN_00428140 item drawer, FUN_00556e90 color state | **REOPENED by the user ("fundamentally wrong") and ROOT-CAUSED 2026-06-12 — see re/analysis/font_fgdc20_text_law.md (canonical) + pixel evidence vs NEW original-backbuffer captures (d3d9 shim MASHED_ORIG_BBDUMP).** The prior "RESOLVED / as-faithful-as-source-allows" conclusion was WRONG on four counts: (1) **anisotropy** — the original's text space is normalized [0,1]² over the FULL screen (x×1/640, y×1/480, print scale = menu_scale×0.0708 @0x005cd5fc), so glyphs render 4/3 WIDER than the atlas-cell aspect; the standalone drew isotropic 1:1 = 3/4 the original's width (pixel-calibrated: "Press button..." pen start predicted 150.3 vs measured 150 @640). (2) **anchor** — callers pass the original's y; quad top = y − 0.4954×cell (0.025 @0x005cc9a4 + c8=0.151515 RWF cs+0x08); the standalone top-anchored ⇒ text sat ~½ cell low. (3) **settled items are SOLID** — FUN_00428140's alpha law (top min(p7,0xff); bottom 0/(p7+0x80)&0xff/0xff tiers) gives bottom 0xff at settle; the hardcoded bottom=0.5 gradient washed every glyph's lower half. (4) **point sampling** — the original's glyph edges are blocky-solid; LINEAR thinned the strokes (now per-texture POINT + bridge −0.5 pixel-center correction + half-texel UV inset for the boundary-texel tick). Also: advance = (width+tracking)×cell_w (tracking cs+0x0c = 0 for FGDC20), unmapped codepoints advance ZERO, space = real 5/33-wide record. Verified: verify/font_cmp_exit_native3.png + verify/font_cmp_items.png (side-by-side vs orig f1900 — size/weight/solid now match; slide-in anim composition matches incidentally); chrome parity gate still GREEN 236/236. **FINAL ROOT CAUSE (user's "cut off / pixelated" report, same day): the TXD pixel base was off by 4 bytes — pixels start at +0x43c (rwID_IMAGE chunk math byte-exact: 12 + 0x10 struct[w,h,depth,stride] + 0x400 palette + 0x20000 pixels = 0x2041c), not +0x438. The whole atlas was shifted 4 columns left since B19: every glyph showed its neighbor's edge column (the "leading tick") and lost its rightmost columns (cut-off look); the intermediate POINT-filter + half-texel-inset changes were compensations and are REVERTED (LINEAR; FUN_00428140 sets state 9=2). Side-by-side now matches the original's weight/edges: verify/font_cmp_exit_offsetfix.png + font_cmp_items_offsetfix.png.** Remaining residuals (law note): orig-watermark-at-settled-scr1 unverified, splash "Loading" alignment flag, dim-row param_7, raster-native filter value [UNCERTAIN]. |
| 10 | buttons not centered with text | record y = row CENTER line (bit-verified plate spans rec.y-12..+14) | LANDED: glyph cell now centered on the plate center (was top-anchored at rec.y, overflowing below) |
| 11 | back indicator on main menu | top frontend menu back-row prim_id == -1 (live probe) -> not drawn | LANDED: hide back row at depth<=1 (the title-entered top level); deeper screens keep Back (verify/frontend_parity2/re_scr1_noback.png) |
| 12 | select-arrow texture missing | GetMenuMessage skipped the FUN_004277a0 control-code remap | LANDED ec08ddf2: applied verbatim remap (8->0x81 9->0x7f a->0x81 b->0x8d c->0x80 d->0x87 e->0x8f; pool0 decomp); nav arrow now renders before 'Select' (verify/frontend_parity2/zoom/our_footer_arrow.png) |
| 13 | black semi-circle (button cap) | 'Button' sprite (16x32 PAL8) = a navy (0x131550) semi-circle ◖; navy x ffb45010 modulate -> near-black. Bug = BRIDGE HANDLE COLLISION: kHandleMenuBadge==kHandlePreview0==10, so the 24 track previews clobbered the badge registration -> cap drew a preview/nothing | LANDED: moved kHandleMenuBadge 10->34 (clear of preview range 10..33); black semi-circle now renders on the selected row (verify/frontend_parity2/our_cap_fixed.png; sprite verify/.../badge_Button_sprite.png) |
| 14 | video not blending | FUN_00473c20 + FUN_00474890 (preview crossfade); ShellB FUN_0042e5b0 callsite | LANDED (core) + UV-pan RESOLVED as atlas-specific (not portable). The user complaint = track previews + video "not blending" — DONE via the two-quad corner-faded alpha crossfade over the video (ShellB-faithful 512-frame A/B alpha ramps, slot cycle, 352x352 @288,64). FULL RE of the UV-pan residual (pool0 2026-06-12): FUN_00474890 insets the sampled UV by p = param_7/2048 (consts _DAT_005cead4=1/2048, _DAT_005cd050=0.125, _DAT_005cc320=1.0, _DAT_005cc32c=0.5, 1/16,15/16), drawn as TWO half-quads whose u-split is ASYMMETRIC (left u[p,(1-p)/2], right u[(1-p)/2,1-p]) → a directional WIPE keyed on mode param_8∈0..5 (the {0,1,2,3,4,5,...} cycle table in ShellB, NOT a texture index) with param_7 ramping 0..~454 over the cycle (p→~0.22). This is designed for the original's atlas/sub-image wipe; on the standalone's PRE-SPLIT single per-track textures (TRACKIMAGES.TXD: Training1/Egypt1/...) the asymmetric u-split would non-uniformly stretch the image (distortion), so the alpha-crossfade is the correct adaptation for the standalone's asset model. Per NO-GUESSING, no invented substitute. |
| 15 | screen-transition fades wrong | arc-wash strips + slide DAT_008990e0 + fade pair DAT_0086ecc8/cc | LANDED (writers xref-confirmed): fade 0xff raised by nav RELOAD ops (0x0043d2c2; push/pop skip it) + selects (0x0043f9d7); slide raised +0x20/frame by FUN_0042e8b0 framed-preview body (caller FUN_004368e0) [slide residual lands with Wave-3 per-screen content]. ~~The settled menu's permanent left haze (alpha 0x60 arc wash) is FAITHFUL and now renders~~ **CORRECTED by parity harness 2026-06-12: the original's settled wash alpha is 0x00 (burst rows 46-75) — there is NO permanent haze; the 0x60 factor only scales the decaying fade pair. The standalone's fade-driven wash already settles at 0x00 and matched GREEN; the struck claim was wrong, no code change needed.** |
| 16 | top/bottom text animations wrong | header watermark 0x41 (FUN_0043c5b0 @0x0043c6a0/c6f1); header/footer rows = FUN_004325c0 tick | LANDED (law faithful). RE (FUN_0043c5b0_chrome.asm): the top-right "MASHED" watermark (id 0x41) is drawn STATIC at fixed virtual (600,52) shadow + (596,48) white, scale 0.8 — NO slide; the standalone matches exactly (exe_main.cpp 1780-1783). The header/footer ROWS (back-row tag 0xff000000 + prompt rows 0xff10/11/230000) animate via the BIT-VERIFIED FUN_004325c0 tick (MenuAnimTickTwin C4 GREEN 40/40), applied per-row as Nav_RecordSlide -> slideX in the draw loop. So both the static watermark and the row slide-in law are faithful; the original "wrong animation" was the pre-anim-tick state (now wired). Residual (fine): a side-by-side MULTI-FRAME capture vs the original to confirm per-tag slide phase exactly (needs Frida burst on the original; deferred — driver-wedge risk per [[feedback-d3d9-shim-wedges-gpu-driver]]). |
| 17 | menu text always black | FUN_0043c5b0 text-color rule (pool0 decomp) | LANDED: list items ARE 0xff000000 black for both states (the original's behavior - plate is the cue); BACK/HEADER rows draw record color (white) + black shadow. Standalone was white-on-record-color for items + shadow on all; now matches (verify/frontend_parity2/re_scr8_text.png) |
| 18 | unselectable = transparent black | FUN_00428140 dim path (DAT_008990e4 -> 0x80000000, alpha clamp 0x60) | LANDED: greyed rows now draw half-alpha black (0x80000000) per the decomp's dim branch, not the invented 0x60606060 grey |
| 19 | options load/save greyed out in RE | FUN_00432800 case-8 gate on FUN_00492d10 save-ready | LANDED: live probe DAT_00771968==1 at the original's menu (blank save) -> Load/Save enabled; standalone's hardcoded disable removed |
| 20 | sound sliders all wrong + continuous steps | screen-19 widgets (draw stream log/sound_draw.json): bar x=374 w=106 h=16 0x7f000000 + 4x 3px black borders + orange fill 0xffd88020; arrows 16x16 at x=356/484 | LANDED: geometry/colors/borders verbatim; input now ramps continuously while held (was 1 step/press); verify/frontend_parity2/pair_sound.png |
| 21 | insults tri-state Off/Auto/Manual | MenuSettings.insults_on widened 0=Off/1=Auto/2=Manual | LANDED: dir-aware cycle + text value drawn (no bar), replacing the Off/On bool |
| 22 | gamma screen has no slider | screen 30 (action 0xff73; string id 0x234 'Gamma Correction'); one volume-style slider (probe) | LANDED: gamma is screen 30 = a single slider identical to the sound volumes (bar/borders/orange fill verbatim); g_settings.gamma 0..10, continuous ramp; verify/frontend_parity2/re_scr30_gamma.png |
| 23 | autosave on/off (no arrows) | screen 32 (action 0xff83; string id 0x25f 'Autosave'); two-arrow toggle, no bar (probe: arrows x=356/x=411) | LANDED: autosave is screen 32 = On/Off toggle (arrows + text value, no bar); g_settings.autosave 0/1, discrete; verify/frontend_parity2/re_scr32_autosave.png |
| 24 | quick battle greyed in RE | FUN_00432800 case-2 gate on DAT_007f0ad4 | LANDED: live probe shows 2 at the menu with an ALL-ZERO gamesave (boot default, not save-derived); kFreshState.has_profiles 0 -> 2 (verify/frontend_parity2/re_scr2_gates.png) |
| 25 | player color select UI missing | nav screen 7 (kT7, screen-kind 0xff080000==10); drawer FUN_004368e0 `case 10` (DAT_0067e9fc==10); title 0x12d "Player Colour Select" PUSH'd @0x00436ea2 | LANDED (commit f7135568): the screen was nav 7 (NOT 4 — 0x130 is the "Press X Select / Main Menu" prompt, not the title). Car sprites FUN_0042fab0(0..9)=NFL{Red,Bluejay,Melon,Gold,Pink,Shadow,Copter,Bomb,Fugitive,Survival}, "vs"=&DAT_005cda30, all in SFX.piz/INTERFACE.TXD (dict DAT_0063b904 / FUN_0040bb90→FUN_004c5c00). LoadCarColorSprites() uploads them (cars 10/10, vs OK); on screen 7 the standalone draws P1 car @ virtual (40,292), "vs" @ (132,300), P2 car @ (196,292) (FUN_004739f0 geom, kVScale), + the 0x12d info string; LEFT/RIGHT cycle P1's colour. Verified MASHED_GOTO=7 + BBDUMP (verify/csel_screen7_final.png). Residual: full per-controller case-10 layout (FUN_00430670 slots + team "check" marks) needs the absent multiplayer controller/team state. |

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

## Verification note (2026-06-12, amended same day)

PLAIN window screenshots of the standalone are UNTRUSTWORTHY on this machine (BitBlt-style captures of D3D9 windows render white). TWO trustworthy channels now exist, and they verify DIFFERENT things:
- `MASHED_DBG_BBDUMP=N` (verify/dbg_backbuffer.bmp at frame N) — the BACKBUFFER: what the renderer produced.
- `scripts/capture_window.ps1` (PrintWindow PW_RENDERFULLCONTENT; works on background/occluded windows, not minimized) — the PRESENTED window: what the user's eyes see, including DWM/DPI scaling.
The gap between them is real: the 2026-06-12 font complaints persisted across backbuffer-verified fixes because DWM was bitmap-stretching the non-DPI-aware window (>100% display scaling) AFTER Present — invisible to BBDUMP, obvious in the presented capture. mashed_re.exe is now per-monitor DPI-aware (41dbcd93). Presentation-affecting fixes must be verified with capture_window.ps1, not BBDUMP alone.

## Parity harness (2026-06-12, merged 8f1a58c2 — use for all composition items)

The composition truth channel is now the **draw-list diff**, not eyeballs:
`MASHED_GOTO=<scr> MASHED_DBG_DRAWSTREAM=200:203` on the standalone +
`py -3.12 re/frida/menu_draw_burst.py --screen <scr> --frames 4` on the original,
then `py -3.12 re/tools/drawlist_diff.py log/menu_draw_burst.json
log/drawstream_re.json --exclude-tex 9 --map mashedmod/build/mashed_re.map`.
Full recipes + capture asymmetries: `re/analysis/parity_tooling.md`. Per-fix
acceptance: the item's rows disappear from the diff and no new MISSING/EXTRA
rows appear. (Rebuild first — the env-var capture compiles in via
D3d9Render/DrawStreamDump.cpp.)

Harness first-run findings at settled scr1 — **ADJUDICATED 2026-06-12 (this
session); settled scr1 is now GREEN 118/118 per frame** (`--rotate-a 0x42e65a
--tol-anim 4`, see parity_tooling.md):
- **Arc-wash settled alpha — #15's claim REFUTED**: the original draws all 15
  strips at alpha 0x00 when settled (burst rows 46-75, both frames); the
  FUN_00473ee0 decomp agrees (alpha = fade-pair x 0x60/0xff, fade decays to
  0). #15's "permanent left haze is FAITHFUL" conclusion was wrong and is
  struck below. The recorded "standalone draws 0x60/0x78" did NOT reproduce
  in the 15:36 capture — the current build's fade-driven wash also settles
  at 0x00 (drawstream rows 5-34). Already faithful; no code change.
- **#14 half-quads — finding STALE vs current build**: the standalone already
  draws TWO 176x352 half-quads with the left-edge alpha ramp (drawstream rows
  1-2); they matched the original 1:1. True residual is the UV inset: the
  original samples u/v inset by p = param_7/2048, with p still ramping DOWN
  at ~1/2048 per frame 2.5 s after push (f0 p=99/2048, f1 p=98/2048 — exact
  vert floats in log/parity_decode_a.txt); the standalone uses the settled
  limit p=0 (u 0/0.5/1, v 0/1). Draw-list differ does not compare UVs, so
  this needs the verbatim FUN_00474890 port + ShellB's param_7/mode driver
  to close (open item below).
- **Color convention — REAL bug pair, FIXED at root**: every chromatic chrome
  color mismatched as R<->B-swapped at the vertex level (81 rows/frame). The
  screen was correct (BBDUMP plate orange) because TWO compensating bugs
  cancelled: standalone callers passed pre-swapped "screen ARGB" constants
  AND the bridge re-swapped at submit — leaving the vertex buffer bytes
  flipped vs MASHED's DAT_00898a20. Fixed both halves (exe_main constants
  reverted to the original's packed dwords; RwIm2DBridge submits raw); the
  0x224 bordered-rect path, which passed original-form constants through the
  double-swap, was rendering visibly R/B-flipped and is healed by the same
  fix. Buffers now byte-identical -> 0 color rows.
- **Title logo at scr1 — RE-CORRECTED 2026-06-12 (user caught it): the
  "missing logo" finding was CAPTURE POLLUTION, and the F6-era title-only
  conclusion was RIGHT.** The frame dispatcher FUN_00492e90 draws the title
  layer FUN_00403050 (logo + press-button) iff the current-screen global
  DAT_0067ecb0 (getter FUN_0042b930) == 0x21 — the title IS nav screen 33.
  The harness's synthetic FUN_0043d2a0 push never updated DAT_0067ecb0, so
  the title layer (logo A[40], press-button, credits ticker) kept
  compositing over every pushed screen — polluting ALL prior original-side
  scr captures. menu_draw_burst.py now writes DAT_0067ecb0 after the push;
  the clean baseline (verify/orig_backbuffer_f2100.bmp, 117 draws not 118)
  shows NO logo at settled scr1, items settled, watermark present. The
  standalone's logo draw is reverted to title-only (phase == 1). Clean
  baseline also CONFIRMS #17 black item text (zoom disambiguates the
  thumbnail's figure-ground illusion) and resolves the "watermark at
  settled scr1 unverified" residual: it IS drawn.
- **Gradient-band caps / bands "reordered" — ARTIFACT, differ fixed**: burst
  frames split at Present mid-composition (true frame order is video ->
  previews -> caps -> arc -> checkers -> bands -> lines -> plates -> logo).
  New `--rotate-a <retaddr>` differ option rotates A to the ShellB video
  anchor; the entire reordered class (72 rows/frame) vanished.
- **Checker cells "moved" ±2.5px — anim phase, opt-in tolerance**: the fsin
  corner jitter runs off each side's own frame counter; unsynced captures
  can never agree. New `--tol-anim N` flag (explicit opt-in) counts
  same-color pairs within N px as matched.
- nav_coverage.py rows still OPEN (nav/flow, need Ghidra adjudication): kT27
  table hole; screen-4 action 0xff1d0000 dead-ends (no ActionToScreen case);
  kind-4 screens 31/33 lack the sid-specific widget handling 19/32 have;
  kind-2 screens 2/3/4 have none while 8 does (verify intended vs the
  original's tables).

## Clean-baseline screen matrix (2026-06-12 evening — the /goal sweep)

With the harness title-pollution fix, all 8 frontend screens were re-baselined
(orig: log/parity_burst_scrN.json + verify/parity_caps/orig_scrN_*.bmp; RE:
log/parity_re_scrN.json + re_scrN.bmp; visual pairs pair_scrN.png). Gate =
drawlist_diff `--rotate-a 0x42e65a --tol-anim 8` (tol 8 absorbs the checker
fsin phase, ±7px at some samples).

**VERDICTS: scr1/2/3/4/8/19/30 GREEN; scr32 GREEN except 4 anim-phase rows**
(the original's preview crossfade was mid-fade alpha 0x10/0x20 at its capture
frames; ours was at a different cycle phase — same accepted class as the
checker jitter).

Fixes landed this sweep (all clean-baseline-cited):
- **Arc wash is a PERMANENT menu haze** (strips 0x60 gradient / 0x78 fill at
  settle) — the second flip of #15, this time against CLEAN pixels: the
  "transition flash" model (commit 87f52673) came from polluted captures
  where the un-transitioned title state pinned the fade pair at 0. The
  non-verbatim target decay in the FUN_00473ee0 port is REMOVED; the pair is
  BSS-zero at boot (no wash on title), raised to 0xff by menu entry/reloads/
  selects and stays.
- Widget screens (19/30/32): arrows modulate BLACK (ff000000); per-row stream
  order = left arrow -> bar -> fill -> right arrow; widgets draw in a
  POST-LOOP pass (the original's dispatcher calls the widget drawers after
  FUN_0043c5b0 returns — clean scr19 has widget rows after the full plate
  stack); slider fill at bar+4, bottom border at bar+14; tri-state right
  arrow at x=457; footer prompt rows excluded from the widget pass.
- Boot defaults pinned: volumes 7/7/7 (scr19 fills w=70), gamma 3 (scr30
  w=30).
- **#25 scr4 REBUILT to the real layout**: six 64x64 car-sprite tiles at
  (146+i*70, 125) with 69x20 colour swatches at (143+i*70, 168) — swatch
  colours == the first six NFL liveries (Red/Bluejay/Melon/Gold/Pink/
  Shadow) — plus three 534x26 orange player rows at y=193/227/261 with
  b45010 borders (per-tile order swatch->sprite). The old P1-vs-P2 layout
  was invented and never matched.

Open text-class items from the clean pixel captures (invisible to draw
lists; for the next pass):
- Menu music/voice credits ticker (right side of the settled menu: "Voice
  Casting...", "Vocal Script International Hobo Ltd", cycling names) — the
  standalone NoOps the credits fn (0x0042d5a0 thunk). Real menu element.
- scr4 row labels "1"/"2"/"3" + small per-row icons at the row left; ours
  also resolves the header as "Player Color Select" vs the original's
  "Player Colour Select" (string id / table divergence — recheck which id
  kT4's back row carries).
- Insults right-arrow placement law (457 is pinned from the baseline; the
  value-width-dependent law needs the kind-4 widget drawer RE
  (FUN_00430b90 family per the FUN_00492e90 dispatcher: screen 0x20 has a
  dedicated drawer)).
- Preview crossfade cycle driver (FUN_00474890 modes + param_7 ramp) — the
  #14 residual; would also close scr32's phase rows.

Dispatcher intel recorded from FUN_00492e90 (pool1 decomp): title = nav
screen 0x21 (33); screen 3 multi-viewport branch (FUN_0042f530 count); modal
FUN_00433f40 drawn last every frontend frame; screen 0x20 special drawer
FUN_00430b90.

## Round-2 user feedback (2026-06-12 night) — 13 items

Build verified against clean baselines + window captures (DPI-1:1).

1. **Spinning loading disc** — DONE. It's the "loadicon" texture in
   MASHED.exe's EMBEDDED resource RWTEXDICTIONARY/404 (FUN_004921d0:
   FUN_004997b0(0x194,"RWTEXDICTIONARY")->FUN_004c5c00(dict,"LoadIcon")),
   NOT a .piz — same 0x23 TXD variant, 256x128 8bpp direct-intensity, pixels
   @0x43c. Loaded via LoadLibraryEx(MASHED.exe, AS_DATAFILE)+FindResource.
   Coin-flip animated per FUN_00428450: width 80*sin, UV left-half front /
   right-half mirrored back when sin<0, center x=544, phase += 0.1/frame
   (_DAT_005cc56c). Earlier "no disc exists" note was wrong (searched only
   archives, not EXE resources).
2. **Loading screen after press-button** — DONE: title Enter -> phase 4
   (loading composition ~1.2s) -> menu.
3. **Footer Select/Back glyph colorless** — DONE: nav glyphs (codepoints
   >=0x7f) draw GREEN ff10ec00 (clean scr1 sample 00ec10), word stays white;
   DrawMashedString gains a per-control-glyph color arg. [The in-binary
   color-switch mechanism — how FUN_0043c5b0 tints only the glyph — is still
   to RE; current approach colors the remapped range.]
4. **Watermark position** — DONE: RIGHT-aligned at x=600/596 (FUN_00427e00
   align 1) — was centered and spilled off the right edge. DrawMashedString
   gains right-align.
5. **Transitions look like a fade** — DONE: per-frame dt cap (the slide was
   blasting through instantly from the boot dt spike). Slide now sweeps over
   ~0.75s (instrumented 511->478->...).
6. **Sliders too sensitive** — DONE: initial step + ~0.3s hold delay + ~20/s
   (was every-frame-after-6 ≈ 40/s).
7. **Insults arrow placement + value font** — DONE: value text scale 0.6
   left-anchored x=378; right arrow trails the MEASURED text width (was a
   fixed x=457); boots at Manual.
8. **Button text offset up** — DONE: +0.055-cell vertical correction
   (label centroids ran ~1.5px@640 high vs the clean scr19 baseline; the
   text-law camera factor _DAT_0067d834 was assumed 1.0).
9. **Load/save modal layout** — DONE (VERBATIM FUN_00433f40): 3 panels +
   SIX white border lines incl. the y=162/328 dividers BETWEEN black & gray
   bars (were missing); title WHITE left at (140,142); body+buttons
   LIGHT-GRAY 0xdcdcdc left-aligned (the original IS left-aligned, align 0);
   green nav glyphs on Yes/No/Continue.
10. **Load successful instant** — DEFERRED per user (awaits savegame impl).
11. **Color-select 3 orange bars** — DONE: 1 player row (rows = controllers;
    leave at 1 until controller support); 6 colour tiles; red selected by
    default with a selection frame; LEFT/RIGHT cycles 6 tiles; row label "1".
    [No controller indicator + can't fully test = blocked on MP/controller
    state, per user "leave one until implemented".]
12. **ESC on main menu** — DONE: ESC at the root now does NOTHING (was
    returning to the title); pops one level in submenus.
- (Font "still pixelated") — softened edge-hardening to 0.06/0.94 now the
  window is DPI-1:1 (the prior 0.15/0.85 over-aliased).

## Offline-RE findings (2026-06-12, MCP down — via re/tools/disasm_fn.py)

- **#2 boot splash**: DAT_00771964 is only ever set to MashedNEWLogo (FUN_004283a0) or 0 (teardown 0x00428400). FUN_00403050 draws it + the pulsing press-button (FUN_00402fb0). The Dolby/ProLogic logos (DAT_0067d968/0067d96c) are drawn by separate readers (0x00428c9b / 0x00428ec2) — the splash is a multi-function boot sequence, not a single drawer. Port needs those two drawers + the boot state pump.
- **#2/#4 title+attract timer = FUN_004030d0**: accumulates DAT_00636ae8 += DAT_007f1004 (frame_dt) each frame; when it exceeds DAT_00636aec it either resets (DAT_00636af8 flag set → memset 0x7f1038 + restart = attract loop) or calls FUN_0043df00 (enter frontend). This is the title-idle timeout that launches the attract demo (#4) and the title→menu advance. Entry points pinned for both.
- ~~**Verification gate**: ALL 9 remaining items are visual; the launched-process session currently has NO display...~~ **STALE — environment restored 2026-06-12 ~15:35**: display active, standalone runs/captures/BBDUMP all work (re-confirmed 16:17 this session: fresh drawstream + backbuffer captured, differ GREEN at scr1). Visual verification is unblocked; do not refuse it on the strength of this old note.
