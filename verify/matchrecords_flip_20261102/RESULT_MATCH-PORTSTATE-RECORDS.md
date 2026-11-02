# RESULT MATCH-PORTSTATE-RECORDS: L-88 = a flip (MASHED_PORTSTATE_RECORDS default ON)

Verdict: **BLOCKED** (gates not complete). D3D9 CreateDevice has failed on LIVING-PC for every boot
since 2026-11-02 09:19 local (hr 0x8876086A, all 8 device/depth combinations, the merge train's own
boots included). The flip commit is built and ready. The gates need one clean re-run.

## What landed on orch/MATCH-PORTSTATE-RECORDS

1. `c1476319c` merge of orch/integration `6a997bfa3` into the RECORDS branch (`d20783166`), with
   `git -c merge.autoStash=false`. No conflicts.
2. `4fc0b3e6b` the flip. `Vehicle/RecordBlockState.cpp` `PortStateRecords_On()` now returns true unless
   `MASHED_NO_PORTSTATE_RECORDS` is set (the opt-out, the pre-flip default). `MASHED_PORTSTATE_RECORDS` is
   still accepted and does nothing. Comments in `RecordBlockState.h`, `Race/CarRegistrant.cpp`,
   `Vehicle/VehicleInit.cpp`, `Vehicle/VehiclePhysicsRun.cpp` say default ON.
3. This commit: `verify/savelink_20261023/run_arms.py` gains `STACK['NPSR'] = ['MASHED_NO_PORTSTATE_RECORDS=1']`
   (arm `H_NPSR`, opt-out check: expected gates.csv `ba407a16`), plus this RESULT.

Build: `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"`, Build OK, `original\mashed_re_dev.asi` not
deployed. Gated exe copied out: `D:/MashedWT/MATCH-PORTSTATE-RECORDS_data/exe_flip/mashed_re.exe`
(sha256 `197fe5b243dfd9ead6656c0a078e46339dc752a4768ada8dc86e9e53ea39dba0`).

## Gates

Expected (L-88): G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3.

| Try | Arms | Result |
|---|---|---|
| 1 (`arms_flip`) | H_DEF | ran 117 s, self-exit 0, but **truncated**: step 2228 / gates 7149 / ptrace 3574 lines (full run: 13962 / 54085 / 27042). The device was lost mid-run. |
| 1 | B_DEF_K0/K1/K26, H_NPSR | exit 3 in 1-2 s, `InitD3D9: CreateDevice(...) -> fail hr=0x8876086A` x8 |
| 2-16 (`arms_flip2` .. `arms_flip16`) | all five | every boot exit 3, same CreateDevice failure. Tries every ~4 min, 09:20 to 10:22 local. |

Partial witness from try 1 (not a gate): the truncated H_DEF prefix is byte-identical to the knob-ON
reference `H_PSR` from the RECORDS PREREG run (`D:/MashedWT/MATCH-PORTSTATE-RECORDS_data/arms_g2`, whose full
hashes are `8284be46/67931834/31fe043e`):

| File | Lines compared | flip H_DEF | g2 H_PSR (knob ON) | g2 H_DEF (old default) |
|---|---|---|---|---|
| step.csv | 2227 | 8fbf0289 | 8fbf0289 | 8fbf0289 |
| gates.csv | 7148 | 1f95250d | 1f95250d | da73e0fb |
| ptrace.log | 3573 | 362a11d5 | 362a11d5 | 362a11d5 |

The `[portstate-records]` witness lines in try 1 report `base=008815a0 vis0=0063d9e0 dbe0_bad=0`, so the knob
is ON by default in this exe.

Environment evidence: System log Kernel-Power 566 at 09:19:18 ("session changed 16 to 18", reason InputHid).
The merge train's pass `D:/MashedWT/mtrain_data/20261102_091940/arms/*.log` shows the same 8 CreateDevice
failures. The previous train pass (`20261102_085559`, 08:55-09:17) created its device OK. A
`SetThreadExecutionState(ES_DISPLAY_REQUIRED)` plus a zero mouse move at ~10:00 did not restore it. Console
session 1 is Active. This needs someone at LIVING-PC (display / TV on, session unlocked).

Not run: the full set (vsum / e-check on the B arms, D2 x3, 13 tracks). They need the same device.

## To finish (any worker, once a boot creates its device)

```
bash D:/MashedWT/MATCH-PORTSTATE-RECORDS_data/gates_flip.sh   # edit arms dir first; exe_flip is the gated exe
```
GREEN = H_DEF `8284be46/67931834/31fe043e`, B_DEF_K0/K1/K26 `1d5bf4f9` x3, H_NPSR gates.csv `ba407a16`.
Then `roadmap.ps1 set MATCH-PORTSTATE-RECORDS review "SOURCE -Full FLIP ready @ <sha>, expected G-CTRL 8284be46/67931834/31fe043e"`.

## Raw files (not committed)

| Path | SHA-256 |
|---|---|
| D:/MashedWT/MATCH-PORTSTATE-RECORDS_data/exe_flip/mashed_re.exe | 197fe5b243dfd9ead6656c0a078e46339dc752a4768ada8dc86e9e53ea39dba0 |
| D:/MashedWT/MATCH-PORTSTATE-RECORDS_data/arms_flip/H_DEF.step.csv (truncated) | 92e2496495b5f32f62d6c571bbe23d7cb5f9f847d7c5661c7238527fbf8bf36c |
| D:/MashedWT/MATCH-PORTSTATE-RECORDS_data/arms_flip/H_DEF.gates.csv (truncated) | 5c3bc8093ae9fdd7352cb97eece78765562c93ebfdcc0ff1e5f65b3a2332c6a3 |
| D:/MashedWT/MATCH-PORTSTATE-RECORDS_data/arms_flip/H_DEF.ptrace.log (truncated) | 3c6d7e3b092345286cc1300940134767fd8b95b1e311de18b0dff7c3cdcaae87 |
| D:/MashedWT/MATCH-PORTSTATE-RECORDS_data/arms_flip/H_DEF.log | 43dfb8f7643c5a6c119e6f4a5a62ee13c1ebdb54b9ff2a733b4ab1e3661ca3cd |
| D:/MashedWT/MATCH-PORTSTATE-RECORDS_data/arms_flip/B_DEF_K0.log (CreateDevice failure) | 780ccd1d97ecb2e8f1e863bbe50717b61882ecb7eb79ca48351e846883ece03a |
| D:/MashedWT/MATCH-PORTSTATE-RECORDS_data/gates_retry.log | 695cbfa0bd873eddde1fb9cb54643d9005e3aa6d9df64622c1bc85695b0473f5 |
