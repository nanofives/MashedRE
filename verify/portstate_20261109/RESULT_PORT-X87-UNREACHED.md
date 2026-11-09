# RESULT PORT-X87-UNREACHED: the original's CW on the audit's unreached PC-sensitive rows

Verdict: **GREEN.** Measurement only, no `mashedmod/` change, so no knob and no PREREG for a port fix.
Lead cmw7xu1960f37p41557tefpnd. Branch `orch/PORT-X87-UNREACHED` from `orch/PORT-X87-PC24-FLIP` `125bab35c` (L-105 (a):
game thread at PC_24 after device creation by default, opt-out `MASHED_NO_X87_PC24=1`).

## 1. Answer

- **Scope:** the audit's 18 matched rows marked `undetermined (not reached)` (17 RVAs, 0x00427ad0 has two TUs), plus the
  13 PC-SENSITIVE transcription rows it also never reached. 30 RVAs in all.
- **Original:** 20 scenarios reached **16 of the 30**: 9 of the 17 matched RVAs and 7 of the 13 transcription rows.
  **Every call to every reached row ran at CW 0x007f** (PC_24). No 0x027f and no other value, over 649 792
  calls on these rows.
- **Exe (L-105 default):** among the 30, the exe calls **only 0x00451ce0** (`port=called`). It runs at **0x007f** on
  Warzone, 3519 calls, the same CW as the original (3134 calls on its Warzone run). It also reads 0x007f with
  `MASHED_NO_X87_PC24=1` (3570 calls), because `Track/WarzonePontoon.cpp` holds a local PC_24 scope around it. The other
  29 rows are `port=body` or `none` (hooks.csv): 25 of them are in the exe image and got 0 calls on Warzone and Training.
  Their mode once called follows from the reached rows: every one of them runs after device creation in the original at
  0x007f, which is what the L-105 game thread gives.
- **Not reached by any existing scenario: 14 rows.** They are listed in section 3 with the call sites that a scenario
  would have to reach.

## 2. Reached rows

orig CW = `X87UNREACHED_CW_ORIG.tsv` (n 0x007f / n 0x027f / n other per scenario). Port column = hooks.csv today.

| RVA | name (TU) | port | scenario that reached it | orig calls @0x007f | exe |
|---|---|---|---|---|---|
| 0x00451ce0 | WarzonePontoonSample (MatchS09) | called | wz | 3134 | **0x007f x3519** (Warzone, default); 0x007f x3570 opt-out (local scope) |
| 0x0042bde0 | HudRectEmitter5 (MatchS07) | body | pb2 (Multi Player screens) | 453 | not called |
| 0x0042f8d0 | MenuMenusBC (MatchS07) | none | qb, champ (Race Results) | 6845 / 2485 | not in image |
| 0x00427ad0 | MenuMenusBB (MatchS06, MenuMenusBB) | body | pb2, cup2 | 454 / 2804 | not called |
| 0x00427ff0 | FontText_DrawTextRotated (MatchS06) | body | cup (challenge select) | 112 | not called |
| 0x004295a0 | HudDualLabelRender (MatchS06) | body | cup2 (Silver cup event 2, rule 10) | 551 | not called |
| 0x00429620 | FUN_00429620 (MatchS06) | body | qb, champ | 123 / 176 | not called |
| 0x00429660 | FUN_00429660 (MatchS06) | body | qb, champ | 123 / 176 | not called |
| 0x00411350 | Replay::TimeFormat (MatchS02) | body | tt (Time Trial) | 3229 | not called |
| 0x00577be0 | RwpSolverCore14 (transcription) | body | wz, qb, champ, t1..t12 | 7..171 per run | not called |
| 0x00577ec0 | RwpSolverCore14 | body | wz, qb, champ, cup2, t1..t9, t12 | up to 29248 | not called |
| 0x00578b20 | RwpSolverCore17 | body | as above, plus attract | up to 92854 | not called |
| 0x00578cb0 | RwpSolverCore17 | body | as 0x00577ec0 | up to 30999 | not called |
| 0x00578d90 | RwpSolverCore17 | body | t5 (Sands) | 2 | not called |
| 0x00546bf0 | RwpSolverMath2Quat | body | t1, t2, t6, t12 | 1..127 | not called |
| 0x00546cb0 | RwpSolverMath2Quat | body | t2, t6, t12 | 2..108 | not called |

