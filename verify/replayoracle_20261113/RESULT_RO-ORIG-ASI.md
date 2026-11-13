# RESULT RO-ORIG-ASI: the boot-mode replay of the original, in mashed_re_dev.asi instead of Frida

Date 2026-11-13. Worker session cmwem2z6q992qsc1bdhu5oa8u (claude2, claude-opus-5-5 medium), parent cmwejtp8k8scesc1b9ir2im1l.
Branch `orch/RO-ORIG-ASI` from `orch/integration` `145dec4d0`. PREREG `PREREG_RO-ORIG-ASI.md` (commit `34c5191ed`, alone,
before any arm). Raw data: `D:/MashedWT/RO-ORIG-ASI_data/<run>/`, SHA-256 of every file in `ro_orig_asi/raw_sha256.txt`.

## Verdict

**GREEN.** The asi replays the original with no Frida in the process. Its per-tick hashes match the Frida `detorig.py --boot`
reference on the same root, strict, over all 3,324 tick records (600 menu ticks + 2,724 race ticks).

| pair (prov_compare.py RUN REF REF, strict: no allowed block) | boot class `0x007f1030` | verdict |
|---|---|---|
| a1 (asi) vs f2 (Frida) | `39a80501` / `39a80501` | **IDENTICAL-FOR-N 3324 / 3324** (menu 600, race 2724) |
| a3 (asi) vs f2 (Frida) | `39a80501` / `39a80501` | **IDENTICAL-FOR-N 3324 / 3324** |
| a3 (asi) vs a1 (asi), and a1 vs a3 | `39a80501` / `39a80501` | **IDENTICAL-FOR-N 3324 / 3324** |
| aneg (asi, `--negctl 0x0067f17c --neg-tick 300`) vs a2 (asi) | `39a82829` / `39a82829` | **DIVERGED at tick 300**, block `0x0067f000` only (negative control, PASS) |
| class-mismatched pairs (a2 vs f1, a1 vs f1, f2 vs f1, aneg vs a1/a3/f2) | differ | DIVERGED at tick 0 in `0x007f1000` (the class dword) and `0x0067d800`: the RO-MENU-REPLAY boot-class rule, not evidence |

Checkpoints (`detorig_report.py --norm re/frida/warp_normalize.tsv`, 15 per run):

| pair | normalized | race state | unexplained | named classes |
|---|---|---|---|---|
| a1 vs f2 | 15/15 | 15/15 | 0 | MASK:os 3, MASK:crt-cookie 1, MASK:crt-clock 2, MASK:audio 6, RULE:HWND 4, RULE:WINMSG 4, RULE:FILEPTR 1, RESIDUE 3, PTR |
| a3 vs f2 | 15/15 | 15/15 | 0 | as above plus MASK:renderware 2, RULE:RING4 3 |
| a3 vs a1 | 15/15 | 15/15 | 0 | no RESIDUE |

Race anchor: dn_warp 109 (f1, f2, a1), 113 (a2), 108 (a3, aneg) main-thread clock calls. The load length differs between
runs and does not change the per-tick verdict, as REPLAY-ORACLE-DESIGN found for the tick clock.

## Prediction check (PREREG)

1. Default OFF: holds by construction. `dll_main.cpp` calls `ReplayAsi_Init()`, which returns false when
   `MASHED_REPLAY_ASI` is unset or `"0"`, and the hooks are then installed as before. With the knob set it returns true
   (no game hook installed) even when the config cannot be read. The Frida references do not load the asi
   (`MASHED_RE_NO_AUTO_HOOK=1` makes `dinput8_shim.cpp` skip it). No boot without the knob was run for this row.
2. Both builds compile with `MASHED_NO_DEPLOY=1`: default build OK, `MASHED_DDKOBJ=1` build OK. `ddkobj.py check`:
   IDENTICAL 1145/1175, DIFFERENT 11 (`0x00401690 0x0042f530 0x004332a0 0x0045a3a0 0x00475a00 0x004929d0 0x00493710
   0x00493900 0x00498810 0x004c0c20 0x004c0f00`), not-in-map 19, refusals 4 (wrapper-redefines-globals). RWC-PLUGIN-ATTACH
   recorded the same 11 RVAs before this branch. This branch changes only asi sources (`git diff orch/integration...HEAD
   -- mashedmod`: `asi_sources.rsp`, `Core/ReplayAsi.cpp`, `dll_main.cpp`). The rva-body lint passes (193 known, NEW 0).
