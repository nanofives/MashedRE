# RESULT SOAK-P0: probe on the ORIGINAL (no build, no gates)

Date 2026-11-14 (runs 10:54 to 16:45, LIVING-PC reboot at 13:56 in between). Branch `orch/SOAK-P0` from `orch/integration`
`2c7f97e41`. Spec `re/analysis/SOAK_DRIVER_DESIGN.md` section 5.1. The row cites `orch/SOAK-DRIVER @ 96c6701d7`: that branch
name no longer resolves (`git branch -a --list "*SOAK*"` lists only `orch/SOAK-P0`), the commit exists in `git log --all`, and
the spec was read from the `orch-integration` worktree. Decision L-155 (a): AI on all four cars, unattended.

## Verdict per question (detail and raw values below)

| # | question | answer from runs |
|---|---|---|
| 1 | slot 0 at state 2 makes the AI drive car 0? | **Partly.** One-car TimeTrial: state 2 moves car 0, state 1 leaves it bit-identical for 16,942 ticks. Four-car QuickRace (mode 10): car 0 moves with state 1 **and** with state 2, and player 0's block at `0x007f1038` carries non-zero values in both. Which code writes it is [UNCERTAIN]. |
| 2 | no-human match reaches match end `0x0063ba8c == 0xb`? | **Only with a control-4 press on player 0.** No input: `0x0063ba8c` stops at 7 after round 1 and stays (38,673 ticks). With 1 Hz control-4 pulses on player 0 and all four slots at state 2: 0xb at tick 5,454, 6 rounds, winner `0x0063b914 = 3`. |
| 3 | warp-entered race reaches Race Results? | **Match end yes, the spec's results detector no.** After 0xb the phase byte goes to 5 and the frontend pushes screen types 1, 2, 4, 6 (depth 1 to 5), then `0x0067eca4 = 0`. Screen type 5 never appears. What screen 6 is: [UNCERTAIN]. |
| 4 | what ends TimeTrial? | **Open.** Mode 2, slot 0 state 2: `0x0063ba8c` reaches 7 at tick 2,407 after 0.95 units of motion, no 0xb in 300 s. State 1: stays 6. No lap was completed in either. |
| 5 | time per match | All-AI + pulses: **4,822 ticks** race anchor to 0xb, 83.0 s wall at the 60 fps cap (about 58 ticks/s), 6 rounds. Human slot + pulses: 5,883 ticks, 5 rounds (wall inflated by machine stalls). Uncapped (`MASHED_FPS_CAP_RACE=0`) not measured. |

## Harness change (existing tool extended, as spec 5.1 says)
- `re/frida/js/detorig.js`: scenario field `slots` -> `FUN_0040e480(k, slots[k])` in `warp()`. Default unchanged: the expression
  `k === 0 ? 1 : (k < s.cars ? 2 : 0)` stays when `slots` is absent. `cfg.probe`: no checkpoints, no end event. New rpc `probe()`:
  plain memory reads, no Interceptor.
- `re/frida/detorig.py`: `--scen <json>`, `--probe`, `--probe-every`, `--probe-after-end`, `--probe-stop-results` -> `probe.jsonl`.
- `verify/soakp0_20261114/`: `scen/*.json`, `gen_scen.py` (pulse scenarios), `run_chain.sh` (the launches), `probe_summary.py`,
  `u_decode.py` (raw-column decoder for the unedited run), this file.
- **These edits exist only on `orch/SOAK-P0`.** From the `orch-integration` worktree (HEAD `2c7f97e41`):
  `git diff orch/integration -- re/frida/detorig.py re/frida/js/detorig.js` = 0 lines, `git status` clean for both files.
  A scan of every `orch/*` branch with `git diff --name-only orch/integration...<branch>` on those two files finds only
  `orch/SOAK-P0`. Other live rows at the time (`RO-ORACLE-GATE`, `IMG-5-TEARDOWN-BODIES`, `PORT-TOP-AUDIOPOOL-5AE650`,
  `L156-ECX-556CA0`) do not edit detorig on their branches. Whether any of them executes detorig was not checked
  beyond that: `IMG-5-TEARDOWN-BODIES`'s `D:/MashedWT/.../run46/boots.sh` does not name detorig.

