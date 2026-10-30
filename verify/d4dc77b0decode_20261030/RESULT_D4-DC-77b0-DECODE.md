# RESULT D4-DC-77b0-DECODE (static, addendum to RESULT_D4-DC-77b0-VALSRC)

Verdict: GREEN. Both [UNCERTAIN] rows named in the step are settled. Each is RESOLVED for what static reading can
decide, and each residual that needs a run names the exact runtime evidence missing. The rows are
`verify/d4dc77b0valsrc_20261030/RESULT_D4-DC-77b0-VALSRC.md` Part 2 row 1 (units of row+0xcc) and Part 1 row 1
(0x00897ffc first arguments). Static only: no build, no game run, no test slot, no Frida. No `mashedmod/` change, no
`.ps1` change, no tracker change.

Sources of evidence
- `re/tools/decomp_pc.py` against pool clone `Mashed_pool0` (read-only, acquired and released by the tool): decomp,
  callees and callers of 0x00407e20, 0x00408610, 0x00408ad0, 0x00407b00, 0x00407b70, 0x00408590, 0x00407d70,
  0x00407db0, 0x00407cd0, 0x00407be0, 0x00426cb0, 0x00426cc0, 0x00426d00, 0x00426cf0, 0x00426bb0, 0x00448700,
  0x00403250, 0x004102f0, 0x00410860, 0x004929d0, 0x00410510, 0x004111c0, 0x00411170, 0x0040fc00, 0x00492290,
  0x00443080.
- capstone 5.0.7 linear disassembly and dword reads of `original/MASHED.exe.unpatched` (SHA-256
  `BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`, the version anchor). It covers 0x00407e20..0x00408587,
  0x00448700..0x0044872a, 0x00410510..0x0041083c, 0x004111c0..0x004117a0 and 0x00410240..0x004102b6. It also scans .text for
  every `E8` rel32 to 0x00448700, every dword 0x00448700, every dword 0x00897ffc and every reference to 0x005f29b8.
- Exe source read at `orch/integration` 009a74b2d: `Race/PathProgStep.cpp`, `Race/PathProgStep_naked.inc`,
  `Race/PathNodes.cpp`, `re/tools/naked_specs/pathprog_p2.json`, `D3d9Render/TrackRenderer.cpp`.

Raw dumps (not committed), `D:/MashedWT/D4-DC-77b0-DECODE_data/`:

| file | SHA-256 |
|---|---|
| decomp_a.txt | 1b4ce462f3bb832f05ad9c811fd9439b1f57126bfef5d1d0ff0a0dff90756ac3 |
| decomp_b.txt | b88fd43dbbd251b007f51cb82b98500ff728efa0b34494e8536129951dbffcef |
| decomp_c.txt | 5c49868cbea078184398b71d7da2198ae2ca592c92dff9995441460433f61c0f |
| dis_407e20.txt | 9ff36d83f3833d1c4c77c0db2657cfaa11f31c51bd9c50183abb48849460ae44 |
| dis_410510.txt | b8fd61881e80e217218fa987ec0a6af3939b09151fd097051618e51b43686e9c |
| dis_4111c0.txt | 293ca0bafead9c45aabe83d4298d45499f45cfbbfde62a9ff268b197e9dfcfef |
| rd_static.py | 4df17f0780a0e961377adb98030cf68d17f11b76132a70a81a34ac5f82622357 |
| scan_calls.py | 8b6ddd83078539494b1846d406b282f9355e3a34f4155bd5337e6461662e78d4 |

## (a) VALSRC Part 2 row 1: units of row+0xcc (0x008a96ec)

### Path from FUN_00408610 to the slot

FUN_00408610 calls FUN_00407e20 twice. The first call passes the row base `0x008a9620 + v*0x30c`, node `row+0xd0`
(0x008a96f0) and group slot `row+0xd8` (0x008a96f8). It also passes entity position `rec+0x30`, output block
0x008a95c0 and float `0xbf000000` (-0.5). The second call, made only when `row+0xd4` (0x008a96f4) != -1, passes
`row+0xd4` / `row+0xdc` and output block 0x008a95e8. Each block is 10 dwords. FUN_00408610 picks `i` = 1 when
`row+0xd4 != -1`, `[0x008a95d4] != 0.0` and `[0x008a95d4] < [0x008a95fc]`, else `i` = 0. It then copies
`(&DAT_008a95c8)[i*10]` to `row+0xcc` (0x008a96ec). So **row+0xcc = output word [2] of FUN_00407e20** for the chosen
probe.

### FUN_00407e20 output word [2], decoded from the bytes

