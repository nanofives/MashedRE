# Kickoff for the next session (written 2026-08-30, second session of the day)

Follows `NEXT_SESSION_20260830.md`. That one offered three options; this session took **A (name
contcfg actions 6 and 7)**, finished it, then went on to ship **B (speed multiplier)** and
**C (rebind UI)** as well. All three are done.

Paste the block at the bottom into a fresh session. Everything above is context for a human.

## What this session was

Started as one question — name the last two unnamed frontend actions — and ended with all three
remaining cheap mods shipped. Two reusable techniques came out of it:

1. **Stop navigating toward a state, force it instead.** `entry.type` IS the screen id, so
   `FUN_0043d2a0(id, 0)` called from a hook on the menu tick renders any screen on demand.
2. **Never accept a feature on an observable your own code writes, or on one that is silently
   conditional.** Four wrong conclusions this session, all that shape. See Corrections.

## Landed, with evidence

| Result | Evidence |
|---|---|
| **Action 6 = PAUSE** | Behavioural A/B with a real negative control + capture. `verify/screen_id/race_fire6_settled.png` |
| **Action 7 = Change Stat** (on Race Results) | Four converging witnesses. Effect not observed firing — U-9059 |
| **Screen 5 = Race Results** | Forced push + image. `verify/screen_id/screen_5.png` |
| `0x0067ed3c` is a **menu stack** | Static, then confirmed live: `gate == TYPE(depth-1)` in every measured state |
| **Footer catalogue, 33 screens** | `verify/screen_id/footers_{0,1}.png`, all captures in `verify/screen_id/` |
| Screen-type map | type 0 = pause, 1 = Game Type Select, 5 = Race Results, 22 = title |
| Detectors scan **8** descriptor slots, not 4 | `0x0042b783..0x0042b834`, stride `0x4c` |
| New harness | `re/frida/force_screen.py` (force-push a screen, drive to a race, A/B with control) |
| **MASHED_SPEED shipped** | Game-speed multiplier, verified LINEAR 0.5x..3.0x by executed-tick count |
| **Rebind configurator shipped** | `re/tools/contcfg_ui.py`, passed `verify_rebind.py` 3/3 |
| New tool | `re/frida/speed_probe.py` — the acceptance harness `QOL_PATCH_PLAN` cited but never had |
| Race-clock artifact root-caused | `DAT_007f0ff4` freezes when `DAT_0063ba8c == 7` (`0x0040fe46`) |
| Trackers | U-9049 / U-9050 / U-9053 resolved, U-9054..U-9057 + U-9059 filed, 24 CHANGELOG entries |

**The one technique worth carrying forward:** `entry.type` *is* the screen id, so instead of
hunting for a screen you can call `FUN_0043d2a0(id, 0)` from inside a hook on the menu tick — on
the game's own thread at a frame boundary — and it renders. That produced the whole 33-screen
catalogue in six batched processes and named screen 5 in one shot. It is an **intervention**, so
it establishes how a screen renders, not that it is reachable in play.

## Corrections and retractions (read these before trusting anything above them)

1. **Retracted, my own claim from earlier the same session.** I filed a "three-way sibling
   finding" into U-9049 — that actions 4/5/6 each set a distinct adjacent outcome byte
   `0x0067eac4`/`5`/`6`. Wrong on both halves: `eac6` has **9 writers** (only one is action 6)
   and **zero readers**. The symmetry was an artifact of looking at write sites and not filtering
   by direction. Cost: one wrong tracker row for about an hour. Caught by the cheap follow-up.
2. **Corrected, also mine.** I cited `DAT_007f0f10 == 2` as proof a probe was "genuinely in a
   race". It reads `2` at the title screen too, and this repo *already* documents it as
   `g_itemSelectorP3` (`frontend_state.md:145`). Real session phase is `0x00771968`
   (`1=menu 2=load+spawn 3=race`). Check existing notes before adopting a global as a predicate.
