# Dual-copy audit — verified copy (.asi) vs shipped copy (.exe)

**Date:** 2026-09-29
**HEAD audited:** `64095a44` … `235e964a` (`git diff --stat 64095a44..235e964a -- mashedmod/` is
**empty**, so every source statement below holds for both; another session was committing to
`re/` in the same tree during the audit).

> **Working-tree churn, 10:15–10:17.** Another session rewrote files under `mashedmod/src/` twice
> during the comparison pass (`PowerupSystem.cpp` 15,355 B → 6,625 B → 15,355 B;
> `TrackRenderer.cpp`, `AiStandalone.cpp`, `Integrate2.cpp` 588 → 695 lines, `VehicleControl.cpp`
> likewise). The tree settled at **10:17:28** and `git status -- mashedmod/` is **clean** at
> `235e964a`, i.e. the on-disk content now equals HEAD. Every line number below was taken from, or
> re-checked against, the settled content. Two artefacts of the churn are recorded because they
> could mislead a reader of the raw agent output: `TrackRenderer.cpp` line numbers shifted by ~150,
> and `aib_ai_target_enable()` was momentarily `return 1;` — it is **`return 0;` at
> `D3d9Render/TrackRenderer.cpp:93`** in the settled tree. **Re-grep by function name before acting
> on any line number here.**
**Scope:** READ-ONLY. No source edited, nothing built, nothing run, no tracker mutated.
**Artifacts:** this note + `re/analysis/dual_copy_audit_2026-09-29.csv` (the machine-readable table).

---

## 1. Why this audit exists

Twice in the week of 2026-09-26 a `hooks.csv` row at C3/C4 said *verified* while the shipping
`mashed_re.exe` ran a **different, wrong body** of the same original function:

- `0x00467650` (A6a) — the exe copy `Vehicle/Integrate2.cpp` was missing the `+0xbf8` start-boost
  block `0x00467d3a..0x00467e44` that the .asi copy `Vehicle/PhysicsChainHooks.cpp` always had.
  That single gap was the whole of U-9140's 8–9× drive-force error and of D3 criterion (e)'s
  −46…−90 % failure (`re/analysis/D3_DRIVE_FORCE_2026-09-29.md`).
- `0x00470670` (A4) — the exe copy passed a hardcoded `0` for `param_1` (the car index).

Three more (`0x00415e20`, `0x00443080`, `0x00416250`) were found the same way by the D3 AI port
(`re/analysis/D3_AI_PORT_2026-09-26.md`).

All five share one mechanism, stated here once:

> `mashed_re.exe` and `mashed_re_dev.asi` compile **different source lists**. Frida C3/C4 evidence
> only ever measures the `.asi`. `hooks.csv` has **one** `file` column, which names the `.asi` copy.
> The exe copy is therefore invisible to the tracker, and nothing in the build fails when the two
> bodies disagree.

Two mechanical facts make it worse:

