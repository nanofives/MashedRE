# DESIGN REPLAY-ORACLE: race replay of the original as the oracle for the MASHED_ORIG_TOP path

Date 2026-11-11. Row REPLAY-ORACLE-DESIGN, worker session cmwbtmppz2d14sc1bn329o991, parent cmwbssxl22bg4sc1ba3ghf7dh.
Branch `orch/REPLAY-ORACLE-DESIGN`. Measurement and verdict: `RESULT_REPLAY-ORACLE-DESIGN.md` (same folder).

Binary: `original/MASHED.exe.unpatched` (SHA-256 anchor `BDCAE093...`) for every static citation, the patched install
`b9977dab...` for every run. Static evidence in this folder:
- `nondet_scan.py` / `nondet_scan.txt`: every reference to the clock, time, thread, wait, input and heap imports' IAT
  slots, the callers of the RNG / clock / input wrappers, and every reference to the clock-derived globals.
- `data_owner_map.py` / `data_owner_map.tsv`: per 4 KB page of the writable image, the code families (hooks.csv
  subsystem) whose instructions reference it.

Addresses are VAs (image base `0x00400000`). "Tick" = one entry of `FUN_00493390` (section 4).

## 0. The idea in one paragraph

Make the original's race a pure function of a small script: selection globals, RNG seeds, a clock that advances
only once per tick, and player-0 controls per tick. Run the same script in the exe (knob `MASHED_REPLAY`, default
OFF) through the bindings the ORIG_TOP path already has. Snapshot the writable image `0x005ea000..0x00915008` at
every tick on both sides. The exe keeps the original's globals at their original addresses (image pad,
`mashedmod/src/mashed_re/exe_main.cpp:272-294`), so the comparison is a diff by address with no per-field
knowledge. The first tick and address where they differ is the finding. The measurement in RESULT shows the
original side of this is already exact: 2,724 of 2,724 ticks identical between runs outside three named
nondeterministic spots, none of them race state.

## 1. Sources of nondeterminism in the original's race

### 1.1 Step structure (what the clock can and cannot change)

- The run loop `FUN_00492290` (`re/match/s15/00492290.c`, 100% match) calls, per iteration, `FUN_004929d0`,
  `FUN_00492d20` (input poll), `FUN_00492d30` (tick), `FUN_00492e90` (render), `FUN_004926c0`, `FUN_00493480`.
- `FUN_00493480` (`re/match/s15/00493480.c`) calls `FUN_00493390` and then discretises `DAT_007f1000` to a
  multiple of 50.
- `FUN_00493390` (`re/match/s15/00493390.c`) stores `DAT_007f1000 = 50` unconditionally. So `FUN_00493480`
  computes `a = DAT_007719d4 + 50`, `n = 1`, and stores `DAT_007f1000 = 50` again.
- In phases 3 and 6 the tick `FUN_00492d30` runs `FUN_004111c0(0x32)` `(DAT_007f1000 - 1) / 0x32 + 1` times
  (`re/match/s15/00492d30.c`, case 3 and case 6), which is **exactly once per iteration**.
- **The number of race steps per frame does not depend on the clock.** The clock reaches game state only through
  the values listed in 1.2.

### 1.2 Clock

Imports (IAT slots, `nondet_scan.txt`):

| import | IAT | readers |
|---|---|---|
| QueryPerformanceCounter | `0x005cc0c4` | `FUN_004950b0` (`0x004950c8`), `FUN_005ab040` (`0x005ab052`), `FUN_004a57e4` (`0x004a5817`, CRT start), `0x005aafb5` (no Ghidra function) |
| QueryPerformanceFrequency | `0x005cc030` | `FUN_00495120` (`0x00495128`, stores `DAT_00771e70/74`), `0x005aafca` (no Ghidra function) |
| timeGetTime | `0x005cc2b4` | `FUN_005ab040` (`jmp` at `0x005ab063` when `DAT_007dce00 != 1`), DirectShow base classes `FUN_0049cfb0`, `FUN_0049d080`, `FUN_0049d240`, `FUN_0049d270`, `FUN_0049d9a0` |
| GetTickCount | `0x005cc0c8` | `FUN_004a1570` (`0x004a159b`, `0x004a1683`; references string `'AMUnblock'` at `0x005d0fc4`), `FUN_004a57e4` |
| GetSystemTimeAsFileTime | `0x005cc1c8` | `FUN_004a43d2`, `FUN_004a57e4`, CRT `__time32` `0x005c2435`, `_clock` `0x005c29cb`, `___inittime` `0x005c2a0e` |

