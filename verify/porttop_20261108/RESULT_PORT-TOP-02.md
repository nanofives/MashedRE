# RESULT PORT-TOP-02: MASHED_ORIG_TOP knob scaffold (L-96 top-down lane, PLAN section 7)

Verdict: GREEN. With MASHED_ORIG_TOP=1 the exe boots to the main menu through the matched WinMain 0x00492370 and run function 0x00492290, and exits cleanly through them. Default gates unchanged. Of the 23 level-1 callees (the run function counted), 8 are matched, 2 port, 8 still stand-ins, 4 missing stubs and 1 an RW stub (replacement order below).

## What the knob does

`MASHED_ORIG_TOP` (default OFF, read once at the top of the exe `WinMain`, `exe_main.cpp`) hands control to the
byte-matched original WinMain `0x00492370` (`re/match/s15/00492370.c`, match.py 100%) and its run function
`0x00492290` (`re/match/s15/00492290.c`, 100%), compiled verbatim (MATCH_PORT, `extern "C"`) in the new exe-only
TU `Match/PortTop_00492370.cpp`. With the knob OFF the default path is the same call sequence as before.

Scaffold, all exe only (`exe_sources.rsp`, not `asi_sources.rsp`, each file `#ifdef MASHED_STANDALONE`):

| file | content |
|---|---|
| `exe_main.cpp` | hand WinMain split into `HandTop_Prelude / _CreateWindow / _Smoke / _InitDevice / _InitRest / _FrameDetTick / _FrameUpdate / _AppDestroy / _DeviceShutdown`. The default WinMain calls them in the old order. An `extern "C"` block exports the stand-ins (`PortTop_Hand*`). The one-time env read of `MASHED_ORIG_TOP` sits at WinMain entry |
| `Match/PortTop_00492370.cpp` | matched `0x00492370` + `0x00492290`, every callee bound by name (`#define FUN_<rva> <exe symbol>`), stand-in / missing stubs, the knob entry `PortTop_WinMain`, the `.rdata` string seed, per-step log lines `[ORIG_TOP] ...` in `mashed_re.log` |
| `Match/PortTop_00499ba0.cpp` | matched window create `0x00499ba0` (s16 h6) |
| `Match/PortTop_00499820.cpp` | matched window procedure `0x00499820` (s16 h6) + its 3 callee stubs |
| `Match/PortTop_00499890.cpp` | matched OS-version log `0x00499890` (s16 h6), links advapi32 |
| `Match/PortTop_00493900.cpp` | command-line switch parser `0x00493900` (s15 h8, best 66.9%, the only source, hooks C2) |
| `Match/PortTop_Leaves.cpp` | matched `0x00499cc0` (s16) |
| `Match/MatchS15_00492d20.cpp`, `Match/MatchS16_00499730.cpp` | existing asi wrappers, now also in the exe list (rva-lint: one body per RVA per target). `MatchS15_00492d20.cpp` gets a `MASHED_STANDALONE` branch binding its `0x004967e0` forwarder to the stand-in instead of the raw address |
| `Match/PortTop_RdataSeed.inc` | 16 original strings (`0x005cf6b4..0x005d041c`) the compiled bodies read, generated from `original/MASHED.exe.unpatched` (anchor checked) by `verify/porttop_20261108/gen_rdata_seed.py`. Written under the knob only: the exe maps the original data range zero-filled (`g_b17_low_arena_pad`) |

Two knob-path rules beyond the binding order:
- Focus gate: the reference runs with the `no_focus_pause` patch (`0x004996d3`), and the hand pump bypasses the same
  gate. The knob path keeps `DAT_0077391c = 1` so the matched pump `0x00499690` never parks in `WaitMessage`.
  `MASHED_ORIG_TOP_FOCUSPAUSE=1` keeps the unpatched behaviour.
- `MainLoopInit 0x00492770` is called through an SEH guard (`PortTop_MainLoopInitGuarded`), the same guard the hand
  boot chain step 7 uses, because its 5 callees still forward to raw original addresses (PORT-TOP-01). Remove the
  guard when PORT-TOP-01 lands. I did not edit `MatchS15_00492770.cpp`.

