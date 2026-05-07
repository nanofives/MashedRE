---
name: sweep
description: Run the parallel-fanout scribe sweep — drain all "Queued" rows in re/SCRIBE_QUEUE.md into the master Ghidra project. This is the follow-up Opus session that runs after one or more parallel analysis sessions have queued their buckets. Triggers on "run the sweep", "run sweep", "drain the queue", "process the scribe queue", "scribe the queued sessions".
---

# sweep — scribe-queue drain session

The sweep is a single Opus session that drains every "Queued" row in `re/SCRIBE_QUEUE.md` into the master Ghidra project in one serial pass. It replaces the per-session scribe step that parallel-fanout sessions are not allowed to run directly (see `re/SESSION_RULES.md` § "Parallel-fanout scribe-queue pattern").

**Recommended model:** `claude-opus-4-7` at medium thinking. Reason: last-writer-wins on master; correctness matters more than speed.

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
     project_name="Mashed"
     program_name="MASHED.exe"
     read_only=false
6. mcp__ghidra__health_ping → must succeed; mcp__ghidra__ghidra_info path must NOT contain "pool"
7. For each Queued row (in order, skipping HOLD rows):
     a. Log: "Draining bucket=<bucket> rvas=<count>"
     b. For each RVA in the row's rvas list — in address order, one at a time:
          i.   mcp__ghidra__function_at <rva>
               → if null: write a C0 listing-level plate instead (see § "C0 fallback" below); do NOT skip the RVA
          ii.  Build plate text: first bullet of re/analysis/<bucket>/<rva>.md § "## Mechanical description",
               prefixed "[C1 YYYY-MM-DD] ". Truncate at last word boundary ≤ 120 chars, append "…" if cut.
               Never paraphrase — truncation is the only compression.
          iii. mcp__ghidra__comment_set  (plate comment, type="plate")
          iv.  mcp__ghidra__bookmark_add  category="first-pass"  description="C1 YYYY-MM-DD session:<bucket>"
          v.   mcp__ghidra__comment_get <rva>  (check for "Library Function:" prefix)
               → if matches single concrete symbol AND Ghidra still shows FUN_XXXXXXXX:
                   mcp__ghidra__function_rename to exactly that symbol name (no prefix/suffix/namespace)
               → otherwise: skip rename
          vi.  If any MCP call in (i)–(v) fails: halt the entire sweep, surface the error verbatim.
               Do NOT skip and continue. The user decides whether to retry from the failed RVA or abort.
     c. After all RVAs in the bucket complete:
          Append to re/analysis/CHANGELOG.md:
          "<YYYY-MM-DD>  $SESSION_SHORT_ID  scribe-release  bucket=<bucket>  writes=<N>  errors=0"
          Move the row in re/SCRIBE_QUEUE.md from "## Queued" to "## Drained",
          appending:  drained-by=<SESSION_SHORT_ID>; <N> plates, <N> bookmarks, <renames> renames
          git add re/SCRIBE_QUEUE.md re/analysis/CHANGELOG.md
          git commit -m "scribe: drained <bucket> via sweep $SESSION_SHORT_ID"
8. mcp__ghidra__program_save              # NOT program_save_as
9. mcp__ghidra__program_close
10. bash scripts/ghidra_pool.sh sync
11. git rm "master.WIP-$SESSION_SHORT_ID"
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
