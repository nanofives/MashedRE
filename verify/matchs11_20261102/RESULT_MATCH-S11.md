# RESULT MATCH-S11, 2026-11-02

**Verdict: BLOCKED. Built @ this branch, gates not run: display D3DERR_NOTAVAILABLE** (static and port legs done). Slice 0x004670a0..0x00474e60
(128 game functions, 0 library rows inside). **100/128** sources are byte-identical under `re/tools/match/match.py`
(XP DDK cl 13.00.9176 / c2 13.00.9178, `/O2 /GS /Ob1`, C, x87), and every one of the 100 has `relocheck.py` OK with
`unchecked=0` in its best source file (`relocheck.tsv`; its BAD lines all belong to functions below 100%, where the
instruction streams do not line up). **50** of them are now the port's only body (50 wrapper TUs
`Match/MatchS11_<va8>.cpp`): 21 in `mashed_re.exe` and the .asi, 29 in the .asi only.
**C4 candidates: 2 rows** (0x00468b40, 0x00472c60), each with a direct exe chain from `WinMain` that is not an
env-gated harness (L-68, L-89). The other 48 swaps have no exe caller.

Branch: `orch/MATCH-S11`, cut from orch/integration d14610367. MATCH-S09 and MATCH-S10 are not merged there; nothing
here depends on them. Insertion anchors are S11-only: the rsp block sits after `"Frontend\DrawQuadPrimitives.cpp"`
(exe line 153, asi line 178; S09 uses `PickupPoolSpawn`, S10 `FloatSliderStep`, T13 and PORTSTATE lines 500+), and
the one `re/console/pc_funcs.csv` row is 0x004714f0 (no other slice touches that address). `x87_tus.txt` is not
edited (below).

## Match (static leg)

| | count |
|---|---|
| slice functions | 128 (`slice_rvas.txt`, from hooks.csv by `tools/mkslice.py`, the cut_slices.py rule; = `verify/matchscope_20261031/slices.tsv` row) |
| raw converter (g2s) at 100% | 17 |
| hand-matched to 100% (helpers h1-h9, `helper_notes.txt`, `helper_assign.txt`) | 83 |
| of the 100, byte-identical by the whole-symbol rule (`by` = `bytes`, Ghidra size short of the real end) | 10 (0x0046baa0, 0x0046d880, 0x00471530, 0x004715a0, 0x00471ac0, 0x00471cf0, 0x004728f0, 0x00473220, 0x004733b0, 0x004736c0) |
| below 100% | 28 (table below; budget spent) |

Tool-data fix: `re/console/pc_funcs.csv` lacked 0x004714f0. Its size (60 B) comes from the pool clone's function
(`decomp_pc.py`, slot Mashed_pool0; `exact_entry` true).

Size the csv gets wrong, as a helper found it (csv not changed): 0x00467650 is 4908 B (1236 instructions), csv 1255.

