# RESULT D4-DC-77b0-BODYDIFF (text-only)

Verdict: GREEN. Every difference below is cited by file:line or RVA. No code, tracker or .ps1 changed. Static source read only: no MASHED.exe run, no Ghidra, no decomp_pc.py. Original-side facts are quoted from the commented listing addresses inside the two ports and from `re/analysis/race_rules_d1/0x004177b0.md` and `0x00408610.md`. Where a claim needs the original bytes it is marked [UNCERTAIN] with the missing evidence.

Pointer correction: the brief cites `Render/TrackRenderer.cpp:8715-8731`. The file is `mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp` (same lines).

Files compared
- A = `.asi` body `Ai/AiPreTick.cpp:88-244` (`AiPreTick`, 0x004177b0, 157 lines, calls the original leaves and reads constants from the live image).
- E = exe body `Ai/AiStandalone.cpp:1682-1823` (`AiPreTickRubberBand`, 142 lines, host stubs, literal constants, harvested tables).

## Part 1. Line-by-line table, A vs E

Control flow is identical in all five phases (same branch order, same loop bounds, same stores). Rows below mark where a value comes from, plus the two behaviour differences (rows 30-33).

| # | Phase | A (AiPreTick.cpp) | E (AiStandalone.cpp) | Difference |
|---|---|---|---|---|
| 1 | entry gate | `TargetEnable()` = call 0x00443080 (A:66,93; 0x004177b6/0x004177c4) | `s_host.ai_target_enable()` (E:1684) | Call to original vs host stand-in. E's comment names FUN_00443080. [UNCERTAIN: needs the host stub body vs the 0x00443080 listing] |
| 2 | p2 speed base | `SpeedBaseGet()` = call 0x0046dd80 (A:67,98; 0x004177d2) | `GearConstGet()` = `F32(0x0061313c)` (E:1642,1689) | Call vs direct read of 0x0061313c. [UNCERTAIN: 0x0046dd80 body not read] |
| 3 | p2 speed set | `VehSpeedSet(v,spd)` = call 0x0046dd90 (A:68,99; 0x004177dc) | `GearConstSet` = `F32(0x008815a0+v*0xd04+0x154)=f` (E:1643,1690) | Call vs direct store. [UNCERTAIN: 0x0046dd90 body not read] |
| 4 | p2 lap term | `PerVehA20(v)` = call 0x00407a20 (A:69,100; 0x004177e2) | `I32(0x008a9648+v*0x30c)` (E:1691) | Equivalent by `Gameplay/RangeTable_ah1.cpp:240-245`: 0x00407a20 returns `(&DAT_008a9648)[idx*0xc3]`, and 0xc3*4 = 0x30c. |
| 5 | p2 race angle | `RaceAngleRaw(v)` = call 0x00408ad0 (A:70,102; 0x004177f4) | `F32(0x008a96ec+v*0x30c)` (E:1693) | Equivalent by `Frontend/SmallLeaves_t1.cpp:36-37` (0x00408ad0 loads 0x008a96ec + idx*0x30c, 0x00408ada) and `D3d9Render/TrackRenderer.cpp:8068,8077`. This is the read the bridge feeds (Part 2). |
| 6 | p2 mode read | `I32(0x007f0fd0)` (A:103; 0x004177f9) | `I32(kGameModeFd0)` (E:1694; `AiState.h:83` = 0x007f0fd0) | None. |
| 7 | p2 scale | `F32(0x005cc328)` read live (A:104) | `kRaceMetricScale` = 0.01f literal (E:723,1695) | Image read vs literal. E cites the same address. [UNCERTAIN: needs a byte read of 0x005cc328] |
| 8 | p2 store | `F32(0x0089a880+v*4)=val` (A:105; 0x0041780f) | same (E:1696) | None. |
| 9 | p2 gate | modes 4,9,7,8 (A:106; 0x00417808..0x00417825) | same (E:1697) | None. |
| 10 | p2 threshold | `F32(0x005cc31c)` (A:107; 0x0041782b) | `kFinishThreshold` = 3.0f (E:724,1698) | Image read vs literal, same address cited. [UNCERTAIN: 0x005cc31c bytes] |
| 11 | p2 slots | 4-slot compare/record vs `F32(0x005cc33c)` (A:109-120; 0x00417842..0x004178f5) | vs `kSlotSentinel` = -1.0f (E:725,1700-1710) | Same shape, same slots 0x0089a870/874/878/87c. Sentinel image read vs literal. [UNCERTAIN: 0x005cc33c bytes] |
| 12 | p3 entry | `fd0==9` (A:127; 0x0041790d) | same (E:1717) | None. |
| 13 | p3 angle | `RaceAngleGet(1)` = call 0x00417730 (A:71,129; 0x00417920) | `F32(0x0089a880+4)` (E:1719) | Call vs direct read of the array written in row 8. E's comment calls 0x00417730 that storage. [UNCERTAIN: 0x00417730 body not read] |
| 14 | p3 thresholds | 0x005cc348, 0x005cc950, 0x005cc9a4, 0x005cc574 live (A:130-133) | `kMode9Thresh1..4` = 1.5, 0.75, 0.025, 2.0 (E:726-729,1720-1723) | Image vs literals, addresses match A. Multipliers: A bit patterns 0x3f733333 / 0x3f666666 / 0x3e800000 / 0x3f933333 (A:130-133) = 0.95 / 0.9 / 0.25 / 1.15. E writes 0.95f / 0.9f / 0.25f / 1.15f (E:1720-1723): same floats. Threshold literals [UNCERTAIN] as row 7. |
| 15 | p3 apply | `VehSpeedSet(1, spd*mult)`; `fd0` reloaded (A:134-136; 0x0041797c..0x00417990) | `GearConstSet(1, spd*mult)`; `fd0` reloaded (E:1724-1726) | Rows 2/3 carry over. |
| 16 | p3b entry | `fd0==4`, v=1..3 (A:140-141) | same (E:1730-1731) | None. |
| 17 | p3b angle | `RaceAngleGet(v)` (A:143) | `F32(0x0089a880+v*4)` (E:1733) | As row 13. |
| 18 | p3b formula | 0x005cc950, 0x005cc574, 0x005cc32c, 0x005cc9a0, 0x005cc55c, 0x005cc56c, 0x005cc8f4, 0x005ccac8, 0x005cc320, 0x005cc8f0, 0x005cc564 (A:144-157) | `kMode9Thresh2`, `kMode9Thresh4`, `kBankRandGate`, `kSteerExtra`, `kBrakeMinSpeed`, `kMode4Mul1`, `kMode4Mul2a`, `kMode4Mul2b`, `kSteerDeadband`, `kMode4SubHi`, `kMode4SubLo` (E:1734-1744), each defined with the same `_DAT_` address in its comment (E:680-734) | Address-for-address match, operand order identical. Image vs literal as row 7. E:731-732 give Mul2a/2b as 0.16666667f / 0.33333334f. [UNCERTAIN: bit equality with 0x005cc8f4 / 0x005ccac8] |
| 19 | p4 alive | `PowerupAlive(v)` = call 0x00454a30 (A:72,166) | `I32(0x00688304+v*0x18)!=0` (E:1754) | Call vs direct read. [UNCERTAIN: 0x00454a30 body not read] |
| 20 | p4 type | `VehType(v)` = call 0x0040e470 (A:73,170) | `s_host.veh_type(v)` (E:1758) | Call vs host stand-in. The original reads `*(*0x005f2770 + 0x34 + v*4)` (`D3d9Render/TrackRenderer.cpp:8079-8081`, 0x0040e474..0x0040e47a). [UNCERTAIN: stand-in body not read] |
| 21 | p4 double | `VehSpeedSet(v, spd+spd)` (A:172; 0x00417aa9) | `GearConstSet(v, spd+spd)` (E:1760) | Rows 2/3 carry over. |
| 22 | p5 mode ladder | `m==4\|\|9 -> 0x0089a368=2`; `m==8 -> 0` (A:179-180; 0x00417ac3..0x00417adc) | same (E:1767-1768) | None. |
| 23 | p5 sub mode | `SubGameMode()` = call 0x0040e350 (A:74,181) | `s_host.game_sub_mode()` (E:1769) | Call vs host stand-in. [UNCERTAIN: stand-in body not read] |
| 24 | p5 timer | `I32(0x0089a36c)+I32(0x007f1008)`, `>180000` (A:183-189) | `kOverrideStep` = 0x007f1008, `>180000` (E:1771-1775; `AiState.h:88`) | None. |
| 25 | p5 row | `Ftol(double F32(0x0089a360))` = FLD + call 0x004a2c48 (A:76-83,192; 0x00417b24/0x00417b2a) | `static_cast<int>(Ftol(F32(0x0089a360)))`, comment "FUN_004a2c48 truncates" (E:1777) | Original __ftol vs the port's helper. Differs only if the helper is not truncation. |
| 26 | p5 tickscale | `float(I32(0x007f0ff8)) * F32(0x005cc948)` (A:194) | `float(I32(kFrame0ff8)) * kTickScale` (E:1779; E:735 = 0.00033333333f at 0x005cc948) | Image vs literal. [UNCERTAIN: bit equality with 0x005cc948] |
| 27 | p5 band ladder | `diff<=0x005cc320`, `<0x005cc574`, `<=0x005cc55c`, `<0x005cc354`, `<=0x005ccd6c`, `<0x005cd0f8`, `<0x005cd0f4`, `<=0x005cc728`, `<0x005cd0f0` (A:198-215) | `kSteerDeadband`, `kMode9Thresh4`, `kBrakeMinSpeed`, `kSlowBand1`, `k20f`, `kSlowBand2`, `kSlowBand3`, `kSlowBand4Lo`, `kSlowBand4Hi` = 1, 2, 10, 12, 20, 22, 42, 60, 62 (E:1782-1798, E:736-740) | Same ladder, including the redundant recheck (A:208, E:1792). Image vs literal as row 7. |
| 28 | p5 commit | store 0x0089a370, 0x0089a374, zero 0x0089a4c4+0x74*i / 0x0089a4c8 (A:217-224; 0x00417c1b..0x00417c3d) | same (E:1800-1807) | None. |
| 29 | p5 early outs | `0x0089a368!=0 -> return`; `ecx==0 -> return` (A:225-226; 0x00417c43/0x00417c4f) | same (E:1808-1809) | None. |
| 30 | p5 prob table 1 | `I32(0x005f30a0 + (row*5+edx)*4)` read live, no bound (A:229-230; 0x00417c57..0x00417c69) | `kDiffProb1[(row*5+edx) % 56]`, harvested copy of 0xe0 bytes (E:1675-1677,1811) | BEHAVIOUR DIFFERENCE. E reduces the index modulo 56, A reads the image unbounded. They agree while row*5+edx < 56 (row <= 10). E:1673-1674 records measured row 2.5 (`verify/d3_ai_20260926/o_inputs`). Table contents transcribed from the original .data per E:1670-1671. [UNCERTAIN: the 56 ints vs the original 0x005f30a0..+0xe0 not re-read here] |
| 31 | p5 prob table 2 | `I32(0x005f3180 + off)` (A:239) | `kDiffProb2[(row*5+edx) % 56]` (E:1678-1680,1818) | Same difference as row 30, table 0x005f3180. |
| 32 | p5 RNG 1 | `RandFloat(0x42c80000)` = call 0x00472650(0,100.0f) (A:65,231; 0x00417c77) | `AiRand(0.0f,100.0f)` (E:819-824,1812) | BEHAVIOUR DIFFERENCE. A calls the original RNG. E reimplements `(hi-lo)*float(u & 0x7fffffff)*4.656612873e-10f+lo` with `u=RwRandomNext()` and a once-opened stream (`s_rw_p`, E:816-824). The formula matches the 0x00472650 description at E:815. Stream state is separate from the original's. [UNCERTAIN: RwRandomNext vs FUN_00534870 bit equality and seed point] |
| 33 | p5 RNG 2 | `RandFloat(0x42c80000)` (A:240; 0x00417ccb) | `AiRand(0.0f,100.0f)` (E:1819) | As row 32. |
| 34 | p5 table-1 hit | `0x0089a368=1`, zero 0x0089a4c0+0x74*i up to 0x0089a690, return (A:233-237; 0x00417c8a..0x00417cb2) | same (E:1814-1816) | None. |
| 35 | p5 table-2 hit | `0x0089a368=2` (A:242; 0x00417cde) | same (E:1821) | None. |
| 36 | linkage | `extern "C" __declspec(dllexport) void __cdecl AiPreTick(void)`; `RH_ScopedInstall(AiPreTick, 0x004177b0)` (A:90-91,245) | plain `void AiPreTickRubberBand()`; call site `AiStandalone.cpp:2129` | Hook install vs direct call. The scanner sees A only (`re/tools/dual_copy_allowlist.txt:38`: E has no RVA comment directly above it). |

