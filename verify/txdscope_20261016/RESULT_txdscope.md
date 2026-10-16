# RESULT txdscope: faithful texture-dictionary scope (U-9724)

**Verdict: kept, default ON.** The standalone now looks a texture name up in the one dictionary the original has current: the track piz's
`Texture_Dictionary_Filename` file for the world, props and sky, the car's own PC `.TXD` for a car, `FX.TXD` for the FX names. On
`race/first-frame-parity` `2cc15b2cf` (`work/txdscope`, `MASHED_NO_DEPLOY=1`, private game root) the 13-track x 3-frame scoring gives
**1047 rows, 0 RED, identical to the base's 1047 / 0**; 36 of 39 draw files are byte-identical to the base and the 3 that differ are Dump's, by
**one draw per frame**: the train roof loses `trainroof`. G-CTRL is unchanged (`d3b90ee0` / `80f01f35` / `56d62ae7`). Plan: `PREREG_txdscope.md`.

- Base exe `log/txdscope/exe_base.exe` SHA-256 `E2102CEA021FC9F9EEC8D6CFBAAA063966BF39766CE3459AC2AD2875A419CC97` (clean build of `2cc15b2cf`).
  After exe `exe_after.exe` `510375F6CE1B96643B45BE6CF87E905F5230D8CC14A9D9DAC50625734ABDA59B` (same tree + this change). Both `Build OK`, `[skip] MASHED_NO_DEPLOY set`.
- Method as in RESULT_rsky / RESULT_rskyverify: `verify/rsky_20261016/run13.py` (private root = robocopy of `original\` with `original\` inside, same traces, shots,
  frames), `drawlist_diff.py` 3D scoring. Replays: base 13 of 13, after 13 of 13, control 6 of 6, all first attempt.

## 1. What the original does (bytes; full citations in PREREG_txdscope.md)

- `Texture_Dictionary_Filename` (registered `0x00440c2e`, handler `0x0047a6f0`) copies its string to the course description base; `CreateFromDescription`
  `0x00479330` loads it (`FUN_0042a6b0` call `0x00479372`) and, when non-null, makes it current (`FUN_004c5c80` call `0x004793de`) before the world BSP, the
  clumps, the sky domes and the Mts are read. Nothing changes it afterwards.
- `FUN_004c5cb0` (`RwTextureRead`; the rwID_TEXTURE reader `FUN_004cf5a0` calls it) walks the **current dictionary only** (default find callback `0x004c7490`,
  case-insensitive), then the read callback `0x004c61b0`; a failure returns 0. The only stores into the callback / current slots in the image are `0x004c5c90`,
  `0x004c6057`, `0x004c60d0`, `0x004c60e3`, so no game code installs another callback; no track piz holds an image entry the read fallback could supply.
- FX names: `FUN_0040bb30` = `FUN_004c5c00(DAT_0063b8f8, name)`, the `fx.txd` dictionary only. Vehicle: `FUN_00420420` makes the vehicle's own `<name>.txd` current
  (`0x004204c0` / `0x004204ce`).

## 2. What changed

| site | before | after (default) |
|---|---|---|
| `TrackRenderer::Load` | every `*.TXD` of the track piz | only the file `Texture_Dictionary_Filename` names (last call wins, case-insensitive, `.txd` appended without a `.`); named file absent: no dictionary; no call (none shipped): every TXD |
| `TrackRenderer::RegisterCarModel` | every `*.TXD` of the car piz | the `*.TXD` entry without a path (not `PS2/<CAR>.TXD`, which decodes to 0 textures) |
| `RwRaceFx.cpp` `SfxTexture` | every TXD of `SFX.piz` | `FX.TXD` only |

`MASHED_NO_TXDSCOPE=1` restores all three. Every other TXD load (Copters, PowerUpIcons, TrackImages, Badges, Interface, Panel, FX.TXD beam / car_shadow) already
opened one TXD by name. `TrackRenderer::Load` is reached only from the track view and the race launch (`exe_main.cpp:2039`, `:9289`), `RegisterCarModel` and
`SfxTexture` only from race code: no frontend screen goes through the changed sites, so the frontend parity screens are not affected (grep, not a run).

## 3. Rows per track (13 tracks x 3 frames, cites `cites_merged.json`; after arm with `cites_txdscope.json`, section 4)

| track | base | after | draw files byte-identical to base |
|---|---|---|---|
| Egypt | 67 | 67 | 3 of 3 |
| Neustein | 64 | 64 | 3 of 3 |
| Arctic | 103 | 103 | 3 of 3 |
| Highway | 58 | 58 | 3 of 3 |
| Sands | 70 | 70 | 3 of 3 |
| SuperG | 210 | 210 | 3 of 3 |
| Roundabout | 88 | 88 | 3 of 3 |
| Storm | 69 | 69 | 3 of 3 |
| Forest | 57 | 57 | 3 of 3 |
| **Dump** | 60 | 60 | **0 of 3** (one draw per frame, section 4) |
| Warzone | 79 | 79 | 3 of 3 |
| City | 59 | 59 | 3 of 3 |
| Training | 63 | 63 | 3 of 3 |
| **all** | **1047** | **1047** | **36 of 39** |

0 RED on both. The instrument gate line is FAIL on both for the same single reason, the known capfix Neustein row 1169 I-NAME miss (73/77, original side);
the gates files differ only on Dump's stand lines (`named` 337/337 -> 336/336, 314/314 -> 313/313, 203/203 -> 202/202: the roof draw no longer carries a name; I-NAME true on all).

## 4. Dump: the one place the scope matters

- `dump_delta.py`: per frame **1 differing event of 341 / 318 / 206**, and nothing else: `tex "trainroof" -> "-"` (row 810 also `td #64x64:21 -> #32x32:21` on that same event; the field's
  meaning is not decoded here). The other 340 / 317 / 205 events are byte-equal. `evidence/dump_delta.txt`.
