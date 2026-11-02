# RESULT MATCH-S15, 2026-11-02

**Verdict: BLOCKED. Built on this branch, gates not run: display D3DERR_NOTAVAILABLE** (L1 instruction: static work only,
DISPLAY IS OFF on LIVING-PC, no gate, no game run). Slice 0x0048cf70..0x00494480 (129 game functions, 0 library rows
inside). **103/129** sources are byte-identical under `re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178,
`/O2 /GS /Ob1`, C, x87), and every one of the 103 has `relocheck.py` OK in its source file (`relocheck.tsv`). 89 have
`unchecked=0`. 14 leave only CRT/import names unchecked: `__ftol` (0x0048cf90, 0x0048f290, 0x0048f8a0, 0x0048f9f0,
0x0048fb10, 0x004917c0, 0x00491860, 0x004943f0), `/GS` cookie helpers (0x0048ff20, 0x00493ac0, 0x00493b50,
0x00493f00), Win32 imports (0x00493ac0 GetACP/GetLocaleInfoA/GetThreadLocale, 0x00493b50 GetVersionExA/
InterlockedExchange, 0x00493bc0 MultiByteToWideChar, 0x00493f00 MessageBoxA), `_free` (0x004943d0). Every BAD line in
relocheck.tsv belongs to a function below 100%, where the instruction streams do not line up.
**38** of the 103 are now the port's only body (38 wrapper TUs `Match/MatchS15_<va8>.cpp`): 14 in `mashed_re.exe` and
the .asi, 24 in the .asi only. **C4 candidates: 1 row** (0x00492770 MainLoopInit), with a direct exe chain from
`WinMain` that is not env-gated (L-68, L-89). The other 37 swaps have no exe caller.
**Port bugs found: 3**, filed as ledger rows MATCH-FIX-492E60, MATCH-FIX-4926C0, MATCH-FIX-492D30 (below).

Branch: `orch/MATCH-S15`, cut from orch/integration 3c6510ee6 (S01..S08 and T01G merged there; S09..S14 and S16 not).
Nothing here depends on an unmerged slice. Merge anchors are S15-only: the rsp block sits after `"Boot\LaunchHandshake.cpp"`
(S11 uses `DrawQuadPrimitives`, S10 `FloatSliderStep`, S09 `PickupPoolSpawn`), the x87 line sits after `CameraEntries`
(S10 after `RwpSolverCore14`, S08 after `RwpSolverCore17`).

## Match (static leg)

| | count |
|---|---|
| slice functions | 129 (`slice_rvas.txt`, from hooks.csv by `tools/mkslice.py`, the cut_slices.py rule; = `verify/matchscope_20261031/slices.tsv` row) |
| raw converter (g2s) at 100% | 42 |
| hand-matched to 100% (helpers h1-h8, `helper_notes.txt`, `helper_assign.txt`) | 61 |
| of the 103, byte-identical by the whole-symbol rule (`by` = `bytes`, Ghidra size short of the real end) | 10 (0x0048e7b0, 0x0048f260, 0x0048f680, 0x0048f6b0, 0x0048f6e0, 0x0048f710, 0x0048f740, 0x004904d0, 0x00491070, 0x00491bd0) |
| below 100% | 26 (table below; budget spent) |

Commands: `py -3.12 re/tools/decomp_pc.py --file slice_rvas.txt --port --callees --json -o decomp.json` from the main
checkout (pool slot Mashed_pool0, acquired and released by the tool); 0x004917a0 is not a function in the pool clone
(probe: instruction inside no function, DATA xref from 0x00440f4a), so it came from `decomp_pc.py --create` (slot
Mashed_pool10, released). `py -3.12 re/tools/match/g2s.py --decomp decomp.json --out g2s --flags "/O2 /GS /Ob1"`.

Tool-data fix: `re/console/pc_funcs.csv` lacked 9 slice functions, so `match.py` refused to score them. Added with the
size the pool clone gives (`decomp.json`, `exact_entry` true): 0x004917c0 155, 0x00491860 155, 0x00491900 164,
0x004919b0 88, 0x00491a10 88, 0x00491a70 164; from `--create`: 0x004917a0 18; from h8's byte-identical whole-symbol
compile (no Ghidra function at either address): 0x00493ac0 127, 0x00493b40 6.

| rva | size | best % | regs % | class | residual (helper notes) |
|---|---|---|---|---|---|
| 0x0048d140 | 485 | 91.6 | 91.6 | b | ours computes (t+1)*7 once (original recomputes); alpha byte store one push later |
| 0x0048d360 | 434 | 90.6 | 90.6 | b | ours computes (t+1)*k once, original reloads t after the size.x store; vel.z operand order |
| 0x0048d540 | 434 | 90.6 | 90.6 | b | same template as 0x0048d360 |
| 0x0048d720 | 434 | 90.6 | 90.6 | b | same template as 0x0048d360 |
| 0x0048e180 | 434 | 90.6 | 90.6 | b | same template as 0x0048d360 |
| 0x0048d900 | 505 | 89.9 | 89.9 | b | second size component's (t+1)*k hoisted; first pos add operand order |
| 0x0048db20 | 505 | 89.9 | 89.9 | b | same template as 0x0048d900 |
| 0x0048dd40 | 505 | 89.9 | 89.9 | b | same template as 0x0048d900 |
| 0x0048df60 | 505 | 89.9 | 89.9 | b | same template as 0x0048d900 |
| 0x0048e360 | 619 | 84.0 | 84.0 | b | original keeps the size stores and recomputes per component (no CSE of rotv*10) |
| 0x0048e820 | 599 | 85.5 | 100.0 | b | ops 100%; esi/edi swapped between i and the hoisted 1.0f; permuter 423 compiles, no gain |
| 0x0048ebf0 | 1010 | 71.8 | 92.9 | b | register rotation; first-call block recomputes param_3*0x488 |
| 0x0048f780 | 287 | 98.6 | 98.6 | b | fade fstp scheduled one store late |
| 0x00490020 | 864 | 90.9 | 97.9 | b | head eax/ecx colouring for player/mode, colour-byte store scheduling |
| 0x00490500 | 2353 | 56.0 | 61.0 | b | structure complete (661/661 ins); original keeps 3 outer induction pointers + 8-aligned frame |
| 0x004910c0 | 620 | 97.2 | 97.2 | b | far-plane fstp scheduled after the next push |
| 0x00491340 | 323 | 98.1 | 98.1 | b | one fld [edi] scheduled after the counter load; permuter no gain |
| 0x00491590 | 491 | 92.9 | 97.2 | b | fst vs fld st(0)+fstp duplicate; eax/ecx/edx naming in the pushes |
| 0x00491b20 | 173 | 93.6 | 93.6 | b | original reloads param_2 twice and reuses edx for the d[] copies |
| 0x00491f00 | 611 | 88.3 | 89.4 | b | x87 operand order on d[0]/d[2]; age kept in two registers |
| 0x004924f0 | 414 | 71.8 | 74.8 | b | original eliminates i (pointer compares), ours keeps it |
| 0x00493640 | 172 | 67.9 | 67.9 | b | original places the return-0 block first; no tried form reproduces it |
| 0x00493710 | 430 | 94.2 | 94.2 | b | original pops the first 3 call args before the 0x200000 call |
| 0x004938e0 | 28 | 80.0 | 90.0 | b | eax/ecx swap of the two loop temporaries |
| 0x00493900 | 448 | 66.9 | 67.7 | b | strtok loop: ours rotates it, original keeps one call site (pointer in ebp) |
| 0x00493fd0 | 631 | 80.1 | 94.4 | b | stack slot order of the short outputs; eax/ecx swap RwGlobals/colour |

Classes: (a) compiler-version codegen, (b) source shape not found in budget, (c) tool limit. All 26 are (b).

Held static callees (in-file copies of another RVA): 0x0048d0f0 (100%, its own file `0048d0f0.c` with a stand-in caller
under `#ifndef __cplusplus`) is also defined in the nine particle emitter files 0048d140 .. 0048e360 (all below 100%,
none swapped); 0x00491e50 (100%) lives in `00491f00.c` (88.3%). Thunks 0x00493540 / 0x00493550 / 0x00493560 matched as
C tail calls (`return FUN_x();`), 5 B each.

