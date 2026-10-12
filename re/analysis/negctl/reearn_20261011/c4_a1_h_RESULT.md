# C4 re-earn, group H (in-race HUD): 0x00403160, 0x0041db80, 0x004295a0

**Verdicts (evidence only, no tracker edit, no re-classify. The parent promotes):**

| RVA | row | verdict |
|---|---|---|
| 0x00403160 | Sub00403160_SubMode0BViewport | **C4 EARNED** |
| 0x0041db80 | Sub0041db80_HudThresholdDispatch | **C4 EARNED** |
| 0x004295a0 | HudDualLabelRender | **C4 EARNED**, with one forced state (section 4: Silver-cup launch gates opened in an install copy's save) |

Date 2026-10-12, branch `work/c4a1-h` (base `225236b5`). Brief: `brief_c4_a1.md`, method from
`c4_menu_button_detect_e_RESULT.md`.

Binaries. Anchor `original/MASHED.exe.unpatched` SHA-256 `BDCAE093...3C0E` (matches CLAUDE.md). The game run is
`original/MASHED.exe`, the ten-patch boot recipe `B9977DAB70607720...`, no unlock patch. Port under test: the deployed
`original/mashed_re_dev.asi` SHA-256 `C5594D6B933B2400...3840`, never overwritten. A `MASHED_NO_DEPLOY=1` build of
this branch's HEAD gives the same normalized bodies for the three exports (`evidence/c4_hud_asi_disasm.txt`).

## 1. Who calls each body (static, `evidence/c4_hud_reach_static.txt`)

Call sites from an `E8`/`E9` scan of `.text`:

- **0x00403160**: one site, `call 0x00403160` @0x0040e0c4 in HudIngameDispatch 0x0040dfc0.
- **0x0041db80**: one site, `jmp 0x0041db80` @0x0040e0ee in the same function.
- **0x004295a0**: three sites, all in FUN_00429e10 (@0x0042a077, @0x0042a08e, @0x0042a16a). FUN_00429e10 has one
  site, @0x00410cfa, in the per-frame race path, behind `FUN_00426ba0() != 0` (@0x00410cf1).

### 1.1 0x00403160 and 0x0041db80

HudIngameDispatch takes the 0x0040e056 branch only when `[0x0063ba8c]` is 5 or 6 (@0x0040dfce..0x0040dfd6). It then
switches on `[0x0067e9fc] - 2` (FUN_0042f6a0 = `mov eax,[0x0067e9fc]`). The index bytes at 0x0040e140 are
`0,1,1,1,2,4,4,4,2,3` for modes 2..11. Mode 0xb gives index 3, and table entry 3 at 0x0040e12c is 0x0040e0c4, the
0x00403160 call. Every case then goes to 0x0040e0e0, which returns when the mode is 0xb (@0x0040e0e5) and otherwise
jumps to 0x0041db80.

- 0x00403160 runs in states 5/6 with mode 0xb.
- 0x0041db80 runs in states 5/6 with any mode except 0xb.

The only `mov [0x0067e9fc],0xb` in the image is @0x0043df19, in FUN_0043df00. That function also writes rule
`[0x007f0fd0] = 6`. Its only caller is 0x0040312c, in the title tick 0x004030d0 (called from 0x00492d63). The title
tick adds frame time to `[0x00636ae8]` and calls FUN_0043df00 once `[0x00636ae8]` passes `[0x00636aec]`
(@0x004030ff/0x0040310a, and `[0x00636af8]` == 0 @0x00403113). That is the attract-demo race. Measured: `[0x00636aec]`
reads 60.0, and the demo starts 60 s after the title appears with no input.

### 1.2 0x004295a0

FUN_00429e10 reaches the call sites only when all of these hold:

- `FUN_00443080() == 1` (`[0x00897ffc]`, @0x00429f88)
- `FUN_0040e350() == 0xb` (`[0x0063ba8c]`, @0x00429f96)
- the switch on `[0x007f0fd0] - 5` (table 0x0042a44c) lands on a call site:
  - rule 5: call if `FUN_00405890() == 0`
  - rule 8: call if `FUN_00417740(0) == -1`
  - rules 9 and 10: call if `[0x007f0fcc] == 0`
  - rules 6 and 7: go to 0x0042a1cc, which has no call

`[0x0063ba8c] = 0xb` is written at 0x00410645 in FUN_00410510 (the rule test). The same path calls
VehicleSeedWritePair 0x00448700 with `push 1` (0x0041067e or 0x00410811), and that function writes `[0x00897ffc]`
@0x0044871e.

Rules 5/8/9/10 come from the cup event table 0x005f65c8, and only in its mode-4 (Silver) and mode-5 (Gold) cells
(`game_mode_rules_REmap_20260616.md` step 3). Mode 3 (Bronze) cells are rules 0 and 4. The attract demo is rule 6,
and the Quick Battle used in section 3 is rule 0. So no stock-save scenario reaches this body. Measured: the Bronze
event 0 reaches state 0xb with rule 0 and 0 calls (`evidence/c4_hud_cup_probe_bronze0.txt`).

## 2. Arms (same layout as the E harness)

One fresh MASHED per run, killed by its own PID.

| arm | setup | hook state | instrumented at |
|---|---|---|---|
| `off` | shim loads the deployed .asi, `MASHED_HOOK_ONLY=__none__` | stock first byte | original entry |
| `on` | deployed .asi, `MASHED_HOOK_ONLY=<rva>` (one row live) | `0xE9` | E9 target |
| `stock` | `MASHED_RE_NO_AUTO_HOOK=1`, nothing loaded | stock first byte | original entry |
| `late` | `NO_AUTO_HOOK=1`, then `Module.load(deployed .asi)` + `InjectHooks` at the title | `0xE9` | E9 target |
| `ctl` | as `late`, with the old-defect control .asi | `0xE9` | E9 target |

References: `on` is compared to `off1`, and `late`/`ctl` to `stock1`. The two runs of each original arm are compared
to each other, which gives the original's own noise.

**Hook-live witness.** On every `on`/`late`/`ctl` run, the first byte at the RVA read `0xE9` before Frida attached
anything. The rel32 resolved into the loaded module at the export RVA:

- 0x00403160 → `mashed_re_dev.asi+0x141a0` (Sub00403160_SubMode0BViewport)
- 0x0041db80 → `+0x142a0` (Sub0041db80_HudThresholdDispatch)
- 0x004295a0 → `+0x18800` (HudDualLabelRender)

These are the export RVAs in the deployed .asi and in each control .asi. The module name in each run's
`pre_witness` is `mashed_re_dev.asi` on `on`/`late` and the control file on `ctl`. Every call counted on those arms
was counted at that reimpl entry, so each one is a game call through the inline JMP. The `off`/`stock` runs read the
stock first bytes: `0x83` at 0x00403160, `0xb8` at 0x0041db80, `0x83` at 0x004295a0.

**Controls.** Each is one old defect restored alone, from `fix_hud_RESULT.md`, built with `MASHED_NO_DEPLOY=1`,
copied out, and the source restored with `git checkout` + touch. Patches are in `evidence/c4_hud_ctl*_patch.diff`.
Static check, `evidence/c4_hud_asi_disasm.txt`: each control differs from the deployed .asi only in its own export,
and the other two exports are IDENTICAL.

| control | defect restored | SHA-256 | static diff |
|---|---|---|---|
| ctl403 | double dereference `*(*(*0x007d3ff8)+0x20)` | `D73FA6C9...21D7` | an extra `mov eax,[eax]` before each of the four `mov eax,[eax+0x20]` |
| ctl41d | inverted 0.34 test (`if (!(c < [0x005cd238])) return`) | `E147D15A...1F49` | one `ja` becomes `jbe` |
| ctl4295 | 2-arg first FUN_00427e00 call | `CBBCACB7...24D1` | first call pushes 2 args, `add esp,8` |

## 3. Scenarios, observables, results

Harness: `re/frida/c4_hud_scenario.py --row <rva> --arm <arm>`. Comparison: `re/frida/c4_hud_compare.py`. Per-run
JSON and logs: `evidence/c4_hud_runs/`. Comparison tables: `evidence/c4_hud_runs/c4_hud_compare_<row>.csv`.

### 3.1 0x00403160, attract demo

**Scenario.** Boot to the title and send no input at all. After 60 s the game itself runs FUN_0043df00: mode 0xb,
rule 6, `[0x0063ba8c]` 5 then 6. HudIngameDispatch calls 0x00403160 about 60 times/s for the whole demo. Observe 30 s
after the first call. The scenario is stable: mode 11 / rule 6 at the first call on 7 of 7 non-control runs (both ctl runs crashed on that first call, section 3.1 table).

**Observable, per call.** The body's own calls and outputs, recorded only while the body executes:

- its return (EAX)
- each FUN_004c1c80 call: camera arg and the two view-window dwords
- each FUN_004c1a00 return
- each FUN_00402fb0 entry
- FUN_00428760's six args
- `cam+0x68/+0x6c` after return

The caller does not consume the return. After @0x0040e0c4 the code goes to 0x0040e0e0, which calls FUN_0042f6a0 and
overwrites EAX, so the return is compared as the body's own output.

Excluded raw values, measured as varying between frames on the original:

- **Saved view window** (`cam+0x68/+0x6c` before the call). The attract camera changes it every frame. It is keyed
  as `PRE` when the restore call and the post-call value equal the pre-call value.
- **`[0x00771964]` pointer.** It is heap and different per boot. It is keyed as `F771964` when FUN_00428760's arg 1
  equals the live value.
- **The first call of each run.** Its pre-call value comes from the previous call's camera, so it is excluded from
  the key set (one call per run).

The original's single key, on all four original runs:
`ret=cam | V(0x3f4ccccd,0x3f4ccccd) B=cam D P(F771964,50.0,30.0,240.0,120.0,0) V=PRE B=cam | post=PRE`

| run | calls (game, through the reimpl on hook arms) | keys | vs ref | crash / exit | result |
|---|---|---|---|---|---|
| off1 | 1769 | 1 | = off2 | 0 / harness kill | GREEN (original repeat) |
| off2 | 1768 | 1 | = off1 | 0 | GREEN (original repeat) |
| **on1** | **1812** | 1 | = off1 | 0 | **GREEN** |
| **on2** | **1705** | 1 | = off1 | 0 | **GREEN** |
| stock1 | 1770 | 1 | = stock2 | 0 | GREEN (original repeat) |
| stock2 | 1814 | 1 | = stock1 | 0 | GREEN (original repeat) |
| **late1** | **1626** | 1 | = stock1 | 0 | **GREEN** |
| ctl1 | entered 1, returned 0 | 0 | missing | AV inside the body, EIP `0x3f1ffc9d` (unmapped), exit `0xC0000005` | **RED** |
| ctl2 | entered 1, returned 0 | 0 | missing | same EIP, exit `0xC0000005` | **RED** |

The original's noise is the call count only (1768..1814, the wall-clock length of the observation window). The
per-call key never varied.

The control fails exactly where the defect predicts: on the first game call, with the body still active
(`inBody=true`), at the first `call [eax+0x20]` after the begin-update succeeded. The extra dereference makes the
call target the dword at `*(*(*0x007d3ff8)) + 0x20`. That dword reads `0x3f1ffc9d`, which is not mapped. So this
scenario separates the fixed port from the old one on the first call. The control reads RED by crash, not by a
divergent value.

A first round of this row (before the harness counted entries and acknowledged crash messages) gave the same picture:
7 GREEN, ctl crashes with exit `0xC0000005`, and ctl2 caught the same EIP 0x3f1ffc9d. Its log is
`evidence/c4_hud_runs/batch_403160_first_41db80.log`, superseded by the rerun above.

### 3.2 0x0041db80, race standings

**Scenario.** The statenav navigation, as in `c4_detect_e_scenario.py`. The run lands on depth-3 entry 1:
`[0x0067f184]` = 11, mode 10, rule 0 on 9 of 9 runs. In the race, control 4 is pulsed at 1 Hz only while
`0x00771968 == 3`. HudIngameDispatch calls 0x0041db80 in states 5/6 from the race start for about 25 s, then the
match ends (state 9/10). The run stops 15 s after the last call.

**Observable, per call.** The counter `[0x0063d588]` (record 0x0063d55c + 0x2c) at entry and exit. And the ordered
list of record slots (+0x00..+0x14) whose `obj+0x48` function the body called.

The six `obj+0x48` pointers are swapped to a recorder for the duration of each call only. The recorder calls the
saved pointer with the same argument and returns its EAX, and all pointers are restored on leave. This is identical
on all arms. The dispatched functions are not instrumented directly because `obj+0x48` is a render callback that
other code calls at high rate. The counter never changed inside a call (`cchg = 0` on every run), so the byte model
of 0x0041dab0..0x0041db79 applies with c constant.

Byte model, thresholds read from .rdata:

| c | slots dispatched |
|---|---|
| c < 0.34 | 0,5 |
| 0.34 ≤ c < 0.82 | 0,5,1 |
| 0.82 ≤ c < 1.3 | 0,5,1,2 |
| 1.3 ≤ c < 1.86 | 0,5,1,2,3 |
| 1.86 ≤ c < 4.0 | 0,5,4 |
| c ≥ 4.0 | nothing |

Constants: `[0x005cd238]` 0.34, `[0x005cd234]` 0.82, `[0x005cd230]` 1.3, `[0x005ccdf4]` 1.86, `[0x005cc35c]` 4.0.
All six buckets are reached on every run.

| run | calls | buckets | slot list per bucket vs ref | mmOrig (≠ byte model) | mmOld (≠ control model) | result |
|---|---|---|---|---|---|---|
| off1 | 1224 | 6 | = off2 | 0 | 948 | GREEN (original repeat) |
| off2 | 1252 | 6 | = off1 | 0 | 720 | GREEN (original repeat) |
| **on1** | **1088** | 6 | = off1 | **0** | 952 | **GREEN** |
| **on2** | **1214** | 6 | = off1 | **0** | 950 | **GREEN** |
| stock1 | 1290 | 6 | = stock2 | 0 | 950 | GREEN (original repeat) |
| stock2 | 1186 | 6 | = stock1 | 0 | 960 | GREEN (original repeat) |
| **late1** | **1280** | 6 | = stock1 | **0** | 806 | **GREEN** |
| ctl1 | 1025 | 6 | differs in buckets <0.34, <0.82, <1.3, <1.86, <4 | **720** | 0 | **RED** |
| ctl2 | 1233 | 6 | same 5 buckets differ | **960** | 0 | **RED** |

The original's noise is the call count and the per-bucket call counts (timing). The bucket-to-slot mapping never
varied, and mmOrig = 0 on all four original runs, so the byte model is validated against the original.

The control departs exactly as its model says: mmOld = 0, and it matches the original only in the c ≥ 4 bucket,
where both dispatch nothing. No crash on any run.

### 3.3 0x004295a0, Silver-cup event fail

**Scenario** (install copy, section 4).

1. Title → Game Type Select → Load modal → Single Player, then depth-3 cursor 0. This is the challenge flow:
   `[0x0067f184]` = 1, mode 3.
2. Colour select, then the challenge select (screen type 6).
3. Control 10 moves the cup selector 1 → 2 and mode becomes 4. This is the step @0x00440415 in FUN_0043dfd0, then
   FUN_0042f6b0. Found by a one-key-at-a-time sweep, `evidence/c4_hud_cup_probe_keysweep.txt`: controls 6..9 changed
   nothing, 10 did.
4. Two downs move `[0x0067f17c]` 0 → 2 (FUN_004322c0). Confirm.
5. The race launches mode 4, `[0x0067f17c]` = 2, rule 10 (event table cell mode 4, track 2 = ev4 → rule 10) on 9 of 9 runs. The probe run's CreateFileA log shows `TRACKS\NEUSTEIN.PIZ` (`evidence/c4_hud_cup_probe_silver2.txt`).
6. **No input in the race.** The event timer `[0x007f0fe4]` counts down from 17.8 and FUN_00410510 takes its rule-10
   fail path: state 0xb, `[0x00897ffc]` = 1, `[0x007f0fcc]` = 0.
7. FUN_00429e10 then calls 0x004295a0 through 0x0042a16a about 48 times/s, and the game stays there. Observe 30 s.

**Observable, per call.** The two FUN_00427e00 calls the body makes while it executes (six stack args each, and each
call's EAX), and the body's return. Excluded raw value: `y` animates (measured 688.3 down to -321.67 on the original).
The y arg is keyed against `[0x0063b910]` read at the body's entry, as `Y+3` on the first call and `Y` on the second.
That is FUN_0040dc80's `fld [0x0063b910]` plus `[0x005cc31c]` (3.0f), and nothing in the body writes 0x0063b910.

The original's single key, on all four original runs:
`n=2 (0xff,Y+3,0x427c0000,0xff000000,0x3f800000,2) (0xff,Y,0x42700000,0xff0000ff,0x3f800000,2) ret=LAST`

| run | calls | keys | vs ref | crash | result |
|---|---|---|---|---|---|
| off1 | 1475 | 1 | = off2 | 0 | GREEN (original repeat) |
| off2 | 1396 | 1 | = off1 | 0 | GREEN (original repeat) |
| **on1** | **1455** | 1 | = off1 | 0 | **GREEN** |
| **on2** | **1070** | 1 | = off1 | 0 | **GREEN** |
| stock1 | 1460 | 1 | = stock2 | 0 | GREEN (original repeat) |
| stock2 | 1559 | 1 | = stock1 | 0 | GREEN (original repeat) |
| **late1** | **1414** | 1 | = stock1 | 0 | **GREEN** |
| ctl1 | 1536 | 88 | 1536 of 1536 calls carry a key the original never produced | 0 | **RED** |
| ctl2 | 1533 | 88 | 1533 of 1533 foreign | 0 | **RED** |

The original's noise is the call count only (1396..1559). The control's first call carries args 3 to 6 from stack
residue, for example `(0xff, Y+3, 0xff000000, 0x042be844, 0xc39f5554, 0x02d1071f)`. Its second call and the return
match the original.

## 4. Forced state (0x004295a0 only), and why it does not touch the measured path

The stock save opens only Bronze row 0. The scenario boots an **install copy** of `original/`
(`<scratchpad>/gameroot/original`, robocopy). Its `MASHED.exe`, `mashed_re_dev.asi` and `d3d9.dll` have the same
SHA-256 as in `original/` (`evidence/c4_hud_install_copy.txt`).

In the copy's `gamesave.bin` only, 26 dwords were set to 1: span 0x24A40, col 2 (+0x08, FrontendModeIndex(mode 4) = 2)
and col 5 (+0x14, FrontendModeIndex(mode 5) = 5) on all 13 rows. `cmp` lists exactly those 26 bytes against
`original/gamesave.bin`. These are the launch-gate columns that `gamesave_edit.py` documents as propagating to
`DAT_007f0a40` on load. `original/` was never written.

The body reads only `[0x0063b910]` (through FUN_0040dc80) and `[0x005cc31c]`, and calls FUN_00427e00. Its gate
values are all written by the game during the run:

- `[0x0063ba8c]` and `[0x00897ffc]` by FUN_00410510's fail path
- `[0x007f0fcc]` by the same path
- `[0x007f0fd0]` by the race launch

No memory was poked. The save edit decides which event can be launched, not what the body sees. All arms of this row
use the same copy.

## 5. C4 checklist

| | 0x00403160 | 0x0041db80 | 0x004295a0 |
|---|---|---|---|
| game itself calls the body | yes, attract demo, call site 0x0040e0c4, no input | yes, race standings, site 0x0040e0ee | yes, Silver event fail, site 0x0042a16a, no in-race input |
| forced state | none | none | Silver/Gold launch gates in an install copy's save (section 4) |
| hook live (0xE9 + target = export) | yes, `+0x141a0` | yes, `+0x142a0` | yes, `+0x18800` |
| game call count on the reimpl | 1812 / 1705 / 1626 | 1088 / 1214 / 1280 | 1455 / 1070 / 1414 |
| behavioural comparison | per-call callee sequence + return + restore, 3/3 GREEN | dispatched slots per counter bucket, 3/3 GREEN, mmOrig 0 | both text calls' 6 args + return, 3/3 GREEN |
| original's own noise | call count only | call counts only | call count only |
| control RED | 2/2, AV at first call (EIP 0x3f1ffc9d) | 2/2, 720 / 960 calls off the byte model | 2/2, every call foreign |
| survival | 7/7 non-control runs alive to the harness kill | 9/9 alive | 9/9 alive |

## 6. Files

Harness:
- `re/frida/c4_hud_scenario.py` (three rows, five arms)
- `re/frida/c4_hud_compare.py`
- `re/frida/c4_hud_cup_probe.py` (cup-flow reachability probe, install copy only)
- `re/frida/c4_hud_feas.py` (stock feasibility counters)
- `re/frida/c4_hud_asi_disasm.py` (export disassembly)

These are new scenario harnesses, as `c4_detect_e_scenario.py` was. None of the boot-to-menu tools reach these
screens, and `hooks_registry.py` vectors are path1 fixtures, not scenarios.

Evidence (`evidence/`):
- `c4_hud_reach_static.txt`
- `c4_hud_asi_disasm.txt`
- `c4_hud_ctl{403,41d,4295}_patch.diff`
- `c4_hud_install_copy.txt`
- `c4_hud_cup_probe_{bronze0,keysweep,silver2}.txt`
- `c4_hud_feas_{attract,quick}.json`
- `c4_hud_runs/` (27 run JSONs + logs, three comparison CSVs, batch logs)

The three control .asi files are not committed. Their SHA-256 values are listed in section 2.
