# Mashed RE orchestrator — resume point (updated 2026-07-31, end of iter22)

MISSION: dual-lane — (A) fix the game per RE_MASTER_PLAN, (B) promote Ghidra functions.

**C1 795 / C2 4005 / C3 881 / C4 185.** Branch `fix/u9025-recharacterise-and-regabi-defects`,
clean, pushed, still 153 ahead of `origin/main` (unmerged).

Resume with `/orchestrate` — it reads `re/orchestrator/state.json`, which is current.
iter22 ran its full 4-cycle budget. No hard stop; the budget simply ran out.

---

## START HERE: author `0x0052df40`. It is authorable NOW.

iter21 handed over "nothing is authorable". **That was wrong**, and iter22's main job was
finding out why (below). Concretely:

- **`0x0052df40`** — SAFE, **leaf** (`callees_depth1: []`, inline copy loops, no calls), so
  the callee-half of the gate is exempt. Two params, both correctly typed as pointers and
  both literally dereferenced (param_1 at +0x24/+0x2c/+0x4c, param_2 at +0x4c). Returns
  `int` 1. **`ptr_seed_observe` matches with no new handler and no additive field**
  (`ARG_TYPES.md:1597`). Caller-gate satisfied by owner **`0x005515a0` C2** (iter21).
  **NON-DEGENERACY — read before seeding:** it returns the constant 1 on every path, so the
  return value is *not* a discriminator. Evidence must come from the destination buffer, and
  the seed must differ per vector at +0x24/+0x2c/+0x4c, or a port that copies nothing still
  scores GREEN.

Batch it with one or two of these (all need one additive field first, none need a new handler):

- **`0x0052ddc0`** — SAFE leaf, same owner `0x005515a0` C2. Needs `count_header_list_ring`
  (`ARG_TYPES.md:55`) **+ a `count` list_op**. Three alternatives were named and refuted by
  MECHANISM: `audio_list_count` (next@+4, wrong layout), `ptr_arg_int_get` (fills a dword
  pattern — a random buffer is not a valid ring list, so it AVs or spins), `thunk_list_count`
  (list at p+0xc, next at node+4, no stride descriptor).
- **`0x0049ff30`** — SAFE (every write relative to param_1: `*(undefined1*)(param_1+0x95)=1`;
  the Enter/LeaveCriticalSection pair uses the CS *inside the passed struct* at +0x14, not
  live engine state). Needs `struct_call_observe` (`ARG_TYPES.md:1490`) **+ a `cs_init_at`
  additive field** (pre-call `InitializeCriticalSection`, exactly analogous to `stub_at`).
  **This supersedes the mutator lane for this row**: the synthetic lane doesn't care how
  often it is called naturally, so `cs_init_at` sidesteps `mutator_ab_pilot`'s open
  call-frequency question entirely. Prefer it.

Pre-flight (`py -3.12 scripts/orch_preflight.py <hook>...`), then verify all of them in
**ONE** `state_batch` boot.

---

## The finding: the s6 brief could never say READY

**24 fresh RVAs, 4 buckets, 4/4 units OK, $2.78 off-quota — and 24/24 came back
`NEEDS_GHIDRA`. In 22 of 24 the deciding column was `CALLERS_NEEDS_GHIDRA`.**

The READY criterion requires "≥1 caller is C2+". The prompt hands the worker **only** plate
paths plus `ARG_TYPES.md`, and explicitly forbids globbing, grepping `hooks.csv`, or
searching `re/analysis`. **Plates do not name their callers.** The one fact READY depends on
is the one fact the worker is structurally denied — the verdict is decided before the screen
runs.

This retro-explains **all 8 prior s6 briefs** tallying 0 READY, which iters 9–11 recorded as
if the candidate pool were dry. It is a prompt defect, not a fact about the candidates.

**Fix (ledger item `brief_caller_gate_fix`, one prompt change, not a new lane):** resolve
callers + caller confidence orchestrator-side — the same way plate paths already are — and
put them **in the table**. That is one `hooks.csv`/Ghidra lookup by the orchestrator, not 24
read-fleet globs, so it does not re-open the read-only boundary the prompt exists to protect.

**Second, smaller fix:** filter the batch against `hooks.csv` **before** building the queue.
`0x00407550` was screened at C2 while already **C3** (`SubsystemARecordFind`, promoted
iter21), wasting a row. (It did independently re-derive the shipped config —
`esi_global_search`, tgt `0x639d80`, stride `0xec`, `key_off 0x44` — a clean incidental
corroboration of that port.)