Priority gaps (`verify/match_topdown_20261101/gap_value.tsv`): 0x004929d0, 0x00492d30, 0x0048f8a0, 0x0048fb10, 0x004922e0
reached 100%; 0x00490020 stays at 90.9%. 0x004929d0 / 0x0048f8a0 / 0x0048fb10 have no port body, so no swap (a new body
with no caller is not C4, L-68). 0x004922e0 is swapped. 0x00492d30 is NOT-SAFE (port bug, below).

## Swap (port leg)

`swaps.tsv` has one row per swapped RVA: wrapper, targets, x87, port symbol, install, source blob, match, relocheck,
old body. `bodies_pre.tsv` / `bodies_post.tsv`: each of the 38 RVAs has exactly one body after the swap, its
`MatchS15_` wrapper.

| targets | rows |
|---|---|
| exe + asi | 14: 0x004904d0, 0x00490f50, 0x00490f80, 0x00492440, 0x004922e0, 0x00492340, 0x004921d0, 0x00492270, 0x00492770, 0x00493480, 0x00493540, 0x00493550, 0x00493560, 0x004938c0 |
| asi only | 24: 0x0048ebc0, 0x0048f260, 0x0048f590, 0x0048f680, 0x0048f6b0, 0x0048f6e0, 0x0048f710, 0x00490ff0, 0x00491010, 0x00491070, 0x00491bd0, 0x004924c0, 0x004924e0, 0x00492d10, 0x00492d20, 0x00493570, 0x00493580, 0x00493590, 0x00493b40, 0x00493f70, 0x00493f80, 0x00493fc0, 0x00494460, 0x00494480 |

