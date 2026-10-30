# RESULT D4-DC-77b0-VALSRC (static, addendum to RESULT_D4-DC-77b0-BODYDIFF)

Verdict: GREEN. Every [UNCERTAIN] value-source row of `verify/d4dc77b0diff_20261030/RESULT_D4-DC-77b0-BODYDIFF.md`
(BODYDIFF below) is listed here, either resolved with an RVA citation or kept [UNCERTAIN] with the missing runtime
evidence named. Static only: no game run, no test slot, no Frida. No `mashedmod/` change, no `.ps1` change, no tracker
change (nothing here closes an `UNCERTAINTIES.md` row, see the end).

Sources of evidence
- `re/tools/decomp_pc.py` against pool clone `Mashed_pool0` (acquired, read-only, released): decomp + callees/callers
  of 0x004177b0 and its 11 callees, `--datarefs` on 16 globals, decomp of the writers 0x00414060 / 0x00448700 /
  0x0046b540 (refs only) / 0x0040e480 / 0x0040e360 and the RNG set 0x00534870 / 0x00534990 / 0x00472690 /
  0x004b44f0 / 0x004b4510.
- capstone 5.0.7 linear disassembly and dword reads of `original/MASHED.exe.unpatched` (SHA-256
  `BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`, the version anchor) for 0x004177b0..0x00417cf0,
  0x00472650, 0x004a2c48, 0x00414060, every `call 0x00448700` site, the 25 constants and the two tables.
- A = `Ai/AiPreTick.cpp` (.asi), E = `Ai/AiStandalone.cpp` (exe), lines as in BODYDIFF.

Raw dumps (not committed), `D:/MashedWT/D4-DC-77b0-VALSRC_data/`:

| file | SHA-256 |
|---|---|
| decomp_fns.txt | 42d96c0f2e2d51896c04ad8603b12b70cfd1658ccbe30e125f7cf0a684ce0349 |
| decomp_writers.txt | f8ce40d6b676e6c5669ad06e247f6195f07ea35c77a4199cbd46b8e56985c3f2 |
| decomp_rng.txt | 8fddb89180583fede64fcfa9360b619a0485ea8dd7efb25fd408fe61db3a90e8 |
| decomp_vehtype.txt | 3166bcbb7bc942be545a2125cb6541db3fc8784fcebe020416003b557866f9b1 |
| datarefs.txt | 8c423bbf478aeae2d74c783c78864bdfbd5bcda386886dff817e43f35a37f579 |
| datarefs2.txt | 882e88d0d6857953a804ea87c922c33c2a31317c2cd94d1e9a7a8a808da1a3cd |
| static_bytes.txt | f73eefc3f70bff178c67f502da9444674f70353117dba5191c7af4cf0e077fc3 |
| static2.txt | bac6732eace00f6f1ef438ddc525e0e12cd566c8e9e2c76500fa3632f8aa01eb |
| static3_phase2.txt | b785be794cbf752a9c9ff934b08d893cbda09740efaca2cc428bcd7978dc12e9 |
| valsrc_static.py | 364b9791b534f6bdd64ee95d8b5cd20ae7cb0a2b8fc1ef4551ab1982c92d1f0a |
| valsrc_static2.py | 040422140fc5cf9c1b0c2d0cc54bf6d2305aaefc0c8bc4df01799013eb70a337 |

## The one shared runtime premise: x87 precision control

The original evaluates phases 2, 3b and 5 and the RNG wrapper on the x87 stack and compares register values, not
stored floats (0x0041780b `fadd` then 0x0041782b `fcomp` on the register; 0x00417b39..0x00417c13 compare the
`fmul`/`fsubr` results in ST0/ST1; 0x00417c7c `fcomp` on the FUN_00472650 return). E is SSE2 (`Ai/AiStandalone.cpp`
is not in `mashedmod/x87_tus.txt`), so every E `float` op rounds to 24 bits. At x87 PC_24 every `fmul`/`fadd`/`fsub`
also rounds to 24 bits (FILD and FLD of a float are exact), so the two agree op-for-op. At PC_53/64 they can differ
in the last bit near a compare threshold. Prior runtime evidence that the original runs at PC_24:
`verify/u9776_20261020/RESULT_u9776.md:17-18` (the device multiply 0x005cb2a0 in the race). This lane did not read
the control word at 0x004177b0 itself. Rows below marked "PC_24-exact" carry that one premise.
[UNCERTAIN: FPU control word at the 0x004177b0 entry in the original; an `fnstcw` sample there closes it for all
PC_24-exact rows at once.]

