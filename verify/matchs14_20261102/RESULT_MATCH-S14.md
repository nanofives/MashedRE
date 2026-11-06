# RESULT MATCH-S14, 2026-11-06 (run 6, resumed from the run-5 WIP)

**Verdict: GREEN.** Slice 0x004840d0..0x0048cee0 (128 game functions). **97/128** sources are byte-identical under
`re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178, `/O2 /GS /Ob1`, C, x87), each with `relocheck.py` OK
and `unchecked=0` in its best source file. **38** of them are now the port's only body (38 wrapper TUs
`Match/MatchS14_<va8>.cpp`): 3 in `mashed_re.exe` and the .asi, 2 in the exe only, 33 in the .asi only.
Gates on the swap build: G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9` x3 = `merge_train.json` refs (unchanged).
**3 rows C3/C2 -> C4 by route `match`** (0x00484c70, 0x00484c90, 0x00484cf0): each body runs in the default arms
(counter build, below). The other 35 swaps are tracker-only (L-68 / held, below).

Branch `orch/MATCH-S14`, cut from orch/integration 3c6510ee6. orch/integration (adf5ff69b) merged at 0d0f06c3e before the
swap (no conflicts). Since then orch/integration moved to b1dd84b7c (S13, S15, S09-FU, D4-DC-77b0-ST5-RING merged). The
refs did not change and no FLIP landed, so this branch was not re-merged. The S14 rsp block sits after `"Race\WorldObjectLinks.cpp"`
in both rsp files (S09 uses PickupPoolSpawn, S10 FloatSliderStep, S11 DrawQuadPrimitives). `x87_tus.txt` is not edited.

## Match (static leg)

| | count |
|---|---|
| slice functions | 128 (`slice_rvas.txt`) |
| raw converter (g2s) at 100% | 35 |
| hand-matched to 100% (helpers h1..h7, run 5) | 62 |
| of the 97, byte-identical by the whole-symbol rule (Ghidra size short of the real end) | 10 (0x00484cf0, 0x00486220, 0x004862d0, 0x00486f90, 0x00489290, 0x004892c0, 0x0048a460, 0x0048a5d0, 0x0048ae00, 0x0048bb70) |
| below 100% | 31 (table below) |

**Lead re-verify (run 6).** Every `re/match/s14/*.c` (108 files) re-run here: `collect.py` -> `results.tsv`,
`relocheck.py` on every `// MATCH:` line -> `relocheck.tsv` (122 lines; every BAD line belongs to a function below 100%,
where the streams do not line up), and the VS2022 `/TP` C++ check -> `cxxcheck.txt` (85 OK, 23 FAIL before the swap fixes).
The 62 helper-reported rows were all confirmed. After the C++-compat edits of the swap leg (5 files), `match.py --bytes`
and relocheck were re-run on those 5 files, and on all 38 swapped sources again (`c4_evidence.tsv`: all BYTE-IDENTICAL,
relocheck OK, source sha256 per row).

