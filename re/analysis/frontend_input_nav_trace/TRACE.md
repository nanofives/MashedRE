# Frontend menu input → navigation trace

Date: 2026-06-07. Slot: Mashed_pool13 (read-only). All addresses verified via Ghidra MCP
on the anchored MASHED.exe. NO-GUESSING: every claim cites the RVA where it appears.

## Question

Earlier in-process input feeders (DI GetDeviceState buffer write, PostMessage WM_KEYDOWN)
both delivered input but the 7 "navigate" suspect handlers never fired and the menu never
visibly moved. Goal: trace the menu state machine and identify the input source it
*actually* reads, so the navigate canonical scenario can be driven in-process.

## The chain (bottom → top)

### 1. Raw keyboard read — DirectInput `GetDeviceState(256)`
`FUN_00496100(out_bitfield)` @ 0x00496100 (C2):
- `(**(code **)(*DAT_00772fb8 + 0x24))(DAT_00772fb8, 0x100, buf)` — this is
  `IDirectInputDevice8::GetDeviceState(cbData=0x100=256, lpvData=buf)`. vtable offset
  0x24 = index 9 = GetDeviceState. On failure it calls `+0x1c` (index 7 = Acquire) then
  retries (0x00496153, 0x0049615f area).
- Device object = `*DAT_00772fb8` (0x00772fb8).
- Packs: for scancode i in 0..0x100, if `buf[i] < 0` (high bit 0x80 = key down) sets bit i
  in the 32-byte output bitfield (`out[i>>3] |= 1<<(i&7)`), loop @ 0x004961b0..0x004962f0.

**=> The frontend keyboard source IS DirectInput `GetDeviceState(256)`** — the exact call
the prior `input_feed.py` hooked. cooked into a 32-byte DIK-scancode bitfield.

### 2. Per-frame input manager
`FUN_004967e0` @ 0x004967e0 (C2):
- `FUN_004972b0()` @ 0x004972b0 — snapshots joysticks into `DAT_0077311c` (axes) /
  `DAT_007730d4` (buttons), then `memset(&DAT_0077313c,0,0x20)` + `FUN_00496100(&DAT_0077313c)`
  → fills keyboard bitfield **`DAT_0077313c`** (0x0077313c, 32 bytes).
- A second bitfield `DAT_00773058` (0x00773058) is filled the same way for direct system-key
  checks (extracted into `DAT_00773000..` flags at 0x004968xx; scancodes F1.. and 0x02..).
- Loops player `iVar2 = 0..3`: if `FUN_00497450(iVar2) != 0` (player active) → `FUN_00496530(iVar2)`
  cooks that player; `DAT_00772ffc` counts active players. Loop @ 0x0049684f..0x00496875.

### 3. Per-player input cook → edge flags
`FUN_00496530(player /* EBX */)` @ 0x00496530 (C2):
- Zeroes the player's 0x4c-byte flag block (`&DAT_007f1038 + player*0x13` dwords = 0x4c bytes).
- Calls `FUN_00497310(player, control)` repeatedly; for the 4 direction controls writes
  0xff into the player edge flags when pressed:
  - `(&DAT_007f1044)[player*0x4c] = 0xff`  — control "left"   (0x0049659e)
  - `(&DAT_007f1045)[player*0x4c] = 0xff`  — control "right"  (0x004965bf)
  - `(&DAT_007f1046)[player*0x4c] = 0xff`  — control "up"     (0x004965e0)
  - `(&DAT_007f1047)[player*0x4c] = 0xff`  — control "down"   (0x00496601)
  (also analog/digital fields 0x7f1040..0x7f1051 earlier in the fn).

### 4. Control resolver (the gate that defeated the old feeders)
`FUN_00497310(player, control)` @ 0x00497310 (C2):
- `(&DAT_007e96fc)[player*0x80]` = player device **type**: `1`=joystick/analog, `2`=keyboard.
- type==2 (keyboard): scancode = `(&DAT_007e96c8)[player*0x80 + control]`; returns 0xff iff
  bit `scancode` set in **`DAT_0077313c`** (`(&DAT_0077313c)[scancode>>3] & (1<<(scancode&7))`)
  @ 0x0049741c.
- type==1 (joystick): tests joy-button bitfield `DAT_007730d4` indexed by the same mapping.
- type!=1,2 (0/none): returns 0 → no input.

Per-player config struct base `DAT_007e95c0` (0x007e95c0), stride player*0x80 dwords (0x200 B):
- +0x108 (`DAT_007e96c8`): control→scancode map array (control index → DIK).
- +0x13c (`DAT_007e96fc`): device type (1 joy / 2 kbd).
- +0x140 (`DAT_007e9700`): joystick id.

