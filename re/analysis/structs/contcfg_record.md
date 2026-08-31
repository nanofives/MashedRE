# `contcfg%d.bin` / controller-config record — field map (2026-08-29)

Source: Ghidra headless (`analyzeHeadless` + `re/tools/ghidra_scripts/DecompBatch.java`) on
slot `Mashed_pool0`, program `MASHED.exe`. Every offset below is cited to an instruction
address in the listing. NO-GUESSING: fields with no observed writer are marked `[UNCERTAIN]`.

## Summary

| | |
|---|---|
| On-disk file | `contcfg%d.bin` (format string `0x005d000c`, formatted into buffer `DAT_007730e4` by `FUN_00497190`) |
| Record size | `0x200` bytes (`PUSH 0x200` at `0x004971dc`, `_fread` count) |
| Slot count | **4** (loop bound `puVar8 < 0x7e9ec8` at `0x00498510`) |
| In-memory table | `DAT_007e95c0`, stride `0x200`, spans `0x007e95c0..0x007e9dc0` |
| Slot index | passed in **EAX** (`SHL ESI,0x9` = ×`0x200` at `0x004971e1`) |
| Loader | `FUN_004971b0` |
| Saver | `FUN_00497230` (`_fwrite` `0x200`, same base) |
| Defaults builder | `FUN_00498510` (zero-fills `0x800` = 4×`0x200` via `FUN_004b6520(&DAT_007e95c0, 0x800)`) |
| Lookup | `FUN_00497310(slot, action)` |

The file is a **raw memcpy of the in-memory record**. There is no serialization step, no
header, no version field, no checksum. Read and write are symmetric `fread`/`fwrite` of
`0x200` bytes at `DAT_007e95c0 + slot*0x200`.

## Record layout (offsets relative to record base)

| Offset | Size | Field | Evidence |
|---|---|---|---|
| `+0x000` | 4 | constant **`6`** | `puVar8[-0x42] = 6` in `FUN_00498510`; `puVar8` = base+`0x108`, so `-0x42` dwords = `-0x108` bytes. Written unconditionally for all 4 slots, including inactive ones. Meaning [UNCERTAIN] — no reader found in the 7 functions decompiled. |
| `+0x004` | `0x104` | device **name** string | joypad branch: `FUN_00495830(iVar11, puVar8 + -0x41)` = base+`0x004`. keyboard branch: byte-copy loop from `DAT_007730b4` to base+`0x004`. Size `0x104` = 260 = `MAX_PATH`, consistent with a DirectInput product-name buffer. |
| `+0x108` | 4×13 | **binding array**, actions `0..12` | `MOV ECX, dword ptr [EAX + EDX*0x4 + 0x108]` at `0x00497330` (joypad path) and `MOV EAX, dword ptr [EAX + ECX*0x4 + 0x108]` at `0x00497406` (keyboard path). |
| `+0x13c` | 4 | **device type** | `MOV ECX, dword ptr [EAX + 0x13c]` at `0x0049731d`. `0` = inactive, `1` = joypad, `2` = keyboard (`CMP ECX,0x1` `0x00497323`, `CMP ECX,0x2` `0x004973fd`; assignments `puVar8[0xd] = 1/2/0` in `FUN_00498510`). |
| `+0x140` | 4 | **joypad index** | `MOV EAX, dword ptr [EAX + 0x140]` at `0x00497337`; assigned `puVar8[0xe] = iVar11` (loop counter) in `FUN_00498510`. |
| `+0x144` | `0xbc` | **unexamined tail** | `[UNCERTAIN]` — no writer in `FUN_00498510`, `FUN_004971b0`, `FUN_00497230`, `FUN_00497310`, `FUN_00496530`, `FUN_00497190`, `FUN_004982a0`. Zeroed by the `0x800` memset. Persisted to disk regardless. |

**CORRECTION to prior notes:** the binding entries are **DWORDs**, not bytes. Both access
sites scale the action index by 4 (`EDX*0x4`, `ECX*0x4`). Earlier project notes describing a
"binding byte table" understated the stride by 4×. The stored *value* is a small integer
(DIK scancode or button index), but the *slot* is 32 bits.

## Action index → meaning

Actions `0..8` are digital bindings; `9..12` are the two analog axis pairs.

- **Joypad (type 1), actions 0..8** — bit test at `0x004973df`:
  `byte [ (binding>>3) + joypadIndex*4 + 0x7730d4 ] & (1 << (binding & 7))`.
  So the binding value is a **button bit index** into a 4-byte-per-joypad button bitmap at
  `DAT_007730d4`.
- **Keyboard (type 2), all actions** — bit test at `0x0049741c`:
  `byte [ (binding>>3) + 0x77313c ] & (1 << (binding & 7))`.
  So the binding value is a **raw DIK scancode** into the 32-byte keyboard bitmap at
  `DAT_0077313c`.
- **Joypad, actions 9..12** — the binding value is *ignored*. The axis floats are read from a
  per-joypad pair table: X at `FLD [EAX*0x8 + 0x77311c]` (`0x0049733d`), Y at
  `FLD [EAX*0x8 + 0x773120]` (`0x0049734e`), `EAX` = joypad index, 8-byte stride.

  | Action | Condition | Scale | Address |
  |---|---|---|---|
  | 9 | `x < 0.0` | `× -255.0` (`0x005d0108`) then tail-jump `FUN_004a2c48` | `0x00497377`, `0x0049737b`, `0x00497384` |
  | 10 | `x > 0.0` | `× +255.0` (`0x005cd04c`) then tail-jump `FUN_004a2c48` | `0x0049739e`, `0x004973a2`, `0x004973ab` |
  | 11 | `y > 0.0` | returns `0xff` (no scaling) | `0x004973c1` |
  | 12 | `y < 0.0` | returns `0xff` (no scaling) | `0x004973d6` |

  Comparison threshold is `DAT_005d757c` = **`0.0f`** (bytes `00000000` at file `0x1d757c`).
  The real deadzone (`0.35f`, `_DAT_00772fa8`) is applied upstream in `FUN_00495870`, not here.

  **Finding:** actions 9/10 return a magnitude quantized to **0..255**, and `FUN_00496530`
  reconstructs a float by multiplying by `1/255.0f` (`_DAT_005ceb90` = `0x3b808081` =
  `0.003921568859`). The X axis therefore passes through an **8-bit quantization** in the
  binding layer. Actions 11/12 return a flat `0xff`, i.e. the Y axis is **digital-only**
  through this path — it has no proportional return.
  `[UNCERTAIN]` `FUN_004a2c48` (float→int conversion) not decompiled this pass; U-3024/S-2285 remain open.

## Action index → per-player descriptor byte

`FUN_00496530` issues exactly 13 `FUN_00497310` calls and scatters each result into the
per-player input descriptor at `DAT_007f1038 + player*0x4c`. Pairing read from the listing
(the `MOV ...,AL` store follows the *next* `PUSH`, so each store belongs to the *preceding*
call):

