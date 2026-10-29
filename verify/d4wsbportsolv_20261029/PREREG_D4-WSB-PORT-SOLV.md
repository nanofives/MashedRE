# PREREG D4-WSB-PORT-SOLV

Committed alone, before any arm. Branch `orch/D4-WSB-PORT-SOLV` off `orch/integration` @ `34235b24c`.
Input: the 18 `D4-WSB-PORT-SOLV` rows of `verify/d4wsb_mappeddisp_20261028/disposition.tsv` (all C2, census EXEC).

## Scope: 14 rows ported or wrapped, 4 rows not ported

| rows | what this step does |
|---|---|
| `005663a0 005672c0 00567350 00567630 005676f0 00567c40 005687a0 005687d0 00568860 00568c40 00568c80 00568cf0 00568d20` (13) | Lane-2 verbatim ports, `re/tools/decomp2port.py --emit-dir Lane2 --apply` from `decomp_pc.py --port` (pool0, 2026-10-29), one TU each `mashedmod/src/mashed_re/Lane2/L2_<rva>.cpp`, shadow wrapper emitted by the tool (`ShadowAB::Run` for the pure returns, `RunTracked` for the writers). No hand edit. |
| `0056a450` (1) | already ported (`Collision/RwpVtableKV1.cpp`, KV Batch B). This step adds a `ShadowAB::RunTracked` wrapper under `#ifndef MASHED_STANDALONE`. |
| `0056a7a0 0056aae0 0056ac40 0056adb0` (4) | **not ported.** `decomp2port.py` refuses `0056aae0` (CALLEE_ARITY), `0056ac40` (float10 return), `0056adb0` (CALLEE_REG_ARG). It accepts `0056a7a0`, but that body reads `in_stack_00000020..0000009c` past its 4 declared params (an outgoing frame built by `0056aae0`, REP MOVSD of 0x22 words, see the deferral note at `RwpVtableKV1.cpp:11-25`), so a transcription would read stack garbage. Each needs its frame recovered from disassembly. They stay C2, handed off. |

## Default-OFF knob

- The 13 Lane-2 TUs register as `L2_FUN_<rva>`. `HookSystem::InstallAll` skips `L2_` names unless `MASHED_HOOK_ONLY`
  lists them or `MASHED_HOOK_LANE2=1` (`Core/HookSystem.cpp:244-253`). They are in `asi_sources.rsp` only, not in
  `exe_sources.rsp`. So a default `.asi` run and `mashed_re.exe` do not carry them.
- `0056a450`: the wrapper and the two Shadow includes are `#ifndef MASHED_STANDALONE`. `cl /EP /DMASHED_STANDALONE` of
  `RwpVtableKV1.cpp` before and after the edit gives identical text (73,456 non-blank lines, no diff), so
  `mashed_re.exe` compiles the same body. Unarmed (`MASHED_SHADOW_AB` unset), the `.asi` wrapper calls the same body.
- Nothing flips a default. No exe source changes.

Build `MASHED_NO_DEPLOY=1` in the worktree: `mashed_re.exe` `d00f585c...4b0a`, `mashed_re_dev.asi` `60bec0a8...9831`.
The `.asi` is copied to the private root `D:/MashedWT/D4-WSB-PORT-SOLV_root/original/` only, never to `original/`.

## Boot check (done before this PREREG)

2026-10-29 05:0xZ: endpoint `OK Philips FTV (NVIDIA High Definition Audio)`, `scenario_launch.py --track 0 --mode 10
--cars 4 --hold 15` on the private root, no hooks: race running (phase 3), 0 Event-1000 MASHED crash events, endpoint
still present after. Transcript `D:/MashedWT/D4-WSB-PORT-SOLV_data/bootcheck.txt`.

## Arm S (shadow A/B, one hook installed alone per boot)