Not imported: `time`, `_time64`, `clock` as imports, `GetSystemTime`, `GetLocalTime`, `SleepEx`.

`FUN_004950b0` (`re/match/s16/004950b0.c`) returns the low dword of `(QPC * 3,000,000) / DAT_00771e70`. It has 15
call sites (`nondet_scan.txt`). Measured per race frame on the main thread (call log of run `smoke`, 3,977 race
calls, histogram by `re/frida/detorig_calllog.py`):
- 10 calls of `FUN_004950b0`: tick start `0x00492d36`, tick end (return `0x00492e2c` frame), render `FUN_00492e90` x5,
  `FUN_004926c0` x2, frame timer via `FUN_00493480` x1.
- About 1 call of `FUN_005ab040` from `FUN_005a6ea0` (audio clock, return `0x005a6ea9`), whose count per frame is not
  constant (section 1.5).
- 0 main-thread calls of `timeGetTime` / `GetTickCount` (pilot, `verify/detorig_pilot_20261101/RESULT_PROMOTE-DETORIG-PILOT.md`).

Clock-derived globals and who reads them (`nondet_scan.txt`, last section):

| global | writer | readers outside the writer |
|---|---|---|
| `DAT_007f1030` last sample | `FUN_00493390` `0x00493459`, `FUN_0040fc00` `0x0040fe24`, `FUN_00495110` `0x00495115` | **39 reads in game code**, among them `FUN_00469df0`, `FUN_00469aa0`, `FUN_00468d80`, `FUN_0046cc40`, `FUN_00470c70`, `FUN_0047d640`, `FUN_0047def0`, `FUN_004252c0`, `FUN_00446520`, `FUN_0040b290`, `FUN_0040c5b0` |
| `DAT_007f1010` float, `now * 3.3333333e-07` | `FUN_00493390` `0x0049344b`, `FUN_0040fc00` `0x0040fdff`, `FUN_00428d30` `0x004291cf` | `FUN_00408610`, `FUN_0040fc00`, `FUN_00428d30`, `FUN_0042e5b0`, `FUN_0042e8b0`, `FUN_00473ee0`, `FUN_004744a0` |
| `DAT_007f0ffc` frame delta | `FUN_00493390` | none (write-only) |
| `DAT_00771984` drift | `FUN_00493390` | none outside `FUN_00493390` |
| `DAT_007f102c` / `DAT_007f1024` / `DAT_007f1028` | `FUN_00493390` | none outside `FUN_00493390` |
| `DAT_00771994`, `DAT_0077197c` | `FUN_004926c0` | none |
| `DAT_007719a0`, `DAT_00771988`, `DAT_00771990`, `DAT_00771998`, `DAT_0077199c`, `DAT_007719a4` | `FUN_00492e90` | none outside `FUN_00492e90` |
| `DAT_007719a8` | `FUN_00492d30` (`0x00492e11`, `0x00492e2e`) | none |
| `DAT_007dca50` / `DAT_007dca54` audio clock | `FUN_005a6ea0` (`0x005a6f16`, `0x005a6f10`) | `FUN_005a66d0`, `FUN_005a6f30` |

So the race reads the clock through `DAT_007f1030` and `DAT_007f1010`, both written once per tick by
`FUN_00493390` from one `FUN_004950b0` value. `FUN_0040fc00` (`0x0040fdeb..0x0040fe24`) advances both from its own
float input: that is game arithmetic on the previous values, not a clock read.

