# RESULT MATCH-S10, 2026-11-02

**Verdict: BLOCKED** (static and port legs done and built; gates not run: the display returned
D3DERR_NOTAVAILABLE for the whole 60 min poll window). Slice 0x0045b9d0..0x00467070 (128 game functions, 0 library rows inside). **110/128** sources are
byte-identical under `re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178, `/O2 /GS /Ob1`, C, x87), and
every one of the 110 has `relocheck.py` OK in its best source file (`relocheck.tsv`; the 14 BAD lines there all belong
to functions below 100%, where the instruction streams do not line up, so the offsets compare the wrong operands).
**22** of them are now the port's only body (22 wrapper TUs `Match/MatchS10_<va8>.cpp`): 3 in both targets, 19 in the
.asi only. Build OK at cf19b66b2 (`MASHED_NO_DEPLOY=1`). `lint_rva_bodies.py`: NEW=0.
**C4 candidate: 1 row**, 0x0045c330 `PredNot45c330`, with a default-path exe caller chain from `WinMain` (below). The
other two exe swaps (0x0045ba00, 0x0045db50) have no exe caller, and the 19 asi-only swaps have none (L-68).
Gates: **not run** (display D3DERR_NOTAVAILABLE, hr=0x8876086A, 25 probes 11:16..12:19, `display_poll.txt`). No C4 is
claimed: the 0x0045c330 promotion waits for the gates.

Branch: cut from orch/integration bf8abf3d0 (MATCH-S01..S08 merged there; MATCH-S09 is not, review on orch/MATCH-S09).
The S10 blocks in `exe_sources.rsp` / `asi_sources.rsp` sit after `"Util\FloatSliderStep.cpp"` and the `x87_tus.txt`
block after `RwpSolverCore14`, away from S09's anchors (`Gameplay\PickupPoolSpawn.cpp`, `WarzonePontoon`).
**Merge with S09:** `git merge-tree HEAD orch/MATCH-S09` auto-merges the rsp files, x87_tus, PromoLoop_round37/47 and
PromoLoop_sessionB, and gives **one conflict, `Gameplay/PickupPredicates.cpp`**: both branches moved a body out of it
(S09 0x004576b0 AnyActive4576b0, S10 0x0045c330 PredNot45c330). Resolution: keep S09's `// MATCH-S09:` note line pair,
drop the old AnyActive4576b0 body and the `s_fn_45bff0` / `PK_SlotEmpty` block (S10 removed it, its only user was the
old PredNot45c330 body), keep S10's `// MATCH-S10:` note. Whichever lands second takes that resolution.

## Match (static leg)

| | count |
|---|---|
| slice functions | 128 (`slice_rvas.txt`, from hooks.csv by `tools/mkslice.py`, the cut_slices.py rule) |
| raw converter (g2s) at 100% | 26 |
| hand-matched to 100% (helpers h1-h9, `helper_notes.txt`, `helper_assign.txt`) | 84 |
| of the 110, byte-identical by the whole-symbol rule (`by` = `bytes`) | 11 (0x0045bae0, 0x0045bf30, 0x0045ca30, 0x0045cbe0, 0x0045d330, 0x00461650, 0x004625b0, 0x00463f40, 0x00465940, 0x00465a30, 0x00466a50) |
| below 100% | 18 (table below; budget spent) |

`re/console/pc_funcs.csv` has all 128 functions (no tool-data fix needed). Scores are my re-run of `collect.py` over
`re/match/s10/*.c` (`results.tsv`), not the helpers' own numbers.

