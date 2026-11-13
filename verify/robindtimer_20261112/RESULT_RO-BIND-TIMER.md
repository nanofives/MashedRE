# RESULT RO-BIND-TIMER (2026-11-12)

Verdict: GREEN on the scope this row binds (0x00493480 / 0x00493390 under `MASHED_OT_TIMER`).
0x004926c0 is bound behind its own sub-knob `MASHED_OT_AUDIOTICK` (default OFF) and stops on frame 0 by design,
see "0x004926c0 finding".

Self-test: `[OT_TIMER] self-test PASS (0 of 6000 checks failed), 0x00493480 calls 600` on both the VS2022 build
(vs_on) and the object build (ddk_on).

## Commits
- f54dbbbb1: exe-only `Match/PortTop_00493390.cpp` (matched 00493390.c, callee 0x004950b0 -> QpcTimeScaledTo3Mhz),
  bindings in `MatchS15_00493480.cpp` / `MatchS15_004926c0.cpp`, `MatchS16_00496930.cpp` in exe_sources.rsp,
  dispatchers + per-call self-test in `Match/PortTop_00492370.cpp`, BINDINGS.tsv rows.
- 684d9a73b: 0x004926c0 moved to sub-knob `MASHED_OT_AUDIOTICK`. `MASHED_OT_TIMER` binds 0x00493480 / 0x00493390 only.

## Builds (at 684d9a73b)
- VS2022: Build OK (`D:/MashedWT/RO-BIND-TIMER_build_vs2.log`).
- Object build (`MASHED_DDKOBJ=1`): Build OK, ddkobj check IDENTICAL 716/798, DIFFERENT 79, not-in-map 3,
  units linked 788/836 (`D:/MashedWT/RO-BIND-TIMER_build_ddk2.log`). None of the 79 DIFFERENT is one of this row's units.
- BYTECHECK.tsv, this row's five units, all IDENTICAL, diff_bytes 0:

| rva | exe symbol | exe VA | size |
|---|---|---|---|
| 0x004950b0 | `_QpcTimeScaledTo3Mhz` | 0x00015730 | 96 |
| 0x00496930 | `_Table773030Get` | 0x00015790 | 16 |
| 0x004926c0 | `_AudioTickAndAvg` | 0x000157d0 | 176 |
| 0x00493480 | `_FpsDiscretise` | 0x000158b0 | 192 |
| 0x00493390 | `_PortTop_Bound_00493390` | 0x00015970 | 240 |

## Knob boots (run 2, after the LIVING-PC reboot)
`test-slot` outside, `mashed_lock.py run --label RO-BIND-TIMER` inside (queued behind RO-MENU-REPLAY, then held
`mashed_machine`). Root `D:/MashedWT/RO-BIND-TIMER_root`, `MASHED_DET_FRAMES=600`. Output `arms_run2.out` (this folder),
logs `D:/MashedWT/RO-BIND-TIMER_data/*.log`.

| arm | exe | knobs | result |
|---|---|---|---|
| vs_off | VS2022 | none | rc 0, 12.4 s |
| vs_on_knoboff | VS2022 | ORIG_TOP | rc 0, 600 frames |
| vs_on | VS2022 | ORIG_TOP + OT_TIMER | rc 0, 600 frames, self-test PASS 0 of 6000, 0x00493480 calls 600, 0x004926c0 calls 0 |
| ddk_on | object build | ORIG_TOP + OT_TIMER | rc 0, 600 frames, self-test PASS 0 of 6000, bodies at 000158B0 / 000157D0 / 00015970 (= BYTECHECK exe VAs) |
| ddk_on_knoboff | object build | ORIG_TOP | rc 0, 600 frames |
| vs_audio | VS2022 | ORIG_TOP + OT_TIMER + OT_AUDIOTICK | expected STOP: rc 0x0045D3F0 after 1.8 s, `STOP: the matched 0x004926c0 called original 0x0045D3F0 (DAT_006147b8 = 1)` |

## 0x004926c0 finding
`DAT_006147b8` is file-initialised to 1 in .data, so the original runs the audio branch of 0x004926c0 on every frame.
Its callees have no exe body yet. Binding 0x004926c0 on the default ORIG_TOP path needs bodies for:
- 0x0045d3f0 (callees 0x00550be0 / 0x005a60b0)
- 0x00466b50 (57 callees)
- 0x0045d430 (callee 0x005a60e0)

Until those land, `MASHED_OT_AUDIOTICK` stays OFF and 0x004926c0 runs as before.

## Notes
- Run 1 (f54dbbbb1) had 0x004926c0 under `MASHED_OT_TIMER`, and vs_on / ddk_on stopped on frame 0 at 0x0045D3F0.
  That is what moved it to the sub-knob in 684d9a73b.
- The first object build of run 1 ran while a merge-train PRIORITY-HOLD appeared (the check at its start missed it).
  The run-2 builds and boots did not overlap a hold: the boots ran under `mashed_lock.py run`.
