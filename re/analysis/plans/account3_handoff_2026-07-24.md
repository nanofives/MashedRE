# account3 hand-off queue — 2026-07-24 (from an account2 session)

Work this account2 session surfaced that needs **account3** (Ghidra-MCP, Frida diff-original,
or C4 canonical-scenario evidence — none available on the Accenture license). Ranked by
leverage. Each item points at its evidence doc; don't re-derive.

Committed this session: `7e3af03b` (veccap quat onboarding + VECCAP-2 tooling/finding).
Uncommitted deliverables on disk (account2 can commit in a clean window — a concurrent session
was active on `main`): `callee_gate_cascade{.tsv,_2026-07-24.md}`, `tracker_health_2026-07-24.md`.

---

## A. NEEDS account3 (Ghidra / Frida / C4 evidence)

**A1 — VECCAP-2: re-transcribe FUN_00566200 to be x87-faithful.** *(Ghidra)*
The port at `Collision/RwpSolverLeaves1.cpp` (AABB×4×4 xform) is NOT bit-identical to the
original: Unicorn PASS but offline replay FAIL 490/513 even on bounded inputs. Fix: disasm
`0x00566200`, record the exact x87 FADD/FMUL order + which intermediates stay 80-bit, re-transcribe
with `float10` intermediates in the correct sum order (mirror the `RwpSolverMath2.cpp` quat/
translate pattern), then re-add the registry entry (kept as a documented comment in
`veccap_registry.py`, use `'synth_domain': 'bounded'`) and confirm replay == unicorn.
Full detail + evidence: **`re/analysis/plans/veccap_finding_2026-07-24.md`**.

**A2 — x87 80-bit ST0 float-return arg_type handler.** *(Frida)*
HARNESS_BACKLOG item 1's "NEXT PULL"; account2 already produced the target analysis
(`re/analysis/plans/frontier_shape_refinement_2026-07-24.md`). Authoring the handler in
`re/frida/diff_template.js` needs Frida to verify, then regen `ARG_TYPES.md`. Unlocks 6 named-shape
frontier rows (3 sin-getters 0x00431b20/b50/b60 + 3 RwV3d bbox accessors 0x004c4270/42d0/4360) and
is the same capability veccap item 6 defers `FUN_005667c0` for.

**A3 — Tracker Tier-1 C4 integrity (16 rows).** *(evidence review; likely Frida/Ghidra to confirm-or-demote)*
From the tracker health sweep. 7 C4 rows tagged an early status
(`0040b6d0 00431ae0 00431af0 00431b00 0046bce0 004c5800 004c5820`) and 9 C4 rows with a blank
`frida_diff` evidence pointer (`00404320 00492770` + the same set). A C4 with neither a resolved
status nor evidence needs its canonical-scenario evidence located, or the row demoted via
`re-classify`. Detail: **`re/analysis/plans/tracker_health_2026-07-24.md`** T1.2/T1.3.

**A4 — Drain the callee-ready C3 promotion backlog.** *(Frida — the actual C2→C3 lever)*
The callee-gate cascade scan proved there is **no cascade** (0 first-party rows below C2), so the
C2→C3 frontier is gated *only* by Frida-diff authoring, not callees. **1,416 non-leaf C2 functions
are callee-ready to author+diff now** (+ the ~19-row leaf frontier). Worklist (small-first):
`re/analysis/plans/callee_gate_cascade.tsv` CLEARED section. Tier-1 ≈ 290 clean trivial shapes
(168 `read_global_u32`, 102 `arg_getter`, 10 `const_return`); then `other`-shape by R7 subsystem.
Caveat: the 15 smallest are 5-byte tail-jmp thunks — per-row check vs the `MIN_BODY` install-clobber
class. Detail: **`re/analysis/plans/callee_gate_cascade_2026-07-24.md`**.

---

## B. Tracker hygiene — account2 COULD do (queued; touch load-bearing trackers via re-classify/tracker-editor)

From `tracker_health_2026-07-24.md`. No Ghidra/Frida needed; listed so account3 can hand them back
down or bundle them. Suggested cheap-model lane.
- **T1.1** — 13 duplicate function rows (file-offset `000xxxxx` twin colliding with the VA row,
  disagreeing name/subsystem; piz/RtFS family). Merge each to one canonical row.
- **T1.4** — status vocab: update the stale header comment; normalize `disasm`×15→`disassembled`,
  `stub`×2→`stubbed`.
- **T1.5** — 32 dead `file` paths (`library_residue/qhull.md`, `StateAccessors.cpp`, a few
  `audio_dsound` plates). Repoint or clear.
- **T1.6** — STUBS Active-section ID collisions (`S-1480`×5, `S-0340/0341/1441/1481/1482/1485`).
  Renumber to unique IDs.
- **T1.7** — move 11 struck rows out of STUBS `## Active`; refresh the census header
  (claims 1,109 open / 147 struck; section-aware actual 1,088 / 173).

Tier-2 (candidate re-review, LOW expected yield — do NOT treat as a work list): 751 UNCERTAINTIES /
398 STUBS / 12 DEFERRED open rows whose referenced RVA is now C3/C4. See memory
`callee-gate-cascade-empty` + `tracker-schema-gotchas` for why these over-count.

---

## Context memories (account2 namespace)
`veccap-onboarding-recipe`, `callee-gate-cascade-empty`, `tracker-schema-gotchas`,
`account2-mashed-workflow`. **Note:** a second session was committing to `main` concurrently on
2026-07-24 (IntroSplash + coupling-bridge commits) — coordinate before large tracker rewrites.
