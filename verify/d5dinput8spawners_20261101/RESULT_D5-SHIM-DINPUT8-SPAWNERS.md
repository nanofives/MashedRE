# RESULT D5-SHIM-DINPUT8-SPAWNERS (2026-11-01)

Follow-up of D5-SHIM-AUTOLOAD-SWEEP (`verify/d5autoloadsweep_20261030/RESULT_D5-SHIM-AUTOLOAD-SWEEP.md`),
which found 46 live spawners that get `mashed_re_dev.asi` only through the `original\dinput8.dll`
proxy (22 never set `MASHED_RE_NO_AUTO_HOOK`, 24 pop it for the hooks-ON arm). Branch
`orch/D5-SHIM-DINPUT8-SPAWNERS` from `orch/integration` 0e3d71a75.

**VERDICT: GREEN (resumed 2026-11-01 after the 14:23 reboot). All 46 live spawners are dealt with in code: 28 rewritten to load the asi themselves (`re/frida/dev_asi.py`), 16 retired, 2 `.ps1` as an exact diff for Mariano (decision, not applied). Static re-scan: 0 live `.py` spawners still pop the var. All 8 per-family smokes and the `.ps1` boot CLI PASS original-side in a root with no dinput8 proxy (section 4). The smokes found one helper bug, fixed here: every FULL install (no `MASHED_HOOK_ONLY`: run_verify_hook, ttd, the boot CLI) died in `InjectHooks` with Frida's `Error: system error` (section 1, last bullet).**

## 1. What changed

One shared helper, `re/frida/dev_asi.py`, replaces the proxy for every Python spawner:

- The spawn env always carries `MASHED_RE_NO_AUTO_HOOK=1`. While the proxy is still deployed it
  therefore maps nothing (`dinput8_shim.cpp:61-71`), and the asi's DllMain never auto-installs
  (`dll_main.cpp:20-28`).
- Hooks ON = frida spawn suspended, `Module.load` of the asi, call its `InjectHooks` export once,
  then resume. `HookSystem::InstallAll` still reads `MASHED_HOOK_ONLY` / `_SKIP` / `_MANIFEST` /
  `_LO` / `_HI` from the spawn env (`HookSystem.cpp:226-262`), so every arm keeps its hook set. The
  JMPs are in place before the first MASHED.exe instruction runs (the proxy reached the same point
  at import resolution).
- Asi path = the proxy's own order (`dinput8_shim.cpp:84-96`): `<exe dir>\mashed_re_dev.asi`, then
  `<exe dir>\..\mashedmod\build\mashed_re_dev.asi`. `MASHED_DEV_ASI` / `MASHED_VERIFY_ASI` override.
- `original_exe()` honours `MASHED_ROOT` (WORKER_RULES private roots); every rewritten spawner uses it.
- A load into a process where the asi is already mapped refuses a second install (`preloaded`).
- `SpawnedProc` gives the `poll/wait/kill` subset of `Popen` over a frida pid (psutil).
- `py -3.12 re/frida/dev_asi.py boot [--seconds N] [--no-hooks] [--exe PATH]` prints `BOOTS t` or
  `DIED t 0xCODE` for the .ps1 boot bisects.
- **Fix found by the smokes:** `InjectHooks` is called through
  `NativeFunction(..., { exceptions: 'propagate' })`. A full `InstallAll` reaches `LoudLog`
  (`HookSystem.cpp:54-63`) for every refused site (`BOOT-PATCH REFUSED` `:121`,
  `DUAL-INSTALL REFUSED` `:146`). After the fix, the root's `hook_dual_install.log` gained
  `DUAL-INSTALL REFUSED` lines for registry entries whose target already starts with E9
  (e.g. `0x00493540` `thunk_LaunchLangGate`, `0x0041EA80` `GlobalField63d7e4_40`), the same kind
  of line the log accumulated (47 MB) under the proxy. `LoudLog` calls
  `OutputDebugStringA` (`HookSystem.cpp:55`). With no debugger attached, that API raises
  `DBG_PRINTEXCEPTION_C` and catches it in its own SEH frame. Frida's default (`steal`) takes the
  first-chance exception and throws `Error: system error`, so every full install failed. The
  proxy path never had Frida on the stack. Diagnostic (`diag_inject.py`, suspended spawn, never
  resumed): full install `error: system error`, no exception handler hit. `MASHED_HOOK_ONLY=0x0042b770`
  gives `installed: true`. `LoadLibraryW` of the asi alone returns a handle with GetLastError 0 in
  both full and minimal env (`diag_load.txt`). Single-RVA installs never reach `LoudLog`, which
  is why the helper smoke and 5 families passed before the fix.