| rva | size | best % | regs % | class | residual (helper notes) |
|---|---|---|---|---|---|
| 0x0045c030 | 211 | 98.6 | 98.6 | a | one discarded x87 result popped before the next lea in the original, after it in ours; permuter 185 + 4 manual forms |
| 0x0045c480 | 83 | 55.0 | 70.0 | b | original uses a second zero register for 0x0088f0a0..ac and keeps the stores before rep stosd, ours sinks them; permuter 224 |
| 0x0045c550 | 232 | 37.3 | 74.7 | b | chain loop: original reloads table[id].next at the top and tests a separate load at the bottom; 7 loop forms, permuter 269 |
| 0x0045cd30 | 563 | 62.8 | 78.1 | b | p=&DAT_0088f680[i] not kept in EDX through the slot loop, loop IV set / frame size differ (permuter 512 compiles) |
| 0x0045d460 | 822 | 94.2 | 97.7 | b | only the voice-count sum loop differs (accumulator edx stored as the initial 0, register assignment) |
| 0x0045d7a0 | 702 | 52.6 | 68.4 | b | flag updates are memory RMW per helper in the original (9176 carries them), cross-product addressing, DAT_0068f620 reg reuse |
| 0x0045dd60 | 272 | 78.7 | 78.7 | b | one x87 operand order (gain loaded first, fmulp); the shift moves later jump targets |
| 0x0045de80 | 236 | 52.9 | 69.0 | b | memset fill = address of "" 0x005cc4de (9176 folds it to mov cl,imm8); shared zero in ebp / spill allocation |
| 0x0045e990 | 1610 | 99.5 | 99.5 | a | pilot body in S10 form, 1 of 2 pilot spots fixed; 2 integer ops scheduled into the fsub->fstp window |
| 0x0045efe0 | 1540 | 89.8 | 98.3 | a | x87 CSE of 22050-sin*500 kept in st(0) in the original, spilled or reloaded by 9176; one spot cascades regs/offsets |
| 0x00461e90 | 1250 | 95.8 | 95.8 | b | nested switch; 0x24 needs a hidden default-equal case; 0x21/0x1a tail-merge target differs |
| 0x004627f0 | 351 | 96.8 | 96.8 | b | first two `|=` on +0x70 fold into one `or 0x50` (bitfields / volatile / inline setters / permuter tried) |
| 0x00462950 | 1122 | 88.8 | 94.7 | b | eax/ecx flip around DAT_0088e670 (count-down sum loop setup) |
| 0x00463640 | 1521 | 94.3 | 94.9 | b | case-6 block kept apart from 2/8/9 only by swapping its two stores; state/gear register choice at 0x00463a1f |
| 0x004647f0 | 186 | 83.3 | 83.3 | b | channel loop: original loads the record field before the global and leaves the loop unaligned |
| 0x00464a50 | 584 | 98.3 | 98.3 | b | fstp of the length result scheduled before the deferred arg pop in the original |
| 0x00464ca0 | 357 | 92.7 | 92.7 | b | sin expression kept on the x87 stack across the handle test in the original, recomputed in ours |
| 0x004669b0 | 160 | 84.4 | 90.6 | b | zero stores are inlined memsets; one shared zero register and eax=1 for DAT_006904e8 not reproduced |

Classes: (a) compiler-version codegen, (b) source shape not found in budget, (c) tool limit.

Held static callees (pilot finding 1, `holds` lines in `helper_notes.txt`): 0045bae0.c holds 0x0045ba10, 0x0045bac0,
0x0045bf30, 0x0045bba0; 0045bfa0.c holds 0x0045baa0; 0045c6b0.c holds 0x0045c640; 0045cb20.c holds 0x0045c4e0,
0x0045c510; 0045cbe0.c holds 0x0045c4e0; 0045cd30.c holds 0x0045c820. None of these files is swapped in this branch.

## Swap (port leg)

Helper reports: `swap_w1.txt` (exe leaves), `swap_w2.txt` (`Util/PromoLoop_sessionB.cpp`), `swap_w3.txt` (other
asi-only promotion TUs). Body maps before/after: `bodies_pre.tsv`, `bodies_post.tsv`. Of the 128 slice RVAs only 27
had a port body at all (9 of them in the exe); 29 of the 110 matched rows had one.

| targets | rows |
|---|---|
| exe + asi | 3: 0x0045ba00 (RaceResultIndexedStore), 0x0045c330 (PredNot45c330), 0x0045db50 (FloatSliderStep) |
| asi only | 19: 0x0045bff0, 0x0045c4e0, 0x0045c510, 0x0045c7f0, 0x0045c810, 0x0045cab0, 0x0045df70, 0x00462760 (PromoLoop_sessionB); 0x0045bfe0, 0x0045c640, 0x0045c850, 0x0045c860, 0x0045caf0, 0x0045dd50, 0x00462500, 0x00462510, 0x004627b0 (PromoLoop_round*); 0x0045d3f0, 0x0045dfc0 (Lane2) |

- x87 TUs added to `mashedmod/x87_tus.txt` (3, block after `RwpSolverCore14`): MatchS10_0045db50, _0045df70, _0045dfc0.
- `re/tools/dual_copy_allowlist.txt` unchanged. `py -3.12 scripts/lint_rva_bodies.py` ->
  `2 finding(s): CROSS-TARGET=1, DUP-IN-TARGET=1 | allowlisted=2 NEW=0`.
- 0x0045c330: the exe binds `FUN_0045bff0` to `PK_SlotEmpty` (Gameplay/PickupTick.cpp, the callee the old exe body
  reached through `s_fn_45bff0`); the .asi binds it to a raw 0x0045bff0 forwarder (the old .asi body made that same
  call; the installed JMP reaches `Pred45bff0`). The dead `s_fn_45bff0` block in PickupPredicates.cpp went with it.
- 0x0045db50: the 0.2f / 0.005f literals have the bits of the .rdata the old body read (0x005cc9c0, 0x005cd0ec). Clamp,
  NaN, -0.0 and out-of-range dir behave the same in both bodies.