### 5. Frontend tick consumes the edge flags
`FUN_0043dfd0` @ 0x0043dfd0 (C1) — the per-frame frontend state machine (phase var
`DAT_0067eca4`: 1=fade-in,2=fade-out,3=active). In phase 3 it reads the edge flags for ALL
players directly, e.g. `DAT_007f1046`/`DAT_007f1047` (up/down) at 0x0043e144+, with the
+0x4c-stride siblings 0x7f1090/0x7f10dc/0x7f1128/0x7f1174/0x7f11c0/0x7f120c/0x7f1258 for
players 1..7. Confirm/select routed through `FUN_0042ae10`/`FUN_0042ac90`. Page handlers
`FUN_0042aff0/b180/b310/b770` (the prior "navigate" suspects) also read these flags +
`DAT_0067e9f8`.

### 6. Menu structural state + draw
- `DAT_0067e9f8` (0x0067e9f8) = **menu-stack depth** (NOT a cursor). `FUN_0043d2a0(menu_id, mode)`
  @ 0x0043d2a0 is the submenu push(mode0)/pop-2(mode1)/reuse(mode2) builder; ++/-- the depth.
- Per-column selected row = `*(int *)(&DAT_0067ed40 + DAT_0067e9f8*0x40)` (0x0067ed40).
- Draw `FUN_0043c5b0` @ 0x0043c5b0 iterates item table 0x00898aec..0x00899104 and renders the
  highlight from that selection state. Called by `FUN_00492e90` (frontend render tick).

## Why the prior feeders failed (resolved)

- **DI-buffer feed (`input_feed.py`)**: OR-ed arrow DIK bytes into the GetDeviceState 256-byte
  buffer — the *correct* stage (step 1 sees the high bit, packs the bitfield). BUT step 4
  gates on the active player's device-type being keyboard(2) AND the injected DIK equalling
  the player's *mapped* scancode `(&DAT_007e96c8)[player*0x80+control]`. If the menu's active
  player isn't keyboard-typed, or arrow-DIK ≠ the mapped key, nothing propagates. (Runtime
  values of `DAT_007e96fc[player]` / the mapping are config-dependent; inspect live.)
- **WM_KEYDOWN PostMessage**: irrelevant — the frontend nav path never reads the Windows
  message pump. WM_KEYDOWN was dispatched but no nav code consumes it.

## Cleanest in-process injection (gate-bypassing)

Write the **cooked edge flags** `DAT_007f1044 + player*0x4c` (player 0 = 0x7f1044/45/46/47)
*after* `FUN_004967e0` finishes cooking and *before* `FUN_0043dfd0` reads them. Hook point:
`Interceptor.attach(0x004967e0, {onLeave})` and set the byte for one frame (tap), then clear.
This bypasses device-type / active-player / scancode-mapping entirely (those only affect how
the flag would be *produced*; the frontend reads the flag itself). 100% in-process — writes the
game's own globals, never touches the OS input queue. Tool: re/frida/input_feed_cooked.py.

Observation hook to prove navigation: read selection index
`*(int*)(0x0067ed40 + (DAT_0067e9f8 * 0x40))` before/after each tap; it should change.

## Menu lifecycle (observed 2026-06-07, re/frida/menu_nav_observe.py, stock spawn, no input)

Phase var `DAT_0067eca4`: observed values 0,1,2,3,4 (1=fade-in, 2=fade-out, 3=active;
0=pre/attract; 4=attract-trigger transition set when `DAT_0067e914 != 0`, FUN_0043dfd0:197).

```
t=  257ms phase=0 depth=0      boot
t= 4611ms phase=1 depth=0      fade-in
t= 4625ms  FUN_0043d2a0(id=0x16, mode=0)   <- main menu pushed
t= 4862ms phase=3 depth=1      ACTIVE main menu
   ... interactive-idle ~4.9s .. 24.5s (~20 s window) ...
t=24541ms  FUN_0043d2a0(id=0, mode=2)
t=24650ms phase=4 depth=1      DAT_0067e914 flipped -> attract trigger
t=24901ms phase=0 depth=1      attract demo running
t=38929ms phase=2 ; t=39180ms phase=3 + FUN_0043d2a0(id=0x16) -> menu returns
```

Tick `FUN_0043dfd0` ran 3873x in 40s (~97/s). **Correction to prior belief:** the main menu
is interactive-idle from ~5s to ~24.5s — NOT "menu only briefly up at 14-16s then attract."
The earlier feeders' timing was fine; the failure was not a timing problem.