"Not called" = 0 calls in the exe on Warzone (3600 frames) and Training (1200 frames) with the stub attached, and
`port` != called in hooks.csv. The exe run used the L-105 default build `4cc8b92b` (log: `[x87] PC_24 after device:
tid=16324 pc 0x10000 -> 0x20000`, `drift=0`).

Rows seen at 0x027f anywhere in these runs (`X87UNREACHED_CW_ORIG_ALL.tsv`): 0x00498910, 0x00498950 (config load,
0x027f only) and 0x00499820, 0x004963e0 (both CWs). These are the audit's pre-device rows, read again by the
frida-spawn harnesses that attach before the device exists. None is in the scope above.

## 3. Rows no existing scenario reached (14)

Call sites from an E8/E9 scan of `MASHED.exe.unpatched` .text. Caller names from hooks.csv. "Needs" = the call site a
scenario would have to reach. Where the gate on that call site is not traced, it says so.

| RVA | name | port | call sites (caller) | needs |
|---|---|---|---|---|
| 0x0044e070 | FUN_0044e070 (MatchS08) | none | 0x0046172f (FUN_00461650, "in-world item proximity SFX", audio) | FUN_00461650 running. [UNCERTAIN: whether the `skip_audio_com` boot patch at 0x005bc750 keeps it from running. A copy with that patch restored would tell] |
| 0x0042fb70 | MenuSpriteDispatchB (MatchS07) | body | 0x0043bfc5 (FUN_0043bf30, frontend sub-menu dispatcher, 14 calls gated on flags 0x0067e7a8..0x0067e838) | the screen that sets its flag. [UNCERTAIN: flag writer not traced] |
| 0x0042fe90 | MenuSpriteDispatchC (MatchS07) | none | 0x0043bfd3 (FUN_0043bf30) | same as above |
| 0x00430b90 | ProgressBarSetA (transcription) | none | 0x0043bf7f (FUN_0043bf30), 0x004932ca (FUN_00492e90, game logic tick) | the FUN_0043bf30 flag, or the FUN_00492e90 branch at 0x004932ca. [UNCERTAIN: gate not traced] |
| 0x00482030 | SubStripQuadUV (MatchS13) | body | 0x004825d2 / 0x004825ec / 0x00482602 (FUN_00482140, collision mesh strip builder, one caller FUN_004826d0) | a run that reaches FUN_004826d0. Not reached on any of the 13 tracks. [UNCERTAIN: FUN_004826d0's trigger] |
| 0x00427990 | FUN_00427990 (MatchS06) | body | 0x004341b0 (MenuModalRender 0x00433f40, the posted-modal renderer) | the posted modal whose branch reaches 0x004341b0. The Load-Successful modal on the Quick Battle route does not |
| 0x00428bf0 | TitleBuildInfoRender (MatchS06) | body | 0x0042925f (FUN_00429240, `switch(DAT_0067d960)`, case 1) | DAT_0067d960 == 1. [UNCERTAIN: its writer] |
| 0x00429870 | LapTimeALessThanB (MatchS06) | body | 0x0042ca28 (FUN_0042c960, Race Results draw-list build), 0x00435267 (FUN_00434720, championship / cup progress screen) | the cup progress screen after a concluded cup event (cup2 ran 90 s and did not conclude), or the 0x0042ca28 branch (Race Results was reached on qb and champ, that site was not) |
| 0x00429b70 | Init429b70 (MatchS06) | body | 0x00429c79 (FUN_00429bd0, caller FUN_00429e10, the race overlay) | FUN_00429bd0's branch at 0x00429c79. cup2 ran FUN_00429e10 (0x004295a0 is its callee) without reaching it. [UNCERTAIN: gate] |
| 0x0055bae0 | RwpSolverBroadphase3 (transcription) | body | FUN_0055bb70 x3, FUN_005729a0 x2 | RW physics paths not hit in 20 race runs |
| 0x00575b60 | RwpSolverCore17 | body | 0x00575b3c (FUN_00575880) | as above |
| 0x0057a250 | RwpSolverCore17 | body | 0x0057abd4 (FUN_0057a9a0) | as above |
| 0x0057a660 | RwpSolverCore17 | body | 0x0057ab0c (FUN_0057a9a0) | as above |
| 0x00566200 | RwpSolverLeaves1X87 | body | 0x005764a1 (FUN_00575fe0) | as above |

None of the 14 is `port=called`, so no default-build behaviour depends on them today.

## 4. Scenarios (original, stock: `MASHED_RE_NO_AUTO_HOOK=1`, nothing loaded)

Each run: `x87_cw_attach.py` starts the harness, attaches to the pid it prints (explicit pid, never by name), arms the
audit's native FNSTCW stub on all 161 inventory RVAs (0x00496400 skipped: `disable_log` patch region; 161/161
attached on every run) and keeps the last counter snapshot. Root `D:/MashedWT/PORT-X87-UNREACHED_root` (copy), under
test-slot and `mashed_lock.py run`.

