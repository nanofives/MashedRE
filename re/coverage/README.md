# Coverage census (one-shot INT3 reach map)

Plan item #4 of the SimGolf technique port (`plans/do-you-think-we-foamy-pelican.md`). It is modelled on
SimGolf `re/tools/coverage_census.py` and `scripts/scoreboard.py`.

| File | Producer | What it holds |
|---|---|---|
| `reach.tsv` | `py -3.12 re/frida/coverage_census.py --runs 3` | one row per hooks.csv RVA: `rva, reached_menu, reached_race, reached_race_load, reached_race_run, runs_menu, runs_race, va` |
| `scoreboard.tsv` | `py -3.12 scripts/scoreboard.py` | R / U / V axes and `priority = R*((3-U)+(3-V))` per hooks.csv row |

`re/analysis/plans/c3_filter_v4.py` reads the scoreboard. It adds `reach` and `reach_priority` columns to
`<prefix>_passed.tsv` and sorts by priority. `--reached-only` drops R=0 candidates. The ranking moves no
C-level. reach.tsv is evidence about the ORIGINAL only: it says nothing about any port.

## Method

- The script spawns `original/MASHED.exe` suspended (`dev.spawn`, as in `canonical_c4_*.py`) with
  `MASHED_RE_NO_AUTO_HOOK=1`, so no inline-JMP hook shares a first byte with an INT3. It attaches to that
  explicit PID, writes `0xCC` at all 5,888 distinct entry VAs and resumes. The VAs come from 5,901
  hooks.csv rows: 13 duplicate a VA, and 29 are stored base-relative and get `+0x00400000`.
- A `Process.setExceptionHandler` callback (the `poll_attach_catch_crash.py` style, no Interceptor) runs
  on the first hit. It restores the original byte, sets EIP back to the entry and records the RVA, so each
  site costs one exception per phase.
- Phases: `menu` covers spawn to `DAT_00771968 == 1`, plus 20 s at the main menu. For `race_load`, one
  RPC re-arms every site and pokes `DAT_00771968 = 2`. This is the `scenario_launch.py` warp: track 0,
  mode 10, 4 slots with slot 0 player and 1..3 AI. `race_run` re-arms at phase 3 and holds 45 s. No
  input is injected. `reached_race = race_load OR race_run`. A row counts as reached when a majority of
  runs hit it.
- Pre-flight: the anchor SHA-256 of `MASHED.exe.unpatched` is checked. Every site was checked to be inside
  an executable section. Before arming, every site was checked to be an instruction start: 5,809 are in
  `function_inventory.csv`, 1 is a call target, 86 follow padding or a RET, 1 (`0x00493ac0`) follows an
  unconditional JMP, and 4 are data-referenced.
  The boot-patch sites `0x004951f0`, `0x004a4541`, `0x00495870`, `0x00498bc0`, `0x00498bd0`, `0x005bc750`
  and `0x00496490` are all function entries, and the INT3 saves the patched byte.

`MASHED_RACE_DEMO=1 MASHED_GOTO=6` is the standalone's race driver (`exe_main.cpp:1449`), so it cannot
drive MASHED.exe. The original-side race uses the warp instead.

## Result, 2026-10-11 (3 runs, transcript `re/coverage/census_2026-10-11.txt`)

| phase | run 1 | run 2 | run 3 | union | intersection | disagreement |
|---|---|---|---|---|---|---|
| menu | 1640 | 1640 | 1640 | 1640 | 1640 | 0.00% |
| race (load or run) | | | | 2302 | 2301 | 0.04% |
| race_load | 1984 | 1871 | 1917 | 1984 | 1871 | 5.70% |
| race_run | 1275 | 1274 | 1273 | 1278 | 1271 | 0.55% |

Gate "3 runs agree within 1%" is PASS on both scenarios (worst 0.04%). The race_load figure alone is
a boundary effect, not instability: race_load and race_run split at the 100 ms phase-3 poll, so the
first race frames fall on either side. The race union is stable. All runs reported 0 clobbered sites
and 0 write errors, and none crashed.

Reached: 3,134 of 5,888 entry VAs, which is 3,145 of 5,901 hooks.csv rows. The scoreboard axes are
R `0=2756 1=1755 2=1075 3=315`, U `1=821 2=3883 3=1197` and V `0=4645 1=59 2=1021 3=176`. Of the
reached rows, 806 of 3,145 (25.6%) are C3+.

Reached C2 rows: 1,918. By subsystem:

| subsystem | reached C2 |
|---|---|
| render | 442 |
| audio | 259 |
| util | 194 |
| third-party-library[msvc-crt-main] | 162 |
| third-party-library[lua-4.0] | 128 |
| boot | 125 |
| particle | 109 |
| gameplay | 103 |
| vehicle | 61 |
| third-party-library[renderware] | 48 |
| frontend | 44 |
| track | 37 |
| third-party-library[qhull-2002.1-continuation] | 32 |
| hud | 31 |
| third-party-library[qhull-2002.1] | 25 |
| ai | 24 |
| third-party-library[RenderWare-Physics-3.7] | 20 |
| input | 15 |
| camera, unknown, d3dx9-backward-extension, physics | 8, 7, 7, 7 |
| powerups, msvc-crt-tail, video | 6, 5, 4 |
| debug-overlay, save | 3, 3 |
| world-objects, sky, d3dx9-fx | 2, 2, 2 |
| msvc-crt, smplfzx, d3dx9-shader-compiler | 1, 1, 1 |

## Static spot-check of 5 "unreached" rows

Sampling: one C2 row with R=0 per subsystem in gameplay, frontend, render, vehicle and audio, drawn with
`random.seed(20261011)`. Callers come from an E8/E9 rel32 scan and a dword scan of the non-executable
sections of `MASHED.exe.unpatched`.

| RVA | Static callers | Verdict |
|---|---|---|
| `0x00451990` (gameplay) | 5 calls in `FUN_00451ad0` (`0x00451b26`, `0x00451bba`, `0x00451bf1`, `0x00451c28`, `0x00451c5f`) and 1 in `FUN_00452140` (`0x0045214f`). Both callers are themselves unreached. | consistent |
| `0x00432450` (frontend) | 2 calls in `FUN_004324a0`, which is reached. `0x00432548` runs only if `[0x007f1a0c] != 0x1000` (`0x004324e9`), `[0x0067eab0] != 0` (`0x004324f5`), `[0x0067eabc]` is neither `0xff210000` nor `0xff220000` (`0x00432510`, `0x0043251c`) and `FUN_0042c220` returns nonzero (`0x00432532`). `0x004325ab` runs only on the `[0x007f1a0c] == 0x1000` path, where `FUN_0042b930` must return neither `0x21` nor `1` (`0x00432556`, `0x0043255b`), `FUN_0042c1f0` must return nonzero, `[0x0067eab0] != 0` and `[0x0067eabc] != 0xff220000` (`0x0043258d`). | consistent: both sites are guarded |
| `0x004f1800` (render) | Its only caller is the stub at `0x004e4240`. That stub's only reference is `push 0x004e4240` at `0x004e359d`, the 4th argument of `FUN_004e8f80(0x510, 0x004e4220, 0x004e4200, 0x004e4240)` at `0x004e35b1`. `0x004e4240` is not a hooks.csv row, so it is not a census site. | consistent, not provable: the census does not instrument the caller [UNCERTAIN] |
| `0x00482930` Replay::New (vehicle) | 2 calls in `Replay::CreateOrLoad` `0x00411d90`, which is reached. The function body runs only if `FUN_0042f6a0() == 2` (`0x00411dad`). `0x00411e21` additionally needs `[0x0063bb04] == 0` (`0x00411e1b`). `0x00411efb` is on the `[0x0063bb08] == 0` path (`0x00411e91`). The third caller, `Replay::Load` `0x00483d10`, is unreached. | consistent: guarded |
| `0x005b3540` (audio) | Its only caller is the stub at `0x005a9180`. It calls only if `[0x007dcbec] != 0` (`0x005a9187`). Its only reference is the dword at `0x00632c3c` (`PTR_LAB_00632c3c`, consumed by `FUN_005ad080` per `re/analysis/bucket_audio_00465c10_005a7b50/005a6150.md`). `0x005a9180` is not a hooks.csv row. | consistent, not provable: the caller is not instrumented [UNCERTAIN] |

No spot-check contradicts the census. Two of the five show a structural limit: the census only
instruments hooks.csv rows. A function that appears only through a pointer to a non-row stub has no
witness for its caller, as with `0x004e4240` and `0x005a9180`.

## Not covered by these two scenarios

- Frontend screens other than the main menu: car and track select, options, championship, save/load and
  results.
- The player driving. Slot 0 sits parked with no input.
- Race end, pause, multiplayer, and any mode other than 10 on track 0.

R=0 means "not executed in these scenarios", not "dead".