`mashed_qol.asi` (the proxy's other load) is not handled: it is inert unless `MASHED_NO_SAVE` /
`MASHED_UNLOCK` / `MASHED_SPEED` are set, and no diff spawner sets them. The player launcher
`scripts/mashed_launch.ps1` still needs it (D5-SHIM-CONSUMERS section 3, unchanged here).

## 2. The 46 spawners

Families from the worker survey (raw: `D:/MashedWT/D5-SHIM-DINPUT8-SPAWNERS_data/survey_out.md`).

### Rewritten to dev_asi (28 files, 8 families)

| family | files | change |
|---|---|---|
| statenav | `re/frida/statenav.py`, `re/frida/verify_scoring_hooks.py`, `scripts/bisect_hooks.py`, `scripts/bisect_hooks_set.py`, `scripts/catch_wedge.py` | statenav and verify_scoring spawn through dev_asi (`--hooks` / not `--baseline` = load before resume). The three scripts that run `statenav.py --hooks` lost their now-meaningless var pop |
| scenario_launch | `re/frida/scenario_launch.py`, `re/tools/shadow_batch.py` (runs `scenario_launch.py --hooks`) | var set on every arm; `--hooks` arm spawns through dev_asi, stock arm keeps Popen |
| c4_*_scenario | `c4_champ_complete_scenario`, `c4_detect_e_scenario`, `c4_hud_scenario`, `c4_m_scenario`, `c4_pb2_scenario` | `off`/`on` arms load before resume with their `MASHED_HOOK_ONLY` (`__none__` / RVA list). `stock`/`ctl`/`late` unchanged (they already set the var; ctl/late keep their own `loadctl`) |
| canonical_c4_* | `canonical_c4_autodiff`, `_leafdiff`, `_navigate`, `_racediff`, `_sidediff`, `_stalkerdiff`, `_verify_hotpath`, `_verify`, `canonical_install_observe` | `dev.spawn`+attach (racediff: Popen+attach) replaced by dev_asi, hooks=True on both ON and OFF runs (OFF = `__none__`, asi loaded, nothing installed, as before) |
| phys_c4 | `phys_c4_bodydiff`, `phys_c4_telemetry` | Popen+attach replaced by dev_asi; bodydiff loads with the no-match sentinel, telemetry `baseline` never maps the asi |
| Popen probes | `nav_champ_probe`, `pickup_pos_probe2`, `replay_verify` | hooks arm through dev_asi (ABOVE_NORMAL priority kept for the first two), stock arm keeps Popen |
| run_verify_hook (path2) | `re/frida/run_verify_hook.py` | spawn through dev_asi with `ASI_PATH`, so the agent's `Module.load` of the same path returns the one copy. Ends the two-copy case of 2026-10-11 (0x004922e0). Agent template unchanged |
| ttd | `scripts/ttd/ttd_reimpl_diff.py` | spawn through dev_asi with the deployed asi |

### Retired (16 files): refuse to run

Each prints `RETIRED 2026-11-01 (D5-SHIM-DINPUT8-SPAWNERS)` and exits 1 before any import, pointing
at the diff-original path (`run_diff_parallel.py` path1, `run_verify_hook.py` path2,
`scenario_launch.py --hooks`). Source kept. Criteria: no live caller outside `verify/` (worker
survey (a) plus a repo grep), last commit 2026-05-15..06-09, superseded by the registry lane.
All 16 checked: exit 1 with the message.

`mass_canonical_observe`, `_r2`, `_r3`, `_wave0`, `hot_path_behavioral`, `implicit_survival_sweep`,
`canonical_observe_viewport_init`, `observe_c4_render_math_hooks`, `observe_c4_save_hooks`,
`observe_c4_sweep_20260517`, `observe_frontend_full_30s`, `observe_hooks_at_menu`,
`observe_lap_hooks_at_menu`, `diag_smoke`, `probe_005abf80`, `manual_test_config7`.

### .ps1 spawners (2): diff for Mariano, not applied

`scripts/bisect_asi_boot.ps1`, `scripts/bisect_hook_index.ps1`. Exact diff:
`verify/d5dinput8spawners_20261101/ps1_spawners.diff` (replaces each `Start-Process` boot with
`py.exe -3.12 re\frida\dev_asi.py boot --seconds $Seconds --exe $exe` and parses `BOOTS`/`DIED`).
Until it is applied these two still need the proxy.

## 3. Static re-scan (live `re/`, `scripts/`)

`grep` for `pop("MASHED_RE_NO_AUTO_HOOK"`, `!= 'MASHED_RE_NO_AUTO_HOOK'`, `del env[...]` and
`= "0"` over `re/` and `scripts/` `*.py` (excluding `prior_art`, `analysis`): 6 hits, all in
retired files (the six `observe_*`, which exit before reaching them). No spawner added or changed
on `orch/integration` since the sweep's base d6448c31e matches the spawn + `MASHED.exe` pattern.

## 4. Smokes (original-side, no dinput8 proxy)

Private root `D:\MashedWT\D5-SHIM-DINPUT8-SPAWNERS_root\original` = robocopy of `original\` **without
`dinput8.dll` / `dinput8_real.dll`**, `MASHED_ROOT` pointed at it, so the asi can reach MASHED.exe
only through dev_asi. Audio check (VB-Audio endpoints present) before each boot. Every boot under
`mashed_lock.py run --label D5-SHIM-DINPUT8-SPAWNERS`.

| # | smoke | status | evidence |
|---|---|---|---|
| 0 | helper: `smoke_dev_asi.py <root> 0x0042b770 15` (`MASHED_HOOK_ONLY=0x0042b770`, var deliberately popped from the caller env to prove the helper forces it) | **PASS** | asi loaded before resume from `<root>\original\mashed_re_dev.asi`, `installed=true`, one asi module in the process, `dinput8` = `C:\WINDOWS\SYSTEM32\DINPUT8.dll` (system, not a proxy), `mashed_qol.asi` absent, byte at 0x0042b770 = `0xE9`, rel32 target `0x5da953c0` inside the asi, pid alive at 15 s. A first run with `MASHED_HOOK_ONLY=0x42b770` installed nothing: the token must be `0x%08x` (`HookSystem.cpp:238-241`). Fixed in the smoke, not a helper bug |
| 1 | run_verify_hook (path2) | **PASS** (run 6) | path2 report: PRE/POST `e9 4b 9c 07 5e`, opcode 0xE9 OK, rel32 `0x5e079c4b` OK, "JMP already installed pre-load (asi loaded before resume); site verified", reimpl interceptor fired 1/1, FAIL (none). Run 4: the script defaults to the worktree's own `mashedmod/build` asi and this worktree has no build, so the driver now passes the script's existing `MASHED_VERIFY_ASI` override = the root asi. Run 5: `system error` (the helper bug) |
| 2 | canonical_c4 (`canonical_c4_verify.py`) | **PASS** (run 4, and run 6 after the fix as control) | OFF alive, ON alive, `0x0042b770 installed=True survived=True` (calls=0 at the menu in 20 s, so the C4 verdict itself is HOLD. This smoke tests the spawner, not the hook) |
| 3 | c4_*_scenario (`c4_detect_e_scenario.py --arm on`) | **PASS** (run 4) | arm on: entry `mashed_re_dev.asi+0x153c0`, JMP 0xE9 live, `verdict=DONE` |
| 4 | scenario_launch (`--hooks 0x0042b770 --hold 10`) | **PASS** (run 5) | "VERDICT: launcher reached a running race and spawned a car. [OK]". Run 4: `frida.TransportError: timeout was reached` in `device.attach` right after spawn. The helper killed its pid, and the same spawn+attach passed for every other family and on the re-run, so it was transient |
| 5 | statenav (`--hooks`, `MASHED_HOOK_ONLY=0x0042b770`) | **PASS** (run 4) | `[hooks] dev .asi ARMED (<root>\original\mashed_re_dev.asi)`, menu navigation to Quick Battle |
| 6 | phys_c4 (`phys_c4_telemetry.py hooked`) | **PASS** (run 4) | `A4 @0x00470670 first byte = 0xe9 (JMP-LIVE)` |
| 7 | Popen probes (`pickup_pos_probe2.py --hooks 0x0042b770`) | **PASS** (run 4) | first spawn attempt succeeded (no `spawn/load failed` line), track TRAINING, CAPTURED, 219 draw calls |
| 8 | ttd (`ttd_reimpl_diff.py ... --reimpl asi:FastSqrt`) | **PASS** (run 6) | `ok-auto` at phase 1, 128/128 bit-identical vs the TTD capture. Runs 4 and 5: `system error` (helper bug, full install) |
| 9 | `.ps1` boot CLI (`dev_asi.py boot --seconds 20`) | **PASS** (run 6) | `BOOTS 20.01` with a full install (no `MASHED_HOOK_ONLY`). Runs 4 and 5: `system error` (helper bug) |

Runs (all `mashed_lock.py run --solo --label D5-SHIM-DINPUT8-SPAWNERS`, audio check first: 3 VB-Audio
endpoints every run): run 4 = all 8 families + boot CLI, run 5 = run_verify_hook, scenario_launch,
ttd + boot CLI, run 6 = run_verify_hook, canonical_c4, ttd + boot CLI after the `dev_asi.py` fix. The
fix only changes how Frida treats an exception raised inside `InjectHooks`. Families 3 to 7 raised
none (they install one RVA), so their run-4 results stand, and canonical_c4 re-passed after the fix.
No MASHED.exe of this step was left running (0 under the private root after run 6). A first
non-solo queue (run 2) starved behind back-to-back solo windows and was stopped. Its 5 own PIDs were
killed, and it never spawned a game. Run 3 got the window, but `subprocess` could not exec the
extensionless `test-slot` shim, so nothing ran.

**Command** (from the worktree; `verify/d5dinput8spawners_20261101/smokes.sh` = audio check, then
`test-slot py -3.12 verify/d5dinput8spawners_20261101/run_family_smokes.py <root> <data> [families]`, then
`MASHED_ROOT=<root> test-slot py -3.12 re/frida/dev_asi.py boot --seconds 20`):

```
py -3.12 C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py run --solo --wait 7000 --label D5-SHIM-DINPUT8-SPAWNERS -- "C:/Program Files/Git/bin/bash.exe" verify/d5dinput8spawners_20261101/smokes.sh [family ...]
```

The private root `D:\MashedWT\D5-SHIM-DINPUT8-SPAWNERS_root` (no dinput8) is kept.

## 5. Gates

Not run: no `mashedmod/` file changed (Python harness, `verify/` and one diff only), so
`mashed_re.exe` and the asi are byte-identical to `orch/integration`.

## 6. Raw data

`D:/MashedWT/D5-SHIM-DINPUT8-SPAWNERS_data/`:

- `smoke_dev_asi.log` SHA-256 `0ed8060f4d99a5d8f34ee53313b49a71e4bfe4def4793173153c1ff1095b74d6` (helper smoke, PASS run)
- `survey_out.md` SHA-256 `48dfb9218656eda31b82b52690d26657648400a8014cc5d5fdcbe9afb8572c10` (worker survey of the 44 `.py` spawners: spawn mechanism, lines, families, callers)
- `family_smokes_run1.txt` SHA-256 `69b3e821bfb628ea69a10af75ad144527ec963e3bd425b5bf5121598e63ffcfb` (queued run, lock WAITING lines only)
- `family_smokes_run4.txt` `9e20e9d03bb0be05fbe68267efd560ddbbb8c4fceb92768cbd00c8a021fa5967`, `_run5.txt` `4bc87f169d69683fcd9567f8dcaa2419f313b2f0df65004a7a927d0a2200c91c`, `_run6.txt` `134c38b5308730ee02d76c7b27254208abdb994cc2bfc8b06badbcf00f88f4da` (driver lines)
- passing logs: `smoke_run_verify_hook.log` `dbccd29c7b2f0e7f845d7e39910eeffc0908679b7af2af7aab9a7bc3dd646848`, `smoke_canonical_c4.log` `65fee0c84da225adc1f081219bf940ff828ac27dbade80b9daf3fb4ea5c763b8`, `smoke_c4_scenario.log` `1f0e648be8072b9a33275aab8217d3ce51401096487551b96df0c6d16cf90329`, `smoke_scenario_launch.log` `d83c44e2e7b75bcd64bf02ffce41ba73ef80e8a0bbf44b096554b7f32712a3b5`, `smoke_statenav.log` `66a1f349ddd18414e6a210098e1f1c1b7e434febb8de91afc01d18c1c4132eb5`, `smoke_phys_c4.log` `50f47e2c6be7aecec98ca9229e24d460d0fe5801f9cb12480007387c59285590`, `smoke_popen_probes.log` `32d3017d65c6c950b08c94f63f7ab8496a74ec99d539bb91eaa10b6b8e6befd7`, `smoke_ttd.log` `df578d906612887ba3064a1b38b727e9f02135e2771d1ae61679cc6da8ad7b66`, `smoke_boot_cli.log` `a2be3cfc3eb7e3509bc6905e4b59be6d7ad96547a96941eb0fbaaa2299cd3a1f`
- failing logs kept: `smoke_run_verify_hook_run4.log` `ae4dca95...`, `_run5.log` `b6edd17d...`, `smoke_scenario_launch_run4.log` `091c7f4a...`, `smoke_ttd_run4.log` / `_run5.log` `d9670e69...`, `smoke_boot_cli_run4.log` `9f5e75da...`, `_run5.log` `e03bad90...`
- `diag_load.txt` `18be368b1f77db6972cfbc1d31b4be5f54997beaa5cbee6440557a4a73ab5595`, `diag_inject.txt` `f4c10d2a7a72fcef49731900688c9aa3e7cf2a54c32cc352695cb3d70c06eb30` (script committed as `diag_inject.py`)

## 7. Next

- Mariano (decision): apply `ps1_spawners.diff` (or retire the two bisects). The boot CLI it
  calls passed here (`BOOTS 20.01`, full install). After that 0 live spawners use the proxy for
  the dev asi.
- Shim removal step 3 is then blocked only by `scripts/mashed_launch.ps1` (QoL asi) and the 25
  historical `verify/` spawners, which are dated evidence and are not re-run.