| tag | harness | what it reached |
|---|---|---|
| wz | `scenario_launch.py --track 11 --rule 8 --cars 4 --hold 60` | Warzone race, pontoon |
| attract | `c4_hud_scenario.py --row 403160 --arm stock` | title idle, attract demo race |
| qb | `c4_detect_e_scenario.py --arm stock` | Quick Battle round, natural Race Results |
| pb2, pb2b | `c4_pb2_scenario.py --arm stock` | Multi Player / Team Play screens. Both stopped at `NAV-TEAMSEL-FAILED` before a race |
| champ | `c4_champ_complete_scenario.py --root <copy> --arm stock` | challenge 0 to its conclusion (`verdict=CALLED`) |
| tt | `scenario_launch.py --track 0 --mode 2 --cars 1 --hold 60` | Time Trial |
| t1..t10, t12 | `scenario_launch.py --track N --cars 4 --hold 25` | every other track (t2 re-run once: the first attach timed out) |
| cup | `c4_hud_scenario.py --row 4295a0 --arm stock --route cup --root <copy>` | stock save: the cup selector fell back, rule-0 race, 0x004295a0 not reached |
| cup2 | the same with `--reset-save gamesave_cupgates.bin` | Silver cup event 2, rule 10, `verdict=DONE calls=594` |

`gamesave_cupgates.bin`: the 2026-10-12 user-approved save-copy edit (`re/analysis/negctl/session_20261012b/
c4save_RESULT.md`): the dwords at +0x08 and +0x14 of the 13 rows at 0x24A40 (stride 0x30) set to 1, in a copy of the
stock save (`bd187881...`). It is forced state: it shows the row's CW once that event runs, not that a stock save
reaches it. `original/` was not written. For `cup` / `cup2` the 0x004295a0 stub was armed 30 s after the attach
(`--late`), because the harness checks that entry is unpatched before it arms its own counter.

Exe probe: the audit's `x87_cw_probe.py --target exe`, stubs on the 26 focus symbols present in the map, Warzone
(`MASHED_TRACK_VIEW=Warzone`, rule 8, 3600 frames) and Training (rule 0, 1200 frames), both self-exited.
`X87UNREACHED_CW_EXE_WZ.tsv`, `X87UNREACHED_CW_EXE_WZ_OPTOUT.tsv`, `X87UNREACHED_CW_EXE_TR.tsv`.

## 5. Files

Committed (`verify/portstate_20261109/`): `x87_cw_attach.py` (the attach probe), `x87_cw_attach_score.py`,
`X87UNREACHED_CW_ORIG.tsv` (the 30 rows), `X87UNREACHED_CW_ORIG_ALL.tsv` (every RVA reached anywhere),
`X87UNREACHED_CW_EXE_*.tsv`, this RESULT.

Raw (`D:/MashedWT/PORT-X87-UNREACHED_data`, not committed):

