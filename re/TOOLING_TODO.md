# Tooling TODO — capability ideas not yet built

Opened **2026-09-09** from the "do we have the right tools?" review. Companion to
`re/HARNESS_BACKLOG.md`, which lists *pulls inside the existing lanes*; this file lists
**new instruments** that do not exist yet, plus the one spike already run.

**Rule (same as HARNESS_BACKLOG):** pull ONE item, finish it, log the outcome in
`re/analysis/CHANGELOG.md`, come back. Every item states what it unblocks and why it is
ranked where it is — no item goes in here without a reason it beats doing nothing.

**Framing that produced this list.** First-party rows are C2 2,536 / C3 918 / C4 180, and
**only 14 of the 2,536 C2 rows have a reimplementation file at all**. The bottleneck is
authoring and default-path acceptance, not verification throughput. Two of the three
default-build gates (D1 render, D3 AI/powerups/modes) have **no acceptance instrument**;
the parity harness is frontend-only by its own scope note and is structurally blind to
text. Rank accordingly.

---

## TT-1 — Matching-compiler lane (period MSVC 13.00) — BLOCKED on toolchain

**Spike run 2026-09-09: `re/analysis/matching_compiler_spike_20260909.md`.** Modern `cl`
19.x gives no byte match on 3/3 C4 functions, but **every** divergence is codegen-era
(instruction fusion, P4-era scheduling, register allocation, CSE) and the logic already
corresponds exactly. Whether a period compiler closes it is untested.

- **Blocked by:** no `cl` 13.x on this machine. `VCToolkitSetup.exe` (archive.org) dies with
  `IDS_ERROR26` on Win11; its payload is IS6-compressed and not carvable; the
  `vc2003toolkit.7z` mirror is 404.
- **Next action:** obtain the **Windows XP DDK (2600)**, which ships `cl` **13.00.9466** as
  plain files with no InstallShield (Server 2003 DDK 3790 → 13.10.3077 is the fallback).
  Verify `[UNCERTAIN]` whether DDK `cl` 13.00.9466 is codegen-identical to retail VS .NET 2002.
- **Payoff if it lands:** static, offline, per-function evidence that needs no Frida, no
  arg_type, no live game — bypassing the harness-safety classes that block 52 of the 88
  ranked C2 frontier rows. Would need a new evidence class in `re/CONFIDENCE.md`.
- **Cost if it fails:** one afternoon; TT-2 is already banked either way.

## TT-2 — Operand-correspondence sweep over all C3/C4 rows — **RUN 2026-09-09**

> **First pass done:** 931 PASS / 155 FAIL / 44 SKIP over 1,130 rows (85.7%).
> One real defect found and filed (**U-9086**, a C4 row). Tooling and the full CSV are
> committed; `re/analysis/matchdiff_sweep_20260909.md` has the artifact taxonomy.
> **Remaining work: triage the other 154 FAIL rows** against the four known false-positive
> classes, and decide whether to run it over the C2 pool as an authoring pre-screen.


Fell out of TT-1 and needs none of its blocked parts. `re/tools/matchdiff.py` compares the
**distinct set** of absolute addresses and immediates between our compiled function and the
original bytes. Compiler-version-independent, so it runs on the MSVC 2022 build today.

- Proven non-degenerate 2026-09-09: 3/3 known-good C4 rows PASS; 2/2 injected single-token
  defects (stride `0x341`→`0x340`; base `0x899f7c`→`0x899f80`) FAIL.
- **Do:** drive it from `exe_sources.rsp`/`asi_sources.rsp` + `hooks.csv` over all 918 C3 and
  180 C4 rows; report per-row PASS/FAIL; triage every FAIL as a candidate transcription defect.
- **Constraint (do not overclaim):** this **moves no C-level up**. It is a defect *detector* —
  it can justify a demotion, and it can pre-screen a batch before harness time is spent. It
  does not substitute for a `diff-original` Frida diff.
- **Why it ranks high:** the U-9085 class (a C3 body that did not match its RVA) and
  `[[wrong-plate-propagates-into-ports]]` were both found by accident. This finds them by
  construction, over the whole tree, offline.
- Skip naked-`__asm` TUs (67 files) — they are already byte-transcribed.

## TT-3 — Emulator-differential promotion for mutator/teardown rows

`re/tools/veccap/` + `unicorn_diff.py` already give x87-bit-exact results (RW fast-sqrt 6/6,
K1 9/9, K2 12/12), but policy keeps those leaves at C1/C2, so a **working bit-exact
instrument currently produces zero promotions**.

- **Do:** (a) settle the evidence question in `re/CONFIDENCE.md` — what C-level does
  emulator-differential equivalence support, and under what non-degeneracy conditions;
  (b) add a **memory-snapshot mode**: run the original in Unicorn against a captured heap,
  run the port natively against the same restored snapshot, diff the write sets.
- **Unblocks:** the classes that dominate the frontier — TEARDOWN 24, WRITES_GLOBAL 20,
  DESTROYS_DEVICE 3. Destroying a device inside an emulator is free.
