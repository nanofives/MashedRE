# RESULT D5-SHIM-STEP3-FRIDA5

Branch `orch/D5-SHIM-STEP3-FRIDA5`, cut from `orch/integration` @ `6a997bfa3`.
Task: the five live `re/frida` spawners that D5-SHIM-STEP3-VERIFYSPAWNERS allowlisted as
`NEEDS-DEV-ASI` (they got `mashed_re_dev.asi` only from the dinput8 proxy autoload). Migrate two to
`re/frida/dev_asi.spawn_original`, retire three, drop all five from the allowlist.

**Verdict: static leg GREEN, boots PENDING (display asleep, D3D9-NOTAVAIL since about 09:20; see section 3)**

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

__BOOTS_FINAL__

## 4. Gates

Not run: no `mashedmod/` file changed (Python harness, allowlist and `verify/` only), so
`mashed_re.exe` and the asi are byte-identical to `orch/integration`.

## 5. Raw data

`D:/MashedWT/D5-SHIM-STEP3-FRIDA5_data/`:

Hashes in `D:/MashedWT/D5-SHIM-STEP3-FRIDA5_data/SHA256SUMS.txt`. Files: `run1*.log`, `run1_crash_eip.txt`
(the 18 `system` events), `run2*.log`, `run3*.log`, `diag_boot.log`, `diag_boot2.log`, `diag3_*.log`,
`bisect_interceptor.log`, `session_patterns.log`, `exitcode.log`.
