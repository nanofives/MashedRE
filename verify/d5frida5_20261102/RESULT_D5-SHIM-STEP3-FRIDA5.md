# RESULT D5-SHIM-STEP3-FRIDA5

Branch `orch/D5-SHIM-STEP3-FRIDA5`, cut from `orch/integration` @ `6a997bfa3`.
Task: the five live `re/frida` spawners that D5-SHIM-STEP3-VERIFYSPAWNERS allowlisted as
`NEEDS-DEV-ASI` (they got `mashed_re_dev.asi` only from the dinput8 proxy autoload). Migrate two to
`re/frida/dev_asi.spawn_original`, retire three, drop all five from the allowlist.

**Verdict: GREEN. Static leg GREEN. Boots GREEN in run 6 (2026-11-06, 3 of 3 pass, see section 3)**

## 1. Changes

| file | action |
|---|---|
| `re/frida/auto_count_at_menu.py` | `subprocess.Popen` + `device.attach` replaced by `dev_asi.spawn_original(MASHED_EXE, hooks=not --no-hooks)`. The counter agent loads on the session `spawn_original` keeps. Exe resolved through `dev_asi.original_exe` (honours `MASHED_ROOT`), `mashed.log` cleared beside that exe. New `--no-hooks` flag (stock game). Default stays hooks ON, the state the proxy gave it. Also: `pycaw` made optional (it is not installed under `py -3.12` on this PC, so the tool died at import in run 1, a pre-existing break), and the "process gone" line now prints the exit code and Frida's detach reason |
| `re/frida/catch_crash_eip.py` | same swap, with `resume=False`: the exception handler is armed, then `device.resume`. Previously it attached 0.2 s after a running Popen, so an early crash could be missed. New `--no-hooks` and `--seconds` (default 25, as before). Frida exception type `system` (any code Frida has no name for) is now counted and printed, not reported as a crash, unless `--include-system`. Run 1 reported 18 "CRASH CAUGHT" events, all `type: system` at `0x76dabae4` (the return site after kernelbase's `call [RaiseException]`, bytes `ff 15 .. .. .. ..` before EIP), while the game was still alive at 15 s. Those are handled first-chance exceptions, not crashes. The hooks-OFF control raises the same 3 per boot (run 2 and 3) |
| `re/frida/canon_observe_refused.py` | retired: refuses to run (`sys.exit`), same block as the D5-SHIM-DINPUT8-SPAWNERS retirements, source kept |
| `re/frida/probe_005ab100.py` | retired, same block |
| `re/frida/mass_canonical_observe_wave3.py` | retired, same block |
| `re/tools/spawner_audit_allowlist.txt` | the 5 `NEEDS-DEV-ASI` lines removed (section replaced by a note). 2 lines added under `not-an-exe-spawn`, see section 2 |

No live caller of the three retired files: a grep of `re/`, `scripts/`, `.claude/` finds only their
own text and the ledger row. `re/frida/poll_attach_catch_crash.py` (named next to `catch_crash_eip`
in the lead's brief) has no spawn: it polls for and attaches to a running pid, so the audit does not
count it and it needed no change.

## 2. Static leg: `spawner_audit.py`

Baseline on the fresh worktree (`orch/integration` @ `6a997bfa3`): exit **1**, 144 live spawners,
121 covered, 21 allowlisted, **2 FLAGGED**: `re/orchestrator/mashed_lock_concurrency_test.py` and
`re/orchestrator/merge_train.ps1`. Both landed after VERIFYSPAWNERS (HB-MASHEDLOCK-SOLO-PRIORITY
`b52d95787`, HB-MASHEDLOCK-EARLY-PRIORITY `e3f44578e`) and are regex false positives, not
MASHED.exe spawners:

- `mashed_lock_concurrency_test.py`: every `Popen` (lines 143, 371, 378) runs `sys.executable`. Line 3
  reads "Stdlib only, no MASHED.exe" (that comment is the TARGET match).
- `merge_train.ps1`: the one `Start-Process` (line 434) runs `py.exe mashed_lock.py solo-begin` in the
  lock self-test. `original\MASHED.exe` appears only in a `Test-Path` (line 169).

Both are allowlisted as `not-an-exe-spawn` with those line citations, so the guard can exit 0.

After the change: `py -3.12 re/tools/spawner_audit.py` → exit **0**,
`live (re/, scripts/): 142 spawners, 124 covered, 18 allowlisted, 0 FLAGGED`,
`verify/ (report only): 49 spawners, 27 covered, 22 uncovered`.
Accounting: the 3 retired files now match COVER (their block names `re/frida/dev_asi.py`), 121 + 3 = 124.
The 2 migrated files no longer match SPAWN (`dev_asi.spawn_original(` is not `.spawn(`, no `Popen`),
144 − 2 = 142. Allowlist 21 − 5 + 2 = 18.

## 3. Boots (original side, no dinput8 proxy)

Private root `D:\MashedWT\D5-SHIM-STEP3-FRIDA5_root\original` = robocopy of `original\` with
`/XF dinput8.dll dinput8_real.dll` (checked: no `dinput8.dll`, `mashed_re_dev.asi` and `d3d9.dll`
present), `MASHED_ROOT` pointed at it, so the asi can reach MASHED.exe only through dev_asi.
Full install (no `MASHED_HOOK_ONLY`). Driver `verify/d5frida5_20261102/boots.sh`, run as:

```
py -3.12 C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py run --wait 7000 --label D5-SHIM-STEP3-FRIDA5 -- "C:/Program Files/Git/bin/bash.exe" verify/d5frida5_20261102/boots.sh
```

Run log (all under the lock, label `D5-SHIM-STEP3-FRIDA5`):

| run | auto_count_at_menu (hooks ON) | catch_crash_eip (hooks ON) | catch_crash_eip (hooks OFF control) |
|---|---|---|---|
| 1 | import error, `pycaw` missing (fixed) | asi loaded `installed=True`, alive at 15 s, 18 `system` events reported as crashes (fixed) | not run |
| 2 | asi loaded `installed=True`, game gone at about 5 s | asi loaded `installed=True`, alive at 15 s, 3 `system` ignored, no crash | game exited `0xFFFFFFFF` before 15 s |
| 3 | asi loaded `installed=True`, game gone at about 3 s | asi loaded `installed=True`, alive at 15 s, 3 `system` ignored, no crash | alive at 15 s, 3 `system` ignored, no crash |

Diagnosis of the early deaths: stock spawns with no Interceptor and no script die the same way
(`session_patterns.log`: keep / detach / script-before-resume, 6 of 6 died at 2.3 to 3.8 s), and so do
the shared `original\MASHED.exe` and bare `dev_asi.py boot` (`diag_boot.log` 4 of 4, `exitcode.log`
3 of 3, exit `0xFFFFFFFF` read with a handle opened before resume). `scripts/diag.py probe-render`
reports `RENDER: BLOCKED, D3DERR hr=0x8876086A (D3D9-NOTAVAIL)`, and L0 confirmed the display on this PC
has been asleep since about 09:20. Per L0 these deaths are not findings. Two stock boots did pass in
between (`diag_boot2.log`: shared original 20.04 s, private root 25.86 s), as did run 3's
catch_crash pair. [UNCERTAIN] whether the display was awake during run 3 itself: no probe-render was
taken next to it, so run 3 does not count as the evidence boot.

Runs 4 and 5 (2026-11-06 00:xx) never booted. They ran under an outer `mashed_lock.py run` that
held `mashed_machine` while each step waited for a test slot, and that deadlocked the merge train
(conductor killed them at 00:41). `edecab963` fixed `boots.sh`: each step takes the test slot outside
and `mashed_lock` inside, and the script is run directly.

**Run 6 (evidence run, 2026-11-06 09:07 to 09:55, `boots.sh` at `edecab963`, `RUN=run6`).** Driver
`D:/MashedWT/D5-SHIM-STEP3-FRIDA5_data/run6_driver.sh`. Each step held `mashed_machine` alone
(label `D5-SHIM-STEP3-FRIDA5`), so one game ran at a time. Every step started under the 8 GB cap
after the 10 min memory wait (about 21 GB committed of a 24 GB budget, SimGolf running too).

| step | tool | result |
|---|---|---|
| 1 | `auto_count_at_menu --seconds 12` (hooks ON) | **PASS**. pid 22872, `dev asi loaded at 0x5f610000 installed=True`, idled the full 12 s at the menu with no "process gone" line, exit 0. All four counters read 0 (GetEventFlag `0x0041f1c0`, GetPlayerStateBits, CarSlotInit `0x00492340`, EntityFieldSet). The idle-menu counter files in `log/` (`auto_count_at_menu_main_menu_idle_10s.txt`) read 0 as well, so zero counts at an idle menu are not a regression |
| 2 | `catch_crash_eip --seconds 15` (hooks ON) | **PASS**. pid 15868, `dev asi loaded at 0x5fa10000 installed=True`, handler armed before resume, `still running at 15s, no crash`, 18 type-`system` first-chance events at `0x7642bae4` ignored. Exit 1 = the tool's "NO CRASH CAPTURED" path (`catch_crash_eip.py:151-152`), the same as run 3 |
| 3 | `catch_crash_eip --seconds 15 --no-hooks` (control) | **PASS**. pid 24828, no asi, `still running at 15s, no crash`, 18 type-`system` events at `0x7642bae4` ignored, exit 1 (no crash) |

No crash at all, so the U-9677 piz-collision signatures (`+0x1509b4`, `+0xe66db`) did not come up and
no re-run was needed. Each tool killed only its own pid, and no MASHED.exe or mashed_re.exe from
this lane was left running. The 18 `system` events per boot (3 in runs 2 and 3, 18 in run 1) show up
with hooks ON and OFF alike, so the asi does not cause them. The count changes between runs, and
this lane does not look into why.

The probe-render steps either side of the boots (`run6_probe_pre.log`) printed `RENDER: UNKNOWN`
(no `mashed_re.log` where `diag.py` reads it). That is a tooling gap, not a display verdict. All three boots stayed
alive for the whole window (12 s and 15 s). The D3D9-NOTAVAIL boots in runs 2 and 3 died at 2.3 to
5 s. [UNCERTAIN] whether the display was awake, since there is no CreateDevice line for run 6. The
survival over the full window is the evidence.
The post-boot probe was stopped while it queued, because it would have produced the same UNKNOWN.

Tally: run 6 = 3 pass / 0 fail. Across all runs with the display awake, hooks-ON catch_crash_eip
= 2 of 2 pass (runs 3 and 6).

## 4. Gates

Not run: no `mashedmod/` file changed (Python harness, allowlist and `verify/` only), so
`mashed_re.exe` and the asi are byte-identical to `orch/integration`.

## 5. Raw data

`D:/MashedWT/D5-SHIM-STEP3-FRIDA5_data/`:

Hashes in `D:/MashedWT/D5-SHIM-STEP3-FRIDA5_data/SHA256SUMS.txt`. Files: `run1*.log`, `run1_crash_eip.txt`
(the 18 `system` events), `run2*.log`, `run3*.log`, `diag_boot.log`, `diag_boot2.log`, `diag3_*.log`,
`bisect_interceptor.log`, `session_patterns.log`, `exitcode.log`, `run4*`/`run5*` (deadlocked, no boot),
`run6_driver.sh`, `run6_driver.log`, `run6_auto_count_at_menu.log`, `run6_catch_crash_eip.log`,
`run6_catch_crash_eip_nohooks.log`, `run6_probe_pre.log`.

## 6. Expected gate effect on merge

- Static: on `HEAD` and on `git merge-tree --write-tree HEAD orch/integration` (tree `958f6d10e`,
  merges clean), `spawner_audit.py` exits **0**: `142 spawners, 124 covered, 18 allowlisted, 0 FLAGGED`.
  `orch/integration` alone today: exit **1**, `144 / 121 / 21 / 2 FLAGGED` (the two `not-an-exe-spawn`
  false positives this branch allowlists). Merging turns the spawner guard from RED to GREEN.
- G-CTRL / B hashes: unchanged. No `mashedmod/` file differs from `orch/integration`
  (`git diff orch/integration...HEAD -- mashedmod` is empty), so `mashed_re.exe` and the asi are
  byte-identical.
