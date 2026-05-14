---
name: discover-c1-batch
description: Generate a parallel-fanout batch file of Claude Code prompts for Ghidra-side first-pass work — C0→C1 discovery, C1→C2 mechanical promotion, deferred-row drains, and struct-extraction sessions. Each session is bounded so it finishes before conversation auto-compacts. Triggers on "make a ghidra batch", "generate a ghidra fanout", "plan a c1 batch", "make a discovery batch", "generate first-pass prompts", "drain DEFERRED into a batch", "scribe-side batch".
---

# discover-c1-batch — generate parallel Ghidra-side batch prompts

This skill is **planning-only**. It produces a text file (`batch_<letter>.txt` at project root, matching the existing `batch_h.txt` pattern) containing N self-contained Claude Code prompts. Each prompt drives one parallel Sonnet 4.6 session that decompiles a bucket of RVAs into per-RVA `.md` plates and queues a row in `re/SCRIBE_QUEUE.md`. The follow-up `ghidra-sweep` (Opus) drains the queue into the master Ghidra project.

This is the **counterpart** of `promote-c3-batch`:

| Generator | Output queue | Sweep | Work shape |
|---|---|---|---|
| `discover-c1-batch` | `re/SCRIBE_QUEUE.md` | `ghidra-sweep` (Opus) | Mechanical Ghidra decomp transcription (C0→C1, C1→C2). No build, no Frida. |
| `promote-c3-batch`  | `re/PROMOTION_QUEUE.md` | `frida-sweep` (Sonnet) | Hook authoring + Frida diff (C2→C3). Build + diff per session. |

## When to invoke

- "Make a ghidra batch" / "generate the next batch"
- "Plan a c1 fanout" / "plan another discovery batch"
- "Drain DEFERRED into a batch"
- After previous batch's `ghidra-sweep` completes and the user wants to keep going.

Do NOT invoke this skill when:
- The user wants C3 promotions — that's `promote-c3-batch`.
- The user wants the sweep to run — that's `ghidra-sweep` (this skill only *generates* prompts; it doesn't run them).
- A previous batch's sweep hasn't completed yet — overlapping sessions on the same RVAs cause drift.

## Inputs

The skill needs to know:
- **N sessions** (**default 6** — the standing batch shape; cap at 8 if the user explicitly asks).
- **Max RVAs per session** (default 20; hard cap 24 — see § "Why 20 / 24" below).
- **Work types** (any mix of):
  - `first-pass` — pull C0 functions from hooks.csv or Ghidra `function_list` not yet mapped. The bread-and-butter case.
  - `cont` — drain DEFERRED.md rows tagged with a specific bucket-cont1/2/3 (the small-cluster mop-up case, like `batch_h.txt` sessions 43-47).
  - `promote-c2` — promote C1 → C2 by reading decomp end-to-end and writing mechanical-description plates (e.g., `audio_promote_c2_rws_loader` from 2026-05-12).
  - `struct` — pure documentation session, no RVA cap (see batch_h session 48 as the template).
- **Subsystem filter** (optional; e.g., `frontend,hud`). Defaults to "whatever DEFERRED.md has most of."
- **Pool slot pre-assignment** — each session gets a distinct `Mashed_poolN` slot to prevent two sessions writing the same Ghidra clone. The skill assigns slots 0..N-1 by default; if some slots are known-locked, skip them.

**Standard batch shape: 6 sessions × 20 RVAs = ~120 RVAs per batch.** This matches `batch_h.txt`. Do not silently shrink unless candidates run low — report the shortfall instead.

## Why 20 / 24 (the per-session cap)

Each Ghidra-side first-pass C0→C1 promotion costs the worker session roughly **3–5k tokens**: open the function in its assigned pool slot, pull decomp via MCP, transcribe the mechanical description into a .md file, cite constants, file any U-IDs. Much lighter than C3 work because there's no build, no Frida diff, no source code authoring.