| rva | size | best % | regs % | class | residual / status |
|---|---|---|---|---|---|
| 0x004840f0 | 59 | 66.7 | 71.4 | b | index arrives in ECX (orig EDX + lea ecx count ptr); static callee held in 004841d0.c |
| 0x004841d0 | 159 | 75.0 | 92.9 | b | ECX/EDX follow the callee convention; letters loop ESI/EDI swapped |
| 0x004847d0 | 628 | 34.9 | 37.3 | b | original keeps one pointer IV per cell field (10, 7 spilled); ours one IV |
| 0x00484ac0 | 424 | 70.3 | 70.3 | b | EBP load before null test, x87 keeps a dead sum, stack slot sharing, loop entry jmp |
| 0x00484de0 | 394 | 99.2 | 99.2 | b | one spill scheduled before `je` instead of after `lea`; 195 permuter variants |
| 0x00484f70 | 254 | 68.1 | 70.3 | b | original 3 pointer IVs at +4; ours derives the j pointer from an i pointer at +0x10 |
| 0x00485b30 | 147 | 46.9 | 70.3 | b | zero reg EBX + data in EDI (3 saved regs) not reproduced; permuter tried |
| 0x00485ef0 | 800 | 19.8 | 40.5 | - | partial, run 5 stopped (no helper note) |
| 0x004864f0 | 273 | 90.1 | 90.1 | b | one `fld` scheduled a slot early in the scatter loop |
| 0x00487020 | 284 | 96.3 | 96.3 | b | x87 keeps a negated term on the stack instead of consuming it |
| 0x00489a40 | 494 | 78.0 | 86.7 | - | partial, run 5 stopped |
| 0x00489dc0 | 657 | 36.1 | 60.5 | - | partial, run 5 stopped |
| 0x0048a130 | 656 | 93.0 | 94.0 | b | fsubr operand order, add order on y; permuter swapped d.x/d.y (natural 87.1) |
| 0x0048a4e0 | 238 | 98.8 | 98.8 | b | guard `test ecx,ecx` vs original `cmp ecx,ebp` (zero CSE); many spellings tried |
| 0x0048a780 | 164 | 49.1 | 58.2 | b | original keeps &b->f8 in ESI and b in EDI |
| 0x0048aa50 | 272 | 85.4 | 85.4 | b | x87 store scheduling of `v*param_3` against the call pushes |
| 0x0048b7c0 | 407 | 46.4 | 52.6 | - | partial, run 5 stopped |
| 0x00484310, 0x004844a0, 0x00484580, 0x00488320, 0x00488e70, 0x0048ab70, 0x0048aff0, 0x0048b230, 0x0048b440 | 68..888 | - | - | - | not attempted (run-5 helper lists stopped) |
| 0x004854e0, 0x00486830, 0x00487280, 0x004883b0, 0x00489500 | 907..2738 | - | - | - | out of scope (>900 B, no port body) |

**No permute in run 6.** None of the 31 rows below 100% has a port body except 0x004840f0, 0x00485b30 (asi) and
0x004847d0, 0x00484ac0, 0x00484de0, 0x00484f70 (exe naked bodies). The exe ones were already worked to budget in run 5
(0x00484de0: 195 permuter variants, one instruction), and the others are structural (class b, under 71%). A row with no
port body can only become a tracker-only match. So run 6 spent the time on the swap leg. The 31 rows go to a follow-up
ledger row (below).

Held static callees (in-file `// MATCH:` copies of another RVA; none of these files is swapped): 004841d0.c holds
0x004840f0, 00484ac0.c holds 0x00484a50, 004853f0.c holds 0x004852e0, 00485420.c holds 0x00485380, 00485c20.c and
00485e50.c hold each other, 00486370.c / 004864f0.c hold each other, 00487150.c holds 0x00487e00, 0048a130.c holds
0x0048a060, 0048a490.c / 0048a5d0.c hold each other, 0048a780.c holds 0x0048a490.

## Swap (port leg)

`swaps.tsv`: one row per swapped RVA (wrapper, targets, port symbol, install, source, match, relocheck, old body).
Helper reports `swap_w1.txt` (w1: world objects, physics, misc asi rounds) and `swap_w2.txt` (w2: PromoLoop_sessionB and
sky/round TUs). `bodies_pre.tsv` / `bodies_post.tsv`: scan before and after.

| targets | rows |
|---|---|
| exe + asi | 3: 0x00484c70 WorldObjectsBaseGet, 0x00484c90 Zero484c90, 0x004853b0 SmplFzxStateBlockGetLogged |
| exe only | 2: 0x00484cf0 RG_484cf0, 0x00485070 RG_485070 (bind to the exe-only naked RG_4847d0 / RG_484ac0 / RG_484de0 / RG_484f70) |
| asi only | 33: 0x004840d0, 0x00484170, 0x004842b0, 0x004850b0, 0x00485340, 0x00485360, 0x00485370, 0x00485a00, 0x00485a70, 0x00485bd0, 0x00485bf0, 0x00486610, 0x00486f90, 0x00487140, 0x00487df0, 0x00487e40, 0x004880a0, 0x004881d0, 0x00488390, 0x00489290, 0x004893a0, 0x004893b0, 0x004893d0, 0x00489450, 0x00489480, 0x004894a0, 0x004894f0, 0x004899c0, 0x00489da0, 0x0048a460, 0x0048a630, 0x0048a830, 0x0048ade0 |