## Part 1 rows

| BODYDIFF row | Status | Evidence |
|---|---|---|
| 1 entry gate (0x00443080) | KEPT [UNCERTAIN] | 0x00443080 = `MOV EAX,[0x00897ffc]` (size 6). One writer: `FUN_00448700` stores its first argument at 0x0044871e. 8 call sites (cdecl, last push = arg 1): **1** at 0x0040326f (FUN_00403250, only when 0x00443080 returned 0), 0x00410a4d (FUN_00410860), 0x00492bea (FUN_004929d0); **0** at 0x00403482, 0x00410300, 0x00410a78; **EBX** (runtime) at 0x0041067e and 0x00410811 (FUN_00410510). E's stand-in `aib_ai_target_enable()` returns constant 0 (`D3d9Render/TrackRenderer.cpp:251`), justified there by a measured 0 on 6259/6259 AI steps of the original race (`TrackRenderer.cpp:245-250`, `verify/d3_ai_20260926/o_inputs`). Equal only in scenarios where the last FUN_00448700 call passed 0. Missing: which of the 8 sites fire per game mode, i.e. a write watch on 0x00897ffc across the mode set (modes 4/7/8/9 and sub-state 6 paths). |
| 2 speed base (0x0046dd80) | RESOLVED (source) | 0x0046dd80 = `FLD float [0x0061313c]`, size 7. E reads `F32(0x0061313c)` (E:1642). Same address. Value at that address is a separate port-state question: original writer FUN_0046b540 (`MOV [0x0061313c],0x3fc00000` at 0x0046b562, `FSTP` at 0x0046b5cd; .data initial 0x3fc00000 = 1.5); exe writer `Vehicle/VehicleInit.cpp:147`. [UNCERTAIN: equality of the stored value needs a read of 0x0061313c in both at the same race frame] |
| 3 speed set (0x0046dd90) | RESOLVED | 0x0046dd90 stores arg 2 at `0x008816f4 + v*0xd04`. E: `0x008815a0 + v*0xd04 + 0x154` = 0x008816f4 + v*0xd04 (E:1643). The argument is passed through `fstp dword [esp]` (0x004177d8, 0x00417986, 0x00417a5d, 0x00417aac), i.e. a float, as E. |
| 7 p2 scale 0x005cc328 | RESOLVED | image 0x3c23d70a = E `0.01f` 0x3c23d70a. |
| 10 p2 threshold 0x005cc31c | RESOLVED | 0x40400000 = 3.0f, equal. Compare is on the register sum (0x0041780b `fadd`, 0x0041782b `fcomp`, `test ah,1` = skip when ST < 3.0): PC_24-exact against E `!(val < 3.0f)`. |
| 11 p2 sentinel 0x005cc33c | RESOLVED | 0xbf800000 = -1.0f, equal. FUN_00414060 writes the same 0xbf800000 to 0x0089a870..87c (0x004140ef..0x00414113). |
| 13 / 17 race angle (0x00417730) | RESOLVED | 0x00417730 returns `(float10)(float)DAT_0089a880[v]` (size 12), the array phase 2 stores at 0x0041780f (`fst dword [esi*4+0x89a880]`). E reads `F32(0x0089a880+v*4)`. FLD of a float is exact. |
| 14 p3 thresholds + multipliers | RESOLVED | 0x005cc348 0x3fc00000 (1.5), 0x005cc950 0x3f400000 (0.75), 0x005cc9a4 0x3ccccccd (0.025f), 0x005cc574 0x40000000 (2.0): all equal to E. Multipliers are immediates 0x3f733333 / 0x3f666666 / 0x3e800000 / 0x3f933333 at 0x00417935 / 0x0041794a / 0x0041795f / 0x00417974 = E's 0.95f / 0.9f / 0.25f / 1.15f bit for bit. Compare senses: 0x00417930 / 0x00417945 / 0x0041795a `test ah,5; jp` = set when ST < c; 0x0041796f `test ah,0x41; jne` = set when ST > 2.0. |
| 15 / 21 p3 apply, p4 double | RESOLVED | 0x00417981 `fmul` of two floats then `fstp dword`: the exact product fits 48 bits, so one rounding to float at any PC, as E's single SSE multiply. 0x00417aa9 `fadd st0,st0` is exact doubling. |
| 18 p3b formula constants | RESOLVED (constants), PC_24-exact (chain) | 0x005cc32c 0x3f000000, 0x005cc9a0 0x3d4ccccd, 0x005cc55c 0x41200000, 0x005cc56c 0x3dcccccd, 0x005cc8f4 0x3e2aaaab, 0x005ccac8 0x3eaaaaab, 0x005cc320 0x3f800000, 0x005cc8f0 0x3e19999a, 0x005cc564 0x3e800000: all bit-equal to E's literals (0.16666667f -> 0x3e2aaaab, 0.33333334f -> 0x3eaaaaab). The chain 0x004179ce..0x004179ec (`fld 10; fsub [0x0089a360]; fmul 0.1; fmul 1/6; fadd 1.0; fstp`) and 0x004179fd..0x00417a19 is four ops per term with one store at the end: equal to E per-op at PC_24, not guaranteed at PC_53. |
| 19 p4 alive (0x00454a30) | RESOLVED | 0x00454a30 returns `DAT_00688304[v*6]` (dword stride 0x18). E reads `I32(0x00688304+v*0x18)` (E:1754). |
| 20 p4 type (0x0040e470) | KEPT [UNCERTAIN] | 0x0040e470 = `MOV ECX,[0x005f2770]` (0x0040e474) then `[ecx+v*4+0x34]`. 0x005f2770 holds 0x005f2728 and has no write reference, so the read is 0x005f275c + 4v. Writer: FUN_0040e480 (`*(ptr+v*4+0x34) = arg2`), 7 callers (0x004111c0, 0x00418860, 0x0042b960, 0x0042b9e0, 0x00432800, 0x0043df00, 0x0043dfd0). E default: `aib_veh_type` returns `v==0 ? 0 : 2` (`TrackRenderer.cpp:149-155`); `MASHED_SLOT_TYPE_LIVE` (default OFF) reads the same 0x005f2770 chain. Missing: the values those 7 callers store for the race scenario, i.e. a runtime read of 0x005f275c..0x005f2768 in the original. |
| 23 p5 sub mode (0x0040e350) | KEPT [UNCERTAIN] | 0x0040e350 = `MOV EAX,[0x0063ba8c]`. 20 direct writes (values 1..0xb; 6 only at 0x004104e3 in FUN_004103a0) plus setter FUN_0040e360 (4 callers). E's `aib_game_sub_mode` returns 7 under `s_roundEnd7` (ROUND_END7, default ON), the live 0x0063ba8c under `MASHED_SUBSTATE` (default OFF, `Race/SubStateCallees.cpp:69-73`), else 6 (`TrackRenderer.cpp:191-197`). Phase 5 runs only when it is 6 (0x00417aec `cmp eax,6`). Missing: the original's 0x0063ba8c per frame of the scenario, to compare against the port's constant 6. |
| 25 row (0x004a2c48) | RESOLVED | Caller does `fld [0x0089a360]` (0x00417b24), `call 0x004a2c48`, `mov esi,eax` (0x00417b35). 0x004a2c48: `fistp qword` (0x004a2c57), `fild` back, `fsubp` = x - rounded, then for x >= 0 `add ecx,0x7fffffff; sbb eax,0` (0x004a2c95..0x004a2c9b) decrements when x - rounded < 0, for x < 0 the sign-flipped twin increments (0x004a2c77..0x004a2c83). Result = truncation toward zero for any rounding mode. E `Ftol(float)` = `static_cast<long>` (E:759) = truncation. Equal for finite x. Port-side value source differs: the exe pins `F32(0x0089a360) = 2.5f` (`TrackRenderer.cpp:656-661`), the original computes it in FUN_00414060 (already filed: `verify/d3_gatefire_20261008/RESULT_A360.md`, U-1305). |
| 26 p5 tickscale 0x005cc948 | RESOLVED (constant), PC_24-exact (product) | 0x39aec33e = E `0.00033333333f` 0x39aec33e. 0x00417b2f `fild [0x007f0ff8]` (exact) then 0x00417b39 `fmul` (one rounding) vs E `float(int)*k`: equal at PC_24 while abs(0x007f0ff8) <= 2^24 (int to float exact). Above 2^24 the product exceeds 5592, so no band fires in either (all band gates need tickscale < 62). Negative values below -2^24: not analysed [UNCERTAIN: value range of 0x007f0ff8 from its writers 0x0040e5a2, 0x0040fe64, 0x0040ff23, 0x00418598]. |
| 27 p5 band ladder | RESOLVED (constants + sense) | 0x005cc320 1.0, 0x005cc574 2.0, 0x005cc55c 10.0, 0x005cc354 12.0, 0x005ccd6c 20.0, 0x005cd0f8 22.0, 0x005cd0f4 42.0, 0x005cc728 60.0, 0x005cd0f0 62.0: all bit-equal to E. Compare senses at 0x00417b47..0x00417c07 (`test ah,0x41; jne` = skip when ST <= c; `test ah,5; jp` = set when ST < c) match E's `!(x <= c)` / `x < c` forms (E:1782-1798). Values compared are register values: PC_24-exact. |
| 30 / 31 prob tables, `% 56` | RESOLVED | (a) Contents: 0x005f30a0 (56 dwords) equals `kDiffProb1` entry for entry. 0x005f3180 equals `kDiffProb2` on indices 0..54. Index 55 differs: image 0x800000ff (-2147483393), E 0. (b) Reach: index = `edx + row*5` (0x00417c57..0x00417c5c, scaled by 4), edx in {0..4} (immediates 0x00417b63 / 0x00417b90 / ebp=2 / 0x00417bd8 / 0x00417c09). `0x0089a360` has one write reference, `fstp [0x0089a360]` at 0x004140f4 in FUN_00414060 (sole caller FUN_004111c0), and that store is clamped: below `[0x005d757c]` = 0.0 it loads 0.0 (0x004140c3..0x004140d2), above `[0x005cc55c]` = 10.0 it loads 10.0 (0x004140da..0x004140e9). The address is BSS (zero at load). So row is 0..10 and the index is 0..54: `% 56` never changes the index, and index 55 is never read. The two bodies are equal on every reachable index. Residual, outside a static read: a NaN input to FUN_00414060 passes both clamps (unordered sets C0/C2/C3, `jp` taken at 0x004140ce, `jne` at 0x004140e5); FUN_004a2c48 then returns 0 in EAX (integer indefinite 0x8000000000000000, low dword 0), while E's `static_cast<long>(NaN)` gives 0x80000000 and `% 56` of a negative int indexes out of bounds. [UNCERTAIN: whether 0x0089a37c (writers 0x00414122 = 0, 0x0041777e `fstp` in FUN_00417750) or the FUN_00431d80 / FUN_0042fe80 returns can be NaN; needs those bodies or a runtime watch. Writes to 0x0089a360 through a computed pointer are also invisible to `--datarefs`; a write watch on 0x0089a360 closes both.] |
| 32 / 33 RNG | RESOLVED (formula + generator), KEPT [UNCERTAIN] (stream position) | Generator: 0x00534870 and E `RwRandomNext` (E:786-801) are the same additive ring step (`*p += *q`, read, `p++`, `>>1`, wrap `p` to base and `q++` without wrap, else `q++` with wrap). Seed: 0x00534990 allocates 0x7c bytes, `[0] = 0x9a319039`, `[i] = [i-1]*0x41c64e6d + 0x3039` for 31 dwords, `p = base+0xc`, `q = base`, `end = base+0x7c`, then 0x136 draws; E `RwRandomOpen` (E:803-814) is identical. 0x00534990 has no direct caller (plugin path via 0x007dc578, written at 0x00534940 in FUN_00534920). Wrapper: 0x00472650 = `fild u&0x7fffffff`, `fmul [0x005cd314]` (0x30000000 = 2^-31, exact scale), `fld hi; fsub lo; fmulp; fadd lo`; the `fadd [0x005cc94c]` at 0x00472667 is unreachable (EAX masked non-negative, `jge` at 0x00472665). At PC_24 the first `fmul` rounds u to 24 bits and the `fmulp` rounds once more, which is exactly E's `float(u)` then `* 100`: enumeration of every u within +-8192 of each k*2^31/100, k = 0..100, gives **0** u where `rnd <= k` differs (`static2.txt`). At PC_53/64 it gives 5051 such u out of 2^31. Compare: 0x00417c7c `fcomp` against the float of the table int, `test ah,0x41; jp` = take table 2 when rnd > prob, i.e. hit when rnd <= prob, equal to E `!(rnd > prob)`. Draw order: table-2 roll only after a table-1 miss in both. Still open: the call index. 0x00534870 is shared by FUN_00472650 (68 callers), FUN_00472690 (7), FUN_004b44f0 (10) and FUN_004b4510 (6), so which ring draw 0x00417c77 receives depends on every earlier draw in the session. E keeps its own ring with only the port's consumers (comment E:775-779). Missing: the original's draw count at 0x00417c77 (a counter on 0x00534870 calls) to compare with the port's `s_rw_draws`. |