Load-time reads: the piz read `FUN_004b67e0` polls an overlapped `ReadFile` and calls the callback `FUN_00429290`
(registered at `0x004027c2`) once per poll. The callback reads the clock through `FUN_0045d3f0(1)` ->
`FUN_005a60b0` -> `FUN_005a6ea0` -> `FUN_005ab040` and polls input through `FUN_004967e0` (xref of `FUN_004967e0`:
callers `FUN_00429290`, `FUN_00492d20`). The number of polls is the disk's completion time
(`verify/detorig_loadhold_20261101/RESULT_PROMOTE-DETORIG-LOADHOLD.md`, "Why the track load took 113 to 215 calls").

### 1.3 Random numbers

- **CRT `_rand` `0x005c229b`**: `ptd = __getptd()` (`0x004a8828`); `ptd+0x14 = ptd+0x14 * 0x343fd + 0x269ec3`
  (`0x005c22a3`, `0x005c22a9`); returns `(state >> 16) & 0x7fff`. Per-thread state.
  - Callers: `0x004c03b4` in the function at `0x004c03b0` (no Ghidra function), registered as `"random"` by the name
    table entry `0x005d8b18` -> `0x004c03b0`; `FUN_00564310` at `0x00564317` (hooks.csv
    third-party-library[RenderWare-Physics-3.7], C3), caller `FUN_00564c80`.
  - `_srand` `0x005c228e` stores its argument at `ptd+0x14`. Its only call is `0x004c0502` in `FUN_004c04f0`, registered
    as `"randomseed"` by `0x005d8b20` -> `0x004c04f0`. The argument is `__ftol` (`0x005c1dac`) of the value
    `FUN_004c0670(arg, 1)` returns. [UNCERTAIN] whether any shipped script calls `randomseed`; the scripts in the piz
    archives were not searched. Without a call the main thread's state is the CRT's initial value.
- **RenderWare ring RNG `FUN_00534870`**: 31-dword ring at `*( *(0x007d3ff8) + *(0x007dc578) )`, seeded by
  `FUN_00534990` from the constant `0x9a319039` (`0x00534a09`), LCG `0x41c64e6d` / `0x3039`, then `0x136` draws
  (`0x00534a46`). Game-side wrappers: `FUN_00472650` (70 callers: AI, camera, particles, HUD, pickups), `FUN_00472690`
  (7), `FUN_004b44f0` (10), `FUN_004b4510` (6). The menu consumes it a run-dependent number of times
  (`re/frida/js/detorig.js`, header item 4).
- No other RNG was found: `rand` has 2 callers, `FUN_00534870` 5 (`nondet_scan.txt`). [UNCERTAIN] a private
  generator inlined in game code (an LCG on a global without a call) would not appear in this census.

### 1.4 Input

- Per iteration `FUN_00492d20` (`0x00492d20`: `call 0x004967e0`) polls input. `FUN_004967e0` calls `FUN_00495fe0`
  (-> `FUN_00495870`, device read), `FUN_00496100`, `FUN_00496530` (cooked input, 13 calls of `FUN_00497310` at
  `0x004965af..0x00496690`), `FUN_004972b0`, `FUN_00497450`; it writes the key tables `0x00772ffc..0x00773000` and
  `0x00773030..0x00773059`.
- `FUN_00497310` (`re/match/s16/00497310.c`) returns a control value from the joystick tables `DAT_0077311c` (axes)
  / `DAT_007730d4` (buttons) or the keyboard path, per binding in `DAT_007e95c0[player]`.
- The tick reads the key tables directly: `FUN_00496920(5)` (`0x00772ffc` table) in `FUN_00492d30`'s tail,
  `FUN_00496930(3)` / `(4)` (`0x00773030` table) in `FUN_004926c0`, which sets `DAT_006147b8`.
