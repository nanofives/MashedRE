# C4 re-earn: MenuButtonDetectE 0x0042b770 (canonical scenario, hook live)

**Verdict: C4 EARNED** (evidence only. No tracker edit and no re-classify in this branch. The parent promotes.)

Date 2026-10-12, branch `work/c4-detect-e` (base `3acbc094`). Port under test: the deployed
`original/mashed_re_dev.asi` (SHA-256 `5DAE5269523970FB38F83EE5F3CEF489C11314A38B12E3565C64EA9BC7539DA3`),
which carries the dc98c93a re-port. Anchor: `original/MASHED.exe.unpatched` SHA-256 `BDCAE093...3C0E`
(matches CLAUDE.md). `original/MASHED.exe` is the ten-patch boot recipe (`B9977DAB7060...`), with no unlock patch.

## 1. Why no existing harness fit, and what the old C4 actually measured

0x0042b770 has exactly one call site in the image: `CALL 0x0042b770` at `0x004409ce` in `FUN_0043dfd0`
(`E8 rel32` scan of `.text`). It is reached only when the per-screen enable `[ESP+0x34]` is non-zero
(`CMP [ESP+0x34],EBX` / `JZ` at `0x004409c8`/`0x004409cc`). That enable is set only by opcode `ff33`, which only
screen 5 (Race Results) carries (U-9060, `re/tools/screen_opcodes.py`). Its result is consumed at `0x004409d3`. If it is
non-zero and `*(0x0067ed3c + [0x0067e9f8]*0x40) == 5` (`0x004409df`), the store at `0x004409e8` sets `[0x0067ec28] = 1`.
`FUN_0042c960` then calls `FUN_0042c510` (return address `0x0042c9c4`), which advances the stat page index
`0x0067ea08` (contcfg_record.md, action 7 = "Change Stat").

