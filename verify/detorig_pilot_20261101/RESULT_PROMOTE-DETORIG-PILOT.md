# RESULT PROMOTE-DETORIG-PILOT: deterministic original (virtual clock + scripted input), pilot

Date 2026-11-01. Worker session cmvy4jzm04099p71hyhpc6fn6, lead cmvy4ht053yvdp71hgkb0x8a6.
Branch `orch/PROMOTE-DETORIG-PILOT`. Review: **TEXT-ONLY** (harness under `re/frida/`, no build input).

## Verdict

Pilot DoD **not met.** Whole-image snapshots never matched (0/15 in every pair), and the negative control was
not measured at equal load length. Delivered: the harness, the scorer, and the measured causes listed below.

**Partial.** Under the harness, the race state of MASHED.exe repeats exactly when the track load takes the same
number of main-thread clock calls. The load does not always take the same number: 113, 118, 119, 122 and 215
calls were measured, and runs whose load length differs diverge. The whole image never repeats byte for byte.
15 checkpoints over 30,000 main-thread clock calls (about 2,700 game frames), pairs whose load took 118 calls:

| verdict (definitions below) | OFF vs OFF | ON vs ON | OFF vs ON (2 C4 hooks live) |
|---|---|---|---|
| RAW, whole writable image byte-identical | 0/15 | 0/15 | 0/15 |
| NORMALIZED, every differing dword explained | 0/15 (1 to 3 dwords left per checkpoint) | 0/15 (1 to 3 left) | 0/15 (1 to 58 left) |
| RACE-STATE, named regions + RNGs + race clock | **15/15** | **15/15** | **15/15** |

Load length (main-thread clock calls from the warp to the first call that sees phase 3), per run: off_c 118,
off_d 118, on_a 118, on_b 118, neg_a 122, off_e 119, off_f 113, neg_b 215. Every pair with equal load length
is RACE-STATE 15/15 (6 pairs). Every pair with unequal load length is RACE-STATE 0/15 (off_e vs off_f, and
the negative-control pairs). `off_e` / `off_f` / `neg_b` also ran with the clock re-base at the anchor
(`--rebase`, see step 4). The re-base did not remove the divergence and adds a run-dependent jump in
`0x00771984`. It is now opt-in and off.

Load-hold runs (`off_g`, `off_h`, `neg_c`): **not run.** They were queued at 19:15 and cancelled by this worker at
20:30 still waiting on the game lock (solo windows and a pending merge train). No MASHED.exe was spawned.
To run them: `bash D:/MashedWT/PROMOTE-DETORIG-PILOT_data/run_arm.sh off_g --timeout 400` (same for `off_h`,
then `neg_c --negctl 1`), each under `test-slot`, then `detorig_report.py` on off_g/off_h and off_g/neg_c.

Before this harness (runs `off_a` / `off_b`, timeline anchored at the warp): **0/15 on all three verdicts**,
974 car-record dwords and both RNG states differing, frame counts 2 frames apart.

## What the harness does

`re/frida/detorig.py` (driver) + `re/frida/js/detorig.js` (agent), ported and cut down from SimGolf's
`re/frida/scenario_ab.py` + `re/frida/js/scenario_det.js`. Scoring: `re/frida/detorig_report.py DIR_A DIR_B`.

1. **Launch.** `Popen` MASHED.exe from the private root (`MASHED_ROOT=D:\MashedWT\PROMOTE-DETORIG-PILOT_root`),
   attach, wait for `DAT_00771968 == 1` (menu). Never `frida.spawn`. Kills only its own PID. Main thread = owner
   of the game window (`GetWindowThreadProcessId`).
2. **Virtual clock.** The IAT slots of `timeGetTime` (`0x005cc2b4`), `GetTickCount` (`0x005cc0c8`) and
   `QueryPerformanceCounter` (`0x005cc0c4`) point at hand-assembled native stubs. On the main thread
   (TEB+0x24 == main tid) a QPC call returns Q and adds `qstep` = 15,000 counts; a `timeGetTime` / `GetTickCount`
   call returns T and adds 1 ms; every call increments N, the virtual timeline. Other threads jump to the real
   API. A call that finds N >= due calls into JavaScript (`onDue`) on the blocked main thread; no other call
   enters JavaScript.