| rva | size | best % | regs % | class | residual (helper notes) |
|---|---|---|---|---|---|
| 0x00467350 | 767 | 97.5 | 97.5 | b | 3-term dot order canonicalised by cl (6 ins); event queue as one struct |
| 0x00467650 | 4908 | 56.1 | 56.3 | b | ESI static callee + stand-in; frame layout, keep-and-pop x87 adds, dot order; budget |
| 0x00468980 | 437 | 90.1 | 90.1 | b | ECX/ESI static callee + stand-in caller; two 3-term dot orders canonicalised by cl (12 ins) |
| 0x00468d80 | 1871 | 82.1 | 84.9 | b | contact pointer +0x20 in EDI vs +0x18 in EBP; dot order; ops 96.5% |
| 0x004694e0 | 1465 | 46.5 | 53.2 | b | frame layout + contact/world pointer registers; ops 90% |
| 0x00469aa0 | 841 | 60.0 | 70.7 | b | ESI-passed car (static + stand-in); EDI and a 0x5c frame, registers rotate from there |
| 0x00469df0 | 5060 | 30.1 | 33.3 | b | first structural pass; EAX/ECX-passed cars land in EDI/ESI; budget |
| 0x0046b1c0 | 810 | 97.7 | 97.7 | b | x87 scheduling of the first midpoint (fadd one store early, fstp/fxch/fmul late) |
| 0x0046b540 | 1368 | 59.2 | 64.4 | b | store-block constant registers, loop induction bases, x87 operand order |
| 0x0046bfc0 | 1446 | 60.9 | 61.2 | b | structure and x87 order right (ops 98.2), frame 0xb4 vs 0xb8 (slot sharing) |
| 0x0046cc40 | 1687 | 73.1 | 76.1 | b | stack slots, prologue push order, ev[6]/ev[7] hoist, loop-tail block layout |
| 0x0046da80 | 339 | 61.2 | 68.2 | b | i kept in EBP (original reloads from stack); loop zero stores via EDX |
| 0x0046ddb0 | 3102 | 81.7 | 83.4 | b | EDI car via static fn + stand-in caller; steer dot z-first; scheduling residue |
| 0x0046e9e0 | 1419 | 81.7 | 81.9 | b | thrust-product factor order; acc zeros kept on the FPU stack |
| 0x0046ef70 | 1872 | 43.9 | 45.2 | b | ops 97.3%; original keeps &contacts[0] in its own slot (frame 0x80 vs 0x7c), every offset shifts |
| 0x0046f6c0 | 3581 | 44.3 | 53.4 | b | ops 90%; stack-slot/register assignment, shared k*12 induction; budget |
| 0x004704c0 | 420 | 91.0 | 91.8 | b | original keeps pos.y-h in ST0 across the matrix inits |
| 0x00470670 | 802 | 98.0 | 98.0 | b | (float)pad[n]*car[0x190]: fild then fmul [mem] in 2 places, ours fimul |
| 0x004709a0 | 705 | 55.7 | 62.9 | b | do-loop count in EBP in the original; 5 register-argument callees as stand-ins |
| 0x00470c70 | 1950 | 69.8 | 73.7 | c/b | register-arg static callees 0x00470670 (eax) and 0x0046f6c0 (eax, edi) not in the file; frame/regalloc |
| 0x00471780 | 831 | 73.3 | 79.8 | b | EBP/stack-slot allocation and fsubr operand order |
| 0x00471ec0 | 634 | 44.4 | 57.2 | b | structure matches; register allocation (x in ebp, record in ebx, sub base +0x458) |
| 0x004726f0 | 72 | 95.8 | 95.8 | a | first fld scheduled after mov ecx; ~20 forms + permuter, no gain |
| 0x00473c20 | 697 | 99.0 | 99.0 | b | y2 spill slot [esp+8] vs [esp+0x14] (2 bytes) |
| 0x00473ee0 | 1449 | 66.3 | 71.5 | b | original inlines the 0x004744a0 body; frame 0x44 vs 0x3c, row-loop regalloc |
| 0x004744a0 | 998 | 99.6 | 99.6 | b | 1 fstp scheduled after test eax |
| 0x00474890 | 1159 | 45.2 | 50.8 | b | original's 6-slot frame (inlined-helper shape) not found |
| 0x00474de0 | 119 | 94.7 | 94.7 | a/b | 2 push/add-esp scheduling slots survive every rewrite and the permuter |

Classes: (a) compiler-version codegen, (b) source shape not found in budget, (c) tool limit.

Held static callees (in-file `// MATCH:` copies of another RVA; none of these files is swapped): 00467110.c holds
0x004670a0, 00471df0.c holds 0x00471530, 0046da80.c / 0046ddb0.c / 0046f6c0.c hold 0x0046c5f0 (the pilot's
source). Stand-in callers (built only outside MATCH_PORT) are listed in `helper_notes.txt`.

## Swap (port leg)

`swaps.tsv` has one row per swapped RVA: wrapper, targets, x87, port symbol, install, source blob, match, relocheck,
old body. Helper reports: `swap_w1.txt` .. `swap_w4.txt` (committed, small).

| targets | rows |
|---|---|
| exe + asi | 21: 0x00468b40, 0x0046c6d0, 0x0046c770, 0x0046d700, 0x0046c570, 0x0046cbe0, 0x0046cc10, 0x0046d510, 0x00472c60, 0x00472f40, 0x004730b0, 0x00473870, 0x00473540, 0x004736c0, 0x00472dc0, 0x00474e60, 0x0046dc00, 0x004671c0, 0x004671d0, 0x00467210, 0x0046b4f0 |
| asi only | 29: 0x00467300, 0x0046bce0, 0x0046d740, 0x004671a0, 0x0046c700, 0x0046bda0, 0x0046dd90, 0x00471530, 0x00472560, 0x004725f0, 0x004726b0, 0x00474d80, 0x0046bd20, 0x0046bd60, 0x0046d320, 0x0046d360, 0x00474db0, 0x0046d660, 0x0046d6a0, 0x0046d6d0, 0x00472520, 0x00472550, 0x00472640, 0x0046dd80, 0x004723d0, 0x00472500, 0x0046d4d0, 0x00472690, 0x00472820 |

