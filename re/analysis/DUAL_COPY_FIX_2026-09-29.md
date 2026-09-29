# Dual-copy fix — schema, rubric, demotions, guard

**Date:** 2026-09-29
**Branch:** `race/first-frame-parity`
**Input:** `re/analysis/DUAL_COPY_AUDIT_2026-09-29.md` (`aa4795af`) +
`re/analysis/dual_copy_audit_2026-09-29.csv`, plus the post-audit fixes in
`re/analysis/PLAYER_REGRESSION_2026-09-29.md` §7.3 (`3e4fba77`, `5ef617db`).
**User decision (2026-09-29):** do all three of the audit's §8 options — demote, add
`exe_file`, add a one-copy build guard.

**This session changed no game code.** Consolidating copies is the allowlist burn-down and
belongs to ROADMAP D4.

---

## 0. The finding in one paragraph

`mashed_re.exe` and `mashed_re_dev.asi` compile **different source lists**, so one original
RVA can have two bodies. Every C3 gate (`RH_ScopedInstall` + runtime-toggleable) and every C4
gate (a clean `diff-original` Frida CSV) is defined over the **`.asi`** — and in the exe
`HookSystem::Register` is an empty function (`mashedmod/src/mashed_re/Stubs/HookSystemNoOp.cpp:19`),
so registration proves nothing there. `hooks.csv` had one `file` column, naming the `.asi`
copy. The exe copy was therefore invisible to the tracker, and nothing in the build failed
when the two bodies disagreed. Five shipping defects reached `mashed_re.exe` that way.

---

## 1. Schema — `exe_file` (commit `d03392db`)

### 1.1 What the column means

| `exe_file` | meaning |
|---|---|
| empty | the exe has **no port** of this RVA |
| `== file` | **one shared TU** serves both targets — the goal state |
| `!= file` | two bodies exist; the C3/C4 evidence covers only the `file` one |
| contains `\|` | **more than one** exe-side body. That is a defect, not a formatting choice. |

Stored with the same `mashedmod/src/mashed_re/` prefix `file` uses, so `file == exe_file` is a
direct string test — which is exactly what the new rubric clause asks a reader to evaluate.

### 1.2 Reader survey — 122 files reference `hooks.csv`

Classified header-keyed (safe when a column is appended last) vs positional (may break).

| class | count | verdict |
|---|---|---|
| `csv.DictReader` / `Import-Csv` readers | 30 | **HEADER-KEYED** — unaffected |
| raw-line `parse` → `serialize` editors (the `reclassify_batch_*` / `harvest_batch_*` family) | ~40 | **POSITIONAL-SAFE** — they round-trip every field, so column 9 survives |
| positional readers touching columns 0–5 only | ~15 | **POSITIONAL-SAFE** |
| appenders writing a fixed 9-element list | 9 | **POSITIONAL-SAFE** — they append rows, they do not rewrite existing ones |
| **asserted a literal 9** | **4** | **BROKE — fixed** |

The four breakers, each verified by reading the site before editing:

| file | what broke | fix |
|---|---|---|
| `scripts/r0/repair_hooks_csv.py:70-71` | `row[:8] + [",".join(row[8:])]` would have **folded `exe_file` into `notes`**, destroying it; plus a `len(r) != 9` fatal | `NCOLS` derived from the live header; the spill-fold now keeps every column *after* `notes` intact |
| `scripts/r0/c4_ledger_repair.py:62` | `len(row) != 9` fatal | `ncols` from the header |
| `scripts/r1/reconfirm_installed_suspects.py:46` | `len(fields) != 9` fatal | `ncols` from the header |
| `scripts/r1/demote_noninstallable_suspects.py:71` | `len(fields) != 9` fatal | `ncols` from the header |

`repair_hooks_csv.py` was the dangerous one: it is a silent data-loss path, not a crash.

