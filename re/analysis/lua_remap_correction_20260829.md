# Correction: Lua 4.0 does NOT drive joypad remap on PC (2026-08-29)

**Verdict: the long-standing claim "Lua 4.0 for joypad remap" is FALSE for `MASHED.exe`.**
Lua 4.0 is present and is used for track/powerup scripting. It has no input role.
The PC remap path is the `contcfg%d.bin` binding table.

Sibling of U-9042 (which corrected the Lua *version* from 5.x to 4.0 on 2026-08-18 but
left the *purpose* claim untouched).

## Evidence

Scanned `original\MASHED.exe.unpatched` (2,846,720 bytes, SHA-256 anchor per CLAUDE.md).

| Byte pattern | Occurrences |
|---|---|
| `remap` | **0** |
| `Remap` | **0** |
| `REMAP` | **0** |
| `LuaRemapButton` | **0** |
| `RB_GAME` | **0** |
| `JOYPAD_BUTTON` | **0** |
| `contcfg` | 1, at file offset `0x5d000c` |
| `Lua 4.0` | 1, at file offset `0x5d8796` (inside `$Lua: Lua 4.0 Copyright (C) 1994-2000 TeCGraf, PUC-Rio $`) |

Every `.lua` / `.LUA` string in the image, exhaustively (regex `[ -~]{4,}\.(lua|LUA)`):

| File offset | String |
|---|---|
| `0x1cd4b0` | `KTCScript.lua` |
| `0x1cd4d4` | `powerups.lua` |
| `0x1cd4f8` | `powerups_gold.lua` |
| `0x1cd578` | `LAPDATA.LUA` |
| `0x1cd584` | `COURSE.LUA` |
| `0x1ce628` | `powerups_all.lua` |

Six strings. All track or powerup scripting. None is a remap script.

## What `remap.lua` actually is

`original\TOASTART\PC\remap.lua` exists on disk (2,754 bytes, plain text). It contains 32
`LuaRemapButton(colour, action, buttonmask)` calls covering RED/BLUE/GREEN/YELLOW across
eight actions (`RB_GAME_ACCELERATE`, `RB_GAME_BRAKE1`, `RB_GAME_BRAKE2`, `RB_GAME_FIRE`,
`RB_POWERUP_TOGGLE`, `RB_PRESENT_SELECT`, `RB_PRESENT_BACK`, `RB_PRESENT_PAUSE`) with
`JOYPAD_BUTTON_00..15` as bitmasks 1..32768. **Buttons only, no axis entries.**

Neither the filename nor the binding name `LuaRemapButton` appears anywhere in the PE image,
so nothing in the PC build can open or execute it. The harvested Lua binding table
(`re/analysis/lua_binding_names_20260818.md`, 70 names, registrars `0x0047b980` / `0x004714f0`)
contains no remap or input binding.

`[UNCERTAIN]` whether the Xbox/PS2 builds consumed it. Not investigated. The file being
shipped in a `PC\` folder while unreferenced by the PC exe is consistent with a console-era
leftover, but that is inference and is **not** asserted here.

## The real PC remap path

| RVA | Role |
|---|---|
| `0x004971b0` | `ControllerConfigLoad_j5` — reads `contcfg%d.bin`, `0x200` bytes per slot |
| `0x00497230` | `SaveControllerConfig` — `fopen` + `fwrite(0x200)` + `fclose` |
| `0x00497190` | filename formatter for `contcfg%d.bin` |
| `0x00498510` | default-binding builder; also `DialogBoxParamA(id=0x67)` |
| `0x00497310` | `ReadInputForAction(slot, action)` — the single binding lookup |

In-memory mirror: `DAT_007e95c0`, stride `0x200` (4 slots, `0x007e95c0..0x007e9ec8`).
Device type at `+0x13c` (`DAT_007e96fc`, 1 = joypad, 2 = keyboard); joypad index at `+0x140`
(`DAT_007e9700`); binding byte array at `+0x108` (`DAT_007e96c8`). For keyboard the binding
byte is a raw DIK scancode.

## Consequences

1. Any rebind feature targets `contcfg%d.bin` / `DAT_007e95c0`, **not** a Lua script.
   `remap.lua` is inert; editing it changes nothing.
2. The `hooks.csv` `subsystem=input` tag on the Lua-core cluster (`0x0047b860`, `0x0047b880`,
   `0x0047b8d0`, and the `0x004b7xxx` / `0x004c0xxx` functions) is a mislabel inherited from the
   `input_lua*` bucket naming. Those are interpreter internals. **Not corrected here** — a
   subsystem retag is a `re-classify` transaction, filed as follow-up.
3. `DEFERRED.md` D-0820's re-pickup condition ("pick up only if Mashed needs scriptable joypad
   remap rebuilt rather than wrapped") rests on the false premise. There is no scriptable
   joypad remap to rebuild. **Not edited here** — DEFERRED mutations go through `re-classify`.
4. Several analysis plates carry `[UNCERTAIN]` speculation seeded by the false claim, notably
   `re/analysis/bucket_00405400/0x00407b00.md`, `0x00407be0.md`, `0x00408610.md`. Those guesses
   are unsupported. Left in place as historical record; do not treat them as leads.

## Docs corrected in this pass

- `CLAUDE.md:24` — "Other tech" line.
- `re/analysis/subsystem_map.md` — input Role and Fingerprints (the `remap.lua` string anchor
  was a fingerprint for a string that does not exist).
- `re/frida/CANONICAL_INPUT_DESIGN.md:39` — the parenthetical justifying a processed-input global.