3. The first race-lane attempt for action 6 was a **weak test that could not have produced a
   positive** (warped-in race, menu ticks not running, competing action-4 pulse). It is recorded
   as non-evidence, not as a null. The second attempt fixed all three defects and succeeded.
4. **Retracted, mine:** "MASHED_SPEED speed-up saturates near ~1.45x." False. Counting executed
   ticks shows it is linear 0.5x..3.0x within 0.3%. The race clock freezes in some states and the
   frozen fraction grows with the multiplier (15% at 1.5x, 38% at 2.0x), which manufactured a
   convincing saturation curve — I had even invented a plausible mechanism (hitch clamp) for it.
5. **Corrected, mine:** action 8 called UNNAMED in the first draft of this file. U-9052 had been
   resolved earlier the same day (play driver voice line). All 13 actions are named.
6. The animated menu background sprang the false-positive trap three more times. A raw
   before/after pixel diff on a frontend screen returned a bbox of the **whole frame** across a
   confirmed no-op. Use footer crops and state globals; never a pixel diff.

## THE FIVE MODS — what is done and what is missing

This is the list from `NEXT_SESSION_20260830.md`, re-assessed after this session.

### 1. Configurable game speed — **SHIPPED**

Engine work is done and measured: `MASHED_DECOUPLE` gives 0.998x real time at 165 fps, with
sub-unit remainder carry so truncation does not cost ~3.5%.

**DONE — `MASHED_SPEED=<mult>`** in `mashedmod/src/qol_asi/mashed_qol.cpp`. Scales the measured
delta fed to the quantizer `FUN_00493480`, Q16.16 fixed point, applied BEFORE the stock 4-tick
hitch clamp so the clamp still bounds the engine step; the existing remainder carry keeps
fractional precision. Unparseable or <=0 falls back to 1.0, never 0.

Verified by **counting executed game ticks** (a counter on the per-tick update `FUN_004111c0`):

| speed | real ticks/s | expected | error |
|---|---|---|---|
| 0.5 | 30.0 | 30 | -0.1% |
| 1.0 | 60.1 | 60 | +0.1% |
| 1.5 | 90.0 | 90 | -0.0% |
| 2.0 | 120.0 | 120 | -0.0% |
| 3.0 | 179.5 | 180 | -0.3% |

Linear, no ceiling, and `present_fps` holds 165.0 throughout so it costs no frames.

**TWO TRAPS, both live, both cost real time this session:**

1. **`mashed_re_dev.asi` silently disables `MASHED_SPEED`.** `FrameDispatch.cpp` installs
   `RH_ScopedInstall(FpsDiscretise, 0x00493480)`, a verbatim port that replaces the quantizer and
   calls `FUN_00493390` directly, bypassing the call site the QOL asi retargets. Ultimate-ASI-Loader
   loads EVERY `.asi` in `original/`, so with both deployed the dev hook wins, with no error.
   **Set `MASHED_RE_NO_AUTO_HOOK=1`** when using `MASHED_SPEED`.
2. **A 1.0x run is worthless as a control** — at 60 fps decoupled and stock are bit-identical BY
   DESIGN (snap band 47..53 -> 50), so it cannot detect that the hook is not running at all.

Acceptance harness: **`re/frida/speed_probe.py`** (new). `QOL_PATCH_PLAN_2026-08.md:90` cited a
"speed_probe" as the method for `MASHED_DECOUPLE` but nothing implemented it. It drives into a
race through the frontend, stops all input, and reports clock rate, real ticks/s, tick units, the
sub-frame accumulator and dt. It is **state-aware**: `DAT_007f0ff4` freezes while
`DAT_0063ba8c == 7` (`FUN_0040fc00` @ `0x0040fe46`; 10..11 gated the same way), so the probe sums
only counting intervals. After that the clock matches the tick counter to 0.1%.

### 2. Power-up frequency — **partial, and the ceiling is known**

Data-driven per track today via `Set_Current_Respawn_Time` in `POWERUPS*.LUA`, and `.piz` repack
works, so per-track tuning is shippable now.