## BINDINGS.tsv summary

`verify/porttop_20261108/BINDINGS.tsv` (260 rows, generator `gen_bindings.py`; columns rva, name, kind, exe_symbol,
note, level, caller; levels count from the two roots `0x00492370` and `0x00492290`).

| level | matched | port | stand-in | missing | rw | under-stand-in |
|---|---|---|---|---|---|---|
| 0 (the two roots) | 2 | | | | | |
| 1 (15 WinMain + 8 run callees, `0x00492290` counted at level 0) | 7 | 2 | 8 | 4 | 1 | |
| 2 | 3 | 31 | 3 | 5 | 8 | 138 |
| 3 (compiled subtrees + RW inventory) | | | 2 | 2 | 44 | |

Level 1 in full:

| rva | kind | exe symbol |
|---|---|---|
| `0x00499ba0` window create | matched | `Orig_00499ba0` |
| `0x00493900` cmdline parse | matched (66.9% source) | `Orig_00493900` |
| `0x00493540` gate -> `0x00495150` | missing | stub returns `DAT_007719e4 == 0`, as `0x00495150` |
| `0x004924f0` data init | stand-in | hand boot chain steps 1-3 (inside the `0x00402750` stand-in) |
| `0x00492270` SubsystemInit | stand-in | `HandTop_InitDevice` (InitD3D9 + librw RaceSubmit_Init) |
| `0x005c9d00` | port | `GetRaceEndTrigger` |
| `0x004996f0` WindowShow | matched | `WindowShow` |
| `0x004963e0` config log | stand-in | `PortTop_CfgLogError` (the exe body is file-internal in `Save/SettingsConfig.cpp`; reference no-op under disable_log) |
| `0x00402750` AppInitialiseOnBootup | stand-in | `HandTop_InitRest` |
| `0x00402a40` AppDestroy | stand-in | `HandTop_AppDestroy` |
| `0x004c5930` RW texdict destroy | rw | logged stub, not reached (`DAT_0077195c` stays 0) |
| `0x00493550` -> `0x004938c0` engine stop | stand-in | `HandTop_DeviceShutdown` |
| `0x00493560` -> `0x004954f0` hardware exit | missing | stub returns 0 |
| `0x00499cc0` | matched | `Orig_00499cc0` |
| `0x00492770` MainLoopInit | matched (guarded) | `MainLoopInit` |
| `0x00499690` WindowMsgPump | matched | `WindowMsgPump` |
| `0x004929d0` game-mode machine | port | `GameModeMachine_Tick` (the hand loop's call) |
| `0x00492d20` | matched | `IntroSplashFrameTickShim` (`0x004967e0` -> stand-in `PollKeyboard`) |
| `0x00492d30` GameTickStateMachine7 | stand-in | `HandTop_FrameDetTick + HandTop_FrameUpdate` |
| `0x00492e90` frame render | stand-in | `RenderFrame` |
| `0x004926c0` AudioTickAndAvg | missing | no-op (exe body forwards its 5 callees raw) |
| `0x00493480` FpsDiscretise | missing | no-op (exe body forwards raw to `0x00493390`) |

Matched exe bodies that exist but cannot be bound as-is because their forwarders call raw original addresses (fault in
the exe, 6 thunks only): `SubsystemInit 0x00492270`, `thunk_LaunchLangGate 0x00493540`,
`thunk_EngineStopDispatch 0x00493550`, `thunk_HwExitDispatch 0x00493560`, `AudioTickAndAvg 0x004926c0`,
`FpsDiscretise 0x00493480`, `DisplayInit 0x004921d0`, `MainLoopInit 0x00492770` (guarded). Each needs its forwarders
given an exe binding (a `MASHED_STANDALONE` branch, as done here for `MatchS15_00492d20.cpp`) once the callee has one.

RW (kind `rw`, 53 rows, PORT-TOP-03 input): levels 1-3 under the two roots, selected by hooks.csv subsystem
`third-party-library[renderware]` or an `Rw/Rp/Rt` name. Includes the `0x00493710` engine bring-up
(`RwEngineInit 0x004c32b0`, `RtFSManagerOpen 0x00550350`, `RwEngineOpen 0x004c30b0`, `RwEngineStart 0x004c2fb0`,
`RwGlobal7d4598/459c Get/Set 0x004cbc60..90`, `0x005584c0`), `RwEngineGetModeInfo 0x004c2ed0` /
`RwEngineGetCurrentMode 0x004c2f00`, the texdict destroy `0x004c5930` with `0x004c5a60` / `RwPluginListDestructDispatch
0x004d8060`, `FontCtxMatrix_AllocInit 0x004c57a0` (under MainLoopInit), and the AppDestroy world/clump teardown
(`RpWorldDestroy 0x004e5700`, `RpClumpDestroy 0x004e6e00`, `RwFrameRemoveChild 0x004e45b0`, `RpWorldRemoveLight
0x004e4860`).

## Knob-ON boot (evidence for step 4, not a gate)

All runs: worktree exe `mashedmod/build/mashed_re.exe` (commit `873e765b1`, sha256 `b5a7c15f...`), private root
`D:/MashedWT/PORT-TOP-02_rootK`, each run under `mashed_lock.py run`, own PID only. Driver:
`verify/porttop_20261108/knob_boot.py`.

1. `MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000` (`knob_on_1`): exit 0 after 3000 frames, 53.8 s. The `[ORIG_TOP]` trace
   shows each step in the original order: rdata seed 16/16, window create `0x00499ba0` (its OS log `0x00499890`
   writes "Operating System: Professional (Build 9200)"), gate `0x00493540` returns 1, `0x004924f0` stand-in,
   `0x00492270` stand-in (InitD3D9 + librw) returns 1, "Calling AppInitialiseOnBootup", `0x00402750` stand-in returns 1
   (`B17-SUMMARY reached-main-loop chrome=YES thunks=6/6`), `MainLoopInit` faults inside its guard (PORT-TOP-01), the
   run-function loop runs 3000 frames, then "Calling AppDestroy", "Calling SoftwareTidyUpBeforeExiting", "Calling
   HardwareExitApplication", `WinMain 0x00492370 returned 0`. The 4 "Calling ..." lines are the seeded original
   strings passed through the matched WinMain.
2. Natural flow, `MASHED_NAV_DEMO=1` (splash, title, press button, main menu, Single Player chain, back, Options,
   Sound, back), knob ON vs knob OFF: the same 13 `NAV_DEMO` steps with the same depth / screen / cursor on both
   paths (`02_mainmenu depth=0 screen=1`, `done ... screen=1 cur=2`), exit 0 on both.
   - With `MASHED_DETERMINISTIC=1`: all 12 captures pixel-identical knob ON vs knob OFF (0 differing pixels each,
     `walk_02_mainmenu.bmp` sha256 `24249b64...` on both). Control: knob OFF vs knob OFF also 0.
   - On the wall clock the captures differ (100k-200k px from frame 2 on), and knob OFF vs a second knob OFF run
     differs by the same amount (1542, 100314, 113520, 187609 ... px), so that is clock noise, not the knob.
3. `MASHED_PARITY=1` (direct goto of 17 screens): same 17 screens on both paths. 15 captures pixel-identical, `re_s6`
   / `re_s7` differ in one 49x51 px region and were captured at different frame indices (phase 332 vs 340, 398 vs
   406). Main menu `re_s1` identical.

Faults left on the knob path: one, caught. `MainLoopInit 0x00492770` faults in its first raw-address callee, inside
the SEH guard, on both paths (the hand boot chain step 7 logs the same fault). No other step faults.

## Default gates (knob OFF)

Exe `b5a7c15f` (commit `873e765b1`), `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`
under `test-slot` + `mashed_lock.py run --label PORT-TOP-02`, root `D:/MashedWT/PORT-TOP-02_root`, all arms exit 0
and self-exited.

| gate | this branch | merge_train.json ref |
|---|---|---|
| G-CTRL | `8284be46/67931834/31fe043e` | `8284be46/67931834/31fe043e` |
| B | `1d5bf4f9/1d5bf4f9/1d5bf4f9` | `1d5bf4f9` x3 |

Build: `mashedmod\build.bat` (MASHED_NO_DEPLOY=1), both targets OK. rva-lint: "OK -- no NEW duplicate bodies".

## Port column (backfill_exe_file.py then backfill_port.py)

11 rows changed, none dropped: `0x00492370 0x00492290 0x00499ba0 0x00499820 0x00499890 0x00493900 0x00499cc0`
(new exe bodies), `0x00492d20 0x00499730` (none -> called, shared wrappers now in the exe), `0x004996f0 WindowShow`,
`0x00499690 WindowMsgPump` (body -> called). `called` is the static reach from the exe WinMain through the knob branch.
At runtime these bodies execute only with `MASHED_ORIG_TOP=1`. rva-lint: OK, no new duplicate bodies.

## Next stand-ins to replace, in order

1. PORT-TOP-01 lands: drop the SEH guard on `0x00492770` (its 5 callees `0x00495110 0x0042b920 0x0042b950
   0x00433240 0x004c57a0`; `0x004c57a0` is RW, PORT-TOP-03).
2. PORT-TOP-03 (RW layer on librw): `0x00492270` -> matched SubsystemInit with `0x00493710` (RW engine bring-up),
   `0x004921d0` DisplayInit, `0x00428590` ViewportInit. Then `0x00493550` -> `0x004938c0` (RW stop/close/term) and
   `0x004c5930`. These replace `HandTop_InitDevice` / `HandTop_DeviceShutdown`.
3. Config log `0x004963e0` / `0x00496400`: bind the PORT-SAVE-01 matched bodies by name once that branch is merged
   (they are file-internal on this base).
4. PORT-TOP-04: `0x00402750` (57 callees) and `0x00402a40` (43) stand-ins out; with them `0x004924f0` (matched source
   s15, C3; callees `0x00431ae0/af0/b00` called, `0x00431b10` none, `0x00431d00` body), `0x00493540` -> `0x00495150`
   (callees `0x00495120 0x004955b0 0x00499710 0x004960a0 0x004963b0 0x00498510`), `0x00493560` -> `0x004954f0`
   (5 callees).
5. PORT-TOP-05: `0x00492d30` (callees `0x004030d0 0x004111c0 0x0043d7c0 0x0043dfd0 0x00496920 0x00498860`) and
   `0x00492e90` (26 callees), then `0x004926c0` (5), `0x00493480` (`0x00493390`), `0x004967e0` (C2).
6. `0x00498bc0` / `0x00498bd0` stand-ins (exe resolution): bind `VideoGetRenderWidth/Height` once `DAT_00616028/2c`
   hold the videocfg mode in the exe (data side, PORT-STATE-01 territory).

## Files, raw data

Committed: `BINDINGS.tsv`, `gen_bindings.py`, `gen_rdata_seed.py`, `knob_boot.py`, this file. Raw data in
`D:/MashedWT/PORT-TOP-02_data`:

| file | sha256 |
|---|---|
| gates_default.log | e1467eb31f577e162cc711438a578b610e3876cfdee00a055f271f9a9ba3dae1 |
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| knob_on_1.log | b0d0dca0a9f314cefba576ad2959e9ecebbcdf25a49f73f2659a9838eb6c304a |
| walk_knob.log / walk_hand.log | 1be3334f... / 14b0d016... |
| navdemo_knobdet.log / navdemo_handdet.log | 51538dc1fe6b21657d6acb6f0c96ea6c2ecc660319ec7b3c9275716eddcb67af / 8119cda3b34d4cc5d6a865c1e3f8f646f35fa04fddc22c370de999d5269177cc |
| navdemo_knob.log / navdemo_hand.log | a8b10fa0... / a879c8bc... |
| navdemo_{knobdet,handdet}/walk_02_mainmenu.bmp | 24249b644e3e8057271f3f1c252514c7820abec3f3da15d316f39dbf156a9066 (both) |
| mashed_re_873e765b.exe (gated build) | b5a7c15fb47b9e3d64dae9a31db603dd8d576983b5c29d2b788108fc5ff39fc7 |

Not done here: no tracker leg (no C-level moves; the port column moved by the backfill scripts). No ledger enqueue.