- Build `MASHED_NO_DEPLOY=1` (exe `a44a36f5`): `[rva-lint] 1 finding(s): CROSS-TARGET=1 | allowlisted=1 NEW=0` (unchanged),
  `[asm-lint] OK -- no bare absolute asm operand in 1253 source files`. `re/tools/dual_copy_allowlist.txt` unchanged
  (no swapped RVA had an entry).
- **No `x87_tus.txt` entries.** None of the old TUs is in `x87_tus.txt` (both helpers checked), so every old body was
  SSE-compiled. The wrappers stay SSE, which keeps each body's codegen mode (S11 precedent). No physics TU is swapped:
  `Physics/SmplFzxStateBlock.cpp` (0x004853b0) is not in `x87_tus.txt` and the function has no float.
- `re/tools/naked_specs/registry_m1.json` drops RG_484cf0 and RG_485070: their verbatim naked bodies left
  `Race/WorldRegistry_naked.inc` (removed by hand, excise anchors on the `#include`), so a regenerated .inc must not
  bring them back.
- Matched-source edits for C++ (DDK bytes unchanged, re-verified): 004853b0.c (`FUN_004987b0(const char*, ...)` under
  `__cplusplus`), 00486610.c (callee args as dwords), 00487140.c (callee `void*`), 0048a830.c / 0048ade0.c (`(byte *)` cast).
- Raw-address forwarders (old body called the original by address, no callable port body): 0x00484170 none;
  0x004853b0 -> 0x00485340, 0x004987b0 (naked variadic tail-jump); 0x00485bf0 -> 0x004a332b (noreturn, as the matched
  source declares it); 0x00486610 -> 0x004864f0; 0x00487140 -> 0x004768c0 (tail-jump, EAX passes through, `/FAs` checked);
  0x004880a0 / 0x004881d0 -> 0x004c0b30, 0x00534b60, 0x004e7e30, 0x004e8090. Port-body binds: 0x004893a0
  `FUN_00476cb0=Batch476cb0`, 0x0048a830 / 0x0048ade0 `FUN_004b6480=BitArrayClear`, 0x004881d0 `FUN_004880a0=Obj7030acCreate`,
  0x00484cf0 `RG_4847d0` / `RG_484ac0`, 0x00485070 `RG_484de0` / `RG_484f70`.
- Input-domain and return notes (w1/w2): 0x004899c0 loops backward as the original (old port body forward; same stores
  unless the source aliases the arrays); 0x00489450 / 0x004894a0 / 0x00486f90 copy floats where the old body copied dwords
  (SSE copies are bit-exact); 0x004850b0 / 0x0048a630 `undefined4` vs old `int` (same bits); 0x004880a0 / 0x004881d0 /
  0x00487140 matched void where the old body returned EAX (capstone: the only caller chain 0x00402918 does not read EAX);
  0x00484170 divides by `[0x005cc320]` (1.0f in the image) where the old body used a literal 1.0f; 0x004853b0's format
  string is the port's own literal (same text as `.rdata` 0x005cf310).

**Not swapped.**