`verify/d4wsbportsolv_20261029/drive_ps.py shadow <rva...>` (copy of `verify/wsbstackonly_20261028/drive_so.py`,
runner name, data path and label changed) around `run_shadow6.py` (copy of `run_shadow5.py`, data path and label
changed), wrapped in the test slot, each boot under `mashed_lock.py`. Order: the 14 rows in address order.

Env on every boot: `MASHED_ROOT=D:/MashedWT/D4-WSB-PORT-SOLV_root`, `MASHED_SHADOW_AB=1 MASHED_SHADOW_PRIVATE=1
MASHED_SHADOW_STW=1`, `TRACK=0`, mode 10, 4 cars, `HOLD=60`, `SAMPLES_DONE=24`. This is the census scenario
(the one that marked these rows EXEC). `scenario_launch.py --hooks 0x<rva>` sets `MASHED_HOOK_ONLY`, which installs
the `L2_` hook.

Audio rule: before each launch `Get-PnpDevice -Class AudioEndpoint -PresentOnly` must list an endpoint. If not, or if
an Event-1000 fault at `0x005baf48` appears: stop, BLOCKED "audio endpoint", no retry. A U-9677 crash
(`MASHED.exe+0x1509b4` / `+0xe66db`) is re-run (max 2), not a finding.

## Read-out per row (fixed now, from `re/tools/shadow_ab_report.py`)

- **C2 -> C3**: verdict `CLEAN` (n > 0 samples, ndiff 0 on every sample, `PATCHBYTE ... A/B-IS-REAL`). Promoted through
  re-classify with the log path, sample count and proof line. Heap pages compared are recorded for a later C4 leg but
  do not gate C3.
- **Stays C2, dated note**: `DIVERGENT` (offsets reported), `DIVERGENT_FLOAT10`, `UNPROVEN`, `SKIPPED`, `NO_SAMPLES`,
  `NO_LOG`, or a non-U-9677 crash with the hook armed (crash reported with its offset).
- **No C4 in this step.** C4 needs the race diff and the write-set leg (`re/CONFIDENCE.md`, L-38/L-40/L-54).
- The 4 unported rows: no arm, stay C2.

## Default gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on this build's `mashed_re.exe`, test slot +
machine lock, must equal `re/orchestrator/merge_train.json` `gctrl.ref` / `b.ref`. A mismatch is a RED for the step.

## Amendment 1 (2026-10-29, after rows 1-2 of arm S, before any re-run)

Arm S rows 1-2 ran on build `.asi 60bec0a8`: `0x005663a0` CLEAN 24/24 `A/B-IS-REAL installed=e9 uninstalled=8b`,
`0x005672c0` NO_LOG (race running for the full 60 s hold, no `shadow_ab.log` written). Cause, read from source: the
generated wrappers use `SHADOW_AB_COUNTER(..., ShadowAB::kPhaseRace)`, and `Core/ShadowAB.h:159` skips the sample while
`*(u8*)0x00771968 < 3`. The census puts 12 of the 14 rows in phase `race_load` (phase 2), so they cannot sample.
Row 3 (`0x00567350`) was stopped mid-boot (its PID and my runner PIDs killed by PID). Its data is not used.

Change (commit `f866c7dcc`, harness gate only, bodies unchanged): the 12 race_load wrappers use `kPhaseAny`.
`0x005663a0` and `0x0056a450` (census `race_run`) keep `kPhaseRace`. New build `MASHED_NO_DEPLOY=1`:
`mashed_re.exe` `c150342c...1ad2`, `mashed_re_dev.asi` `b99d6d3a...dc8f`.

Re-run: arm S as written above, on the new `.asi`, rows `005672c0 00567350 00567630 005676f0 00567c40 005687a0
005687d0 00568860 00568c40 00568c80 00568cf0 00568d20 0056a450`. `0x005663a0` keeps its row-1 result (its wrapper
and body are unchanged by `f866c7dcc`). Read-out unchanged. The SAMPLES_DONE guard stays 24: a race_load row that
samples fewer than 24 calls runs the full hold, and any n > 0 with the CLEAN verdict is read as written.
