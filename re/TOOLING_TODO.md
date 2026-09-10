# Tooling TODO — capability ideas not yet built

Opened **2026-09-09** from the "do we have the right tools?" review. Companion to
`re/HARNESS_BACKLOG.md`, which lists *pulls inside the existing lanes*; this file lists
**new instruments** that do not exist yet, plus the one spike already run.

**Rule (same as HARNESS_BACKLOG):** pull ONE item, finish it, log the outcome in
`re/analysis/CHANGELOG.md`, come back. Every item states what it unblocks and why it is
ranked where it is — no item goes in here without a reason it beats doing nothing.

**Framing that produced this list.** First-party rows are C2 2,536 / C3 918 / C4 180, and
~~only 14 of the 2,536 C2 rows have a reimplementation file at all~~ **[CORRECTED
2026-09-09: that 14 came from `hooks.csv`'s `file` column, which for C2 rows usually
holds the ANALYSIS-NOTE path, not the reimplementation — 50 of 62 comparable C2 rows
are in that state. The real figure is ~185 C2 rows with a compiled reimplementation.
See `re/analysis/matchdiff_c2_prescreen_20260909.md`.]** The conclusion stands (185 of
2,536 is still a small fraction), but do not requote the 14. The bottleneck is
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
> **Triage done (same day):** 98 of 155 explained by six evidence-tested artifact classes
> (WINDOW 31, FOLDED-MEMBER 29, WRAPPER 16, UNROLLED 10, INDUCTION 9, INLINED 3);
> **57 candidates remain for hand review** (C3 35 / C4 22) -- see
> `re/analysis/matchdiff_triage_20260909.md`, which names three systematic sub-patterns
> that are probably one explanation each rather than N defects.
> **Leads cleared + C2 pre-screen run (2026-09-09).** RWP guard explained, RW-math
> hypothesis disproven, union extended cross-TU and made asymmetric; 52 candidates
> genuinely open. C2 pre-screen: 176/185 PASS (95.1%), 3 candidates — worth wiring
> into `promote-c3-batch` selection, but it reaches only 185 of 2,536 C2 rows.
> **Wired into `promote-c3-batch` (2026-09-09):** per-function workflow step 6
> (`matchdiff_sweep.py --symbol <Name>`, runs after authoring and BEFORE the Frida
> boot, nonzero exit on FAIL) plus filter-3 drift-row validation, a `(v5)` matrix row,
> and two anti-patterns. Not a candidate filter — a fresh candidate has no reimpl to check.
> **Next:** hand-review the 52 (start with the 9 at size ratio <0.5).


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

## TT-10 — TTD capture -> offline replay diff — **REPLAY DONE; RECORDING BLOCKED BY POLICY**

> **Recording un-deferral attempted 2026-09-09 and FAILED.** Defender ASR rule "Block use
> of copied or impersonated system tools" kills `tools\ttd_x86\TTDInject.exe`; the machine
> is now organization-managed so the rule cannot be disabled locally. The rule targets
> COPIED system tools, and copying the recorder out of the WinDbg AppX is exactly how this
> lane works (WindowsApps ACLs block running it in place). Needs an admin path exclusion.
> Detail + ways forward: `scripts/ttd/README.md`.
>
> Consequence: the `asi:<Export>` backend works but is bounded by the ONE existing capture
> (8 distinct inputs). **This raises TT-11 above TT-10**: the shadow-A/B lane needs no
> external instrumentation tool, so it has no ASR surface.

> `scripts/ttd/ttd_reimpl_diff.py` gained the `asi:<Export>` backend its own docstring
> had been asking for: spawn MASHED muted, use the dinput8-auto-loaded `.asi`, wait for
> the RW engine, call the export once per TTD-captured input. One boot per CSV.
> First result: `asi:FastSqrt` **128/128 bit-identical** vs the 2026-06-17 capture of
> `0x004c3b30` — but that capture has only **8 distinct inputs**, and it is path1
> (direct export call), so it is not an automatic C4. Controls pass: a wrong export
> fails loudly, and `asi:FastInvSqrt` on the same capture scores 9/128.
>
> **Remaining: TTD RECORDING is still deferred** (2026-07-17). Until it runs, this lane
> can only consume the one existing capture. Widening that 8-value domain and capturing
> more RVAs is now the binding constraint — not the diff code.

## TT-11 — Generalise the in-process shadow A/B into a macro (mass lane) — **MASS LANE LIVE 2026-09-10**

2026-09-10 status: `re/tools/shadow_gen.py` generates sites mechanically (79 return-value +
15 verified region sites from the 153 installed C2 ports), `shadow_batch.py` boots them in
groups with crash bisection, `shadow_ab_report.py` produces verdicts. First sweep: 44 CLEAN,
9 DIVERGENT (mostly non-idempotent functions — `rand()`, first-call flags — NOT port defects
until reviewed), 5 float10 lane-limit, 21 unreached. Full write-up:
`re/analysis/shadow_lane_20260910.md`. Remaining ceiling = scenario reach + idempotency +
55 void ports without a verified region (12 need a two-span `RunRegion`).

The B5c pattern in `Collision/RwpIntegrator.cpp:35-130` snapshots the output region,
`HookSystem::Uninstall`s the inline-JMP, calls the ORIGINAL at its RVA, restores, runs the
port, and bit-compares — at the REAL call site, with **zero Frida overhead**.

That sidesteps every blocker the Frida lane dies on: no `arg_type` (the compiler passes the
args), no synthetic-call safety problem (state is real, the write region is restored), no
hot-path instability, and N functions self-test per boot instead of one boot per function.

- **State:** ~50 lines of hand-written boilerplate per function, 6 sites in
  `RwpIntegrator.cpp` plus `VehicleCouplingBridge.cpp`. `HookSystem.h` has no macro for it.
- **Do:** a `SHADOW_AB(Name, RVA, out_region)` macro + a registry, so adding a function is
  one line. That is what turns a proven pattern into a mass lane.
- **Limits:** `.asi` only (verifies logic, not standalone runtime); needs Uninstall/Install
  to be safe (single-threaded, phase-gated — `b5cInRace()`); only covers what the scenario
  reaches; each function needs its output region defined.

## TT-12 — DynamoRIO `drwrap` as the capture feeder

Never tried here. Wraps N functions' entry/exit and dumps args/returns at a few percent
overhead, where Frida `Interceptor` destabilises MASHED above ~1000 calls/s. It is the
missing capture half for TT-10 on hot functions, and pairs with the `drcov` idea in TT-4.

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

## TT-13 — Lane 2: decomp→port transcriber — **PILOT RUN 2026-09-10**

`re/tools/decomp2port.py` + `DecompPC.java --port` mode. Ghidra decompilation → verbatim MSVC
C++ TU per function (Ghidra typedefs, absolute-address globals, raw-RVA thunks with C-cast
args, `L2_` opt-in hook prefix, shadow wrapper on return-value ports), refusing register-ABI,
indirect-call, pseudo-op and unknown-type bodies with a per-row reason. Pilot on 53 reachable
unported C2 rows: 25 TUs compile (5 return-value, 20 void), 22 refused for indirect calls,
4 for register ABI, 1 compile failure pruned. Design + scaling needs:
`re/analysis/lane2_decomp2port_design_20260910.md`. Next: known-vtable idiom table for the
RW device slot, disasm-based cc detection for other fn-ptr calls, reachability at pool scale.

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
