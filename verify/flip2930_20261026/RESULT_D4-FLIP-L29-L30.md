# RESULT D4-FLIP-L29-L30 (2026-10-26): MASHED_VIDEOCFG_WRITE and MASHED_AI_VEHICLES default ON

L-29 (a) and L-30 (a), Mariano. Branch from `orch/integration` @ `338aa6ac8`, which contains D4-WSF-VIDEOCFG `6e3d44674`
and the D3-AI-VEHICLES merge `e16c1561d`. Exe sha8 `2112aaba`. Machine LIVING-PC. Private root
`D:/MashedWT/D4-FLIP-L29-L30_root`. `original/` untouched, no `.asi` deployed.

## Verdict: DECISION (flip GREEN, one finding for Mariano)

Both knobs are default ON, `=0` opts out, and the six default gates equal the refs with the knobs ON and with both opted
out. On the gate arms AI_VEHICLES is never reached. The videocfg write fires and is gate-neutral. **Finding:** the
written file matches the original's only when the exe runs under the `DWM8And16BitMitigation` compat layer. Every
original `MASHED.exe` path on this machine carries that layer, and the standalone does not. Without it the standalone's
D3D9 lists no 16-bit modes and the default becomes `640 x 480 x 32` (3 bytes differ).

## Change
- `Race/RaceSession.cpp` `AiVehiclesOn()`: `!(e && e[0] == '0')` (was `e && e[0] == '1'`). Adds a `once()` line
  `[D3-AI-VEHICLES] applied ...` when the branch runs, so a run shows whether the knob did anything.
- `Boot/VideoCfgFirstRun.cpp`: `!(e && e[0] == '0')` (was set and not `0`).
- Comments stating "default OFF" updated in those two files, `Save/SettingsConfig.cpp` and `exe_main.cpp`. No other
  source, script or doc names either knob outside `verify/`.

## Gates (`run_gates.py` = `verify/savelink_20261023/run_arms.py` unchanged plus stack `OPT0` = both knobs `=0`)
| arm set | G-CTRL | B |
|---|---|---|
| `H_DEF B_DEF_K*` (new defaults) | `5943c5f1 / b493262b / 09551e6a` | `3a03670c / e85ea6fb / 53778664` |
| `H_OPT0 B_OPT0_K*` (both `=0`) | `5943c5f1 / b493262b / 09551e6a` | `3a03670c / e85ea6fb / 53778664` |
| `H_DEF B_DEF_K*` re-run with `log/` present (`run_gates_logged.py`) | same | same |

All equal `merge_train.json` refs.

## What the knobs do on the gate arms (measured, `run_gates_logged.py`, race log per arm)
- **AI_VEHICLES:** `[D3-AI-VEHICLES] applied` 0 times and `RaceSession::Begin` 0 times in all four arms. These arms
  (`MASHED_CAR=1`, Training) never call `RaceSession::Begin`, so the knob cannot act on them. That matches the
  structural note in `verify/aiveh_20261026/RESULT_D3-AI-VEHICLES.md`. The knob's effect was shown there on the
  menu-launched race, not here.
- **VIDEOCFG_WRITE:** the root has no `videocfg.bin` in its cwd, so the first arm (H_DEF) writes one
  (`first-run write ret=1 modes=53 pick=1 ... '640 x 480 x 32 [Fullscreen]'`). The three B arms log
  `videocfg.bin present, no write`. The file is not a gate input. Hashes are unchanged.
- The first gate pass had no `ROOT/log` folder, so both modules' log lines were dropped silently. Its hashes stand. The
  logged re-run is the measurement.

## videocfg on the new default (`run_vcfg.py`, a copy of the wsfvcfg driver with V1/V2 knob unset, V3 `=0`)
| arm | result |
|---|---|
| O2 original first run, this root (`run_wsf.py W2`) | `cdba8772...`, the same file as wsfvcfg's `O_V.bin` / `V_0.bin` |
| V1 default, no file | 512 bytes, `7f130171...`, **3 bytes differ** vs `O_V.bin`: `+0x110/+0x111` "32" vs "16", `+0x194` 32 vs 16. Re-run gives the same |
| V1 default, no file, `__COMPAT_LAYER=DWM8And16BitMitigation` | `cdba8772...`, **0 bytes differ**, log `modes=79` (vs 53) |
| V2 default, canonical file present | unchanged, `f87df3c8...` before and after |
| V3 `=0`, no file | no file written |

