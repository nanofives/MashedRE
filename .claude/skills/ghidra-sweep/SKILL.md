---
name: ghidra-sweep
description: Run the parallel-fanout SCRIBE sweep — drain all "Queued" rows in re/SCRIBE_QUEUE.md into the master Ghidra project. This is the follow-up Opus session that runs after one or more parallel Ghidra-discovery sessions have queued their buckets. Triggers on "run the ghidra sweep", "run the scribe sweep", "drain the scribe queue", "process the scribe queue", "scribe the queued sessions", and legacy "run sweep" / "run the sweep" when only SCRIBE_QUEUE.md has work. Counterpart: frida-sweep (drains PROMOTION_QUEUE.md after C3 batches).
---

# ghidra-sweep — SCRIBE_QUEUE drain session

The Ghidra sweep is a single Opus session that drains every "Queued" row in `re/SCRIBE_QUEUE.md` into the master Ghidra project in one serial pass. It replaces the per-session scribe step that parallel-fanout sessions are not allowed to run directly (see `re/SESSION_RULES.md` § "Parallel-fanout scribe-queue pattern").

**Not interchangeable with `frida-sweep`** (the merge-and-verify sweep that drains `re/PROMOTION_QUEUE.md` after a C3 batch). They write to different queues, use different tooling, and never overlap. If both queues have work, run `ghidra-sweep` first (its outputs may unblock C3 candidates), then `frida-sweep`.

**Recommended model:** `claude-opus-4-7` at medium thinking. Reason: last-writer-wins on master; correctness matters more than speed.

## Commit policy (always commit, never ask)

When the user invokes the sweep, **assume no other session is running in parallel**. The sweep owns the working tree for its duration. Every commit step in this skill (claim, per-bucket drain, release) must be executed without asking for confirmation. The default "ask before commit" reflex does not apply here — the sweep protocol is *defined* by its commit sequence, so skipping or deferring a commit breaks the protocol.

Concretely:
- Never ask "should I commit?" or "want me to commit these changes?" at any point during the sweep.
- Stage and commit per the steps below as soon as each step's prerequisites are met.
- If a commit fails for a real reason (hook error, conflict), surface the error verbatim and halt — but do not interpret "do you want me to commit?" as a valid recovery path.
- The only thing that halts the sweep mid-flight is an MCP failure, an anchor mismatch, a pre-existing `master.WIP-*` flag, or an unresolvable git error. None of those are commit-confirmation prompts.

## Pre-conditions (check before starting)

1. **No live fanout sessions.** Verify that no `mashed_pool/Mashed_pool*.lock` files exist for slots referenced by Queued rows. If any slot is still locked, halt — the analysis session may still be running.
2. **git pull --rebase** to ensure all queued rows are visible locally.
3. **No existing `master.WIP-*` flag.** Run `bash scripts/ghidra_assert.sh scribe-check`. Flag present → halt, surface the holder to the user, do not proceed.
4. **Anchor check.** `sha256sum original/MASHED.exe` must equal `BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`. Mismatch → halt.
5. **MCP liveness.** `mcp__ghidra__health_ping` must succeed before opening master.

## Session ID

Construct once at session start, reuse everywhere:

```bash
SESSION_SHORT_ID="sweep-$(date -u +%Y%m%d-%H%M)"
# Example: sweep-20260506-2157
```

## HOLD rows

Any Queued row that has a `HOLD=<reason>` tag is **skipped silently** — do not attempt to drain it. Report it in the end-of-session output under "Skipped (HOLD)". The user resolves HOLD rows manually.

## Sweep execution

The master write is **scripted**, not a per-RVA MCP loop. Two helpers carry it:
- `scripts/sweep_build_manifest.py` (Phase A, local/filesystem only) — turns the
  queued rows + plate `.md` files into a nested-by-bucket `sweep_manifest.json`.
- `scripts/ghidra/sweep_apply_eval.py` (Phase B) — a `ghidra_eval` module that
  applies the entire manifest to master in ONE transaction (plate + bookmark +
  conservative rename + C0 fallback), returning a structured result.

