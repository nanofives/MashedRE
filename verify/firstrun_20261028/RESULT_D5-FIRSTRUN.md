# RESULT D5-FIRSTRUN (2026-10-28): first run with no videocfg.bin / contcfg%d.bin, GREEN, no source change

Branch `orch/D5-FIRSTRUN` off `orch/integration` @ `49a5beb5a`. Standalone-only (LIVING-PC). The original was read
statically (`re/tools/decomp_pc.py`, `re/tools/xref.py`, `re/tools/pointer_callers.py`, capstone over
`original/MASHED.exe.unpatched`, SHA-256 `BDCAE093...` = the anchor). `MASHED.exe` was never launched, no Frida.

## Verdict

The standalone already matches the original's first-run behaviour for both files. No code change.

| File | Original, file absent | `mashed_re.exe` today, file absent | Match |
|---|---|---|---|
| `videocfg.bin` | writes a 512-byte default (`FUN_00499400` first-run branch, see wsfvcfg) | writes a 512-byte default (`Boot/VideoCfgFirstRun.cpp`, `MASHED_VIDEOCFG_WRITE` default ON since L-29 (a)) | yes, with L-31's 3-byte note |
| `contcfg%d.bin` | **writes nothing**. Builds the defaults in memory only | reads nothing, writes nothing. Keyboard / joypad defaults are in memory (`exe_main.cpp` `DeriveInputSlots`, action-4 default `0x1c`) | yes |

So ROADMAP D5's line "First run works with no `videocfg.bin` / `contcfg%d.bin` present (defaults are written)" holds,
with one correction to its wording: for `contcfg%d.bin` the original's defaults are **built in memory, not written**.
The original writes a `contcfg%d.bin` only after the player confirms a per-slot dialog (below). This agrees with L-28's
earlier line "contcfg*.bin: neither writes, GREEN" (`verify/wsfsave_20261026`), now with the RVAs.

## The original's contcfg%d.bin path (static, cited)

- **Only loader** `FUN_004971b0` (only caller `FUN_00498510`, `xref.py`; no stored pointer, `pointer_callers.py`):
  `_fsopen(name, "rb")` (`0x004971d0`, mode `0x005cf010`), on success `_fread` 0x200 bytes into `0x007e95c0 + slot*0x200`
  and return 1, else log `"\tUnable to load controller config %s\n"` (`0x005d001c`) and return 0. Name from `FUN_00497190`,
  format `"contcfg%d.bin"` at `0x005d000c` into `0x007730e4`.
- **Only saver** `FUN_00497230` (only caller `FUN_004982a0`, no stored pointer): `_fwrite` 0x200 bytes from the same slot.
- **Defaults builder** `FUN_00498510(param_1, lang)` (only caller `FUN_00495150`, which passes `[0x006147c0]`):
  zero-fills `0x007e95c0` for 0x800 bytes (`FUN_004b6520`), then per slot 0..3 (bound `0x7e9ec8`): `+0x0 = 6`; slot `i <`
  joypad count (`FUN_00495790`) gets type 1 (`+0x13c`), joypad index `i` (`+0x140`), the device name, bindings 0..8 =
  `0,1,2,3,0,1,4,2,5`; slot `i ==` joypad count gets type 2 (keyboard), the `LoadStringA` name at `0x007730b4`, bindings
  0..12 = `0x1f,0x2d,0x1e,0x20,0x1c,1,1,0x1e,0x2e,0xcb,0xcd,0xc8,0xd0`; other slots type 0.
  Then per slot it saves the default record, calls `FUN_004971b0`, and **keeps the loaded record only when** its `+0x0`
  equals the default's (6) **and** its name string at `+0x4` equals the default's (byte compare loop `0x00498720..0x00498749`).
  Otherwise the default record is copied back (`rep movsd` `0x0049874d..0x00498758`). With no file the load returns 0 and
  the default stays. **No write in this function.**
- **The only write path**: `FUN_00498510` calls `DialogBoxParamA(hinst, 0x67, ..., FUN_00498360, 0)` only when
  `param_1 != 0`. `FUN_00498360` (WM_COMMAND `0x111`) sends ids `0x3ef / 0x3f0 / 0x3f1 / 0x3f2` to `FUN_004982a0(0..3)`.
  `FUN_004982a0` returns 1 at once for a type-0 slot (`[0x007e96fc + slot*0x200] == 0`), else opens dialog `0x68`
  (`FUN_00497fd0`) and calls `FUN_00497230` **only when that dialog returns 1**. Return 0 restores the slot's saved copy.