**D-11069 folded in.** That deferred row asked for exactly this ("give `hooks.csv` a way to
record per-target implementations", `DEFERRED.md:1233`). Its four RVAs — `0x0046cbe0`,
`0x00431f30`, `0x004f8660`, `0x004f8690` — now carry both paths and no longer need the "cannot
express it" caveat. *(Side observation, not acted on: the D-11069 row is duplicated verbatim
at `DEFERRED.md:1233` and `:1235`.)*

### 1.3 Migration proof

`scripts/add_exe_file_column.py` appends one `,` per data line and `,exe_file` to the header —
byte-preserving for all nine existing fields, quoting included. Self-verifying and idempotent:

```
pre:  header + 5899 data rows (all 9 fields) + 31 comment lines
post: header + 5899 data rows, all 10 fields, fields 0..8 byte-identical, field 9 empty  [VERIFIED]
(second run) already migrated: header already carries exe_file
```

Readers re-run on the 10-column file, all clean: `progress.py` (5899 rows), `c2_gate_audit.py`,
`promote_frontier.py`, `dualinstall_audit.py`, `area_residue.py --subsystem vehicle`,
`caller_screen.py`, `orch_preflight.py`, `merge_trackers.py` (a self-merge emits
`...,notes,exe_file` — the new column passes through), and all four fixed scripts.

### 1.4 Backfill

`scripts/backfill_exe_file.py`, two channels unioned:

- **A** — RVA-anchored definitions in exe-compiled TUs (`scripts/rva_body_scan.py`): 544 RVAs.
- **B** — the audit CSV's `exe_side_copy`, for ports the exe implements as a *named* standalone
  method with no RVA comment (`VehicleInit`, `ForceIntegrator`, `ScoreAward`): 78 RVAs.

542 `hooks.csv` rows filled, **24 of which name more than one exe TU**.

**The number that matters** — of the **1213** C3/C4 rows:

| | rows |
|---|---|
| `exe_file == file` (one shared TU; evidence covers what ships) | **203** |
| `exe_file != file` (two bodies; evidence covers only one) | **212** |
| `exe_file` empty (the exe has no port at all) | **798** |

---

## 2. Rubric — `re/CONFIDENCE.md` (commit `bd367bc1`)

New clause **"Which copy the evidence covers"**, between C4 and the demotion rules:

- `file` names the copy the evidence measured; `exe_file` names the exe's body.
- A row is C3/C4 **for the shipping exe** only if `exe_file` is empty or `== file`, **or** the
  exe copy has its own evidence cited in `notes`.
- When `exe_file != file` the level describes the `.asi` copy only, and **must not** be cited
  in a parity, D3-criterion or Definition-of-Done argument.
- **Fixing the exe copy by reading is not evidence for the exe copy.** It is a C2-grade
  statement: the decomp was read and the body matches, but nothing measured it.
- **A "fixed" exe copy does not restore the row.** Demotion is judged per copy, not per RVA.

Plus a fourth automatic demotion trigger: *the shipping-exe copy is shown to differ in
behaviour from the copy the evidence measured.*

---

## 3. Demotions (commits `7c178945`, `f75ea717`)

**29 rows demoted to C2 across two transactions — 9 × C4→C2, 20 × C3→C2.**
`C4 184 → 175`, `C3 1029 → 1009`, `C2 3865 → 3894`, 5899 rows throughout.

| transaction | scope | demoted |
|---|---|---|
| 1 — `7c178945` | the audit's 26 **DIFFERS-BEHAVIOUR** pairs | **25** (8 × C4, 17 × C3) |
| 2 — `f75ea717` | the audit's 53 **UNREVIEWED** worklist, + 1 relabel | **4** (1 × C4, 3 × C3) |

### 3.1 Re-verification came first

Every row was re-read against the tree **at HEAD**, not taken from the audit — three exe
copies were fixed after the audit was written and those fixes had to be credited. It was done
**twice, independently**: once in-session by direct read, once by a separate pass that was
given the audit's claims and asked to re-locate every one by function name. The two agree on
all 26 rows, and the second pass found three things the first did not (below).

### 3.2 Fixed since the audit — credited, and still demoted

