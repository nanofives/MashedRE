# RESULT MATCH-S19, 2026-11-06

**Verdict: GREEN.** Gates on the merged build equal the merge_train refs (G-CTRL 8284be46/ba407a16/31fe043e, B 1d5bf4f9 x3). Slice 0x004c0740..0x004c7760 (117 game functions, 28 library rows inside skipped).
**84/117** sources are byte-identical under `re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178), and
every one of the 84 has `relocheck.py` OK with `unchecked=0` (`relocheck.tsv`). **32** of them are now the port's only
body (32 wrapper TUs `Match/MatchS19_<va8>.cpp`): 3 in `mashed_re.exe` and the .asi, 29 in the .asi only.
**C4 candidates: 0.** None of the 3 exe-side swaps has a direct exe chain from `WinMain` (L-68, L-89, table below); same on the merged build.

Branch: `orch/MATCH-S19`, cut from orch/integration 3c6510ee6 (run 5). orch/integration adf5ff69b merged before the
swap leg, then 0f5899d1c (MATCH-S10, MATCH-S11 and others) merged after the first gate run. Gated builds, both with
`MASHED_NO_DEPLOY=1`: pre-merge exe `a992ab0d`, merged exe `b1e4cc92` (sha256
b1e4cc92d8a35a25f8dbd9dea70ffcfb054894ee729ffaf28508a12e64c74a54), asi `2636b31d`. A trial merge with the newer
orch/integration b1dd84b7c (MATCH-S13, S15, S09-FU) is conflict-free (`git merge-tree`); it is left to the train.
Insertion anchor: the S19 rsp block sits after `"Boot\VideoConfig.cpp"` (exe and asi).

## Finding that changes how to match this range: the RW core is a Pentium build

The RenderWare-core functions of this slice match only with **`/G5`** added (helpers h5 and h6, 2026-11-06; `/G3` and
`/G4` give the same bytes on the files tried). Under the game flags `/O2 /GS /Ob1` alone (= `/G6` scheduling, no
byte-register ALU ops) they sit at 73-97% with prologue load scheduling and `or al,1`-style byte-op residuals that no
source form moves. 0x004c6750 has no stack cookie despite `char[256]` locals, so the RW core is also built without
`/GS` (h5: `/O2 /Ob1 /G5` gives 83.3%, with `/GS` 69.6%). The run-5 flag scan (8 near-misses over `/Ob1`, `/GS-`,
`/G6`, `/Ox`, `/Ob2`) never tried `/G5` alone. Game code (0x004c4680 BodyOrientationIntegrate and similar) stays on the
game flags. Each file records its flags on line 1, which `match.py` uses.

A closing flag sweep (`tools/flagsweep.py`, `/O2 /GS /Ob1`, `+/G5`, `/O2 /Ob1 /G5|/G4|/G3`) over the 33 files still
below 100% moved none to 100% (largest gain +6.7 points, 0x004c3880).

## Match (static leg)

| | count |
|---|---|
| slice functions | 117 (`slice_rvas.txt`, `tools/mkslice.py`) |
| raw converter (g2s) at 100% | 20 (run 5) |
| hand-matched to 100% (run 5 helpers h1-h4, run 6 helpers h5 and h6) | 64 |
| of the 84, no Ghidra size in `re/console/pc_funcs.csv` (scored by the whole compiled symbol, below) | 4 |
| below 100% | 33 (table below; budget spent) |

No-size rows (0x004c07b0 124 B, 0x004c0830 61 B, 0x004c4430 61 B, 0x004c4470 191 B): Ghidra has no function at these
entries, so `collect.py` refuses them. Each was compared with the size forced to the compiled symbol's length: 100%
instructions, 100% registers, `bytes_bad` empty, and each ends where the next function begins (0x004c07b0+124 =
0x004c082c, next 0x004c0830; 0x004c4430+61 = 0x004c446d, next 0x004c4470; 0x004c4470+191 ends 1 byte before
0x004c4530). `pc_funcs.csv` is not changed here (follow-up below).

Held static callee: 004c0c20.c and 004c0f00.c hold 0x004c0ad0 (in-file copy for the caller's codegen; 0x004c0ad0 has
its own 100% file and no port body, neither file is swapped).

| rva | size | best % | regs % | class | residual (helper notes) |
|---|---|---|---|---|---|
| 0x004c0c20 | 330 | 98.9 | 100.0 | b | sibling-search loop loads next via the old reg; 8 more loop forms, helpers, flags, permuter: no move |
| 0x004c0f00 | 313 | 98.8 | 100.0 | b | same loop as 0x004c0c20; permuter 174 compiles no gain |
| 0x004c18c0 | 113 | 65.2 | 65.2 | b | original repeats xor eax,eax;ret per compare, ours tail-merges; 6 forms + flagscan; h1's 100% claim does not reproduce |
| 0x004c1a70 | 150 | 90.7 | 90.7 | b | zBufferNear/Far copied via fld/fstp in original, mov in ours; casts, struct local, /Op, permuter: no move |
| 0x004c1b40 | 104 | 92.7 | 95.1 | b | lea ecx,[eax+0x94] vs add, dec esi placement; loop/counter/dot forms + permuter |
| 0x004c1f80 | 1682 | 17.4 | 21.6 | b | first structural pass (run 5); not reworked in run 6 |
| 0x004c2620 | 1040 | 19.0 | 23.8 | b | first structural pass (run 5); not reworked in run 6 |
| 0x004c2a30 | 334 | 49.6 | 49.6 | b | x87 operand choice (fld mem; fmul st(i) vs fld st(i); fmul mem) throughout |
| 0x004c2f60 | 79 | 76.9 | 92.3 | b | mov eax,[g]; lea esi,[eax+0x10] prologue and rotated reload regs; typed forms, return form, 19-flag scan: no move |
| 0x004c2fb0 | 130 | 75.6 | 88.9 | b | same prologue/rotation as 0x004c2f60; not reworked further |
| 0x004c3040 | 102 | 97.1 | 97.1 | b | add eax,0x10 one push earlier in original; 4 forms + permuter |
| 0x004c32b0 | 768 | 37.7 | 36.6 | b | first structural pass (run 5); not reworked in run 6 |
| 0x004c3730 | 150 | 40.4 | 40.4 | b | RwV3dTransformPoint C fallback; x87 schedule (in.x kept on stack, y/z spilled via one arg slot) not reproduced; 6 forms + permuter |
| 0x004c3880 | 133 | 51.1 | 51.1 | b | RwV3dTransformVector C fallback; x kept on x87 via fld m;fmul st(2) in original, 9176 dups x; permuter no gain |
| 0x004c3910 | 149 | 91.5 | 91.5 | b | (h3 file) FLAGS + /G5: 83.0 -> 91.5; only the y/z scale x87 operand order remains; permuter no gain |
| 0x004c39b0 | 271 | 69.9 | 88.0 | b | RwV3dNormalize (sqrt+invsqrt tables, RWERROR); ops all match, in-ptr eax vs ecx register choice; permuter no gain |
| 0x004c3c60 | 250 | 97.3 | 97.3 | b | RwV2dNormalize (/G5): one x87 operand order (out->y = in->y * recip); permuter no gain |
| 0x004c4360 | 200 | 65.2 | 65.2 | b | matrix identity error (12 float locals, /G5): orig stores the memory-resident locals lazily between the arithmetic; permuter no gain |
| 0x004c4680 | 968 | 34.5 | 36.3 | b | first structural pass (run 5); not reworked in run 6 |
| 0x004c4a50 | 717 | 77.8 | 77.8 | b | RwMatrixRotateOneMinusCosineSine (/G5), RwMatrixMultiply inlined via static __forceinline: identity exits tail-merged, zero stores hoisted, x87 operand order |
| 0x004c4d20 | 155 | 90.9 | 90.9 | b | RwMatrixRotate (/G5; 81.8% plain): orig keeps recip live on x87 through the scale (fld m; fmul st1; final fstp st0) |
| 0x004c4dc0 | 225 | 91.2 | 91.2 | b | RwMatrixInvert (/G5): x87 operand order inside the 3 pos dot products; 288-form row search + permuter no gain |
| 0x004c4eb0 | 338 | 47.7 | 63.3 | b | general matrix invert (/G5): det int-test through a param-slot union + x87 spill choices; permuter gains only via wrong statement order |
| 0x004c5010 | 398 | 84.7 | 87.8 | a | RwMatrixScale (/G5): only the x87 load order of matrix vs scale operands; source operand order has zero effect (greedy commutation of every product) |
| 0x004c51a0 | 323 | 14.7 | 34.9 | b | first structural pass (run 5); not reworked in run 6 |
| 0x004c52f0 | 378 | 12.6 | 23.8 | b | first structural pass (run 5); not reworked in run 6 |
| 0x004c5470 | 525 | 25.2 | 32.1 | b | first structural pass (run 5); not reworked in run 6 |
| 0x004c5680 | 234 | 34.2 | 34.2 | b | first structural pass (run 5); not reworked in run 6 |
| 0x004c5930 | 133 | 89.1 | 91.3 | b | esi push shrink-wrapped around the ForAll loop in ours; loop forms, inline/no-inline, flags: no move |
| 0x004c5cb0 | 231 | 89.4 | 90.6 | b | RwTextureRead; edi push shrink-wrapped past the early return (as 0x004c5930); forms, flags, permuter |
| 0x004c6580 | 464 | 32.7 | 53.1 | b | first structural pass (run 5); not reworked in run 6 |
| 0x004c6750 | 1092 | 83.3 | 85.6 | b | texture image read + resample; FLAGS /O2 /Ob1 /G5 without /GS (orig has no cookie despite char[256] buffers; with /GS 69.6%); callee-saved push shrink-wrap residual |
| 0x004c6ba0 | 1631 | 14.9 | 52.0 | b | first structural pass (run 5); not reworked in run 6 |

Classes: (a) compiler-version codegen, (b) source shape not found in budget, (c) tool limit. Rows with no residual
text had a first structural pass in run 5 and were not reworked in run 6 (large, low score, budget).

## Swap (port leg)

`swaps.tsv`: one row per swapped RVA (wrapper, targets, x87, port symbol, install, source blob, match, relocheck, old
body). `bodies_pre.tsv` / `bodies_post.tsv`: the body scan before and after; each of the 32 RVAs now has exactly one
body, its `MatchS19_` wrapper.

| targets | rows |
|---|---|
| exe + asi | 3: 0x004c19f0 RwVtableSlot07Call, 0x004c2f00 RwEngineGetCurrentMode, 0x004c3df0 RwV3dTransformPoints |
| asi only | 29: 0x004c0b10, 0x004c0e50, 0x004c1a00, 0x004c1bb0, 0x004c1c80, 0x004c2c90, 0x004c2d70, 0x004c2d90, 0x004c2de0, 0x004c2e10, 0x004c2e40, 0x004c2e70, 0x004c2ea0, 0x004c2ed0, 0x004c2f30, 0x004c45f0, 0x004c5800, 0x004c5820, 0x004c5830, 0x004c5850, 0x004c5890, 0x004c5ae0, 0x004c5b50, 0x004c5bc0, 0x004c5c00, 0x004c5c80, 0x004c5ca0, 0x004c75c0, 0x004c75e0 |

- `py -3.12 scripts/lint_rva_bodies.py` -> `1 finding(s): CROSS-TARGET=1 | allowlisted=1 NEW=0` (unchanged).
  `py -3.12 scripts/lint_inline_asm_abs.py` -> OK. `re/tools/dual_copy_allowlist.txt` unchanged (no swapped RVA had an
  entry). `x87_tus.txt` unchanged: no old TU was in it, and the only swapped body with float arithmetic (0x004c1c80,
  two `1.0f / v` divisions) was SSE in its old TU (`HUD/ViewportDimsSet_ag.cpp`) and stays SSE.
- **Return widths (matched-source edits, DDK bytes unchanged, `match.py` 100% and `relocheck` OK re-run after each):**
  0x004c0b10 `byte` -> `int`, 0x004c2e70 / 0x004c2f30 `bool` -> `int`, 0x004c2d90 `void` -> `int` (returns the
  FUN_004d7de0 result the original leaves in EAX), 0x004c5c80 `void` -> `undefined4` returning `param_1` (the original
  leaves it in EAX: `mov eax,[esp+4]` at 0x004c5c8c). The DDK emits the same bytes either way; VS2022 C++ may leave the
  upper EAX bits or EAX itself undefined for the narrow/void forms, and the original's callers read EAX.
- **C++-compat edits (DDK 100%, relocheck OK):** 004c2d90.c callee prototype `FUN_004d7de0(undefined4 *, undefined4 x4,
  int)` (was `int *` / `undefined *` params, which C++ rejects for the `undefined4` args); 004c3df0.c `typedef int code(...)`
  gets a 4-arg form under `__cplusplus`.
- **Raw-address forwarders** (old body called the callee through its raw address; no port body on that path):
  0x004c2e10 / 0x004c2e70 / 0x004c2ed0 / 0x004c2f30 / 0x004c2de0 / 0x004c2e40 / 0x004c2ea0 / 0x004c2f00 -> 0x004c2c90,
  0x004c2d90 -> 0x004d7de0, 0x004c5890 -> 0x004d8000, 0x004c5ae0 / 0x004c5b50 / 0x004c2c90 -> 0x004d7ff0 and 0x004d8480,
  0x004c1c80 -> 0x004c0e50. 0x004d7ff0 is declared variadic in the matched sources; its forwarders read exactly the
  ints those call sites pass (4 after the code in 5ae0/5b50, 1 in 2c90) and make the same raw call.
- **In-TU callers kept:** `Frontend/IntroSplash.cpp` calls `IntroSplashVtableSlot6` and `IntroSplashRenderState`; it now
  carries `extern "C"` declarations of both (C linkage, so they link to the wrappers' definitions).
- **Behaviour notes (old body vs matched):**
  - 0x004c19f0: the old exe body called the slot `void (*)()` with no pushed argument; the matched source passes
    `camera` as the original's tail-jump does (`jmp [eax+0x1c]` with the caller's stack). Same whenever the old call
    was compiled as a tail jump or the slot ignores its argument.
  - 0x004c2f00: the old body set `local_4 = 0` before the driver call; the original (and the matched source) does
    not, so the two differ only when the driver returns nonzero without writing the mode. The swap takes the
    original's form. No port caller (exe or asi).
  - 0x004c75e0, 0x004c5bc0: the matched sources return the first argument as the original leaves it in EAX; the old
    bodies returned void.
  - 0x004c2c90: the old body was a switch over `cmd - 0xd`; the matched source is the original's switch. Same results
    for every request (0xd/0xf/0x11/0x12 return 1, 0xe/0x10 return `a5 == 0` with the message call on 0xe, others report
    error 0x18 and return 0).

**9 byte-identical rows NOT swapped (NOT-SAFE).**

| rva | old body | reason |
|---|---|---|
| 0x004c3b30 | Math/RwLutTables.cpp LutSqrt (exe+asi) + install FastSqrt (Math/RwSqrt.cpp) | LutSqrt reads the port's own table (`T().sqrt_`), the original reads RwGlobals+DAT_007d3ffc; FastSqrt branches on `SqrtFoldOn()` (MASHED_NO_SQRT_FOLD) and the fold log knob |
| 0x004c3b90 | install FastInvSqrt (Math/RwSqrt.cpp) | same knob pair (`SqrtFoldOn`, `SqrtFoldLogOn`), port LUT body `Math::LutInvSqrt` |
| 0x004c3ac0 | install Vec3Magnitude (Math/Vec3.cpp) | install-only physics math entry (no RVA body to replace); the strict Math rule keeps it out |
| 0x004c3bf0 | Math/RwV2d.cpp Vec2Length (exe+asi) | `std::sqrt` fallback when the LUT root is null (standalone path); sum order `v[1]*v[1] + v[0]*v[0]` |
| 0x004c4270 | Math/MatrixOrthoResidual.cpp (exe+asi) | naked x87 asm keeping 80-bit intermediates and returning a double; the matched source returns float, and VS2022 codegen of it cannot be shown to keep the same intermediates |
| 0x004c42d0 | Math/MatrixOrthoResidual.cpp (exe+asi) | same as 0x004c4270 |
| 0x004c4600 | Collision/RwpSolverMath2.cpp FUN_004c4600 (exe+asi) | `ShadowAB::Run` A/B harness wrapper (kPhaseRace) around `FUN_004c4600_impl` |
| 0x004c7600 | install RasterUnlock_Entry (Render/RasterDeviceWrappers.cpp) | naked entry into a self-test dispatcher (`RasterUnlockDispatch`, selftest log), not an RVA body |
| 0x004c76f0 | install RasterCanLock_Entry (same TU) | same self-test pattern |

SKIP 1: 0x004c1be0 has no port body (`Frontend/IntroSplash.cpp` `OrigRenderTargetHelper` is a raw call-through the
scanner counts; MatchS06 wrappers forward to the raw address the same way).

**42 byte-identical rows with no port body**, not swapped (a new body with no caller is not C4, L-68): 0x004c0740,
0x004c07b0, 0x004c0830, 0x004c0ad0, 0x004c0b70, 0x004c0ed0, 0x004c0ef0, 0x004c1300, 0x004c1340, 0x004c13e0,
0x004c1480, 0x004c1520, 0x004c1680, 0x004c1740, 0x004c1800, 0x004c1840, 0x004c1a10, 0x004c1a40, 0x004c1b10,
0x004c1c10, 0x004c1cf0, 0x004c2d80, 0x004c2dc0, 0x004c3270, 0x004c35f0, 0x004c3d60, 0x004c3d90, 0x004c3dc0,
0x004c40b0, 0x004c4430, 0x004c4470, 0x004c4530, 0x004c4670, 0x004c5770, 0x004c5860, 0x004c59c0, 0x004c5dd0,
0x004c6140, 0x004c7620, 0x004c76c0, 0x004c7730, 0x004c7760.

## Exe callers (L-68, L-89)

`tools/exe_reach.py mashedmod/build/mashed_re.exe mashedmod/build/mashed_re.map <symbol>` (direct call/jmp rel32
graph from the linker map, walked from `_WinMain@16`).

| rva | symbol | direct sites | reach WinMain | callers |
|---|---|---|---|---|
| 0x004c19f0 | RwVtableSlot07Call | 11 | no | ViewportInit (0x00428590, MatchS06), FUN_00428a30, TitleBuildInfoRender, FUN_00428d30, FUN_00429e10; their only direct caller is FUN_00429290, which has none (ViewportInit and FUN_00429e10 have 0 direct sites) |
| 0x004c2f00 | RwEngineGetCurrentMode | 0 | no | none (address taken = install table) |
| 0x004c3df0 | RwV3dTransformPoints | 0 | no | none (MatchS11_0046d510 calls 0x004c3df0 by raw address) |

The 29 asi-only swaps have no exe body. **0 C4 candidates**, so no counter build was needed.

## Gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, private root `D:/MashedWT/MATCH-S19_root`
(`MASHED_ROOT`, robocopy of `original\`), test slot outside and `mashed_lock.py run --solo --label MATCH-S19` inside
(opened after the merge train's priority windows cleared). Refs read from `re/orchestrator/merge_train.json` at gate
time (no FLIP had landed: gctrl `8284be46/ba407a16/31fe043e`, b `1d5bf4f9` x3).

| run | exe | arm | file | sha8 | ref |
|---|---|---|---|---|---|
| 1 (pre-merge, 12:28) | a992ab0d | H_DEF | step.csv / gates.csv / ptrace.log | 8284be46 / ba407a16 / 31fe043e | 8284be46 / ba407a16 / 31fe043e |
| 1 | a992ab0d | B_DEF_K0, K1, K26 | step.csv | 1d5bf4f9 x3 | 1d5bf4f9 x3 |
| 2 (merged, 14:03) | b1e4cc92 | H_DEF | step.csv / gates.csv / ptrace.log | 8284be46 / ba407a16 / 31fe043e | 8284be46 / ba407a16 / 31fe043e |
| 2 | b1e4cc92 | B_DEF_K1, K26 | step.csv | 1d5bf4f9 x2 | 1d5bf4f9 |
| 2, re-run 14:22 | b1e4cc92 | B_DEF_K0 | step.csv | 1d5bf4f9 | 1d5bf4f9 |

Run 2's first B_DEF_K0 exited 3 after 34 s with no step.csv: its log shows `InitD3D9: CreateDevice(HAL/HW,...) -> HUNG
hr=0x80004005` and the REF retry HUNG too, so the arm never reached the race. That is the display device, not the
port; the arm alone was re-run under the same locks and matched the ref. Every other arm exit 0, self-exited.

The merge train's `-Full` pass (D2 x3, 13 tracks) was not run here (`mashedmod/src` is in `full_set_paths`, so the
train runs it on enqueue). With 0 exe callers for the 3 exe-side swaps, no gate scenario executes a swapped body in
the exe; the gates show the swap leaves the default exe behaviour unchanged.

## Re-classify

Tracker-only, through the re-classify skill: the 32 swapped rows get `file` (and `exe_file` for 0x004c19f0,
0x004c2f00, 0x004c3df0) = their `Match/MatchS19_<va8>.cpp` wrapper and the note `match evidence: identical, swapped,
exe-callers=0; C4 when a caller lands (L-68 ruling)` with the match.py flags, relocheck counts, source blob sha256 and
this RESULT. **No level moves** (0 C4 candidates). Three of the rows were already C4 on an earlier route (0x004c2d70,
0x004c2f00, 0x004c5820) and stay C4 on that route. 32 CHANGELOG lines (`Cn=Cn notes/file only`). No UNCERTAINTIES or
STUBS rows created (the matched bodies carry no `[UNCERTAIN]` marker or stub). The 52 byte-identical rows that were
not swapped (9 NOT-SAFE, 1 SKIP, 42 with no port body) are not touched in hooks.csv; their evidence is this RESULT,
`results.tsv` and `relocheck.tsv`. `re/orchestrator/tracker_lint.py --repo . --base orch/integration` ->
`OK ... pre-existing=4 NEW=0`.

## Follow-ups (filed as open ledger rows, not started)

| id | what |
|---|---|
| MATCH-S19-FU-MATHSAFE | the 7 NOT-SAFE math rows (0x004c3b30, 0x004c3b90, 0x004c3ac0, 0x004c3bf0, 0x004c4270, 0x004c42d0, 0x004c4600): decide per row whether the matched body can replace the knob / LUT / asm / ShadowAB body |
| MATCH-S19-FU-RASTER-SELFTEST | 0x004c7600 / 0x004c76f0: retire the raster self-test installs or swap the matched bodies in |
| MATCH-S19-FU-PCFUNCS | add 0x004c07b0, 0x004c0830, 0x004c4430, 0x004c4470 to `re/console/pc_funcs.csv` from a pool clone |
| MATCH-RWCORE-G5 | re-score RW-core near-misses in other slices with `/O2 /Ob1 /G5` (no `/GS`) and brief later RW-range slices with it |

## Raw data (not committed), `D:/MashedWT/MATCH-S19_data`

| file | sha256 |
|---|---|
| decomp.json | 31db8bf8728e4f0ef005f4b07487b19e5514812faaf5ca2b49620a134b9a5b27 |
| g2s/results.tsv | dd299c7c623c2feab148a3124fe125874815ad4d8cfea0902a8ff45d70b10401 |
| bodies.tsv (pre-swap scan, = bodies_pre.tsv here) | 8ca7da643ff9a2c3a414ff92683f050288706fe4754053166a17b3e06635a108 |
| excise.log / excise2.log | 6e33b3990514d2d6b043a69549b31dc2c6260542e62ddf5489211b8647f05105 / 00e58ab290ee5a76030f6be3af19f52109c16c1d52d22f452b0720f5aa701cda |
| flagsweep.txt | 3bc0fbb0bb3b34312b9fdea3f789b2629d9261aa601b365eb86d5e14d3fd0029 |
| exe_reach.txt | a8a350a79a780f46547ee219b330b6449f1d8eed17b8e1d3c843c2d7ac063841 |
| build1.log | 99df875e9acb2aa9c7aae2eed714afbb2d705a6691bef6204e92877c2dce80c7 |
| exe_reach2.txt (merged build) | 671bd54d5641e1868324f6dc300a2d0806a7a4f036a9fe3227150afec8e9d0aa |
| build2.log (merged build) | aad2fc62b111d2d8182b1e548bdcc169458257a3d0a3407a0315df86e4e21d54 |
| arms2/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms2/H_DEF.gates.csv | ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb |
| arms2/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms2/B_DEF_K1.step.csv, arms2/B_DEF_K26.step.csv, arms2k0/B_DEF_K0.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 (each) |
| arms2/B_DEF_K0.log (the HUNG CreateDevice run) | 739ae20a1900e7447919596faadf5f8cc2b315929c195d8ebf3eb52856e7cb6f |
| arms/ (run 1, pre-merge exe) | same six hashes as run 2 (gates.log) |
| notes/h1..h6.md, scratch/, g2s/ | helper reports, scratch, raw converter output |