The 7 prior "navigate" suspects (0x42aff0/b180/b310/b770/ee00/ebe0/430a10) fired **0x** across
the entire window — they are NOT the main-menu (id 0x16) handlers.

## In-process injection result (re/frida/input_feed_cooked.py) — clean NEGATIVE

Wrote the cooked flags directly in `Interceptor.attach(0x004967e0,{onLeave})` during the
confirmed interactive window (settle 8s). Tested progressively:
1. edge flag `0x7f1047` (player 0 down) only — no change.
2. all 4 directions, player 0, broad probe of 8 selection globals — no change.
3. **all reps × all 4 players**: edge flags `0x7f1044+p*0x4c+ctrl` AND working bytes
   `0x7f1038+p*0x4c+ctrl`, for p=0..3 — **still no change.** No selection global moved,
   depth stayed 1, suspects 0.

Since the bytes are written directly (bypassing the cook), the flag block IS 0xff when any
consumer reads it. => the main menu's row-movement is **gated/driven on the consumer side**,
not satisfiable by the digital flag block alone.

Candidates for the actual directional consumer (next RE layer, if pursued):
- the analog float fields `0x7f104c`/`0x7f1050` (per-player, written by FUN_00496530 from
  axis deltas) — directional nav may read these, not the digital bytes.
- proper edge-pair: current `0x7f1042/44+` set AND previous-frame copy `0x7f1502/04+`
  (the `0x7f14f8` snapshot) clear — FUN_0042ae10 @ 0x0042ae10 does exactly this for the
  ACTION/confirm button (0x7f1042). The directional consumer likely mirrors it.
- the consumer-side gate: tick line ~200 `if (FUN_004325c0()==0 || DAT_0067eab0!=0) skip`.
  `FUN_004325c0` @ 0x004325c0 returns 1 once item animation settles; `DAT_0067eab0`
  (0x0067eab0) is an unidentified nav-enable gate — confirm it is 0 during the window.
- the id-0x16 page handler inside the FUN_0043dfd0 switch that owns the selected-row write
  (writer of `*(int*)(0x67ed40 + depth*0x40)` = 0x67ed80 at depth 1) — not yet pinned.

## 2026-06-07 (cont) — resolver-override drives the menu; directional = analog axes

Built re/frida/input_resolver_drive.py: hooks FUN_00497310(player@esp+4, control@esp+8)
(cdecl, confirmed by --probe: players 0..2 active, control indices 0..12) and overrides its
return to 0xff for chosen (player,control) through the LEGITIMATE pipeline (so the cook +
edge-detect + gates are all satisfied).

- **--sweep** (force each control, watch selection/depth): forcing **control 4** for player 0
  caused depth 1->2 (submenu PUSH) => **control 4 = CONFIRM/select**. Control 6 also pushed.
  => the per-player pipeline DOES drive the menu; earlier direct-flag injection failed only
  because it wrote the wrong representation. No single control moved the selection *row*.

- **--dumpmap** (read the live per-player binding table): ALL 4 players have **type=1
  (joystick)**, control map holds joystick indices {0,1,2,3,4,5} for controls 1-3,5-8, and
  **controls 9-12 are unset (scancode 0)**. Cross-ref FUN_00497310: switch **cases 9/10/11/12
  threshold the analog axes fVar1/fVar2 (DAT_0077311c) vs DAT_005d757c** — i.e. controls 9-12
  are the ANALOG directional controls (stick up/down/left/right), NOT digital buttons.

**=> Directional menu nav is driven by the analog STICK axes (DAT_0077311c), which are 0 with
no joystick attached.** That is the real reason no digital flag write / single FUN_00497310
override moved the selection row: directional input on this config has no digital binding.
The menu sits idle (no direction) until the attract timer — consistent with the lifecycle.

Working in-process injection paths (all OS-input-free):
- CONFIRM/select: override FUN_00497310(player, 4) -> 0xff (PROVEN: submenu push).
- DIRECTIONAL: drive the analog axis snapshot DAT_0077311c (per-joystick, FUN_004972b0 fills
  it from FUN_004957a0), OR override FUN_00497310 cases 9-12 to return their analog "pressed"
  sentinel (case 11/12: CONCAT31(..,0xff); case 9/10: FUN_004a2c48()). NEXT STEP to test.
- ALT faithful path: set a player to keyboard — write type=2 at (&DAT_007e96fc)[p*0x80]
  (0x7e96fc+p*0x200) and arrow DIK scancodes into (&DAT_007e96c8)[p*0x80+ctrl], then drive
  the keyboard bitfield DAT_0077313c. Untested.

## Focus-pause patch (2026-06-07)

