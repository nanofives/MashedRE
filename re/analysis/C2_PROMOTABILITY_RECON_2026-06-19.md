# C2 Promotability Reconnaissance — 2026-06-19

**Purpose.** A second concurrent session is *building and running* promotion
methods. This session was dedicated (user, 2026-06-19) to **target
reconnaissance** instead: characterize the C2 pool so the running session aims
its methods at the high-yield subset rather than a blind pool (the
"8/48 collapse" failure mode). Read-only on `hooks.csv`; no tracker mutations.

All counts from `hooks.csv` at the date above (5,862 mapped rows).

---

## 1. The funnel: 3,913 C2 → 2,233 genuinely-promotable first-party

| Stage | Removed | Remaining | Why |
|---|---:|---:|---|
| All C2 | — | 3,913 | confidence == C2 |
| − subsystem-tagged `third-party-library[*]` | 1,213 | 2,700 | library-skip ruling ([[feedback-library-skip-bands]]) |
| − rows in known library **address bands** | 389 | 2,311 | mis-tagged library (libpng/zlib 133, msvc-crt 89, d3dx9-psgp 104, d3dx9-shader 63) |
| − `boot` rows with CRT/FidDB/SEH markers | 78 | **2,233** | CRT init/heap/SEH library code wearing a `boot` label |

The **389 + 78 = 467 mis-tagged rows are cleanup, not promotions** — lists at
`re/analysis/recon_c2/reclassify_out_libband.tsv` and
`reclassify_out_boot_crt.tsv`. Promotion methods run against them either RED or
produce bogus C3. Two of the boot-CRT rows (`CrtSehProlog` 0x004a5984,
`CrtSehEpilog` 0x004a59bf) were already **demoted C3→C2 because hooking them
crashes** — promotion traps.

**Convergence check (validates the funnel):** this independently-derived split
matches the `promote-c3-batch` skill's existing `C2_GATE_AUDIT` taxonomy —
their `crt-fiddb` 371 ≈ our library bands 467; their `com-directshow` 90 ≈ our
COM 90–103; their `arg-shape`+`rw-api-orchestrator`+`struct-getter` ≈ our
RW-struct tier. Both maps agree.

---

## 2. The cheap veins are confirmably DRAINED (don't re-grind them)

- `re/analysis/plans/promote_frontier.tsv` (leaf + scalar + size-gated frontier):
  **30 rows** today (was ~174). 0 `AUTO`-classifiable.
- `re/analysis/plans/promote_classified.tsv`: **122 STATE + 17 MANUAL, zero
  AUTO**. The immediately-promotable display-independent shapes
  (`const_return`, `read_global`, …) are exhausted.

Re-running `promote_frontier.py → promote_classify.py` on the leaf pool will
not produce new throughput. **This is the empirical evidence behind ROADMAP v2's
demotion of batch fanout** ([[project-roadmap-v2]]).

---

## 3. HEADLINE FINDING — 122 STATE candidates just became runnable

`promote_classify.py` parks a leaf as **STATE** when it *"reads a live `.data`
global or derefs an arg → needs a booted game."* These were un-runnable while
the boot crash blocked the `run_diff` lane. **That crash was root-caused and
fixed 2026-06-13/15** (`patch_mashed_fix_camera_res.py`; booted `run_diff` lane
OPEN — [[project-boot-crash-rw-nullderef-not-display]]). So **all 122 are now
runnable for the first time**, via the booted-game scenario harness.

They are small (25–176 B) and spread across live subsystems:

| subsystem | n | | subsystem | n |
|---|---:|---|---|---:|
| render | 33 | | particle | 5 |
| audio | 26 | | boot | 4 |
| gameplay | 17 | | input / hud / ai | 2 each |
| util | 13 | | smplfzx | 2 |
| vehicle | 8 | | sky / powerups | 1 each |
| frontend | 6 | | | |

**Worklist:** the `STATE` rows of `re/analysis/plans/promote_classified.tsv`.
Two sub-lanes by what state the global belongs to:
- **menu-reachable globals** → immediately runnable (boot-to-menu scenario).
- **race-state globals** → run under the existing race scenario
  (`MASHED_RACE_DEMO` / full-race harness, [[project-round1-race-flow]]).

This is the single highest-leverage handoff: zero new tooling, a whole
previously-blocked tier, ready now. **Recommend the running session re-point
here before grinding anything else.**

---

## 4. The structural gap, SUBDIVIDED (`scripts/promote_subdivide.py`)

`C2_GATE_AUDIT` flagged `unclassified | 1311 | sample to subdivide`. **Built and
run 2026-06-19.** `promote_subdivide.py` reuses `promote_frontier.analyze()` (the
capstone call graph + sizes) and the `promote_classify` STATE heuristic, then
adds the one signal the leaf-only frontier throws away — **callee confidence** —
to route every clean first-party C2 (2,195 by its slightly tighter cut) into a
single primary lane. No Ghidra, no game; collision-free.