Probe reads (all raw): phase byte `0x00771968`, in-race UI word `0x0063ba8c`, `0x0063b90c` ("ended"), `0x0063b914` ("winner"),
frontend depth `0x0067e9f8`, frontend phase `0x0067eca4`, screen type `*(0x0067ed3c + depth*0x40)` (addresses from
`re/frida/c4_detect_e_scenario.py`), mode `0x0067e9fc`, car k position floats at `0x008815a0 + k*0xd04` `+0x958/+0x95c/+0x960`
(car record base and stride from `detorig.py` REGIONS; the `+0x958` = `+0x928` matrix row `+0x30` meaning is from the memory
note `msd-world-position-is-the-0x928-matrix-row`, not re-derived here), slot state `*(*0x005f2770 + 0x34 + 4k)` (reads back the
`slots` written in every run), and from a2/b2 on player 0 and 1 dwords of `0x007f1038 + p*0x4c` (named "cooked input
0x007f1038" in `detorig.py` REGIONS; that name is the file's, not verified here).

## Run discipline
- Private root `D:\MashedWT\SOAK-P0_root` (robocopy of `original\`, 482.7 MB), `MASHED_ROOT` set, Frida boot mode
  (`--boot --clock tick --warp-tick 600`), track 0, rule 0, car 0, team 0, `MASHED_FPS_CAP` 60, `--crash-catch 30`.
  Every launch through `mashed_lock.py run --label SOAK-P0`. `original\` untouched, no build, no gates.
- **test-slot:** runs `b_ai4` (before the reboot) and `a_base` (first step after it) ran **without** test-slot. `u`, `c`,
  `e`, `d`, `d1`, `a2`, `b2` ran as `test-slot <mashed_lock run ... -- detorig ...>` (`run_chain.sh`) with
  `PROYECTOS_TEST_HUB_VERBOSE=1`. Every one printed
  `[test-hub] running locally: C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py is outside the repo, the hub would not have it`
  (the hub was also offline: `test-slot --hub` -> `ssh: connect to host 192.168.68.114 port 22: Connection timed out`).
  Each waited 600 s at the memory gate (`memory still short after 10 min, starting anyway under the cap`).
- **Stale lock:** the first post-reboot acquire printed `[mashed-lock] breaking STALE 'mashed_machine' (pid=19856 alive=False age=12000s)`.
  The tool reclaimed it itself (header line 19, "Stale-safe"). No manual release or delete was run.
- **Chain incidents (own PIDs only):** a bash-launched chain never wrote its log and was stopped with TaskStop. Its script
  kept running, so a second launch made a duplicate chain. I stopped the duplicate by explicit PIDs 22180, 10928, 20516, 18360 and 20716
  before it took the lock. In that window the first `u_unedited` attempt exited about 4 s after its slot without any output
  (two tee processes were writing the same log), cause [UNCERTAIN]. I reran it alone and unfiltered, and it completed (below).
  Before the reboot, the queued `a_base`/`c`/`e` launches were cancelled before any game started.
- The `CRASH {"what": "stall", ...}` lines in `e`, `a2` are the crash catcher's 30 s main-thread stall samples (EIP in
  `ntdll.dll` / `win32u.dll`) while the PC sat at about 24 GB committed. No first-fault event and no `game exited` in any run.

## Runs and observed values

Tick numbers are the tick-clock count from process start (warp at tick 600, race anchor at the first phase-3 tick).

| run | scen | slots | input | result |
|---|---|---|---|---|
| b_ai4 | mode 10, 4 cars | 2,2,2,2 | none | 3->5->6->7 at 1916, stays 7 to tick 38,673 (840 s timeout). Car disp 17.38 / 22.38 / 19.17 / 18.44, static from about tick 1978 |
| a_base | mode 10, 4 cars | 1,2,2,2 | none | 3->4->5->6->7 at 2458, stays 7 to tick 8,851 (200 s). Disp 25.84 / 14.97 / 13.41 / 25.49 |
| c_ai4_pulse4 | mode 10, 4 cars | 2,2,2,2 | control 4 = 0xff for 110 clock calls every 660 from dN 8000 (about 1 Hz) | round ends (7) at 1964, 2624, 3347, 3829, 4491, each followed by 5->6. **0xb at 5454**, `0x0063b90c` 1 then 2 at 5514, winner 3. Then phase 5, UI 9, frontend (depth, type) (2,1) 5693, (3,2) 5993, (4,4) 6053, (5,6) 6114, `0x0067eca4` 0 from 6173 to the last sample (tick 10,688) |
| e_human_pulse4 | mode 10, 4 cars | 1,2,2,2 | same pulses | round ends at 2475, 3729, 4802, 5776. **0xb at 6525**, winner 3, same post-match sequence, depth 5 type 6 phase 0 to tick 9,684 |
| d_timetrial | mode 2, 1 car | 2,0,0,0 | none | car 0 (0.5009, 0.4345, -1.9989) -> (-0.4473, 0.4401, -2.0228), 0.95 units. 3->5->6, 7 at 2407, stays 7 to tick 10,946 (300 s) |
| d1_tt_human | mode 2, 1 car | 1,0,0,0 | none | car 0 identical floats from first to last sample. 3->5->6 at 1491, stays 6 to tick 16,942 (300 s) |
| a2_base_cin | mode 10, 4 cars | 1,2,2,2 | none | timeout at tick 1,768 (heavy stalls). From UI 6 on, player 0 dwords at `0x007f1038`: dword 1 = 0xff / 0xff00 / 0xffff, dword 0 = 0x1b, 0xa, 0x5600, 0x1700, 0xff00. Player 1: dword 9 = 1 throughout |
| b2_ai4_cin | mode 10, 4 cars | 2,2,2,2 | none | UI 7 at end (tick 3,941). Player 0 dwords from UI 6 on: dword 1 = 0xff / 0xff00, dword 0 = 0x38, 0x73, 0x4c00, 0xd800, 0x9d00. All zero again at UI 7. Player 1 same as a2 |
| u_unedited | UNEDITED `orch-integration` `re/frida/detorig.py` default `SCENARIO` / `INPUT` (slot 0 = 1, control 4 held from the warp, steering pulses), `--tick-cap 3000 --tick-raw 0x0063ba8c-0x0063ba90,<car positions>` | 1,2,2,2 (hard-coded) | default | rc 0, 15 ckpts, 3,000 ticks. Per-tick `0x0063ba8c`: 2,3,4,5 at 601-604, 6 at 716, **7 at 1653, 8 at 1753**, 2,4,5 at 1754-1756, 6 at 1868, **7 at 2411, 8 at 2511**, 2,4,5, 6 at 2626. Cars all moving (disp 23.96 / 24.79 / 19.03 / 24.22 at tick 3000) |

### On the conductor's reproduction request (item 2)
The unedited harness **cannot set slots** (`orch/integration:re/frida/js/detorig.js:278`,
`e480(k, k === 0 ? 1 : (k < s.cars ? 2 : 0))`, `SCENARIO` is a constant) and **cannot run without input**: its `INPUT`
holds control 4 from the warp (`detorig.py` `INPUT[0] = [0, 4, 0xff]`). So the b scenario cannot run on it, and neither can the
no-input a scenario. Its closest form ran (u). There `0x0063ba8c` reaches 7 at the end of each round and **leaves it after
exactly 100 ticks** (1653 -> 1753, 2411 -> 2511) through 8. The freeze at 7 therefore does not reproduce on the unedited
harness, whose input holds control 4. In the edited harness the freeze appears in every run with no input (b, a, d, b2), and
pulsed control 4 clears it (c, e). That makes the freeze a property of "no control 4 on player 0", not of `slots`. The edit's
only behavioural change is the `FUN_0040e480` argument, and a_base / e (slot 0 = 1, the unedited value) show the same freeze
and the same release as b / c (slot 0 = 2).

## Answers in detail

**1.** One-car TimeTrial (mode 2) is the clean contrast, with no other car to push car 0. At state 2, car 0 moved 0.95 units and
`0x0063ba8c` went 6 -> 7. At state 1, car 0's three floats did not change in any digit over 16,942 ticks. In the four-car
QuickRace, car 0 moved in both states (state 1: 25.84 and 16.11, state 2: 17.38 and 17.38). In both states the dwords at
`0x007f1038` for player 0 went non-zero from UI 6 with no scripted input. So in mode 10, slot 0 at state 1 is driven by something.
Whether that something is the original's AI routine, and whether state 2 routes car 0 through it, is [UNCERTAIN]. Missing: the
writer of `0x007f1038` player 0 in a and b. Follow-up: detorig `--watch 0x007f1038` (existing write-watch flag, pc per write) on
`a_base` and `b_ai4`.

**2.** With no input, no: after round 1 the UI word stops at 7 and nothing advances it (b: 36,757 ticks at 7, tick 1916 to 38,673). With control 4
pulsed on player 0, yes, with all four slots at state 2 (c). Match end follows the `points_system.md` description: `0x0063ba8c`
0xb, `0x0063b914` = 3. `0x0063b90c` read 1 and then 2 (the spec expects 1). For the soak driver, an all-AI cell needs a scripted
control-4 press on player 0 between rounds. Whether that counts as "unattended" under L-155 (a) is a lead call. The press is
input, but scripted, not a person.

**3.** After 0xb on a warp-entered race the original moves on: phase byte 5, UI word 9, four frontend pushes. The spec's results
criterion (`0x0067eca4 == 3` and type 5) never held. The stack ended at depth 5, type 6, `0x0067eca4 = 0` for 78 s (c, wall 121.5 to 199.5) and 69 s (e, wall 340.2 to 409.4).
Whether type 6 is the Race Results screen of this route is [UNCERTAIN]. Missing: one menu-entered match (statenav qb with pulses)
to compare the post-match push sequence.

**4.** Open. Neither TimeTrial run produced 0xb. With state 2 the UI word reached 7 at tick 2407 after 0.95 units of motion.
What wrote 7 there is [UNCERTAIN], and no lap was completed. Missing: a mode-2 run with control-4 pulses and the lap counter read.

**5.** 4,822 ticks from race anchor to match end in c (6 rounds, 83.0 s wall at the 60 fps cap). 5,883 ticks in e (5 rounds).
Per round: 460 to 940 ticks of racing plus about 120 ticks from 7 back to 6 at the 1 Hz pulse spacing (100 ticks with control 4
held, u). The spec's estimate of 5 to 8 minutes per cell is about 1.4 minutes at the cap here. Uncapped not measured.

## Proposed follow-up rows
- `SOAK-P0b` (harness, original only): `--watch 0x007f1038` on a_base and b_ai4 (closes 1). One menu-entered QuickRace with
  pulses (closes 3). Mode 2 with pulses plus the lap counter (closes 4). One c run with `MASHED_FPS_CAP_RACE=0` (closes 5 uncapped).
- Spec update for `SOAK-RUNNER`: an all-AI cell needs a control-4 press on player 0 at each round end (`0x0063ba8c == 7`).
  The match end criterion stays `0x0063ba8c == 0xb`. The results criterion needs revising (type 5 never appears after a warp).

## Raw data (D:/MashedWT/SOAK-P0_data, SHA-256)
```
e804e99e1154128f30ba791c8470fb0b4b12d89a705f0d779d6db10f6f14f889  b_ai4/probe.jsonl
552bd73a31b1f21c3062078c9a7767e9b69875da6b5a410ee9027744c584b589  b_ai4/meta.json
efc65c8337a8aab3a6130ce7000d05a8fa1d85c26401740f4628f3e1c1e59901  b_ai4.log
0d7211888cc18a0d33aec45e63901ba15fece34948be5d801432689bf7052d3e  a_base/probe.jsonl
bc68de1573a2c3dd820651d1f0c90874454f8bb58bee8c592a4a5517da018a68  a_base/meta.json
4337e9bb4462aaac1b74d8042c2b5618d29de8b76281c5d488b97cbfc621b43c  a_base.log
18de7f5cea6816fc1cefbd2ae73a082a4164b888e300bc8e13e7c554bead57e7  c_ai4_pulse4/probe.jsonl
0ac180db49e7c5b7c6f5637b3a535448c0c5d9ae60cd7cc9209e09c67c9125ff  c_ai4_pulse4/meta.json
6897f99d04b13a13e8bcb52db399d54d9fd0049c9d3c697a8c8e66148840414e  c_ai4_pulse4.log
92ca5bf8a8cd646f393942468a928ade4672d12ebdd4f6902d229d279e54a54a  e_human_pulse4/probe.jsonl
6c51d008a2abf039830d873d9af16e0b07722eef6105bb4717d2dd8b93e79644  e_human_pulse4/meta.json
d2592308ee363b1ace8c7040d5cf5ed6c6f0c41ba0520f1cb2bd0ef9e1c216b6  e_human_pulse4.log
d57c55cdc2218839ebb54b94f1927984d41222f7a4f4da25f8298aee4122db22  d_timetrial/probe.jsonl
a1921e92be8b5cc0e43f7bd8600a2862832d9a923bbbfae39987d216757abce6  d_timetrial/meta.json
3f137529dd73d17fd00d7fb860045dd256d0263ecabe104aa270f873f862ca9f  d_timetrial.log
5d89b796655560c24be33146a3d87f3ad1222ecae3543611cb5c1e88e3cd525d  d1_tt_human/probe.jsonl
2efe889cc6581820ccdd7bed600a82f87f3362c41fd51428c659c04bf0a4673f  d1_tt_human/meta.json
a614486ba17d8e6c84124c88c6b642a2611855bd554a0759fb758f2d0ac6aa0e  d1_tt_human.log
ac7d064df88674c49134a84ab6040a0df8fbefc2688a0bb9f74ba4175c1dbbca  a2_base_cin/probe.jsonl
cdb2a04d84b85c2cfd908630f4119da8f14a7fe6cac3c9c88027aac4c11c3a39  a2_base_cin/meta.json
db50eba038929876a298b274a30e039f73ab502771ca95d791a3bb7aa5164d79  a2_base_cin.log
4e842381a173ab3a6b8f78630f04585c8a8529b3af1cd22a71a6d0777eaffa38  b2_ai4_cin/probe.jsonl
c5670dd03dcaa8d306ab41e772f10fd7d6bd158d0f49839b84d9b52f85e4fe1e  b2_ai4_cin/meta.json
63e6e800fd535210919d9b265959e9f39e67c64f4c5505bcb6e0b1ca66fcf1a7  b2_ai4_cin.log
b63684a01585aaf4027010b1de0064887e97fd4969f72c34b4f5c54e1dedeca3  u_unedited/meta.json
40f11c6bcadee5a84022d79f1a854e93574bc0482b52c329cb11b017395bcea3  u_unedited/ticks.bin
5118d09d8d1de02f764683d81dc979ad5a3b67835d7f64e61cf4e77e5c1e2a50  u_unedited.log
```
Summaries: `py -3.12 verify/soakp0_20261114/probe_summary.py <run>/probe.jsonl`, `py -3.12 verify/soakp0_20261114/u_decode.py D:/MashedWT/SOAK-P0_data/u_unedited`.