Sonnet 4.6 working budget ≈ 150k tokens before compact. With ~12k of fixed startup context (CLAUDE.md + this skill's prompt + SESSION_RULES.md sections + initial DEFERRED.md inspection), a session has ≈138k usable:

- **20 RVAs** = 70-100k → 82-112k total → ~30-50k headroom (preferred default; matches batch_h.txt usage and proven across many batches)
- **24 RVAs** = 84-120k → 96-132k → ~5-15k headroom (hard cap; risky if any RVA expands to >5k due to large decomp + many callees)

Beyond 24, compaction during MCP work is nearly guaranteed, which is bad: a compacted session loses bucket context and can write inconsistent plates. The cap is firm.

For **promote-c2** work (deeper analysis per function), cap drops to **15 RVAs**. C1→C2 is heavier per function (full decomp read, struct-offset chasing, drift-correction of stale rows). 15 keeps the budget comfortable.

For **struct** sessions: no RVA cap. The work is markdown authoring + grep across existing plates. Cap is implicit (a session naturally finishes after 3-5 struct docs).

## Model declaration (per session)

| Role | Model | Reason |
|---|---|---|
| Per-session first-pass worker | **Sonnet 4.6** (`claude-sonnet-4-6`) | Mechanical transcription. Sonnet handles MCP+decomp+plate-writing cleanly. |
| Per-session promote-c2 worker | **Sonnet 4.6** | Same — promotion is still mechanical, just deeper. Use Opus only if the function exceeds 200 lines of decomp and has cross-subsystem implications. |
| Per-session struct-extract worker | **Sonnet 4.6** | Mostly grep + markdown. Opus is wasted. |
| End-of-batch sweep | **Opus 4.7** | Drives `ghidra-sweep` — master Ghidra writeback, last-writer-wins, correctness > speed. |

Write the model id verbatim into each session block (`Model: Sonnet 4.6 (claude-sonnet-4-6) — hard cap N RVAs`).

## Candidate selection algorithm

The skill pulls candidates from several sources depending on requested work type:

### first-pass
1. `hooks.csv` rows with `confidence=C0` and analysis_file=empty:
   ```bash
   awk -F',' 'NR>1 && $4=="C0" && $6=="" {print $1","$2","$3}' hooks.csv
   ```
2. Or `function_list` from Ghidra MCP minus already-mapped RVAs (only if `hooks.csv` C0 set is exhausted).
3. Filter for "has at least one xref" (drop dead code / unreferenced stubs).
4. Cluster by xref proximity — RVAs in the same call tree go to the same session so the bucket name reflects a coherent cluster.

### cont
1. Grep `DEFERRED.md` for the specified bucket-cont tag:
   ```bash
   grep -E '\<bucket-name\>-cont[0-9]+' DEFERRED.md
   ```
2. Pull the RVAs out of those rows.
3. If a single DEFERRED row contains > 24 RVAs (cap-split case), allow it to span TWO sessions of 20 each; the rest stays deferred to a future batch.

### promote-c2
1. `hooks.csv` rows with `confidence=C1` in the requested subsystem.
2. Drop ones whose analysis note has only structural placeholders (need actual decomp reading).
3. Drop ones whose hooks.csv row was added < 7 days ago (the C1 plate is still fresh — wait for it to "settle" before pushing to C2).
4. Cluster by Ghidra address range (sessions get contiguous 0x1000-range buckets when possible).

### struct
1. Read `re/analysis/structs/REPORT_*.md` files for known-missing structs.
2. Or accept the user's explicit list of struct names.
3. Cross-reference S-DoD requirement #3 from `ROADMAP.md` — prioritize structs that gate subsystem S-DoD.

## Ghidra pool slot binding

Each session block pre-declares a Ghidra pool slot. The skill assigns them 0..N-1 (or N-1..N-K skipping known-locked slots — read `mashed_pool/Mashed_pool*.lock` to check).

```
Pool slot:   Mashed_pool3 (pre-assigned — do not call ghidra_pool.sh acquire)
```

Sessions consume their pre-assigned slot directly. If the slot turns out to be stale-locked, the session falls back to `ghidra_pool.sh acquire` and records the actual slot in its queue row. Pre-assignment is a hint, not a contract.

**No worktrees** for Ghidra-side fanout. Git isn't the conflict surface — Ghidra's `.gpr` locking is, and per-session `.md` files in distinct bucket dirs don't conflict. The only shared file each session touches is `re/SCRIBE_QUEUE.md` (append-only Queued section); merges of "two sessions appended different rows" usually go clean, and the sweep handles cleanup either way.

## Session block template

```
================================================================================
Session <N> — <bucket>  (<WORK-TYPE>, <K>-RVA <drain|sweep>)
================================================================================

Pool slot:   Mashed_pool<S> (pre-assigned)
Model:       Sonnet 4.6 (claude-sonnet-4-6) — hard cap <CAP> RVAs (this session: <K>)
Skill used:  parallel-fanout first-pass  |  parallel-fanout promote-c2  |  struct-extract
Bucket dir:  re/analysis/<bucket>/
Drains to:   re/SCRIBE_QUEUE.md (Queued); ghidra-sweep picks up later

# Mission
<One paragraph: what these RVAs represent, why they cluster, what the
session produces. For cont-drains, cite the DEFERRED.md D-IDs being
closed.>

# Inputs
- DEFERRED.md rows: <list of D-IDs or "none">
- hooks.csv rows: <RVAs being added or updated>
- Adjacent buckets to cross-reference: re/analysis/<other-bucket>/

# Workflow
1. Startup with $SLOT=Mashed_pool<S>. If the slot is stale-locked, fall
   back to `bash scripts/ghidra_pool.sh acquire` and record the actual slot.
2. Pull the K RVAs listed in § "Candidates" below. Drift-skip any already
   in hooks.csv with confidence >= the target (the bucket is stale).
3. For each RVA:
   a. mcp__ghidra__function_at <rva> (or listing_code_unit_at if no function)
   b. mcp__ghidra__decomp_function for the body
   c. Write re/analysis/<bucket>/<rva>.md with frontmatter (rva, name,
      size_bytes, confidence target, callees_depth1, callers_noted,
      opened_in_slot, session_date) + `## Mechanical description` bullets
      + `## Constants` table + `## Uncertainties` (file U-IDs) + `## Stubs
      encountered` (file S-IDs).
4. After all RVAs done:
   - Append a row to re/SCRIBE_QUEUE.md in the Queued section, format per
     re/SESSION_RULES.md § "Parallel-fanout scribe-queue pattern".
   - Resolve DEFERRED.md D-IDs that this bucket closes.
   - Commit per the session's commit policy (one commit at end, not per RVA).

# Candidates
| RVA        | Source (D-ID / hooks.csv / etc.) | Notes |
|------------|----------------------------------|-------|
| 0x00xxxxxx | <source>                         | <note> |
| ...        | ...                              | ...   |

# Output
- <K> per-RVA .md files in re/analysis/<bucket>/
- <K> new (or updated) C0->C1 / C1->C2 row entries authored to hooks.csv
- Drained D-rows resolved (if applicable)
- Queued row in SCRIBE_QUEUE.md
- Optional: NEW struct doc in re/analysis/structs/<name>.md if the bucket's
  cross-references make a struct shape obvious (>= 3 callers agree on
  offsets, widths, signs).

# STOP-AND-ASK if
- A candidate's Ghidra `function_at` returns null AND `listing_code_unit_at`
  also returns null — surface the missing RVA and skip.
- A bucket turns out to be a library function family (memcpy, _ftol2, RW
  primitive) — note the library match in the plate, do NOT rename in this
  session (rename is a master-Ghidra mutation reserved for the sweep).
- A candidate's signature is too ambiguous to even mechanically transcribe
  (e.g., decompiler renders parameters as `undefined4` but xrefs imply
  pointer-to-struct) — write the plate with `[UNCERTAIN]` markers and file
  a U-ID; do not block the rest of the bucket.
```

For **struct** sessions, omit the `# Candidates` table and the per-RVA workflow steps; substitute the batch_h session-48 template (markdown authoring + grep across plates).

## Output file naming

- Generated by this skill: `batch_<letter>.txt` at project root.
- Letter assignment: next unused letter (`a`, `b`, `c`, …). The current high-water mark is **h** (per `batch_h.txt` present in repo root).
- The next batch generated would be `batch_i.txt`. Read the project root to find the current highest letter and increment.
- C3 batches use the *different* namespace `c3_batch_*.txt` (per `promote-c3-batch`). Don't collide.

## Anti-patterns the skill must reject

- More than 24 RVAs per first-pass session, or 15 per promote-c2 session — compaction risk.
- Mixing work types within one session (a session is first-pass OR promote-c2 OR struct; never two).
- Pre-assigning the same Ghidra pool slot to two sessions — instant collision.
- Generating prompts that mutate the master Ghidra project — that's the sweep's job. Sessions only write to per-RVA `.md` files in their bucket dir.
- Generating a batch while a previous batch's `ghidra-sweep` is still running (master is in mid-write) — the skill should warn and ask the user to confirm.
- Recommending Opus for a routine batch — Sonnet handles first-pass fine; reserve Opus for the sweep and the genuinely hard cases.

## Default batch + scale guidance

**Production default: N=6, K=20 first-pass.** Wall time ~1-2 hours of human-attended parallel Claude work. ~120 RVAs per batch. Run as fast as candidates exist.

Mashed has ~5,800 functions. Roughly half (~2,900) are still C0 or unmapped (snapshot 2026-05-12). At 120 per batch, that's ~24 batches to drain the C0 backlog — a multi-month effort, which is the realistic Phase 1/2 completion horizon.

**Always run `ghidra-sweep` between batches.** Sessions in batch N+1 should not be sequenced against an undrained SCRIBE_QUEUE from batch N — the master Ghidra project hasn't received batch N's plates yet, so batch N+1 sessions can't cross-reference them.

## Relationship to other skills

- **ghidra-pool** — sessions pre-assigned slots from the pool; skill reads `mashed_pool/Mashed_pool*.lock` to skip locked slots during assignment.
- **ghidra-sweep** — drains the rows this batch's sessions queue. Always run after a batch.
- **promote-c3-batch** — counterpart on the Frida side. Same fanout pattern, different domain.
- **multi-session** — etiquette for parallel sessions; this skill's batch is one form of multi-session work.
- **re-classify** — sessions invoke it inline for their own per-RVA C0→C1 / C1→C2 promotions. The sweep does NOT promote.
