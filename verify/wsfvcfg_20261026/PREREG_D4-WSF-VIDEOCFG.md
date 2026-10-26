# PREREG D4-WSF-VIDEOCFG (2026-10-26): port the original's first-run videocfg.bin default write

Committed alone, before any arm. Answers L-28 (2) (Mariano: port it). Follows `verify/wsfsave_20261026` (P6 RED: the
original writes a 512-byte default on a first run, the standalone writes none).

## What the original does (read 2026-10-26, `decomp_pc.py` on `Mashed_pool10/12`, disassembly `disasm_fn.py` of `MASHED.exe.unpatched`)

Call chain: `FUN_004951f0` calls `FUN_00499400([0x006147bc])`, the boot patch `skip_selector` makes the argument 0.

1. `FUN_00499400` calls `FUN_00498c00` (`0x00499413`). That one, for every subsystem `i`, calls `FUN_004c2e70(i)`
   (driver cmd `0x10`), `FUN_004c2e10(table + i*0x50, i)` (cmd `0xe`), `FUN_004c2ea0()` (cmd `5`, mode count) and
   `FUN_004c2ed0(modes_i + j*0x18, j)` (cmd `6`) for each mode. Tables: `DAT_007731fc` (0x50 per subsystem),
   `DAT_007731f8` (counts), `DAT_00773408` (0x18 per mode).
2. Driver (`FUN_004c7a70`, the RW D3D9 system function) case `0x10`: adapter = the argument (`DAT_007d410c`), then
   `FUN_004c90a0` builds the mode list. Case `0xe` (`0x004c7ffb..0x004c802b`): `IDirect3D9::GetAdapterIdentifier(adapter,
   0, &id)` (vtable `+0x14`), then `rwstrcpy(out, id.Description)` (`[RwGlobals+0xcc]`, source `esp+0x230` = `id+0x200`).
3. `FUN_004c90a0` (mode list, entry 0x14 bytes = `D3DDISPLAYMODE` + flags): entry 0 = `GetAdapterDisplayMode` (vtable
   `+0x20`). If its format is one of `0x15 0x16 0x17 0x18 0x19 0x23`, entry 0 flags = 0 and the count is 1, else the count
   is 0. Then for each format in the 3-dword table at `0x005d8b80` = `0x17` (R5G6B5), `0x16` (X8R8G8B8), `0x23`
   (A2R10G10B10): `GetAdapterModeCount` (`+0x18`), `EnumAdapterModes` (`+0x1c`) into the next slot. When the count is
   above 1, entries `1..count-1` are searched for equal width, height and format. A match keeps the higher refresh rate
   (unsigned compare) and drops the new mode. Otherwise the new entry gets flags 1 and the count grows.
4. Case `6` (mode info, 0x18 bytes): width, height, depth (`0x15 0x16 0x23` -> 32, `0x17 0x18 0x19` -> 16, else 0),
   flags (entry `+0x10`), refresh, format code (`0x15/0x23` -> `0x500`, `0x16` -> `0x600`, `0x17` -> `0x200`,
   `0x18` -> `0xa00`, `0x19` -> `0x100`, else 0).
5. First-run branch of `FUN_00499400` (`param_1 == 0`, `0x004994e3`): `ConfigLoad 0x00498950` (`0x00499555`). When it
   returns 1 the file is not written. When it returns 0: `DAT_0077340c = 0` (`0x00499576`), `DAT_00773200 =
   FUN_00498e40()` (`0x0049957c`), `FUN_00498ea0()` (`0x00499586`), `ConfigSave 0x004989b0` (`0x0049958b`).
6. `FUN_00498e40`: first index whose mode has flags != 0 (`0x00498e63`) and width >= `0x280` (`0x00498e74`) and width
   >= `0x1e0` (`0x00498e7c`, the same register `edx = [ecx]`, so the height is not tested). None -> 0.