Totals: 36 rows. Identical rows: 6, 8, 9, 12, 16, 22, 24, 28, 29, 34, 35 (11). Same shape with only the value source changed: 11, 18, 27 (counted with the source rows). Source-of-value differences: 1-5, 7, 10, 11, 13-15, 17-21, 23, 25-27. Behaviour differences: 30/31 (modulo 56 on the table index) and 32/33 (separate RNG stream). Row 36 is linkage.

The allowlist note (`dual_copy_allowlist.txt:38`) says the .asi body is about 160 lines and the exe's about 140. Counted: 157 and 142. Its "reads from places no exe TU writes (0x008a9648 / 0x008a96ec, FUN_00407a20 / FUN_00408ad0 leaves)" is rows 4/5: both reduce to direct reads of the same addresses. Its "harvested kDiffProb tables" is rows 30/31.

## Part 2. 0x008a96ec: bridge vs FUN_00408610 as two producers of one slot

Slot: `0x008a96ec + v*0x30c` = row base 0x008a9620 + v*0x30c + 0xcc. Readers: A row 5 (via 0x00408ad0, 0x00408ada) and E row 5 (E:1693).

| # | Aspect | Bridge (`D3d9Render/TrackRenderer.cpp:8715-8731`) | FUN_00408610 (plate `race_rules_d1/0x00408610.md`; port `Race/PathProgStep.cpp`) |
|---|---|---|---|
| 1 | what is written | `ap = fmod(race_[v].arcprog, total_len_) / total_len_ * 100.f` (8729-8730), a percentage of the port spline's arc length | The plate step 8 lists `row+0xcc` among fields carried over from the probe blocks `DAT_008a95c0` / `DAT_008a95e8` filled by FUN_00407e20 (plate steps 7-8). The plate does not decode the field's formula. [UNCERTAIN: FUN_00407e20 body, so units and range of +0xcc are not stated here] |
| 2 | cars | all `kRaceCars`, unconditional (8727-8731) | per car, gated by FUN_0040da50's predicate. The port mirrors it as `IsCarSlotActive(v)` and `VehicleSlotGetter(v)==1` (`PathProgStep.cpp:316-317`), with `[UNCERTAIN U-9210]` on the 0x0046c770 branch (`PathProgStep.cpp:285`). Early outs inside the original: plate steps 1 and 3 (`FUN_0046cbb0` flag != 0, == 1). |
| 3 | inputs | `race_[v].arcprog`, `total_len_` (port spline) | entity position `param_1+0x30..0x38`, the checkpoint and track tables (FUN_00426cf0, FUN_00407b00/b70/e20, FUN_00408590) and the row's own previous state (plate steps 4-9) |
| 4 | timing | in `UpdateRace`, before `Ai_Standalone_Tick` (comment at 8715-8718) | original: FUN_0040fc00 runs FUN_00425a40 (0x0040fc6c) then FUN_0040da50 (0x0040fc75) (`PathProgStep.cpp:252`). Port arm R: `PathProg_RacePctTick` (`PathProgStep.cpp:312-338`) at the end of `UpdateCar` by default (comment at TrackRenderer.cpp:8725-8726), or in `UpdateRace` before the AI tick when `MASHED_PP_PUBLISH & 0x8` (TrackRenderer.cpp:8697-8698; default mask 0, `PathProgStep.cpp:206-216`). |
| 5 | gating by default | `!getenv("MASHED_NO_RACEPCT_BRIDGE")` (default ON since 2026-10-09, 8722-8723) AND `!PathProg_Enabled()` AND `!RacePctOrig_Active()` AND `total_len_ > 1e-3f` (8725) | `RacePctOrig_Enabled()` default ON (`Race/PathNodes.cpp:346-349`, opt-out `MASHED_NO_RACEPCT_FUN408610`). `RacePctOrig_Active()` = enabled AND `s_triNode` non-empty, i.e. the course block was built from an AI BSP (`PathNodes.cpp:353-355`). |
| 6 | who writes when | the bridge writes only where arm R is inactive: a track whose AI BSP built no course block, after `MASHED_NO_RACEPCT_FUN408610`, or with `MASHED_PATHPROG` off and no tri data | arm R writes only where `PathNodes_TriCount() != 0` (`PathProgStep.cpp:313`); on those tracks the bridge stands down (TrackRenderer.cpp:8724-8725) |
| 7 | other fields | writes +0xcc only | arm R runs the verbatim step on a shadow row `s_shadow[v]` (`PathProgStep.cpp:200-203,327-331`) and publishes only +0xcc by default (line 337). Lap counter +0x28, node progress +0xc8 and the rest of the row go out only under `MASHED_PP_PUBLISH` bits 0x1 / 0x2 / 0x4 (lines 332-336). The full `MASHED_PATHPROG` knob (default OFF, `PathNodes.cpp:341-344`) lets FUN_00408610 own the real row (TrackRenderer.cpp:8724). |
| 8 | sibling slot | none | the original also writes row+0x28 = 0x008a9648, the lap counter (plate step 10). By default no arm-R publish of it (mask bit 0x1 off), so A/E row 4 read whatever the image holds there. `RangeTable_ah1.cpp:247` notes it "stays 0 in Quick-Battle arena". [UNCERTAIN: no check here that no other TU writes 0x008a9648] |