Arguments are cdecl, with `ebx` = output block (`[esp+0x58]` at 0x00407ea4) and `esi` = group slot (`[esp+0x50]` at
0x00408533). The tail runs 0x00408528..0x0040857d:

| addr | instruction | effect |
|---|---|---|
| 0x00408538 | `mov [ebx], ebp` | out[0] = final node index n |
| 0x0040853d | `mov [ebx+4], edx` | out[1] = x (float, the local built at 0x00408094..0x0040809c, below) |
| 0x00408537 / 0x00408540 | `push esi; call 0x00407d70` | C = FUN_00407d70(group) |
| 0x00408545..0x0040854c | `push eax; push ebp; push esi; call 0x00407db0` | D = FUN_00407db0(group, n) (third push = C, unused by the 2-arg body) |
| 0x00408551 / 0x00408554 | `fld [ebx+4]; fld st0` | x, x |
| 0x0040855d | `call 0x004a2c48` | EAX = trunc(x) (FUN_004a2c48 truncates toward zero, VALSRC row 25), pops one x |
| 0x00408566 | `fild [esp+0x4c]` | push trunc(x) |
| 0x0040856d | `fsubr st(1)` | ST0 = x - trunc(x) |
| 0x0040856f | `fiadd [esp+4]` | + D |
| 0x00408573 | `fidiv [esp+0x4c]` | / C |
| 0x00408577 | `fmul [0x005cc568]` | x 0x42c80000 = 100.0 (dword read from the image) |
| 0x0040857d | `fstp [ebx+8]` | out[2] |

So **row+0xcc = 100.0 * ((x - trunc(x)) + D) / C**.

The terms are as follows:
- x is out[1] (0x00408094..0x0040809c, Ghidra `local_28`), equal to `|d1| / (|d0| + |d1|) + n`. d0 and d1 are two
  signed dot products of `(pos - P[k])` with `N[k]`. `P[k]` = `0x00663664 + k*0x4c` (FUN_00426cb0) and `N[k]` =
  `0x00663658 + k*0x4c` (FUN_00426cc0), taken from the node walk in 0x00407e20..0x00408052. When `C <= x` (ordered
  compare at 0x004080b0, `test ah,0x41; jp`), x -= C (0x004080cc..0x004080d0).
- C = FUN_00407d70(group) walks the route table returned by FUN_00426090 (`mov eax,0x66ce58`). It returns 1 plus the
  sum of `(hi - lo)` over the group's runs, where a `0x100` marker counts 1. The plate calls this the count of distinct
  values in the group.
- D = FUN_00407db0(group, n) is n plus `(acc - lo)` for each run `[lo, hi]` of that table containing n with
  `[0x0066ce5c] < lo`. Here `acc` is the running sum of earlier run lengths. The plate calls this the group-local
  cumulative offset.
- Early-out path (0x00407e89..0x00407ec4): node 0, group 0 and FUN_00407be0(0,0) == -10. Then out[0] = 0, out[1] = 0 and
  out[6..9] = 0, and the same tail gives `100.0 * D(0,0) / C(0)`.

**Units (RESOLVED, static):** row+0xcc is a percentage scaled by 100.0 (0x005cc568). Its quantity is the car's position
counted in **route-table node steps**: the node's group-local offset D plus the fractional plane-distance ratio between
two node planes, over the group's node count C. It is not a world-unit arc length.

[UNCERTAIN, range only: the stated formula holds, but `0 <= row+0xcc < 100` needs `0 <= D <= C-1` for every node, which
depends on the per-track route table at 0x0066ce58 (built from `Lap_Line` / `Lap_Line_End` / `Lap_Line_Change`,
`Race/PathNodes.cpp:13`). It also needs `|d0| + |d1| != 0` (both zero gives 0/0). Missing evidence: a read of
0x0066ce58 per track (or of 0x008a96ec in the original over a lap).]

### The two exe writers of the slot

