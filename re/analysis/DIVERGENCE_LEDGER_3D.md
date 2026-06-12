# 3D divergence ledger — standalone vs original (2026-06-10 reconciliation)

References: `verify/parity3d/orig_race_t01..t05.png` (original Quick Battle,
captured live via `re/frida/race_refs.py` — t01 = in-race wide shot, t03 =
the REAL "Current Standings" screen) vs `verify/parity3d/sa_race_dump.png`
(standalone, same piz, after the fog/sky/alpha gap closures).

Legend: **[GAP]** = renderer feature missing/cheap to add · **[SCAFFOLD]** =
invented behavior that must be REPLACED by an RE'd port · **[DATA]** = needs
more format/RE work · ✅ = closed this pass.

## Ranked divergences

1. **Fog / distance haze** — ✅ CLOSED: `Setup_Fog(start_frac, end, r,g,b)`
   parsed from COURSE.LUA → D3D9 linear vertex fog + fog-colored clear.
   Verified on dump (grey haze matches the reference's depth falloff).
2. **Sky** — ✅ wired (`Sky_Filename` → sky clump drawn first, z-write off,
   unfogged). NOTE: per-track verification pending; the original's sky has
   cloud layers/UV scroll that a static clump won't animate. [GAP residue]
3. **Distinct cars** — ✅ CLOSED 2026-06-10: real selection ported
   (FUN_0040d110): all four cars are LIVERIES of one model (vehicle table
   0x005f37a8, 6 DFF variants per piz, car = (id/6)*6 + livery). Standalone
   AI cars now load Advantage1..3 (sa_liveries_t05.png: green/blue/red pack).
   Residual adapter: livery-per-player default = player index (mode-0xb
   verbatim); DAT_007f1a1c writer untraced.
4. **Camera** — ✅ CLOSED 2026-06-10: the real shared race camera
   (FUN_00446520) verbatim-ported to Race/RaceCamera.cpp — pair framing,
   LED.piz per-gate angles, zoom/distance/pitch laws, springs, sway.
   Validated against a live original trace (zoom 286/286 rows; offset/
   pitch within hmix margin). The orbit/free cameras remain as dev-viewer
   tools only. Residual: countdown/start uses the race cam (original may
   use a start cinematic — unverified).
5. **Scoring/HUD** — ✅ DATA CLOSED 2026-06-10: the real points system is
   ported (FUN_0040eee0 + FUN_0040b290 + Race::EvaluateResult 0x00410510):
   per-elimination awards (4P: −2/−1/+1-capped/+2), score floor 0, signed
   delta + 6000 ms flash, elimination order, match win at score > 11.
   Standalone rounds verified (mashed_re.log: runner-up +1 / winner +2
   accumulation). [residual: PRESENTATION — the standings drawer
   (FUN_0043a610 lobby panel; 0x0041af50.. HUD sprite cluster; badge
   sprites/bars layout) still approximated, not verbatim]
6. **Elimination rule** — ✅ CLOSED 2026-06-10: the real rule is neither
   ">7 gates" nor literally screen-edge: a car dies when the camera's
   REQUIRED ZOOM saturates at 10.0 (cam+0x9a0, checked by fcomp at
   0x00410ee3), and the victim is the lesser-progress member of the
   most-separated pair (wrap-adjusted 80/20/100). Ported verbatim
   (RaceCamera::EliminationCheck); empirically confirmed live (zoom hit
   10.0 → race-phase 6→7).
7. **Handling** — velocity/grip/drag rates harvested from live telemetry but
   the force pipeline is approximated; input-matched diff deferred (needs an
   in-race input injector — the menu input override does not reach the race).
   [SCAFFOLD + DATA]
8. **AI driving** — gate-ribbon + lanes + corner braking is invented; the
   original follows AI.BSP with its own controller. [SCAFFOLD → RE]
9. **Lighting on vehicles** — original applies `Vehicle_Shininess_Range`
   lighting; standalone uses raw DFF prelight (cars look flatter/darker).
   [GAP — Option-B port STARTED 2026-06-10: Lua handler located at
   0x0047ac80 (registered at 0x00440d40 via 0x0047b980), stores (min,max)
   to course-ctx `[DAT_006bf1cc]+0x204/+0x208`; ctx init 0x0047a020
   defaults (0.0, 1.0). Consumer holds the ctx ptr in a register — next
   step: Frida read-watch on ctx+0x204 in a live race to find the
   lighting application site.]
10. **Powerups, shadows, particles, skid marks, UV anims (Sea.uva), Mist/
    windows overlays** — absent entirely. [DATA/feature — out of scope here]

## What is faithful (keep)

World geometry/textures/prelight (13/13 validated), collision, props+MTS
placement, AI gate positions, DFF models, fog parameters, the piz/TXD/DAT
pipeline — i.e. the entire DATA layer. Divergences above live in the
RENDERER and BEHAVIOR layers only.

## FRONTEND divergences (added 2026-06-10 after user review — the parity pairs
## verify/parity/parity_gts.png + parity_sound.png are NOT visually faithful)

F1. **Menu background** — CORRECTED 2026-06-10: the moving backdrop is
    **VIDEO** — `toastart/pc/movies/frontend.mpg` (MPEG-1 PS, 512x512 @30fps,
    38.7 MB; untouched by the intro-skip patch). The original plays it via
    DirectShow ("Could not run the DirectShow graph!" strings @0x005cfb18)
    into the menu scene — `Common/Frontend.piz` MAIN.BSP (materials
    'main'/'carbase'/'rendergs') is the menu STAGE; the 512x512 video is
    texture-sized for its 'main' surface. The "car driving around" in the
    burst is video content. Standalone shows a checkerboard fallback.
    [BACKDROP CLOSED 2026-06-10: DirectShow-to-texture playback landed
    (D3d9Render/MpegVideoTexture, the original's own mechanism — graph
    RUNNING 512x512; verify/frontend_parity2/sa_video_menu_t06.png shows
    the real dockside scene behind the menu). Residual RESOLVED 2026-06-10 (user-directed probe,
    re/frida/mainbsp_probe.py): the world-render path (FUN_00478cd0 /
    FUN_004270f0 / guard DAT_0066d704 / world slot DAT_00646e58) NEVER
    fires through menu depth 4 — MAIN.BSP is NOT a menu component.
    Deferred to the vehicle-select 3D stage work. Video loop verify still
    pending]
F2. **Menu item plates/chrome** — ✅ CLOSED + PROMOTED 2026-06-10: the menu
    draw loop FUN_0043c5b0 is BIT-IDENTICAL (nav-driven draw-sequence diff
    GREEN 20/20, 117 draws/frame; C2→C3, twin MenuDrawLoopTwin.cpp). The
    diff recovered the real plate language: 5-piece border kit on EVERY
    row, right-fade gradient at 60+plate_w, Button cap at x=45, persistent
    color scratch with FUN_0042aad0 (EAX-arg) as the per-row grey/alpha
    engine, single 7-arg text call. Standalone geometry backported
    (sa_final_t07.png). [residuals: per-vertex gradient fade in the
    standalone bridge (flat-quad stand-in, tagged); the FUN_00432b30
    prompt strip port; list-screen 0.578-scale nuance]
F3. **Animated logo** — ✅ CLOSED + PROMOTED 2026-06-10: BIT-IDENTICAL to
    the original (nav-driven draw-sequence diff GREEN 20/20, 74 Im2D draws
    per frame byte-compared; FUN_00473ee0 C2→C3). The diff surfaced a
    MISSING ANIMATION TERM — the x-sine wave (amp = f64 4.0 @0x005ce1f8,
    phase col+row, corner offsets {0,1,1,2}) — plus the reciprocal scale
    chain, bit-exact strip constants, and the x87 single-rounding sin
    chain; all ported. Title also uses the right asset (MASHEDNEWLOGO.PNG)
    at burst proportions. Bit-identity supersedes the planned frame-by-
    frame pixel validation.
F4. **Footer strings** — ✅ CLOSED + PROMOTED 2026-06-11: FUN_00432b30
    verbatim-ported. Hook twin (PromptStripTwin.cpp) bit-identical GREEN
    264/264 (synthetic on-game-thread sweep, all 10 keys × b920-gate sides
    × modes 0/1/2; jump tables re-derived from binary dwords at
    0x433060/0x433088..178 — the diff caught 4 mistranscribed rows) →
    C3 → C4 (subset-install canonical observe ×3, JMP-LIVE, organic
    boot+Enter nav fires; re/analysis/standalone_menu_sm/
    c4_promptstrip_20260611.md). Standalone port: PromptStripAppend in
    MenuNavSM.cpp at the Nav() Phase-7 call sites (mode 0 push / mode 1
    pop-reveal rel via the depth+1 popped slot / mode 2 reload appends
    nothing), rows drawn in the exe_main record walk under the
    bit-verified prim/sec type rule. Suppression REMOVED. Verified:
    verify/f4_promptstrip_standalone_screen1.png ("Select", root push,
    settled DEF row sec=0x42) + ..._screen8.png ("Select Back", key2 row
    settled sec=0x43). Organic semantics pinned by probe: key = SHOWN
    screen kind, rel = the OTHER side's kind, cmp = pushed screen id
    (gate vs 0x16 hides the title-push glyph).
F7. **Panel.piz** — RESOLVED-OUT 2026-06-10: the menu draw loop does NOT
    reference Panel.piz. The item plates are COLORED QUADS (the FUN_0043c5b0
    fill immediates, ported); Panel.piz contents (RACEPANEL/STARTLIGHTS/
    TEAM*PANEL/PANEL0..3 DFFs) are in-RACE HUD panels — they belong to the
    race-HUD presentation work (ledger #5 residual), not the frontend.
F6. **Boot screen/items** — found 2026-06-10 (parity2_menu.png): standalone
    shows screen 0's records (Continue/Restart Race/...) at boot; the
    original gates item drawing on the frontend PHASE (DAT_0067eca4 ladder
    in FUN_0043c5b0: ==1 minimal, 2..3 items) so the TITLE phase shows the
    logo only, and title-confirm pushes screen 1 (the real main menu —
    Single Player/Multi Player/...). Fix = port the phase ladder + the
    title-confirm push (part of the F2 draw-loop pass). [SCAFFOLD gap]
F5. **Animation reference captured** 2026-06-10: 40 frames @50 ms of the
    original main menu (verify/frontend_ref/menu_burst_*.png +
    menu_burst_montage.png) — acceptance reference for F1–F3. (The earlier
    "no video in the menu path" note here was WRONG — superseded by the
    corrected F1: the backdrop motion is frontend.mpg video.)

## Frontend parity2 (2026-06-12) — chrome from the decoded draw stream

New evidence lane: `re/frida/menu_draw_dump.py` decodes the original's full
menu Im2D vertex stream (117 quads/frame, exact x/y/w/h + per-corner ARGB in
640x480 backbuffer coords) — `standalone_menu_sm/menu_draw_dump_20260612.json`.
Beats window-pixel forensics (capture scaling ambiguities) and found errors
eyeballing could not.

CLOSED this pass (all verbatim from the dump; commit 3de78e14):
- right-side white wash (fade 288..464 + solid 464..640, y 64..416) — was
  absent entirely;
- band-edge black fades (384..512 over both bands);
- top/bottom bands as vertical ff->a0 gradients; divider lines 1px at 64/416;
- video backdrop 512x512 at origin 1:1 (was fullscreen-stretched);
- plate right-fades as true horizontal alpha gradients (HudIm2DQuadCorners);
- idle/disabled border navy ff131550/30131550 (R/B was swapped);
- back/header row: no plate chrome + record color (tint was invented);
- text cell = scale x 0.0708 (_DAT_005cd5fc @0x005cd5fc) x 480.

OPEN residuals (each with its exact next step):
- **Checker-flag bands** (dump draws 35..76: ~21px textured quads x3 rows,
  top-right + bottom-right, ff808080 modulate, wave-jittered): need the
  checker texture identity (not in the Frontend.piz 8-texture set by name)
  + the jitter source (sibling of the FUN_00473ee0 wave). Until then the
  corners show the plain band gradient.
- **15-strip curved wash** (draws 5..34: strip h=512/15, curve min 294.2 at
  mid — the "0.578" nuance): vertices carry alpha-0 colors, so the visible
  effect depends on the original's blend state at that point; needs the
  render-state recorded per draw (extend the dump with state 8/6/0xc + blend
  mode reads) before reproducing.
- **Text metrics**: cell height is now binary-cited but glyph rendering still
  visibly differs (original heavier/larger). Exact route: capture the
  original's RtCharset vertex submissions (text quads do NOT pass through the
  0x00898a20 scratch — different pipe) and calibrate DrawMashedString's
  cell->glyph mapping against them.
- **Plate slide-in animation shape**: the bit-verified draw loop animates
  plate geometry off the slide counter (MenuDrawLoopTwin.cpp holds the exact
  form); the standalone still maps slide to a horizontal text offset. Port
  the twin's slide branch into the record walk.
- **Standalone goto=1 boot flake** (dev harness only): a single-screen
  MASHED_GOTO sometimes lands on root records; chains ("19,1") always land.
  Phantom UP/LEFT input is also visible in the B13 debug title across fresh
  boots (drifting controller or injected-key residue) — investigate before
  trusting interactive standalone captures.