- `py -3.12 scripts/lint_rva_bodies.py` -> `2 finding(s): CROSS-TARGET=1, DUP-IN-TARGET=1 | allowlisted=2 NEW=0`
  (unchanged). `py -3.12 scripts/lint_inline_asm_abs.py` -> OK (1171 files). `re/tools/dual_copy_allowlist.txt`
  unchanged (no swapped RVA had an entry).
- **x87_tus.txt: one entry, `MatchS15_00493480`** (FpsDiscretise). Its old body sat in an SSE TU but did the float step
  in inline-asm `fild/fmul/fstp`, so x87 keeps that form. Every other old body was SSE-compiled C and its wrapper stays
  SSE, which keeps each body's codegen mode (the S11 rule). 0x0048f590 uses float but only copies 4-byte values, which
  SSE copies bit-exactly.
- **Matched-source edits (C++ compat, DDK bytes unchanged, `match.py` 100% and `relocheck` OK re-run after each):**
  00492270.c (prototype FUN_00493710(int)), 00494480.c (prototypes FUN_004938e0 / FUN_00493fc0 take one arg),
  00493bc0.c (`wchar_t` typedef under `#ifndef __cplusplus`), 0048ffd0.c (callee renamed `RpClumpDestroy` ->
  `FUN_004e6e00` so relocheck checks it).