The port's code is unchanged since wsfvcfg. Only the D3D9 mode list differs: under the layer `GetAdapterModeCount` reports
R5G6B5 modes, which come first in the original's format table `0x005d8b80` (`0x17, 0x16, 0x23`), so mode 1 is
16-bit. Without the layer there are none, so mode 1 is 32-bit. The original's paths are registered with the layer in
HKCU/HKLM `AppCompatFlags\Layers` (by `scripts/setup_mashed_compat.ps1` and the PCA). No `mashed_re.exe` path is.
[UNCERTAIN] Why wsfvcfg's V1 (`D4-WSF-VIDEOCFG` worktree exe) saw 79 modes without a layer entry: no registry entry
for that path is present now, and the worktree has been removed. Evidence missing: the layer state of that process.

## Decision for Mariano
The standalone writes the original's videocfg.bin only when it runs under `DWM8And16BitMitigation`, as the patched
original always does here. (a) Leave it: the file follows the process's own D3D9 view (32-bit default on a bare
Win11). (b) Make the standalone see the same mode list as the original, for example by applying the layer to itself.
That is new behaviour, which may affect more than this file. Recommendation (a): the file is never read by the
standalone, and (b) touches the device. Either way this flip can merge, since gates are unchanged.

## Raw evidence (off git, `D:/MashedWT/D4-FLIP-L29-L30_data/`), SHA-256
| file | sha256 |
|---|---|
| `G_videocfg_after_gates.bin` (default, no layer) | `7f1301713a118a86372d06b89d503334ddeb6619b7f23088e09ed453d12bab88` |
| `V1.bin` (last V1, under the layer) | `cdba8772972fbf72b7c2d4aab142b6c09f02863fc93ccf853447a230f6fee3a7` |
| `V_0.bin` (O2 original first run) | `cdba8772972fbf72b7c2d4aab142b6c09f02863fc93ccf853447a230f6fee3a7` |
| `O_0.bin` (O2 gamesave side product) | `95dd10e73553a6897b4b428d234088f24ef3104eb9b35dbdd5ba1f51ff57bf60` |
| `run_all.log` (gates DEF + OPT0, V1-V3) | `024f3572440a186cd9e635101977f233efb9556f30df47c6537998a59ab58393` |
| `gates_logged.log` | `a19e116da29411bae98701b2f6163ab8e8320b119799ae07792fa2bc5d3976a9` |
| `run_vcfg.log` | `1b2e40c552720d9e3fba11b397a19a071fb189b2a384daa9ad6fc8644b850a12` |
| `V1_rerun.log` / `V1_compat.log` | `8668e7c00631a539243009ff86c14690e66fb1daf741d597f91c66446f0596ee` / `f63dc09a61be5156266a515b58ee0b6bd71d47409e9f36d0464099f046f6049c` |
| `O2_console.log` / `run_wsf.log` | `d7be22a6021e6dd5dbc0a9805f5f06cd2474e368cd84fc1ca28e727a84b19281` / `5fdedbd4d183d3b50092b109d5880f2d092e24a3015aa1cecb072d2a99857ce1` |
| `build.log` | `a9b701a39b4c34c4138b6e29c7b00c7fcce7834920532044a22f3fa042d60069` |
| `arms/H_DEF.step.csv` = `arms/H_OPT0.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `arms/H_DEF.gates.csv` = `arms/H_OPT0.gates.csv` | `b493262bed8ea6ecad4399f0ebaa53b902965423e74bfa07359188559fff479f` |
| `arms/H_DEF.ptrace.log` = `arms/H_OPT0.ptrace.log` | `09551e6a5b1b456b3421a0e1aac768ced7851c072c99a9341a22ca966cdfe3e1` |
| `arms/B_DEF_K0.step.csv` = `B_OPT0_K0` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `arms/B_DEF_K1.step.csv` = `B_OPT0_K1` | `e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8` |
| `arms/B_DEF_K26.step.csv` = `B_OPT0_K26` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |
| `arms_logged/H_DEF.racelog` | `54057bad43814916c9a1e48e93ba9d4efc623ac1536446faebd5918d4e8a5d74` |
| `arms_logged/B_DEF_K0.racelog` = `B_DEF_K1.racelog` | `bb840c88459996e17fa7028b2e50fb6295ad9d55388c21895496027395228fa2` |
| `arms_logged/B_DEF_K26.racelog` | `a4d45186819f481de14cf78cc223134dfe02bc4e260f2cf1c834c47eb2ce66aa` |
