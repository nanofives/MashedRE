# RESULT MATCH-S12, 2026-11-06

**Verdict: GREEN. Built @ this branch, default gates equal to the merge_train.json refs.** Slice 0x00474e70..0x0047aaa0 (129 game functions, 0 library rows inside). **108/129**
sources are byte-identical under `re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178, `/O2 /GS /Ob1`, C,
x87), and every one of the 108 has `relocheck.py` OK in its best source file (`relocheck.tsv`; 21 rows carry 1-3
`unchecked` names, all CRT or import symbols: `__ftol`, `__security_check_cookie`/`___security_cookie`,
`_RpClumpStreamRead`, `_FID_conflict__wprintf`, which relocheck leaves unchecked by name). **18** of them are now the
port's only body (18 wrapper TUs `Match/MatchS12_<va8>.cpp`): 8 in `mashed_re.exe` and the .asi, 10 in the .asi only.
**C4 candidates: 0 rows.** The three swaps with a direct exe chain from `WinMain` reach it only through the pickup glow,
which runs only with `MASHED_RFX_EMITTERS=1` (default OFF, not in FaithDefaults: `Gameplay/PickupGlow.cpp:47-53`,
`Gameplay/PickupGlow.h:15`), so under L-89 none has a default-build caller (table below).

Branch: `orch/MATCH-S12`. Run 5 stopped at the match leg (WIP b2f318ff7, 85/129). Run 6 (this one) merged
orch/integration (9a0f7def5: S09, S11 and the T13/PORTSTATE rows are in) before the swap leg. Insertion anchors are
S12-only: the rsp block sits after `"Render\RfxEmitter.cpp"` (S11 uses `DrawQuadPrimitives`, S13 `RwpIntegrator`,
S15 `LaunchHandshake`, S16 `Boot\VideoConfig.cpp`), the x87 block after `MenuMenusBB`.

## Match (static leg)

| | count |
|---|---|
| slice functions | 129 (`slice_rvas.txt`) |
| raw converter (g2s) at 100% | 31 |
| hand-matched to 100% (helpers h1-h6 in run 5, h7-h9 in run 6; `helper_notes.txt`) | 77 |
| of the 108, byte-identical by the whole-symbol rule (`by` = `bytes`, Ghidra size short of the real end) | 12 (0x00475ef0, 0x00476390, 0x004764f0, 0x00476650, 0x00476710, 0x004767c0, 0x00476df0, 0x004775b0, 0x00477870, 0x00477a10, 0x00478200, 0x00478cf0) |
| below 100% | 21 (table below; budget spent) |

Tool-data fix: `re/console/pc_funcs.csv` lacked 8 track functions (0x00479030, 0x0047a440, 0x0047a750, 0x0047a7d0,
0x0047a7f0, 0x0047a810, 0x0047a950, 0x0047a9b0), so match.py refused to score them. Sizes from `decomp.json`
(`decomp_pc.py`, `exact_entry` true): 174, 88, 64, 31, 31, 103, 93, 112 B. All 8 then score 100% by instruction.

Bounded permute (run 6): `match_permute.py --rounds 6 --walk 40 --jobs 2 --time 300` on the six near-misses with no
earlier permuter pass (0x00475210, 0x00475a00, 0x00476320, 0x004770a0, 0x00477920, 0x004783f0): round 1 found no
improvement on any of them (85-245 compiles each), so each stopped there. 0x00477f00, 0x004785e0, 0x0047a020 and
0x0047a320 had permuter passes in run 5 (helper notes).

| rva | size | best % | regs % | class | residual (helper notes) |
|---|---|---|---|---|---|
| 0x00474e70 | 186 | 86.0 | 89.5 | b | p1-p3 vector: fsubr operand order on y/z, two fstp/lea scheduling slips |
| 0x00475210 | 182 | 93.1 | 93.1 | b | y/z sums load out.y/z before param_1->y/z (4 ins); permuter no gain |
| 0x004752f0 | 997 | 95.0 | 95.0 | b | keyframe slerp sampler; RwMatrixTranslate pushes and pos zero stores scheduled elsewhere |
| 0x00475770 | 186 | 54.4 | 54.4 | b | matrix->euler: x87 register allocation of m6/m10 across the compare, |m2| spill/reload |
| 0x00475a00 | 91 | 93.1 | 100.0 | b | [edx+edi] vs [edi+edx] SIB swap on both stores; permuter no gain |
| 0x00475ab0 | 337 | 32.4 | 46.6 | a | max-search loop unrolled x4 by the original, x8 by 9176 (as 0x00401690 / 0x00499d90) |
| 0x00475f90 | 804 | 96.4 | 96.4 | b | emitter render; 7 misses past Ghidra size, 1 fadd 2 slots late in loop 3 (in place 99.1%) |
| 0x00476320 | 103 | 91.7 | 91.7 | b | z add operand order + return-pop order after FUN_004c39b0; permuter no gain |
| 0x004770a0 | 31 | 90.0 | 100.0 | b | SIB base/index swap on the plugin-offset load; permuter no gain |
| 0x00477920 | 228 | 93.3 | 93.3 | b | age stored local-first vs memory-first (fst/fstp order); permuter no gain |
| 0x00477be0 | 535 | 93.2 | 93.2 | b | trail quad push: `t=*pos` copy schedule, count++ as load/inc/store vs `inc [mem]` |
| 0x00477e60 | 154 | 98.3 | 98.3 | b | plane from 3 points: `pop edi` one slot earlier than the original |
| 0x00477f00 | 70 | 96.4 | 96.4 | a | param_2 load scheduled before the first fld; dot order, struct, permuter tried |
| 0x00477f50 | 213 | 91.7 | 94.4 | b | plane distance/classify: dot z operand order + three one-slot slips |
| 0x004783f0 | 482 | 90.8 | 90.8 | b | node-base lea scheduling; permuter no gain |
| 0x004785e0 | 121 | 97.8 | 97.8 | a | one `fld st(0)` after the norm-arg load/test; permuter tried |
| 0x00478660 | 1606 | 22.6 | 27.8 | b | run 5 structural pass only, budget (no helper note line) |
| 0x00479100 | 551 | 69.1 | 77.6 | b | run 5 structural pass only, budget (no helper note line) |
| 0x00479330 | 2888 | 35.6 | 62.7 | b | run 5 structural pass only, budget (h6 scratch `b330_*`) |
| 0x0047a020 | 196 | 92.2 | 92.2 | b | over the real 199 B 98.0%: +0xf4 store 2nd in the original, 5th in ours |
| 0x0047a320 | 121 | 95.2 | 95.2 | b | count-pointer add hoisted above the copy loop |

Classes: (a) compiler-version codegen, (b) source shape not found in budget, (c) tool limit.

Held static callees (in-file `// MATCH:` copies of another RVA): 00477490.c holds 0x004773f0 / 0x004777d0, 00477730.c
holds 0x004773f0, 004775b0.c holds 0x004773f0, 00477500.c holds 0x00477810, 00477580.c holds 0x00477870, 00477760.c
holds 0x00477450, 00477810.c holds 0x00477500, 00477870.c holds 0x00477580, 004778e0.c holds 0x00477920, 00477a10.c
holds 0x004778e0, 00475f90.c holds 0x00475f30 / 0x00475f50, 00476650.c / 00476710.c / 004767c0.c hold 0x004764f0.
None of these files is swapped. In the two swapped files that held one, the copy is now `// HELD:` under
`#ifndef MATCH_PORT` (004773f0.c holds 0x00477730, 00477450.c holds 0x00477760; neither callee has a port body).