| file | sha256 |
|---|---|
| cw_wz.json | aeb133fb2307c211e7d15f24d48469c295e83dfae737425a663876615c6557f8 |
| cw_attract.json | 5d55287bd67ebaba5072db960e294738c44c641c28beb29adb55e6431780d6ba |
| cw_qb.json | ddc221b6206a6364bfc5c25ffff4460ba4fe870555cf3f2a2a68b064549c6e2d |
| cw_pb2.json | f24374b14b9f99210acde0bbe2e5e5d610a3283c9ef4788daf4f8508416e8dc0 |
| cw_pb2b.json | 89fe3d3d5bb6ea7261314ccc1add9bb2e64a55148087f069a80924631d772cc6 |
| cw_champ.json | ead064ac3643eb822f06f927b23ee5c159cc2f119991349227ec7bc69bb803e4 |
| cw_tt.json | 1d2f4e5390552445abea0ab85c3cd05afec23027c70991391e5d47afc28e787a |
| cw_cup.json | 7b127c2181efd13f305d7ee3de289841462fe9cc8d4a1b5a3856fdae5c62b4b8 |
| cw_cup2.json | 6ae8ed01a81b96ee26749faca07ae9eaaeed6af4f901d68a408b5544242c76c4 |
| cw_trk1.json | f6eaf559f8fe30ccfd821db26d1c9aca0624e69f4e5c60aa93350835ceb56e8c |
| cw_trk2.json | 119e039485966dd84d12ac8125f60e94e682115b217d63796d52a57643bc0add |
| cw_trk3.json | c24a4b45a7095162b5991c7825f99f5064fa69ae9963d3d898c9b59951cfd3a0 |
| cw_trk4.json | ee414d4c2f5e64f672e0fbff259e9dea68e054305989b8d0008e2d532f1f0c98 |
| cw_trk5.json | 91256c7257701f1c8ba4af0aabeb58a14ba6227252835ce4ef40fe0d16a1c1d8 |
| cw_trk6.json | aaa37e85733996b2efd6b7de559c05a7810ffbc9cf3caf2cee0023cdf4dbe0a0 |
| cw_trk7.json | 7ada133b19788041134b606b2a5556034de60b503eda31eff71f4f0b3863bae7 |
| cw_trk8.json | 1b5acd00dbbd39d1feaebb904908713438afc3cace82eb9a4837ab55f57ee080 |
| cw_trk9.json | 0f27d703760dd2b736eeeee2e998171c443053189c0f55bf338557c7f24e5ee7 |
| cw_trk10.json | e1e2106497c639e88e3d7090c24f25453c830c091824fe5d58812952743a6375 |
| cw_trk12.json | 4ac423f2b99d404d665e9048facf20f7d1a8520b960ffec6623404819eb0e531 |
| cw_exe_wz.json | a51a0c75c4e3ec14b5856341be64dad900e99bce22c3c1ccc223bae6c5c9ea21 |
| cw_exe_wz_off.json | 94a66bf8f96f1ef4678b1bdd667b06ad9b2ee3bb1da65630282d3187d9f63d85 |
| cw_exe_tr.json | 6f35e57e647e249e45206184c16bbb1a4e5ccf1590e429c953966e2f95028f58 |
| exe_wz.log (`[x87]` / `[cw]` lines) | 42edba7c5bb20da64a366f11610e633031e75683a1745ed90c3b0093c44eec63 |
| gamesave_cupgates.bin | 80ccb2d449f0e5b75e57e2b74ad7a3c9f886fc87f4eb6c74185016402cbe2032 |
| mashed_re.exe (worktree build, `MASHED_NO_DEPLOY=1`) | 4cc8b92bc92c7bda9a5d0565e059e80b08e9bfb005786510f28e52ec1ad56f58 |
| build.log | 4c9f974501d2d96011d614d90060de1328e1512b9bc8dc6a17493e7caf2a2780 |
| root MASHED.exe (patched copy) | b9977dab70607720cf58bc7832937c203b1e0b33ff5896da586baf56aa79fe87 |

Harness logs and per-harness JSON: `batch2.log`, `batch3.log`, `batch4.log`, `cw_cup.log`, `cw_cup2.log`, `h_*`.
No piz-collision crash occurred. Gates: not run (no `mashedmod/` change).