| Action | Store address (player 0) | Descriptor offset | Call site |
|---|---|---|---|
| 4 | `0x007f1042` | `+0x0a` | `0x004965af` |
| 5 | `0x007f1043` | `+0x0b` | `0x004965bd` |
| 6 | `0x007f1041` | `+0x09` | `0x004965cb` |
| 7 | `0x007f1049` | `+0x11` | `0x004965d9` |
| 0 | `0x007f103c` | `+0x04` | `0x004965e7` |
| 1 | `0x007f103d` | `+0x05` | `0x004965f5` |
| 2 | `0x007f103f` | `+0x07` | `0x00496603` |
| 3 | `0x007f1040` | `+0x08` | `0x00496611` |
| 8 | `0x007f1077` | `+0x3f` | `0x00496622` |
| 9 | `0x007f1038` `+` flag `0x007f1044 = 0xff` | `+0x00`, `+0x0c` | `0x0049663c` |
| 10 | `0x007f1039` `+` flag `0x007f1045 = 0xff` | `+0x01`, `+0x0d` | `0x00496658` |
| 11 | `0x007f103a` `+` flag `0x007f1046 = 0xff` | `+0x02`, `+0x0e` | `0x00496674` |
| 12 | `0x007f103b` `+` flag `0x007f1047 = 0xff` | `+0x03`, `+0x0f` | `0x00496690` |

Plus one non-call copy: `0x007f104b` (`+0x13`) `=` `0x007f103f` (`+0x07`), i.e. a duplicate of
action 2's result (`MOV AL,[0x7f103f]` at `0x0049662d`, `MOV [0x7f104b],AL` at `0x00496636`).

Actions 9..12 store **conditionally** (`TEST AL,AL` / `JZ` at `0x00496644`, `0x00496660`,
`0x0049667c`, `0x00496698`) and set a companion `0xff` flag byte only when nonzero.

### Cross-check against existing behavioural evidence

`UNCERTAINTIES.md` U-0407 / U-0413 / U-9043 (RESOLVED 2026-08-24, capture
`verify/a8_steer_20260824`) established descriptor bytes `[0]/[1]` = steer and `[4]/[5]` =
accel/brake. Combining with the table above:

- `[0]/[1]` = actions **9/10** = the X axis pair. **Steering is action 9 (X−) and action 10 (X+).**
- `[4]/[5]` = actions **0/1**. **Accelerate is action 0, brake is action 1.**
- Corroboration: the invert-steering swap at `0x00496717` (gated on `DAT_007f0f30`) exchanges
  descriptor `[0]`↔`[1]`, which is exactly the action 9/10 pair. Three independent witnesses agree.

Actions 2, 3, 4, 5, 6, 7, 8 have **no behavioural binding yet**. See the consumer analysis
below, which narrows them but does not name them.

## Consumer analysis for actions 2..8 (static, 2026-08-29)

Method: `re/tools/ghidra_scripts/XrefRange.java` over `0x007f1038..0x007f1084` (player-0
descriptor) — 204 refs — filtered to the seven unnamed bytes, excluding the writer
`FUN_00496530`. Direct callers then resolved by scanning `.text` for `E8 rel32` encodings
targeting each consumer (exact, no heuristics).

| Action | Descriptor byte | Consumer | Consumer's callers | Lane |
|---|---|---|---|---|
| 2 | `+0x07` | `FUN_0045bba0` @ `0x0045bd72` | — | **gameplay** (powerup dispatcher) |
| 3 | `+0x08` | `FUN_0045bba0` @ `0x0045be31` | — | **gameplay** (powerup dispatcher) |
| 4 | `+0x0a` | `FUN_0042ae10` | `0x0043da02`, `0x0043da0d`, `0x0043da3f`, `0x0043da9b` (`FUN_0043d7c0`); `0x0043e4b0` (`FUN_0043dfd0`) | **frontend** |
| 5 | `+0x0b` | `FUN_0042aeb0` | `0x0043da19`, `0x0043da5e`, `0x0043dab3` (`FUN_0043d7c0`); `0x0043f6de` (`FUN_0043dfd0`) | **frontend** |
| 6 | `+0x09` | `FUN_0042af50` | `0x0043da7c` (`FUN_0043d7c0`); `0x0043feff` (`FUN_0043dfd0`) | **frontend** |
| 7 | `+0x11` | `FUN_0042b770` | `0x004409ce` (`FUN_0043dfd0`) | **frontend** |
| 8 | `+0x3f` | `FUN_0045d0e0` | `0x0045d1e6` (`FUN_0045d1e0`) | gameplay-side `[UNCERTAIN]` |
| dup | `+0x13` | `FUN_004197e0` | `0x0041a197` (`FUN_0041a180`) | `[UNCERTAIN]` |

`FUN_0042ae10` / `FUN_0042aeb0` / `FUN_0042af50` / `FUN_0042b770` are one family: each is an
**any-player rising-edge detector** for a single descriptor byte. Shape (`FUN_0042b770`,
`0x0042b783` onward): `descriptor[off] != 0 && snapshot[off] == 0`, where the snapshot is the
copy made at the top of `FUN_00496530` into `DAT_007f14f8` (`+0x4c0` from the live
descriptor). Returns 1 if any player produced an edge.

**CORRECTION (2026-08-30): the detectors scan 8 descriptor slots, not 4.** Earlier text here
said "each of the 4 players". The unrolled branch in `FUN_0042b770` reads eight bytes at
`0x007f1049`, `0x007f1095`, `0x007f10e1`, `0x007f112d`, `0x007f1179`, `0x007f11c5`,
`0x007f1211`, `0x007f125d` (`0x0042b783`..`0x0042b834`), stride `0x4c`, i.e. **8 descriptor
slots**. Both detector branches are selected by `DAT_007f1a0c == 0x1000` (`0x0042b778`):

- `== 0x1000` — scan all 8 slots directly, unconditionally.
- otherwise — walk a 4-entry **indirection table** at `DAT_007f1a14`, stride `0x10`
  (`0x0042b841`, bound `CMP ESI,0x7f1a54` at `0x0042b87b`). Each entry holds a slot index at
  `+0x00`, `-1` meaning empty, and the slot is skipped when `FUN_0040e470(i) == 2`
  (`0x0042b84c`..`0x0042b857`).

So the descriptor array has **8 physical slots**, of which at most 4 are mapped to live
players through `DAT_007f1a14`. `[UNCERTAIN]` what `0x1000` denotes in `DAT_007f1a0c`.

**Result: actions 4, 5, 6, 7 are frontend/menu actions.** Their only consumers are the two
menu-phase tick functions `FUN_0043d7c0` and `FUN_0043dfd0` (the same pair documented in
`re/analysis/QOL_PATCH_PLAN_2026-08.md` as ticking per-frame in menu phases 1/4/7). No
gameplay code reads them.

**Actions 2 and 3 are gameplay powerup actions** — their sole consumer is `FUN_0045bba0`, the
powerup dispatcher documented in `re/analysis/structs/powerup_system.md`.

This narrows each action to a lane but does **not** name it. Distinguishing 4 from 5 from 6
from 7, and 2 from 3, requires behavioural A/B (below). `[UNCERTAIN]` until then.

Note the convergence with `remap.lua`'s `RB_*` names (four `RB_PRESENT_*` frontend actions,
`RB_GAME_FIRE` + `RB_POWERUP_TOGGLE` gameplay) is **suggestive only and is not evidence** —
the PC exe never loads that file (`re/analysis/lua_remap_correction_20260829.md`). Do not use
it to shortcut the A/B.

## A/B plan to name actions 2..8

Search space is now small enough to be one session. Per action, the experiment is:

1. Write a `contcfg0.bin` binding action N to a scancode nothing else uses, leaving all other
   actions bound to `0x00`. Record `0x200` bytes per the layout above; type field `+0x13c = 2`.
   Note `patch_mashed_skip_controller_dialog` must be restored first, or the file is never read
   (`FUN_004971b0` currently returns 0 and defaults win).
2. Drive the key and observe the lane's own observable, not a screenshot:
   - actions 4..7: the frontend state machine — capture the menu screen id transition. The
     `nav_coverage.py` / draw-list harness already reads that.
   - actions 2, 3: `FUN_0045bba0` state — the per-player held-powerup slot at `0x0088fbe0`
     (stride `0xb4`, `+0xa8` active type ptr, `+0xac` armed handle).
3. Negative control: same run with the action bound to `0x00`, confirming the observable does
   not move.

Constraint: read state via memory reads, no `Interceptor` on `FUN_0045bba0` — it runs at
60/s and the project's hot-path rule applies (`CLAUDE.md`, "Frida overhead on hot paths").

## A/B run 1 — frontend actions 4..7 (2026-08-29, PARTIAL)

Harness: `re/frida/probe_action_semantics.py`. Mechanism used is **not** the binding-table
write originally planned: it overrides the *return value* of `FUN_00497310` for exactly one
call of one `(player, action)` pair (same technique as `canonical_c4_navigate.py`). Nothing is
written to disk, to `DAT_007e95c0`, or to the keyboard bitmap. The `Interceptor` is armed only
after settle, to keep the hot-path window short.

**Method correction made mid-run:** the first pass probed all four actions in one process and
produced a false negative for actions 5, 6, 7 — action 4 had already moved the frontend off
the baseline screen, so the later probes started from an altered state. Re-run with **one
fresh process per action**, all from the same baseline. Only the re-run is reported.

Baseline in all four runs: `CURSCREEN` (`0x0067ecb0`) = **33**, `PHASE` (`0x0067eca4`) = 3,
stable. Negative control (identical dwell, no press) never moved in any run.

| Action | Result | `override_landed` |
|---|---|---|
| 4 | screen **33 → 0** | yes |
| 5 | no movement | yes |
| 6 | screen **33 → 0** | yes |
| 7 | no movement | yes |

`override_landed` is read from the agent's own fire counter, so a null result is a real null,
not a press that never happened.

**What this establishes:** actions 4 and 6 are both accepted by the frontend on screen 33 and
produce the same transition. Actions 5 and 7 are inert there. Combined with the static result
(all four are frontend-only), 4 and 6 are confirmed live frontend inputs.

**What this does NOT establish:** it does not separate 4 from 6, and it does not name any of
them. Screen 33 accepts both, so it is not a discriminating screen. `[UNCERTAIN]` remains for
all of 2..8.

**Blocked on:** screen 0 is a stable absorbing state that does not respond to action 5
(verified, 5 s trace). Progress needs a screen where confirm / back / pause diverge, which
needs the frontend screen-id map — `re/analysis/frontend_config_screens_REmap_20260614.md`
maps 17 screens and was not consulted this pass.

Data: `log/action_sem_a{4,5,6,7}.json`, `log/action_sem_recon.json`.

### Capture channel — root cause and fix

Run 1 used `scripts/capture_window.ps1` (PrintWindow + `PW_RENDERFULLCONTENT`) and got an
all-white client area. **This was not a regression and not caused by the force-kills** — it is
the pre-existing condition documented in `mashedmod/src/d3d9_shim/d3d9_shim.cpp:213`:
*"window screenshots are untrustworthy on this machine (multi-monitor Present issue)"*. That
comment is exactly why the backbuffer dump exists.

Checked and ruled out first: `DWM8And16BitMitigation` **is** present on
`original\MASHED.exe` in both `HKCU` and `HKLM` Layers (the `HKLM` one with the system-added
`$` prefix), which `CLAUDE.md` names as a d3d9-proxy breaker. The PCA Store is empty, so PCA
is not currently re-adding it. Since the backbuffer channel works fine with the layer in
place, **the layer was not the cause of the white capture** and nothing was changed in the
registry.

Fix: `probe_action_semantics.py --shots` now uses the shim's on-demand protocol
(`MASHED_ORIG_BBDUMP_REQ`) — write the target `.bmp` path into the request file, the shim
dumps the backbuffer at the next `Present` and deletes the request. Bypasses DWM entirely.
Known wart: a dump requested immediately before `dev.kill` lands truncated.

## A/B run 2 — frontend actions 4..7 with the visual channel (2026-08-29)

The visual channel immediately explained both null results from run 1:

- **Screen 33 = the title screen**, "Press button to start". It accepts several actions, which
  is why 4 and 6 produced the same transition. Not a discriminating screen.
