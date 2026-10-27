# RESULT D4-FU-SPANINIT-WRITERS (2026-10-27): first-run save writers + boot span seed, default-OFF knobs

PREREG `PREREG_D4-FU-SPANINIT-WRITERS.md` (commit `a2963a909`, alone, after the code commit `4f61c31a1` and before
any arm). Driver `run_spw.py`. Base `orch/integration` @ `1158f5c1c`. Exe sha8 `dc0f596d`. Machine LIVING-PC.

## Verdict: GREEN (knobs OFF by default, no flip)

P1..P5 held (8 of 8 checks), P6 held (gates unchanged). With `MASHED_SAVE_FIRSTRUN_WRITERS=1` the `0x24efc` byte (`0x00409e30`)
closes. The gather byte closes for every input slot the port derives as active. On this run the machine had **0 game
controllers attached**, so only slot 0 (keyboard) was active, and the first-run save differs from the original's `O_0`
by **15** bytes (17 before). `O_0` was captured with 3 active slots (2 controllers). The 2 bytes left in the gather
bucket (slots 1 and 2) come from that hardware difference, not from the model: P2b holds against the pre-registered
formula `13 + m`, with `m` = 2. With 2 controllers attached the formula gives 13. That configuration was not run here.
`MASHED_SPAN_BOOT_SEED=1` seeds the live span at boot. The dump matches `O_0`'s span except at the 4 bytes the PREREG
named.

## Arms
| pred | arm | result |
|---|---|---|
| P1 | `N_0`, knob off, first run | **held**: == D4-FU-SETTRACKPU `N_0` byte for byte (sha `604d373f`) |
| P2a | `K_0`, knob on, first run | **held**: vs `N_0` differs at `0x24efc` (0 -> 1) and `0x24f54` (1 -> 0) only. Logged slots: `game controllers 0, types 2 0 0 0, active 1`, so A = {0} |
| P2b | `K_0` vs `O_0` | **held**: 15 differ (expected 13 + 2). Col 4 rows 1..12 = 12, gate `0x24f2c` = 1, gather `0x24f55` / `0x24f56` = 2 (`O_0` 0, slots inactive here). `0x24efc` and `0x24f54` now match |
| P3 | `K_P` / `K_G` / `K_1`, knob on, loaded files | **held**: output == input byte for byte, all three |
| P4 | `K_P0`, knob on, `MASHED_SAVE_KEEP_IMAGE=0`, `F_P` | **held**: vs SETTRACKPU `N_P0` differs only at `0x24f54`. vs `F_P`: 2 differ (`0x24aa4`, `0x24f2c`), was 3 |
| P5 | `S`, `MASHED_SPAN_BOOT_SEED=1`, absent save, boot dump | **held**: dump vs `O_0` span: 4 differ at span `+0x4bc`, `+0x514`, `+0x515`, `+0x516`, as predicted. Log: `R2-2 gamesave load: blank/absent -> fresh defaults`, `span boot seed: FUN_004924f0 + powerups_all.lua` |
| P6 | GATES, no env | **held**: G-CTRL `5943c5f1 / 364350ae / 52052890`, B `3a03670c / e85ea6fb / 53778664` (= `merge_train.json` refs), all four self-exited, exit 0 |

## What the knobs do (source)

- `Race/GameFlow.cpp`, `MASHED_SAVE_FIRSTRUN_WRITERS=1`:
  - `Campaign_LoadProgress`: no save file (`nRead == 0`) -> `g_saveCounter = 1`. That is `0x00409e30`
    `MOV [0x008a95ac],1` (case 16 of switch `0x00409b00`), which runs after the case-14 creation write `0x00409db8`.
    Serialize copies the counter to file `0x24efc` (`0x00404f03` / `0x00404f23`).
  - `SaveProgress` rebuild path: span `+0x514 + i` = 0 for each slot with a derived type != 0. That is `FUN_004967e0`
    -> `FUN_00496530` (`0x00496820`, gated by `FUN_00497450(i)` = `[0x007e96fc + i*0x200] != 0`), `rep stosd`
    `0x00496568` over record `0x7f1038 + i*0x4c`. Serialize gathers record `+0x24` (`0x7f105c + i*0x4c`) into
    `0x7f0f54 + i` (`0x00404ef0` / `0x00404ef2`, 12 records to `0x7f13ec`). Read on `MASHED.exe.unpatched` with
    `re/tools/disasm_fn.py`.
  - The save round-trip harness runs before `WinMain`. With the knob on it derives the slots window-less
    (`ExeInput_DeriveSlotsHeadless` -> `DeriveInputSlots`, `exe_main.cpp`).
- `exe_main.cpp` boot save restore, `MASHED_SPAN_BOOT_SEED=1`: no save restored -> `GameFlow_BootSpan` (`InitSpanDefaults`
  + `POWERUPS_ALL.LUA`) -> `Nav_GameStateSeedSpan` (`Frontend/MenuNavSM.cpp`). That function shares the span copy (private
  model + live `0x007f0a40`) and the menu-gate re-derive with `Nav_GameStateLoadSave`, now factored into
  `CopySpanAndRederive`. `MASHED_SPAN_BOOT_SEED_DUMP=<path>` is arm-test only.
