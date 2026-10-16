# PREREG txdscope: faithful texture-dictionary scope (U-9724)

Written 2026-10-16, before the first measuring run of any ported code. Base `2cc15b2cf` (`race/first-frame-parity`), branch `work/txdscope`,
worktree `.worktrees/txdscope`. User decision (Mariano, 2026-10-16): make the texture lookup faithful.

## What the original does (static decode, this lane, before any measurement)

- **Lua handler.** `Texture_Dictionary_Filename` is registered at `0x00440c2e` (push of the string) (string `0x005cdf70`, handler `0x0047a6f0`). The handler copies its Lua
  string argument to `DAT_006bf1c8` (the course description base, +0x0; the sibling `0x0047a1b0` writes +0x1980). Last call wins.
- **Load.** `Course::CreateFromDescription` `FUN_00479330`: `FUN_0042a6b0(desc, 0, 0)` (call `0x00479372`) loads the named file
  (`.txd` appended when the name has no `.`, `PTR_DAT_005f65b8`), stores it at course+0x105d0 (`0x0047937c`), and **only when non-null** (test at `0x004793d3`)
  calls `FUN_004c5c80(dict)` (`0x004793de`). That call precedes every other asset load of the function: world BSP `FUN_0042a640(desc+0x140)` (`0x004793fc`),
  collision BSPs, the 0x40 clumps `FUN_0042a5d0(desc+0x980..)`,
  the object world, sky domes (`desc+0x40..`), Mts (`FUN_00478110`), splines. The function never calls it again with another value, so the course
  dictionary is the current one for the whole track load.
- **Current dictionary.** `FUN_004c5c80` stores into the texture-module block slot +0x10 (`DAT_007d4054` + `DAT_007d3ff8`); `FUN_004c5ca0` reads it back.
  `0x004c5800` / `0x004c5820` (named SetCurrent / GetCurrent in the trackers) address slot +0x1c. `FUN_004c6ba0` reads slots +0x1c (`0x004c6cb3`) and
  +0x20 (`0x004c6cb9`) to pick the value it passes on as `0x4` / `0x8004` / `0x9004` (`0x004c6cbd`, `0x004c6cc6`), and the stream reader `FUN_00550130`
  saves, clears and restores both around a texture read. Mechanically they are not the dictionary the lookup walks, so the tracker names of
  `0x004c5800` / `0x004c5820` do not match the bytes (new U-row).
- **Resolve.** `FUN_004c5cb0(name, mask)` (the rwID_TEXTURE reader `FUN_004cf5a0` reads the name and mask strings with `FUN_004d8810` and calls it; a 0 result
  makes `FUN_004cf5a0` return 0): find callback [+0x18], default `0x004c7490` (set at
  `0x004c60d0`), walks the **slot +0x10 dictionary** comparing names case-insensitively; on a miss, read callback [+0x14], default `0x004c61b0` (set at
  `0x004c60e3`); a failure raises error `0x16` and returns 0 (`0x004c5d09` / `0x004c5d2f`); a success is linked into the slot +0x10 dictionary
  (`0x004c5d4d`). No other dictionary is searched.
- **No custom callbacks.** The only stores into slots +0x10 / +0x14 / +0x18 in the whole image are `0x004c5c90`, `0x004c6057`, `0x004c60d0`, `0x004c60e3`
  (capstone with skipdata over `.text`), and all 54 code references to the module offset `0x007d4054` lie in `0x004c5800..0x004c74a0`.
- **Disk fallback cannot help a track.** No track piz holds a `.png/.bmp/.dds/.ico` entry (census of all 59 piz; image entries exist only in Font36, Perm,
  Powerups, SFX, dirt). `0x004c61b0` internals are not decoded here: [UNCERTAIN] what path it searches; moot for tracks.
- **TXD stream reader** `FUN_0054f8d0`: saves the current dictionary (`FUN_004c5ca0`), makes the new one current while reading, restores it
  (`0x0054fd22`). Loading a TXD never changes the caller's current dictionary.
- **Other contexts** (every `FUN_004c5c80` caller read): the global FX dictionaries (`fx.txd`, `badges.txd`, `TrackImages.txd`, `Interface.txd`,
  `FUN_0040bbb0`) end with `FUN_004c5c80(0)`. FX textures by name go through `FUN_0040bb30(name)` = `FUN_004c5c00(DAT_0063b8f8, name)`: **`fx.txd` only**
  (`RWObjShad` `0x0040bcbd`, `Headlight` `0x00459297`, `lensflare-fs8` `0x00402908`/`0x00454f22`/`0x00458484`). Vehicle: `FUN_00420420` loads one
  dictionary (`vehicle table +0x20` + `.txd`, `FUN_0042a6b0` at `0x004204c0`) only when `DAT_0063d7e8 == 0`, makes it current (`0x004204ce`) for the DFF read
  (`0x0042050f`), reads the env names from it with `FUN_004c5c00(DAT_0063d7e8, ...)` (`0x00420595`) and ends with `FUN_004c5c80(0)`; `FUN_0041ec00` frees `DAT_0063d7e8` only when
  the live-vehicle counter `DAT_0063e498` is 1, so one vehicle dictionary serves every car of a race (the first one instantiated).
  Copters: `FUN_00425c00` makes `DAT_00644150` (`Copters.txd`) current. Airstrike: `FUN_0041a060` makes `airstrike` current and does not reset it.