**Missing:** a runtime dial. Needs the pod pool timer at `0x0068b198 +0x1c`. Unchanged this
session.

### 3. Key rebinding — **SHIPPED** (configurator built and verified 2026-08-30)

The data side was proven end-to-end last session (hand-written `contcfg2.bin` → loader → name
validation → binding table → descriptor → frontend transition, 3/3 including a negative
control). The stated blocker was *"actions 2..8 must be named before a UI can label them."*

**That blocker is now FULLY cleared — all 13 actions are named:**

| Action | Name | How established |
|---|---|---|
| 0 | Accelerate | descriptor `[4]`, prior behavioural |
| 1 | Brake | descriptor `[5]`, prior behavioural |
| 2 | Fire | static, `FUN_0045bba0` → type method `+0x08` |
| 3 | Discard held power-up | static, `FUN_0045bac0` → `+0x10` DEACT, clears `+0xa8`/`+0xac` |
| 4 | Select / Confirm | behavioural, two witnesses |
| 5 | Back | behavioural, two witnesses |
| **6** | **Pause** | **this session — A/B with negative control + capture** |
| **7** | **Change Stat** | **this session — image + code, effect not yet observed firing** |
| 8 | Play driver voice line | U-9052, resolved earlier the same day by the previous session |
| 9 / 10 | Steer left / right | X axis pair, three witnesses |
| 11 / 12 | Y axis up / down | digital-only through this path, flat `0xff` |

**DONE.** `re/tools/contcfg_ui.py` — Tk configurator over `contcfg_edit.py`: slot picker, 13 labelled actions, live key capture (VK -> DIK), stock-defaults restore, save, plus a headless `--detect` that reads the live table from a running game. A file written by the UI passed `verify_rebind.py` 3/3 including the negative control. Remaining polish only: no joypad *button* capture (numeric entry instead), and no in-game screen (see mod 3 notes below). **CORRECTION to an earlier draft of this file: it
said action 8 was unnamed. Wrong — U-9052 was resolved earlier the same day (action 8 = play
driver voice line), recorded in `contcfg_edit.py:46` and the CHANGELOG. All 13 are named.**
One caveat a UI must respect: **joypad axis remap is not expressible** in this
record — actions 9..12 ignore the binding value on the joypad path, so changing which stick
steers needs a code change at `0x0049733d`/`0x0049734e`. Keyboard axis rebinding *is* expressible.
Also: no `contcfg*.bin` exists in `original/` today, and slot assignment is not fixed (a Keychron
enumerates as two DirectInput devices, so the keyboard landed in slot **2** on this machine).

### 4. Resizable window — **unchanged, still blocked the same way**

Scaling works, resize does not. Backbuffer and the getters at `0x00498bc0`/`0x00498bd0` must move
atomically or you hit the null-raster AV at `0x004c7785`. Aspect at non-4:3 is unaudited.
Nothing this session touched it.

### 5. More than 4 players — **subsystem-scale, but this session found one real encouragement**

Still gated on fixed-size `.data` tables, unrolled player count, and U-1908 (per-player camera
raster rect).

**New and relevant:** the **input layer already has 8 player slots, not 4.** The frontend edge
detectors read eight descriptor slots at `0x007f1049 + k*0x4c` for `k=0..7`
(`0x0042b783..0x0042b834`), and the branch selected by `DAT_007f1a0c == 0x1000` scans all eight
directly. The 4-player limit in that layer comes from the **indirection table** at `DAT_007f1a14`
— 4 entries, stride `0x10`, bound `0x7f1a54` — which maps live players onto slots. So input is
not where the 4-player ceiling is built in. That does not make the feature cheap, but it removes
one suspected obstacle and tells you where to look next (`DAT_007f1a14`'s population, and
U-9055: what `0x1000` denotes).

### Summary