- Window messages: `FUN_00499690` calls `PeekMessageA` (`0x0049969d`) into the `MSG` at `0x007e95a0`. `0x006147d8` is
  written at `0x00493eeb`, `0x00494485`, `0x0049479a`. Both change between runs (section RESULT, "What still differs").
- Input is also polled once per piz poll during the load (1.2).

### 1.5 Audio, video and threads

- `CreateThread` (`0x005cc0d0`) is called only from CRT `__beginthread` (`0x005c2eca`), whose only caller is
  `FUN_005aef30`. `FUN_005aef30` passes the trampoline `0x005aef70` (calls `[arg+4](arg+8)`) and sets the priority
  (`0x005aef5e`). Callers of `FUN_005aef30`: `FUN_005a8060`, `FUN_005bb000`, `FUN_005be260` (audio, hooks.csv).
- The audio stream reader issues overlapped reads (`FUN_00550da0`: `ReadFile` `0x00550e16`, `0x00550eb9`;
  `FUN_00551090`: `GetOverlappedResult` `0x005510ef`) and stores a status word at `0x007dc764` (store `0x005505dc`).
  Values `0x3e5` / `0x3e4` were seen there, differing between runs.
- The audio clock `FUN_005ab040` is called from the main thread and from other threads; calls from other threads go
  to the real API (about 60/s, pilot). Its main-thread call count per frame is not constant: run `t2_a` made one more
  or one fewer than `t2_b` from tick 8 on (`reports/ticksign_t2_a_t2_b.json`, `n_misalign_at`), while race state stayed
  identical.
- DirectShow: the base-class code at `0x0049c..0x004a2` (timeGetTime and GetTickCount readers above, the
  `'AMUnblock'` thread loop `FUN_004a1570`) serves `frontend.mpg`. Its clock readers made 0 main-thread calls in the
  race call log. [UNCERTAIN] no thread-level census of DirectShow during a race.
- Audio is not muted on the original: `MASHED_MUTE` is read only by the exe's `Audio/AudioEngine.cpp:215` and
  `Audio/AudioWire.cpp`. The runs here played through the VB-CABLE endpoint.

### 1.6 Values read before they are written

- `FUN_00469aa0` copies 16 stack dwords into the 256 x 0x40 ring at `0x007ec9e4`, of which fields `+0x28..+0x3c` were
  never written by the function (`0x00469d90..0x00469daa`; LOADHOLD RESULT, "The 0x007ec9e4 ring"). Field `+0x2c`
  holds a return address in the original. Readers of fields `+0x24..+0x2c`: `0x0040bebe..0x0040beca`,
  `0x0040c2d8..0x0040c2e4`, `0x00422e60..0x00422e70`. [UNCERTAIN] what they compute was not traced.
- Between two runs of the **same** binary these stale values are identical (same code, same stack history), so they
  do not break original-against-original. Between the exe and the original they differ by construction, because
  the stack holds different return addresses. The oracle masks the ring's `+0x28..+0x3c` fields (`RULE:STACKROW`
  in `re/frida/detorig_report.py`) until row RO-STACKROW-READERS shows whether the readers can move race state.
- [UNCERTAIN] no systematic search for other uninitialised reads on the race path was done. The hand port's
  layout sensitivity (memory "Sim trajectory is code-layout sensitive") is a property of the exe's hand code. The
  oracle finds such reads as a first divergence whose value is a code address or stack address.

### 1.7 Heap-address dependence

- The heap moves between runs: blocks `0x40000` apart (pilot), and in run `t_b` at `0x00d0a644` / `0x00d0eee0`,
  below the old `0x01000000` floor of the pointer rule (now `PTR_LO = 0x00995000`, the image end,
  `re/frida/detorig.py`).
- Race state (car records, pickups, input, RNGs, race clock) was identical at all 15 checkpoints in every pair with
  the heap at different addresses, and the per-tick hashes agree outside three named spots (RESULT). In this
  scenario no heap address reached race state.
