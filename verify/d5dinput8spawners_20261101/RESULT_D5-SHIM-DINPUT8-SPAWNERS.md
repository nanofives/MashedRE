# RESULT D5-SHIM-DINPUT8-SPAWNERS (2026-11-01)

Follow-up of D5-SHIM-AUTOLOAD-SWEEP (`verify/d5autoloadsweep_20261030/RESULT_D5-SHIM-AUTOLOAD-SWEEP.md`),
which found 46 live spawners that get `mashed_re_dev.asi` only through the `original\dinput8.dll`
proxy (22 never set `MASHED_RE_NO_AUTO_HOOK`, 24 pop it for the hooks-ON arm). Branch
`orch/D5-SHIM-DINPUT8-SPAWNERS` from `orch/integration` 0e3d71a75.

**VERDICT: HANDOFF. All 46 live spawners are dealt with in code: 28 rewritten to load the asi themselves (`re/frida/dev_asi.py`), 16 retired, 2 `.ps1` as an exact diff for Mariano. Static re-scan: 0 live `.py` spawners still pop the var. The shared helper passed its original-side smoke in a root with no dinput8 proxy. The 8 per-family smokes were NOT run: the machine lock was held by D4-DC-INC-pathprog plus a 12 h solo window (D4-WSB-PORT-BROAD-2), and the lead stopped the wait. GREEN needs those 8 smokes (command in section 4).**

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
| 1 | run_verify_hook (path2) | NOT RUN | lock |
| 2 | canonical_c4 (`canonical_c4_verify.py`) | NOT RUN | lock |
| 3 | c4_*_scenario (`c4_detect_e_scenario.py --arm on`) | NOT RUN | lock |
| 4 | scenario_launch (`--hooks 0x0042b770 --hold 10`) | NOT RUN | lock |
| 5 | statenav (`--hooks`, `MASHED_HOOK_ONLY=0x0042b770`) | NOT RUN | lock |
| 6 | phys_c4 (`phys_c4_telemetry.py hooked`) | NOT RUN | lock |
| 7 | Popen probes (`pickup_pos_probe2.py --hooks`) | NOT RUN | lock |
| 8 | ttd (`ttd_reimpl_diff.py ... --reimpl asi:FastSqrt`) | NOT RUN | lock |
| 9 | `.ps1` boot CLI (`dev_asi.py boot --seconds 20`) | NOT RUN | lock (not in the driver; run by hand) |

The driver queued at 13:22 and never got the lock (`family_smokes_run1.txt` holds only WAITING lines).
It was stopped and its 4 own PIDs (lock waiter, py launcher, sh, tee) killed; it never spawned a
MASHED.exe.

Static checks done instead: every rewritten file compiles (`py_compile`), every retired file exits 1
with the RETIRED line, and the re-scan in section 3.

**Command to finish this step once the lock is free** (from the worktree, audio check first):

```
pwsh -NoProfile -Command "Get-PnpDevice -Class AudioEndpoint -PresentOnly | ? FriendlyName -like '*VB-Audio*'"
py -3.12 C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py run --label D5-SHIM-DINPUT8-SPAWNERS -- py -3.12 verify/d5dinput8spawners_20261101/run_family_smokes.py D:/MashedWT/D5-SHIM-DINPUT8-SPAWNERS_root D:/MashedWT/D5-SHIM-DINPUT8-SPAWNERS_data
MASHED_ROOT=D:/MashedWT/D5-SHIM-DINPUT8-SPAWNERS_root py -3.12 C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py run --label D5-SHIM-DINPUT8-SPAWNERS -- py -3.12 re/frida/dev_asi.py boot --seconds 20
```

The driver prints `[N/8] <family> PASS|FAIL rc=.. s=..` and writes `smoke_<family>.log` per family
into the data dir. `rc=0` is the driver's PASS; each log's own verdict line (path2 report, C4 verdict,
`[hooks] asi loaded before resume`) still needs reading before GREEN. The root
`D:\MashedWT\D5-SHIM-DINPUT8-SPAWNERS_root` (no dinput8) is kept for it. A U-9677 piz-collision AV
(`MASHED.exe+0x1509b4` / `+0xe66db`) in any family is a re-run.

## 5. Gates

Not run: no `mashedmod/` file changed (Python harness, `verify/` and one diff only), so
`mashed_re.exe` and the asi are byte-identical to `orch/integration`.

## 6. Raw data

`D:/MashedWT/D5-SHIM-DINPUT8-SPAWNERS_data/`:

- `smoke_dev_asi.log` SHA-256 `0ed8060f4d99a5d8f34ee53313b49a71e4bfe4def4793173153c1ff1095b74d6` (helper smoke, PASS run)
- `survey_out.md` SHA-256 `48dfb9218656eda31b82b52690d26657648400a8014cc5d5fdcbe9afb8572c10` (worker survey of the 44 `.py` spawners: spawn mechanism, lines, families, callers)
- `family_smokes_run1.txt` SHA-256 `69b3e821bfb628ea69a10af75ad144527ec963e3bd425b5bf5121598e63ffcfb` (queued run, lock WAITING lines only)

## 7. Next

- Run the 8 family smokes + the boot CLI (section 4 command) when the lock is free. Any FAIL is a
  bug in that family's rewrite, fixed on this branch.
- Mariano: apply `ps1_spawners.diff` (or retire the two bisects). After that 0 live spawners use
  the proxy for the dev asi.
- Shim removal step 3 is then blocked only by `scripts/mashed_launch.ps1` (QoL asi) and the 25
  historical `verify/` spawners, which are dated evidence and are not re-run.