MASHED pauses when its window loses focus: main-loop pump FUN_00499690 (0x00499690) calls
`WaitMessage()` (0x004996d5) whenever the focus flag DAT_0077391c==0 (set by WndProc
FUN_00499820 WM_ACTIVATE). DAT_0077391c has exactly one reader, so that is the only gate.
scripts/patch_mashed_no_focus_pause.py flips the JNZ (75) at 0x004996d3 to JMP (eb) so
WaitMessage is always skipped -> game runs full-speed when unfocused (reversible: --restore;
trade-off: spins a CPU core while unfocused). Lets background input testing run while the
user multitasks. NOTE: during Frida spawns the window had focus anyway (tick ~107/s), so the
pause was NOT the cause of the directional-injection negative — that was the joystick-default
config above.

## 2026-06-07 (cont 2) — SOLVED: in-process menu navigation, visually proven

The earlier directional negatives were because depth 1 (menu id 0x16) is the **TITLE / press-
start screen** (MASHED logo splash, verify/menu_baseline_pw.png) — NOT an item list. So
directionals at depth 1 do nothing; the title only takes confirm.

Driving via input_resolver_drive.py --sweep (override FUN_00497310(player,control)->0xff, one
clean step per dwell-slot via --list), the full flow:
1. confirm (control 4): title (depth 1) -> Game Type Select (depth 2), but a modal
   "MASHED / Load Successful. / Continue" is up (verify/menu_depth2.png).
2. confirm again: dismiss the modal -> clean Game Type Select (verify/menu_nav_test.png):
   Single Player(0) / Multi Player(1) / Options(2) / Bonus Features(3, LOCKED/greyed) /
   Exit To Windows(4).
3. control 12 (DOWN) / control 11 (UP): move the selection. Observed cycle 0,1,2,4 — SKIPS
   index 3 (locked Bonus), exactly matching the on-screen greyed row.

**Confirmed control map (player input control index -> menu action):**
- **4 = CONFIRM/select**  (digital; FUN_00497310 default/analog case)
- **11 = UP**, **12 = DOWN**  (FUN_00497310 analog-axis cases 11/12; vertical list nav)
- 9 / 10 = analog LEFT/RIGHT (horizontal; for Options sliders — not yet exercised)

**Visual proof:** scripted `--list 4,4,12,12` (confirm,confirm,down,down) -> selection
0->1->2, screenshot shows **Options highlighted** (verify/menu_nav_options.png). Fully
in-process (Frida return override on FUN_00497310), ZERO OS input.

### How to drive the menu in-process (reusable)
```
py -3.12 re/frida/input_resolver_drive.py --sweep --list <controls> --settle 7000 \
    --seconds N [--shot verify/x.png --shotat <sec>]
# controls: 4=confirm 11=up 12=down 9=left 10=right ; e.g. 4,4,12,12,4 = enter Options
```
Each list entry fires once per 1.5s dwell-slot. Selection index read at
*(int*)(0x67ed40 + DAT_0067e9f8*0x40). PrintWindow screenshot works even when occluded
(no focus change), so it composes with the focus-pause patch for background testing.

### Implication for C4
The navigate canonical scenario is NO LONGER a dead end. A scenario can now: boot -> confirm
past title+modal -> navigate to a target item -> confirm, all in-process, and observe the
hook under test on the resulting screen. This is the OS-input-free driver the C4 navigate/
save rows were waiting on.

## C4 navigate harness (2026-06-07)

re/frida/canonical_c4_navigate.py wires the nav driver into the canonical_c4_verify.py
evidence model: it runs a "boot -> navigate to screen X" scenario TWICE (OFF: installs
nothing, Interceptor-counts the candidate while navigating, proves exercised-on-scenario; ON:
MASHED_HOOK_ONLY=<candidates>, confirms each inline-JMP live (0xE9), navigates, survives), and
emits the exercised+installed+survived trio per candidate. Navigation is the FUN_00497310
return override (control 4=confirm, 11=up, 12=down) — fully in-process, zero OS input. Optional
PrintWindow screenshot of the ON run (works occluded; pair with patch_mashed_no_focus_pause.py).

```
py -3.12 re/frida/canonical_c4_navigate.py 0xRVA[,..] --nav 4,4,12,12 \
    [--settle 7000] [--dwell 1500] [--seconds 26] [--shot verify/x.png --shotat 18]
```

Demonstrated on MenuChromeShellA (0x0042e3a0) + MenusBodyA (0x0042d5a0) with --nav 4,4
(confirm past title + "Load Successful" modal -> Game Type Select): OFF exercised both
(2183 / 1983 calls on the navigated screen), ON installed (0xE9) + survived, screen reached =
Game Type Select with hooks live (verify/c4nav_gametypeselect.png).