This collapses the old ~5-round-trips-per-RVA chatter (which made a 300-RVA sweep
take minutes against the shared, single-writer master) into one in-JVM pass. The
sweep stays SOLO — master is single-writer and the MCP server serializes, so the
fix is fewer round-trips, not parallel writers. (Superseded prototype:
`scripts/ghidra/BulkPlateBookmark.py`, which used the pre-12 `CodeUnit.*_COMMENT`
API and `0x*.md`-only globbing.)

```
1. git pull --rebase
2. bash scripts/ghidra_assert.sh scribe-check           # must be clear
3. echo "master.WIP-$SESSION_SHORT_ID" > "master.WIP-$SESSION_SHORT_ID"
   git add "master.WIP-$SESSION_SHORT_ID"
   git commit -m "scribe: claim $SESSION_SHORT_ID"
4. Append to re/analysis/CHANGELOG.md:
   "<YYYY-MM-DD>  $SESSION_SHORT_ID  scribe-claim  buckets=<N queued, M skipped-HOLD>"
5. mcp__ghidra__project_program_open_existing
     project_location="C:/Users/maria/Desktop/Proyectos/Mashed"
     project_name="Mashed"  program_name="MASHED.exe"  read_only=false
   → capture the returned session_id for steps 6/8/9/11.
6. mcp__ghidra__health_ping → must succeed; the open result's program_path must be
   "/MASHED.exe" (NOT a "pool" path — a pool path means a clone, not master).
7. BUILD THE MANIFEST (local, no Ghidra):
     py -3.12 scripts/sweep_build_manifest.py --queue re/SCRIBE_QUEUE.md \
       --out sweep_manifest.json --date <YYYY-MM-DD> --level <C1|C2>
   (or --rows <fragment files...> when draining author-only fanout fragments
   that are not yet lifted into SCRIBE_QUEUE.md). Confirm the printed per-bucket
   plated counts == the queued rva counts and missing_md == 0 BEFORE applying.
   The builder resolves each RVA to its plate via the plate's frontmatter `rva:`
   (filename-stem fallback — survives the 0x/bare-hex inconsistency), extracts
   the first chunk of "## Mechanical description", truncates ≤120 chars at a word
   boundary, and prefixes "[<level> <date>] ". Never paraphrases.
8. APPLY THE WHOLE BATCH IN ONE TRANSACTION via mcp__ghidra__ghidra_eval — this
   collapses the legacy ~5-MCP-round-trips-per-RVA loop into a single in-JVM
   pass (hundreds of RVAs go from minutes to seconds):
     _ns = {}
     exec(open(r"<repo>/scripts/ghidra/sweep_apply_eval.py", encoding="utf-8").read(), _ns)
     _ = _ns["run"](sessions, "<session_id>", r"<repo>/sweep_manifest.json")
   Bind the result to `_` — ghidra_eval returns context["_"] in (multi-statement)
   exec mode. The apply IS the protocol, server-side: PLATE comment
   (CommentType.PLATE), "NOTE" bookmark (category = manifest bm_category, comment
   "<level> <date> session:<bucket>"), C0 listing-level fallback when
   function_at is null (no bookmark), and a conservative rename only when the
   manifest entry carries an explicit `rename` OR a pre-existing single-line
   "Library Function: <sym>" attestation, AND the name still starts with FUN_.
   The batch commits the transaction only if totals.errors == []; any per-RVA
   error rolls the WHOLE transaction back (no partial master state).
   → If totals.errors is non-empty: HALT, surface them verbatim, do NOT save.
     Fix the manifest/plates and re-run step 8 (setComment/setBookmark are
     idempotent, so a clean re-run is safe).
   → Spot-check 2–3 plates with mcp__ghidra__comment_get (type=plate) before saving.
9. mcp__ghidra__program_save              # NOT program_save_as
10. Record the drain. Because the apply is ONE atomic transaction, a single
    commit covering all buckets is correct (the legacy per-bucket commit was a
    crash-recovery checkpoint for the serial loop, which no longer exists). For
    each drained bucket append to re/analysis/CHANGELOG.md:
       "<YYYY-MM-DD>  $SESSION_SHORT_ID  scribe-release  bucket=<bucket>  writes=<N>  errors=0"
    and move its row in re/SCRIBE_QUEUE.md from "## Queued" to "## Drained",
    appending  drained-by=<SESSION_SHORT_ID>; <N> plates, <N> bookmarks,
    <renames> renames  (note any C0-listing RVAs). Then commit with TARGETED
    adds — never `git add -A` (a concurrent session may hold unrelated edits):
       git add re/SCRIBE_QUEUE.md re/analysis/CHANGELOG.md re/analysis/<bucket dirs>
       git commit -m "scribe: drained <batch> (<K> buckets / <N> plates) via sweep $SESSION_SHORT_ID"
11. mcp__ghidra__program_close
12. bash scripts/ghidra_pool.sh sync     # tolerate "device busy" on a held clone — non-fatal
13. rm "master.WIP-$SESSION_SHORT_ID"  (it is .gitignored here; if tracked, git rm)
    Append to re/analysis/CHANGELOG.md:
    "<YYYY-MM-DD>  $SESSION_SHORT_ID  scribe-release  buckets=<N drained>  errors=0"
    git add re/analysis/CHANGELOG.md
    git commit -m "scribe: release $SESSION_SHORT_ID"
```