- [UNCERTAIN] race state that lives on the heap (Lua state of `powerups.lua` / `COURSE.LUA`, RenderWare-Physics
  objects, RwFrame matrices) is not in the static image. It is seen only through what it writes into static records.

### 1.8 The d3d9 shim's frame limiter

The shim calls `QueryPerformanceCounter` through its own imports (`mashedmod/src/d3d9_shim/d3d9_shim.cpp:376-478`),
not through MASHED.exe's IAT, and sleeps real time. Under the virtual clock it changes wall time only: the game's
clock reads come from MASHED.exe's IAT slots, which the harness owns.

### 1.9 Boot residue

The harness installs at the menu. What MASHED.exe wrote before that from real time stays in the image:
`__security_cookie`-style seeds in `0x00616038` (`FUN_004a57e4`: GetSystemTimeAsFileTime, PID, TID, GetTickCount,
QPC), the frame counter `0x007f0ff0`, the 60-frame counter `0x00771978` and the 96-row table at `0x00868320`
(written by `FUN_00492e90`), frontend floats. They are the RESIDUE / OFFSET / MODOFFSET classes of
`detorig_report.py`. None is race state. The per-dword classes are listed in the checkpoint reports.

## 2. Record on the original

**Pin instead of record.** Every source that reaches race state can be set to a fixed function of the tick on both
sides, so the replay "recording" is a script, not a capture:

| source | rule (both sides) | hook on the original | frequency |
|---|---|---|---|
| clock | main-thread QPC returns `Q(tick)`, timeGetTime / GetTickCount return `T(tick)`, both advance only at the tick boundary (`qtick` = 166,667 counts = QPF 10^7 / 60; `ttick` = 17 ms) | the three IAT slots `0x005cc0c4`, `0x005cc2b4`, `0x005cc0c8` point at native stubs (`re/frida/js/detorig.js`, `qpcStub` / `tgtStub`, `qstep = step = 0`); the tick advance is a native CModule `onEnter` on `FUN_00493390` | 11 main-thread clock calls per frame, no JavaScript |
| timer residue | at the warp: `DAT_007f1030 = DAT_007f102c = FUN_004950b0()` value, `DAT_00771984 = 0`, `DAT_00771998 = DAT_0077199c = value / 3000` | `pinTimer()` in `detorig.js` (main thread, in the warp event) | once |
| CRT rand | `ptd+0x14 = holdrand` at the warp and at the race anchor | `pinRngs()` | twice |
| RW ring RNG | rewrite the ring the way `0x005349cf..0x00534a6d` seeds it, reset cursors, `0x136` draws | `pinRngs()` | twice |
| player-0 controls | scripted value per control, keyed by tick (by N in `detorig.py` today) | `FUN_00497310` replaced by a CModule that calls the original and returns the script's value for player 0 | 13 x players per frame, native |
| raw key tables, window messages | not pinned yet (masked) | none | |
| selection | track 0, mode 10, rule 0, car 0, team 0, `FUN_0040e480(k, 1/2/2/2)`, phase 2 | `warp()` | once |

Proposed file format, `replay.json` (one per scenario, committed; the values below are an example, the clock and RNG
values are the ones this row ran with):

```json
{ "format": "mashed-replay-1",
  "scenario": {"track": 0, "mode": 10, "rule": 0, "car": 0, "team": 0, "cars": 4},
  "warp": {"after_ticks": 120},
  "clock": {"q0": "0xC0000000", "qtick": 166667, "t0": "0x01000000", "ttick": 17},
  "rng": {"crt_holdrand": "0x12345678", "rw_seed": "0x9a319039", "rw_draws": 310},
  "input": [[0, 0, 4, 255], [545, 0, 0, 255], [600, 0, 0, 0]],
  "trace": {"ticks": 2700, "block": 1024, "raw": ["0x00603400-0x00603800"], "full_every": 180} }
```