- 0x0045c640: the old naked body touched only EAX because the unhooked original caller 0x0045c6b0 keeps its loop index
  in EDX across the call. The VS2022 /O2 listing of the wrapper uses EAX+ECX only (0 EDX uses), and all 5 original
  callers rewrite ECX before reading it. The contract is written in the wrapper header; re-check it if that TU's flags
  change.
- 0x0045c7f0: the old naked body returned int, the matched one is void; both original callers (0x0045d356, 0x0045d38c)
  tail-jump to 0x0045c6b0, which overwrites EAX at 0x0045c6d1 before reading it.
- 0x0045df70 / 0x0045dfc0: the old bodies read 0.025f (0x005cc9a4) and 1.0f (0x005cc320) from .rdata at run time; the
  matched sources use the same literals.
- 0x0045d3f0 / 0x0045dfc0 (Lane2, asi-only): the opt-in `MASHED_SHADOW_AB` shadow wrapper is dropped with the old body.
  `L2_FUN_0045d3f0` keeps its symbol (MatchS06_00429290.cpp binds `FUN_0045d3f0` to it in the .asi branch).

**NOT-SAFE (7), not swapped:**

| rva | old body | reason |
|---|---|---|
| 0x0045baa0 | Powerup/PowerupTypeSearch.cpp PowerupTypeSearch + sessionB Search45baa0_RegAbi | the exe caller PowerupSystem::Lookup (PowerupSystem.cpp:98) passes the port's own C++ table; the matched body hard-codes 0x005f9998 / [0x005f9bd8] / 0x40 |
| 0x0045bac0 | Powerup/PowerupSystem.cpp PowerupSystem::Deactivate | C++ member on port-owned slot state; 0045bae0.c also calls 12 functions with no port body, and 0x0045bba0 is the port's PowerupSystem::Tick |
| 0x0045c110 | Powerup/PowerupContact.cpp SurfaceGate | takes a port WorldHit*, has an injector branch and a CSV log; the 20 scripted-gate cases always return 0 there |
| 0x0045c350 | Powerup/PowerupContact.cpp SweepConfirm | stand-in (slot, WorldHit*, retAddr): never walks the material table, never calls 0x0045c110 / 0x00426c00 / 0x0044d5e0 / 0x004c51a0 / 0x00452140 |
| 0x004625b0 | Audio/AudioCharacterBankPaths.cpp | names from the port's kBankName table (not 0x006041f0), snprintf capped at 0x80; never calls 0x004a2b60 (no port body) |
| 0x0045ca30 | sessionB FloatClamp3Level45ca30 | old body walks 6 floats, the original and the matched source walk 36 (0x0088f5e0..0x0088f670); NaN differs. A swap moves the asi toward the original: lead decision, not done here |
| 0x0045dbe0 | (SKIP) Util/FloatSlider2SelfTest.cpp | A/B harness, not a body: Slider2_Entry always runs the original through a trampoline |

**88 byte-identical rows with no port body**, not swapped (a new body with no caller is not C4, L-68; they wait for a
caller or a top-down row). List: `results.tsv` rows with `best%` = 100 and `exe_body` = `-`, minus the 22 swapped.

## Exe callers (L-68, L-89)

Binary witness on this build: `py -3.12 tools/exe_reach.py mashed_re.exe mashed_re.map <symbols>` (direct
`call`/`jmp rel32` graph from the linker map, walked from `_WinMain@16`; `exe_reach.txt`).

| rva | symbol | exe callers |
|---|---|---|
| 0x0045c330 | PredNot45c330 | 1 direct site, PK_458920 (Gameplay/PickupCollect_naked.inc:53). PK_458920 is called by `Gameplay::Pickups_Callback` (PickupTick.cpp:336), which `RG_CbDispatch` (Race/WorldRegistry.cpp:80) runs for target 0x00458920. Default path: `_WinMain@16` -> RenderFrame -> `TrackRenderer::UpdateCar` -> `Race::Registry_FrameTick` (TrackRenderer.cpp:6421 racing, :344 `PreGoRegistryTick`, both unconditional) -> RG_485070 -> RG_484de0 -> RG_CbDispatch -> Pickups_Callback -> PK_458920 -> PredNot45c330. exe_reach's shortest chain goes through the env-gated `PickupCb_MaybeReplay` (`MASHED_PKCB_REPLAY`); under L-89 that one does not count, but the per-frame registry path above does. |
| 0x0045ba00 | RaceResultIndexedStore | 0 direct sites (address taken only by its RH install record): exe-callers=0 |
| 0x0045db50 | FloatSliderStep | 0 direct sites (same): exe-callers=0 |
| 19 asi-only rows | | none (exe-callers=0) |