| rva | reason |
|---|---|
| 0x00484a50 | NOT-SAFE: no file of its own. The matched form is the register-arg static callee inside 00484ac0.c, whose own line is 70.3%. A swap needs a second file plus a naked EAX/ECX-to-stack adapter for the naked callers in `WorldRegistry_naked.inc` (0x00484b8d, 0x00484c44). Old body stays. Follow-up. |
| 0x00486f50, 0x00489890 | SKIP: the only port code is an installed asi body in `Lane2/L2_00486f50.cpp` / `L2_00489890.cpp` (ShadowAB transcription), no port callers. Lead decision whether a Lane2 body is swapped. |
| 0x00487d00 | SKIP: `HUD/ScenarioWriters_sa2s1.cpp` `Particle3rdEmitterSubmit` (both rsps, installed). Its RVA comment is not in the scanner's format, so the scanner lists only the install. No port callers. |
| 59 other 100% rows | no port body anywhere (L-68: a new body with no caller is not C4) |

## Exe callers and execution (L-68, L-89)

`exe_reach.txt` (`tools/exe_reach.py` on `mashed_re.exe` / `.map`, direct call graph from `_WinMain@16`):

| rva | symbol | direct sites | shortest chain |
|---|---|---|---|
| 0x00484c90 | Zero484c90 | 1 | Race::Registry_RaceInit <- Ai_BridgeLoad <- TrackRenderer::Load <- WinMain |
| 0x00484c70 | WorldObjectsBaseGet | 1 | TG_414c30 <- ModeSeven_MaybeReplay <- TrackRenderer::Load <- WinMain (also reached via `Body7()` in AiTargetClosest, an indirect call the direct graph does not see) |
| 0x00484cf0 | RG_484cf0 | 3 | Registry_MaybeReplay <- TrackRenderer::Load <- WinMain (shortest only; the other sites are Registry_Register, `PickupTick_naked.inc:196`, `CarRegistrant_naked.inc:26`, MatchS05_0041f710) |
| 0x00485070 | RG_485070 | 2 | Registry_MaybeReplay (shortest; the default caller `Registry_FrameTick` calls it through a cast pointer) |
| 0x004853b0 | SmplFzxStateBlockGetLogged(Body) | 0 | none (address-taken only: its `/EXPORT` and install) |

Because two of the shortest chains run through env-gated replay harnesses (L-89: not an exe caller), execution was
measured directly. **Counter build** (uncommitted; `counter_build.diff` in the data dir): one `S14Count(k)` call at the
top of each of the 5 exe bodies, logging each power-of-two count to `counts.txt`. Exe `5827b640`. H_DEF (`MASHED_GAME_MODE=10`,
Training, no `MASHED_*REPLAY*` variable) and B_DEF_K0 run on it in the same solo window as the clean H_DEF retry. The counter
build's H_DEF hashes are the reference hashes too (`8284be46/ba407a16/31fe043e`).

| rva | calls reached (power-of-two log, both arms) | C4 leg (2)+(exe caller) |
|---|---|---|
| 0x00484c90 | 1 per race | holds |
| 0x00484c70 | >= 16384 | holds |
| 0x00484cf0 | >= 65536 | holds |
| 0x00485070 | >= 8192 | executes, but see "held" below |
| 0x004853b0 | 0 | exe-callers=0 |

## Gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on the swap build `a44a36f5` (copy in
`D:/MashedWT/MATCH-S14_data/exe_clean/`, `A4_EXE`), private root `D:/MashedWT/MATCH-S14_root` (`MASHED_ROOT`), test slot
outside, `mashed_lock.py run --solo` inside. First window: the three B arms exit 0 with `1d5bf4f9`. H_DEF's HAL
`CreateDevice` hung (`InitD3D9: CreateDevice(HAL/HW,depthA,vsdef) -> HUNG hr=0x80004005`), the run fell back to the REF
device and exited 0xC0000005 at 38 s with no output (`arms/H_DEF.log`). That is a display failure, not the swap: the B arms of
the same exe got the HAL device. H_DEF re-run once (`merge_train.json` gctrl `retries: 1`) in a second window: exit 0, 527 s.