- This is the off-process generalisation of the `AiControllerAB.cpp` snapshot/restore lane
  that `HARNESS_BACKLOG.md` already asks for in-process.

## TT-4 — Coverage ledger: executed-on-canonical vs wired-in-exe

No tool answers *"which code the real game runs is still scaffold in our exe"*. Frida
Stalker destabilises hot paths (>1000 calls/s → ~6 s to failure) and TTD recording is
deferred, so there is no whole-run coverage today.

- **Do:** run the original under **DynamoRIO `drcov`** (never tried here; block-level
  coverage, few-percent overhead) across the canonical scenarios; intersect the executed RVA
  set with the exe's linked-and-reached set; publish per-subsystem.
- **Why:** this is the demand-driven metric ROADMAP v2/v3 actually asks for, and it should
  **replace the C-level histogram as the headline number** — 180/5930 measures the wrong
  thing when 2,536 C2 rows have no reimplementation and D0.7 established that many linked
  TUs are dead exports.

## TT-5 — D1 acceptance instrument (in-race render vs the ORIGINAL)

D1's A/B only ever compared librw against our own D3D9 path, never against the original, and
`ROADMAP.md` §D1 records that every in-race shot so far was framed by an **invented chase
rig** (`TrackRenderer.cpp:4097-4123`) while the verbatim pose is computed and discarded.

- **Do:** formalise the ad-hoc d1_recheck pieces (`MASHED_CAM_POSE` + the shim's
  `draw3d.json` + `imgdiff.py`) into a **matched-pose original-vs-standalone** gate; wire the
  verbatim `Race/RaceCamera.cpp` pose into the renderer so the comparison is meaningful.
- **Also:** hook the original's charset renderer (`FUN_00554940`) so original captures stop
  being textless — `parity_tooling.md:87` already names this as the fix, and font raster
  currently has *no* automated coverage at all.

## TT-6 — D3 acceptance: extend the rules-oracle to AI / powerups / modes

The live oracle pattern (read inputs → predict the ported law in JS → compare per call,
~60 calls/s, under the hot-path limit) closed D-11052..55 for the rule engine. Nothing
equivalent exists for the three scaffolds the default build still runs.

- **Do:** point the same pattern at the WS-C AI family (`FUN_00418860`), the powerup
  dispatcher (`FUN_0045bba0` + 9-entry table) and per-mode rules.
- **Note:** U-9040 — both steer conventions are dead on the default path until the
  ControlStep path executes, so the AI oracle needs the scenario work first.

## TT-7 — Scheduled C3/C4 regression sweep

Nothing re-runs the 918 C3 / 180 C4 diffs after a merge. Two drift incidents (U-9085;
duplicate-RVA implementations, U-9065) were both found by accident.

- **Do:** drive `run_diff_scenario_batch.py` (N live-state hooks per ONE boot) from the
  existing daily `MashedRE-Hygiene` Task Scheduler job; RED demotes via `re-classify`.
- Cheap: reuses a built lane, adds no new harness.

## TT-8 — ASan on the x86 standalone

MSVC's `/fsanitize=address` supports x86. Never tried here.

- **Would have caught** the `pc=0x44` crasher class (contiguous-locals stack smash;
  installed-hook ABI mismatch) at the write, instead of via minidump archaeology.
- One build + one canonical race to evaluate. Dev-only; never ships.

## TT-9 — Rerank HARNESS_BACKLOG by rows-unlocked

`HARNESS_BACKLOG.md` names the **x87 ST0 float-return handler** as NEXT PULL: it unlocks
**6** named frontier rows. The frontier's own `brief_verdict` split is MUTATOR_LANE **43** /
NEEDS_GHIDRA 29 / NEEDS_NEW_HANDLER 9 / DEFER 6 / READY 1.

- **Do:** rerank by rows-unlocked. The generalised mutator snapshot lane (TT-3, 43 rows)
  should sit above the ST0 handler (6 rows).
- Also folded in: `entity_field_set` per-side sentinel reset is a **known shipped
  false-GREEN** (`RE_MASTER_PLAN` §8) and should be fixed before any lane-B batch reuses it.

---

## Considered and deliberately NOT pursued

Recorded so nobody re-derives them. Consistent with CLAUDE.md's stated non-goals.

- **Symbolic / concolic execution** (angr, KLEE, Triton, z3) — the binary is fully
  decompiled and the hard problems are semantic-equivalence and live-state, not path finding.
- **Fuzzing** — no untrusted input surface; the port's correctness question is equivalence
  to a reference, which differential testing already answers better.
- **BinDiff / Diaphora** beyond the existing Xbox `xtwin` lane — no second symbolised binary
  to match against, and `xtwin` is explicitly a reading aid, never behavioral evidence.
- **CI / GitHub Actions / pre-commit** — "No CI/automation until a second contributor
  appears" (CLAUDE.md). The Task Scheduler hygiene job covers the periodic need (TT-7).