- Raw-address forwarders (the old body called the original by address, no port body on this branch): 0x004921d0 (12
  callees), 0x00492270 (0x00493710, 0x004921d0, 0x00428590, as the old body did on purpose), 0x00492770 (0x0042b920,
  0x0042b950, 0x00433240, 0x00495110, 0x004c57a0), 0x00493480 -> 0x00493390, 0x00493540 -> 0x00495150, 0x00493550 ->
  0x004938c0, 0x00493560 -> 0x004954f0, 0x004938c0 (5 callees), 0x00492d20 -> 0x004967e0, 0x00494460 -> 0x00494320 /
  0x004c7650, 0x00494480 -> 0x004942b0. Port-body binds: 0x004922e0 binds `FUN_0040e470` to `CarSlotStateGet` and
  `FUN_0040e350` to `GetRenderSubMode` (MatchS02); 0x00494480 binds `FUN_00493fc0` to `AspectRatioGlobalGet` (this
  slice's wrapper) and `FUN_004938e0` to `Walk4938e0` (asi). When 0x004942b0 / 0x00494320 / 0x00493390 get port
  bodies, rebind.
- 0x004921d0 DisplayInit: the matched source reads its two strings at 0x005cf6a4 / 0x005cf698 where the old body used
  port literals with the same bytes. In the exe that range holds no MASHED .rdata, but the function has no exe caller
  (same pattern as merged MatchS04_0041c320).
- 0x00490f50 / 0x00490f80: the exported `ByteQuadSet50/80(int,int,int,int)` keep the old shadow A/B front and call the
  matched `ByteQuadSet50Body/80Body` (S06 0x004274e0 precedent).
- 0x00490ff0 RainSetCameraScale is now void like the original; the old body returned param_2 to copy eax for the
  int_pair A/B, so the hooks_registry `ret='uint32'` entry may need `void`.
- 0x00494460 / 0x00494480: the old "bodies" were static raw calls into the original (`OrigVideoClose`,
  `OrigInputCheck`); the wrappers name them `IntroVideoClose` / `IntroInputCheck` (install 0), and
  `HardwareShowIntroVideo` calls them. 0x00493fc0: the static raw forwarder `OrigAspectRatioGlobalGet` (a second
  scanner body) is gone; `AspectRatioSnapshot` calls the wrapper.
- Return-type changes with the same value: 0x00493540 bool, 0x004924e0 `unsigned char*` (0x006147b4), 0x0048f590
  `(float*, float)` (same 4-byte stores).
- Scanner note (w4): `rva_body_scan`'s second pass counts an `extern` declaration as a body when a `{` follows within 6
  lines; the `AspectRatioGlobalGet` declaration in `SplashGameMode_t5.cpp` was moved to the top of the TU for that.

**4 byte-identical rows with a port body NOT swapped (NOT-SAFE).**

| rva | old body | reason |
|---|---|---|
| 0x004943d0 | Render/Vtbl5cf9e8ScalarDeletingDtor.cpp (asi) | `MASHED_NEGCTL` knob with three negative-control branches; `free` would need a raw forwarder to 0x004a3bda |
| 0x00492e60 | Boot/SubsystemInit.cpp SetDefaultViewWindow (both, install mass-disabled, no callers) | **port defect:** old calls FUN_004671a0(-1, &v) then FUN_004c1c80(handle); original FUN_004671a0(-1) then FUN_004c1c80(handle, v). -> MATCH-FIX-492E60 |
| 0x004926c0 | Boot/FrameDispatch.cpp AudioTickAndAvg (both, install mass-disabled, no callers) | **port defect:** button-4 branch inverted (old: if button { if toggle run FUN_0045d3f0/FUN_00466b50/FUN_0045d430 else toggle=0 }; original: if button toggle=0 else if toggle run the three); old divides unsigned by 60, original signed. -> MATCH-FIX-4926C0 |
| 0x00492d30 | Util/UtilMid.cpp GameTickStateMachine7 (asi, install mass-disabled, no port callers) | **port defect:** switch paths differ: cases 2/3/default call 0x0043dfd0+0x0043d7c0 in the old body where the original goes to the tail; 5/6 skip both where the original calls both; 4/7 skip 0x0043d7c0. The .c also needs C++ arity fixes (FUN_004111c0, FUN_00498860). -> MATCH-FIX-492D30 |

**61 byte-identical rows with no port body**, not swapped (a new body with no caller is not C4, L-68): 0x0048cf70,
0x0048cf90, 0x0048d0f0, 0x0048e5f0, 0x0048e7b0, 0x0048eac0, 0x0048f290, 0x0048f420, 0x0048f740, 0x0048f8a0,
0x0048f9f0, 0x0048fb10, 0x0048fce0, 0x0048fd10, 0x0048fd40, 0x0048fd70, 0x0048fdd0, 0x0048fe70, 0x0048fef0,
0x0048ff20, 0x0048ffd0, 0x00490000, 0x00490380, 0x00490490, 0x00490e50, 0x00490e70, 0x00490fb0, 0x00491030,
0x00491490, 0x004914b0, 0x00491530, 0x00491780, 0x004917a0, 0x004917c0, 0x00491860, 0x00491900, 0x004919b0,
0x00491a10, 0x00491a70, 0x00491c00, 0x00491e50, 0x00492170, 0x004921b0, 0x00492290, 0x00492370, 0x004927c0,
0x004929d0, 0x00492e90, 0x00493390, 0x004935a0, 0x004935d0, 0x00493600, 0x00493ac0, 0x00493b50, 0x00493bc0,
0x00493c00, 0x00493c60, 0x00493f00, 0x004942b0, 0x00494320, 0x004943f0.

## Exe callers (L-68, L-89)

Binary witness on this build: `py -3.12 verify/matchs15_20261102/tools/exe_reach.py mashedmod/build/mashed_re.exe
mashedmod/build/mashed_re.map <symbols>` (direct `call`/`jmp rel32` graph from the linker map, walked from
`_WinMain@16`; `exe_reach.txt`). `addr_taken=yes` on every row is the install table, not a caller.

| rva | symbol | direct chain from WinMain |
|---|---|---|
| 0x00492770 | MainLoopInit | ExecuteFrontendBootChain (exe_main.cpp:9300, step 7, inside SEH, unconditional) <- WinMain (exe_main.cpp:9549, after RaceSubmit_Init, no env gate) |
| other 13 exe+asi rows, 24 asi-only rows | | none (exe-callers=0) |

## Gates

**Not run: display unavailable** (L1 instruction for this run: DISPLAY IS OFF on LIVING-PC, `CreateDevice` returns
D3DERR_NOTAVAILABLE; no gate, D2 arm, probe-render or game run attempted). Owed before review:
`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on this exe, compared with
`re/orchestrator/merge_train.json` `gctrl.ref` / `b.ref`, and the merge train's `-Full` pass (D2 x3, 13 tracks), since
`mashedmod/src` is in `full_set_paths`. The swap the default build executes on every start is 0x00492770 MainLoopInit
(boot chain); none of the 38 runs in the race frame.

## C4 re-classify text (apply after the merge and the gates, through re-classify; not applied here)

Common evidence: `match.py re/match/s15/<va8>.c` 100% under `flags=/O2 /GS /Ob1 tc=ddk`, `relocheck.py` OK,
one body (`Match/MatchS15_<va8>.cpp`), exe caller chain from `WinMain` (table above), gates unchanged on this build.

| rva | name | level | match.py | relocheck | old body (removed) |
|---|---|---|---|---|---|
| 0x00492770 | MainLoopInit | current -> C4 | 100% | OK unchecked=0 | Boot/FrameDispatch.cpp |

CHANGELOG line: `2026-11-0x  0x00492770  MainLoopInit  ->C4  route match (c4=match), exe-callers via
ExecuteFrontendBootChain <- WinMain; evidence: verify/matchs15_20261102/RESULT_MATCH-S15.md`.
All 37 other swaps: stay at their level, `notes` += `match evidence: identical, swapped, exe-callers=0; C4 when a caller
lands (L-68 ruling)`.

## Build

`set MASHED_NO_DEPLOY=1&& mashedmod\build.bat`, Build OK on the first build, .asi not deployed. exe sha256
`c1bafbac04b92df9f48ad45caa3a040fca6c834484dfbbad08cd8ddef07f158a`, asi
`beb4fd3b27059eeee5e187b86282fb1fa15f83b549921329aefb1a293944461c`.

## Raw files (not committed; `D:/MashedWT/MATCH-S15_data`)

| file | sha256 |
|---|---|
| decomp.json (decomp_pc.py --port --callees, 129 entries, slot Mashed_pool0; 0x004917a0 entry merged from decomp_create.json) | 7dd716422a562cff858fd41dba3a24598c49a82e4a60570b09a6fc1398ce8794 |
| decomp_create.json (decomp_pc.py --create, 0x004917a0, slot Mashed_pool10) | b16f906e978ec924539826338f439644c347859330a439d5d9ceed8035d94e01 |
| g2s/results.tsv (raw converter scores) | 841c50e38a7d31831b5c9cfd791d32cd2e8b7b9eb25b98e3d8319181f376e521 |
| bodies.tsv (pre-swap body scan; committed copy bodies_pre.tsv) | d5cb6605ac39ec517804c20b36005a1b617ffba75e0fec8eb66b314b1549e592 |
| build1.log (Build OK) | 5e77d1d3b1380f6eacbd80cf2b4691d82d724157e904fcf754aa623e234f31a3 |
| g2s/, notes/h1..h8.md, scratch/, swap_cands.txt | raw converter output, helper notes, scratch |