## Swap (port leg)

`swaps.tsv` has one row per swapped RVA (wrapper, targets, x87, port symbol, install, source blob, match, relocheck,
old body); `bodies_pre.tsv` / `bodies_post.tsv` are the scanner before and after.

| targets | rows |
|---|---|
| exe + asi | 8: 0x004769a0, 0x00476a10, 0x00476a30, 0x00476b90, 0x00476be0, 0x00476cb0, 0x00476d00, 0x00478cc0 |
| asi only | 10: 0x004759b0, 0x00475a60, 0x00475ef0, 0x00475f30, 0x004773f0, 0x00477450, 0x00477b40, 0x00477b60, 0x00477e00, 0x00478cb0 |

- `py -3.12 scripts/lint_rva_bodies.py` -> `1 finding(s): CROSS-TARGET=1 | allowlisted=1 NEW=0` (unchanged);
  `bodies_post.tsv`: each of the 18 RVAs has exactly one body, its `MatchS12_` wrapper.
  `py -3.12 scripts/lint_inline_asm_abs.py` -> OK (1233 files). `re/tools/dual_copy_allowlist.txt` unchanged (no
  swapped RVA had an entry; the RfxEmitter RVAs appear only in the D4-DC-RFX-CORE comment).
- **`x87_tus.txt`: 3 entries** (block after `MenuMenusBB`): `MatchS12_00475f30` and `MatchS12_004773f0` (old bodies
  were naked x87 asm), `MatchS12_00476d00` (old body copied 0x006924d8 to +0x20 with fld/fstp; only `/arch:IA32` keeps
  that, checked with `/FAs`). Every other old body was in an SSE TU (none of the old TUs is in `x87_tus.txt`), and
  its wrapper stays SSE, as in S11.