| RVA | what was fixed | where |
|---|---|---|
| A3 `0x0046b540` | output stride 16 B → `0x40` in all three loops | `Vehicle/VehicleInit.cpp:145`, `:153`, `:163` |
| A5 `0x0046ddb0` | **eight** `.rdata` constants to exact bits (the audit named four) | `Vehicle/ForceIntegrator.h:41-57`, `asFb(bits)` |
| A6a `0x00467650` | gear-index clamp bound 6 → 5 | `Vehicle/Integrate2.cpp:141`, `:150`, `:165` |
| A4 `0x00470670` | the hardcoded car index → a real `int car` | `Vehicle/VehicleControl.cpp:93`, `:194` |

**All four rows are still demoted, for two reasons that stack:** each still carries other
differences (§3.3), *and* each fix was made **by reading the original**, which the new rubric
clause calls a C2-grade statement about the exe copy. A fixed copy does not restore the row.

### 3.3 C4 → C2 (8)

| RVA | the difference that survives at HEAD |
|---|---|
| `0x00468980` A6b | rotation-apply still **dead**: `VehicleControl.cpp:195` passes `nullptr`, both guards (`AeroStabilize.cpp:74`, `:91`) fail. **And this one indicts the `.asi` too** — see §3.5. |
| `0x0046b540` A3 | stride fixed; the 1-entry handling-override stub (`VehicleInit.cpp:36`, `[UNCERTAIN U-A3-TABLE]`), the decimal `0.397094f` at `:94` beside its own exact-bits mirror at `:95`, and the four never-written handling globals `0x00613108/14/30/3c` all remain |
| `0x0046ddb0` A5 | constants fixed; the added `std::memset` at `ForceIntegrator.cpp:73` and the unresolved `TriangleFaceNormal` operand order at `:214` remain |
| `0x00467650` A6a | gear bound fixed; a **second, distinct** clamp remains — the `+0x478` gear-column read is clamped at `Integrate2.cpp:206` and unclamped at `PhysicsChainHooks.cpp:1902` |
| `0x00470670` A4 | car index fixed; sole residual is the `nullptr` it hands A6b — the A6b defect lives at this call site |
| `0x0040b290` | exe `ScoreAward` keeps only the prev/delta/6000 ms/floor tail; the mode-1 gate, mode-2 gate, network clamp and the whole event-ring write are absent |
| `0x004c39b0` | `RaceCamera.cpp:55` `Vec3Norm` — `std::sqrt` + divide, and **copies the input through on zero magnitude** (`:60`) where the C4 body leaves scale 0 |
| `0x004c4d20` | `RaceCamera.cpp:66` `RotateAboutAxis` — CRT `cos`/`sin` and decimal `0.01745329252f` vs inline `FSIN`/`FCOS` and `0x3c8efa35` |

For the two `RaceCamera` rows note the shape precisely: **both** TUs link into the exe, so the
camera path uses the private approximation while every other exe caller uses the verified RW
body. That is a `DUP-IN-TARGET`, not a missing port.

### 3.4 C3 → C2 (17)

- **AI (8)** — `0x004177b0`, `0x00415e20`, `0x00416250`, `0x00416a30`, `0x00417da0`,
  `0x00418560`, `0x00418860`, `0x00443080`. The load-bearing ones: `rate1` pinned `0.0f`
  (`AiStandalone.cpp:983`, `:1102`) makes the brake gate permanently false and fires the
  curvature multiplier unconditionally; `int mode = 0` (`:844`) kills eight targeting modes;
  `SteerAngleError` still takes heading from velocity (`:175`) while its own sibling at `:215`
  uses body-forward. **D3 criterion (b) is the phase's sole open blocker and it runs on these.**
- **scoring / race (3)** — `0x0040eee0` (only the 4-participant arm exists), `0x00410510`
  (different global for the loop bound, every `LAB_0041062a` side effect absent), `0x00408ad0`
  (a different expression entirely).
- **powerups (2)** — `0x0045baa0` (different **return contract**: index/`-1` vs entry
  pointer/`0`), `0x004b4650` (three implementations, none bit-identical, and the exe emits a
  `Log` call the original has not).