| arm | file | sha8 | ref (merge_train.json, read 2026-11-06 at orch/integration b1dd84b7c) |
|---|---|---|---|
| H_DEF (retry) | step.csv / gates.csv / ptrace.log | 8284be46 / ba407a16 / 31fe043e | 8284be46 / ba407a16 / 31fe043e |
| B_DEF_K0, K1, K26 | step.csv | 1d5bf4f9 x3 | 1d5bf4f9 x3 |

The merge train's `-Full` pass (D2 x3, 13 tracks) was not run here (the train runs it).

## Trackers (re-classify, `tools/reclass_s14.py`, one transaction)

`hooks.csv`: 38 rows, `file` = the wrapper, `exe_file` cleared, notes appended with the match command, relocheck line,
source sha256, port body, gates and this RESULT. `re/analysis/CHANGELOG.md`: 38 lines below `<!-- ENTRIES -->`.
`re/orchestrator/tracker_lint.py --repo . --base 76991c005`: `OK -- hooks.csv 5969 rows ... pre-existing=4 NEW=0`.
`lint_rva_bodies.py`: NEW=0. No STUBS / UNCERTAINTIES / DEFERRED rows (the matched bodies call no stub helper).

| outcome | rows |
|---|---|
| **C4 by route `match`** (`c4=match`) | 0x00484c70 WorldObjectsBaseGet C3->C4, 0x00484c90 C3->C4, 0x00484cf0 C2->C4 |
| held at C2, NOT C4 (rule leg 2) | 0x00485070: matched, the exe body executes, but the .asi still hooks 0x00485070 with a different body (`Lane2/L2_00485070.cpp` `L2_FUN_00485070`, a ShadowAB transcription). C4 when that install uses the matched source or is dropped |
| L-68, exe-callers=0, level kept | 0x004853b0 (C2) and 29 asi-only rows (C3) |
| L-68, already C4 on the frida route | 4 asi-only rows: notes/file only |

## Follow-ups (ledger rows added)

- `MATCH-S14-FU`: the 31 rows below 100% (26 in scope + the 5 large ones, list above) plus 0x00484a50 (needs a second
  file and an EAX/ECX adapter) and the 0x00485070 Lane2 asi install.

## Raw data (not committed), `D:/MashedWT/MATCH-S14_data`

```
8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 *arms_h2/H_DEF.step.csv
ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb *arms_h2/H_DEF.gates.csv
31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 *arms_h2/H_DEF.ptrace.log
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 *arms/B_DEF_K0.step.csv
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 *arms/B_DEF_K1.step.csv
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 *arms/B_DEF_K26.step.csv
2b90a376be1b66131bd05a3900d24d75562b7692f13074a16ab6ba8be40c6278 *arms/H_DEF.log   (first window, HAL hang)
8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 *arms_cnt/H_DEF.step.csv   (counter build)
ae97a1da9be4f0a8ade2b8277d235c0117ce222d62ef0717799062d5f408e5f3 *counts.txt
df74acff3f636c870fa0ca5a63fc42186581411455afce8e9f84c30aacf489a0 *counter_build.diff
52db2e05bf1d22b97ac28b7145a9e1897b54a53ba4dd9bf94b628ac36fa3cc1b *gates.log
2476320f2d67383f9cfa79549f5a4b14a64456e7391bb1275230a0e65af549b4 *gates_h2.log
a44a36f50aab2889f3428b2514c2df2258550341b44a6c58c65130fd35854e24 *exe_clean/mashed_re.exe
5827b640c93d752fbefff19d21e1c87eadbb47b10f73e875c5637680fe9795f1 *exe_counter/mashed_re.exe
bdb693b22bbef88fc2d7c6830c7fe4ef098e97b33c3262477131716ebb21f462 *decomp.json
a435f1d3a85427ac3014481aa09db4ac3de5f8094035a1e14e99f9d78372b4c5 *g2s/results.tsv
```
Also: `build1.log`, `build_counter.log`, `run6/` (collect, relocheck, cxx raw), `notes/h1..h7.md`, `scratch/`.