- Register-arg wrappers (pilot finding 1, `MATCH_STATIC`): 0x00475f30 (`ScaledMulAdd475f30` = naked EAX-out adapter
  over the cdecl body), 0x004773f0 (`Init4773f0` = naked EAX adapter), 0x00477450 (`Init477450_RegAbi` keeps the
  installed register ABI over the cdecl `Init477450`). The adapters hold no RVA comment; the body is compiled C.
- Port-body binds: 0x00475a60, 0x00475ef0, 0x00477b40 bind `FUN_004b6520` to `ZeroFillWrapper` (Util/TimerSlot.cpp,
  as S05/S11 did; old bodies called 0x004b6520 by raw address); 0x004759b0 binds `FUN_00474db0` to
  `RwMallocArrayZeroFill` (`Match/MatchS11_00474db0.cpp`).
- Matched-source edits (C++ compat, DDK bytes unchanged, match.py 100% and relocheck OK re-run): 00475f30.c,
  004773f0.c, 00477450.c (`MATCH_STATIC`, HELD callees), 00477b40.c (two `(undefined4)` casts).
- 0x00475ef0: the old body was the static `_impl` in `Lane2/L2_00475ef0.cpp` behind a `ShadowAB::RunTracked` wrapper;
  the install moved to the wrapper, which drops that RVA's `MASHED_SHADOW_AB` shadow lane (env-only).
  `L2_00475ef0.cpp` now holds no code and stays in `asi_sources.rsp` (as S11 kept its emptied TUs).
- 0x004759b0: the old body returned `void*`; the original is `void` (100% match), its caller 0x0040bbb0 does not
  read EAX.

**11 byte-identical rows NOT swapped (NOT-SAFE).**