| writer | formula | units |
|---|---|---|
| arm R, `PathProg_RacePctTick` (`Race/PathProgStep.cpp:312-338`), default ON via `RacePctOrig_Enabled()` (`Race/PathNodes.cpp:346-349`), live where `PathNodes_TriCount() != 0` (`PathProgStep.cpp:313`) | runs `PP_408610` and `PP_407e20` on a shadow row and copies `after+0xcc` into the real row (`PathProgStep.cpp:337`). `PP_407e20` is the verbatim naked transcription of 0x00407e20 (`naked_specs/pathprog_p2.json`, `PathProgStep_naked.inc:398`). The tail instructions at 0x00408551..0x0040857d are present one for one (`PathProgStep_naked.inc:981-995`). The constant reads `g_pp_c_5cc568 = 0x42c80000` (`PathProgStep.cpp:39`), equal to the image dword at 0x005cc568. `PP_426090` returns 0x0066ce58 (`PathProgStep_naked.inc:1504-1507`), the same table address. FUN_004a2c48 is replaced by `PP_Ftol2` (truncation, `PathProgStep.cpp:11,63`). | **same as the original**: 100.0 * (node-step offset + fraction) / node count |
| bridge (`D3d9Render/TrackRenderer.cpp:8770-8780`), only when `!PathProg_Enabled() && !RacePctOrig_Active() && total_len_ > 1e-3f` (8773) | `fmod(race_[v].arcprog, total_len_) / total_len_ * 100.f` (8775-8776). `arcprog` = laps * total_len_ + `gate_cumlen_[seg] + t * gate_seglen_[seg]` (8716-8720, "position-in-lap (world units)") | 100.0 * **arc length** fraction of the port gate spline |

Consequence: arm R writes the original's units. The bridge writes a percentage on a different measure: arc length
on the port spline instead of node steps on the 0x0066ce58 table. The two agree in scale (0..100) and in
direction. They agree in value only where node steps are uniform in arc length. Which writer is live on a shipped
track stays the VALSRC/BODYDIFF runtime question (`PathNodes_TriCount()` per track). It is not re-opened here.

## (b) VALSRC Part 1 row 1: first argument of every FUN_00448700 call

Writer: FUN_00448700 runs `FUN_004464c0(0x00897fe0)` 100 times (0x00448701..0x00448714). It then does
`mov eax,[esp+8]` (arg 1) at 0x00448716, **`mov [0x00897ffc],eax` at 0x0044871e** and `mov [0x00898000],ecx` (arg 2) at
0x00448723. A .text dword scan finds 0x00897ffc at exactly two places: 0x0044871f (this store) and 0x00443081 (the
reader FUN_00443080). 0x00897ffc is past the .data raw end (0x005ea000 + 0x4d000 = 0x00637000), so it is BSS and 0 at
load. The scan finds **8** `E8` calls to 0x00448700 and no dword 0x00448700 anywhere in the file, so there are no
indirect calls.

| # | call site | function | arg 1 push | arg 1 | reached when (static gate) |
|---|---|---|---|---|---|
| 1 | 0x0040326f | FUN_00403250 | `push 1` 0x0040326d | **1** | FUN_00443080() == 0, i.e. [0x00897ffc] == 0 (0x00403263..0x0040326a). FUN_00403250 is called only at 0x0040fdb7 in FUN_0040fc00, on the FUN_0042f6a0() == 0xb branch (0x0040fd45..0x0040fd59, byte table 0x0040fe84[9] = 3 -> 0x0040fdb1) |
| 2 | 0x00403482 | FUN_00403250 | `push 0` 0x00403480 | **0** | same function, after `[0x00636af0] > [0x00636af4]` or one of 8 flag-pair tests (0x00403463..0x0040346c) |
| 3 | 0x00410300 | FUN_004102f0 | `push 0` 0x004102fe | **0** | `[0x005f29b8] == 100000` (0x004102f2). FUN_004102f0 = sub-state 3 of FUN_004111c0 (`call` at 0x00411288, jump table 0x00411324[2]) |
| 4 | 0x0041067e | FUN_00410510 | `push ebx` 0x0041067d | **1** | `mov ebx,1` at 0x00410640, with no write to ebx between it and either call (0x00410640..0x0041067e, and 0x0041080f..0x00410811, whose only branch source in 0x00410510..0x0041083c is the `je` at 0x00410673). Path: esi != -1. VALSRC's "EBX (runtime)" is corrected here to the constant 1 |
| 5 | 0x00410811 | FUN_00410510 | `push ebx` 0x00410810 | **1** | as #4, path esi == -1 |
| 6 | 0x00410a4d | FUN_00410860 | `push 1` 0x00410a4b | **1** | `[0x005f29b8] > 199` and the flag test, then a winner index != 0, or FUN_0042f6a0() == 2 and FUN_00429860() == 0xb. Followed by `[0x0063ba8c] = 9` |
| 7 | 0x00410a78 | FUN_00410860 | `push 0` 0x00410a6c | **0** | same gate, no winner. `[0x0063ba8c] = 8` at 0x00410a6e |
| 8 | 0x00492bea | FUN_004929d0 | `push 1` 0x00492be8 | **1** | `switch([0x00771968])` case 3, FUN_0040e450() != 0. Caller FUN_00492290, loop `while ([0x00828300] == 0 && FUN_00499690() == 0)` |

