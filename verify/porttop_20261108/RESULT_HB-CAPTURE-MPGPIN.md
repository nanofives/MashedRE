# RESULT HB-CAPTURE-MPGPIN (2026-11-09)

Verdict: **GREEN.** 12-capture drivers now run with `MASHED_DET_MPG_PIN=0`. Harness only. The deterministic-mode
default and the exe are unchanged.
Branch `orch/HB-CAPTURE-MPGPIN` from `orch/PORT-TOP-KNOBON-CAPFLAKE` (`b32923896`), with `orch/integration` merged in
(`git -c merge.autoStash=false`) because `re/tools/port_batch_bisect.py` lives there.

## Changes

| file | change |
|---|---|
| `verify/porttop_20261108/knob_boot.py` | nav-demo runs (`MASHED_NAV_DEMO=1`) get `MASHED_DET_MPG_PIN=0` unless the caller passes the variable as an argument or in its own environment. An empty value = pin off. This covers every PORT-TOP / bisect / run_ssm nav call, including the uncommitted `run_all_04*.py` drivers under `D:/MashedWT/*_data`, which all call `knob_boot.py` |
| `re/tools/port_batch_bisect.py` | passes `MASHED_DET_MPG_PIN=0` explicitly on its nav run, docstring updated |
| `verify/portstate_20261108/run_nav.py`, `verify/dualcopy4_20261016/run_nav.py` | `MASHED_DET_MPG_PIN=0` in the env (acts only when the caller adds `MASHED_DETERMINISTIC=1`; `extra` can override) |
| `verify/porttop_20261108/capflake_runs.py` | pin-off specs now pass an empty value, so the new default does not turn them on. `@dflt` leaves it to `knob_boot.py`. Data / root dirs overridable (`CAPFLAKE_DATA`, `CAPFLAKE_ROOT`) |
| `re/orchestrator/WORKER_RULES.md` | one line under "Builds and game runs": 12-capture checks run with `MASHED_DET_MPG_PIN=0` |

Not changed: `verify/portstate_20261109/savespan/exe_snap.py` / `run_snaps.sh` (Frida memory snapshot at walk_04, not
the capture check) and `verify/run_navdemo.bat` (manual launch, not deterministic). Race drivers (`run_race.py`) are
not nav-demo captures.

## Reference capture list, re-taken with the pin

PORT-TOP-KNOBON-CAPFLAKE arm A3 is the re-take: 12 nav runs with `MASHED_DET_MPG_PIN=0`, TOP knob ON / OFF / OFF / ON
x3, exe `409560ed`. All 12 runs gave the same 12 hashes. Freeze probe `pinned pos=55 hash=28538c74` (frontend.mpg
frame 0) in 12/12. The list equals the 04E list, so `NAV_REF` in `port_batch_bisect.py` and the `cmp_caps.py` list
need no change:

```
walk_01 83c8cbb913899494  walk_02 24249b644e3e8057  walk_03 feb2c803554cd028  walk_04 2871487f1fcef51c
walk_05 9f02cb91d09e0500  walk_06 b1f095c07072b28d  walk_07 3465159db8526481  walk_08 843e3fc49d166754
walk_09 5db0164fb2c6df8c  walk_10 ca2f64dc5e7a9407  walk_11 536f710d21e50619  walk_12 6d41bc3ac8ad8d8a
```

Source: `verify/porttop_20261108/capflake_runs.tsv` rows `A3` (committed on the CAPFLAKE branch).

## Checks

- Injection logic, no game launched: `knob_boot.py` run against a stub exe (a copy of `cmd.exe` that prints
  `set MASHED_DET`):

  | arguments | value the child gets |
  |---|---|
  | `MASHED_NAV_DEMO=1` | `MASHED_DET_MPG_PIN=0` |
  | `MASHED_NAV_DEMO=1 MASHED_DET_MPG_PIN=` | empty (off) |
  | `MASHED_NAV_DEMO=1 MASHED_DET_MPG_PIN=28` | `28` |
  | `MASHED_NAV_DEMO=1`, with `MASHED_DET_MPG_PIN=5` in the caller's environment | `5` |
  | `MASHED_ORIG_TOP=1`, no nav demo | not set |

- No exe change on this row, so no build and no gate run. The default gates on the CAPFLAKE exe were unchanged:
  G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3.
- A nav run on the merged exe through the `@dflt` path was deferred. A merge train held the machine (PRIORITY-HOLD
  from 21:09 and a 13-track SOLO window from 21:33). The first PORT-OT-BOOT capture set runs through this path.