Rows not marked [UNCERTAIN] in BODYDIFF Part 1 (4, 5, 6, 8, 9, 12, 16, 22, 24, 28, 29, 34, 35, 36) are not repeated.
The A-side calls (rows 2, 3, 13, 19, 20, 23) are one-instruction getters/setters, so the "call vs direct read" source
difference reduces to the address identities above.

## Part 2 rows (bridge vs FUN_00408610), checked for scope

| BODYDIFF Part 2 | Status | Evidence |
|---|---|---|
| row 1, units of row+0xcc | KEPT [UNCERTAIN] | Not decoded here: needs the FUN_00407e20 body, outside this step's function set. |
| row 8, other writers of 0x008a9648 | PARTLY RESOLVED | Literal-address references in `mashedmod/src/mashed_re`: readers at `Ai/AiStandalone.cpp:1691`, `D3d9Render/TrackRenderer.cpp:7234`, `Gameplay/RangeTable_ah1.cpp:245`; one writer, `Util/UtilLeaves_ac.cpp:53-58` (port of 0x00407a60, stores 0xffffffff at +0x28), in `exe_sources.rsp:205` but with no exe call site (its only reference is `RH_ScopedInstall` at `UtilLeaves_ac.cpp:63`). Writes through the row base 0x008a9620 (e.g. `PathProgStep.cpp` publish bit 0x1) are not visible to a literal grep. [UNCERTAIN: needs a write watch on 0x008a9648 in the exe] |
| mechanical consequence, per-track TriCount | KEPT [UNCERTAIN] | Needs a run, as stated in BODYDIFF. |