- Row identities (`evidence/compare_rows_base_vs_after.txt`): `EXTRA trainroof` (C30-TXD-SCOPE on 735, C01-CULL on 775 / 810) is REMOVED, a `MOVED '-'` row is ADDED,
  and `MISSING '-' C03-UNTEXTURED` goes 8 -> 7: the roof, now untextured on both sides, **pairs** with the original's untextured roof draw (`matched` 330 / 313 / 202 unchanged).
  Row count per frame unchanged (21 / 18 / 21).
- **That new row was RED under the old cites** (`evidence/after_with_merged_cites/`). Cause read from the rows: the standalone-side box of the new `MOVED` row equals, to the
  printed digit, the box of the old `EXTRA trainroof` row on all three frames (`[-4.02,2.95,49.27,5.77,3.11,50.05]` on 735), so the roof geometry did not move; the original's roof
  sits 0.05..0.06 behind it in x, the same maximum shift (**0.0571**) as the already-cited train body row `C28-ANIM-CLUMP` (`[-4.11,...,5.75]` vs `[-4.06,...,5.80]`). Before, the texture mismatch
  kept the two roof draws from pairing, which hid the train's own one-tick lead (U-9725). `cites_txdscope.json` = the 42 merged cites minus `C30-TXD-SCOPE` (retired) plus
  `C30-TRAIN-ROOF-LEAD` (kind MOVED, texture `-`, Dump only, detail must carry `moved by up to 0.0571`); it cites exactly these 3 rows and nothing on another track.

## 5. Controls

- **Opt-out (`ctl/`).** `exe_after.exe` with `MASHED_NO_TXDSCOPE=1` on Neustein, Sands, Dump, Warzone, City, Training: **18 of 18 draw files byte-identical to the base**, Dump included
  (`evidence/compare_draws_base_vs_ctl.txt`).
- **Determinism.** Dump replayed again alone on each exe (`dump_base/`, `dump_after/`): 3 of 3 byte-identical to that exe's first Dump run; `dump_after/mashed_re.log`:
  `TXDSCOPE texdic 'dump.txd' -> 1 of 2 TXD(s) of the piz`.
- **G-CTRL** (`run_gctrl_priv.py`, Training, 14400 frames): base and after both give step `d3b90ee0` (18671 lines), gates `80f01f35` (55433), ptrace `56d62ae7` (27716) = the expected default.

## 6. Textures that resolved only through the broad search (rule 6)