A direct chain shows the exe calls the body. It does not show the gate scenario executes it (no per-function trace
was taken): the call happens when a car overlaps a pickup in the world registry.

## Gates

**Not run.** The display has been unavailable since 09:19 (other rows hit the same wall). Probe: one `B_DEF_K0` arm
(`verify/savelink_20261023/run_arms.py`, 1200 frames) under `mashed_lock.py run` + test slot, private root
`D:/MashedWT/MATCH-S10_root` (`MASHED_ROOT`), exe `5d77845c` (and `906f1896` before the swaps). Every probe exited in
~8 s with all three `InitD3D9: CreateDevice(...) -> fail hr=0x8876086A` lines (HAL/HW-imm, HAL/SW, REF/SW) and no
step csv. Polled every ~4.5 min from 11:16 to 12:19 after the built source was committed: 25 probes, 25 failures
(`display_poll.txt`). `mashed_lock.py status` was free before each window.

To run when the display is back (from this worktree; `D:/MashedWT/MATCH-S10_data/gates.sh` is the S09 recipe with S10
paths, not yet written):
`A4_OUT=D:/MashedWT/MATCH-S10_data/arms A4_ROOT=D:/MashedWT/MATCH-S10_root MASHED_ROOT=D:/MashedWT/MATCH-S10_root
py -3.12 ../orch-integration/re/orchestrator/mashed_lock.py run --solo --label MATCH-S10 -- py -3.12
C:/Users/maria/Desktop/Proyectos/.claude/scripts/testslot/proyectos_test_slot.py py -3.12
verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, then sha256 of H_DEF.step/gates/ptrace and
the three B step csvs against `re/orchestrator/merge_train.json` (`gctrl.ref` / `b.ref`), then the `-Full` pass.

## C4 re-classify text (apply after the merge, through re-classify; not applied here)

Apply only after the gates come back unchanged on this build (and after the merge), through re-classify.

| rva | name | level | match.py | relocheck | old body (removed) |
|---|---|---|---|---|---|
| 0x0045c330 | PredNot45c330 | C3 -> C4 | 100% (8/8 ins) | OK, unchecked=0 | Gameplay/PickupPredicates.cpp |

Evidence: `match.py re/match/s10/0045c330.c` 100% under `flags=/O2 /GS /Ob1 tc=ddk`, `relocheck.py` OK, one body
(`Match/MatchS10_0045c330.cpp`, both targets), exe caller chain from `WinMain` through the per-frame
`Registry_FrameTick` (table above), gates unchanged on this build (pending). The `file` cell moves to the wrapper.
CHANGELOG line: `2026-11-0x  0x0045c330  PredNot45c330  C3->C4  route match (c4=match), exe-callers via
Registry_FrameTick -> RG_CbDispatch -> Pickups_Callback -> PK_458920; evidence: verify/matchs10_20261102/RESULT_MATCH-S10.md`.

The other 21 swaps stay at their level, `notes` += `match evidence: identical, swapped, exe-callers=0; C4 when a
caller lands (L-68 ruling)`: 0x0045ba00, 0x0045db50 (both targets, no exe caller) and the 19 asi-only rows.

## Build

`set MASHED_NO_DEPLOY=1&& mashedmod\build.bat`, Build OK, .asi not deployed (`original\mashed_re_dev.asi` still dated
2026-10-24). exe sha256 `5d77845c201c03c862d6c00fdea822bf5ed64b76d26417536719019161d738f9`, asi
`89199f442baac0bb05177e9ec3f662eab7ceabf9590e29a39b9987141868c94b`.

## Raw files (not committed; `D:/MashedWT/MATCH-S10_data`)

| file | sha256 |
|---|---|
| decomp.json (decomp_pc.py --port --callees, 128 functions) | aba275b43311710aa5ac85a63fe526c0195e14f2de9fb821e6382603b9f7f176 |
| g2s/results.tsv (raw converter scores) | 04cb3ba943fb56bc9fe3fe338a416baee91b01e02eeb2e1bc392e313d10fb5d5 |
| build0.log (pre-swap, Build OK) | b310dcd2bb825a32bfdcc04174093856694e87a319e2999dfb5e85bf5596c4a5 |
| build1.log (post-swap, Build OK) | 6da7930cb9a85ab2202d5d5176f74798ae27b8bf2267acd1b65bac2f261f4da8 |
| poll.log (= committed display_poll.txt) | ccc996bafaacdd8227b6ba1f4199a23e80ace0ae453d9fead79edd2f6e0c3cfc |
| probe/B_DEF_K0.log (last probe, CreateDevice fails) | 5b9a6e8bac9faf78f7df22d7debbf353694792697a1dd95c08615774ab02ffc5 |
| g2s/, notes/h1..h9.md, scratch/, bodies.tsv | raw converter output, helper reports |