- Knobs off: no stored value changes. P1 and P6 cover this.

## Residual on the first-run save (knob on, this machine)
| bucket | bytes | cause |
|---|---|---|
| col 4 rows 1..12 | 12 | port-private progression (`BuildSpanFromProgress` unlock 2 / 0), unchanged |
| gate `0x24f2c` | 1 | port-private `any ? 1 : 0`, unchanged |
| gather `0x24f55..56` | 2 | slots 1..2 inactive here (0 controllers). `O_0` was captured with them active. Hardware, not model |

## Not claimed / `[UNCERTAIN]`
- No 2-controller run of `K_0`, so the 13-byte case is predicted, not measured.
- (a) covers a missing file only. Not decoded: what the state machine does with an invalid file or a loaded file
  before state 16. Missing evidence: a decode of `0x0040aa60` + byte table `0x0040aab4`, or a write watch on
  `0x008a9588` over a load boot.
- The first-run creation write (`0x00409db8`, counter 0) is not ported. The port writes no file at boot.
- The live seed does not model the per-frame record clear (gather bytes in the live span stay at the boot value 1).
  That is the predicted P5 difference.
- No tracker change, re-classify not run. No status moves for `0x00409e30` / `0x00496530` / `0x004967e0` / `0x0047b280`.

## Next
1. Decision row if wanted: flip `MASHED_SAVE_FIRSTRUN_WRITERS` and/or `MASHED_SPAN_BOOT_SEED` (gates unchanged with both off).
2. Optional 2-controller `K_0` run to measure the 13-byte case.
3. Port the first-run creation write (`0x00409db8`) and decode the state 1 -> 12 path.

## Raw evidence (off git, `D:/MashedWT/D4-FU-SPANINIT-WRITERS_data/`), SHA-256
| file | sha256 |
|---|---|
| `N_0.bin` | `604d373fc3b60fdcaf5903257ab70d62cf32611fdbadcd42226adf8d74a718dd` |
| `K_0.bin` | `f003d0db7bbd0760a46439239cc25ea777e1921843a7d75f64a9636dbe7d6ecd` |
| `K_P.bin` (= `F_P`) | `697b7af8175b418f24fcd9da81851b8fa42b4d236ec8fb6b90d110d48e53f078` |
| `K_G.bin` (= `F_G`) | `e2eabeacda9820bac9b2937bbc055b6abbd72f88b32eefecabd966be7ab289e9` |
| `K_1.bin` (= `F_1`) | `dca746c9d4339c13434daeda4d56077119441652a59285319400159cfeda35b8` |
| `K_P0.bin` | `e5fd7aa5af97b7c3d6c9c0e289fe2bd325fc68db5fa6e052d24548c6e4caaa0a` |
| `S_span.bin` | `7a917e737eca2cf46d022abbc558b145b49f41ad483a8ee6d372555b217fc107` |
| `S_log_extract.txt` | `34bd1af8a00a0054521eecf9b5d05ca6c8024ed43ab6d8cd4e04c9a232424d8a` |
| `run_console.log` (N S CMP) | `9c5bc9ae768ee40885576321d03bf4739ad94dce5a1e0d0f1b2af50da207cffd` |
| `build.log` | `86389a25000a94daa6f25b69598813ee6595987e2b00ebf039728422913d22e6` |
| `gates.log` | `dd8c7647b719085b51a015494dd6855c83992844ffcd88d36d902fcd4cb8a724` |
| `run_console_gates.log` | `a46ea3c1f872b28d2e8f87f166d6c07c5bf546c46508349ab07a08a5e3cb8b55` |
| `run_spw.log` | `6841f0b20cf8879ad2b1c6059056cfe5cc7d16ee8cfdf54c482e559005e42564` |
| `arms/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `arms/H_DEF.gates.csv` | `364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4` |
| `arms/H_DEF.ptrace.log` | `520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757` |
| `arms/B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `arms/B_DEF_K1.step.csv` | `e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8` |
| `arms/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |
| `mashedmod/build/mashed_re.exe` (branch build) | `dc0f596d1bcf2dd4a3615024b4b096968a4085009ef153f14e8e0cca6d3b05eb` |

Inputs: `O_0` `D:/MashedWT/D4-WSF-SAVE-RT_data/O_0.bin`, SETTRACKPU `N_0` / `N_P0` `D:/MashedWT/D4-FU-SETTRACKPU_data/`.
Private root `D:/MashedWT/D4-FU-SPANINIT-WRITERS_root` (robocopy of `original/`, `dinput8.dll` renamed `.off`).
`original/` untouched, `original/mashed_re_dev.asi` not redeployed. Runs under `mashed_lock.py` + test-slot, own PIDs only.