- `py -3.12 scripts/lint_rva_bodies.py` -> `2 finding(s): CROSS-TARGET=1, DUP-IN-TARGET=1 | allowlisted=2 NEW=0`
  (unchanged); `bodies_post.tsv`: each of the 50 RVAs has exactly one body, its `MatchS11_` wrapper.
  `py -3.12 scripts/lint_inline_asm_abs.py` -> OK (1183 files). `re/tools/dual_copy_allowlist.txt` unchanged (no
  swapped RVA had an entry; `DUP-IN-TARGET:0046d4a0` stays, that RVA is NOT-SAFE for the reason it records).
- **No `x87_tus.txt` entries.** None of the 29 old TUs was in `x87_tus.txt`, so every old body was SSE-compiled. The
  wrappers stay SSE, which keeps each body's codegen mode. Helpers w2 and w4 measured that x87 would change two
  things the old bodies did: the two-step `W*x*c` screen scaling of the Im2D draws, and dword-exact float copies
  (fld/fstp quiets a signaling NaN). The C4 rule's x87 clause is about physics TUs that are x87; none is swapped here.
- **Matched-source edits (C++ compat or port codegen; DDK bytes unchanged, `match.py --bytes` byte-identical and
  `relocheck` OK re-run after each):** 00472c60.c, 00472f40.c, 004730b0.c, 00473540.c, 004736c0.c, 00472dc0.c write
  `/ 640.0f` / `/ 480.0f` as `* (1.0f / 640.0f)` / `* (1.0f / 480.0f)`. The DDK emits the same multiply by
  0x005cd5a8 / 0x005cc560 (0x3acccccd / 0x3b088889) either way, as the original does; VS2022 /fp:precise keeps a real
  divide for the first form, so without the edit the port would divide where the original multiplies.
  00474db0.c: `typedef int code(int,int)` under `#ifdef __cplusplus`.
- Raw-address forwarders (old body called the original by address, no port body): 0x00467300 -> 0x00413c70,
  0x0046d510 -> 0x004c3df0, 0x00472690 -> 0x00534870, 0x00472820 -> 0x004a3907, the Im2D draws and 0x004671a0 /
  0x004671d0 / 0x00467210 -> 0x0042b8b0 / 0x0042b8c0 / 0x0042b930 / 0x0042f510. Port-body binds: 0x00472560 and
  0x004725f0 bind `FUN_0046be10` to `Write46be10` (its installed hook), 0x00474db0 binds `FUN_004b6520` to
  `ZeroFillWrapper` (as S05 did).
- Input-domain notes (finite inputs give the same result): 0x0046cc10 compiles `(int)float` to `__ftol2_sse` where the
  original calls `__ftol` 0x004a2c48 (differs for |x| >= 2^31 or NaN, as S08 accepted for 0x0043c000); 0x0046d700 /
  0x0046bce0 / 0x0046d740 copy floats where the old body copied dwords (only an SNaN differs under x87; the wrappers
  are SSE); 0x004723d0 writes the same 40 dwords in the original's order.
- 0x004671d0 / 0x00467210: the old TU keeps `FollowTargetField40` / `FollowTargetField10` (ShadowAB entries with their
  RH_ScopedInstall, no RVA comment) as thin callers of the matched `_impl` symbols (S02 ReleaseResourcePair pattern).
- 0x004671a0: first port body of this RVA (the old "body" was a static raw-address call in IntroSplash.cpp).
- Two old TUs now hold no code (`Util/UtilRandIntRange_wfb0f.cpp`, `Util/Vec3ValidateMixed_cfd622.cpp`); they stay in
  `asi_sources.rsp` so this branch does not touch those rsp lines.

**13 byte-identical rows NOT swapped (NOT-SAFE).**