- **math / menu (2)** — `0x004a2c48` (truncation vs the naked x87 `__ftol`), `0x0042fa00`
  (different guard, missing clamp — and `MenuNav.cpp:272-273` argues the **exe** copy is the
  faithful one, so the C3 sits on the copy its own source calls the worse of the two).
- **streams (2)** — `0x004cbd30`, `0x004cc050` (type-3 only; and the exe writes the position on
  a failed skip where the `.asi` does not).

### 3.5 What the re-verification added that the audit did not have

1. **A5 is eight constants, not four.** `kGripRampK`, `kRandScale`, `kPrngScale` and
   `kSpeedMin` were also decimal truncations. All eight now bit-pinned.
2. **A6b's `.asi` C4 copy is itself unsupported.** The original sets A6b's ESI from a **stack
   slot** — `0x0047093b mov esi, dword ptr [esp + 0x3c]`, two instructions after A6a's
   `mov esi, edi` — which contradicts the exe's `nullptr` **and** the `.asi` forwarder's
   `mov esi, ecx` (`PhysicsChainHooks.cpp:220`). A6b's ESI use sits in the fully-airborne
   branch, which a held-lock donut barely exercises, so a C4 diff could pass without covering
   it. **Neither copy is established.** `U-9149`.
3. **A6a carries a second, distinct clamp** at `Integrate2.cpp:206`, separate from the
   `local_54` bound that was fixed. Measured inert (`gear` ∈ 0..4), still a difference.

### 3.6 Two rows deliberately NOT demoted, and why

- **`0x0042d3e0` stays C3.** The audit filed it DIFFERS-BEHAVIOUR; both independent re-reads
  say **NOT-A-PAIR**. `MenuEntryArrayInit` (`Frontend/MenuInit.cpp:73`) writes 14 selected
  offsets over `0x00898ac4..` and skips `+28`; `RecordsZero` (`Frontend/MenuNavSM.cpp:386`)
  memsets the standalone's own `g_records` and zeroes `g_record_count`. **Disjoint memory,
  both reached on purpose** — not two implementations of one function, so the demotion rule
  does not apply. The real defect is an RVA comment claiming an RVA the function does not
  implement: filed **U-9150**.
- **`0x00443080` was demoted, but its evidence is recorded rather than dismissed.** The exe's
  literal `return 0` **is** measured against the original and the measurement holds:
  `tgt_7ffc` is 0 on **6259/6259** AI steps of
  `verify/d3_ai_20260926/o_inputs.msd.aistep.csv`, re-counted here. That establishes agreement
  **with a constant** on one captured race — not a reimplementation of a global read
  (`*(uint32*)0x00897ffc`). The C3 gate requires a reimplementation, so a measured stub is C2.

### 3.7 The 53 UNREVIEWED candidates, classified (transaction 2)

The audit listed these as a **worklist**, explicitly not scored — *"candidates from the
structural channels, not verdicts"*. All 53 are now classified, in four parallel read-only
passes, and every DIFFERS-BEHAVIOUR row was re-verified in-session by direct read before it
was touched.

| verdict | rows | what it means |
|---|---|---|
| **NO-EXE-COPY** | **21** | the exe has no body at all. The honest statement is *"the evidence does not cover the default build"*, which the empty `exe_file` cell now says without a demotion. |
| **NOT-A-PAIR** | **20** | a trampoline, an extern function-pointer to the original, a comment-only mention, an A/B harness twin, or complementary fragments |
| **BOTH-SHARED** | **8** | both TUs are in **both** rsp lists, so there is no per-target divergence — a `DUP-IN-TARGET` the guard already flags |
| **DIFFERS-BEHAVIOUR** | **3** | demoted |
| **DIFFERS-COSMETIC** | **1** | not demoted — see below |

So **41 of the 53 are not dual-copy hazards**. That is worth stating plainly: the audit's
structural channels are deliberately over-inclusive, and this pass is what separates a real
second body from an `extern` pointer with the same RVA in a comment above it.

**The 3 demoted (C3 → C2):**