## C0 fallback (RVA not a Ghidra function object)

When `mcp__ghidra__function_at <rva>` returns null, the code unit may exist as a data/listing item. Before skipping:

1. Try `mcp__ghidra__listing_code_unit_at <rva>`. If that returns non-null, write the plate via `comment_set` at the listing address (this is a valid C0 write — plate without a function object). Skip bookmark_add (no function to bookmark). Note in the CHANGELOG row: `<rva> written as listing-level (no Ghidra function object — C0 plate)`.
2. If listing_code_unit_at also returns null: surface the null to the user inline and continue to the next RVA. Note it in the end-of-session report under "Missing RVAs".

## Per-RVA .md file lookup

The plate text comes from the analysis note at `re/analysis/<bucket>/<rva>.md`, specifically the **first bullet** of `## Mechanical description`. The RVA in the file path is the bare hex without `0x`, e.g. `0047b9b0.md` not `0x0047b9b0.md`. Check the bucket directory for the exact filename if in doubt — sessions are inconsistent about the `0x` prefix.

If the .md file does not exist for a given RVA (e.g. the HOLD case, or a missing file noted in the queue row), skip that RVA's plate/bookmark and note it in the end-of-session report under "Missing .md files".

## Commit sequence (git log pattern)

```
scribe: claim sweep-YYYYMMDD-HHMM
scribe: drained <bucket1> via sweep sweep-YYYYMMDD-HHMM
scribe: drained <bucket2> via sweep sweep-YYYYMMDD-HHMM
...
scribe: release sweep-YYYYMMDD-HHMM
```

One claim, one release, one "drained" commit per bucket successfully processed.

## End-of-session report

```
Sweep ID:         sweep-YYYYMMDD-HHMM
Buckets drained:  <N> (list: bucket1 (M rvas), bucket2 (M rvas), ...)
Buckets skipped:  <N> HOLD rows (list with HOLD reason)
Total plates:     <N>
Total bookmarks:  <N>
Total renames:    <N>
Missing RVAs:     <list> (null function_at AND null listing_code_unit_at)
Missing .md files:<list>
MCP failures:     none | <verbatim error>
Sync outcome:     ok | <error>
```

## What the sweep does NOT do

- Does not re-run analysis or decomp. It only writes what the analysis sessions already produced.
- Does not touch `hooks.csv`, `STUBS.md`, `UNCERTAINTIES.md`, or `DEFERRED.md` — those were written by the analysis sessions.
- Does not drain rows that are already in `## Drained`.
- Does not promote any function beyond C1 — C2+ promotions go through `re-classify`.
- Does not auto-resolve HOLD rows — surfaces them and stops.

## Relationship to other skills

- **ghidra-pool** — the sweep opens the **master** project directly (not a pool slot). Do not call `ghidra_pool.sh acquire` for the sweep.
- **multi-session** — the sweep is the only session allowed to hold `master.WIP-*` during a fanout. Coordinate via `re/SESSION_RULES.md` § "Parallel-fanout scribe-queue pattern".
- **re-classify** — use after the sweep if a function warrants promotion above C1.