| rva | old body | reason |
|---|---|---|
| 0x0046c7b0 | Vehicle/VehicleState.cpp VehicleSlotGetter | exe arm reads `g_vehicleArrayBase` (H1a rebind), returns 0 on a null base; matched reads absolute 0x008815a4 |
| 0x0046cbb0 | Vehicle/VehicleState.cpp VehicleCarStateRead | exe arm reads through the VehRecord rebind, writes 0/0 on a null base |
| 0x0046dbe0 | Vehicle/VehicleState.cpp VehicleRacePositionGet | exe arm reads VehRecord+0x08 (port record) |
| 0x0046d780 | Vehicle/LaunchRevCharge.cpp | exe arm uses `g_vehicleArrayBase` and skips both FUN_00422b50 calls (`#ifndef MASHED_STANDALONE`) |
| 0x0046d7f0 | Vehicle/LaunchRevCharge.cpp | exe arm reads port global `g_launchAccelByte`, writes `g_vehicleArrayBase`+0xbf4 |
| 0x0046d4a0 | VehicleRecordPtr.cpp + PickupCollect_naked.inc PK_46d4a0 | two bodies; exe arm rebound to `g_vehicleArrayBase`; matched equals only the absolute form |
| 0x0046baa0 | Race/SubStateCallees.cpp VehiclePhysicsStateReset | exe arm writes through SSC_REC = `g_vehicleArrayBase`, returns 0 on a null base |
| 0x0046c730 | Race/SubStateCallees.cpp | exe arm reads `g_vehicleArrayBase`+0xbf8 with a null-base guard |
| 0x0046c750 | Race/SubStateCallees.cpp | exe arm reads `g_vehicleArrayBase`+0xbf4 with a null-base guard |
| 0x0046d570 | Ai/AiLeafTargets.cpp AL_46d570 (exe-only naked) | adds `g_al_recdelta` (record rebase), calls `RwLutNormalize_cd`; callers run it under the `MASHED_AILEAF_PC` knob |
| 0x0046d2e0 | Util/PromoLoop_sessionB.cpp WheelGet46d2e0 | old bounds unsigned (negative i/wheel return 0); original signed (`jge 0x0046d2e7`, `jl 0x0046d2f0`) |
| 0x0046c5c0 | Frontend/SmallLeaves_n2.cpp VehicleSlotInit | **port defect:** old body writes a BYTE 0 at 0x008815a4 + i*0x341; original writes a DWORD 0 at 0x008815a4 + i*0xd04 |
| 0x0046c790 | Frontend/SmallLeaves_n2.cpp VehicleSlotFieldSet | **port defect:** old body writes 0x008815b0 + i*0x341; original 0x008815b0 + i*0xd04 (same for i=0 only) |

SKIP 1: 0x0046c5f0 already has a matched port body (`Match/MatchPilot_0046c5f0.cpp`, pilot source; h3's
`re/match/s11/0046c5f0.c` is a second 100% source of it, not swapped).

**36 byte-identical rows with no port body**, not swapped (a new body with no caller is not C4, L-68): 0x004670a0,
0x00467110, 0x004672d0, 0x0046be10, 0x0046be50, 0x0046bf50, 0x0046c7d0, 0x0046cb30, 0x0046d400, 0x0046d880,
0x0046dc20, 0x00471430, 0x00471450, 0x00471490, 0x004714f0, 0x00471560, 0x004715a0, 0x00471ac0, 0x00471cf0,
0x00471df0, 0x00471eb0, 0x004721b0, 0x004722e0, 0x00472380, 0x004725c0, 0x00472650, 0x00472740, 0x004728a0,
0x004728f0, 0x004729b0, 0x00472ad0, 0x00472b10, 0x00473220, 0x004733b0, 0x004739f0, 0x00474d60.
(0x0046c7d0, the steer-0 heading assist, is ported without an RVA body anchor; 0x0046be10 has only an install,
`Write46be10`.)

## Exe callers (L-68, L-89)

Binary witness on this build: `py -3.12 verify/matchs11_20261102/tools/exe_reach.py mashedmod/build/mashed_re.exe
mashedmod/build/mashed_re.map <symbols>` (direct `call`/`jmp rel32` graph from the linker map, walked from
`_WinMain@16`; `exe_reach.txt`). `addr_taken=yes` on most rows is the install table, not a caller.

| rva | symbol | direct chain from WinMain |
|---|---|---|
| 0x00468b40 | VehicleContactHistoryLookup | WheelTerrainContactClassifier (CarWorldContacts.cpp:153, via the ContactSolvers.h:20 forwarder) <- Collision::WheelContactSolver (WheelContactSolver.cpp:206, unconditional) <- Vehicle::SolveWheelContacts <- StepSubstep <- VehiclePhysics_StepCarsOuter <- TrackRenderer::UpdateCar <- RenderFrame <- WinMain. The shortest chain exe_reach prints runs through `Collision::CollWorld_RunReplayIfRequested` (an env-gated replay, not a caller under L-89); the chain above is the default-physics one, checked separately on the same map. |
| 0x00472c60 | ChromeBaseDraw | RenderFrame (exe_main.cpp:4191-4194, the standings overlay, gated by game state, not an env knob) <- WinMain |
| 0x00472f40 / 0x004730b0 | TextGradientV0V1Override / V2V3Override | only `FUN_00428d30` (MatchS06) <- `FUN_00429290` (no direct caller), plus the B16 raw-address thunks at exe_main.cpp:9665/9666: no direct WinMain chain, not counted |
| 0x00467210 | FollowTargetField10_impl | FollowTargetField10 <- FUN_0040fc00 <- FUN_004111c0 (MatchT01G, no direct caller): not counted |
| other 17 exe+asi rows, 29 asi-only rows | | none (exe-callers=0) |

