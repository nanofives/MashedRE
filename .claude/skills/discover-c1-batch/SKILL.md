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

Beyond 24 on Sonnet, compaction during MCP work is nearly guaranteed, which is bad: a compacted session loses bucket context and can write inconsistent plates. The cap is firm for Sonnet.

For **promote-c2** work (deeper analysis per function), cap drops to **15 RVAs**. C1→C2 is heavier per function (full decomp read, struct-offset chasing, drift-correction of stale rows). 15 keeps the budget comfortable.

For **struct** sessions: no RVA cap. The work is markdown authoring + grep across existing plates. Cap is implicit (a session naturally finishes after 3-5 struct docs).

### Opus 1M caps (C1 brute-force mode, 2026-05-18)

When the user passes `--model opus` (or the work shape is "C1 brute-force"), the per-session cap rises:

- **first-pass on Opus 1M: 80 RVAs (hard cap 100)** — Opus 1M has ~6× Sonnet's working budget. 80 RVAs × 4k tokens per RVA = 320k → with ~30k of fixed startup context, total ≈350k → ~650k headroom remains, well clear of compaction. The hard cap of 100 leaves ~500k headroom for cross-RVA shared-context buildup (typical when clustering by call-graph proximity).
- **promote-c2 on Opus 1M: 60 RVAs (hard cap 80)** — heavier per-RVA cost (~6k tokens) so a more conservative ratio.
- **struct on Opus 1M: no cap** — same as Sonnet; the work is naturally session-bounded.

**Standard batch shape on Opus 1M: 6 sessions × 80 RVAs = 480 RVAs per batch.** This is ~4× the Sonnet cadence (120/batch). At this rate, the ~2,500-RVA "real game code" residue (after Levers 1+3 pre-filter — see "Pre-filter levers" below) drains in 5–6 batches.

**Cost calibration.** Opus 4.7 input/output rate is ~5× Sonnet 4.6. But because Opus reads the bucket context once and amortizes across 80 RVAs instead of 20, per-RVA token cost is only ~1.5–2× Sonnet's. Net: Opus 1M C1 brute-force runs roughly 2× the wall-clock-equivalent throughput for ~1.6× the cost-per-RVA — favorable when the goal is "drain the C0 pool in weeks not months."

**When NOT to use Opus 1M:** small DEFERRED-drain sessions (cont-buckets with <20 RVAs), promote-c2 work that touches < 30 candidates, or any session where the per-RVA cost is genuinely Sonnet-tractable. The Opus 1M shape is for **bulk drain** of a large pool; small targeted sessions stay on Sonnet.

### Pre-filter levers (run BEFORE any Claude session)

Before generating a brute-force batch, drain the trivial cases mechanically so the worker sessions only see "real game code":

1. **Lever 1 — RW string-anchor sweep.** `re/tools/rw_string_anchor_scan.py` pulls all `Rw*`/`Rt*`/`Rp*`/`rwID_*`/`_rw*`-shaped strings from MASHED.exe rdata, xrefs each via Ghidra MCP, and emits a proposal TSV the user reviews before any function-rename writeback. Expected yield: 80–150 functions named.
2. **Lever 2 — RW signature match against librw / gta-reversed-modern.** `re/tools/rw_signature_match.py` hashes the first-N opcodes of each `Rw*`/`Rp*` function in `re/prior_art/renderware/{librw,gta-reversed-modern}` and brute-greps MASHED.exe `.text` for matches. Proposal-output only. Expected yield: 300–500.
3. **Lever 3 — Ghidra FidDB library-tag drain.** One dedicated session that bulk-renames any function with a `Library Function:` tag in Ghidra. Pure MCP scripting; no decomp reading per RVA. Expected yield: 200–400 CRT renames.

Running all three drops the brute-force pool from ~3,800 unmapped to ~2,500–2,800 of real game code. Levers 1+2 are read-only proposals you review before any Ghidra writeback; Lever 3 is bulk-rename, deferred to its own sweep.

## Model declaration (per session)