`scope_census.py` reads every texture name referenced by a track piz's DFF / BSP entries and looks it up in the named dictionary and the piz's other TXDs (`evidence/scope_census.txt`).
Twelve tracks have one TXD, so no name can resolve outside the named one. Dump has two names found only in `CITY.TXD`:

| track | name | referenced by | cause | effect |
|---|---|---|---|---|
| Dump | `trainroof` | `TRAIN.DFF` (loaded, drawn) | in `CITY.TXD`, not in `DUMP.TXD`; COURSE.LUA line 52 names `dump.txd` | roof (32 triangles) now untextured, as the original draws it (A-side draw, Dump 735) |
| Dump | `building_02nt` | `TRUNK.DFF` | COURSE.LUA line 89 is `--Clump_Filename(11,"Trunk.dff")`, commented out | none: neither build loads the clump (the census scans every entry of the piz) |

Names referenced but present in no dictionary (Forest 12, Warzone 1, City 2, Sands 1, Training 1, Dump 4) were untextured in the base too; unchanged.

## 7. Prediction scorecard (PREREG_txdscope.md)

- Pr-1 (12 other tracks identical; car and FX sites change no reachable set): **held**, 36 of 36 files.
- Pr-2 (roof untextured, C30 row gone, **no new row**): the roof and C30 held; **"no new row" did not hold**: one `MOVED '-'` row per Dump frame appeared (section 4), uncited until the train lead was recognised on it.
- Pr-3 (Dump 60 -> at most 59): **did not hold**, 60 -> 60 (one row replaced by one, same count).
- Pr-4 (G-CTRL unchanged): held. Pr-5 (opt-out reproduces base): held, 18 of 18.

## 8. Not measured

Multi-car races (the captures carry the player's car only; the original loads ONE vehicle dictionary for every car, see section 9). Powerup, HUD and frontend dictionaries (not touched).
Pixels were not compared: the draw lists carry the texture binding, which is what changed.

## 9. Found on the way (new U-rows)

1. **Tracker names for `0x004c5800` / `0x004c5820` are wrong** (C4 rows named RwTexDictionarySetCurrent / GetCurrent): the slot they address (+0x1c) is read by `FUN_004c6ba0` only as a format flag
   (`0x004c6cb3`), and `FUN_00550130` / `FUN_004cf5a0` save, clear and restore it around a texture read. The dictionary `FUN_004c5cb0` and `0x004c7490` walk is slot +0x10, written by `0x004c5c80` and read by `0x004c5ca0`.
   The C4 evidence is about the bodies and stands; only the names move.
2. **One vehicle dictionary serves every car of a race**: `FUN_00420420` loads it only when `DAT_0063d7e8 == 0`, `FUN_0041ec00` frees it when the live-vehicle counter `DAT_0063e498` is 1. Each car's TXD holds
   `Glass`, `CarGloss`, `<Car>Small`, `<Car>`, so a second, different car would resolve only `Glass` / `CarGloss` in the first car's dictionary. The standalone registers a TXD per model. [UNCERTAIN] whether the
   original ever instantiates different models in one race; needs a multi-car original capture.
3. `0x004c61b0` (default read callback) internals are not decoded. [UNCERTAIN] what path it searches; moot for the 13 tracks (no image entries in any track piz).

## 10. Reproduce

```
py -3.12 verify\txdscope_20261016\make_cites_txdscope.py
pwsh -NoProfile -File verify\txdscope_20261016\run_txdscope.ps1 -Root <private gameroot> -Out log\txdscope     # after arm, control, G-CTRL (base arm: run13.py run with exe_base.exe)
py -3.12 verify\rsky_20261016\run13.py score --out log\txdscope\base  --cite verify\rskyverify_20261016\cites_merged.json --prefix tx_
py -3.12 verify\rsky_20261016\run13.py score --out log\txdscope\after --cite verify\txdscope_20261016\cites_txdscope.json --prefix tx_
py -3.12 verify\rsky_20261016\compare_rows.py log\txdscope\base log\txdscope\after --prefix tx_
py -3.12 verify\txdscope_20261016\compare_draws.py log\txdscope\base log\txdscope\after
py -3.12 verify\txdscope_20261016\dump_delta.py log\txdscope\base log\txdscope\after
py -3.12 verify\txdscope_20261016\scope_census.py
```