| Mod | State | Next concrete step |
|---|---|---|
| Game speed | **SHIPPED** | `MASHED_SPEED`, linear 0.5x..3.0x |
| Power-up frequency | Per-track yes, runtime no | Pod pool timer `0x0068b198 +0x1c` |
| Key rebinding | **SHIPPED** | `contcfg_ui.py`, verified 3/3 |
| Resizable window | Blocked | Atomic backbuffer + getter move |
| >4 players | Subsystem-scale | Input layer is NOT the blocker; look at `DAT_007f1a14` |

**UPDATE 2026-08-30: THREE of the five are now done or unblocked.** Rebinding and game speed are
both shipped and verified. What remains are the two genuinely large ones (resizable window,
>4 players) plus the power-up runtime dial.

## SECOND HALF -- 2026-08-31 (same continuous session)

The 08-30 half above named actions 6 and 7 and shipped rebinding + game speed. The 08-31 half
went after U-9059's residual and turned up something more useful than the residual itself.

### The headline: which actions a screen accepts is DATA

`[ESP+0x34]`, action 7's producer gate, is not computed from game state. `FUN_0043dfd0` zeroes
seven enable slots at `0x0043e3ff..0x0043e417`, then walks the CURRENT SCREEN'S `ff<op>0000`
opcode stream and sets one slot per bare opcode (`0x0043e420..0x0043e48f`):

| opcode | slot | meaning |
|---|---|---|
| `ff11` | `[ESP+0x20]` | |
| `ff0b` | `[ESP+0x24]` | |
| `ff0c` | `[ESP+0x28]` | |
| `ff0d` | `[ESP+0x2c]` | |
| `ff0e` | `[ESP+0x30]` | |
| `ff10` / `ff23` | `EDX` | |
| **`ff12`** | **`[ESP+0x1c]`** | **action 6 enable** (gate `0x0043fef9`) |
| **`ff33`** | **`[ESP+0x34]`** | **action 7 enable** (gate `0x004409c8`) |

The scan runs between an `ff09` marker and `ff0a`. **If `ff09` is absent where expected, the
`JNZ` at `0x0043e3ec` skips the ENTIRE action-dispatch block** to `0x004409f2` -- the likeliest
reason a forced push of screen 5 left the gate clear.

**New tool: `re/tools/screen_opcodes.py`.** Decodes all 34 records in `PTR_DAT_005f7638` and
prints each screen's message id and enable run; `--op ff33` asks which screens carry an opcode.
Use it to answer "which actions does screen N accept" from data instead of by navigating -- that
question cost several failed probe runs before the tool existed.

It also produced an **independent witness for both action names**, from a direction with nothing
to do with images or behaviour: `ff33` appears in exactly ONE screen's enable run (5, Race
Results -- the screen whose footer says `Change Stat`) and `ff12` in exactly ONE (0, the pause
menu -- and action 6 is Pause). Both names now rest on three independent witnesses.

### Structural finding: no frontend menu tick runs during a race

Measured over 4 s in phase 3: `FUN_0043dfd0` 0/s, `FUN_0043d7c0` 0/s, `FUN_0042ae10` 0/s,
`FUN_0042af50` 0/s, `FUN_0042b770` 0/s -- while the input cook `FUN_00496530` runs at 180/s.
**When PAUSED all of them run at 60/s**, except `FUN_0042b770` which stays 0/s (consistent with
its entry-type-5 gate). So action 6/7 probing can only ever work in a menu or paused state. The
in-race attempts that failed earlier were structurally doomed, not badly executed.

### The observable that unblocked menu probing

The per-entry **selection index** is at `0x0067ed80 + idx*0x40` (written by `FUN_0043d2a0` mode 0
as `(&DAT_0067ed80)[idx*0x10] = 0`). **`depth` and `gate` do NOT move when the cursor moves**,
which made a fully working input path look dead. With `sel`, action 12 walks the cursor reliably
and action 4 selects.

**Retracted:** an earlier claim in this session that the pause menu "reads input through a path
the `FUN_00497310` return-override does not reach". It reaches it fine; the observable was
missing.

### Pause menu, mapped end to end