Mechanical consequence: per frame per track 0x008a96ec has one writer by construction (`!RacePctOrig_Active()` at TrackRenderer.cpp:8725 against `PathProgStep.cpp:313`). The two writers differ in value formula (row 1), car set (row 2) and phase (row 4). Which one a default run uses depends on whether the track built tri data. Not decided here: whether the bridge branch is reachable on any shipped track. [UNCERTAIN: needs a per-track read of `PathNodes_TriCount()`, which needs a run]

## Findings for the burn-down (no action taken)
1. E ports all five phases with control flow identical to A (rows 6, 8, 9, 12, 16, 22, 24, 28, 29, 34, 35).
2. Two behaviour differences versus A: table index modulo 56 (rows 30/31) and the port-owned RNG stream (rows 32/33).
3. The remaining rows are value-source differences whose equality rests on a cited comment, not on bytes read in this lane. Unread bodies: 0x00443080, 0x0046dd80, 0x0046dd90, 0x00417730, 0x00454a30, 0x0040e470 (stand-in), 0x0040e350 (stand-in), the 0x004a2c48 helper. Unread image constants: 0x005cc31c, 0x005cc328, 0x005cc33c, 0x005cc348, 0x005cc950, 0x005cc9a4, 0x005cc574, 0x005cc948 and the others in rows 18 and 27.
4. Closing those needs Ghidra or the Frida AbAiPreTick driver (`Ai/AiControllerAB.cpp:707-802`), an execution step, not text.

## Lint
`py -3.12 scripts/lint_rva_bodies.py` run before and after: output identical. Result both times: `16 finding(s): CROSS-TARGET=16 | allowlisted=16 NEW=0`. Hashes and the diff result are in the commit message and the REPORT.