**RESOLVED (static): all 8 first arguments are constants.** Sites 1, 4, 5, 6 and 8 pass 1. Sites 2, 3 and 7 pass 0. The
value written at each site does not depend on the game mode. The mode enters only in *which* site runs.

Static per-mode reach (game mode = `[0x007f0fd0]`; sub-state = `[0x0063ba8c]`, FUN_004111c0's jump table at 0x00411324,
index = sub-state - 1):
- Race start in every mode except 6: sub-state 1 stores `[0x005f29b8] = 100000` (0x004100d3) and sets sub-state 2.
  Sub-state 2 sets 3 when `[0x005f29b8] == 100000` (0x0041026d / 0x00410279), else 4 (0x00410287). Sub-state 3 runs
  site 3, which writes **0**, when the countdown word is still 100000 at 0x004102f2.
- Mode 6: sub-state 2 overrides to 4 (`cmp [0x007f0fd0],6` 0x004102a3, store 0x004102ac). Sub-state 3 and site 3 are
  skipped on that path.
- Round end, sub-state 6 (FUN_00411170 -> FUN_00410510, `call` 0x004112cd): mode-dependent winner selection
  `switch([0x007f0fd0])` for cases 4, 5, 7, 8, 9 and 10, plus the points tests on `[0x008a94d0]` 2/3/4 and on mode 2.
  Every exit that reaches the 0x0041062a block writes **1** (sites 4/5) and sets sub-state 0xb.
- Sub-state 7 (FUN_00410860, `call` 0x004112dd): **1** (site 6) or **0** (site 7). Its flag gate contains
  `[0x007f0fd0] != 1 && no active slot`.
- Sites 1/2 do not test the mode. Their gate is the FUN_0042f6a0() return 0xb.
- Site 8 is in the FUN_00492290 loop, gated on `[0x00771968]` and FUN_0040e450(). It does not test `[0x007f0fd0]`.

Against the exe: E's stand-in `aib_ai_target_enable()` returns 0 (`D3d9Render/TrackRenderer.cpp:251`). Statically,
the last write before a race's racing sub-states (4/5) is site 3's 0 on the 1 -> 2 -> 3 path. Sites writing 1 sit on
sub-states 6, 7 and 0xb, on the FUN_0042f6a0() == 0xb branch and in the FUN_00492290 loop. This agrees with E's 0
during racing *when* sub-state 3 ran.

[UNCERTAIN, kept: (1) mode 6 skips site 3, so on that path 0x00897ffc keeps the value of the previous write, which
can be 1 from a previous round's sites 4/5/6 or from site 8. (2) whether `[0x005f29b8]` still equals 100000 at
0x004102f2: .text holds 14 dword references to 0x005f29b8. One is the 100000 store (0x004100d5) and one is this
compare (0x004102f2). The other 12 (0x0040e4a1, 0x0040ff05, 0x0041026f, 0x00410293, 0x00410351, 0x0041036c,
0x00410393, 0x004103d1, 0x004103f2, 0x004104c2, 0x004108ff, 0x00410912) were not all read here. (3) which
FUN_0042f6a0() value and which `[0x00771968]` case occur in each mode. Missing evidence: a write watch on 0x00897ffc
(pc + value) across the mode set (4, 5, 6, 7, 8, 9, 10, and mode 2's points path). A single run per mode closes all
three.]

## Tracker effect

None. Both rows were [UNCERTAIN] markers in a RESULT file, not `UNCERTAINTIES.md` rows. The static facts here are
recorded only in this file. They are: the row+0xcc formula, arm R carrying the original's units, the bridge's
different measure, the constant first arguments, and the corrected site-4/5 value. They feed the D4 dual-copy
burn-down for 0x004177b0.

## Summary for the burn-down
- (a) RESOLVED: `row+0xcc = 100.0 * ((x - trunc(x)) + FUN_00407db0(g,n)) / FUN_00407d70(g)` (0x00408551..0x0040857d,
  constant 0x005cc568 = 100.0). Arm R reproduces it from the same bytes. The bridge's value is an arc-length
  percentage on a different measure. Range residual: 0x0066ce58 contents per track.
- (b) RESOLVED: first arguments 1/0/0/1/1/1/0/1 at 0x0040326f / 0x00403482 / 0x00410300 / 0x0041067e / 0x00410811 /
  0x00410a4d / 0x00410a78 / 0x00492bea, all constants (EBX = 1 from 0x00410640). KEPT [UNCERTAIN]: the per-mode
  firing order, which needs the 0x00897ffc write watch named above.