**Keep verbatim: the mandatory-refutation clause.** iter22 added "before writing
NEEDS_NEW_HANDLER, state which MECHANISM lines you considered and why each fails; if the gap
is a small knob, call it ADDITIVE_FIELD instead." Result: **0 NEEDS_NEW_HANDLER in 23 of 24
rows**, versus six consecutive prior runs that each invented a handler that already existed.
The sole exception (`0x0049d240`) named and refuted four handlers by mechanism —
`fastcall_reg`, `thiscall_nested_field_get`, `reg_this_call_observe`, `vtable_table_dispatch`
— before claiming a gap (ECX-as-this with a 2-level deref *and* a dynamic vtable stub).
Still confirm against the implementation before writing it.

## Two stale ledger notes reconciled (iter22 cycles 2–3)

1. **`handler_specs_22` was stale → closed as `promoted`.** Its two "AUTHORABLE NOW" rows
   were in fact authored *and* promoted in iter21 and are C3 today: `0x004b6b00`
   `StoreEaxAtEcx` (GREEN 5/5) and `0x00407550` `SubsystemARecordFind` (GREEN 4/4). The
   iter21 handoff named only `0x00407580` + `0x005b1160` and so **undercounted its own
   output** — which is exactly why this run opened believing the ladder was dry.
   **Lesson: the ledger, not the handoff prose, is the source of truth. Reconcile briefed
   notes against `hooks.csv` before declaring anything dry.**
2. **`mutator_ab_pilot`'s "NEXT" was superseded.** It said "screen the remaining 12
   WRITES_GLOBAL rows"; iter14 already screened **all 44** and the answer is **2**, not 12
   (AB_READY 2 / NONDET 7 / UNENUMERABLE 11 / ONESHOT 4 / IRREVERSIBLE 20). Survivors:
   `0x0049ff30` and `0x0045c820`. The only remaining gate is a **runtime** measurement — one
   `MASHED_COUNT_RVAS` boot — not more screening. There is no existing evidence for it:
   `re/analysis/plans/ab_reachable.tsv` is a *static* write-surface survey (columns
   rva / resolved-store-count / call-tree-size, per `scripts/survey_ab_reach.py:79-81`), not
   a call counter. Prefer the `cs_init_at` synthetic route for `0x0049ff30` instead.

## Recurring defect classes confirmed again

- **Pointer param declared `int`**: 6 of 24 this run (`0x00451cc0`, `0x00451730`,
  `0x004722e0`, `0x0049f2e0`, `0x0048fef0`, `0x0049ff30`). Reliable recurrence, not anecdote.
- **Plate-vs-`hooks.csv` confidence conflicts**: 4 genuine ones in `state_util_b1_s6`
  (`0x00495110`, `0x004af32d`, `0x004292d0`, `0x0045d430` — plate C1, table C2). No winner
  picked; route via `re-classify` before any promotion.

## Do NOT re-pick

- `0x00495110` — QPC timer via `FUN_004950b0`; rejected in iter12, re-confirmed twice since.
- The 6 heap-address writers (iter14) — allocation addresses differ per run.
- The 14 NO_OWNER ORPHAN rows via reference-chain BFS — it ran out of edges on them.
- `0x0048fce0` / `0x0048fd10` — both DESTROYS_DEVICE via the same
  `(**(code**)(DAT_007d3ff8+0x20))(8,0)` disabling RW renderstate 8.

## Standing rules (unchanged, all measured)

1. Pre-flight before every boot: `py -3.12 scripts/orch_preflight.py <hook>...`.
2. Author 3–4 rows, verify in ONE `state_batch` boot.
3. `NEEDS_NEW_HANDLER` is a hypothesis, not a fact — check `ARG_TYPES.md` MECHANISM lines,
   prefer an additive defaulted field (`stub_at`, `null_args`, `this_reg:'stack'`, `key_off`,
   `eax_from_test`, `reseed_per_side`).
4. A decompiler summary is not evidence about x87. Read the raw listing.
5. A "absent" harness reading has two causes — absent, or a broken probe — and they look
   identical. Make probes report *why* they failed.
6. `out3_idx` is retired; the harness throws if anything references it.

## Hygiene

- Pool slots **5 and 10** hold leaked `.lock~` (clears only when the MCP JVM restarts).
  **Slots 9 and 11 work.** Slot 4 is a stale broken clone.
- Open the root-level `mashed_pool/Mashed_poolN.gpr` only; the `Mashed_poolN/` subdirectory
  projects are stale 0-byte duplicates.
- iter22 spawned **no** MASHED process and created no worktrees; `original/` untouched.
- Another session is active in this checkout — leave `scenario_launch.py`,
  `PromoLoop_sessionB.cpp`, `UNCERTAINTIES.md`, and
  `re/analysis/bucket_audio_005ab710_005af040/0x005ab980.md` alone.
- **Account2 policy v29 (2026-07-31 15:59)**: adds `allow_routines=False`. No impact on the
  read-fleet — it only disables scheduled/cloud routines, alongside the already-disabled
  Remote Control and Workflows. Read/Grep/Glob via `delegate.ps1` is unaffected.