## Census of the shipped data (from the piz bytes)

| archive | TXD entries | COURSE.LUA `Texture_Dictionary_Filename` |
|---|---|---|
| 12 tracks | exactly one (`TEXTURES.TXD`, `CITY.TXD`, `NEUSTEIN.TXD`, `SANDS.TXD`, `WARZONE.TXD`) | that one |
| `dump.piz` | `DUMP.TXD` (56 textures), `CITY.TXD` (104; 90 names are not in DUMP.TXD, e.g. `trainroof`) | `dump.txd` |
| 13 vehicle piz | `<CAR>.TXD` (4 textures) and `PS2/<CAR>.TXD` (decodes to 0 textures; Crusader's is `PS2/CREEPER.TXD`) | n/a |
| `SFX.piz` | `FX.TXD` (36), `INTERFACE.TXD` (30), `BADGES.TXD` (23), `TRACKIMAGES.TXD` (39), `XBOX/` and `PS2/BADGES.TXD` (0) | n/a |

The five FX names the standalone looks up (`RWObjShad`, `Headlight`, `lensflare-fs8`, `car_shadow`, `beam`) exist only in `FX.TXD`.

## The standalone today

Three sites search more than the original: `TrackRenderer.cpp` track load (every `*.TXD` of the piz, `:1561`), `TrackRenderer::RegisterCarModel`
(every `*.TXD` of the car piz, `:3607`) and `RwRaceFx.cpp` `SfxTexture` (every TXD of `SFX.piz`, `:353`). Everything else (Copters, PowerUpIcons,
TrackImages, Badges, Interface, Panel, FX.TXD for beam / car_shadow) already opens one TXD by name.

## Port

- Track: decode only the TXD that COURSE.LUA names (`Texture_Dictionary_Filename`, last call wins, case-insensitive match against the piz entry, `.txd` appended
  when the name has no `.`). Named file absent from the piz: no dictionary (the original leaves slot +0x10 unchanged and every lookup misses; logged `WARN`).
  COURSE.LUA without the call (none shipped): the legacy every-TXD behaviour.
- Car: the entry named `<anything>.TXD` without a path separator (the PC dictionary), not the `PS2/` copy.
- FX: `SfxTexture` looks only in `FX.TXD`.
- Opt-out `MASHED_NO_TXDSCOPE=1` restores all three to the base behaviour exactly. Default ON.

## Predictions

- **Pr-1 (11 of the 12 single-TXD tracks plus Dump's siblings):** the draw files of the 12 tracks other than Dump are byte-identical to the base, row for row,
  because the decoded dictionary set does not change (one TXD each). Car and FX sites change no set that a lookup can reach (PS2 TXDs decode empty; FX names exist
  only in FX.TXD): identical.
- **Pr-2 (Dump):** the material `trainroof` (and every other material of Dump's BSP or props whose name exists only in `CITY.TXD`) resolves to no texture,
  as the original's A-side draw at the roof AABB (Dump 735, cite C30) shows. The C30 row disappears. Draws that lose a texture are listed per name in
  `RESULT_txdscope.md`. **No new row** on any frame of any track.
- **Pr-3 (counts):** 13 tracks x 3 frames: rows 1047 -> at most 1047 (Dump 60 -> at most 59), 0 RED.
- **Pr-4 (G-CTRL):** unchanged, `d3b90ee0` / `80f01f35` / `56d62ae7` (TRAINING has one TXD; the sim does not read textures). Measured on a baseline exe built from `2cc15b2cf`.
- **Pr-5 (control):** the new exe with `MASHED_NO_TXDSCOPE=1` reproduces the base draw files byte for byte on the six tracks measured.

If a track other than Dump moves, or a row is added, the cause is read from the bytes before anything is kept. A texture that only resolved through the
over-broad search is listed with its track and cause, not hidden.

## Acceptance

Matched-pose per-draw replay + `drawlist_diff.py` 3D scoring on the 13 tracks x 3 frames (`verify/rsky_20261016/run13.py`, private game root, cites
`verify/rskyverify_20261016/cites_merged.json`), base arm and after arm on exes built from the same tree; G-CTRL (`run_gctrl_priv.py`) on both;
opt-out control on Dump, Neustein, Sands, Warzone, City, Training.