`input` rows are `[tick after the race anchor, player, control, value]`. The original's reference trace is the
output of one run: `meta.json`, `ticks.bin` (per tick: tick, N, one FNV-1a hash per 1 KB block, raw ranges) and full
dumps at chosen ticks (`ckptNN.data.bin`), the format `detorig.py` writes today.

Where a human-played input sequence is wanted (a scenario no script reaches), log the `FUN_00497310` return values
for player 0 per tick from the same CModule (row RO-INPUT-RECORD) instead of the DirectInput keyboard buffer
`record_attach.py` records: the control level is what the exe binds.

Hot sources and how they stay off JavaScript: QPC is about 640 calls/s (pilot) and `FUN_00497310` has 13 call sites
in `FUN_00496530`, which runs every frame [UNCERTAIN: its call rate was not counted]; both run native code only (IAT
stubs, TinyCC CModule). `FUN_00493390` is entered once per frame. No `Interceptor.attach` with a JavaScript callback
runs per frame. This row launched the game 13 times: 12 runs completed with 15 checkpoints, 1 (`t_a`) failed to
attach within 20 s and was not re-run; 0 crashes, 0 hangs.

For the exe-against-original oracle the original-side install should move to process start (an ASI loaded by the
Ultimate ASI loader `original/dinput8.dll`, or the existing `mashed_re_dev.asi` in a replay mode). That removes the
boot residue of 1.9, which today is classed, not equal (row RO-ORIG-ASI).

## 3. Inject in the exe

On the ORIG_TOP path matched bodies call Win32 and the CRT through the exe's own imports (for example
`re/match/s16/00495120.c` maps `QueryPerformanceFrequency` under `MATCH_PORT`). Injection points, all behind
`MASHED_REPLAY=<replay.json>` (default OFF):

| source | exe binding today (`verify/porttop_20261108/BINDINGS.tsv`, hooks.csv `port`) | injection |
|---|---|---|
| clock | `0x004950b0` matched body `Match/MatchS16_004950b0.cpp` (port `called`), still a stand-in at `MatchS16_00495110_Call004950b0` on part of the path; `0x005ab040` port `none` | in the `MATCH_PORT` preamble map `QueryPerformanceCounter`, `timeGetTime`, `GetTickCount` to `Replay_*` wrappers (one TU, `Core/ReplayClock.cpp`): main thread and knob on -> `Q(tick)` / `T(tick)`, else the real API. Scoped to matched units, so the hand present/limiter keeps real time |
| tick boundary | `0x00493390` under-stand-in, `0x00493480` FpsDiscretise `missing` (no-op on the knob path) | bind both to their matched bodies (`re/match/s15/00493390.c`, `00493480.c`); the binding of `0x00493390` calls `Replay_Tick()` first when the knob is on |
| timer residue | `0x007f1030`, `0x007f102c`, `0x00771984`, `0x00771998`, `0x0077199c` at original addresses | write them at the warp with the same rule as `pinTimer()` |
| CRT rand | `0x005c229b` port `none`; the exe's UCRT `rand` keeps per-thread state with the same LCG (`0x343fd` / `0x269ec3`) | `srand(holdrand)` on the main thread at the warp and at the race anchor; the exe binding of `0x005c229b` must be the UCRT `rand` of the main thread |
| RW ring RNG | `FUN_00534870` port `none`; its state hangs off `*(0x007d3ff8)` (RwGlobals), which librw does not provide | bind `0x00534870` to a port with its own 31-dword ring and cursors, seeded by the `FUN_00534990` rule, re-pinned at the warp and anchor |
| player-0 controls | `0x00497310` C4 matched source, port `none`; `0x004967e0` stand-in `PortTop_StandIn_004967e0 -> PollKeyboard` | bind `0x00497310` to its matched body; its binding returns the script's value for player 0 when the knob is on. The raw poll binding (`0x004967e0` / `0x00495fe0`) reports no device when the knob is on, on both sides |
| selection and warp | `FUN_0040e480` port `none` | the knob runs the same warp at `warp.after_ticks` |