| Role | Model | Reason |
|---|---|---|
| Per-session first-pass worker | **Sonnet 4.6** (`claude-sonnet-4-6`) | Mechanical transcription. Sonnet handles MCP+decomp+plate-writing cleanly. |
| Per-session promote-c2 worker | **Sonnet 4.6** | Same — promotion is still mechanical, just deeper. Use Opus only if the function exceeds 200 lines of decomp and has cross-subsystem implications. |
| Per-session struct-extract worker | **Sonnet 4.6** | Mostly grep + markdown. Opus is wasted. |
| End-of-batch sweep | **Opus 4.7** | Drives `ghidra-sweep` — master Ghidra writeback, last-writer-wins, correctness > speed. |

Write the model id verbatim into each session block (`Model: Sonnet 4.6 (claude-sonnet-4-6) — hard cap N RVAs`).

### Dispatch via project agents (2026-07-01 — token-economy routing)

When the batch (or any slice of it) is executed by **dispatching subagents from
an orchestrating session** via the Agent tool — instead of human-pasted parallel
sessions — do NOT use default/general-purpose agents. Dispatch the project agent
types defined in `.claude/agents/` (each pins its model; do not override it):

| Work shape | `subagent_type` | Pinned model |
|---|---|---|
| first-pass / cont bucket worker | `scribe-transcriber` | sonnet |
| bucket where ALL candidates are pure leaves (`callees_depth1: []`) | `leaf-decoder` | sonnet |
| promote-c2 worker | `scribe-transcriber` | sonnet |
| struct-extract worker | `scribe-transcriber` | sonnet |
| log / post-mortem tallies (batch yield counts, sweep verdicts) | `log-parser` | haiku |
| central SCRIBE_QUEUE / DEFERRED / hooks.csv row mutations | `tracker-editor` | sonnet |

The agent definitions already carry the standing guardrails (no `original/`
writes, worktree removal only via `diag.py wt-remove`, targeted git adds, no
invented arg_types — `re/frida/ARG_TYPES.md` is the lookup), so Agent-tool
dispatch prompts don't need to restate them. Session blocks emitted for
human-pasted sessions still must.

## Candidate selection algorithm

The skill pulls candidates from several sources depending on requested work type:

### first-pass
1. `hooks.csv` rows with `confidence=C0` and analysis_file=empty:
   ```bash
   awk -F',' 'NR>1 && $4=="C0" && $6=="" {print $1","$2","$3}' hooks.csv
   ```
2. Or `function_list` from Ghidra MCP minus already-mapped RVAs (only if `hooks.csv` C0 set is exhausted).
3. **Filter out candidates in known library bands.** The MSVC CRT static-linkage residue lives at `0x004a0000..0x004b3fff` (calibrated against batch_s session 1: the bucket `0x004b0068..0x004b3a60` turned out to be 100% CRT, with 29 of 60 RVAs already attesting FidDB names in Ghidra); the qhull/RW-Physics island lives at `0x0057c5b0..0x005a5820`. See § "Library-band exclusion" below for the full table + recipe. Drop candidates in these ranges from first-pass buckets entirely.
4. Filter for "has at least one xref" (drop dead code / unreferenced stubs).
5. Cluster by xref proximity — RVAs in the same call tree go to the same session so the bucket name reflects a coherent cluster.

### cont
1. Grep `DEFERRED.md` for the specified bucket-cont tag:
   ```bash
   grep -E '\<bucket-name\>-cont[0-9]+' DEFERRED.md
   ```
2. Pull the RVAs out of those rows.
3. If a single DEFERRED row contains > 24 RVAs (cap-split case), allow it to span TWO sessions of 20 each; the rest stays deferred to a future batch.

