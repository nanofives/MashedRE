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

## Bottom line

The ASK ("which input source the menu reads") is fully answered: DirectInput8
`GetDeviceState(256)` → bitfield 0x77313c → remappable per-player resolver → cooked flag
block 0x7f1038+p*0x4c → frontend tick. The navigate *canonical scenario* remains deferred:
driving it in-process needs the directional consumer pinned (analog float fields / edge-pair
/ id-0x16 page handler), which is a further focused layer. Tools (all in-process, OS-input-
free): re/frida/input_feed_cooked.py (cooked-flag injector + selection probe),
re/frida/menu_nav_observe.py (lifecycle observer).