3. Format: holds. Each asi run wrote `meta.json` (keys `boot`, `warp`, `race`, `ticks` with `phases`, 15 `ckpt`, `rc` 0),
   `ticks.bin` (3,324 x 5,551 dwords, the Frida layout: nblk 3,245 + raw 2,304), `ckpt00..14.data.bin` (3,321,864
   bytes each). `detorig_ticks.py`, `prov_compare.py`, `detorig_report.py` and `detorig.py --compare` read them unchanged.
4. ASI vs ASI, same class: holds (a1 / a3).
5. ASI vs Frida, same class: holds (a1 / f2, a3 / f2), per tick and at the checkpoints. The PREREG `[UNCERTAIN]` on
   stale-stack fields did not materialise: the ring blocks (`0x007ec800..`, RULE:STACKROW, not masked by prov_compare) are equal at every tick.
6. Negative control: holds, DIVERGED at tick 300 exactly, block `0x0067f000`.
7. No crash in 7 asi launches (smoke a1 + 6 arms) and 2 Frida launches.

## Two differences between the asi launch and the Frida spawn

Both are outside race state and are written by the OS or the launcher, not by game code.

1. **`0x00616658` is the CRT TLS index** (now a `MASK:os` row in `re/frida/warp_normalize.tsv`). Writer `0x004a8a16`
   `mov [0x616658], eax` after `call [0x005cc1fc]` (TlsAlloc) at `0x004a8a0d`. Reader `0x004a8830` pushes it as the
   TlsGetValue argument in `_getptd` `0x004a8828`, and the other references are in the same CRT code
   (`0x004a87fc..0x004a8a3c`, `0x005c2de6`, `0x005c2dfc`). Value 0x17 under Frida, 9 under the asi, at every checkpoint.
   The OS gives out the next free slot, so the count depends on which modules took slots before the CRT start (Frida's
   agent, or the asi's static CRT). It was the only value difference in block `0x00616400` (the other two differing dwords,
   `0x00616460` / `0x006164a0`, are heap pointers hashed as 0). Without the mask a1 vs f2 is DIVERGED at tick 0 in that
   block only. With it the result is IDENTICAL-FOR-N 3324.
2. **RESIDUE `0x00616100` / `0x00616120` / `0x00616140`**: the `_file` fields of the CRT stdin, stdout and stderr FILEs.
   They are -1 in every asi run and 0 / 1 / 2 in the Frida runs. `detorig.py --asi` starts the game with `subprocess.Popen`
   with no std handle passed, while Frida's spawn passes them. They lie in block `0x00616000`, which already holds masked
   dwords (`0x00616038` cookie, `0x00616110` RULE:FILEPTR) and is not scored per tick. ASI vs ASI shows no RESIDUE.
   [UNCERTAIN] Passing explicit std handles in `run_asi` would make these 0/1/2. That was not tried here.

## How it works (`mashedmod/src/mashed_re/Core/ReplayAsi.cpp`, asi only)

- Knob `MASHED_REPLAY_ASI=<cfg.txt>`, default OFF. `detorig.py --asi` writes the config as one `key values` line per
  setting (scenario, input script, checkpoints, PIN rows of `warp_normalize.tsv`, raw ranges, clock constants), sets the
  knob, unsets `MASHED_RE_NO_AUTO_HOOK`, launches with Popen, waits for `asi_done`, then kills only its pid and builds
  `meta.json` from `asi_events.jsonl`.