### promote-c2
1. `hooks.csv` rows with `confidence=C1` in the requested subsystem.
2. **Library-band exclusion (do this BEFORE clustering — see § "Library-band exclusion").** promote-c2 pulls by subsystem label, but vendored-library functions are routinely MISLABELED (`util`, `render`, `audio`) in hooks.csv until reclassified — so a subsystem filter alone does NOT catch them. Drop a candidate if EITHER:
   - its subsystem already starts with `third-party-library[` (already attested — never hand-promote), OR
   - its address falls in a known vendored-library band (qhull/RW-Physics `0x0057c5b0..0x005a5820`, D3DX9 PSGP `0x004ec000..0x004fc9e0`, CRT `0x004a0000..0x004b3fff`) REGARDLESS of its hooks.csv subsystem label.
   These belong in a library-tag + FidDB drain, not a hand-plate C1→C2 session. (batch_ag-s5/s6 burned 110 of 360 RVAs on the qhull island before this rule existed — s6 full-halted, s5 wasted 50 plates.)
3. Drop ones whose analysis note has only structural placeholders (need actual decomp reading).
4. Drop ones whose hooks.csv row was added < 7 days ago (the C1 plate is still fresh — wait for it to "settle" before pushing to C2).
5. Cluster by Ghidra address range (sessions get contiguous 0x1000-range buckets when possible).

### struct
1. Read `re/analysis/structs/REPORT_*.md` files for known-missing structs.
2. Or accept the user's explicit list of struct names.
3. Cross-reference S-DoD requirement #3 from `ROADMAP.md` — prioritize structs that gate subsystem S-DoD.

## Library-band exclusion (CRT + vendored libraries)

Several address ranges in MASHED.exe are almost entirely Ghidra-FidDB-attested / vendored library code (MSVC CRT, qhull-2002.1 via RenderWare Physics, D3DX9 PSGP, etc.). Spending a worker session "discovering" or hand-plating these is wasted budget — Ghidra already knows, and the functions are often still MISLABELED (`util`/`render`/`audio`) in hooks.csv, so a subsystem filter alone won't catch them. **Exclude by ADDRESS BAND. Applies to both `first-pass` and `promote-c2`.**

**Calibrated library bands:**

| Range | Library | Source / note |
|---|---|---|
| `0x004a0000..0x004b3fff` | MSVC CRT static-linkage residue | batch_s s1 hit 100% CRT in `0x004b0068..0x004b3a60` (29/60 had FidDB names); real game code resumes ~`0x004b4000` |
| `0x0057c5b0..0x005a5820` | qhull-2002.1 (vendored via RenderWare Physics 3.7) | ~165KB island; provenance string @0x005e5f58; batch-t-s5/v-s5/y-s6/z-s1/ag-s6 all HALTED here; routinely mislabeled `util` in hooks.csv. See `memory/project_qhull_rwphysics_island.md` |
| `0x004ec000..0x004fc9e0` | D3DX9 PSGP (Microsoft, statically linked) | ~67KB / 93 RVAs; SSE/SSE2/3DNow! dispatch table @FUN_004fbe7a. See `memory/project_d3dx9_psgp_band.md` |

**Pre-filter recipe.** Before assigning RVAs to first-pass OR promote-c2 sessions:

1. **Range short-circuit.** Any candidate in a calibrated library band above is suspect. Drop unless the user has explicitly asked to "drain a library band" (a different work shape: bulk-rename + library-tag, not C0→C1 / C1→C2 hand-plating).
2. **Ghidra FidDB attestation check.** For each remaining candidate, hit Ghidra MCP:
   ```
   mcp__ghidra__function_at(<rva>)   # returns the function record incl. signature + tags
   ```
   If the response includes a name matching any library prefix (CRT: `^(___|__|_crt|_strncpy|_strcpy|_strcmp|_strlen|_memcpy|_memset|_memmove|_ftol|_fdiv|_alloca|_sprintf|_vsprintf|_setjmp|_longjmp)`; qhull: `^qh_?` or contains `qhull`) OR a `Library Function: <foo>` tag — exclude. The candidate is already classified by FidDB; running a worker session on it is duplicate work.
3. **Bucket veto.** If after applying steps 1+2, more than **30%** of a proposed bucket falls in any library band, refuse the bucket and re-pick from a different address range. A library-dense bucket cannot be salvaged by trimming individual RVAs — the xref cluster itself is library code.

These library bands are allowed to be the subject of a dedicated "library-tag drain" session (different work shape), but those prompts come from a separate generator, not this skill.

