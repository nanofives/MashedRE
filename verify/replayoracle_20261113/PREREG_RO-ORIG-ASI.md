# PREREG RO-ORIG-ASI (2026-11-13)

Row: replay oracle 6. Move the boot-mode replay of `re/frida/js/detorig.js` (RO-MENU-REPLAY configuration: tick clock,
timer pin, RNG pins, warp at a fixed tick, scripted input, boot-counter PIN rows, per-tick block hashes, checkpoints)
into a replay mode of `mashed_re_dev.asi`, loaded at process start on the original, with the same `meta.json` /
`ticks.bin` / `ckptNN.data.bin` format. Kind: race-path harness row (no G-CTRL, no B, no 12 captures). Committed alone,
before any arm.

## Change under test
- `mashedmod/src/mashed_re/Core/ReplayAsi.cpp` (new, asi only, `asi_sources.rsp`): knob `MASHED_REPLAY_ASI=<cfg.txt>`,
  default OFF. With the knob set, `dll_main.cpp` calls `ReplayAsi_Init` and returns before `InjectHooks` (no game hook
  is installed). Init patches a JMP at the PE entry point. At the entry point, on the main thread, the stub restores
  the 7 entry bytes and installs, in the order of `detorig.js install()`:
  - player-0 script over `FUN_00497310` (inline JMP, trampoline = the 5 stolen bytes `push ecx; mov eax,[esp+8]`)
  - the three clock IAT slots `0x005cc2b4` / `0x005cc0c8` (timeGetTime / GetTickCount) and `0x005cc0c4` (QPC):
    naked stubs with the same instruction-level behaviour as `tgtStub` / `qpcStub` (main-thread test on `fs:[0x24]`,
    no stack write on the fast path, N count, `onDue` on N >= due, other threads jump to the real API)
  - the tick listener at the entry of `FUN_00493390` (inline JMP over `push ecx; push edi; call 0x004950b0`)
  - boot steps (1 ms / 15,000 counts per call) until the first tick, then tick-only (166,667 counts / 17 ms per tick)
- `mashedmod/src/mashed_re/dll_main.cpp`: the knob test.
- `re/frida/detorig.py`: `--asi` (no Frida): writes the config, sets `MASHED_REPLAY_ASI`, unsets
  `MASHED_RE_NO_AUTO_HOOK` (else `dinput8_shim.cpp` does not load the asi), Popen, waits for the done marker, builds
  `meta.json` from the asi's event log. Kills only the pid it launched.

## Arms
Private root `D:\MashedWT\RO-ORIG-ASI_root` (robocopy of `original\`), the asi built from this branch with
`MASHED_NO_DEPLOY=1` copied to `<root>\original\mashed_re_dev.asi` (never to the shared `original\`). Every launch inside
`mashed_lock.py run --label RO-ORIG-ASI` under `test-slot`. Configuration = RO-MENU-REPLAY's `run_arm.sh`
(`--clock tick --boot --q0 0xC0000000 --warp-tick 600 --tick-cap 4000 --normalize re/frida/warp_normalize.tsv
--load-sleep-ms 0`, the same `--tick-raw` ranges).

| arm | driver | note |
|---|---|---|
| f1, f2 | `detorig.py --boot` (Frida, asi not loaded) | references |
| a1, a2, a3 | `detorig.py --asi` | ASI replay |
| aneg | `detorig.py --asi --negctl 0x0067f17c --neg-tick 300` | negative control in the menu segment |

More f / a runs are added only to get a boot-class match (RO-MENU-REPLAY finding 2: tick-0 `0x007f1030` class and
`0x006147d8`). Every run is listed in the RESULT.

## Predictions (pass criteria)
1. Default OFF: without `MASHED_REPLAY_ASI` the asi takes the unchanged `InjectHooks` path (code inspection of
   `dll_main.cpp`; the Frida references do not load the asi at all).
2. Both builds compile (`MASHED_NO_DEPLOY=1`; object build `MASHED_DDKOBJ=1`). `ReplayAsi.cpp` is asi only, so
   `ddkobj.py check` is unchanged.
3. Format: every asi run writes `meta.json` (keys `boot`, `warp`, `race`, `ticks`, `ckpt` with 15 entries, `rc` 0),
   `ticks.bin` and `ckpt00..14.data.bin`, readable by `detorig_ticks.py`, `prov_compare.py` and `detorig.py --compare`
   with no change to those tools.
4. ASI vs ASI (same boot class): IDENTICAL-FOR-N over every tick record (600 menu + race ticks to the cap), strict.
5. ASI vs Frida (same boot class): per-tick IDENTICAL-FOR-N over every tick record, and checkpoints race-state equal.
   [UNCERTAIN] stale-stack fields (RULE:STACKROW / RINGSTALE / RWPAD, RESULT_RO-STACKROW-READERS.md): the asi stubs and
   Frida's Interceptor leave different stack contents below esp. A difference confined to those rule fields is
   reported as such (named dwords, rule class), not as a pass of prediction 5.
6. aneg: DIVERGED at tick 300 exactly, block `0x0067f000`.
7. No crash other than the U-9677 piz collision (re-run).

Verdict rule: GREEN when 2, 3, 4, 5 and 6 hold. RED on a NEW block outside the MASK set in a class-matched ASI vs
Frida pair, a missed negative control, or a format the tools cannot read.