- `ReplayAsi_Init` (in the asi's DllMain) checks the 7 entry bytes (`push 0x60; push 0x5d1290` at `0x004a4bb7`) and
  patches a JMP there. The entry stub restores them and installs on the main thread. That is the same moment as
  `detorig.js` cfg.boot: the PE entry point, after the loader has bound the IAT and run every DllMain.
- Install order as `detorig.js install()`:
  - player-0 script over `FUN_00497310` (JMP, trampoline = `push ecx; mov eax,[esp+8]` + jmp `0x00497315`)
  - clock IAT slots `0x005cc2b4`, `0x005cc0c8`, `0x005cc0c4`: naked stubs that follow `tgtStub` / `qpcStub` instruction
    by instruction (no stack write on the fast path, other threads jump to the real API)
  - the tick listener at `0x00493390` (JMP over `push ecx; push edi; call 0x004950b0`, re-executed after the listener)
- The listener, `onDue`, warp, `pinRngs`, `pinTimer`, `pinNorm`, the race-anchor watch, checkpoints and the tick record
  layout are the `detorig.js` boot-mode code, ported line by line. FPU and XMM state are saved with `fxsave` around
  every call into C from a detour.
- Not ported (not used by the boot-mode reference configuration): `--prov` bodies and counters, write log, write watch,
  call log, crash catcher, poison, audio-off, `--warp-clock`, `--menu-ckpt`, rebase. These still run under Frida.

## Runs

Root `D:\MashedWT\RO-ORIG-ASI_root` = robocopy of `D:\MashedWT\RO-MENU-REPLAY_root\original`, the same binaries:
`MASHED.exe` `b9977dab...`, `d3d9.dll` `47901951...`, `dinput8.dll` `5f850f29...`, `mashed_qol.asi` `1567c95b...`. The asi in
the root is this branch's VS2022 build, `mashed_re_dev.asi` `18f57fb0a2244479a130d3106985dc96bc8b0fef89f82880a8281b8392ee1ca2`
(copy in `D:/MashedWT/RO-ORIG-ASI_data/mashed_re_dev_vs.asi`). The shared `original\` was not touched. Every launch ran
under `test-slot` with `mashed_lock.py run --label RO-ORIG-ASI` inside, through `run_arm_asi.sh` / `batch_asi.sh`.
The configuration is RO-MENU-REPLAY's: `--clock tick --boot --q0 0xC0000000 --warp-tick 600 --tick-cap 4000
--normalize re/frida/warp_normalize.tsv --load-sleep-ms 0`, with the same `--tick-raw` ranges.

| run | driver | ticks.bin SHA-256 (first 16) | meta.json |
|---|---|---|---|
| f1 | Frida `--boot` | `f0bdb0627017ce46` | `780c7d6d8e3a78d6` |
| f2 | Frida `--boot` | `79b43d655d7ea16d` | `8d75ff71f8c7e3eb` |
| a1 | `--asi` | `2c0f85a9d7ca280b` | `577adf9acf198443` |
| a2 | `--asi` | `799cc767555af1b3` | `eeea80a1534a99fd` |
| a3 | `--asi` | `2d60ba234c2594e5` | `4d939594c29d4cf3` |
| aneg | `--asi --negctl 0x0067f17c --neg-tick 300` | `e00cc2443333dd63` | `4ab43c5943d4e392` |

A smoke a1 before the arms ran to the end (15 checkpoints, 3,324 ticks). The driver then failed to read the event log,
because the asi's `fopen_s` denied shared reads. It was fixed (`_fsopen` `_SH_DENYNO` plus a wait after the kill) and
the asi rebuilt before the arms. Its output was overwritten by arm a1.

## Files (this branch)

- `mashedmod/src/mashed_re/Core/ReplayAsi.cpp` (new), `mashedmod/src/mashed_re/dll_main.cpp`, `mashedmod/asi_sources.rsp`
- `re/frida/detorig.py`: `--asi` (`run_asi`)
- `re/frida/warp_normalize.tsv`: `0x00616658` MASK:os
- `verify/replayoracle_20261113/`: `PREREG_RO-ORIG-ASI.md`, this file, `run_arm_asi.sh`, `batch_asi.sh`,
  `ro_orig_asi/meta/` (each run's `meta.json` and asi event log), `ro_orig_asi/reports/` (scores, prov_compare and
  detorig_report JSON, run logs, `ddkobj_check.log`), `ro_orig_asi/raw_sha256.txt` (130 files)

Gates: not run (race-path harness row: the asi's replay mode is off unless the knob is set, and `mashed_re.exe` sources
are unchanged). No tracker edit, no `ROADMAP.md` edit, no default flip, no change to `original\`.

## For the next rows

- RO-EXE-TICK-TRACE / RO-ORACLE-GATE can take references from `detorig.py --asi`. It is the same format, and no Frida
  agent is in the reference process.
- The boot class (tick-0 `0x007f1030`) still varies under the asi: a1 and a3 had `39a80501`, a2 and aneg had `39a82829`
  (Frida: f1 `39a84b51`, f2 `39a80501`). A reference set needs runs of each class or a re-run to match, as before.