So the game never calls 0x0042b770 at the main menu or any boot-to-menu screen (CHANGELOG 2026-08-31 measured 0/s at
the menu and while paused). The previous C4 evidence ("subset-install canonical boot-to-menu observe, 0xE9 live,
25 s survival", `log/install_observe_r1b_20260609.txt`, and the 20 s `main_menu_navigate_arrows` run) installed the JMP
and survived. In that scenario the function body **never executed**, so it was never a behavioural diff.
`canonical_install_observe.py`, `canonical_c4_autodiff.py` and `canonical_c4_navigate.py` all drive boot-to-menu or
`4,4` menu screens, so none can exercise this row. U-9061 records that a naturally entered Race Results had never been
reached.

New harness, as allowed by the brief when nothing fits: `re/frida/c4_detect_e_scenario.py` (+ `c4_detect_e_compare.py`).
It reuses `statenav.py`'s Nav class, Quick Battle navigation order and `FUN_00497310` return override, unchanged.

## 2. Scenario

1. Spawn MASHED (own PID, killed by PID at the end). Navigate title -> Game Type Select -> Load modal -> Single
   Player -> down -> Quick Battle -> colour -> track -> start (statenav sequence).
2. In the race, pulse control 4 (player 0, 200 ms, 1 Hz) only while the session-phase byte `0x00771968 == 3`. This is
   `scenario_launch.py`'s in-race recipe (skip intro, keep driving, continue between rounds). A first attempt with
   **no** input sat in the race for 240 s with no round end (`log/c4_detect_e/c4_detect_e_feas_off.json`, not committed).
3. Wait until the frontend phase `0x0067eca4 == 3`, `TYPE(depth-1) == 5` (`0x0067ed3c + depth*0x40`) and the detector has
   been entered. Reached naturally in every one of the 10 final runs (match end, `0x00771968 == 1`, depth 1), with no
   push and no poke. **This is the U-9061 route: a naturally entered type-5 screen.**
4. Idle 3 s, then press action 7 (the `FUN_00497310(p, 7)` override, which cooks into `0x007f1049 + p*0x4c`, contcfg
   table `0x004965d9`) for player `[0] [1] [2] [3] [0] [1] [2] [3] [0,1,2,3]`. Each press is 200 ms followed by a
   2 s window.

Live inputs on Race Results (identical in all 10 runs): `0x007f1a0c = 1` (slot path, not 0x1000). The four records at
`0x007f1a14 + i*0x10` have ids `0, 6, 7, 8`. `FUN_0040e470(i)` (`*(*0x005f2770 + i*4 + 0x34)`) reads
`1, 2, 2, 2` for i = 0..3, so records 1..3 are skipped by the `== 2` test (`0x0042b854`) and only pad 0 is eligible.
The cook raised held bytes for pads 0, 1 and 2. A player-3 press never set `0x007f1049 + 3*0x4c` in any run.

Coverage, stated plainly: all four records of the 0x10-stride walk are visited. The eligible/skip split is 1 eligible,
3 state-2 skips. The `0x1000` branch (`0x0042b783..0x0042b836`) is **not** executed in this scenario. It is covered only
by the C3 fixture (`fix_diff_menu_button_detect_e.csv`).

## 3. Arms

| arm | setup | 0x0042b770 at arm time | detector instrumented at |
|---|---|---|---|
| `off` | shim loads deployed `.asi`, `MASHED_HOOK_ONLY=__none__` | `0xA1` (stock `MOV EAX,[0x7f1a0c]`) | original entry |
| `on` | shim loads deployed `.asi`, `MASHED_HOOK_ONLY=0x0042b770` | `0xE9`, target `mashed_re_dev.asi+0x12df0` | E9 target |
| `stock` | `MASHED_RE_NO_AUTO_HOOK=1`, nothing loaded | `0xA1` | original entry |
| `late` | `NO_AUTO_HOOK=1`, then `Module.load(original/mashed_re_dev.asi)` + its `InjectHooks` export at the title | `0xE9`, `+0x12df0` | E9 target |
| `ctl` | as `late`, but the control `.asi` (below) | `0xE9`, `+0x12df0` | E9 target |

**Hook-live witness** (memory hook-live-witness-snap-the-rva-dword): on every `on`/`late`/`ctl` run, the first byte
at 0x0042b770 read `0xE9` **before** Frida attached anything. The rel32 resolved into the loaded `mashed_re_dev.asi`
module (path recorded per run) at `+0x12df0`, which is the `MenuButtonDetectE` export RVA in both `.asi` files
(`evidence/c4_detect_e_asi_disasm.txt`). All detector calls on those arms (496..689 per run) were counted at that
reimpl entry, i.e. the game's own `CALL 0x0042b770` at `0x004409ce` ran through the inline JMP into our code.
(After our attach, the off/stock arms also read `0xE9` at 0x0042b770. That is Frida's own trampoline. Hence the
witness is taken before attach.)

**Control** `.asi` (SHA-256 `8AD6363E...448F`): the pre-dc98c93a slot walk alone (4-byte stride, 8 iterations, index 0..7)
restored into `Frontend/MenuButtonDetect.cpp` at `3acbc094`. Built with `MASHED_NO_DEPLOY=1`, copied out, source restored
with `git checkout`. Diff: `evidence/c4_detect_e_ctl_patch.diff`. Static check (`c4_detect_e_asi_disasm.txt`): the
deployed body walks `shl eax,4` / `add eax,0x7f1a14` / `cmp eax,0x7f1a54`. The control walks `add edi,4` / `cmp ebx,8`.
The deployed `.asi` was never overwritten. The control was never deployed.

## 4. Observable

Per press, from the arm's own counters:
- `e_ret1`: detector calls returning 1 (0x0042b770's output).
- `c510_flagpath`: `FUN_0042c510` entries returning to `0x0042c9c4`. This is the `0x0067ec28` chain that the detector's
  result drives through `0x004409e8`. It is the consumer-side witness of what the return did to menu state.
- reported, **not compared**: `c510_other` and `page_delta`. `FUN_0042c510` has a second caller at `0x0042d01b`, return
  `0x0042d020`, inside the `FUN_0042c960` body (next function start in hooks.csv is above `0x0042d200`). It is guarded by
  the scroll accumulator `[0x0067ebcc] -= 0xc` (`0x0042cffa`), `fild [0x0067ebcc]` + `[0x0067ec20]` vs `[0x005cc728]`
  (`0x0042d00e`). It fires 0, 1 or 2 times per run at varying presses, **including between two stock runs**
  (`c4_detect_e_round1_superseded/`). It does not depend on 0x0042b770, and it moves `0x0067ea08` too, so the raw page
  delta is non-deterministic.

Per call, every input the bytes read is snapshotted at entry (`0x007f1a0c`, `0x007f1a14..0x007f1a50`, held/previous
columns for pads 0..7, `FUN_0040e470(0..7)`). Two models are evaluated on the snapshot: `m_orig` (the bytes
0x0042b770..0x0042b888) and `m_old` (the pre-fix port). `mmOrig` / `mmOld` count calls whose actual return differs.

## 5. Results (final round, `evidence/c4_detect_e_compare_ref{off1,stock1}.csv`)

| arm | setup-matched ref | presses matched | e_ret1 total | mmOrig | mmOld | crash | result |
|---|---|---|---|---|---|---|---|
| off2 | off1 | 9/9 | 3 | 0 | 2 | 0 | GREEN (original repeat) |
| **on1** | off1 | **9/9** | 3 | **0** | 2 | 0 | **GREEN** |
| **on2** | off1 | **9/9** | 3 | **0** | 2 | 0 | **GREEN** |
| stock2 | stock1 | 9/9 | 3 | 0 | 2 | 0 | GREEN (original repeat) |
| **late1** | stock1 | **9/9** | 3 | **0** | 2 | 0 | **GREEN** |
| **late2** | stock1 | **9/9** | 3 | **0** | 2 | 0 | **GREEN** |
| ctl1 | stock1 | 7/9 | 5 | 2 | 0 | 0 | **RED** (idx 1, 5) |
| ctl2 | stock1 | 7/9 | 5 | 2 | 0 | 0 | **RED** (idx 1, 5) |

The per-press original behaviour (all four original runs): player 0 -> detector 1, flag-path advance 1. Player 1 ->
0, 0. Player 2 -> 0, 0. Player 3 -> 0, 0. All four together -> 1, 1. The fixed port reproduces it on all four
hook-live runs.

The control fails exactly where the bytes say it must. With the 4-byte walk it reads ids `0,0,0,0,6,1,3,0` from
`0x007f1a14..0x007f1a30` with index 0..7, so i = 5 takes id 1 with `FUN_0040e470(5)` = `6235944` (not 2). Pad 1's
press then returns 1 and advances the stat page through `0x0042c9c4`, where the original returns 0 and nothing
happens. `mmOld = 0` on the control means its returns match the old-port model on every call. `mmOrig = 2` means those
two calls are the only departures from the bytes. The `late` arms use the same load mechanism and are GREEN, so the RED
is the stride, not the load path. Survival: every run reached the end alive (exit `0xDEADBEEF` is the harness's
`dev.kill`), with no AV.

Detector calls per run: 437..689. Across all 10 final runs `mmOrig` is 0 on every non-control arm, i.e. the byte model
of 0x0042b770 agrees with the original's real return on every live call (validated on the four original arms).

## 6. C4 checklist

- canonical scenario in which the game itself calls 0x0042b770: yes (natural Race Results, call site `0x004409ce`)
- hook installed as an inline JMP and proven live in the run: yes (`0xE9` pre-attach, rel32 into `mashed_re_dev.asi+0x12df0`)
- clean behavioural diff vs the original in the same scenario: yes, 4/4 hook-live runs, two load paths, two references
- the comparison can fail: yes, the old-stride control reads RED 2/9 on both runs, at the presses the model predicts
- survival: 10/10 runs alive, 0 crashes
- other F-DoD items: open `[UNCERTAIN]` on the row are U-1651 / U-3445, both filed, structural, "Blocks: none". No
  STUBS row for 0x0042b770.

Not covered: the `0x1000` branch (C3 fixture only). The parent may want to name that in the hooks.csv note.

## 7. Side finding for U-9061 (not acted on, parent's call)

This scenario reaches Race Results the intended way (no push, no poke), and an action-7 edge on player 0 returns 1 at
`0x004409ce`. It is followed by a `FUN_0042c510` entry returning to `0x0042c9c4` and a page advance, on 8 original runs (4 final + 4 in round 1).
`0x0067ec28` itself was only polled from Python and read 0 at every sample, so the literal 0 -> 1 store was not
captured. U-9061's resolution text asks for that store. An entry hook on
`FUN_0042c960` reading `0x0067ec28` would capture it with this same scenario.

## Files

- harness: `re/frida/c4_detect_e_scenario.py`, `re/frida/c4_detect_e_compare.py`
- per run: `evidence/c4_detect_e_{off,on,stock,late,ctl}{1,2}.{json,txt}`
- comparisons: `evidence/c4_detect_e_compare_refoff1.csv`, `..._refstock1.csv` (+ `_summary.json`)
- control: `evidence/c4_detect_e_ctl_patch.diff`. Static: `evidence/c4_detect_e_asi_disasm.txt`
- first round (before the `0x0042d020` attribution, compared raw page delta, superseded):
  `evidence/c4_detect_e_round1_superseded/`
- PrintWindow screenshots were blank (D3D surface) and are not committed. Screen identity rests on `TYPE(depth-1) == 5`.