## Gates

**Not run: display unavailable.** Every boot on this machine fails `CreateDevice` with `hr=0x8876086A`
(D3DERR_NOTAVAILABLE) on all three device types (HAL/HW, HAL/SW, REF). Probe: `B_DEF_K0` through
`mashed_lock.py run` + test slot on this build (exe `6ced2511`, private root `D:/MashedWT/MATCH-S11_root`), once every
~4.5 min for the 30 min bound: 12:54, 12:58, 13:03, 13:07, 13:12, 13:16, 13:21, all failed, no step.csv
(`display_poll.txt`). The lock was free (`mashed_lock.py status` before the first probe).

Owed before review: `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on this exe, compared with
`merge_train.json` (`gctrl.ref` `8284be46/ba407a16/31fe043e`, `b.ref` `1d5bf4f9` x3), and the merge train's `-Full`
pass (D2 x3, 13 tracks), since `mashedmod/src` is in `full_set_paths`. Swaps that run every race frame in the gate
scenario (0x00468b40 in the default wheel-contact solver) are the ones a gate would catch. The verdict and the C4
re-classify below wait on those gates.

## C4 re-classify text (apply after the merge, through re-classify; not applied here)

Common evidence: `match.py re/match/s11/<va8>.c` 100% under `flags=/O2 /GS /Ob1 tc=ddk`, `relocheck.py` OK with
`unchecked=0`, one body (`Match/MatchS11_<va8>.cpp`), exe caller chain from `WinMain` (table above), gates unchanged
on this build. The `file` cell moves to the wrapper.

| rva | name | level | match.py | relocheck | old body (removed) |
|---|---|---|---|---|---|
| 0x00468b40 | VehicleContactHistoryLookup | C3 -> C4 | 100% (53 B) | OK checked=0 unchecked=0 (no relocations) | Vehicle/VehicleState.cpp |
| 0x00472c60 | ChromeBaseDraw | C3 -> C4 | 100% (347 B whole symbol) | OK checked=24 unchecked=0 | Frontend/DrawQuadPrimitives.cpp |

CHANGELOG line per row: `2026-11-0x  0x<rva>  <name>  C3->C4  route match (c4=match), exe-callers via <chain>;
evidence: verify/matchs11_20261102/RESULT_MATCH-S11.md`.

`notes` for 0x00472c60: matched source writes the /640 and /480 as reciprocal multiplies (same DDK bytes, the
original's multiply). All 48 other swaps: stay at their level, `notes` += `match evidence: identical, swapped,
exe-callers=0; C4 when a caller lands (L-68 ruling)`.

Follow-up rows worth filing: the 0x0046c5c0 / 0x0046c790 stride defect (0x341 vs 0xd04) in
`Frontend/SmallLeaves_n2.cpp`; the `g_vehicleArrayBase` rebinds that keep 9 vehicle-record leaves NOT-SAFE.

## Build

`set MASHED_NO_DEPLOY=1&& mashedmod\build.bat`, Build OK, .asi not deployed (`original\mashed_re_dev.asi` still
dated 2026-10-24). exe sha256 `6ced25115171167414ab3eb546d6ea0f89d8e03798e79257600a251a5c6eda3f`, asi
`8256dc0a4ee9d6373da6d03994ebdbb880ac1c5e58fe4126b576c77483297f61`.

## Raw files (not committed; `D:/MashedWT/MATCH-S11_data`)

| file | sha256 |
|---|---|
| decomp.json (decomp_pc.py --port --callees, 128 entries, slot Mashed_pool0) | 5fb87b9b5e389b5ea5eb8ad0b69bad0786762c90719cf29d7d9a60a1de51cc4b |
| g2s/results.tsv (raw converter scores) | 2f6eb5f425b689b745216c810c44a2493332f5ac2371056dd31ecf9c85a3f5c9 |
| build1.log (Build OK) | cc3992d7b88bb3bcbd769c3374e17445060dcfdc8503c8d2d841881e95e1f617 |
| poll.out (display probe output) | 5c1883e30199a40c3415c5e9c82e9a2c7a82001f34dba73bb6bb85b3c4b74be6 |
| g2s/, notes/h1..h9.md, scratch/, probe/, swap_cands.txt | raw converter output, helper notes, scratch |