| RVA | the difference, verified at HEAD | shipping consequence |
|---|---|---|
| `0x00417640` | exe pins `const float rate = 0.0f;` (`Ai/AiStandalone.cpp:1363`), so the gate `kPowerupBrakeRateGate < rate` is **provably always false** and the copy always falls through to `ctrl[4]=0; ctrl[5]=0`. The `.asi` calls the real `call_0046d6d0` (`Ai/AiController.cpp:131`) and can apply full brake at `:141`. | opponent AI on track `0x21` never executes the powerup-brake full-stop override. Same `rate1`-pinning family as §3.4's AI block. |
| `0x0042d5a0` | the exe has **no port** — `exe_main.cpp:8351` installs a thunk to `Standalone_CreditsNoOp`; the `.asi` installs the full 766-byte `MenusBodyA` credits renderer (`Frontend/MenuMixed.cpp:143`). | the credits screen renders **nothing** in the shipping build. |
| `0x00497450` | different **data source**: exe returns the standalone placeholder `g_game_state.player_active[player]` (`Frontend/MenuNavSM.cpp:462`); the byte-verified `.asi` reads `*(u32*)(0x007e96fc + i*0x200)` (`Util/PromoLoop_sessionB.cpp:231`). The exe's own comment at `:459` also mis-states the stride as `*0x80`. | drives the screen-`0x1c` grey-out predicate. |

**Not demoted, and why:** `0x00417180` (DIFFERS-COSMETIC). The exe rolls its variety value
with an LCG stand-in `RandUnit()` (`Ai/AiStandalone.cpp:1324`) where the `.asi` calls the real
`RandFloat(0x3f800000)` (`Ai/AiPreTick.cpp:154`). Branch structure and offsets agree, and
`AiStandalone.cpp:1242` asserts it *"does not affect steer/accel/brake output"*. **That
assertion is an in-file claim, not a measurement**, and it is the only thing holding the row.
Left at C3 rather than moved on an unmeasured claim in **either** direction — flagged here
instead, as a candidate for the D4 pass to settle.

### 3.8 One relabel the audit did not ask for — flagged so it can be reversed

**`0x0040e180` `MostSeparatedPair`, C4 → C2.** The audit scored it DIFFERS-COSMETIC, but its
own text says *"cosmetic in structure but not in output"*. The exe copy takes the pair
magnitude with `std::sqrt` (`Race/RaceCamera.cpp:50`, called at `:194`) where the original and
the C4-verified `.asi` copy forward to the RW fast-sqrt LUT at `0x004c3ac0`
(`Race/CameraClusterHooks.cpp:38`). That magnitude is the operand of the `best <= m`
comparison at `RaceCamera.cpp:196` **that chooses the pair**, so an approximation difference
flips near-ties. The exe copy's own comment records **7.8% of 766 captured frames choosing a
genuinely different pair** from the original (`RaceCamera.cpp:225-233`).

**Stated honestly:** that comment names a *different* leading suspect for the 7.8% — the
offline driver's `active` derivation, untested — so the **cause is not established**. What is
established is that the shipping copy is a different implementation and disagrees with the
original on 7.8% of frames, on the default-build camera path. That is enough to say the C4
does not describe what ships. If you disagree with the relabel, this is the one row to revert.

### 3.9 What the C-levels do and do not now say

The 29 demoted rows have **not** lost their analysis. Their decomp is read and transcribed on
both sides — that is precisely what C2 means. What they have lost is the claim that anything
**measured** the body `mashed_re.exe` runs. Each re-earns C3/C4 through the normal gates once
its pair is consolidated (§5).

And the converse is worth stating, because it is the larger number: **798 C3/C4 rows have an
empty `exe_file`** and were **not** demoted. For those the exe has no port at all, so the
evidence is not *wrong* — it simply does not cover the default build, and the empty cell now
says so without moving a level.

---

## 4. Guard — one RVA, one body (commit `bbe817e3`)

`scripts/lint_rva_bodies.py`, called from `mashedmod/build.bat` immediately after `vcvars32`
and **before either compile step**. `MASHED_SKIP_RVA_LINT=1` skips it.