CAVEAT (no overclaiming): the trio is sufficient C4 ONLY for a pure leaf whose C3 force-call
diff already covered the input domain. Non-leaf dispatchers (e.g. these menu drawers)
additionally need behavioral-diff coverage; promote through the re-classify skill (it gates
leaf vs non-leaf), NOT off this tool's "C4-READY" label.

## Mass navigate C4 testing (2026-06-07)

re/frida/c4_navigate_batch.py orchestrates canonical_c4_navigate.run() over many candidates:
- candidate sources: --rvas | --manifest <f> | --from-hookscsv <subsystem> [--status impl].
- ONE --nav path per invocation = one target screen; re-run with a different --nav for another.
- PHASE 1 (exercise, OFF): Interceptor-count candidates in CHUNKS (default 6/boot) to bound
  aggregate rate; counts gated to AFTER nav completes (COUNT_GATE) so "exercised" = "fires on
  the target screen", not boot/intro. PHASE 2 (ON): install ALL together (one boot); on crash,
  BISECT to isolate culprit RVA(s) so one bad hook doesn't fail the batch's survived flag.
- Per-candidate verdict: exercised + installed(0xE9) + survived + NOT hot. Hot fns (rate >
  ~800/s, e.g. ChromeBaseDraw 0x00472c60 ~3956/s) are flagged HOT and routed to the behavioral-
  survival lane (Interceptor on hot paths destabilizes — CLAUDE.md), NOT trusted off the count.
- Self-filtering: mass-run a whole subsystem at one screen; fns that don't fire there report
  HOLD (calls=0) — only genuinely on-screen fns pass. Demo (10 frontend C3 impl, --nav 4,4 ->
  Game Type Select): 6 trio-OK, 3 HOLD not-exercised (MenuGroupCount/IsMultiplayerMode/
  LangIndexSeedFromCli), 1 HOT (ChromeBaseDraw). Reports: log/c4_nav_batch_result.{json,md},
  ON screenshot verify/c4nav/c4nav_ON_all.png (Game Type Select, all installed, survived).
- REPORT ONLY — never auto-promotes; the re-classify skill gates leaf vs non-leaf for the
  actual C3->C4 promotion. Pair with scripts/patch_mashed_no_focus_pause.py for background runs.

## Option A full sweep result (2026-06-07) — 122 frontend C3 impl @ Game Type Select (nav 4,4)

Full results: c4_nav_A_gametypeselect.{json,md} (this dir). Per-group ON screenshots:
verify/c4nav_A/c4nav_ON_g{2..7}.png (survivor groups; g0/g1 crashed -> bisected).

- **14 trio-OK** (exercised-on-screen + installed 0xE9 + survived + not-hot): MenuChromeShellA
  0x0042e3a0, MenusBodyA 0x0042d5a0, TextGradientV0V1Override 0x00472f40,
  TextGradientV2V3Override 0x004730b0, MenuIm2DQuad 0x0042aae0, IntroSplashFrameTickShim
  0x00492d20, IntroVideoDimGetter 0x00493f80, AspectRatioGlobalGet 0x00493fc0, RwVtableSlot07Call
  0x004c19f0, IntroSplashVtableSlot6 0x004c1a00, IntroSplashRenderState 0x004c1bb0,
  MenuReadinessCheckA 0x0042ae10, AspectRatioSnapshot 0x00494f30, CarSlotStateSet 0x0040e480.
  (Many are per-frame fns that also fire pre-nav; the nav-specific ones are the Menu* drawers
  + MenuReadinessCheckA.) NOT auto-promoted — hand to re-classify, which leaf-gates: pure-leaf
  getters (AspectRatio*, IntroVideoDimGetter) auto-C4 off the trio; non-leaf draw dispatchers
  (MenuChromeShellA, MenusBodyA, ...) still need behavioral-diff coverage.
- **1 HOT**: ChromeBaseDraw 0x00472c60 (~3976/s) -> behavioral-survival lane, not Interceptor.
- **2 CRASHERS (real regressions the C3 force-call diff missed!)**: MenuDimSet 0x0042aad0
  (exercised 940 calls, but crashes when installed; eip=0x727d9e44) and MenuMenusBA 0x004282a0
  (crashes installed; eip=0x00427813). These crash on the canonical navigated scenario with the
  inline-JMP live -> they FAIL C4 and warrant a reimpl fix (and arguably a C3 review).
- **Install-failures (status=impl but no 0xE9 under MASHED_HOOK_ONLY)**: FUN_004c5c00 0x004c5c00
  (exercised 2010 calls), MenuMenusBC 0x0042f8d0 -> allowlist/registration discrepancy to chase.