7. `FUN_00498ea0` fills the 0x200-byte buffer `0x00773208`: `+0x0` = 5 (`0x00498ecb`); `+0x4` = rwstrcpy of subsystem
   0's name (`0x00498edb`); `+0x104` = `FUN_00498a00`, i.e. `sprintf("%d x %d x %d %s")` (`0x005d0170`) of width, height,
   depth and a tag: `[Window]` (`0x005d01f4`) when flags bit 0 is clear, `[Fullscreen]` (`0x005d0200`) when set, then
   `[Interlaced]` (`0x005d01e4`, bit 1), `[Flicker Free Interlaced]` (`0x005d01c8`, bit 2), `[Fullscreen antialiasing
   mode 0]` (`0x005d01a4`, bit 8), `[Fullscreen antialiasing mode 1]` (`0x005d0180`, bit 9); `+0x184` subsystem
   (`0x00498ebc`), `+0x188` mode (`0x00498ed5`), `+0x18c` / `+0x190` / `+0x194` width / height / depth, `+0x198`
   flags & 1 (`0x00498f21..0x00498f4f`). The rest of the buffer is never written on this path (BSS, and the failed
   `ConfigLoad` read nothing).

## The port (default OFF)

New exe-only TU `Boot/VideoCfgFirstRun.cpp`, knob `MASHED_VIDEOCFG_WRITE=1`, called from `InitD3D9` right after
`Direct3DCreate9` and before `CreateDevice` (the original's order: `FUN_004951f0` runs `FUN_00499400` before the device).
It runs items 3, 4 and 6 against the standalone's own `IDirect3D9` on adapter 0, item 2's name from that same object, and
items 5 and 7 into the exe's private settings buffer through the existing `ConfigLoadCore` / `ConfigSaveCore`
(`Save/SettingsConfig.cpp`, exe-only accessor added). File path: `videocfg.bin` relative to the exe's cwd, as
`ConfigSaveCore` already has it. With the knob unset the function returns before any D3D call. The `.asi` is unchanged.

Departure (stated, not tested): a subsystem name of 80 or more characters overflows the original's 0x50-byte table entry
into the next subsystem's entry. The port reads the name straight from the adapter.

## Arms (private root `D:/MashedWT/D4-WSF-VIDEOCFG_root`, data `D:/MashedWT/D4-WSF-VIDEOCFG_data`, machine LIVING-PC)

- **O1** (one original boot, private root, ASI loader renamed away, no videocfg.bin, no gamesave.bin): the
  `verify/wsfsave_20261026/run_wsf.py W2` recipe -> `O_V.bin` (fresh original first-run videocfg).
- **V1** (branch exe, `MASHED_VIDEOCFG_WRITE=1`, cwd = private root, no `videocfg.bin` in the cwd, 30 s): file written.
- **V2** (same, a `videocfg.bin` = `scripts/canonical/videocfg_windowed.bin` present in the cwd): file after the run.
- **V3** (same, knob unset, no `videocfg.bin`): file after the run.
- **Gates**: `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on the branch exe, knob unset.

## Predictions
- P1 (O1): `O_V.bin` 512 bytes, `+0x0` = 5, equal to the earlier capture `V_0.bin`
  (`cdba8772972fbf72b7c2d4aab142b6c09f02863fc93ccf853447a230f6fee3a7`). A difference here (driver or desktop change since)
  is reported, and P2 then compares against `O_V.bin`.
- P2 (V1): `videocfg.bin` exists, 512 bytes, byte-identical to `O_V.bin` (0 bytes differ).
- P3 (V2): the present file is unchanged (sha256 before == after).
- P4 (V3): no `videocfg.bin` created (as S3 in `wsfsave_20261026`).
- P5 (gates, knob unset): unchanged against `merge_train.json` refs G-CTRL `5943c5f1/b493262b/09551e6a`,
  B `3a03670c/e85ea6fb/53778664`.

A prediction that fails is reported as it came out. A piz-collision crash (U-9677) is a re-run. Flipping the knob default
is a decision row, not this step.