3. **Warp** at N = 600 after install, inside `onDue` on the main thread: the selection globals of
   `scenario_launch.py` (track 0, mode 10, rule 0, car 0, team 0), `FUN_0040e480(slot, 1|2|2|2)` (player + 3 AI),
   RNG pins, then `DAT_00771968 = 2`.
4. **Race anchor.** The first main-thread clock call that sees `DAT_00771968 == 3`. The RNGs are pinned again
   there, and every input event and checkpoint is scheduled at N relative to it. The rebase runs (`off_e`,
   `off_f`, `neg_b`, `--rebase`) also set T / Q there to the values of an anchor at N = 800. The load-hold runs
   (`--load-sleep-ms 50`, default from `off_g` on) sleep 50 ms of real time in every main-thread clock call
   between the warp and phase 3.
5. **RNG pins.** CRT `rand` (`0x005c229b`) keeps its state at `_getptd()+0x14` (`_getptd` = `0x004a8828`,
   called on the main thread): set to `0x12345678`. RenderWare's ring RNG `FUN_00534870`: the 31-dword ring
   is rewritten the way `0x005349cf..0x00534a6d` seeds it (`0x9a319039` at `0x00534a09`, LCG `0x41c64e6d` /
   `0x3039`), cursors reset, then `0x136` draws (`0x00534a46`) by calling `FUN_00534870` itself.
6. **Scripted input.** `FUN_00497310(player, control)` is replaced (`Interceptor.replaceFast`) by a CModule that
   calls the original and returns the scripted value for player 0 (default 0). Script relative to the race
   anchor: control 4 = `0xff` from 0; control 0 `0xff` 6,000..6,600; control 1 `0xff` 9,000..9,900; control 0
   `0xff` 12,000..12,300.
7. **Checkpoints** every 2,000 N after the race anchor (15). Each dumps `0x005ea000..0x00915008` (`.data` +
   `_rwdseg`, 3,317,768 bytes) plus T, Q, N, `DAT_007f0ff0`, `DAT_007f0ff4`, `DAT_007f101c`, the CRT state,
   the RW ring and cursors, and the first code byte at `0x00468980` / `0x00470670` (witness, runs after on_a).

### Call rates and mechanism (planner note)

Measured on `off_a` (`stats` lines, 5 s apart): main-thread QPC **about 640 calls/s, 11 per game frame**;
main-thread `timeGetTime` / `GetTickCount` **0 calls** (T never moved from its start value in any run);
calls forwarded from other threads to the real API **about 60/s**; game at about 59 fps (`DAT_007f0ff0`
+50 per frame). `DAT_00771e70` (QPF) = 10,000,000, so one virtual frame = 11 x 15,000 / 10^7 = 16.5 ms.

Chosen mechanism: **native code on every clock call, JavaScript only at scheduled events.** The stubs are
x86 bytes in a Frida-allocated page; the input override is a TinyCC CModule. No `Interceptor.attach` runs on a
hot path, so the ~1,000 calls/s Frida limit does not apply: 0 crashes or hangs in 12 runs (probe1, off_a..f,
on_a, on_b, neg_a, neg_b). An `.asi`-side port of the same stubs is the next step only if the harness has to
run without Frida (see Next).

## Determinism measured

Definitions (`detorig_report.py`):

- **PTR**: both values are 4-aligned and in `[0x01000000, 0x30000000)`, the heap range on this machine. The
  heap starts `0x40000` apart between OFF runs and at non-page offsets when the `.asi` installs hooks.
- **RESIDUE**: the dword holds one value through all 15 checkpoints in both runs, so it was last written
  before the race window.
- **OFFSET**: a - b is the same at every checkpoint. **MODOFFSET/p**: (a - b) mod p is the same, p >= every value.
  **TOGGLE**: both runs only take values 0..3.
- **RACE-STATE**: car records `0x008815a0` (4 x `0xd04`), frame timer `0x007f0ff4..0x007f1034`, cooked input
  `0x007f1038` and `0x007f14f8` (4 x `0x4c`), A5 scratch `0x00881560`, pickup slots `0x0088fbe0` (64 x `0xb4`),
  `0x00771984`, plus CRT state, RW ring and cursors, `DAT_007f0ff4`, `DAT_007f101c`, phase. PTR dwords and
  `0x007f102c` excluded. `0x007f102c` is written at `0x00493478` from a QPC sample; it holds the real QPC value
  sampled before the clock install in every run, because virtual Q stays below it and the branch at
  `0x00493451` never fires again.

