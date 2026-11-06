# RESULT HB-TRAIN-INFRA-PRECHECK (run 31, section HARNESS)

Verdict: GREEN. Both paths are proven by `merge_train.ps1 -SelfTest`: 47/47 PASS, 0 failures. The INFRA path holds the
row in the queue, and the normal path still bounces RED. Proof is offline only. The display has been asleep since
~09:20, so no game arm and no real probe-render ran (parent instruction).

Branch `orch/HB-TRAIN-INFRA-PRECHECK`, based on orch/integration `bf8abf3d0`. `merge_train.json` was not touched:
the self-test checks that its SHA-256 (`5B6E938AE452...`) is the same before and after.

## Problem

Run 31 pass 2 (`20261102_091940`) bounced MATCH-S08FU-NOTES to red-merge as "default gates changed". The display was
asleep, so every boot failed `CreateDevice` with D3DERR hr=0x8876086A and every gate hash came out MISSING.

## Change

`re/orchestrator/merge_train.ps1`
- `Get-ArmInfra(out, arms, hashes)` classifies a gate that did not match its reference:
  - D3D9-NOTAVAIL signature (`CreateDevice\(.*hr=0x8876086[AC]`, the same regex as diag.py's catalog) in an arm
    log that has no `CreateDevice(...) -> OK` line = INFRA. InitD3D9 tries several attempts and logs each one
    (`mashedmod/src/mashed_re/exe_main.cpp:3189-3203`), so a failed attempt followed by a working one is not the
    signature.
  - Every hash MISSING and no arm wrote a log = INFRA (the runner or the lock never ran the exe).
  - Every hash MISSING, logs present, no signature = `ALL-MISSING`, and the caller probes.
  - Otherwise not INFRA.
- `Test-RenderProbe(wt)` runs `diag.py probe-render --exe <merged exe> --cwd <CFG.root>` under
  `mashed_lock.py run --priority`. It returns OK, BLOCKED, UNKNOWN, or UNAVAILABLE (no RENDER line). Output goes to
  `<run data>/probe_render.txt`.
- `Run-Gates` (the gate part of Try-Batch, moved out so the self-test can drive it):
  - Ensure-Root, then the render precheck. A BLOCKED precheck = INFRA before any arm runs.
  - UNKNOWN or UNAVAILABLE goes on to the arms.
  - Gate-Default and Gate-Full report `infra`.
- `Gate-Default`:
  - A G-CTRL arm with the signature stops at once: no G-CTRL rerun, no B arms.
  - A RED result is classified with Get-ArmInfra. `ALL-MISSING` gets a post-arm probe: BLOCKED = INFRA, anything
    else stays RED and the gates text notes "a boot failure of the merged exe, not a display fault".
- `Gate-Full`: D2 arms that are not 3/3 are checked for the signature, in case the display went to sleep after the
  default gates.
- `Resolve-Batch(items, first)` is the step-2 outcome, returned as a hashtable and never dot-sourced:
  - INFRA on the batch: 0 bounced, no one-at-a-time re-merge.
  - INFRA during one-at-a-time: stop there. Rows already found RED stay bounced, and the rows after it are not tried
    and stay queued.
  - The normal RED and GREEN logic is unchanged.
- Run body:
  - `$verdict = INFRA` when INFRA occurred, otherwise RED or GREEN as before. Only `$bounced` rows get `red-merge` +
    `dequeue`. The REPORT `gates:` line says `not scored: ...` and `next:` carries `infra [...]`.
  - A GREEN row found before the INFRA still lands.
- Self-test fix (separate from the deliverable): the HB-D2RERUN block hardcoded `D2_DEF_1/2`. Since run 23,
  `merge_train.json` lists `D2_DEFH_1..3`, so 3 checks failed on bf8abf3d0 already
  (`selftest_baseline_bf8abf3d0.txt`). The block now reads the arm names from the config.

`scripts/diag.py` `probe-render`
- Adds optional `--exe`, `--cwd` (sets `MASHED_ROOT`, the log lands there) and `--secs`.
- Defaults are unchanged.
- Terminates only the PID it spawned.
- `check_render_capability` takes an optional log path.

`re/orchestrator/MERGE_TRAIN.md` gains one INFRA note. `.claude/skills/roadmap-loop/SKILL.md` gets two edits to the
train task: stop on INFRA as on RED, and re-run once the display is awake, never in a loop (the rows stay queued, so
"repeat until nothing queued" would spin).

### Deviation from the literal row text, and why

The row asks that all-MISSING gates give INFRA. Here all-MISSING gives INFRA only with display evidence:
- the signature in a log, or
- no arm log at all, or
- a BLOCKED probe after the arms.

All-MISSING with a live display (probe OK or UNKNOWN) stays RED. Otherwise a merged exe that crashes at boot would
classify as INFRA, never bounce, and block the queue head on every later pass. Likewise only a BLOCKED precheck is
INFRA: an UNKNOWN precheck (the exe died before CreateDevice) goes on to the arms and is classified there. If the lead
wants the literal rule instead, it is a one-line change in `Gate-Default` (treat `ALL-MISSING` as INFRA without the
probe).

## Proof (offline)

`py -3.12 .../testslot/proyectos_test_slot.py pwsh -NoProfile -File re/orchestrator/merge_train.ps1 -SelfTest`
-> `SELFTEST PASS (0 failure(s))`, 47 PASS lines (`selftest_HB-TRAIN-INFRA-PRECHECK.txt`). The 23 checks before
this step all still pass. The new checks drive the real `Get-ArmInfra`, `Run-Gates`, `Gate-Default` and
`Resolve-Batch`. Only the arm runner, the probe, `Ensure-Root`, `Remove-Temp` and `Try-Batch` are fakes.

| Path | Case | Result |
|---|---|---|
| INFRA | precheck BLOCKED | INFRA, 0 arms run |
| INFRA | precheck OK, G-CTRL log has hr=0x8876086A | INFRA, G-CTRL ran once, B not run |
| INFRA | precheck UNKNOWN, arm has hr=0x8876086A | INFRA |
| INFRA | arms wrote no log | INFRA |
| INFRA | all MISSING, clean logs, post-arm probe BLOCKED | INFRA, 2 probes |
| INFRA | Resolve-Batch: single row INFRA | 0 bounced, 0 merged, row left queued |
| INFRA | Resolve-Batch: 3-row batch INFRA | 0 bounced, 1 Try-Batch call (no re-merge) |
| INFRA | Resolve-Batch: INFRA mid one-at-a-time | earlier RED bounced, later row not tried |
| normal | precheck OK, refs match | GREEN, 1 probe |
| normal | changed B hash | RED "default gates changed" with `(ref ...)` |
| normal | all MISSING with probe OK | RED, noted as a boot failure of the merged exe |
| normal | hr=0x8876086A then a working device, changed hash | RED, not INFRA |
| normal | Resolve-Batch single RED | bounced |
| normal | batch RED, singles GREEN/RED/GREEN | first GREEN lands, RED bounced, second GREEN queued |
| normal | batch GREEN | all merged |
| AST | run body | Try-Batch -> Resolve-Batch, red-merge only for `$bounced`, verdict INFRA when `$infra` |
| unit | Get-ArmInfra | 7 cases incl. 0x8876086C and the fallback-device log |
| guard | merge_train.json | SHA-256 unchanged |

diag.py side (offline, synthetic logs fed to `check_render_capability(False, log)`):

| Log | Status |
|---|---|
| hr=0x8876086A only | BLOCKED |
| 086A then REF OK | OK |
| OK only | OK |
| no CreateDevice line | UNKNOWN |
| no log file | UNKNOWN |

`probe-render --help` lists `--exe --cwd --secs`, and a missing `--exe` returns rc 2.
Probe convention = run_arms' convention: cwd = private root, `MASHED_ROOT` = root (`verify/savelink_20261023/run_arms.py:242,275`).

## Not proven here

- A live probe-render and a live train pass. Both need an awake display. The first real train pass after this
  merges is the live witness, so check its `probe_render.txt` for a `RENDER: OK` line.
- The 13-track gate has no INFRA classification. It only runs after the default gates passed, so a mid-run sleep
  there still reads as RED. Follow-up candidate if it recurs.

## Evidence files

| File | SHA-256 |
|---|---|
| `verify/harness_20261102/selftest_HB-TRAIN-INFRA-PRECHECK.txt` | `34f2de7d63594d7807082172770bac0eea6173ca8283a26646c74a4be15a6670` |
| `verify/harness_20261102/selftest_baseline_bf8abf3d0.txt` (unmodified script, 3 pre-existing D2 FAILs) | `f3c29f15decbc9648f5a5c85d2bd1bbea75d64dd7e31cb0e45eedfdeef987de9` |

Gates: not run. The display is asleep, every boot returns hr=0x8876086A, and the change does not touch any build
input (`mashedmod/` is untouched).