| rva | old body | reason |
|---|---|---|
| 0x004769d0 | Render/RfxEmitter.cpp ParticleEmitter_SetVelocity | exe null default is the port static `s00613288` {1.0f,1.0f} (RfxEmitter.cpp:47, MASHED_STANDALONE); matched reads 0x00613288, 0 in the exe image pad |
| 0x004769f0 | Render/RfxEmitter.cpp ParticleEmitter_SetColour | exe default is port static `s00613290` = 0xffffffff; matched reads 0x00613290 |
| 0x00476a40 | Render/RfxEmitter.cpp ParticleEmitter_SetRGBA | exe default is port static `s00613294` {0,0,1.0f,1.0f}; matched reads 0x00613294 |
| 0x00476a80 | Render/RfxEmitter.cpp Copy476a80 | exe default is port static `s006132a4` (8 dwords); matched reads 0x006132a4 |
| 0x00476ae0 | Render/RfxEmitter.cpp Batch476ae0 | exe default `s00613288`; Create passes nullptr when flags&0x80 |
| 0x00476b30 | Render/RfxEmitter.cpp Set476b30 | exe default `s00613290`; Create / SetBatchColour pass nullptr when flags&0x100 |
| 0x00476c10 | Render/RfxEmitter.cpp Batch476c10 | exe default `s00613294`; Create's SetBatchUv2(e,nullptr) runs for flags 0x817 |
| 0x00476c60 | Render/RfxEmitter.cpp Batch476c60 | exe default `s006132a4`; Create passes nullptr when flags&0x1000 |
| 0x00476df0 | Render/RfxEmitter.cpp `Rfx::Flush` | C++ on the port Emitter/PTank (std::vector storage); port LockArray replaces FUN_00535700/FUN_00535910 (no C port body); skips the null-lock copy; extra `!e.ok` return |
| 0x004770c0 | Render/RfxEmitter.cpp `Rfx::Create` | C++ on the port Emitter; std::vector Alloc replaces FUN_00474db0; port PTank replaces FUN_00534b60/FUN_00534d00; FUN_004b6520, FUN_004c0b30, FUN_004e7e30 skipped; FUN_004e8090 replaced |
| 0x004756e0 | D3d9Render/TrackRenderer.cpp `TrackRenderer::ClumpAnimTick` (exe only) | port-owned C++ state (`clump_anims_` vector, `ca.t/m/ticked`, a Track::HAnim); loops every record (FUN_00426700's job); CopterAnimMatrix/LinearAnimMatrix instead of FUN_004752f0/FUN_004b47e0; never calls FUN_004c1480 |

The eight null-default rows become SAFE if the exe maps those `DAT_00613288/90/94/a4` names onto the port's static
copies under `MASHED_STANDALONE` (MATCH-T13-SEED-WRITER did this for `DAT_005f2a70`, `Frontend/SmallLeaves_t1.cpp:98`):
a guard in each `.c` around its `MGLOBAL` line. Filed as a follow-up row, not done here (beyond a C++-compat edit).

**79 byte-identical rows with no port body**, not swapped (a new body with no caller is not C4, L-68): 0x00474f30,
0x00474fb0, 0x00474fd0, 0x00475010, 0x00475180, 0x004752d0, 0x00475830, 0x004758d0, 0x00475c10, 0x00475d30,
0x00475d90, 0x00475e50, 0x00475ea0, 0x00475f50, 0x004762c0, 0x00476390, 0x00476430, 0x00476440, 0x004764e0,
0x004764f0, 0x00476650, 0x00476710, 0x004767c0, 0x00476860, 0x00476880, 0x004768a0, 0x004768c0, 0x00477490,
0x00477500, 0x00477580, 0x004775b0, 0x00477730, 0x00477760, 0x004777d0, 0x00477810, 0x00477870, 0x004778e0,
0x00477a10, 0x00477e40, 0x00478030, 0x004780c0, 0x00478110, 0x004781b0, 0x00478200, 0x00478220, 0x00478cd0,
0x00478cf0, 0x00479030, 0x004790e0, 0x00479fb0, 0x0047a0f0, 0x0047a130, 0x0047a180, 0x0047a1b0, 0x0047a1e0,
0x0047a280, 0x0047a3a0, 0x0047a440, 0x0047a4a0, 0x0047a540, 0x0047a580, 0x0047a5b0, 0x0047a5e0, 0x0047a610,
0x0047a6b0, 0x0047a6f0, 0x0047a720, 0x0047a750, 0x0047a790, 0x0047a7d0, 0x0047a7f0, 0x0047a810, 0x0047a880,
0x0047a8b0, 0x0047a950, 0x0047a9b0, 0x0047aa20, 0x0047aa50, 0x0047aaa0.

## Exe callers (L-68, L-89)

Binary witness on this build: `py -3.12 verify/matchs12_20261102/tools/exe_reach.py mashedmod/build/mashed_re.exe
mashedmod/build/mashed_re.map <symbols>` (direct `call`/`jmp rel32` graph from the linker map, walked from
`_WinMain@16`; `exe_reach.txt`). `addr_taken=yes` is the install table, not a caller.

| rva | symbol | direct chain from WinMain | default build |
|---|---|---|---|
| 0x00476b90 / 0x00476be0 | Batch476b90 / Batch476be0 | `Rfx::Create` <- `PickupGlow_DbgPost` <- `Pickups_FrameTick` <- `TrackRenderer::UpdateCar` <- RenderFrame <- WinMain | not reached: `PickupGlow_DbgPost` returns unless `On()` (`MASHED_RFX_EMITTERS=1`) and `MASHED_DBG_PUGLOW`; `Create` takes these two only for flags other than 0x817 |
| 0x00476d00 | MultiArrayScatter476d00 | `PK_GlowSubmit` <- `PK_459000` <- `Pickups_MaybeReplay` <- `TrackRenderer::Load` <- WinMain | not reached: `PK_GlowSubmit` returns unless `On()` (`PickupGlow.cpp:131`) |
| 0x004769a0, 0x00476a10, 0x00476a30, 0x00476cb0, 0x00478cc0 | | 0 direct call sites in the exe | none |
| 10 asi-only rows | | | none (exe-callers=0) |

## Gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, exe b2ec5d9f, private root
`D:/MashedWT/MATCH-S12_root` (robocopy of the MATCH-S09-REMERGE root, `A4_ROOT` / `MASHED_ROOT`), test slot outside
(`test-slot`, slot 3/4 after a 10 min memory wait), `mashed_lock.py run --solo --label MATCH-S12-gates` inside (the open
waited behind the merge train's priority claim and MATCH-S19's solo window, then ran 12:28 to 12:38). Every arm exit 0
and self-exited, one run each, no reruns. Refs read from `merge_train.json` at gate time (orch/integration 0f5899d1c,
unchanged).

| arm | file | sha8 | ref (`merge_train.json`) |
|---|---|---|---|
| H_DEF | step.csv / gates.csv / ptrace.log | 8284be46 / ba407a16 / 31fe043e | 8284be46 / ba407a16 / 31fe043e |
| B_DEF_K0, K1, K26 | step.csv | 1d5bf4f9 x3 | 1d5bf4f9 x3 |

Equal to the refs. The merge train's `-Full` pass (D2 x3, 13 tracks) was not run here; `mashedmod/src` is in
`full_set_paths`, so the train runs it.

## Re-classify (tracker-only, L-68; no C4 in this slice)

Common evidence: `match.py re/match/s12/<va8>.c` 100% under `flags=/O2 /GS /Ob1 tc=ddk`, `relocheck.py` OK, one body
(`Match/MatchS12_<va8>.cpp`), gates unchanged on this build. For the 18 swapped rows: level unchanged, `file` cell
moves to the wrapper, `notes` += `match evidence: identical, swapped, exe-callers=0 in the default build; C4 when a
caller lands (L-68 ruling)`.

Applied on this branch (re-classify transaction, `hooks.csv` 18 rows + 18 CHANGELOG lines, `tracker_lint.py --base
orch/integration` OK NEW=0): levels unchanged (16 x C3, 0x00475ef0 C2, 0x00475a60 C4). 0x00475a60 was already C4 on an
earlier route: its note and CHANGELOG line say the match route adds no level (L-68) and the C4 stays on that route.

Follow-up rows filed: `MATCH-S12-FU-RFXDEF` (map `DAT_00613288/90/94/a4` onto RfxEmitter.cpp's port statics
under `MASHED_STANDALONE`, so the 8 null-default NOT-SAFE rows can swap). Not filed, recorded here: `TrackRenderer::ClumpAnimTick`
(0x004756e0) is a C++ re-port on port state, a swap needs FUN_00426700 / FUN_004752f0 / FUN_004b47e0 ported first.

## Remerge (run 6, after the lead's GO)

The first gate pass (above, exe b2ec5d9f) was on the branch merged with orch/integration 9a0f7def5. `git merge-tree`
against 0f5899d1c conflicted in `hooks.csv` and `re/analysis/CHANGELOG.md`, so the branch merged orch/integration twice
more: 0f5899d1c (merge e526ccf65; the two trackers resolved as integration's file plus this slice's 18-row re-classify
transaction re-applied, `D:/MashedWT/MATCH-S12_data/reclass2.py`, idempotent) and b1dd84b7c (MATCH-S09-FU, S13, S15,
D4-DC-77b0-ST5-RING; merge 1e0b14534, no conflict). On the merged tree: `tracker_lint.py --base orch/integration` OK
NEW=0 (diff vs integration = the 18 rows + 18 CHANGELOG lines), rva-lint NEW=0, asm-lint OK (1317 files), each of the
18 RVAs still has one body (its wrapper). S13 now makes raw-address calls into two S12-matched RVAs that have no port body
(`MatchS13_0047c2d0_Call00478030`, `MatchS13_0047c270_Call004780c0`): no second body, and a raw-address call is not a
caller of a port body.

`match.py` + `relocheck.py` re-run on the 18 swapped sources after the merge: identical to before (17 at 100% by
instruction, 0x00475ef0 by the whole-symbol rule: 57 B symbol, 0 bad bytes, Ghidra size 49; relocheck OK, unchecked=0
on all 18).

Build (`MASHED_NO_DEPLOY=1`): Build OK, rva-lint NEW=0, asm-lint OK, .asi not deployed. exe sha256
`6b196a1b2303949f66999ca4dfe21605be88ad07b6415d59f2ea2c87a930734a`, asi
`77728530a7630e7262b089e79e0f0c93e72d06bf3a59a42aa2ec44a79704165b`. exe_reach unchanged (same three env-gated chains,
`exe_reach.txt` is from this build).

Gates on the merged exe: same command and root, test slot outside, `mashed_lock.py run --solo --label MATCH-S12-gates2`
inside, 14:22 to 14:36, refs re-read from orch/integration's `merge_train.json` at gate time (unchanged). Every arm
exit 0 and self-exited, one run each.

| arm | file | sha8 | ref |
|---|---|---|---|
| H_DEF | step.csv / gates.csv / ptrace.log | 8284be46 / ba407a16 / 31fe043e | 8284be46 / ba407a16 / 31fe043e |
| B_DEF_K0, K1, K26 | step.csv | 1d5bf4f9 x3 | 1d5bf4f9 x3 |

Raw (`D:/MashedWT/MATCH-S12_data`): `arms2/` files carry the same sha256 as the `arms/` rows below (byte-identical
outputs); `build2.log` ce960e90e18b01c855047e7fcf8f3fae92dbcb4cbdccb8568760570034152ab0; `gates2.log`
e52b33f83e884b755a816ca11ec5443cf2d503d5f3147072e882a0e550f95f6a.

Per the conductor (via the lead): not enqueued; the conductor gates a combined branch (integration + S12 + S14 + S16).

## Build (first pass)

`set MASHED_NO_DEPLOY=1&& mashedmod\build.bat`, Build OK, rva-lint NEW=0, asm-lint OK, .asi not deployed
(`original\mashed_re_dev.asi` still dated 2026-10-24). exe sha256
`b2ec5d9fa15051b0942486591e70cee9e9ad38809a2e7535a3524b83bd5cfa63`, asi
`a92fd62afa9e714ee6785eb3f606cf179dae09b495ecf8663ce8e1da57dae128`.

## Raw files (not committed; `D:/MashedWT/MATCH-S12_data`)

| file | sha256 |
|---|---|
| decomp.json (decomp_pc.py --port --callees, 129 entries) | ea165ba5e8349fd6b12dc6442093daa2fa58c7275977c18db53fed7e1756ad06 |
| g2s/results.tsv (raw converter scores) | b08e1237ce4c4c73d6efe375046e6e72d58d7bd227f83208681f019486b40202 |
| slice_rows.tsv | 0af3aa2e134542b6fdb9a5117d519ec6b971a34ade103b50af4c79ee09c86f6a |
| raw100.txt | 0131f847b0a458c51b4c0416aae393d347b47ed9875d17141f796e5c91ad1314 |
| build1.log (Build OK) | 8dc00caf9e7db22682605ac15c91578d24f163fa9ee8e42a23f4953442109fda |
| permute/00475210_0x00475210.log | 092881847cc9b773c76cd494a60068a536fc013829efb24eb4a49f77a0fc00c3 |
| permute/00475a00_0x00475a00.log | c9d6d9459d92c375118db70cd589317584d8e0bdce30367e629da3088c36acb1 |
| permute/00476320_0x00476320.log | 674bbc8b4e42a9659c4d2d54f41adf308e36ee22caa223221a50f2a2ddb30712 |
| permute/004770a0_0x004770a0.log | e9d9a1426a7a1ba932291bf9c72a12b1ddc9cc312d83c1e0ec56b28eb6a5c472 |
| permute/004778e0_0x00477920.log | 29b6b54148fec70f9aa8c0a5c46e400247f448e3e6acb0f4f4511e872db509b3 |
| permute/004783f0_0x004783f0.log | 956f132b67a4f782a46fc922691ea97727ffd515c6c786963c77dad60a4ba5ff |
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0.step.csv, B_DEF_K1.step.csv, B_DEF_K26.step.csv (identical) | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| gates.log (slot + lock + run_arms output) | 327b7ca052c1f4a71ef7d4622a740ee85826c9ab7b4509eb8c115be7957fe274 |
| g2s/, notes/h1..h9.md, scratch/ | raw converter output, helper notes, scratch |