| check | count | what it means |
|---|---|---|
| `DUP-IN-TARGET` | **72** | one RVA, two bodies in the SAME target (exe 18, asi 54). Only one can run. |
| `CROSS-TARGET` | **11** | a body in an exe-only TU **and** in an asi-only TU — the dual-copy class |
| `DUP-INSTALL` | **27** | one RVA, two **active** `RH_ScopedInstall` in `.asi` TUs. Kept as a **separate list** (§4.0): it is the U-9065 defect (one body silently dead), not a body-count defect. 27 matches the audit §6 exactly. |

### 4.0 The 27 double-installed RVAs, as a separate list

Two active `RH_ScopedInstall` for one RVA means one body is silently dead in the `.asi`, and
the row cannot say which — the U-9065 failure (`path1-green-does-not-prove-install`). Counts
and membership re-derived here independently and they match the audit §6 exactly: **21 C3 + 6
C4**.

**Six at C4** (each `install A | install B`):

| RVA | name on the row | the two installing TUs |
|---|---|---|
| `0x00407a20` | `Table8a9648Get` | `Gameplay/RangeTable_ah1.cpp` \| `Util/PromoLoop_round42.cpp` |
| `0x004098a0` | `Ret63a5f0` | `Gameplay/RangeTable_ah1.cpp` \| `Util/PromoLoop_round40.cpp` |
| `0x0040b9a0` | `MaxScoreFlags40b9a0` | `Gameplay/ScoreMasks_ah3.cpp` \| `Util/PromoLoop_sessionB.cpp` |
| `0x0040ba60` | `Active4Slots40ba60` | `Gameplay/ScoreMasks_ah3.cpp` \| `Util/PromoLoop_sessionB.cpp` |
| `0x0046cbb0` | `CarStatePairGet` | `Util/PromoLoop_round25.cpp` \| `Vehicle/VehicleState.cpp` |
| `0x00498bf0` | `DisplayActiveFlagGet` | `Boot/FrameDispatch.cpp` \| `Boot/VideoConfig.cpp` |

**Twenty-one at C3:** `0x00407640`, `0x004077e0`, `0x0040b970`, `0x0040ba00`, `0x00415d00`,
`0x00416060`, `0x00426cb0`, `0x004298c0`, `0x0042af50`, `0x0042bde0`, `0x00431f30`,
`0x0046c730`, `0x0046c750`, `0x0046cbe0`, `0x004955b0`, `0x00496930`, `0x004f8660`,
`0x004f8690`, `0x00556cc0`, `0x00556cd0`, `0x005b3580`.

**None of these 27 was demoted.** The audit examined the bodies and found them to agree in
every case it read, so this is tracker hygiene rather than behaviour — but it is exactly what
cost U-9065 a round, and the C4 rows cannot say which of their two installs won (the winner is
decided by static-init, i.e. **link order**, so it can flip when a source list changes). The
fix is deleting one `RH_ScopedInstall` per pair; it is item (3) in the D4 attack order.

Shared scanner `scripts/rva_body_scan.py` restates the audit §2 method mechanically so the
guard and the backfill cannot drift apart. Two refinements the audit did not need:

- **only the closest RVA comment anchors a definition.** A header block listing several
  addresses above one function otherwise reports each as a body — that alone was 634 false
  RVAs down to 544.
- **`.text` window `0x00401000..0x005d0000`.** The audit's upper bound removes `.rdata`
  comments; the lower bound removes `// 0x00000341`-style struct-offset comments.

Stated limit, unchanged from the audit: this is a **text** scanner. It proves a body is
*compiled into* a target; it does not prove the target's call graph reaches it, and it cannot
see a port with no RVA comment above the definition. The counts are a floor.

### 4.1 Allowlist

`re/tools/dual_copy_allowlist.txt`, seeded with today's **110** findings, each line tagged with
its 2026-09-29 audit verdict:

| tag | entries |
|---|---|
| `audit=UNREVIEWED` | 44 |
| `audit=DIFFERS-BEHAVIOUR` | 19 |
| `audit=IDENTICAL + DUPLICATE-INSTALL` | 17 |
| `audit=not-in-audit` (found only by the scanner) | 16 |
| `audit=NOT-A-PAIR` | 6 |
| `audit=DIFFERS-COSMETIC (+ DUPLICATE-INSTALL)` | 5 |
| `audit=IDENTICAL` | 3 |

The header says in as many words that an allowlisted entry is **not** known-safe — it is
allowlisted so the guard can go live — and that adding a line is a decision, because each of
the five 2026-09-29 defects would have produced exactly such a line.

### 4.2 Guard proof

**WARN run** — both targets build clean with the guard in the path:

```
[rva-lint] 110 finding(s): CROSS-TARGET=11, DUP-IN-TARGET=72, DUP-INSTALL=27  |  allowlisted=110 NEW=0
[rva-lint] OK -- no NEW duplicate bodies (110 known, tracked in re\tools\dual_copy_allowlist.txt)
=== Building mashed_re.exe ... ===   [exe] all 215 objects up to date
=== Building mashed_re_dev.asi ===   [asi] all 418 objects up to date
exit=0
mashed_re.exe      1,942,016 B
mashed_re_dev.asi  1,134,592 B
```

**FORCED-FAIL run** — a temporary `Stubs/DualCopyGuardProbe.cpp` carrying a second
`// 0x00467650` body, added to `exe_sources.rsp`:

```
[rva-lint] 111 finding(s): ... allowlisted=109 NEW=2
[rva-lint] ERROR DUP-IN-TARGET 0x00467650  [exe] two bodies: Stubs/DualCopyGuardProbe.cpp:7
           (DualCopyGuardProbe_A6a), Vehicle/Integrate2.cpp:122 (Vehicle_Integrate2)
[rva-lint] ERROR CROSS-TARGET  0x00467650  exe-only Stubs/DualCopyGuardProbe.cpp,
           Vehicle/Integrate2.cpp  vs  asi-only Vehicle/PhysicsChainHooks.cpp
[rva-lint] FAILED: 2 NEW duplicate-body finding(s).
[ERROR] rva-body lint failed
build exit=1
```

The build **never reached a `=== Building` line** — the guard stops it before any compile.
Note `allowlisted` fell 110 → 109: the `CROSS-TARGET` key encodes the exact file set, so
adding a file to a known pair is itself reported as new. Probe and `.rsp` entry removed; guard
back to `NEW=0`, exit 0.

---

## 5. Burn-down plan

Tracked as a named **ROADMAP D4** item, and D4's gate now includes
"`re/tools/dual_copy_allowlist.txt` is empty".

**The fix per entry is consolidation, not re-verification.** Collapse the pair into ONE shared
TU judged against `original/MASHED.exe`, so `exe_file == file` and one body serves both
targets. Where the two genuinely need different wrappers (a register-ABI naked thunk for the
`.asi`, standalone state for the exe), put the arithmetic core in the shared TU and keep only
the thunks target-specific — the audit's P4.

Order of attack, by shipping risk:

1. the physics A-chain `0x00467650`, `0x00468980`, `0x0046b540`, `0x0046ddb0`, `0x00470670` —
   default race path, all C4 before today;
2. the AI copies in `Ai/AiStandalone.cpp` — D3 criterion (b) depends on them;
3. the 27 `DUP-INSTALL` rows, where the fix is deleting one `RH_ScopedInstall` (six C4; this is
   tracker hygiene, not behaviour, but U-9065 shows what it costs);
4. everything else.

Each consolidation re-earns the row's C-level through the normal gates.

Not done here, and worth naming: the guard's `CROSS-TARGET` check finds 11 pairs where channel
A alone sees both sides, while the audit found ~104 by also reading named methods. The gap is
the scanner's stated blind spot. The audit's **P3** (a required `// TWIN-OF: <file> <rva>`
header on both sides of a deliberate pair) would close it and make the guard's coverage a
census rather than a floor. Not implemented today.