| lane | rows | meaning / where to point the harness |
|---|---:|---|
| **state-runnable-now** | **1,288** | callee-clean + reads global/derefs arg → **booted `run_diff` (just unblocked)** |
| **synthetic-candidate** | **315** | callee-clean + display-independent → synthetic `run_diff` (callees execute → booted fallback if a callee derefs) |
| indirect-dispatch | 289 | vtable / jump-table / import — no resolvable target to gate on |
| callee-blocked | 280 | ≥1 direct callee is <C2 → callee-first (blockers listed per row) |
| oversize | 23 | >2 KB — one-fn-per-session only |

**1,603 are directly actionable now** (the two callee-clean lanes). Per-lane
worklists at `re/analysis/recon_c2/subdivide_<lane>.tsv`; all-in-one at
`subdivide_all.tsv` (cols: rva, sub, name, lane, size, n_callees, n_blocking,
blocking, arg_hint, n_callers_c2plus, notes).

**The pool is non-leaf-dominated — this is WHY leaf tooling stalled.** Of the 315
synthetic-candidates only **4 are leaves** (3 "prime" = leaf+≤300 B+C2+ caller);
of the 1,288 state rows, 120 are leaves (≈ the known 122 STATE — consistency
check passes) and 1,168 are non-leaves. The leaf vein really is drained; the
remaining value is the **~1,479 callee-clean NON-LEAVES** the frontier's
leaf-only gate could never surface.

### 4a. Keystone blockers — the highest-multiplier C1 targets

The 280 `callee-blocked` parents are gated behind a *small* set of <C2 callees.
None are one-cheap-leaf-hop from clean (`subdivide_order.tsv` is empty) — the
blockers are themselves **C1 non-leaves**. But they concentrate hard, so
promoting a handful of keystones unblocks dozens of parents:

| blocker (now C1) | parents unblocked | band |
|---|---:|---|
| `0x004e7e30` | 32 | render/RW-core |
| `0x0057c210` | 32 | RW-Physics-adjacent |
| `0x004e6e00` | 30 | render/RW-core |
| `0x004c0b30` | 29 | render/RW-core |
| `0x004c15c0` | 21 | render/RW-core |
| `0x004e45b0` · `0x0055ac00` · `0x004e4800` | 15 each | render / RW-Physics |

Promoting the top ~8 keystones (C1→C2→C3) cascades into ~150 parent unblocks.
Full ranking printed by the tool; the parents each lists its blockers in
`subdivide_callee-blocked.tsv`.

---

## 5. Tier files (this session's output)

Under `re/analysis/recon_c2/` (TSV: rva, subsystem, name, notes):

| file | rows | meaning |
|---|---:|---|
| `tier1_run_diff_able_now.tsv` | 54 | notes-explicit scalar/leaf + reimpl exists |
| `tier2_live_state_thiscall.tsv` | 90 | needs INSTALLED canonical (not synthetic) |
| `tier3_rw_struct_arg.tsv` | 80 | gated on a `diff_template.js` arg_type |
| `tier4_com_d3d_gated.tsv` | 90 | COM/DirectShow/D3D-device — harness-hard |
| `tier5_oversize.tsv` | 2 | too large for one session |
| `tier0_uncharacterized_remainder.tsv` | 1,917 | the §4 gap — needs the subdivider |
| `reclassify_out_libband.tsv` | 389 | §1 cleanup (mis-banded library) |
| `reclassify_out_boot_crt.tsv` | 78 | §1 cleanup (CRT under `boot` label) |

Note: §5 tier counts are **notes-prose-derived (lossy)** — lower bounds. The
authoritative tiering of the 1,311/1,917 comes only from the §4 structural
subdivider.

---

## 6. Recommended order of work for the running session

The drained leaf frontier (30 rows) is the wrong place to keep grinding. The
ranked, validated worklist is now:

1. **315 `synthetic-candidate`** (`subdivide_synthetic-candidate.tsv`) — callee-
   clean + display-independent; cheapest, no game needed. Try synthetic
   `run_diff` first; fall back to booted if a callee derefs.
2. **1,288 `state-runnable-now`** (`subdivide_state-runnable-now.tsv`, sorted
   smallest-first) — the bulk of the remaining pool; the booted `run_diff` lane
   that the 2026-06-13/15 boot fix just opened. 943 are ≤300 B.
3. **Keystone C1→C2→C3** (§4a) — promote the top ~8 blockers (`0x004e7e30`,
   `0x0057c210`, `0x004e6e00`, …) to cascade-unblock ~150 `callee-blocked`
   parents.
4. **467 reclassify-OUT** (§1) — cleanup via `re-classify`; removes promotion
   traps and sharpens every future yield estimate.
5. Deprioritize `indirect-dispatch` (289) and `oversize` (23) until the above is
   worked; both need bespoke harness/one-off effort.

Re-run `scripts/promote_subdivide.py` after a batch to refresh the lanes (it is
read-only and writes only under `re/analysis/recon_c2/subdivide_*`).
