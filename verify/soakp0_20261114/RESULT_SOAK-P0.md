# RESULT SOAK-P0 (probe on the ORIGINAL, no build) — PARTIAL, HANDOFF for the LIVING-PC reboot

Date 2026-11-14. Branch `orch/SOAK-P0` from `orch/integration` `2c7f97e41`. Spec: `re/analysis/SOAK_DRIVER_DESIGN.md`
(row cites `orch/SOAK-DRIVER @ 96c6701d7`: the branch name no longer resolves in `git branch -a --list "*SOAK*"`, the
commit `96c6701d7` exists in `git log --all`; the spec file was read from the `orch-integration` worktree).
Decision L-155 (a): AI on all four cars, unattended, as designed.

## What changed (harness only, existing tool extended as the spec says)
- `re/frida/js/detorig.js`: scenario field `slots` (4 states) feeds `FUN_0040e480(k, slots[k])` in `warp()`; default unchanged
  (`[1, 2, 2, 2]` cut at `cars`). `cfg.probe` skips checkpoints and the end event. New rpc `probe()`: plain reads, no Interceptor.
- `re/frida/detorig.py`: `--scen <json>` (scenario + `slots` + `input`), `--probe`, `--probe-every`, `--probe-after-end`,
  `--probe-stop-results`; samples go to `probe.jsonl`.
- `verify/soakp0_20261114/`: `scen/*.json`, `gen_scen.py`, `probe_summary.py`, this file.