Items `Continue / Options / Restart Race / Quit Race / Quit Game` at `sel` 0..4. Quit Race opens
a modal, "Are you sure you want to quit the race?", Yes = action 4 / No = action 5. **The modal
moves neither `depth` nor `gate` nor `sel`**, which is why select looked inert until it was
screenshotted. Confirming Yes lands on entry type **7 (Challenge Select)**, `sphase` 4 -> 1 -- so
Quit Race does NOT pass through Race Results.

### Open finding, not yet its own row: cars are FROZEN in a live race

All four car records (`0x008815a0`, stride `0xd04`, velocity `+0x9b0..b8`) read `vel=[0,0,0]` in
a live phase-3 race -- **including the AI car** -- while the engine demonstrably ticks at 60/s.
Records 0/1 have `grounded=4` (two cars visible on the track, no countdown overlay), 2/3 have
`grounded=0`. Held accelerate does nothing; three `action 4` confirms do not clear it.
**PRE-EXISTING:** `scenario_launch.py` prints `vel=[0, 0, 0]` in its own success verdict at the
same point, which is presumably why that harness pulses control 4 continuously. This blocks
EVERY "drive the race" probe, not just action 7's. Worth its own uncertainty row.

### Tracker outcome

- **U-9060** (was mis-numbered U-9059) **RESOLVED** -- the gate question is answered.
- **U-9061** opened, deliberately narrow: the one unobserved link, an action-7 edge setting
  `DAT_0067ec28` with the enable genuinely satisfied rather than poked. Written up as a
  COMPLETENESS item, not a correctness doubt.
- **ID COLLISION, multi-session:** `U-9059` was allocated twice -- by me and by the render-lane
  session, which inserts at the TOP of the Active section while this lane inserts at the bottom,
  so neither write saw the other. **Checking max-id before writing is not sufficient; re-check
  for duplicates AFTER.**
- Also flagged, pre-existing and untouched: ~130 rows in `UNCERTAINTIES.md` with a non-8 pipe
  count and 6 duplicated ids. A repair pass needs its own session.

### Method note worth carrying

Five wrong conclusions this session, every one from an observable that was **missing,
self-written, or silently conditional**: `kEffectsClock` (written by my own hook), a 1.0x speed
baseline (bit-identical to stock by design), the race clock (gated on `DAT_0063ba8c == 7`), the
menu cursor (no `sel` observable), and the "1.45x ceiling" (noise from the frozen clock, for
which I had invented a plausible mechanism). In four of the five the fix was to find an
observable the instrumentation does not touch. In the fifth -- today's -- the fix was to stop
driving the game and read the data table.

## Open, ranked by cost

1. **The frozen-car condition** — cars at `vel=[0,0,0]` in a live phase-3 race while ticks
   run at 60/s. Blocks every in-race probe. Not yet filed as a row; should be.
2. **`DAT_0063ba8c`** — the game-state enum that freezes the race clock at 7 (and 10..11).
   Known mechanically, not named. Cheap, and it firms up every clock-based measurement.
2. **U-9059** — confirm action 7 actually fires. Enter Race Results by finishing a race, fire
   action 7, watch `DAT_0067ea08`. Upgrades action 7 from image+code to observed.
3. **U-9053-adjacent, U-9054** — what panel state `2` causes; decompile one paired tick function
   (e.g. `FUN_0042f0c0`) and the whole 18-global family is answered.
5. **U-9057** — the external frontend message bank has no reader. Would resolve message ids to
   text and close several `[UNCERTAIN — not yet pinned]` screens in the frontend screen map.

## Cautions

- **Another session was active in this repo throughout**, working the render lane
  (`TrackRenderer.cpp`, `RwRaceSubmit.cpp`, `race_draw_burst.py`) and writing to the same
  trackers — it filed U-9058 and six CHANGELOG entries *between* two of my writes. Nothing
  collided because every CHANGELOG insert matched the **CRLF-exact** marker line and asserted
  uniqueness. `<!-- ENTRIES -->` appears **three** times in that file, twice in prose; a naive
  substring insert writes into the header. Never rewrite that file wholesale.