## Subsystem prediction is best-effort

Subsystem labels assigned at batch-generation time are educated guesses based on adjacent code; sessions routinely discover the actual subsystem mid-decomp. The 2026-05-17 batch_s post-mortem showed 3 of 4 first-pass buckets reclassified their subsystem during the run:

- audio → shader-compiler
- audio → csl-pipeline
- audio → bimodal-qhull + mixer split

**Rules for the generator:**

1. **Do not hard-claim a subsystem in the session prompt.** Phrase the bucket as "cluster anchored at RVA X" or "first-pass bucket at 0x00xxxxxx", not "audio bucket".
2. **Provide the working hypothesis as a *hint*, not as a label.** Example session-block prose: "Working hypothesis based on adjacent xrefs: audio. Confirm or reclassify during decomp; report reclassification in the SCRIBE_QUEUE row's `note=` field."
3. **Require sessions to report reclassification.** The session's SCRIBE_QUEUE row must include `subsystem_observed=<X>` if it differs from the bucket's nominal subsystem. ghidra-sweep will reconcile.
4. **Do not auto-rename the bucket directory** based on the hypothesis. Use a neutral name (e.g., `re/analysis/bucket_004b4000/`) and let ghidra-sweep rename to the observed subsystem after the queue drains.

## Ghidra pool slot binding

Each session block pre-declares a Ghidra pool slot. The skill assigns them 0..N-1 (or N-1..N-K skipping known-locked slots — read `mashed_pool/Mashed_pool*.lock` to check).

```
Pool slot:   Mashed_pool3 (pre-assigned — do not call ghidra_pool.sh acquire)
```

Sessions consume their pre-assigned slot directly. If the slot turns out to be stale-locked, the session falls back to `ghidra_pool.sh acquire` and records the actual slot in its queue row. Pre-assignment is a hint, not a contract.

### `.pool_slot` collision in parallel-fanout (2026-05-17 batch_s s2)

batch_s session 2 reported a collision on the project-root `.pool_slot` marker file: two parallel sessions on the same checkout overwrote each other's slot record, and the `ghidra-pool` assert (`recorded slot matches owned slot`) tripped on whichever lost the race.

Two acceptable fixes; the generator must apply **one** of them in every session block:

1. **Per-session `.pool_slot` naming (preferred).** Each session block sets `POOL_SLOT_FILE=.pool_slot_session_<id>_s<N>` and exports it before invoking `ghidra_pool.sh` / `ghidra-pool` skill. The skill consumes the env var if set; falls back to `.pool_slot` otherwise. Recipe in the session block:
   ```bash
   export POOL_SLOT_FILE=".pool_slot_session_<batch-id>_s<N>"
   ```
2. **Slot-match assert exemption.** If the session block declares `PARALLEL_FANOUT=1` in its env, the ghidra-pool skill's slot-match assert is downgraded to a warning. Recipe:
   ```bash
   export PARALLEL_FANOUT=1
   ```

Generator preference is option (1); it's more deterministic and doesn't weaken the assert. Use option (2) only when sessions explicitly need to share a working directory (rare for discover-c1-batch, since each session has its own bucket directory anyway).

**No worktrees** for Ghidra-side fanout. Git isn't the conflict surface — Ghidra's `.gpr` locking is, and per-session `.md` files in distinct bucket dirs don't conflict. The only shared file each session touches is `re/SCRIBE_QUEUE.md` (append-only Queued section); merges of "two sessions appended different rows" usually go clean, and the sweep handles cleanup either way.

## Session block template

```
================================================================================
Session <N> — <bucket>  (<WORK-TYPE>, <K>-RVA <drain|sweep>)
================================================================================

Pool slot:   Mashed_pool<S> (pre-assigned)
Model:       Sonnet 4.6 (claude-sonnet-4-6) — hard cap <CAP> RVAs (this session: <K>)
Agent type:  scribe-transcriber (leaf-decoder if the bucket is pure leaves) —
             applies when dispatched via the Agent tool instead of a pasted session
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