### OFF vs OFF (off_c vs off_d, race-anchored, no rebase)

- Race anchor at the same N in both: 718 (118 calls after the warp), frame index 0.
- RACE-STATE **15/15**. Car positions at `+0x958` are equal to the last bit at every checkpoint and move
  (car 0 x/z = 0.123/-9.387 at checkpoint 0, -0.102/26.989 at 8, 10.884/-8.181 at 14).
- Whole image: 3,838 PTR, 35 RESIDUE, 7 OFFSET, 98 MODOFFSET (97 with p = 60, 1 with p = 4), 2 TOGGLE, 3 UNEXPLAINED.
- **What differed** (mechanical, no semantic claimed):
  - `0x007f0ff0` (+50 per frame) differs by 1,100 = 22 frames: the number of frames run between boot and the
    clock install. The 7 OFFSET dwords (`0x00616110`, `0x0077198c`, `0x007719a4`, `0x007d7108`, `0x007dc958`,
    `0x007dca50`, `0x007f0ff0`) and the 97 period-60 MODOFFSET dwords (`0x00771978` and 96 dwords from
    `0x00868348` at stride `0x44`, 38 apart mod 60 at every checkpoint) step alike in both runs from different
    starting values.
  - 35 RESIDUE dwords, among them `0x00616038` (written by `FUN_004a57e4` from GetSystemTime / PID / TID /
    GetTickCount / QPC), `0x00771e78` / `0x00772fb4`, and frontend floats at `0x00636ae8`, `0x0067d858`, `0x0067d974`.
  - 3 UNEXPLAINED: `0x00771994` (steps of 9,000 / 13,500 / 18,000, wraps near 540,000; a - b is 346,500 at
    checkpoint 0 and 342,000 at the other 14) and `0x007d70d0` / `0x007d70d8` (`0x5178` or `0x6c48` written into one
    or the other at 2 checkpoints; neighbour `0x007d70c8` is a MODOFFSET/4 index 2 apart). [UNCERTAIN] the
    writers of these three were not identified.
- Run `off_a` vs `off_b` (anchor at the warp, before step 4 existed): the track load took a run-dependent
  number of frames (`DAT_007f101c` 169 vs 167 at checkpoint 0), and from there 0/15 on every verdict.

### ON vs ON and OFF vs ON (MASHED_HOOK_ONLY=0x00468980,0x00470670)

The hook set is `A6b_Entry` `0x00468980` (VehicleAeroStabilizer, C4) and `A4_Entry` `0x00470670`
(VehicleControlUpdate, C4), both in `hooks.csv` as C4 / race / green. The `.asi` is the copy in the private
root, SHA-256 `906f8601201f69b4670bfef68b438b719e515e0e4e523f892d92c9f095b8a8ad`. `on_b` proves the hooks
were live: its manifest lists `514 0x00470670 1 A4_Entry` and `517 0x00468980 1 A6b_Entry`, and the witness
byte at both RVAs is `0xE9` at the anchor and at all 15 checkpoints. In OFF runs the bytes are `0xD9` / `0x51`
(original prologues). `on_a` ran before the witness existed and has the same results as `on_b`.

- on_a vs on_b: RACE-STATE **15/15**, NORMALIZED leaves 1 to 3 dwords per checkpoint (the same 3 addresses as
  OFF/OFF plus `0x007e95b0`).
- OFF vs ON (off_c or off_d vs on_a or on_b, 4 pairs): RACE-STATE **15/15** in every pair. Car records, pickups,
  input, RNGs and race clock are the same with the two hooks live.
- The whole image shows the hook set: 82 UNEXPLAINED dwords. Per checkpoint the count runs 1, 3, 4, 10, 16, 31,
  32, 33, 35, 45, 29, 34, 34, 47, 58, the same sequence for on_a and on_b. 77 of the 82 are field `+0x10` of
  rows at stride `0x40` in `0x007ec000..0x007f0000`. OFF stores `0x00470948` there and ON stores `0x008845d4`.
  `0x00470948` is the return address of `call 0x00468980` at `0x00470943` inside `FUN_00470670`, the call that
  both hooks replace. `0x008845d4` = car record base + `0x3034`. [UNCERTAIN] the writer of these rows was not
  identified. The values are consistent with the rows copying a stack dword that the original leaves holding a
  return address. That reading is not proven.