- Nothing from this session is committed. `UNCERTAINTIES.md` and `re/analysis/CHANGELOG.md` are
  modified; `re/analysis/structs/contcfg_record.md`, `re/frida/force_screen.py` and
  `verify/screen_id/` are new and untracked.
- PID hygiene held: ~12 MASHED spawned, every one killed by explicit pid, never by name. The one
  probe that attached to another tool's process left it alive for its owner.
- `capture_window.ps1` still returns all-white. Use `MASHED_ORIG_BBDUMP_REQ`.

---

## Paste this

```
Continue the Mashed work. Read re/analysis/NEXT_SESSION_20260831.md first --
BOTH halves, the 08-30 one and the "SECOND HALF -- 2026-08-31" section. The
five-mod status table is the map; re/analysis/structs/contcfg_record.md has the
input-lane detail.

DONE and verified: all 13 contcfg actions named; key rebinding (re/tools/
contcfg_ui.py, passed verify_rebind.py 3/3); game speed (MASHED_SPEED, linear
0.5x..3.0x by executed-tick count). U-9049/U-9050/U-9053/U-9060 resolved.

Pick ONE:
  A) Power-up runtime dial -- per-track already works via POWERUPS*.LUA; the
     runtime knob needs the pod pool timer at 0x0068b198 +0x1c.
  B) Resizable window -- backbuffer and the getters at 0x00498bc0/0x00498bd0
     must move ATOMICALLY or you hit the null-raster AV at 0x004c7785.
  C) The frozen-car condition: all four car records read vel=[0,0,0] in a live
     phase-3 race (incl. the AI car) while ticks run at 60/s. PRE-EXISTING --
     scenario_launch.py shows it too. Unblocks every in-race probe if solved.
  D) Small: close U-9061 (find which guard bails on a forced push of screen 5 --
     the ff09 test at 0x0043e3ec, or iVar8 != 0 && DAT_0067eab0 == 0 -- then
     fire action 7; needs no race), or name DAT_0063ba8c.

HARD-WON GOTCHAS, do not rediscover these:
- Which actions a screen accepts is DATA: ff12 enables action 6, ff33 enables
  action 7, in the screen record. Use re/tools/screen_opcodes.py instead of
  navigating to find out.
- NO frontend menu tick runs during a race (all 0/s); they run at 60/s only when
  PAUSED. In-race action probing cannot work.
- The menu CURSOR is at 0x0067ed80 + idx*0x40. depth/gate do NOT move when it
  does. Modals move none of the three -- screenshot them.
- mashed_re_dev.asi SILENTLY disables MASHED_SPEED (RH_ScopedInstall of
  FpsDiscretise at 0x00493480). Use MASHED_RE_NO_AUTO_HOOK=1.
- A 1.0x speed run is NOT a control: at 60fps decoupled == stock bit-identically.
- The race clock DAT_007f0ff4 FREEZES when DAT_0063ba8c == 7. Gate on it before
  fitting any rate; speed_probe.py does. Above 1.0x prefer counting real ticks.
- Player 0 sits on a JOYPAD slot on this machine (Keychron enumerates as two
  GAMECTRL devices; keyboard is slot 2). Keyboard-bitmap injection drives a
  DIFFERENT slot -- use the FUN_00497310 return-override for slot-0 probing.
- Never pixel-diff a frontend screen; the background is animated.
- Ghidra MCP not connected; analyzeHeadless works via
  re/tools/ghidra_scripts/{DecompBatch,XrefRange}.java (RVAs in a FILE, cmd.exe
  eats commas). Use MASHED_ORIG_BBDUMP_REQ for screenshots.
- Another session shares this working tree AND the trackers, and inserts at the
  TOP of UNCERTAINTIES.md while this lane inserts at the bottom. U-9059 got
  allocated twice. Re-check for duplicate ids AFTER writing, and insert CHANGELOG
  entries with a CRLF-EXACT marker match.
```