1. **In the exe, `RH_ScopedInstall` is a no-op.** `Stubs/HookSystemNoOp.cpp:19` defines
   `HookSystem::Register` as an empty function ("No-op: the standalone exe is the implementation;
   there is nothing to patch"). So a shared TU's ported body is linked into the exe but **reached
   only if some exe-side caller calls it by C++ symbol**. Registration proves nothing about the exe.
2. **The two targets do not even compile the same shared TU identically.** The exe adds
   `/DMASHED_STANDALONE` and `/EHa`; the .asi uses `/EHsc` (`mashedmod/build.bat:198` vs `:226`).

The build already acknowledges this class of hazard once, for the float model
(`mashedmod/x87_tus.txt`, echoed at `mashedmod/build.bat:191-193`): *"compiling them SSE2 here while
the .asi compiles them x87 would make the two targets compute physics differently — an A/B verified
under the .asi would not transfer to the standalone."* That is exactly the argument of this note,
applied to source bodies instead of codegen flags.

---

## 2. Method

1. **TU sets.** Parsed `mashedmod/exe_sources.rsp` and `mashedmod/asi_sources.rsp` (one quoted
   relative path per line) into three sets. Added the four TUs `build.bat` compiles outside the
   rsp lists (`LibRw/RwBridge.cpp`, `RwRasterBridge.cpp`, `RwSceneBuild.cpp`, `RwRaceSubmit.cpp`,
   `build.bat:81-96`) as exe-only; verified they contain **zero** RVA-anchored ports
   (`grep -c '0x00[0-9a-f]{6}'` → 0/0/0/1, and no `^// 0x00` definition comments).
2. **Body detection, channel A.** For every TU in either list, matched comment lines whose first
   token after `//` is an RVA (`// 0x004xxxxx …`, bullet/box-drawing prefixes allowed) and bound
   each to the next following function *definition* (a signature that reaches an opening brace,
   skipping intervening comments; `if/for/while/switch/return/#…` rejected). RVAs ≥ `0x005d0000`
   were dropped as data (not `.text`), which removes extern-global declarations that sit under an
   RVA comment.
3. **Body detection, channel B.** For every C3/C4 `hooks.csv` row whose `file` is an **asi-only**
   TU, checked whether the RVA is mentioned anywhere inside an **exe-only** TU (or inside
   `Ai/AiStandalone.cpp`, which is nominally shared but *is* the exe's AI). This channel is what
   catches the A5/A6b/A3 class, where the exe-side body is a named method
   (`VehicleInit`, `ForceIntegrator`) that never carries the RVA in a definition comment.
4. **Linkage.** Each detected body classified `external` / `static` / `anon-ns` / `extern` by
   reading the definition line and scanning backwards for an anonymous namespace.
5. **Install channel.** Collected every `RH_ScopedInstall(Name, 0xRVA)`, **excluding commented-out
   lines** (a plain regex over-counts by 62 RVAs; `Frontend/GameModeCarSelect.cpp:158` is one such
   dead line, and the *"MASS-DISABLED C3 bodies are unverified"* precedent is exactly this trap).
6. **Join.** Left-joined `hooks.csv` (`rva, name, subsystem, confidence, status, file`) onto both
   channels; `file` normalised by stripping any `mashedmod/src/mashed_re/` prefix and lower-casing.
7. **Body comparison.** The 40 highest-risk pairs were read in full and classified
   IDENTICAL / DIFFERS-COSMETIC / DIFFERS-BEHAVIOUR / NOT-A-PAIR, with differing lines quoted at
   `file:line`. Pairs outside that set are listed in the CSV as **UNREVIEWED** — they are candidates
   from the structural channels, not verdicts.

**Limits of the method, stated plainly.** Channel A cannot see a port whose author did not write an
RVA comment immediately above the definition; channel B cannot tell "the exe has a second body" from
"the exe merely mentions the RVA in prose". Neither channel proves *which* body the exe's call graph
reaches — that needs reading the callers, which was done only for the pairs marked with a verdict.
So the counts below are **a floor, not a census**.

---

## 3. TU counts

| set | count |
|---|---|
| `.cpp` on disk under `mashedmod/src/mashed_re/` | 495 |
| in `exe_sources.rsp` | 215 |
| in `asi_sources.rsp` | 418 |
| **shared** (both lists) | **164** |
| **exe-only** | **51** (+4 `LibRw/` compiled directly by `build.bat` = 55) |
| **asi-only** | **254** |
| in neither list (orphan source) | 26 |

Of the 26 orphans, 4 are the `LibRw/` bridges and `Collision/QhullBridge.cpp` (all compiled
explicitly by `build.bat`), 9 are `tests/` and `*_selftest.cpp`, and the rest are genuinely dead in
both targets: `Boot/CrtEnvArgv.cpp`, `Compat/PizOpenBypass.cpp`,
`Frontend/HudFrontendDispatchers_t4.cpp`, `Lane2/L2_00421960.cpp`, `Lane2/L2_004219c0.cpp`,
`Lane2/L2_00495fe0.cpp`, and four stray `MixedC3Sweep.cpp` copies (`Frontend/`, `Input/`, `Render/`,
`Util/` — only `Audio/MixedC3Sweep.cpp` is in a list).

---

## 4. The structural finding: most C3/C4 evidence does not cover the exe

`hooks.csv` holds **1029 C3 + 184 C4 = 1213** rows. Classifying each by where its `file` column
lands:

| where the C3/C4 row's `file` lands | rows |
|---|---|
| an **asi-only** TU — **not linked into `mashed_re.exe` at all** | **632** |
| a `re/analysis/…md` note, not a source file (tracker cannot answer the question) | 304 |
| a TU the exe **does** compile | 264 |
| a path that **does not exist on disk** (stale tracker pointer) | 11 |
| a TU in neither rsp list (dead source) | 2 |

So **264 of 1213 C3/C4 rows (22 %)** name a file the shipping exe actually compiles.

That 632 is not 632 defects. For **531** of them no exe-linked TU mentions the RVA at all — the
standalone simply does not implement that original function (most of the render/audio/menu
internals), so the honest statement is *"the evidence does not cover the default build"*, not
*"the exe is wrong"*. The dangerous residue is the **30** rows where an **exe-only** TU does touch
the RVA (§5) plus the **9** AI rows `Ai/AiStandalone.cpp` touches (§6).

**Stale pointers (11 rows)** — all a `Util/` vs `Gameplay/`,`Vehicle/` directory drift in the
tracker, the files exist under the other directory:
`0x0042b8d0` C4, `0x004298c0` C3, `0x0046cbb0` C4, `0x00407640` C3, `0x004077e0` C3, `0x00407a20` C4,
`0x004098a0` C4, `0x0040b970` C3, `0x0040b9a0` C4, `0x0040ba00` C3, `0x0040ba60` C4.

**Dead-source pointers (2 rows)** — `0x004abc53` and `0x004abf28` (both C3, boot) point at
`Boot/CrtEnvArgv.cpp`, which is in **neither** build.

---

## 5. Dual-copy RVAs

The two structural channels found **104 RVAs** with a body in two places under the two build
targets: 27 at C4, 73 at C3, 4 at C2. By subsystem: frontend 31, vehicle 14, gameplay 13, ai 13,
render 11, hud 6, util 5, audio 4, save 3, input 2, powerups 1, crt 1. The body-comparison pass
added **3** more that neither channel caught (`0x00416250`, `0x00418560`, `0x0040eee0` — the exe
side is a named standalone method with no RVA-anchored definition comment), giving **107 rows**.

Full table: `re/analysis/dual_copy_audit_2026-09-29.csv`
(`rva, name, subsystem, confidence, status, hooks_file, exe_side_copy, asi_side_copy, channel,
verdict, evidence`).

**54 of the 107 RVAs were read in full on both sides and given a verdict.** The remaining **53**
are **UNREVIEWED** structural candidates — listed in the CSV so the next pass has a worklist, not
scored.

| verdict | count |
|---|---|
| **DIFFERS-BEHAVIOUR** | **26** |
| NOT-A-PAIR (trampoline / extern pointer / no body / complementary fragments) | 14 |
| IDENTICAL | 11 |
| DIFFERS-COSMETIC | 3 |

### 5.1 DIFFERS-BEHAVIOUR — physics / vehicle (default race path)

| RVA | C | exe copy | .asi copy (the evidence) | what differs |
|---|---|---|---|---|
| `0x00468980` A6b | **C4** | `Vehicle/AeroStabilize.cpp:66` | `Vehicle/PhysicsChainHooks.cpp:2445` | **The whole rotation-apply is dead in the exe.** Both applications are guarded `if (orient)` (`AeroStabilize.cpp:72`, `:91`) and the **only** call site in the tree passes `nullptr`: `Vehicle/VehicleControl.cpp:195: Vehicle_AeroStabilize(self, nullptr, dt);`. So the state-0 branch computes `dts` and discards it, and the state-≠0 branch only zeroes `+0x9bc/0x9c0/0x9c4`. The .asi preconcats both rotations unconditionally. Airborne auto-level (pitch + roll) and the velocity-align rotation never execute in the shipping build. |
| `0x0046b540` A3 | **C4** | `Vehicle/VehicleInit.cpp:52` | `Vehicle/PhysicsChainHooks.cpp:2880` | **Output stride is 4× smaller in the exe** for loops 3/4/5. exe: `char* o = rec + 0x4bc; … o += 0x10;` (`VehicleInit.cpp:130-131`, and `:138-139`, `:148-149`) = **16 bytes**. asi: `float* p8 = (float*)(0x00881a5c + o); … p8 += 0x10;` (`PhysicsChainHooks.cpp:3003`, `:3011`) = **64 bytes**, and its own comment reads "stride 0x10 **floats**". Bases and source strides agree; only the destination stride differs, so three suspension attach-distance tables land at different record offsets. Also: the handling-override table is a 1-entry stub with an open marker (`VehicleInit.cpp:36-42`, `[UNCERTAIN U-A3-TABLE]`) against the asi's live walk of `0x00613148` (`:2891-2904`); seven record fields are hardcoded literals the asi copies from live globals (`:78-85` vs `:2929-2936`); `+0x170` is written as the decimal `0.397094f` (`:94`) where its mirror at `:95` uses the exact bits `0x3ecb4fe8`; and the four handling globals `0x00613108/14/30/3c` are never written. |
| `0x0046ddb0` A5 | **C4** | `Vehicle/ForceIntegrator.cpp:31` | `Vehicle/PhysicsChainHooks.cpp:586` | Four `.rdata` constants are decimal approximations in `ForceIntegrator.h` where the asi bit-pins the same address: `kDt` `:31` `3.33268e-4f` vs `Cf(0x39aec33e)` ≈ 3.3334e-4 (scales the whole drive-drag term at `ForceIntegrator.cpp:90`); `kSteerOut` `:45` `0.00333f` vs `Cf(0x3b5a740e)` = 1/300 (~0.1 % low on all four steer-scratch writes); `kThird` `:34` `0.333333f` vs `Cf(0x3eaaaaab)`; `kDraftDot` `:32` `0.989999f` vs `Cf(0x3f7d70a4)` = 0.99. Plus an added `std::memset(m, 0, sizeof m)` at `:73` the asi lacks. **Separately `[UNCERTAIN]`:** the `TriangleFaceNormal` operand order at `ForceIntegrator.cpp:211-212` may be swapped relative to the asi's register setup at `PhysicsChainHooks.cpp:237/247-249`, which would sign-flip the steer-feedback normal every grounded frame — but `Collision/CarWorldContacts.cpp:46` and `PhysicsChainHooks.cpp:235` state **different ABIs for the same callee**, so this is not established. Resolving it needs the disassembly at `0x0046e6f3..0x0046e728`. |
| `0x00467650` A6a | **C4** | `Vehicle/Integrate2.cpp:121` | `Vehicle/PhysicsChainHooks.cpp:1863` | **The 2026-09-29 fix landed and holds**: the `+0xbf8` start-boost block is present at `Integrate2.cpp:255-354` and matches `PhysicsChainHooks.cpp:1952-1979` including the 5e6/8e6 selector, the `Ri(v,0xbf4)==0` early clear and the state-2 zeroing. Two residual gaps: the **gear-index clamp bound is 6 in the exe and 5 in the asi** (`Integrate2.cpp:131` declares `local_54[6]` but `:140` only writes `[0..4]`, so `:155` reads a never-written `0.0f` at `gear == 5` where the asi falls back to `local_54[0]` — `local_cc` is the gear speed cap on the whole drive force); and the `+0x478` gear-column read is clamped in the exe (`:196`) and unclamped in the asi (`:1902`). |
| `0x00470670` A4 | **C4** | `Vehicle/VehicleControl.cpp:93` | `Vehicle/PhysicsChainHooks.cpp:274`,`:517` | **The car-index fix landed** (`:93` takes `int car`, `:194` forwards it, both call sites pass the real slot at `VehiclePhysicsRun.cpp:768`,`:849`). The one remaining difference is the `nullptr` A6b argument at `:195` — see `0x00468980` above. Body math is otherwise term-for-term identical. |

### 5.2 DIFFERS-BEHAVIOUR — AI (D3's active phase)

`Ai/AiStandalone.cpp` is the exe's AI; `Ai/AiControlStep.cpp`, `AiTargeting.cpp`, `AiController.cpp`
and `Util/PromoLoop_round26.cpp` are asi-only.

| RVA | C | exe copy | .asi copy | what differs |
|---|---|---|---|---|
| `0x004177b0` | C3 | `Race/RuleEngine.cpp:20` **and** `Ai/AiStandalone.cpp:1457` | `Ai/AiPreTick.cpp:171` | **Two exe copies, one fed from nothing.** `RuleEngine::UpdateFinishOrder` is the finish-order fragment only. `AiStandalone::AiPreTickRubberBand` (called every frame at `:1705`) is a fuller port, but it reads `I32(0x008a9648 + v*0x30c)` (`:1466`) and `F32(0x008a96ec + v*0x30c)` (`:1468`), and the only writer of `0x008a9648` anywhere is `Util/UtilLeaves_ac.cpp:53` `UtilTableInit8a9620`, whose install is a no-op in the exe — every other site is a read (`SmallLeaves_t1.cpp:46`, `MenuLeaves_af4.cpp:54`, `RangeTable_ah1.cpp:275`, `PromoLoop_round42.cpp:27`). So that copy's metric is 0 for every car and it never records a finisher, while `RuleEngine` records from the real standalone metric. Absent from **both** exe copies vs the asi: the metric write at `AiPreTick.cpp:185`, mode-9 and mode-4 speed scaling (`:207-241`), powerup speed doubling (`:244-255`), and the entire `DAT_0089a368` state machine with both probability rolls (`:257-323`). (`Ai/AiControllerAB.cpp:721` is **NOT-A-PAIR** — an A/B harness gated on `MASHED_AI_AB`+`MASHED_HOOK_ONLY`.) |
| `0x00416a30`, `0x00417da0` | C3 | `Ai/AiStandalone.cpp:977`, `:1096` | `Ai/AiControlStep.cpp:382`, `:392` | **Wrong input quantity.** exe `:982`/`:1101` `speed = sqrt(vx*vx + vz*vz)` replaces the asi's two distinct callees (`FUN_0046d6a0` for the brake delta, `FUN_0046d6d0` for the steer magnitude, `AiControlStep.cpp:130`). `rate1` is pinned `0.0f` (`:983`, `:1102`, `[U-C-RATE1]`), which makes the brake gate `kRate1Brake < rate1` permanently false and makes the curvature multiplier `mag *= kSteerExtra` (`:1002`,`:1023`,`:1121`,`:1142`) fire **unconditionally** instead of the asi's `val * (dist40 * F32(0x005cc9a0))` gated on `mode == 0 && F32(0x005ccd6c) < dist40` (`:241-243`,`:276-278`). The history slots `0x008032d8/dc` are written (`:995-996`,`:1016-1017`) but never read, where the asi reads them into `fv10` to drive the second brake gate (`:231-233`,`:304`). M49 also drops the `DAT_0089a368 == 1` debug-accel override the asi has for non-M8 variants (`:352-358`). |
| `0x00415e20` | C3 | `Ai/AiStandalone.cpp:156` `SteerAngleError` **and** `:199` `SteerAngleErrorFwd` | `Ai/AiTargeting.cpp:360` | **The copy M49/M8 call takes the heading from velocity, not the body forward vector.** `AiStandalone.cpp:193-198` records the D3 2026-09-26 re-read that `0x0046d510` returns body forward; `SteerAngleErrorFwd` uses `own_fwd_xz`, but `SteerAngleError` (`:175`) still uses `own_vel_xz` and that is the copy M49/M8 call. Wrap polarity also disagrees at exactly 0: exe `:210`/`:218` `while (… <= 0.0f)` vs the asi's `while (… < kZero)`, so 0 maps to 360 in the exe. Scale is hardcoded `57.2957802` (`:146`) where the asi reads `F64(0x005cc970)` live. |
| `0x00416250` | C3 | `Ai/AiStandalone.cpp:830` | `Ai/AiControlStep.cpp:372` | `int mode = 0;` (`:844`) makes the whole targeting chain (`AiControlStep.cpp:147-218`) unreachable, so modes 1,2,3,5,7,8,9,10 never run. Two early returns are missing: `if (r == 2) { ctrl[4]=0; ctrl[5]=0xff; return; }` (`:154-157`) and the mode-6 fire-and-return `if (hit && Los(...)) { ctrl[5]=0xff; ctrl[0]=0; ctrl[1]=0; return; }` (`:199-204`), replaced by `AiFireDecision` (`:852`) which the source says returns 0 on every path. Different gate operand at `:847`, and a different mode-2 dot product at `:940` (y term zeroed) vs `:336`. |
| `0x00418560` | C3 | `Ai/AiStandalone.cpp:1600` | `Ai/AiController.cpp:163` | Three whole arms missing: the `gameMode == 5` startup-countdown hold (`:177-193`), the debug-spline override (`:250-258`), and the mode-8 track-`0x21` brake clear (`:265-273`). Plus a missing global write: the asi zeroes the stored spline index on every short bank (`:215-216`,`:227-228`,`:239-240`); the exe's `pick()` (`:1209-1212`) does not, so the index can stay > 0 pointing at an unusable bank. |
| `0x00418860` | C3 | `Ai/AiStandalone.cpp:1682` | `Ai/AiController.cpp:317` | The `DAT_007f0fd0 == 7` force-step of vehicle 0 (`:336-340`) is missing. `CarSlotStateSet` returns early when `0x005f2770` is 0 (`AiStandalone.cpp:1404-1409`), so `*(PTR_005f2770 + 0x34 + v*4) = 2` never happens. Note the .asi **also** has a second body for this RVA at `HUD/ScenarioLeaves_sa2.cpp:438` whose install is commented out at `:479`. |
| `0x00443080` | C3 | `D3d9Render/TrackRenderer.cpp:93` `int aib_ai_target_enable() { return 0; }` | `Util/PromoLoop_round26.cpp:59` `return *(uint32*)0x00897ffc;` | The exe has **no port** — a host callback returning a literal. The in-file comment records that `return 1` "(a) skipped FUN_00416250's sub-state-6 block, (b) stepped the PLAYER through the AI tick, (c) forced `DAT_0089a368 = 2` every frame". **This one is a decision, not an analysis.** |

### 5.3 DIFFERS-BEHAVIOUR — race / scoring / render

| RVA | C | exe copy | .asi copy | what differs |
|---|---|---|---|---|
| `0x0040eee0` | C3 | `D3d9Render/TrackRenderer.cpp` `ScoreOnElimination` / `ScoreOnEliminationTeams` | `Race/ScoringHooks.cpp:203` | The exe implements **only the `Participants()==4` arm**. Missing: the `DAT_008a94d0 == 2` arm (`:236-244`), the whole `== 3` arm FFA and team (`:245-323`), the FFA 3-alive `GameMode()==2` block (`:350-364`) and the `{3,4,5,10}` block (`:365-378`), the FFA 2-alive blocks (`:381-410`), and the FFA 1-alive `LAB_0040fbbb` progress equalise (`:495-501`, which sets every car's progress to the survivor's) — the exe has that only in its *team* 1-alive arm. |
| `0x0040b290` | **C4** | `D3d9Render/TrackRenderer.cpp:3912` `ScoreAward` | `Race/ScoringHooks.cpp:119` | The exe keeps only the prev-snapshot / delta / 6000 ms timer / floor-at-0 tail. Missing: the mode-1 gate (`:122-137`), the mode-2 gate (`:138-146`), the network clamp (`:150-152`) and the entire event-ring write (`:154-161`). The exe comment at `:3909-3911` declares the omission and scopes it to mode 0. |
| `0x00410510` | C3 | `Race/RuleEngine.cpp:143` | `Race/ScoringHooks.cpp:515` | Case 7's loop bound is `c.participants` = `DAT_008a94d0` (`RuleEngine.cpp:186`, `RuleEngine.h:64`) where the asi re-reads `FUN_0040e340()` (`:557`,`:562`) — two different globals. Every `LAB_0041062a` global side effect is absent from the exe (`:524`, `:594-613`); `RuleEngine.cpp:222-225` records them as "the race layer's job". `:226` also adds a `won0 ||` disjunct with no counterpart at `:609`. |
| `0x00408ad0` | C3 | `D3d9Render/TrackRenderer.cpp` `RacePct` | `Frontend/SmallLeaves_t1.cpp:42` | Different expression entirely: the asi leaf is `return *(float*)(0x008a96ec + v*0x30c)` (`:46`); the exe computes `fmod(progress, n)/n*100`, labelled "0x00408ad0 equivalent". `SmallLeaves_t1.cpp` is in **both** lists, so the exe also links a dead export reading an address no exe TU writes. |
| `0x0045baa0` | C3 | `Powerup/PowerupSystem.cpp` `Lookup` | `Util/PromoLoop_sessionB.cpp:1298`/`:1316` | **Different return contract:** the exe returns a 0-based index and `-1` on miss; the original/asi returns the entry pointer `0x005f9998 + i*0x40` or `0` (`:1303-1306`). Count is compile-time `9` in the exe vs `*(int*)0x005f9bd8` (`:1299`). The asi installs the register-ABI shim `Search45baa0_RegAbi` (`:1316`, installed `:1324`), not the C body. |
| `0x004b4650` | C3 | `Powerup/PowerupContact.cpp:173` | `Util/PromoLoop_sessionB.cpp:1432` (installed `:1467`) | **Three implementations, none bit-identical.** exe keeps diffs in locals (`:175-178`); `Render/PromoLoop_round22.cpp:49-56` stores and reloads them as f32 with a `volatile` on the third component — and is **not installed** (`:69` commented out, DEFERRED for x87 bit-identity); the installed body is the naked x87 `Lerp4b4650`. The exe copy also emits a `Log(0x004b4650, …)` call (`:179`) the original has not. |
| `0x004cbd30` | C3 | `D3d9Render/RwWorldStream.cpp:53` | `Render/TextureLoader_q6.cpp:76` | exe handles **only stream type 3** (`:56 if (p[0] != 3) return 0;`); the asi dispatches four cases including the fread path (`:86`) and the callback path (`:122`). The exe also emits an error on short read (`:60`) that the asi does not (`:100-103`). |
| `0x004cc050` | C3 | `D3d9Render/RwWorldStream.cpp:78` | `Render/TextureLoader_q6.cpp:169` | On a failed type-3 skip the exe **writes the position** (`:87 p[3] = p[4];`); the asi returns without touching it (`:188-190`). The exe adds an `n == 0` early-out (`:81`) and omits the file/callback cases. |
| `0x004c39b0` | **C4** | `Race/RaceCamera.cpp:55` `Vec3Norm` | `Math/RwV3dNormalize.cpp:61` | exe uses `std::sqrt` + divide and **copies the input through** on zero magnitude (`:60`); the C4 copy uses the RW two-level LUT (`:91`,`:97`), leaves `scale = 0` on zero input (`:71`,`:104`) and has a degenerate-magnitude error path (`:108-109`) the exe omits. |
| `0x004c4d20` | **C4** | `Race/RaceCamera.cpp:66` `RotateAboutAxis` | `Math/RwMatrixRotate.cpp:53` | Different function shape (rotates a vector, no `RwMatrix`) and different numerics: CRT `cos`/`sin` (`:70`) and `std::sqrt` normalise (`:68`) vs the asi's inline `FSIN`/`FCOS` (`:108-116`) and `FastInvSqrt` (`:99`), written that way on purpose (`:22-23`). Deg→rad is the decimal `0.01745329252f` (`:69`) vs the bit pattern `0x3c8efa35` (`:69`). **Note the other two sites are fine:** `Collision/ContactStubs.cpp:89` and `Vehicle/ForceIntegratorStubs.cpp:60` now **forward to** `RwMatrixRotate` (bound 2026-09-28, commit `f39747af`). |
| `0x004a2c48` | C3 | `Race/RaceCamera.cpp:43` | `Math/FPURound.cpp:63` | exe is `static_cast<int>(v)` truncation (`:44`); the installed copy is the verbatim naked x87 body with the residual correction (`:79-89`). `FPURound.cpp:42-46` names the RaceCamera copy an approximation itself. |
| `0x0042d3e0` | C3 | `Frontend/MenuInit.cpp:73` **and** `Frontend/MenuNavSM.cpp:386` | — | **Both reached in the exe, on different memory.** `MenuInit` writes 14 selected offsets over `0x00898ac4..` and deliberately skips `+28` (`:100`); `MenuNavSM::RecordsZero` memsets the standalone's own `g_records` (`:351`,`:387`) and zeroes `g_record_count` (`:388`). Reached from `exe_main.cpp:8214`/`:8215` and from `MenuNavSM.cpp:595` (live nav) respectively. Not alternatives — different targets. |
| `0x0042fa00` | C3 | `Frontend/MenuNav.cpp:230` (**not installed**, `:257`) | `Frontend/SkeletonAndScatter_t6.cpp:249` | Guard differs (`*team != 0` at `:251` vs `*piVar5 > 0` at `:270`) and the asi adds a trailing clamp (`:278-279`) the exe lacks. `MenuNav.cpp:272-273` argues the **exe** copy is the faithful one and the installed asi copy is not. |

### 5.4 IDENTICAL / DIFFERS-COSMETIC (all are duplicate-install cases — see §6)

`0x00498bf0` (C4), `0x00407640`, `0x004098a0` (C4), `0x00407a20` (C4), `0x0040b9a0` (C4),
`0x0040ba60` (C4), `0x004298c0` — same computation under two export names, both registered.
`0x004f8690` and `0x00431f30` are DIFFERS-COSMETIC (naming / `return`-vs-`break` only), likewise
both registered. `0x00405890` is IDENTICAL in logic (exe reads `p.collectTotal/collectDone`, asi
reads `0x0063a5d0/0x0063a5d4`). `0x0040e180` is DIFFERS-COSMETIC-at-the-adapter: identical
structure, loop order and tie rule, but the exe takes magnitude with `std::sqrt`
(`RaceCamera.cpp:50-52`) where the asi forwards to the RW fast-sqrt LUT at `0x004c3ac0`
(`CameraClusterHooks.cpp:38-40`) — `RaceCamera.cpp:230-233` already names this the leading suspect
for its unexplained 7.8 % pair mismatch, so it is cosmetic in structure but not in output.
`0x00468b40`, `0x0046dbe0` and `0x0046cbb0` are IDENTICAL (note `0x0046dbe0`'s two copies carry
contradictory names for the same field: "per-car contact-count getter" at
`ForceIntegrator.cpp:324` vs `VehicleRacePositionGet` "0=1st, 1=2nd…" at `VehicleState.cpp:72-75`
— one of those names is wrong).

### 5.5 NOT-A-PAIR (12)

`0x004c4600`, `0x004c51a0`, `0x004c4dc0`, `0x004c5010` (all four: `HUD/FontCtx.cpp:40-43` holds
extern **pointers to the original**, not bodies), `0x0042f7b0` (`Util/PromoLoop_round72.cpp` has no
body; the RVA appears only in a comment at `:47`), `0x0040e470`, `0x0042b8b0`, `0x0042b8c0`,
`0x00473870`, `0x004c19f0`, `0x004a4541`, `0x004987b0`, `0x00485340`, `0x00410d10`.

Three of these are worth keeping:

- **`0x004987b0` has no port at all.** Both sites are naked `push`/`ret` trampolines
  (`Physics/SmplFzxStateBlock.cpp:36`, `Input/DirectInput.cpp:53`) and there is **no
  `RH_ScopedInstall(…, 0x004987b0)` anywhere in the tree**. The row is C2, so nothing is
  overclaimed, but the tracker implies a port exists.
- **`0x004a4541`**: the two trampolines declare **different arities** for the same RVA —
  `Save/SettingsCfg.cpp:81` 2-arg vs `Save/SettingsConfig.cpp:70` 3-arg. `__cdecl` keeps both safe,
  but one comment is wrong about the original's call shape.
- **`0x00410d10`**: `Race/RaceCamera.cpp:499` and `Race/RuleEngine.cpp:67` are **complementary
  fragments**, not duplicates, and the exe calls both. The original's head gate
  `if (FUN_00443080() == 1) return 0;` is not wired — the call site passes a literal
  `/*resultDeclared=*/false` and substitutes `if (match_winner_ >= 0) return;`.

---

## 6. Duplicate `RH_ScopedInstall` inside the .asi (a second, independent defect)

Filtering to **active** (non-commented) installs in asi-compiled TUs: **1288** RVAs are installed,
and **27 of them are installed twice from two different files**. Two `RH_ScopedInstall` at one RVA
means one body is silently dead in the `.asi` — the precedent is U-9065, where a duplicate install
shipped a C3 that was never patched in (`path1-green-does-not-prove-install`).

Six are C4:

| RVA | C | subsystem | install A | install B |
|---|---|---|---|---|
| `0x00407a20` | C4 | gameplay | `Gameplay/RangeTable_ah1.cpp:277` `GetAiLapCounter` | `Util/PromoLoop_round42.cpp:29` `Table8a9648Get` |
| `0x004098a0` | C4 | gameplay | `Gameplay/RangeTable_ah1.cpp:59` `GetLedEntryArrayBase` | `Util/PromoLoop_round40.cpp:33` `Ret63a5f0` |
| `0x0040b9a0` | C4 | gameplay | `Gameplay/ScoreMasks_ah3.cpp:121` `PlayerScoreMaxTest` | `Util/PromoLoop_sessionB.cpp:4916` `MaxScoreFlags40b9a0` |
| `0x0040ba60` | C4 | gameplay | `Gameplay/ScoreMasks_ah3.cpp:135` `PlayerScoreGateInvert` | `Util/PromoLoop_sessionB.cpp:4863` `Active4Slots40ba60` |
| `0x0046cbb0` | C4 | vehicle | `Util/PromoLoop_round25.cpp:63` `CarStatePairGet` | `Vehicle/VehicleState.cpp:143` `VehicleCarStateRead` |
| `0x00498bf0` | C4 | render | `Boot/FrameDispatch.cpp:73` `DisplayActiveFlagGet` | `Boot/VideoConfig.cpp:89` `DisplayGetCursorGate` |

The remaining 21 are C3: `0x00407640`, `0x004077e0`, `0x0040b970`, `0x0040ba00`, `0x00415d00`,
`0x00416060`, `0x00426cb0`, `0x004298c0`, `0x0042af50`, `0x0042bde0`, `0x00431f30`, `0x0046c730`,
`0x0046c750`, `0x0046cbe0`, `0x004955b0`, `0x00496930`, `0x004f8660`, `0x004f8690`, `0x00556cc0`,
`0x00556cd0`, `0x005b3580`.

`0x00498bf0` is the cheap illustration: `Boot/FrameDispatch.cpp:69-71` and
`Boot/VideoConfig.cpp:84-87` are byte-for-byte the same computation
(`return *(uint32_t*)0x00773204;`) under two different export names, both installed at the same RVA.
Verdict IDENTICAL, but one install is dead and the C4 row cannot say which.

---

## 7. Third drift channel: `#ifdef MASHED_STANDALONE` inside a *shared* TU

Only 4 non-test files use it, and 3 of those are in **both** source lists — meaning the same file
compiles to different behaviour in the two targets, with the `.asi` (verified) side taking the
`#else` arm:

| file:line | RVAs affected | what the exe does differently |
|---|---|---|
| `Frontend/GameModeCarSelect.cpp:54` | `0x00431d00` (C2), the LEFT/RIGHT twin at `0x00440337` | exe **skips the call to `FUN_00431b80` entirely** and returns the already-moved cursor slot; the cross-player collision de-dup is dropped. Documented in-place as deferred, NOT-GUESSED. |
| `Frontend/MenuButtonDetect.cpp:68` | `0x0042b960` (C3 `CarSlotInit1P`), `0x0042b9e0` (C3 `CarSlotAssign`) | exe makes `CallSlotWrite` (callee `0x0040e480`) a **no-op**; 7 call sites at `:293-296`, `:333`, `:396`, `:435`. The per-slot car-alive entry is never written in the exe. |
| `Save/GameSaveBuffer.cpp:101` | the GameSaveBuffer export pair | exe rebinds every MASHED global to local storage; the file's own comment records that both exports are **dead in the exe** (`RH_ScopedInstall` is a no-op there and the live save path is `GameSaveFormat.h` via `Race/GameFlow.cpp`). Already captured by memory `save-subsystem-two-paths`. |

`Frontend/TextCtrlCodeRemap.cpp:50` also gates on `#ifndef MASHED_STANDALONE`, but that TU is
asi-only, so there is no second target to diverge from.

Note the knock-on: `0x0040e480` `CarSlotStateSet` is **C3 impl** with `file = Frontend/RaceResults.cpp`,
an **asi-only** TU. The exe neither links that body nor calls the RVA — it no-ops the call site. The
C3 evidence for `0x0040e480` says nothing about the shipping build.

---

## 8. Ranked demotion candidates and exe-copy fixes

### 8.0 First, the rubric caveat — these are not automatic demotions

`re/CONFIDENCE.md` §C3 requires "the function is hooked through `RH_ScopedInstall` and
runtime-toggleable", and C4 requires a `diff-original` Frida run. Both criteria are **defined over
the `.asi`**. Strictly read, none of the rows below has regressed against the rubric it was
promoted under — the rubric simply never claimed to cover the shipping exe.

That is the finding, not a loophole. ROADMAP v3's current phase is *"make the default build
faithful"*, so a C3/C4 that says nothing about `mashed_re.exe` is not the assurance the phase needs.
**The decision the user owns is whether to (i) demote these rows, (ii) leave the C-level and add the
`exe_file` column (§9 P2) so the gap is visible, or (iii) both.** Nothing in this note mutates a
tracker; `re-classify` runs after that decision. Suggested tracking id for the class: **U-9146**
(next free; `UNCERTAINTIES.md` tops out at U-9145).

### 8.1 Demotion candidates, highest risk first

**8 of the 184 C4 rows** carry a DIFFERS-BEHAVIOUR verdict: `0x00468980`, `0x0046b540`,
`0x0046ddb0`, `0x00467650`, `0x00470670`, `0x0040b290`, `0x004c39b0`, `0x004c4d20`. Five of those
eight are the physics A-chain.

| # | RVA | C | why the evidence does not cover the shipping exe |
|---|---|---|---|
| 1 | `0x00468980` A6b | **C4** | The exe's entire rotation-apply is **dead code**: `Vehicle/VehicleControl.cpp:195` passes `nullptr` and both `if (orient)` guards (`AeroStabilize.cpp:72`, `:91`) fail. Airborne auto-level and velocity-align never run. Default race path, and it silently invalidates any airborne-attitude parity comparison. |
| 2 | `0x0046b540` A3 | **C4** | 4× output-stride disagreement on three suspension tables (`VehicleInit.cpp:130-131`,`:138-139`,`:148-149` = 16 B vs `PhysicsChainHooks.cpp:3003`,`:3011`,`:3033` = 64 B), plus a 1-entry handling-override stub (`:36-42`) that gives every non-Arctic track default `+0x18c` — A6a's grip divisor. Spawn-time, so it poisons every later frame. |
| 3 | `0x0046ddb0` A5 | **C4** | Four `.rdata` constants are decimal approximations of addresses the .asi bit-pins; `kDt` (`ForceIntegrator.h:31`) scales the whole drive-drag term. Plus the unresolved `TriangleFaceNormal` operand-order question. |
| 4 | `0x00470670` A4 | **C4** | The car-index fix landed, but A4 is the sole caller of A6b and hardcodes its `orient` to `nullptr` (`VehicleControl.cpp:195`) — item 1's defect lives at this RVA's call site. |
| 5 | `0x00467650` A6a | **C4** | Boost block restored, but the gear-index clamp bound is 6 in the exe and 5 in the .asi (`Integrate2.cpp:131`/`:155` vs `PhysicsChainHooks.cpp:1877`/`:1887`), and `+0x478` is clamped on one side only. |
| 6 | `0x0040b290` | **C4** | exe `ScoreAward` (`TrackRenderer.cpp:3912`) drops the mode-1/mode-2 gates, the network clamp and the whole event-ring write that the C4 body has (`ScoringHooks.cpp:122-161`). |
| 7 | `0x004c39b0`, `0x004c4d20` | **C4** | `Race/RaceCamera.cpp:55` and `:66` are private approximations (CRT `sqrt`/`sin`/`cos`) of two C4-verified RW math functions, with different zero-input behaviour. The camera is default-build. |
| 8 | `0x004177b0` | C3 | Two exe copies, and `AiStandalone::AiPreTickRubberBand` reads `0x008a9648`/`0x008a96ec`, which no exe TU writes at runtime — it can never record a finisher. Rules 4/7/8/9 depend on finish order. |
| 9 | `0x00416a30`, `0x00417da0` | C3 | Wrong input quantity for the steer magnitude, `rate1` pinned 0, and the curvature multiplier therefore firing unconditionally. **This is the AI steering output and D3 criterion (b) is the phase's sole open blocker.** |
| 10 | `0x00415e20` | C3 | The M49/M8 copy takes heading from velocity, contradicting the file's own D3 re-read; plus `<` vs `<=` wrap polarity at exactly 0. |
| 11 | `0x00416250`, `0x00418560`, `0x00418860` | C3 | `mode` pinned 0 kills eight targeting modes; two early returns, three whole arms and a spline-index reset are missing. |
| 12 | `0x0040eee0`, `0x00410510` | C3 | Only the 4-participant arm exists; 2- and 3-participant matches score nothing, and the `LAB_0040fbbb` progress equalise and every `LAB_0041062a` global write are absent. |
| 13 | `0x0045baa0` | C3 | Different return contract (`-1` index vs entry pointer / `0`) and a compile-time count where the original reads `0x005f9bd8`. |
| 14 | `0x004cbd30`, `0x004cc050` | C3 | exe stream readers are type-3-only and disagree with the measured copy on the failed-skip position write. |
| 15 | `0x00408ad0`, `0x0042d3e0`, `0x0042fa00`, `0x004b4650`, `0x004a2c48` | C3 | Different expression / different target memory / different guard-and-clamp / three non-agreeing implementations / truncation-vs-x87. |

Also flag, separately from the dual-copy class: **`0x00443080`** (C3) — the exe has no port, only a
literal `return 0;` at `TrackRenderer.cpp:93`, and the transient tree revision had `return 1;`.
**`0x004987b0`** (C2) — no `RH_ScopedInstall` exists anywhere; the row's file points at two
trampolines. Neither is overclaimed at its current level, but both look like ports in the tracker.

### 8.2 Top exe-copy fixes, highest value first

These are the smallest edits that would remove the largest slice of default-build divergence. All
are proposals; none was applied.

1. **Bind A6b's `orient`.** `Vehicle/VehicleControl.cpp:195` — pass the vehicle world-transform
   instead of `nullptr`, then drop the two `if (orient)` guards at `AeroStabilize.cpp:72`/`:91`. The
   in-source note says this was "deferred until A8"; A6b is C4-verified, so the body is ready.
2. **Settle the A3 output stride.** `VehicleInit.cpp:130-131`, `:138-139`, `:148-149`. The `.asi`
   tiles the three tables as `0x4bc..0x580` / `0x5bc..0x780` / `0x7bc..0x940` with two identical
   `0x3c` gaps; the exe's 16-byte stride leaves irregular holes. Which one matches `0x0046b540`
   needs one Ghidra look at the store addressing — **do not pick by symmetry**.
3. **Replace the four A5 decimal constants with the `.rdata` bit patterns** already used in
   `PhysicsChainHooks.cpp:119-144` and `Integrate2.cpp:68-109`: `ForceIntegrator.h:31`, `:34`,
   `:45`, `:32`. Mechanical, zero-risk, removes a whole class.
4. **Resolve the `TriangleFaceNormal` ABI.** `Collision/CarWorldContacts.cpp:46` and
   `Vehicle/PhysicsChainHooks.cpp:235` state different ABIs for the same callee. One disassembly
   read of `0x0046e6f3..0x0046e728` settles it and either confirms or clears item 2 of §8.1.
5. **Fix the A6a gear clamp bound** to 5 (`Integrate2.cpp:131`, `:155`, `:196`) so `gear == 5`
   falls back to `local_54[0]` as the C4 body does.
6. **Bind the remaining Collision stubs.** `Collision/ContactStubs.cpp:93` `Rw_MatrixDerive` is an
   **empty body** while the C3 port `Render/RwMatrixInvert.cpp:194` is already linked into the exe;
   it is called at `Collision/CarCarContacts.cpp:192`,`:194` with `unsigned char local_40[64]` as
   output (`:191`), so `Rw_TransformPoints` at `:193`/`:195` transforms by **uninitialised stack**.
   `Rw_SetRotation` (`:95`, cites `0x004c52f0`, C3) and `Math_Acos` (`:97`, returns `0.0f`, so the
   angle at `WheelContactSolver.cpp:237` is always 0) are the other two.
   *(`Rw_MatrixFromAxisAngle` at `:89` is **already bound** — commit `f39747af`, 2026-09-28.)*
7. **Take the AI steer magnitude from the right quantity** — `AiStandalone.cpp:982`/`:1101` — and
   un-pin `rate1` (`:983`, `:1102`, `[U-C-RATE1]`). This is the most direct lever on D3's open
   criterion (b).
8. **Point `SteerAngleError` at the body-forward vector** (`AiStandalone.cpp:175`), matching
   `SteerAngleErrorFwd` and the file's own 2026-09-26 re-read at `:193-198`.
9. **Deduplicate the 27 double-installed RVAs** (§6) — delete one `RH_ScopedInstall` per pair. The
   bodies agree in every case examined, so this is tracker hygiene, not behaviour, but it is what
   U-9065 cost a round.
10. **Correct three source claims that the code does not support**: `ContactStubs.cpp:99` calls
    `FUN_004a2c48` a "monotone tick counter" (it is `__ftol`, `FPURound.cpp:1-2`);
    `ForceIntegrator.cpp:324` and `VehicleState.cpp:72-75` give the same field two contradictory
    names; `Save/SettingsCfg.cpp:81` and `Save/SettingsConfig.cpp:70` give `0x004a4541` two
    different arities. Also three stale headers: `VehicleControl.cpp:22` and `:28`
    ("not yet in the exe source list" — it is), and `VehiclePhysicsRun.cpp:786-788`
    ("`Fi_GameMode()` is a STUB returning 0" — it returns 6 since 2026-09-29).

---

## 9. Proposal: stop this recurring (proposal only, nothing implemented)

Ranked by cost-to-build vs defects caught.

**P1 — a build-time lint: one RVA, one body.** Add a script (`scripts/lint_rva_bodies.py`) run from
`build.bat` before the compile step. It parses both `.rsp` lists, extracts every RVA-anchored
definition and every **active** `RH_ScopedInstall`, and **fails the build** when, for one RVA:
(a) two bodies are reachable in the same target, or (b) a body exists in an exe-only TU *and* in an
asi-only TU, or (c) two active installs target the same RVA in the `.asi`. Escape hatch: an explicit
`// DUAL-COPY-OK: <rva> <other-file> <reason>` pragma at both sites, so the deliberate pairs
(§7, and the declared twins below) pass while a new accidental one cannot. This is the single
highest-yield item — it catches all five known incidents and the 27 duplicate installs, and it is
pure text processing over files the build already lists.

**P2 — a second `hooks.csv` column, `exe_file`.** Today one `file` column has to describe two
targets and cannot. Add `exe_file` (may equal `file`, may be `-` for "the exe has no port"). Make
`re-classify` refuse a C3/C4 promotion unless `exe_file` is filled in, and record in the row's notes
whether the evidence covered the `.asi` copy, the exe copy, or both. Without this, a demotion
decision has to re-derive §4 by hand every time.

**P3 — make the pairing declared, not implicit.** The frontend already has the right pattern:
`Frontend/MenuAnimTickTwin.cpp:1-11` and `Frontend/PromptStripTwin.cpp:1-10` open by naming the
exe-side port they twin (`MenuNavSM.cpp`, with line ranges) and the diff harness that compares them.
`Vehicle/PhysicsChainHooks.cpp` ↔ `Vehicle/Integrate2.cpp` had **no** such cross-reference until the
2026-09-29 fix added one. As of this audit `Vehicle/ForceIntegrator.cpp`, `Vehicle/AeroStabilize.cpp`
and `Vehicle/VehicleInit.cpp` still contain **zero** mentions of `PhysicsChainHooks` — they are the
three A-chain siblings nobody has cross-checked. Require a `// TWIN-OF: <file> <rva>` header line on
both sides; P1 can then enforce it.

**P4 — prefer one shared TU per RVA where the ABI allows.** The real reason two bodies exist is that
the `.asi` copy often needs a register-ABI naked thunk and MASHED's absolute global addresses, while
the exe copy needs standalone state. That is a *wrapper* difference, not a *body* difference: the
arithmetic core can live in one shared TU taking a state struct, with the two thunks in the
target-specific TUs. Doing this retroactively for 104 RVAs is not worth it; doing it for the physics
A-chain (`0x00467650`, `0x00468980`, `0x0046b540`, `0x0046ddb0`, `0x00470670` — all C4, all on the
default race path) is.

**P5 — close the 304 note-pointing rows.** A C3/C4 row whose `file` is a `re/analysis/*.md` cannot be
audited by any tool. Backfill them to a real source path (or `-`) as part of the P2 migration.