Every row above changes nothing when the knob is off. None is built in this row.

## 4. The tick boundary and the compare

**Boundary: the entry of `FUN_00493390`.** It is entered once per run-loop iteration from `FUN_00493480`
(`0x00493480`), in every phase. Measured: 2,724 entries for 30,000 main-thread clock calls after the race anchor,
11.01 calls per tick. The race step `FUN_004111c0` runs exactly once in the same iteration in phases 3 and 6 (1.1).
The snapshot at the boundary sees the state after `FUN_004929d0`, input, tick, render and `FUN_004926c0` of that
iteration.

**What is snapshotted.** The writable image `0x005ea000..0x00915008` (`.data` 0x32a704 bytes + `_rwdseg` 8 bytes,
3,317,768 bytes), on the exe the same VAs inside the image pad.
- Every tick: one FNV-1a hash per 1,024-byte block (3,245 blocks, 12,988 bytes per tick), plus raw copies of named
  ranges. A dword in `[0x00995000, 0x30000000)` with the low two bits clear is hashed as 0 (pointer rule).
- Every Nth tick (or on demand at a divergence tick): the full image.

**Cost.** Original side, TinyCC CModule: arms with 2,724 hashed ticks plus 1,792 raw words per tick ran 57 to 68 s
wall each, boot, menu and load included; the batch-1 arms with the N clock and per-tick hashes (n_a, n_b) ran 60 to
71 s, against about 90 s per arm for the LOADHOLD arms without hashing (which also had a 6 s load hold). So the hash
fits inside the 60 fps frame budget of the shim's limiter. [UNCERTAIN] not timed per tick. Exe side: the same loop
compiled by MSVC, expected cheaper, not measured.

**Pointers and heap-held state.**
- Car records are static: `0x008815a0`, 4 x `0xd04`. Pickup slots `0x0088fbe0` (64 x `0xb4`), cooked input
  `0x007f1038` / `0x007f14f8` and the race clock `0x007f0ff4..0x007f1034` are static too (`re/frida/detorig.py`, `REGIONS`).
- A dword that is a heap pointer on both sides is class PTR, compared as "both pointers", not by value. On the exe the
  heap is the exe's own, so this rule is the only one that can hold there.
- Heap-held race state (Lua, RW-Physics objects, RwFrames) is out of the first oracle. Divergence there shows up in
  the static records it writes each tick, one tick later at most for the car physics state. [UNCERTAIN] not proven
  for the Lua powerup state.

**Ranges that are legitimately different on the exe.** These are masked by address, not by page, because pages
are shared between families (`data_owner_map.tsv`; for example `0x00773000` is referenced by msvc-crt-main 84,
render 41, boot 27, input 22, save 19 addresses):
- bound families: msvc-crt-* (CRT globals, heap manager), third-party-library[renderware] state under
  `*(0x007d3ff8)` and the RW globals pages, d3dx9-*, directshow-baseclasses, qhull, lua-4.0, audio (`0x007dc000..0x007de000`)
- OS-filled words: `MSG` `0x007e95a0..0x007e95bb`, `0x006147d8`, `0x00771a00`
- stale stack: ring `0x007ec9e4` fields `+0x28..+0x3c`
- boot residue (1.9), unless both sides normalise it at the warp (row RO-WARP-NORMALIZE)

The family map in this row is page-level and from absolute operands only (674 of 812 pages have none: BSS arrays
reached through a register base). Row RO-MASKS turns it into a per-dword mask file.

**Output: first-divergence report.** `re/frida/detorig_ticks.py A B` already prints the first tick at which a block
that was equal starts to differ, with `--ignore` for masked ranges, `--explain` to classify the block's dwords from
the full dumps, and the raw words' first differing tick and values. The full report the oracle needs (row
RO-FIRST-DIVERGENCE):
1. tick, block from the hashes;
2. re-run both sides to that tick with full dumps at tick - 1 and tick: the differing dwords, both values;
3. the owning symbol: the containing global from hooks.csv / Ghidra labels (the `re/tools/xref.py` facts cache) and
   the functions that reference that address (`data_owner_map.py` at address level);