## Run discipline
Private root `D:\MashedWT\SOAK-P0_root` (robocopy of `original\`, 482.7 MB), `MASHED_ROOT` set, every launch through
`mashed_lock.py run --label SOAK-P0`. Command of run (b):
`py.exe -3.12 re/orchestrator/mashed_lock.py run --label SOAK-P0 -- py.exe -3.12 re/frida/detorig.py --label b_ai4 --out D:\MashedWT\SOAK-P0_data\b_ai4 --boot --clock tick --warp-tick 600 --tick-cap 16 --scen verify/soakp0_20261114/scen/b_ai4.json --probe --probe-stop-results --probe-after-end 90 --timeout 840 --crash-catch 30`
(Frida path, not the ASI: the ASI has no `slots`, that is row `SOAK-SLOTS`.) FPS cap default 60 (`MASHED_FPS_CAP`).
The reboot notice stopped the batch: a_base was queued behind another lock holder and cancelled before the race; c and e never started.

## Observed: run (b), `slots [2,2,2,2]`, track 0, mode 10, rule 0, no input rows
Raw: `D:/MashedWT/SOAK-P0_data/b_ai4/probe.jsonl` (804 samples, 1 Hz wall, 335,110 B), SHA-256
`E804E99E1154128F30BA791C8470FB0B4B12D89A705F0D779D6DB10F6F14F889`; `meta.json` `552BD73A31B1F21C3062078C9A7767E9B69875DA6B5A410EE9027744C584B589`;
log `D:/MashedWT/SOAK-P0_data/b_ai4.log` `EFC65C8337A8AAB3A6130CE7000D05A8FA1D85C26401740F4628F3E1C1E59901`. rc 6 = the 840 s timeout, no crash line.
Summary: `py -3.12 verify/soakp0_20261114/probe_summary.py D:/MashedWT/SOAK-P0_data/b_ai4/probe.jsonl`.

Probe reads: phase `0x00771968`, in-race UI `0x0063ba8c`, `0x0063b90c`, `0x0063b914`, frontend depth `0x0067e9f8`, frontend phase
`0x0067eca4`, screen type `*(0x0067ed3c + depth*0x40)`, car position floats at record `0x008815a0 + k*0xd04` `+0x958/+0x95c/+0x960`
(the `+0x928` matrix row, `+0x30`; position meaning taken from memory note `msd-world-position-is-the-0x928-matrix-row`, not re-derived here),
slot state at `*(*0x005f2770 + 0x34 + 4k)` (read back `[2,2,2,2]`, so the warp's `FUN_0040e480` writes land there for `slots [2,2,2,2]`).

Timeline (tick, wall s, phase, `0x0063ba8c`): (600, 20.5, 2, 0) warp; (603, 26.6, 3, 3) race anchor; (1333, 40.2, 3, 5); (1456, 42.4, 3, 6);
(1916, 50.7, 3, 7). Then `0x0063ba8c` stays 7 for 755 consecutive samples to tick 38,673 (wall 840 s). `0x0063b90c` = 0, `0x0063b914` = 0,
frontend depth 1, frontend phase 3, screen type 22 for the whole run. Car displacement from the first race sample (raw floats in the file):

| car | first sample (x, y, z) | final (x, y, z) | distance moved |
|---|---|---|---|
| 0 | 0.5009, 0.4345, -1.9989 | 12.7772, -0.00076, -14.2977 | 17.38 |
| 1 | -1.1012, 0.4688, -0.9019 | 7.7559, -0.00093, -21.4484 | 22.38 |
| 2 | 1.1367, 0.5033, 0.2024 | 2.2172, -0.01682, -18.9310 | 19.17 |
| 3 | -0.4654, 0.5376, 1.2995 | 12.3976, -0.1293, -11.8941 | 18.44 |

Positions are static from about tick 1978 (17.28 / 22.38 / 19.30 / 18.44 at tick 1978, 17.38 / 22.38 / 19.17 / 18.44 at tick 4551 and after).
Wall rate: about 300 ticks per 6 s at the 60 fps cap, so about 50 ticks/s.

## Answers so far
1. **Does slot 0 at state 2 make the original's AI drive car 0?** Observed: car 0 moved 17.38 units between tick 1456 and tick 1978, the same order as
   cars 1 to 3 (18.44 to 22.38), with no input row and the slot readback `[2,2,2,2]`. Not run yet: the control (a) `slots [1,2,2,2]` with no input, to show
   car 0 stays put when it is a human slot. Until (a) runs, "AI drives car 0" rests on equal motion with the three AI slots, not on a contrast. [UNCERTAIN]
2. **Does a no-human match reach match end (`0x0063ba8c == 0xb`) and Race Results?** Observed NO within 840 s / 38,673 ticks: `0x0063ba8c` went 3, 5, 6, 7 and stayed 7
   from tick 1916 to the end, all four cars stationary, `0x0063b90c` 0, results screen never shown (screen type 22, not 5).
   What state 7 is, and whether a control press advances it (the 2026-10-12 note in the spec: 1 Hz pulses of control 4 reached Race Results in about 60 s with a human slot 0),
   is not measured. The mechanism is [UNCERTAIN]. That is exactly run (c) below.
3. **Does a warp-entered race reach results?** Open (needs question 2 resolved first). In run (b) the frontend stack stayed at depth 1, phase 3, screen type 22 across the race.
4. **What ends TimeTrial?** Open, not run. Scenario file `scen/d_timetrial.json` is ready (mode 2, `cars 1`, `slots [2,0,0,0]`).
5. **Time per match.** Open. Measured only: about 50 ticks/s wall at the 60 fps cap, race anchor at wall 26.6 s, the stall at tick 1916 (50.7 s). `MASHED_FPS_CAP_RACE=0` not tried.

## Proposed follow-up (what is left, exact next commands)
All from the `.worktrees/SOAK-P0` worktree, after the reboot and a free `mashed_lock.py status`; `$env:MASHED_ROOT='D:\MashedWT\SOAK-P0_root'`, wrap each as
`py.exe -3.12 re/orchestrator/mashed_lock.py run --label SOAK-P0 -- py.exe -3.12 re/frida/detorig.py --label <L> --out D:\MashedWT\SOAK-P0_data\<L> --boot --clock tick --warp-tick 600 --tick-cap 16 --scen verify/soakp0_20261114/scen/<S>.json --probe --probe-stop-results --probe-after-end 90 --timeout <T> --crash-catch 30`
(`py.exe -3.12 verify/soakp0_20261114/gen_scen.py` first, it writes `c_ai4_pulse4.json` and `e_human_pulse4.json`; the root already exists, check `original\MASHED.exe` is there):
1. `a_base` / `a_base`, T 200: control for question 1 (car 0 displacement with slot 0 at state 1).
2. `c_ai4_pulse4` / `c_ai4_pulse4`, T 420: all-AI plus 1 Hz control-4 pulses on player 0 from dN 8000. Does `0x0063ba8c` leave 7, reach 0xb, and does the results screen (`fphase 3`, screen 5) appear after the warp? Answers 2 and 3.
3. `e_human_pulse4` / `e_human_pulse4`, T 420: the 2026-10-12 condition inside a warp; gives time to match end (question 5) and the warp-vs-menu comparison.
4. `d_timetrial` / `d_timetrial`, T 300, then the same with an `MASHED_FPS_CAP_RACE=0` pair for question 5.
5. One MP-family cell (`mode 6`) per the spec's run (e) if time allows.
If run 2 shows control 4 is what advances state 7 for an all-AI match, then the soak driver needs a scripted accept press on slot 0 (or the state-7 reader identified): a design row, not assumed here.

## Notes
- `mashed_lock.py status` showed a stale `HELD 'mashed_machine' pid=19856 alive=False label=SOAK-P0` after I stopped my queued chain (my own run's lock, dead owner, no live process of mine matches `SOAK-P0` in its command line).
- Gates: not run (probe). No build. `original\` untouched.