- So these two C4 hooks are bit-identical to the original in race state, and differ from it in one table
  field whose contents depend on the stack.

### Negative controls

- `neg_a` (no rebase, +1 on car 0's `+0x958` float at the anchor): **confounded, not counted.** The +1 was
  overwritten: car positions equal off_c at all 15 checkpoints. In the same run the load took 122 calls
  instead of 118. The race therefore started at a Q 4 x 15,000 counts later, and 681 to 682 dwords differ
  (pages `0x007ea000`, `0x00828000`, `0x0089a000`). This exposed the defect that step 4's rebase fixes.
- `neg_b` (rebase, +1 on the RW RNG word at the ring cursor at the anchor): RACE-STATE 0/15 against off_e and
  off_f (RW ring differs from checkpoint 0 on). Its load took 215 calls against 119 / 113, so this pair does
  not separate the control from the load-length divergence either.
- `neg_c` (load hold, RW RNG word +1): not run (see above). **No negative control has yet been measured at
  equal load length**, so the harness's ability to see a changed value in RACE-STATE is not shown by this pilot.

## Limits and open items

- **Boot-to-install residue.** The clock is installed at the menu, so the number of frames before the warp is
  real-time dependent (22 frames between off_c and off_d). That accounts for every OFFSET / MODOFFSET /
  RESIDUE dword. Installing before the first frame (create the process suspended, or the `.asi`) would make
  the whole image comparable without classes. Not done in the pilot.
- **Load length is the open blocker.** Phase 2 took 113 to 215 main-thread calls. The phase-3 anchor and RNG
  pins align runs whose load lengths match, and do not align runs whose load lengths differ. [UNCERTAIN] What
  the main thread waits on during phase 2 was not identified. The candidates are the threads that forward
  real clock calls (about 60/s); no RVA has been traced. The load hold (50 ms per main-thread clock call
  during phase 2) tests whether giving other threads time removes the variance.
- **Pointer class is a heuristic** (range + alignment). A float below 4.7e-10 with the low two bits clear would
  be taken for a pointer. RACE-STATE compares the named regions after excluding PTR pairs only.
- Coverage: one scenario (Training, QuickRace, 4 cars, about 45 virtual seconds, scripted player input). Audio
  was muted (`MASHED_MUTE=1`) as in `scenario_launch.py`. Audio state is not in the verdict.

## Next

- Make the load length repeat. If the load hold (`off_g` / `off_h`) does not, trace what the main thread polls
  in phase 2 (`FUN_004929d0` case 2 -> `FUN_0040d440`), e.g. with a `--call-log`-style return-address log on the
  clock stubs (SimGolf's `clockLog`).
- Until then, the RACE-STATE A/B can serve as a C4 scenario route (RW, RW-Physics) only for runs paired by load
  length: `detorig.py` records it (`meta.json` -> `race.dn_warp`), so re-run an arm until its load length matches.
  Before each batch, re-run a negative control at equal load length so the harness is shown to see a changed value.
- For whole-image equality, also install the clock before the first frame (suspended create, or move the stubs
  into the `.asi`) and identify the writers of `0x00771994`, `0x007d70d0/d8` and the `0x007ec000` rows.

## Evidence

Committed here: this file, `reports/*.json` (detorig_report output per pair), `meta/*.json` (per-run checkpoint
metadata), `raw_sha256.txt`. Raw checkpoint dumps (15 x 3.3 MB per run) and run logs stay in
`D:/MashedWT/PROMOTE-DETORIG-PILOT_data/<run>/`, hashed in `raw_sha256.txt`.

Binaries in the private root: `MASHED.exe` `b9977dab70607720cf58bc7832937c203b1e0b33ff5896da586baf56aa79fe87`
(the patched install, `.unpatched` anchor unchanged), `d3d9.dll` `479019513e752bf68b6c4cb77fa2cc36d69fdcfb5be2c56fb87b295d0c88c7be`,
`mashed_qol.asi` `1567c95b4fdb38f27a1313f3857983f707867c0700b5ec99cc4988b57bf59df0`, `mashed_re_dev.asi` as above.
No change to `original\MASHED.exe`, no unlock patch, no redeploy of `original\mashed_re_dev.asi`.

Gates: not run. The harness is under `re/frida/` and changes no build input, so the exe gate hashes cannot move.
