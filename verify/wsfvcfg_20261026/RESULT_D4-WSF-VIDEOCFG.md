# RESULT D4-WSF-VIDEOCFG (2026-10-26): first-run videocfg.bin default write, ported (default OFF)

PREREG `PREREG_D4-WSF-VIDEOCFG.md` (commit `8763e5c68`, alone, before any arm). Port commit `56462bfc9`. Drivers: O1 =
`verify/wsfsave_20261026/run_wsf.py W2` (`WSF_ROOT` / `WSF_DATA` set to this step's), V1-V3 = `run_vcfg.py`, gates =
`verify/savelink_20261023/run_arms.py`. Branch exe sha8 `51677949`. Machine LIVING-PC. Private root
`D:/MashedWT/D4-WSF-VIDEOCFG_root` (robocopy of `original/`, ASI loader renamed to `dinput8.dll.off` for O1). `original/`
untouched, no `.asi` deployed.

## Verdict: GREEN

With `MASHED_VIDEOCFG_WRITE=1` and no `videocfg.bin` in its cwd, the standalone writes a 512-byte `videocfg.bin` that is
byte-identical to the original's first-run file on the same machine (0 of 512 bytes differ). A present file is not
touched. With the knob unset nothing is written and the default gates are unchanged. Flipping the default is a decision.

| pred | arm | result |
|---|---|---|
| P1 | O1 original first run | **held**: `O_V.bin` 512 bytes, sha `cdba8772...` = the earlier `V_0.bin` |
| P2 | V1 knob ON, no file | **held**: `videocfg.bin` 512 bytes, sha `cdba8772...`, 0 bytes differ vs `O_V.bin` |
| P3 | V2 knob ON, canonical file present | **held**: sha `f87df3c8...` before and after, log `videocfg.bin present, no write` |
| P4 | V3 knob unset, no file | **held**: no `videocfg.bin` |
| P5 | gates, knob unset | **held**: G-CTRL `5943c5f1 / b493262b / 09551e6a`, B `3a03670c / e85ea6fb / 53778664` (= `merge_train.json` refs) |

Written file (V1 = O1): `+0x0` 5; `+0x4` "NVIDIA GeForce RTX 3070"; `+0x104` "640 x 480 x 16 [Fullscreen]"; `+0x184..+0x198`
0 / 1 / 640 / 480 / 16 / 1; every other byte zero. The port's log line: `modes=79 pick=1`.

## The original's writer (citations)

- `FUN_004951f0` -> `FUN_00499400([0x006147bc])`, 0 under `skip_selector`. First-run branch `0x004994e3`: `ConfigLoad
  0x00498950` (`0x00499555`); on 0: `DAT_0077340c = 0` (`0x00499576`), `DAT_00773200 = FUN_00498e40()` (`0x0049957c`),
  `FUN_00498ea0` (`0x00499586`), `ConfigSave 0x004989b0` (`0x0049958b`). On 1 no write.
- Tables from `FUN_00498c00` (`0x00499413`): per subsystem, RW driver `FUN_004c7a70` cases `0x10` (adapter, mode list
  `FUN_004c90a0`), `0xe` (`0x004c7ffb..0x004c802b`: `GetAdapterIdentifier(adapter, 0, &id)` vtable `+0x14`, rwstrcpy of
  `id.Description`), `5` (count), `6` (mode info: depth 32 for `0x15/0x16/0x23`, 16 for `0x17/0x18/0x19`).
- `FUN_004c90a0`: entry 0 = `GetAdapterDisplayMode` (`+0x20`), flags 0, kept when its format is one of
  `0x15..0x19, 0x23`. Then formats `0x005d8b80` = `0x17, 0x16, 0x23`, `GetAdapterModeCount` (`+0x18`) /
  `EnumAdapterModes` (`+0x1c`), duplicates on width + height + format from entry 1 keep the higher refresh, new entries
  flags 1.
- `FUN_00498e40`: first mode with flags != 0 (`0x00498e63`), width >= `0x280` (`0x00498e74`), width >= `0x1e0`
  (`0x00498e7c`, the same `edx = [ecx]`, height never tested). None -> 0.
- `FUN_00498ea0`: `+0x0` = 5 (`0x00498ecb`), `+0x4` name (`0x00498edb`, `[RwGlobals+0xcc]`), `+0x104` `FUN_00498a00`
  (`0x00498eff`, EBX = the mode, `0x00498ef7`; `"%d x %d x %d %s"` `0x005d0170` and the tags `0x005d01f4 / 0x005d0200 /
  0x005d01e4 / 0x005d01c8 / 0x005d01a4 / 0x005d0180`), `+0x184` / `+0x188` subsystem / mode (`0x00498ebc` / `0x00498ed5`),
  `+0x18c..+0x198` width / height / depth / flags & 1 (`0x00498f21..0x00498f4f`).

## What changed in source (default OFF)
- New exe-only `Boot/VideoCfgFirstRun.cpp` (`exe_sources.rsp` only): the chain above on the exe's own `IDirect3D9`,
  adapter 0, knob `MASHED_VIDEOCFG_WRITE=1`. Returns before any D3D call when unset.
- `exe_main.cpp`: `InitD3D9` calls it after `Direct3DCreate9`, before `CreateDevice` (the original's order).
- `Save/SettingsConfig.cpp`: exe-only accessor `ConfigSettingsBuf()` to the private 0x200-byte buffer, header note.
  No change to the save-progress code (`Race/GameFlow.cpp`), disjoint from D4-WSF-SAVE-FLIP.
- The `.asi` does not compile the new TU and its SettingsConfig code is unchanged.

## Notes
- Path: `videocfg.bin` relative to the exe's cwd, which `ConfigSaveCore` already used. Under `MASHED_ROOT` that is the
  root next to `original/`, not `original/videocfg.bin` (same rule as `mashed_re_gamesave.bin`). The canonical
  `original/videocfg.bin` is never written.
- Departure, untested: an adapter name of 80+ characters overflows the original's 0x50-byte subsystem entry. The port reads
  the name from the adapter directly.
- Trackers not touched. `FUN_00498e40` / `FUN_00498ea0` / `FUN_00498a00` / `FUN_004c90a0` stay at their levels: the
  file match is exe-path evidence, not a hook diff.

## Decision for Mariano
Flip `MASHED_VIDEOCFG_WRITE` default ON? (a) flip: a first run with no `videocfg.bin` in the cwd writes the original's
default file, 0 bytes differ on LIVING-PC. The gate roots have no `videocfg.bin` in their cwd, so after a flip every gate
run writes one there (a new file, not a gate hash input). The flip's train should confirm the hashes. (b) keep OFF and
exempt the write in the D5 checklist.
Recommendation (a), via the decided-flip protocol, since L-28 (2) chose the port.

## Raw evidence (off git, `D:/MashedWT/D4-WSF-VIDEOCFG_data/`), SHA-256
| file | sha256 |
|---|---|
| `O_V.bin` (O1 original first-run videocfg) | `cdba8772972fbf72b7c2d4aab142b6c09f02863fc93ccf853447a230f6fee3a7` |
| `V1.bin` (standalone knob ON) | `cdba8772972fbf72b7c2d4aab142b6c09f02863fc93ccf853447a230f6fee3a7` |
| `O1_gamesave.bin` (O1 side product) | `95dd10e73553a6897b4b428d234088f24ef3104eb9b35dbdd5ba1f51ff57bf60` |
| `W2_probe.log` | `029d75acafd9dd0f80267ab20c735db4e0f5c8563ca9301b62f7d38ae7582a72` |
| `run_wsf.log` | `590f70616a14edc1235c7389f60187f0e2c97036701c17e92bf5bb4ebc627be0` |
| `run_vcfg.log` | `886b3f5101bc86c895a05b8e4eb350210e5c938ae1db271ebe3b6fd9dd90e9b0` |
| `V_mashed_re.log` (port log lines) | `34a096da52e8f32dbcfc96c84585ff486c6c5447f8853d70482a32cdc87b08dc` |
| `gates.log` | `65ab03e0fa7d6d4fa086c20bd6d398cc7bbaa7fe89dee642a48f1889662cae55` |
| `arms/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `arms/H_DEF.gates.csv` | `b493262bed8ea6ecad4399f0ebaa53b902965423e74bfa07359188559fff479f` |
| `arms/H_DEF.ptrace.log` | `09551e6a5b1b456b3421a0e1aac768ced7851c072c99a9341a22ca966cdfe3e1` |
| `arms/B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `arms/B_DEF_K1.step.csv` | `e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8` |
| `arms/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |
| `decomp1..4.txt`, `dis_*.txt` (decomp / disassembly reads) | listed in the data folder |

The first gate attempt died before any arm on a missing output folder (`gates_try1_nooutdir.log`). The re-run is the
reported one.

## L-31 answer (Mariano, 2026-10-26)

(a): leave the videocfg mode list as it is. The 3-byte difference in the standalone's first-run `videocfg.bin` (default mode 640x480x32 instead of x16, 53 modes instead of 79) comes from the absent `DWM8And16BitMitigation` compat layer, which every original `MASHED.exe` path has and `mashed_re.exe` lacks. It is not a port difference. Under that layer the write is 0 of 512 bytes different.