- **~100 HOLD not-exercised**: do not fire on Game Type Select -> the Option B worklist (deeper
  screens: Single-Player setup, Options, Multiplayer, records/results, player/score getters).
  HOLD is "not on this screen", NOT a C4-negative; these rows are untouched (report-only).

Takeaway: Option A cleanly separated the menu-screen subset (14) + 2 genuine crashers from the
~100 deeper-screen hooks. The crashers are the highest-value find — the navigate C4 gate caught
installed-scenario regressions that force-call C3 could not.

## Post-Option-A actions (2026-06-07)

**Crashers fixed (root-caused via the navigate-C4 gate — both were crash_equal_ok C3s):**
- MenuDimSet 0x0042aad0: `mov [kDimEnableFlag],1` (C++ constexpr) assembled as a write to the
  symbol's read-only .rdata, not absolute 0x008990e4 -> faulting write in the .asi
  (eip 0x727d9e44). Fixed -> `mov dword ptr ds:[0x008990e4],1`. Now installed+survived+
  exercised(1300) on Game Type Select. (commit d48e99e1)
- MenuMenusBA 0x004282a0 + latent MenuMenusBB 0x00427ad0: FUN_004277a0 is a REGISTER-ARG fn
  (reads in_EAX = FUN_00427780's returned ptr, writes unaff_EBX = local buffer). Calling both
  as void __cdecl clobbered EAX between them -> AV 0x00427813. Reimplemented both helpers'
  logic inline (pure string-table lookup + control-code transcode). No more crash.
- Build footgun found+fixed: build.bat now deploys build/mashed_re_dev.asi -> original/ (the
  loader path); a stale .asi had been masking the fix. (see memory feedback-build-deploys-asi)

**Install-failures = disabled-by-design:** MenuMenusBC 0x0042f8d0 + LinkedListStringSearch
0x004c5c00 had RH_ScopedInstall commented out (MASS-DISABLED) -> no inline-JMP -> fail C3's
"hooked via RH_ScopedInstall" gate. DEMOTED C3->C2. (commit 77024999)

**Trio-OK -> re-classify (no overclaiming):** C4 gate (CONFIDENCE.md) = "clean Frida CSV diff
between original and modded on a canonical scenario." The navigate harness proves install+
survive+exercise (the C4 *prerequisite*) but NOT a behavioral diff -> NONE auto-promoted to C4.
The leaf getters among the trio are C4-candidates pending a canonical-scenario behavioral diff
(return-value/written-bytes original-vs-modded). That diff layer is the remaining work to earn
the C4s; survival evidence alone does not.

## Option B (deeper screens) result + ceiling (2026-06-07)

Validated deeper nav paths (input_resolver_drive / canonical_c4_navigate + PrintWindow shots):
- Options menu: `4,4,12,12,4` -> Sound/Gamma/Load Game/Save Game/Autosave (verify/optB_options.png)
- Single Player: `4,4,4` -> Challenge Cup/Quick Battle/Time Trial mode-select (verify/optB_singleplayer.png)

Option B sweep at Options (120 frontend C3 impl, nav 4,4,12,12,4): 13 trio-OK + 1 HOT, 0 crashes
(re/analysis/frontend_input_nav_trace/c4_nav_B_options.{json,md}). But only **1 net-new**
exercised hook vs Game Type Select: MenuReadinessCheckB 0x0042aeb0 (HOLD on GTS -> exercised on
Options). The other 13 are the same per-frame menu/chrome drawers + getters that fire on ANY
menu screen. CarSlotStateSet 0x0040e480 was GTS-only (not exercised on Options).

**Ceiling found:** the ~100 remaining HOLDs are NOT on the general menu screens. By name they are
gameplay/results-gated: PlayerScoreAcc*/PlayerScoreTeamAcc*/PlayerBlock2* (0x00423xxx in-race/
results score accumulators), LapLapsGetBySlot/LapSecsGetBySlot/TimeDiffDecompose/MenusLapTimeFmt
(records/results), RaceResultIndexedStore/EndOfRoundAccumulator/FrontendRaceResultsDispatch (race
results), VehicleSlotInit/CarSlotInit/VehicleSlotFieldSet (car-select/race setup). These only fire
during an actual race or on a results screen -> NOT reachable by menu navigation. Exercising them
needs an in-race/results canonical scenario (drive a full race), which is a separate, much larger
automation effort beyond the frontend menu navigate driver (the FUN_00497310 menu-input path does
not apply in-race). Further menu-setup sweeps have diminishing returns (each re-surfaces the shared
drawers + at most a couple new readiness/mode hooks).

Net menu-reachable navigate-C4 prerequisite coverage: ~15 distinct frontend hooks across Game Type
Select + Options (install+survive+exercise confirmed). C4 promotion of these still needs the
canonical-scenario behavioral-diff layer (above).

## Leaf C4s earned via canonical-scenario behavioral diff (2026-06-07)

re/frida/canonical_c4_leafdiff.py closes the C4 gate for pure leaves: captures the leaf's
OBSERVABLE BEHAVIOR in OFF (original, Interceptor on RVA) and ON (modded, Interceptor on the
.asi export the inline-JMP tail-jumps to) on the SAME navigated screen (Game Type Select),
and compares. For getters reading run-varying globals (pointer/handle, not a stable value) it
compares SAME-RUN (return/writes == source global) so cross-boot variance is not a false diff.

Promoted C3->C4 (clean diff + inline-JMP live + pure-leaf-no-stubs):
- MenuDimSet 0x0042aad0          : writes byte=0x30 + flag@0x008990e4=1 (identical both runs)
- IntroVideoDimGetter 0x00493f80 : writes dims 512.0f x4 (identical)
- AspectRatioGlobalGet 0x00493fc0: ret==[0x00771a18] both runs
- AspectRatioSnapshot 0x00494f30 : d1(0x771a50)==d2(0x771a54)==[0x771a18] both runs (callee
  AspectRatioGlobalGet is C4 -> no stub)
=> C4 115 -> 119. Evidence: c4_leafdiff_gts.json. Method generalizes to any pure-leaf getter/
writer reachable on a navigable screen.

HELD (not promoted, honest): MenuReadinessCheckA/B 0x0042ae10/0x0042aeb0 — return 0/1 but the
idle canonical scenario only exercises the trivial 0-branch (+ they call FUN_0040e470); the
draw dispatchers (MenuChromeShellA/MenusBodyA/MenuIm2DQuad) are non-leaves needing side-effect
diffs; ChromeBaseDraw is HOT. These need a richer diff (branch coverage / side-effect capture).

## Branch-coverage / side-effect diffs — readiness checks + draw dispatchers (2026-06-08)

Extended the diff harnesses to reach non-leaf / branchy frontend functions:
- canonical_c4_sidediff.py: SIDE-EFFECT diff. `buffer` kind (capture N output bytes) and
  `calltrace` kind (capture in-target ordered (callee,args) sequence; animated scalar args
  masked via same-run formula check, e.g. MenusBodyA arg == [0x0067ebc0]-15000 -> delta d=0).
- canonical_c4_leafdiff.py --readiness: capture EAX return DURING input taps (gate-at-settle)
  so BOTH branches (0 idle / 1 input-pending) are exercised.

Earned C3->C4 (clean canonical diff + inline-JMP live + no stubs):
- MenuIm2DQuad 0x0042aae0     : 112-byte vertex buffer @0x0067ec30 bit-identical.
- MenuChromeShellA 0x0042e3a0 : identical draw-call sequence (MenusBodyA d=0 + 4 const draws).
- MenuReadinessCheckA 0x0042ae10 : both branches {0,1} identical (input-driven, GTS).
=> C4 119 -> 122.
HELD honestly: MenuReadinessCheckB 0x0042aeb0 (only 0-branch reachable under our taps; its +1
byte offset checks control 0x7f1043 which down-taps don't set) ; ChromeBaseDraw 0x00472c60
(HOT, behavioral lane).

## Part 2 (in-race/results scenario) — scoped, blockers identified, NOT completed this session

Goal: exercise the ~100 gameplay/results-gated HOLD hooks (PlayerScoreAcc*, Lap*GetBySlot,
RaceResult*, VehicleSlotInit/CarSlotInit, EndOfRoundAccumulator...). Path mapped:
  menu -> [4,4] Game Type Select -> [4] Single Player -> [12,12,4] Time Trial ->
  **Player Colour Select** (verify/p2_timetrial_sel.png; shows controller "1" + icon row)
  -> [track select?] -> [car select?] -> race -> results.

BLOCKERS (why this is a much larger effort than Part 1):
1. **Fixed-dwell deep nav is unreliable** past ~3 screens: each screen's transition timing
   varies, so blind timed taps desync (the colour-sweep oscillated d2<->d3 and fell back to
   GTS, verify/p2_colour_sweep.png; blind-confirm chain landed on Options,
   verify/p2_blindconfirm.png). FIX NEEDED: state-aware nav — poll DAT_0067e9f8 (depth)/
   DAT_0067eca4 (phase) and issue each input only once the expected screen is reached. This is
   the concrete next deliverable and the prerequisite for everything below.
2. **Player Colour Select needs a player-JOIN input**, not the generic menu-confirm (control 4):
   blind confirms do not advance past it (the screen waits for player 1 to "join" with the
   highlighted colour; likely a fresh-press edge on a specific control/joystick button, and
   players default to joystick type — see [[feedback-no-global-input-injection]] binding dump).
   Screen text is data-driven (USA.DAT) so no string anchor in the exe; the join control must be
   found empirically (state-aware sweep on the parked colour screen) or by RE'ing the page state
   machine.
3. **Beyond colour**: track select + car select (each likely VehicleSlotInit/CarSlotInit fire
   here — a subset of the HOLDs reachable WITHOUT finishing a race), then race start.
4. **Race itself**: score/lap/results hooks fire only after a race progresses/completes.
   Automating an actual race (driving the car through a track, or an abort-to-results path) is
   the hardest piece and likely the bulk of the remaining work.

Honest status: Part 2 is a multi-component effort (state-aware nav -> join RE -> setup nav ->
race/abort -> results capture). Genuinely attempted and de-risked (path + precise blockers
mapped); not completable in one session. Recommended start: build the state-aware nav driver
(reusable; unblocks reliable deep navigation and clean join discovery), then iterate inward.

## Part 2 BREAKTHROUGH — in-race scenario reached via state-aware nav (2026-06-08)

re/frida/statenav.py solves blocker #1 (state-aware nav): it polls DAT_0067e9f8 (menu depth)
+ DAT_0067eca4 (phase) and issues each in-process input (FUN_00497310 override) only when the
expected screen is reached, waiting for the transition. This reliably drives the FULL chain:
  boot -> [confirm] GTS(d2) -> [confirm] dismiss modal -> [confirm] Single Player(d3) ->
  [down,down] Time Trial -> [confirm] Player Colour Select(d4) -> [confirm] Track/Challenge
  Select(d5, "Angel Peak/Kharga Temple/...") -> [confirm track] -> **IN RACE** (phase->0,
  verify/p2/sn_descend2.png shows the live track + car).

Blocker #2 RESOLVED: the colour-select "join" is just confirm (control 4) — the earlier
blind-nav failure was purely fixed-dwell desync, not a special control. State-aware timing
cracked it.

**In-race exercises gameplay-gated hooks** (the ~100 HOLDs): of 10 representative HOLDs counted
in-race, 6 fire: VehicleSlotInit 0x0046c5c0(3), RaceScoreFloatGetBySlot 0x00408ad0(16130),
LapLapsGetBySlot 0x00429a80(2240), RaceResultIndexedStore 0x0045ba00(6), LocalPlayerSlotCheck
0x00436810(3360), CarSlotStateGet 0x0040e470(58625). The 0-count ones (PlayerScoreAccA,
EndOfRoundAccumulator, TiebreakFlagGet, CarSlotInit) fire at RACE-END/results -> need the race
to FINISH (cross line / time out), the remaining piece.

Status: in-race canonical scenario WORKS and exercises gameplay hooks. Remaining for full Part 2:
(a) integrate statenav's descent into the OFF/ON exercise+diff harness to sweep all ~100 HOLDs
in-race (the in-race subset is now reachable); (b) drive the race to completion for the
results-screen subset. Both are now tractable (the hard "reach the race" gate is solved).

## Bottom line

The ASK ("which input source the menu reads") is fully answered: DirectInput8
`GetDeviceState(256)` → bitfield 0x77313c → remappable per-player resolver FUN_00497310 →
cooked flag block 0x7f1038+p*0x4c → frontend tick FUN_0043dfd0. AND the navigate scenario is
now SOLVED in-process: override FUN_00497310(player, control) returns — control 4=confirm,
11=up, 12=down — boots to title, confirms past the title + "Load Successful" modal, and
navigates the Game Type Select item list, all OS-input-free and visually verified
(verify/menu_nav_options.png shows Options highlighted via confirm,confirm,down,down).

Tools (all in-process, OS-input-free):
- re/frida/input_resolver_drive.py — the menu driver (--probe / --dumpmap / --sweep --list /
  --force) + PrintWindow screenshot. THE tool for navigate scenarios.
- re/frida/menu_nav_observe.py — lifecycle observer (phases, transitions, handler counts).
- re/frida/input_feed_cooked.py — cooked-flag/analog injector + selection probe + screenshot
  (superseded for nav by input_resolver_drive, kept for flag-level experiments).
- re/frida/input_consumer_watch.py — MemoryAccessMonitor (one-shot per page; limited use).
- scripts/patch_mashed_no_focus_pause.py — disable focus-loss pause for background testing.