- **The dialog gate** `[0x006147c0]`: `.data` initial value `01 00 00 00` (file bytes of the anchored image). Written only by
  the command-line parser `FUN_00493900` (`__stricmp`): `-cs0` (`0x005cf8bc`) stores EBX = 0 (`0x004939ae`), `-cs1`
  (`0x005cf8b4`) stores EDI = 1 (`0x004939ce`). EBX = 0 at `0x00493931`, EDI = 1 at `0x0049393d`. The `launch.exe`
  equivalent passes `-CS<0|1>` (`re/prior_art/MashedRunner/Mashed.cs:56`). The dev boot patch `skip_controller_dialog`
  replaces the read at `0x004951aa` with `xor eax,eax`.

So on a first run the original writes no `contcfg%d.bin` unless the player opens a slot's dialog and confirms it.

## The standalone today (survey)

- `contcfg`: no `exe_sources.rsp` TU opens it. `Input/DInput.cpp` (calls the original's `FUN_00497190` formatter) and
  `Harness/HarnessStubs.cpp` are in `asi_sources.rsp` only. `exe_main.cpp:1078` `DeriveInputSlots` rebuilds
  `FUN_00498510`'s slot types from `EnumDevices` and states "no contcfg<i>.bin override". `exe_main.cpp:2781` ports the
  type-2 action-4 default `0x1c` (`0x00498698`).
- `videocfg`: `exe_main.cpp:2919` calls `VideoCfgFirstRunWrite` between `Direct3DCreate9` and `CreateDevice`.
  `ConfigLoadCore` (0x00498950) is the presence test, `ConfigSaveCore` (0x004989b0) the write, `videocfg.bin` in the cwd.
  The exe never uses the file's contents (`Save/SettingsConfig.cpp:22`, the window is fixed).
- `exe_main.cpp:134` (the ledger's "mentions videocfg.bin only in a comment") is a stale header comment, not the read path.

## Arms (`verify/firstrun_20261028/run_fr.py`, private root `D:/MashedWT/D5-FIRSTRUN_root`, cwd = root, 20 s boot)

Exe `mashedmod/build/mashed_re.exe` sha `edab968a...`. Before each FR1-type arm the scratch root had no `videocfg.bin`
(root and `root/original/`) and no `contcfg*.bin` anywhere. Shared `original\` untouched.

| Arm | Env | Alive 20 s | `videocfg.bin` | `contcfg*.bin` created | Port log |
|---|---|---|---|---|---|
| FR1 | default, no `MASHED_*` | yes | written, 512 B, `7f130171...` | none | `first-run write ret=1 modes=49 pick=1 ... '640 x 480 x 32 [Fullscreen]'` |
| FR2 | default, file from FR1 present | yes | unchanged (sha and mtime) | none | `videocfg.bin present, no write` |
| FR1b | default (repeat) | yes | `7f130171...` | none | same as FR1 |
| FR1G / FR1Gb | gate `base_env` (run_arms.py, 1200 frames) | yes / yes | `7f130171...` / `7f130171...` | none | same as FR1 |
| B_DEF_K0 via `run_arms.py` (fresh file) | gate recipe | self-exit 0 | `7f130171...` | none | same as FR1 |

Written file vs the original's first-run file `O_V.bin` (`cdba8772...`, wsfvcfg O1): **3 bytes differ** at `0x110`,
`0x111` (label `"x 32"` vs `"x 16"`) and `0x194` (depth 32 vs 16). This is exactly L-31's recorded difference: no
`DWM8And16BitMitigation` layer on `mashed_re.exe`, decided (a) "leave it" (Mariano, 2026-10-26). Mode count here 49
(L-31 recorded 53), which is not stored in the file.

[UNCERTAIN] After the first gate pass (H_DEF, B_DEF_K0/K1/K26, root freshly copied 03:46) the root held a
`videocfg.bin` with sha `cdba8772...`, i.e. the original's x16 file. The writing run is not identified: `ROOT/log/` did
not exist during those runs, so `VideoCfgFirstRun.cpp`'s `Log()` (`fopen("log/mashed_re.log")`) dropped its line, and
FR1 deleted the file. Five later controlled first-run boots (table above) all wrote `7f130171...`. Missing evidence: a
`[videocfg]` log line from the run that wrote `cdba8772`. It does not touch any gate hash (the file is not a gate input).

## Gates (this exe, `verify/savelink_20261023/run_arms.py`, root `D:/MashedWT/D5-FIRSTRUN_root`)

G-CTRL H_DEF `5943c5f1 / 364350ae / 52052890`, B `3a03670c / 9fcdbd3a / 53778664`. Both equal the `gctrl.ref` /
`b.ref` in `re/orchestrator/merge_train.json` at `orch/integration` (B K1 ref `9fcdbd3a` since `a4c3d3389`, L-43).
Unchanged, as expected for no source change. Re-run B_DEF_K0 `3a03670c`.

## Why no knob / PREREG

No behaviour change was made. The original-faithful first-run default for `videocfg.bin` already ships (L-29 (a)), and
for `contcfg%d.bin` the faithful first-run behaviour is "write nothing", which is what the standalone does.

## Out of scope, noted for the D5 / input owners

- The standalone ignores a **present** `contcfg%d.bin` (no `FUN_004971b0` load, no `+0x0 == 6` / name check). A player
  file written by the original's dialog is not honoured. That is a loader port, not a first-run default.
- The standalone has no controller-setup dialog (`0x67` / `0x68`, the original's default `-cs1` path), so it never writes
  `contcfg%d.bin` at all. Whether v1.0 wants that dialog is a scope call for the D5 checklist, not this step.
- The keyboard default binding table above (13 DIK codes, `FUN_00498510`) vs the standalone's own keyboard map was not
  compared here; only action 4 (`0x1c`) is cited as ported.

## Files

Committed: `run_fr.py`, `run_fr_log.txt` (= raw `run_fr.log`), `gates_hashes.txt`, `decomp_contcfg_extract.txt` (decomp of `0x00498510`,
`0x004982a0`, `0x00498360`, `0x00495150`, `0x00499710`, `0x00499720`, read-only pool slot `Mashed_pool0`).

Raw, off git (`D:/MashedWT/D5-FIRSTRUN_data/`), SHA-256:

| file | sha256 |
|---|---|
| `decomp_contcfg.txt` | `27d19a3f73cf93e1029398e411191af8560916f9099c8bc4c4b39ec867eb4a17` |
| `fr.log` | `4c22526a7302f00f66ff58e91e5f18b48d2344d75c375fe0c06e315eff218bab` |
| `run_fr.log` | `ff5ab085d08cf56ec8a121a4a09254c7fc64d33d0a65ae4a03a9c5069b71a560` |
| `FR1_videocfg.bin` (= FR1b, FR1G, FR1Gb) | `7f1301713a118a86372d06b89d503334ddeb6619b7f23088e09ed453d12bab88` |
| `FR_root_log_mashed_re.log` | `e1c48a97e8362b5a492be979dd157d153df4b0fe99c5685fb3d73b9a61ed7d3a` |
| `gates.log` | `d22e21a9180885e3dec8709ac50a8cb3259909c8320e8474dc212c43a4c2801d` |
| `gates_vc.log` | `ef208d7e85a81f12d36db9c3a5875731b5f67fddf57b3da0af41e4c7dcd02a74` |
| `arms/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `arms/H_DEF.gates.csv` | `364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4` |
| `arms/H_DEF.ptrace.log` | `520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757` |
| `arms/B_DEF_K0.step.csv` (= `arms_vc/`) | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `arms/B_DEF_K1.step.csv` | `9fcdbd3a6901025544e377132c575910aa5cd001faba802c1e7e8a9ac0d2886f` |
| `arms/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |
| `arms/RUNLOG.txt` | `57bb3445bddb631d4419116109e6deb3416602c7acae3760da88b7827fc32e43` |
| `mashed_re.exe` (worktree build) | `edab968af20ecede944825bc3629021ea74138cbf82e9d178c25f3bd02a6c1ea` |

Trackers not touched (no level change: the evidence is static plus exe-path, no hook diff).