- **Screen 0 = "Game Type Select", but initially covered by a modal** ("Load Successful. /
  Continue"). Run 1's probes were firing at a modal, not at a menu.
- `CURSCREEN` stays **0** for the modal, the root menu, *and* the Single Player submenu, so it
  is not a usable observable below the title screen. The image is.

Method: `--prefix 4,4` to dismiss the modal and land on the root menu, `--prefix 4,4,4` for
the submenu. One fresh process per action. Diffs are read from the image, not from pixel
statistics — the menu background is an animated track flyby, so a raw before/after pixel diff
reports "changed" for every action including no-ops and is worthless here.

| Action | At root menu | In submenu | Verdict |
|---|---|---|---|
| 4 | enters `Single Player` → Challenge Cup / Quick Battle / Time Trial; footer gains a **Back** prompt | — | **Select / Confirm — CONFIRMED** |
| 5 | no change; footer shows only `Select` (no Back available at root) | **returns to Game Type Select**; footer loses the Back prompt | **Back / Cancel — CONFIRMED** |
| 6 | no change (12 s dwell) | no change | `[UNCERTAIN U-9049]` — null EXPLAINED 2026-08-30, see below |
| 7 | no change (12 s dwell) | no change | `[UNCERTAIN U-9050]` — null EXPLAINED 2026-08-30, see below |

**2026-08-30 update:** both nulls are now accounted for statically and are *not* evidence of
inert actions. Action 6 is suppressed at these screens by two gates; action 7's gate needs a
stack entry type that no literal push produces. See "Actions 6 and 7 — effects pinned
STATICALLY" below. Do not re-run blind navigation for either.

Title screen (33), from run 1: actions **4 and 6 both advance** 33 → 0; actions 5 and 7 do not.

**Action 4 = Select** and **action 5 = Back** each rest on two independent observations: the
screen transition itself, and the footer prompt set changing to match. Action 5's null at the
root is corroborating, not contradicting — the footer confirms no Back target exists there.

**Action 6 is NOT named — U-9049.** It is accepted *only* on the title screen (33 → 0, same as
action 4) and is inert on both menus. **Retraction:** an earlier reading in this same session
held that action 6 removed the menu list at the root. That was wrong — the 3.5 s shot caught
the attract-mode credits sequence fading the background, and a 12 s re-run shows the menu
intact. The same rolling credits appear in the action-7 shot naming a different crew member,
which is what gives it away. Do not label action 6 "back" or "exit".

**Action 7 is NOT named — U-9050.** No effect on any of the three screens, both menus re-run at
12 s. Not dead code: `FUN_0042b770` is C4 with exactly one caller (`0x004409ce` inside the live
menu tick `FUN_0043dfd0`), so it is reachable and simply was not exercised here. Cheapest next
step is to decompile around `0x004409ce` and read which screen gates the call, rather than more
blind navigation.

**Defaults cannot discriminate these.** Actions 5 and 6 share `DIK_ESCAPE`; actions 2 and 7
share `DIK_A`. Two logically distinct actions sharing a default key is normal here.

**Method warning.** The menu background is an animated track flyby with a rolling credits
overlay. It produced a false positive twice in one session. Any before/after visual claim on
these screens needs a dwell long enough to outlast the background cycle, and a raw pixel diff
is not a substitute — it reports "changed" for every action including confirmed no-ops.

## Actions 2, 3 and 8 — resolved STATICALLY, no race needed (2026-08-29)

A race run was planned for these. It turned out to be unnecessary for 2 and 3, and the static
read is stronger than a behavioural null would have been — worth noting because the project's
own `QOL_PATCH_PLAN_2026-08.md` records that `scenario_launch` **never has an active pod or
powerup** (measured repeatedly), so a race probe for 2/3 would most likely have produced
uninformative nulls for want of a held powerup.

**Action 2 = FIRE.** `FUN_0045bba0` at `0x0045bd72` reads the live byte `0x007f103f` (CL) and
the snapshot `0x007f14ff` (DL, = `0x007f14f8 + 0x07`) and builds a three-state phase in
`[ESP+0x14]`:

| Condition | Phase |
|---|---|
| live != 0 | 3 (hold) |
| snapshot != 0 && live == 0 | 1 (release) |
| live != 0 && snapshot == 0 | 2 (press) |

Then at `0x0045bdb2..0x0045bdbb`: `EDX = phase; ECX = [EDI+0x18]; PUSH phase; PUSH slot;
CALL [ECX+0x8]`. Per `re/analysis/structs/powerup_system.md:110` the type-record method at
**`+0x08` is FIRE**. So action 2 drives the powerup FIRE entry point with press/hold/release
semantics — which is what a flamethrower or machinegun needs, and matches the pinned per-type
constants (GUN fire rate `0.06 s` at `0x0045628f`).

**Action 3 = DEACTIVATE / discard the held powerup.** `FUN_0045bba0` at `0x0045be31` reads
live `0x007f1040` and snapshot `0x007f1500` (= `+0x08`), fires only on a rising edge, and
calls `FUN_0045bac0`. That function (28 bytes, `0x0045bac0..0x0045badb`) is unambiguous:

```
MOV EAX,[ESI + 0xa8]      ; active type record
CALL dword ptr [EAX + 0x10]   ; type method +0x10 = DEACT
MOV [ESI + 0xa8], 0       ; clear active type ptr
MOV [ESI + 0xac], 0       ; clear armed handle
```

`+0x10 = DEACT` per `powerup_system.md:110`, and `+0xa8` / `+0xac` are the active-type pointer
and armed handle in the per-player slot (`0x0088fbe0`, stride `0xb4`). Clearing them is not an
interpretation — it is what the instructions do.

**Action 8 is NOT named — U-9052.** Consumer `FUN_0045d0e0(player)` is a per-frame predicate:
it returns 0 when the live byte `0x007f1077` (`+0x3f`) is zero and 1 on a rising edge against
the snapshot `0x007f1537` (= `0x007f14f8 + 0x3f`), gated behind `FUN_00431b70() == 2` and
`FUN_0040e470(player) < 2`. Its caller `FUN_0045d1e0` then, if two further conditions hold
(`DAT_0088f0c0[player] == 1` and `DAT_008aa2e0[player] == 0`), reads a **position triple** from
`*(DAT_0088f6a0 + player*4)` and runs `FUN_0045c880(x,y,z)`, `FUN_0045cd30(player)`,
`FUN_004656e0(player)`, `FUN_0045cbe0(x,y,z)`. The shape is a per-player state change keyed on
a stored position, but none of those four callees is named, so the action is not named either.

Images: `verify/action_sem/` (`crop_a{4,5,6,7}.png` root, `sub_a{5,6,7}.png` submenu).
Data: `log/action_sem_menu_a*.json`, `log/action_sem_sub_a*.json`.

## Default bindings (`FUN_00498510`)

Slot assignment: `iVar15 = FUN_00495790()` (joypad count). Slots `[0 .. count-1]` get type 1
(joypad, index = slot); slot `[count]` gets type 2 (keyboard); the rest get type 0.
So the keyboard player is always the slot immediately after the last joypad.

| Action | Joypad default (button index) | Keyboard default (DIK) | DIK name |
|---|---|---|---|
| 0 | 0 | `0x1f` | `DIK_S` |
| 1 | 1 | `0x2d` | `DIK_X` |
| 2 | 2 | `0x1e` | `DIK_A` |
| 3 | 3 | `0x20` | `DIK_D` |
| 4 | 0 | `0x1c` | `DIK_RETURN` |
| 5 | 1 | `0x01` | `DIK_ESCAPE` |
| 6 | 4 | `0x01` | `DIK_ESCAPE` |
| 7 | 2 | `0x1e` | `DIK_A` |
| 8 | 5 | `0x2e` | `DIK_C` |
| 9 | — | `0xcb` | `DIK_LEFT` |
| 10 | — | `0xcd` | `DIK_RIGHT` |
| 11 | — | `0xc8` | `DIK_UP` |
| 12 | — | `0xd0` | `DIK_DOWN` |

Joypad entries write only actions 0..8 (`*puVar8` through `puVar8[8]`); actions 9..12 are left
at the memset zero, consistent with the joypad axis path ignoring the binding value.

## Load-time VALIDATION — a gate that was not documented (2026-08-29)

`FUN_004971b0` reading the file is **not** the end of it. `FUN_00498510`'s second pass
(`0x00498694` onward, decompiled) does this per slot:

1. copy the freshly-built DEFAULT record into a `0x200` local;
2. call `FUN_004971b0`, which reads the file **over** the live record;
3. accept the loaded record only if **both**:
   - `record[+0x000] == default[+0x000]` (i.e. the constant **6** — this is what `+0x000`
     is FOR; it gates the load. Narrows U-9048: still no ordinary reader, but this
     comparison is a consumer), and
   - `strcmp(record + 0x004, default + 0x004) == 0` — the **device NAME must match**;
4. otherwise **copy the defaults back over the record**, discarding the file.

So a config only applies to the device it was saved for. A wrong name does not error: the
load "succeeds", is silently overwritten, and looks exactly like the file never being read.

**Slot assignment is NOT fixed, and this bites.** `FUN_00498510` gives slots
`0..joycount-1` to joypads and slot `joycount` to the keyboard. Measured on this machine:

| Slot | Type | Name |
|---|---|---|
| 0 | 1 joypad | `Keychron Link ` |
| 1 | 1 joypad | `Keychron Link ` |
| 2 | **2 keyboard** | `Keyboard` |
| 3 | 0 inactive | `` |

The keyboard is slot **2**, because a Keychron keyboard enumerates as **two** DirectInput
`GAMECTRL` devices. So `contcfg0.bin` is a JOYPAD config here. Any tool must read the live
table (or the files) to find the keyboard slot rather than assuming 0.

## END-TO-END REBIND VERIFIED (2026-08-29)

`re/tools/contcfg_edit.py` (dump / defaults / set) writes the record;
`re/frida/verify_rebind.py` proves it works. Test: take the live slot-2 record, rebind
action 4 (Select) from `DIK_RETURN` `0x1c` to `DIK_SPACE` `0x39`, install as
`original\contcfg2.bin`, boot.

| Check | Result |
|---|---|
| CLAIM 1 — file loaded byte-identical into `DAT_007e95c0 + 2*0x200` | **PASS** |
| CLAIM 2 — injecting `0x39` (SPACE) into the keyboard bitmap drives Select: screen 33 -> 0 | **PASS** |
| CONTROL — injecting unbound `0x14` (T) the same way | **PASS** (no movement) |

Injection is into the DirectInput keyboard bitmap `DAT_0077313c` on entry to `FUN_00496530`
(between the fill in `FUN_004972b0` and the read in `FUN_00497310`), for exactly one frame.
Deliberately **not** the `FUN_00497310` return-override that `probe_action_semantics.py`
uses — that bypasses the binding table, which is the thing under test here.

This exercises the whole chain: hand-written file -> loader -> name validation -> binding
table -> `ReadInputForAction` -> descriptor -> edge detector -> frontend transition.

**A rebind UI is unblocked on the data side.** What remains is UI, not RE.

## Consequences for a rebind feature

1. **The persistence format needs no new design.** Write a `0x200` record to `contcfg<N>.bin`
   and the stock loader consumes it. No checksum to defeat.
2. **Steering is rebindable through the same table as buttons** (actions 9..12), on the keyboard
   path. On the joypad path the axis binding value is ignored, so remapping *which stick* drives
   steering is **not** expressible in this record — it would need a change at `0x0049733d` /
   `0x0049734e` (the hardcoded `0x77311c` / `0x773120` pair table).
3. **No `contcfg*.bin` exists in `original\` today.** `patch_mashed_skip_controller_dialog` is
   applied, so `FUN_00498510`'s dialog never runs; `FUN_004971b0` returns 0 and the hardcoded
   defaults above are what the game uses. Any rebind work must either write the file directly or
   restore the dialog path.
4. Actions 2..8 must be named behaviourally (A/B capture per action) before a UI can label them.
   The `remap.lua` `RB_*` names are **not** transferable: see
   `re/analysis/lua_remap_correction_20260829.md` — the PC exe never loads that file.

## Actions 6 and 7 — effects pinned STATICALLY (2026-08-30)

Method: `analyzeHeadless` + `DecompBatch.java` on slot `Mashed_pool0` (`FUN_0043dfd0`,
`FUN_0043d7c0`, `FUN_0042af50`, `FUN_0042b770`, `FUN_0042c960`, `FUN_0043d2a0`, `FUN_00432800`,
`FUN_00431d90`, `FUN_0042c510`, `FUN_00431f30`), plus `XrefRange.java` over
`0x0067e900..0x0067f100` (932 + 473 refs). Ghidra MCP not used.

Neither action is given a UI-facing **name** by this pass. Both now have a fully-cited
**mechanical effect**, and — more useful — the reason each behavioural probe came back null is
now explained rather than open.

### The `0x0067ed3c` table is a menu STACK, and the index is `DAT_0067e9f8`

The gate the handoff asked about is not indexed by an arbitrary `idx`. Listing at
`0x004409d7`..`0x004409e6`:

```
004409ce  CALL 0x0042b770                       ; action-7 edge detector
004409d3  TEST EAX,EAX
004409d5  JZ   0x004409f2
004409d7  MOV  EAX,[0x0067e9f8]                 ; stack depth
004409dc  SHL  EAX,0x6                          ; * 0x40
004409df  CMP  dword ptr [EAX + 0x67ed3c],0x5   ; entry TYPE == 5
004409e6  JNZ  0x004409f2
004409e8  MOV  dword ptr [0x0067ec28],0x1       ; the effect
```

`FUN_0043d2a0(screen_id, mode)` is the stack manipulator and supplies the layout
(`0x0043d43b`..`0x0043d46a`), writing with the base `0x0067ed7c`:

| mode | Meaning | Evidence |
|---|---|---|
| 0 | **push**: `entry[d].ptr = PTR_DAT_005f7638[screen_id]`, `entry[d].type = screen_id`, `entry[d].sel = 0`, then `FUN_00432800(d)` | `0x0043d45d`, `0x0043d463`, `0x0043d46a` |
| 1 | **pop**: `DAT_0067e9f8 -= 1`, returns if `< 1`; pre-decrement it calls `FUN_00431f30(entry[d-2].type)` | decomp lines 28-32, 89-94 |
| 2 | **neither**: depth and current entry left alone; calls `FUN_00431d90()` | decomp lines 33-35, 86-88 |

Entry stride is `0x40`; `entry[k]` base is `0x0067ed7c + k*0x40`. The action-7 gate reads
`0x0067ed3c + d*0x40`, which is the **same field one entry lower** — i.e. `TYPE(d-1)`. The
`type` value **is the screen id** (mode 0 stores `param_1` into it verbatim).

This is why a range xref found no writer for `0x0067ed3c`: the only writer addresses the field
through the *sibling* base `0x0067ed7c`, exactly as the handoff predicted.

### Action 6 (U-9049) — closes every open frontend panel, without touching the stack

Detector `FUN_0042af50` **verified** to be action 6: it reads `(&DAT_007f1041)[i*0x4c]`
(descriptor `+0x09`) against snapshot `(&DAT_007f1501)[i*0x4c]` (`0x0042af50` decomp lines
29-31).

Entry guard at the top (`0x0042af50`): returns 0 when
`DAT_0067eab0 != 2 && DAT_0067e7c8 == 0 && DAT_00898ab0 != 0`.

Two consumers:

1. `FUN_0043dfd0` @ `0x0043feff` — gated on the per-screen enable `[ESP+0x1c] != 0`
   (`0x0043fef9`). On edge: `DAT_0067f19c = 1` (`0x0043ff0b`), `FUN_0043d2a0(0, 2)`
   (`0x0043ff11`), `DAT_0067e914 = 1` (`0x0043ff19`).
2. `FUN_0043d7c0` @ `0x0043da7c` — on edge: `DAT_0067eab0 = 0`, **`DAT_0067eac6 = 1`**
   (`0x0043da8b`), `DAT_0067f19c = ESI`.

`FUN_0043d2a0(0, 2)` is mode 2, so `param_1 = 0` matches none of the per-screen special cases
and the **only** effect is `FUN_00431d90()` (plus `FUN_00472640(0xff)` and `FUN_0042d3e0()`).

`FUN_00431d90` (`0x00431d90`..`0x00431f23`) applies one expression to **18** frontend panel
globals: `x = (x != 1) - 1 & 2`, which is `x == 1 ? 2 : 0`. The 19th, `DAT_0067e7b0`, is
special-cased to keep the value `1` and zero anything else (`0x00431dac`..`0x00431dbe`).

Per `re/analysis/frontend_config_screens_REmap_20260614.md:194-196` these globals are the
per-panel state words (`DAT_0067e7b0 -> FUN_0042f0c0`, `e7f8 -> FUN_00430b90`,
`e838 -> FUN_00431240`, `e830 -> FUN_004314b0`, `e820 -> FUN_00431710`, `e7e8 -> FUN_0042fb70`,
`e810 -> FUN_0042fe90`, `e818 -> FUN_00430120`). So the mechanical effect is: **every panel in
state 1 moves to state 2, every other panel to 0, with the menu stack depth and current entry
unchanged.** `[UNCERTAIN]` that state 2 is a close/exit animation — no writer of the state
machine that consumes 2 was read this pass.

**Three-way sibling finding.** In `FUN_0043d7c0`, actions 4, 5 and 6 each set a *distinct
adjacent byte*: action 4 → `DAT_0067eac4` (`0x0043da2c`, `0x0043da51`), action 5 →
`DAT_0067eac5` (`0x0043da6d`), action 6 → `DAT_0067eac6` (`0x0043da8b`). Given actions 4 and 5
are CONFIRMED Select and Back, action 6 is the third member of that same outcome set on this
screen. `[UNCERTAIN]` — no reader of `0x0067eac4..0x0067eac6` was resolved this pass.

**Why the A/B was null at the root menu and submenu:** two independent gates, either of which
suppresses it — `FUN_0042af50`'s entry guard, and the per-screen `[ESP+0x1c]` enable at
`0x0043fef9`. The run-2 null is therefore consistent with the action being live, and is not
evidence that it does nothing.

### Action 7 (U-9050) — advances a paged list, only on stack entry type 5

Detector `FUN_0042b770` **verified** to be action 7: descriptor `+0x11` (`0x007f1049`) against
snapshot `0x007f1509`.

Effect chain, each link cited:

1. `0x004409e8` — `DAT_0067ec28 = 1` (only when `TYPE(d-1) == 5`, and only when the per-screen
   enable `[ESP+0x34] != 0` at `0x004409c8`).
2. `FUN_0042c960` is the sole consumer (`0x0042c9b7` read, `0x0042c9c4` clear). Its whole body
   runs only when `DAT_0067e7c8 == 1` — one of the 18 panel globals action 6 resets. On the
   flag: `FUN_0042c510(); DAT_0067ec28 = 0;`.
3. `FUN_0042c510` (`0x0042c510`..`0x0042c7be`): `DAT_0067ea08 += 1`; wrap limit is `14`, or `2`
   when `DAT_0067e9fc == 2` (computed as `(bVar9 - 1 & 0xfffffff4) + 0xe`); zero-fills the
   `0x14`-dword queue at `DAT_0067ebd0`; resets `DAT_0067ebc8`, `DAT_0067ebcc`, `_DAT_0067ec20`;
   then skip-filters candidate entries against `DAT_007f0fd0` (`== 0` / `== 4` branches).
4. `FUN_0042c960` rebuilds the queue from `(&PTR_DAT_005f7854)[DAT_0067ea08]`, or
   `(&PTR_DAT_005f78ac)[DAT_0067ea08]` when `DAT_0067e9fc == 2`, appending ids into
   `(&DAT_0067ebd0)[DAT_0067ebc8]` and accumulating `FUN_004282a0(id, 0x3f333333)` into
   `_DAT_0067ec20`. `0x3f333333` is `0.7f`.

So action 7 **advances a paged content list by one page and rebuilds its item queue**.
`[UNCERTAIN]` what the pages hold: `FUN_004282a0(id, 0.7f)` accumulating a float into
`_DAT_0067ec20` is consistent with a text-width measurement at scale `0.7`, but
`FUN_004282a0` was not decompiled, so the accumulator is **not** named here.

**Why every A/B probe was null:** the gate needs `TYPE(d-1) == 5`, and **no literal push of
screen 5 exists in either menu tick**. The literal pushes in `FUN_0043dfd0` and `FUN_0043d7c0`
are `1, 4, 6, 7, 8, 0xa, 0xb, 0xd, 0xf, 0x10, 0x12, 0x13, 0x14, 0x18, 0x1e, 0x1f, 0x20, 0x21`.
`[UNCERTAIN]` — only those two functions were surveyed for callers of `FUN_0043d2a0`; a
whole-image caller scan was not run, so a literal `5` push elsewhere is not excluded. Within
the two menu ticks, screen 5 can only arrive through the two computed pushes:

- `FUN_0043d2a0(DAT_0067ecac, 0)` at `0x0043e0d2` — the fade-transition target, reached with
  `DAT_0067eca4 = 2` and `DAT_0067eca8 = 0xff`.
- `FUN_0043d2a0(uVar10, 0)` at `0x0043fc8b`-region (decomp line 847).

`DAT_0067ecac` has exactly three writers: `FUN_00432080` @ `0x004321cc`, `FUN_004331a0` @
`0x004331e1`, `FUN_00433240` @ `0x00433289`.

### U-9050 follow-up — the three writers were decompiled; static path is EXHAUSTED (2026-08-30)

All three were decompiled and **none stores a literal**. Each is a one-shot screen-entry
routine gated on `DAT_0067eca4 == 0`, taking the screen id as `param_1`:

| Function | Notable body |
|---|---|
| `FUN_004331a0` | sets `DAT_0067eca4 = 1`, `DAT_0067ecac = param_1`, `DAT_0067e9f8 = 0`, **`DAT_0067ea08 = 0`**, zero-fills the same `0x14`-dword queue at `DAT_0067ebd0`, resets `DAT_0067ebc8` / `DAT_0067ebcc` / `_DAT_0067ec20` |
| `FUN_00433240` | sets `DAT_0067ecb0 = 0x21` (**CURSCREEN 33, the title screen**), `DAT_0067e9f8 = 0`, `DAT_0067ecac = param_1` |
| `FUN_00432080` | writes `DAT_0067ecac` at `0x004321cc` |

`FUN_004331a0` initialises **exactly the paged-list state that action 7 advances**, which makes
it the entry point of interest. But its `param_1` is not a constant.

A `.text`-wide `E8 rel32` caller scan (exact encoding match, no heuristics) finds each has a
single-digit number of callers and **every one passes a computed value**, `PUSH EAX` from an
immediately preceding call:

| Callee | Call site | Preceding | Argument |
|---|---|---|---|
| `FUN_004331a0` | `0x00492af0` | `CALL 0x0042b910` | `PUSH EAX` |
| `FUN_00433240` | `0x00403474`, `0x004927a0` | `CALL 0x0042b920` (at `0x004927a0`) | `PUSH EAX` |
| `FUN_00432080` | `0x00492aca` | `CALL` (target outside `.text`) | `PUSH EAX` |

So the initial screen id is **computed at run time**, and no static constant `5` is reachable
along this path.

**Screen records are a tagged opcode stream, not a struct.** `PTR_DAT_005f7638` has 34 live
entries (`0x00`..`0x21`; `0x1b` is NULL, and `0x22` already reads out of bounds into the string
`"No Track"` at `0x005cd760`, which fixes the table length at 34). Each record is a sequence of
`ff<op>0000` tags with inline operands. Screen 5 (`0x005f6d58`) begins:

```
ff000000 00000037   ff080000 00000008   ff020000 00000050
ff030000 00000078   ff040000 ffffffff   ff140000 ff240000
ff060000 ff090000   ff230000 ff330000   ff0a0000 ff070000
```

versus screen `0x21` (title, `0x005f6f90`): `ff00 026b`, `ff08 0004`, `ff02 0050`,
`ff03 0078`, `ff04 026d`. The `ff00` operand is a **message id** — screen 5's is `0x37` (55),
the title screen's is `0x26b`. `[UNCERTAIN]` the meaning of every other opcode.

**Where it stops:** resolving message id `0x37` to text would name the screen and therefore the
action, but **the frontend strings are not in the exe**. A byte scan for `"Team 1"` (known to be
message `0x9f` per `frontend_config_screens_REmap_20260614.md:25`) finds no occurrence in
`MASHED.exe`, so the message bank is an external localised asset and no tool in `re/tools/`
reads it today.

### SCREEN 5 IDENTIFIED — "Race Results". Action 7 = "Change Stat" (2026-08-30)

Method: `re/frida/force_screen.py`. Rather than hunt for the screen, **push it**: call
`FUN_0043d2a0(5, 0)` (mode 0 = push) from inside a hook on the menu tick `FUN_0043dfd0`, i.e. on
the game's own thread at a frame boundary, then dump the backbuffer via
`MASHED_ORIG_BBDUMP_REQ`. One fresh process per run; own pid killed by pid.

**The static menu-stack model is now CONFIRMED behaviourally.** Live readings:

| State | `depth` | `types[]` | gate `*(0x67ed3c + depth*0x40)` | CURSCREEN |
|---|---|---|---|---|
| title screen | 1 | `[22, 0, 0]` | **22** = `types[0]` | 33 |
| root menu (prefix `4,4`) | 2 | `[22, 1, 0, 0]` | **1** = `types[1]` | 0 |
| after forced push of 5 | 3 | `[22, 1, 5, 0, 0]` | **5** = `types[2]` | 0 |

So `gate == TYPE(depth-1)` holds in every state measured, entry base `0x0067ed7c` stride `0x40`
is correct, and **the title screen is entry type 22 (`0x16`)**, the root menu type 1. Note
CURSCREEN (`0x0067ecb0`) numbers differently from the stack type — 33 vs 22 for the same screen.
Do not conflate them.

**Screen 5 renders as "Race Results"** (`verify/screen_id/screen_5.png`): header `Race Results`,
columns `Player` / `Result`, place rows `1st`/`2nd`/`2nd`/`2nd`, a `Stat -` element at left, and
a footer advertising exactly **two** actions: `Continue` and **`Change Stat`**
(`verify/screen_id/screen_5_footer.png`).

**Action 7 = Change Stat**, on four converging witnesses:

1. Screen 5 is Race Results, by image.
2. That screen advertises exactly two actions in its own footer.
3. `Continue` is action 4, already CONFIRMED Select/Confirm on two independent witnesses.
4. The action-7 block at `0x004409c8..0x004409e8` is the only code keyed to entry type 5, and its
   mechanism — advance page index `DAT_0067ea08` with wrap 14, clear the queue, rebuild content
   from `PTR_DAT_005f7854[page]` — is exactly "cycle to the next statistics page".

**RESIDUAL, stated plainly: the effect was never observed FIRING.** With the gate satisfied
(`gate = 5`) and action 7 forced, `DAT_0067ea08` stayed `0` and `DAT_0067ec28` stayed `0`. The
diagnosis is precise and rules out the consumer: pushing screen 5 sets `DAT_0067e7c8` from `0`
to `1`, so `FUN_0042c960` (the sole consumer, which requires `== 1`) *would* run. What does not
execute is the **producer** gate `[ESP+0x34]` at `0x004409c8` — the per-screen action-7 enable
computed earlier in `FUN_0043dfd0`, which a forced push does not set up. Filed as U-9059.
A true behavioural confirmation needs the screen entered by finishing a race.

**Method warning, re-confirmed:** the raw before/after pixel diff across firing action 7 returned
a bbox of `(0, 15, 640, 464)` — effectively the whole frame — because the background is animated.
It is worthless as an observable here, exactly as the run-2 warning above says. The footer crop
and the state globals are the usable channels.

Also negative-control by accident: a variant run pressed `4,4` after the push, which navigated on
to entry type 7; action 7 then fired with `gate = 7` and correctly did nothing.

Data: `log/force_screen_5.json`, `log/force_screen_5_fire7b.json`, `log/force_screen_5_diag.json`.

### FRONTEND FOOTER CATALOGUE — 33 screens, and action 6 is RULED OUT of the frontend (2026-08-30)

Same `force_screen.py` technique applied to every screen id `0x00..0x21` except `0x1b` (NULL
table entry). Six processes, batched; all 33 pushed without a crash. Footer crops assembled into
`verify/screen_id/footers_0.png` and `footers_1.png`.

| Footer | Screen ids |
|---|---|
| `Select` + `Back` | 2, 3, 4, 6, 8, 9, 11, 12, 13, 15, 16, 18, 20, 24, 29 |
| `Confirm` + `Back` | 19, 26, 28, 31, 32, 33 |
| `Select` only | 0, 1, 21, 23 |
| `Back` only | 10, 14, 17, 30 |
| `Select` + **`Main Menu`** | **7** |
| `Continue` + **`Change Stat`** | **5** |
| (empty footer) | 22, 25 |

**Only three prompt icons exist in the whole frontend:** the green circular arrow (action 4 —
`Select` / `Confirm` / `Continue`), the red circular arrow (action 5 — `Back`, relabelled
`Main Menu` on screen 7), and the square (action 7 — `Change Stat`, screen 5 only). Screen 7's
`Main Menu` is action 5 with a different label, not a fourth action: the icon is the red arrow.

**Result for U-9049: action 6 is never advertised anywhere in the frontend.** No screen shows a
fourth prompt. Combined with the other evidence — inert on both menus, its `FUN_0043d7c0`
outcome byte `0x0067eac6` a dead store (U-9053), and its only live effect being
`FUN_0043d2a0(0, 2)` = close all open panels without touching the stack — the frontend is
**ruled out** as the place action 6 can be named. That is a real narrowing, not a null: it
redirects U-9049 to the race / pause lane, which is the option the row originally listed second.

Consistency note: closing all panels without a stack change is precisely what dismissing the
title panel would do, which matches the one place action 6 IS accepted (screen 22, the title
screen, where the footer is empty and the body reads "Press button to start"). Action 6 is
therefore distinguishable from action 5 — on the title screen 4 and 6 advance while 5 and 7 do
not — but "an alternate dismiss that only acts where a panel is open and no stack navigation is
involved" is a mechanism, not a name. NOT named.

Bonus: this catalogue covers 33 screens against the 17 in
`re/analysis/frontend_config_screens_REmap_20260614.md`, several of which are marked
`[UNCERTAIN — not yet pinned]` there. Captures are `verify/screen_id/screen_<id>.bmp`.

### Action 6 in a race — NULL, but a WEAK test that should not be cited as evidence (2026-08-30)

Attempted per U-9049's option (2). `scenario_launch.py --track 0 --mode 10 --cars 1 --hold 150`
warped into a running race; `force_screen.py --attach <pid> --watch 8 --fire 6` sampled state at
4 Hz for 8 s as a control, forced one action-6 edge, then sampled 8 s more. The probe attached to
the launcher's pid and did **not** kill it.

Confirmations: `fired_delta == 1` (the override reached `FUN_00497310`, so the input layer was
live). **CORRECTION 2026-08-30:** this passage originally also cited `DAT_007f0f10 == 2` as proof
the process was "genuinely in a race". That was WRONG — `0x007f0f10` reads `2` at the title
screen too, and `re/analysis/structs/frontend_state.md:145` already documents it as
`g_itemSelectorP3`, an item selector, not a race flag. The real session phase is
`0x00771968` (U8, `1=menu 2=load+spawn 3=race`, per `scenario_launch.py:41`). That run WAS in a
race, but on the launcher's own verdict, not on this global. Result:
every sampled global was flat across both windows — `depth=1`, `types=[22,0,0]`, `curscreen=33`,
`e7c8=0`, `eab0=0`, `flag=0`.

**Do not record this as evidence that action 6 is inert in a race.** Three defects, the third
disqualifying:

1. The race was **warped into**, not entered naturally, so the menu stack was never populated —
   `depth=1`, `types=[22,0,0]` is still the *title screen's* stack left over from boot.
2. `scenario_launch.py` **pulses action 4 continuously** to skip intro rounds, which would dismiss
   a transient overlay. 4 Hz sampling makes a persistent overlay visible, but not a one-frame one.
3. Decisive: action 6's **only** consumers are the two menu ticks `FUN_0043d7c0` and
   `FUN_0043dfd0`. Those plainly were not running in this state — the frontend globals never
   moved at all, including ones the launcher's own action-4 pulses would have perturbed. So this
   configuration **could not have produced a positive** regardless of what action 6 does. A null
   from a test that cannot fail is not evidence.

What a real test needs: a race entered through the frontend (so the menu stack is live beneath
it) with no competing input pulse, then one forced action-6 edge while watching `DAT_0067e9f8`
and the panel globals. Until then U-9049 stays open and the race lane stays untested, not
excluded.

### ACTION 6 = PAUSE — U-9049 CLOSED (2026-08-30)

Proper retry of the race lane: enter a race **through the frontend** with no competing input
pulse. `re/frida/force_screen.py --to-race` presses action 4 at 1.8 s intervals, samples the
stack after each, and **stops pressing** the moment session phase `0x00771968` reads `3`.

Drive path, menu-stack types at each confirm: `[22] -> [22,1] -> [22,1,2] -> [22,1,2,4] ->
[22,1,2,4,6] -> race`. Eight confirms, arriving at depth 5, `sphase = 3`, `phase = 0`.

| Window | Result |
|---|---|
| Control, 8 s at 4 Hz, no input | **flat** — `depth=5`, `types=[22,1,2,4,6]`, `sphase=3` |
| One forced action-6 edge (`fired_delta=1`) | `depth` **5 -> 0** (types zeroed) -> `1` with type `0`; `sphase` **3 -> 5 -> 4** |
| **Negative control**, identical timing, press that never lands (`fired_delta=0`) | **flat for the full 16 s** — no spontaneous transition |

The negative control is what makes this attributable, and it is the discipline this row lacked
for three prior rounds.

**The settled state, captured: the PAUSE MENU** (`verify/screen_id/race_fire6_settled.png`).
Header `Transmission Interrupted`, options `Continue` / `Options` / `Restart Race` / `Quit Race`
/ `Quit Game`, footer `Select`, with the race frozen behind it.

**ACTION 6 = PAUSE.**

Every earlier null is explained by this, none of them contradicted:

- Absent from all 33 frontend footers because pause is not a frontend action.
- Inert on the root menu and the submenu because there is nothing to pause.
- Accepted on the title screen because its mechanism is `FUN_0043d2a0(0, 2)` = close every open
  panel with no stack change, which dismisses the title panel.
- Its `FUN_0043d7c0` outcome byte being a dead store (U-9053) is consistent: that function is a
  frontend tick, not the pause path.
- Sharing `DIK_ESCAPE` with action 5 (Back) is idiomatic for pause.

**Screen-type map extended** (menu-stack entry type, NOT `CURSCREEN`):

| Type | Screen | How established |
|---|---|---|
| 0 | **Pause menu** | this run; footer `Select` only, matching the catalogue entry for type 0 |
| 1 | Game Type Select (root) | live read at the root menu |
| 5 | Race Results | forced push + image |
| 22 | Title screen | live read at boot |

Data: `log/race_a6_frontend2.json` (fire), `log/race_a6_control.json` (negative control),
`log/race_a6_settle.json` (settle + capture).

**The remaining generic step is BEHAVIOURAL, not static.** `TYPE(d-1)` is a plain global expression
— read `*(0x0067ed3c + DAT_0067e9f8*0x40)` live while navigating, and screenshot whichever
screen reports `5`. That identifies the screen by image instead of by string bank, and it is a
memory read, so the hot-path `Interceptor` rule does not apply. Reuse the navigation and
`MASHED_ORIG_BBDUMP_REQ` capture already in `re/frida/probe_action_semantics.py`. Do **not**
resume blind dwell-navigation: without reading `TYPE` there is no observable telling you when
you have arrived.

## Open items

| Ref | Gap |
|---|---|
| new | `+0x144..+0x1ff` (`0xbc` bytes) has no writer in the 7 functions examined; persisted but unexplained |
| new | `+0x000` constant `6` has no reader found |
| new | semantic names for actions 6, 7, 8 (2, 3, 4, 5 are named; 6 and 7 have pinned effects but no UI name) |
| U-9050 | which of `FUN_00432080` / `FUN_004331a0` / `FUN_00433240` stores `5` into `DAT_0067ecac` — names screen 5 and therefore action 7 |
| U-9049 | reader of the outcome bytes `0x0067eac4` / `0x0067eac5` / `0x0067eac6` (action 4 / 5 / 6 on `FUN_0043d7c0`'s screen) |
| new | confirm panel state `2` is a close/exit transition — read the consumer of the 18 globals `FUN_00431d90` writes |
| new | `DAT_007f1a0c == 0x1000` — meaning of the detector-branch selector |
| new | `FUN_004282a0(id, 0.7f)` — is `_DAT_0067ec20` a text width or a duration? |
| new | why action 2's result is duplicated to descriptor `+0x13` |
| new | Y axis (actions 11/12) returns flat `0xff` with no proportional path — is analog Y reachable at all? |
| U-3024 / S-2285 | `FUN_004a2c48` not decompiled |
| — | writers of the axis tables `0x0077311c` / `0x00773120` (read-only in `FUN_00497310`) |

## Reproducing

```
bash scripts/ghidra_pool.sh acquire            # -> Mashed_poolN
printf '0x004971b0\n0x00497230\n0x00497190\n0x00498510\n0x00497310\n0x00496530\n0x004982a0\n' > valist.txt
"C:\Users\maria\Desktop\Proyectos\TD5RE\ghidra\ghidra_12.0.3_PUBLIC\support\analyzeHeadless.bat" \
  "C:\Users\maria\Desktop\Proyectos\Mashed\mashed_pool" Mashed_poolN \
  -process MASHED.exe -noanalysis -readOnly \
  -postScript DecompBatch.java valist.txt <out_dir> \
  -scriptPath "C:\Users\maria\Desktop\Proyectos\Mashed\re\tools\ghidra_scripts"
```

Note: the VA list must be a **file**. `analyzeHeadless.bat` runs under `cmd.exe`, where comma
and semicolon are argument delimiters, so an inline comma-separated list is silently split and
the second VA is consumed as the output directory.