4. the last writer on the original from a hardware write watch on that dword from tick - 1 (`detorig.py --watch`,
   already built and used in LOADHOLD), and on the exe from the same debug-register watch in-process.

## 5. What exists and can be reused

| tool | gives today | missing for the oracle |
|---|---|---|
| `re/frida/detorig.py` + `js/detorig.js` | menu-less race warp, virtual clock (now also the tick clock), RNG pins, scripted player-0 input, full-image checkpoints, call log, write watch; this row adds `--clock tick`, `--tick-hash`, `--tick-raw`, `--q0`, the timer pin | install at process start; input keyed by tick (it is keyed by N); key-table and window-message pins |
| `re/frida/detorig_report.py` | checkpoint scorer: RAW / NORMALIZED / RACE-STATE with classes and writer rules | exe run directories (same meta format) |
| `re/frida/detorig_ticks.py` (this row) | per-tick block compare, IDENTICAL-FOR-N, NEW-block classes, raw-word first differences | address-level masks file; owner and writer lookup |
| `re/frida/detorig_calllog.py` | per-call-site histogram of clock calls | none for the oracle |
| `verify/savelink_20261023/run_arms.py` | exe arm runner with knob stacks, private root, gate recipes (G-CTRL, B, D2) | a replay arm kind (`RO_<tag>`) that sets `MASHED_REPLAY` and collects `ticks.bin` |
| MSD1 record dumps (`MASHED_RECDUMP`, `Vehicle/VehiclePhysicsRun.cpp:953`) | one car record per race frame after physics, exe side | the original side and the rest of the image; superseded by the per-tick hash for detection, kept for car-level reading |
| aistep CSVs (`AI_STEPDUMP` in `run_arms.py` recipes) | per-step AI internals, exe side | original side; useful as a second witness after a divergence is in AI |
| `re/frida/hooks_registry.py`, `run_diff_scenario_batch.py` | per-function A/B in one booted game | function-level, not whole-state; stays the per-hook route |
| `re/frida/race_draw_burst.py` | original draw-call totals in a race (render parity) | render is not compared by the oracle |
| `scenario_launch.py --watch-write`, `detorig.py --watch` | last writer of a dword, original side | exe-side equivalent |
| `record_attach.py` / `replay_session.py` | DirectInput keyboard buffer by render frame, replayable on the original | control-level, tick-keyed capture (RO-INPUT-RECORD) |
| D2 arms | wall-clock-bound exe arms | not a fit: the oracle is call- and tick-bound, not wall-clock-bound |

## 6. Tolerance mode (if DDKOBJ-SPIKE fails)

If the exe keeps VS2022-compiled bodies, float results differ from the first x87 rounding difference on, and an
address diff becomes a stream of float differences from tick 1.

Exact, even then:
- integer and state words: phases (`0x00771968`), race and car state machines, counters, indices, flags;
- RNG state and call count at each tick (CRT `ptd+0x14`, RW ring cursors). These stay exact only while control
  flow is identical, so they are the first control-flow alarm.
- the per-tick clock-call count (N per tick): a change means different control flow.

Scored, not compared:
- float dwords. Classify a dword as float by the declared type of its global (MGLOBAL types in `re/match/**`
  sources, struct plates) and, where untyped, by value.
- Score each tick by the largest ULP distance and the largest relative error per named field (car position
  `+0x958..+0x960`, velocity, orientation basis), against per-field bands in the style of `re/tools/b_envelopes.json`.

Verdict:
- **HARD FAIL**: the first tick where an integer, state, RNG or N word differs.
- **SOFT**: the per-tick float error curve until then, with the tick where a band is first exceeded.

The flip rule stays a decision for Mariano: an exact oracle can gate a flip by itself, a tolerance oracle only
narrows where to look.