## Tracker effect

None. No `UNCERTAINTIES.md` row is resolved: U-8938 (structure of the 0x0089a360 record) is untouched, and
U-D3-DIFF360 is not a tracker row (`re/analysis/D3_AI_PORT_2026-09-26.md:243`; carried into U-1305 by
`verify/d3_gatefire_20261008/RESULT_A360.md:74`). The new static facts (single clamped writer of 0x0089a360, reach
0..54, generator/seed identity, PC_24 bit-identity of the RNG wrapper) are recorded here only.

## Summary for the burn-down
- Resolved statically: rows 2 (address), 3, 7, 10, 11, 13/17, 14, 15/21, 18 (constants), 19, 25, 26 (constant), 27,
  30/31 (no reachable behaviour difference), 32/33 (generator, seed, formula at PC_24).
- Kept [UNCERTAIN], runtime evidence named: row 1 (0x00897ffc per mode), row 20 (0x005f275c..68 values), row 23
  (0x0063ba8c per frame), row 2 stored value, 32/33 draw index at 0x00417c77, the NaN/indirect-write residual of
  30/31, and the one PC_24 premise for rows 10, 18, 26, 27, 32/33.
- The `% 56` and the table-2 entry 55 difference are dead on every reachable index. They do not block the dual-copy
  burn-down. The RNG stream position is the one behaviour difference left between A and E in this body.
