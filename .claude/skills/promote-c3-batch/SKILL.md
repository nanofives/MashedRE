---
name: promote-c3-batch
description: Generate a parallel-fanout batch file of Claude Code prompts that drive C2→C3 promotions for Mashed RE. Each session is bounded so it finishes before the conversation auto-compacts. Triggers on "generate a c3 batch", "plan a c3 fanout", "make a promotion batch", "batch of promotion prompts", "fan out the C3 work".
---

# promote-c3-batch — generate parallel C3-promotion batch prompts

This skill is **planning-only**. It produces a text file (analog of `batch_h.txt` for Ghidra discovery) containing N self-contained Claude Code prompts. Each prompt drives one parallel Sonnet 4.6 session that promotes a handful of C2 functions to C3. The skill does **not** execute the prompts — the user pastes each session block into a separate Claude session.

The C2→C3 step is mechanical for most candidates (read analysis note → author small reimpl → diff via Frida → re-classify). What makes it parallelizable is `scripts/frida_pool.sh` + `re/frida/run_diff_parallel.py` (already built) plus per-session worktree isolation (already supported by the `worktree` skill).

## When to invoke

- "Generate a C3 batch of N sessions"
- "Plan a fanout for the next batch of util C2 leaves"
- "Make me a promotion batch for the frontend C2 cluster"
- After a successful pilot batch, when scaling to more candidates

## Inputs

The skill needs to know:
- **N sessions** (**default 6** — the standing batch shape; cap at 8 if the user explicitly asks).
- **Max functions per session** (default 5; hard cap 8 — see § "Why 5/8" below).
- **Subsystem filter** (optional; e.g., `util`, `util,frontend`). Defaults to "any C2 function with a known signature."
- **Complexity profile** (optional): `pure-leaves` (default for first batches), `near-leaves` (one callee that must be at C2+), or `mixed`.

If a parameter is missing, prefer the conservative default — never balloon a session to a complexity the cap can't absorb.

**Two production batch shapes:**

| Shape | Sessions × K | Model | Total promotions | When to pick |
|---|---|---|---|---|
| **standard** (default) | 6 × 5 | Sonnet 4.6 | 30 | Routine batches when the candidate pool has lots of small leaves. Sonnet handles K=5 cleanly inside a 150k context budget. |
| **high-throughput** | 6 × 10 | Opus 4.7 1M | 60 | Roughly 2-3× the token cost for ~2× the promotions per round. Pick when the candidate pool justifies it (e.g. ≥60 v4-filter-passed candidates) or when the user has explicitly asked for "bigger" / "Opus" / "high-throughput". c3_batch_j ran this shape successfully. |

Both shapes use the same 6-session fanout — only the per-session K and worker model differ. Default to **standard**; produce **high-throughput** when the user asks for it OR when v4-filter-passed candidates ≥ 60 AND the user is operating under "make it fast" mode rather than a budget-constrained mode. If you up-shape on your own initiative, declare the cost trade-off in the batch file header.

## Why 5 / 8 / 10 (the per-session cap)

Each C2→C3 promotion costs the worker session roughly **10–15k tokens**: read analysis note, scan existing similar hook, write a small `.cpp`, edit `build.bat`, edit `hooks_registry.py`, build, run `run_diff_parallel.py`, then a ~5–10k transaction in re-classify (hooks.csv slice reads + multiple Edits across hooks.csv / STUBS.md / CHANGELOG / analysis note frontmatter).

**Sonnet 4.6 (standard shape):** effective working budget before auto-compact is ≈150k tokens. With ~13k of fixed startup context (CLAUDE.md + CONFIDENCE.md + this skill's prompt + hooks_registry inspection), a session has ≈137k usable. That fits:

- **5 functions** = 75k → 88k total → ~50k headroom (preferred default; survives one drift-promote or struct-doc detour)
- **8 functions** = 120k → 133k → ~17k headroom (hard Sonnet cap; no room for surprises)

Beyond 8 on Sonnet, compaction is likely mid-session. A compaction in the middle of a re-classify transaction is a recipe for tracker drift, so the Sonnet cap is firm at 8.

**Opus 4.7 1M (high-throughput shape):** effective working budget is ≈1M tokens. At 10–15k per promotion, K=10 lands at ~150k total — well within budget. K=15 (~225k) is also fine; K=20 starts to pile up tool-result history. The Opus session cap used in production is **10** (matches c3_batch_j) with a hard absolute ceiling at **15**.

## Model declaration (per session)

| Role | Model | Reason |
|---|---|---|
| Per-session worker (standard shape) | **Sonnet 4.6** (`claude-sonnet-4-6`) | C2→C3 authoring is mechanical: read note, copy template, paste address, run harness. Sonnet handles it cleanly at K=5–8 and is ~3-4× cheaper than Opus per token. |
| Per-session worker (high-throughput shape) | **Opus 4.7 1M** (`claude-opus-4-7[1m]`) | 1M context fits K=10–15 comfortably. Use when total promotion target ≥60 or when filter-passed pool justifies the token cost. |
| End-of-batch merge / promotion-queue sweep | Sonnet 4.6 (`claude-sonnet-4-6`) per `frida-sweep` SKILL.md | Mechanical conflict resolution + observed integration diff. Opus is wasted. |
| Caller drift-promote (if needed mid-session) | Same as the session model | Inline within the same session — Opus only if the caller turns out to be > 200 lines of decomp with semantic puzzles. |

Always write the model id verbatim into the batch row (`Model: Sonnet 4.6 (claude-sonnet-4-6) — hard cap N RVAs` or `Model: Opus 4.7 1M (claude-opus-4-7[1m]) — hard cap N RVAs`). Future-you will thank current-you when comparing token spend across batches.

## Candidate selection algorithm

**All filter steps below run at batch-generation time, not at worker-session time.** The 2026-05-17 c3_batch_h post-mortem showed that mentioning these filters in prose without applying them programmatically produced sessions full of un-promotable RVAs (3 of 6 sessions landed 0 promotions). Each step has a runnable recipe — use it before emitting the batch file.

1. **Pull C2 rows from `hooks.csv`** in the requested subsystem(s):
   ```bash
   awk -F',' 'NR>1 && $4=="C2" && $3 ~ /^(util|frontend)$/ {print $1","$2","$6}' hooks.csv
   ```
2. **Drop candidates that are already C3+** in any other row (drift detection — see today's session for an example: same RVA appearing 3x with different confidences).

3. **Stale-impl-row filter (tracker drift, NOT a C3 candidate).** A row marked C2 in hooks.csv but with an existing reimpl on disk + a GREEN diff CSV in `log/` is tracker drift — the prior re-classify transaction never landed. Surface separately and skip from the batch:
   ```bash
   # rva is like 0x00xxxxxx
   for rva in <candidate_list>; do
     rva_no_prefix=${rva#0x}
     has_impl=$(grep -rl "$rva_no_prefix" mashedmod/src/ 2>/dev/null | head -1)
     has_diff=$(ls log/diff_*${rva_no_prefix}*.csv 2>/dev/null | head -1)
     if [[ -n "$has_impl" && -n "$has_diff" ]]; then
       echo "DRIFT $rva impl=$has_impl diff=$has_diff"
     fi
   done
   ```
   Emit a separate "tracker-drift" report alongside the batch file. The user runs a single `re-classify` pass on those rows rather than burning worker sessions on no-op "promotions".

4. **Inline-`[UNCERTAIN]`-in-body filter.** A `[UNCERTAIN]` marker anywhere in the note body OUTSIDE the `## Uncertainties` collected-items section is a hard refusal per the `re-classify` rubric. Markers inside `## Uncertainties` are legitimate (the catalog of known holes); inline markers in `## Mechanical description`, `## Constants`, etc. are not.
   ```bash
   # Returns 0 (and prints the path) if the note has inline UNCERTAIN outside ## Uncertainties.
   awk '
     /^## Uncertainties[[:space:]]*$/ {in_unc=1; next}
     /^## / && in_unc {in_unc=0}
     !in_unc && /\[UNCERTAIN\]/ {found=1}
     END {exit !found}
   ' "$note_path" && echo "DEFER inline-uncertain: $note_path"
   ```
   This was the single largest source of refusals in c3_batch_h: session 2 lost 5/5 candidates to inline-UNCERTAIN body markers.

5. **Depth-1-callee-at-C2+ gate.** Per `re/CONFIDENCE.md`, C3 requires every depth-1 callee at C2 or above. Read `callees_depth1` from the note frontmatter, then verify each against `hooks.csv`:
   ```bash
   # Pull callees_depth1 from the note frontmatter (single-line YAML list).
   callees=$(awk '/^callees_depth1:/{sub(/^callees_depth1:[[:space:]]*/,""); gsub(/[\[\]"]/,""); print; exit}' "$note_path")
   for c in $(echo "$callees" | tr ',' ' '); do
     [[ -z "$c" ]] && continue
     row=$(grep "^$c," hooks.csv | head -1)
     conf=$(echo "$row" | awk -F',' '{print $4}')
     case "$conf" in
       C2|C3|C4) ;;  # ok
       *) echo "DEFER callee-gate: $note_path callee=$c conf=${conf:-MISSING}" ;;
     esac
   done
   ```
   c3_batch_h session 3 lost 9 of 12 candidates here (depth-1 callees still C1). Catching this at batch-generation time saves the entire session.

6. **Harness arg_type compatibility filter.** The candidate's signature must already be expressible in `re/frida/diff_template.js`. Authoring a new arg_type belongs in a dedicated harness-extension session, NOT inside a promotion batch.
   ```bash
   # Pull the currently-supported arg_types directly from the harness (source of truth — list grows per-batch).
   supported=$(grep -oE "arg_type === '[a-z_0-9]+'" re/frida/diff_template.js \
                 | sort -u | sed -E "s/arg_type === '([^']+)'/\1/")
   # For each candidate, read its frontmatter signature_arg_type (or arg_type) field.
   note_arg_type=$(awk '/^(arg_type|signature_arg_type):/{print $2; exit}' "$note_path")
   echo "$supported" | grep -qx "$note_arg_type" \
     || echo "DEFER unsupported-arg-type: $note_path arg_type=$note_arg_type"
   ```
   c3_batch_h refusals in this bucket included ECX+EAX dual-register, ESI-implicit, FPU-6-arg-with-x87, long-double return, and struct-ptr-returning alloc — none of those have a harness entry yet. Queue a harness-extension session first if the candidate is otherwise viable.

7. **Filter for size & misc viability:**
   - `size_bytes` > 200 → defer to a separate "complex C3" batch with smaller per-session counts.
   - Note body describes a function very different from what hooks.csv says → defer (the note has drift; flag for re-discover).

8. **Caller-at-C2+ gate** (per `re/CONFIDENCE.md`): look up the candidate's noted callers in `hooks.csv`. If at least one caller is C2+, the gate passes. If all callers are C1, flag the session for an in-session drift-promote step (acceptable for 1–2 callers per session; if more, split the session).
9. **Cluster by subsystem** so each session reuses the same subdirectory in `mashed_re/`. This minimizes `build.bat` merge surface and lets the session reuse one C++ file for multiple hooks.
10. **Balance load**: aim for similar `total_size_bytes` per session, not just function count. Many tiny leaves + one mid-size dispatcher is fine; eight near-cap-size functions is not.

## Pre-flight viability matrix

Each filter above rejects a specific failure mode. Run the recipes, log how many candidates each filter eliminates, and only then commit to the batch shape. If a single filter rejects > 50% of an initial bucket, the bucket is the wrong shape — re-pick from a different subsystem before emitting.

| Filter | Rejects | c3_batch_h calibration (2026-05-17) | Action on hit |
|---|---|---|---|
| (2) Already-C3+ duplicate row | Stale row from a prior promotion | Low rate but always present | Drop silently |
| (3) Stale-impl + GREEN diff in `log/` | Tracker drift (re-classify never landed) | s1: 3 RVAs (entire session was tracker-fix material) | Emit to tracker-drift report; NOT to the batch |
| (4) Inline `[UNCERTAIN]` outside `## Uncertainties` | C3 rubric refusal | s2: 5 of 5 candidates (whole session lost) | Defer to a "needs uncertainty-resolution pass" |
| (5) Depth-1 callee < C2 | C3 rubric refusal | s3: 9 of 12 candidates | Defer; queue a callee-first promotion |
| (6) `arg_type` absent from `diff_template.js` | Harness can't diff the candidate | s4: ~7 candidates across signatures | Queue a harness-extension session FIRST |
| (v4-a) Signature-unsupported phrase in body | Harness can't diff (sig.-level) | c3_batch_j: 6 of 9 s3/s4 refusals had detectable phrases (`__fastcall`, `EAX-implicit`, `in_EAX`, `EAX+ESI`, `5-arg`) | Queue a harness-extension session FIRST |
| (v4-b) Live-state side-effect call | Synthetic Frida call corrupts game state | c3_batch_j s6: 4 of 5 refused candidates (DialogBoxParam, fopen/fwrite/fclose, CloseHandle) | Defer; can never be diffed via synthetic harness |
| (v4-c) Tighter callee regex | v3 missed RVAs embedded in `FUN_xxxxxxxx`-style mentions (\b doesn't fire on `_`-boundary). The 0x004669b0 note had 4 C1 callees v3 silently dropped. | c3_batch_j: 36 of 107 v3-passes additionally rejected once `FUN_/DAT_/LAB_` prefixes are recognized | Defer; queue a callee-first promotion |
| (v4-d) `Blocks: <C-level>` honoring | Catalogued U-IDs with `Blocks: C3` cannot be C3-promoted regardless of where the marker sits in the note | c3-batch-i-s1: 0x005aea00 lost to U-0125 Blocks=C3 (worker-side); v4 catches this at filter time | Defer until the U-ID is resolved |

**Aggregate calibration.** c3_batch_h fielded ~30 candidates across 6 sessions; the upgraded filter set would have rejected roughly 15 of them before reaching any worker, and sessions 2/3/4 (which landed 0 promotions) would have been re-bucketed instead of run.

**c3_batch_j calibration (filter v4, 2026-05-18).** v4 ran against the same 197-candidate C2 pool that v3 saw, and yielded **68 passes** vs v3's **122** — a 44% tightening. Of v3's 107 unpromoted-after-c3_batch_j passes, v4 newly rejects 53: 36 callee-gate (the `FUN_xxx` regex fix), 9 signature-unsupported, 8 live-state-side-effect. Catch-rate on 12 known-bad RVAs from c3_batch_j_s3/s4/s6 refusals: **9 of 12 (75%)**. The remaining 3 leaks (0x0046cbe0, 0x00467300, 0x004960e0) are clean-noted candidates whose harness-incompatibility isn't visible in the note text — worker-session refusal still catches them, but at the cost of one candidate slot.

### Filter v4 — runnable artifact

The viability filter is implemented at `re/analysis/plans/c3_filter_v4.py`. Run before emitting any batch from this skill:

```bash
py -3.12 re/analysis/plans/c3_filter_v4.py \
    --subsystems audio,save,vehicle,input,boot \
    --out-prefix c3_batch_<id>
```

It writes `re/analysis/plans/c3_batch_<id>_passed.tsv` (the worker-eligible pool) and `..._rejected.tsv` (with one-line reasons per RVA, for the skill's deferred-report).

v4 supersedes v3's `c3_batch_j_filter.py` — keep v3 around for historical comparison but emit batches from v4 going forward. v4 adds four checks v3 over-permitted on, summarized as rows (v4-a..v4-d) in the matrix above.

## Subsystem yield expectations

Expected C3 yield (promotions / candidates ever-attempted) varies sharply by subsystem maturity. Use this table at batch-generation time to size the batch — over-pull from low-yield subsystems if the user asks for them, and warn explicitly when the batch is front-loaded with a low-yield bucket.

| Subsystem | Expected yield | Notes |
|---|---|---|
| util-small (leaf math, small inlines) | ~67% | Highest. Pure leaves dominate. Default first-batch target. |
| util-mid (small dispatchers, glue) | ~42% | Callee-gate starts mattering. Expect about half to defer. |
| HUD | ~33% | Struct-write patterns; `arg_type` often needs `entity_field_set`-class extensions. |
| frontend | ~0% on c3_batch_h baseline | Deep struct chains, inline `[UNCERTAIN]` density high. Treat as C2-readiness work, not C3 work, until the analysis notes mature. |

When the user requests a "frontend C3 batch", cite this yield expectation back and propose a 2-session pilot before committing to N=6.

## Worktree binding

Each session block must include worktree-acquire commands at the top so two sessions never edit the same `build.bat`/`hooks_registry.py`. The `worktree` skill handles the heavy lifting:

```bash
NAME=c3-batch-<id>-session-<n>
BRANCH=c3/batch-<id>-session-<n>
git worktree add ".worktrees/$NAME" -b "$BRANCH"
cd ".worktrees/$NAME"
# Frida pool is shared across worktrees — slots are PID-based.
# Each session acquires its own slot at diff time, not at session start.
```

Worktrees merge back to main via the user-driven sweep at end-of-batch (see § "When sweep is needed").

## Pool slot semantics

- **Ghidra slots are NOT needed** for C2→C3 work. The analysis is already done (the function is at C2; the note exists). No fresh decomp work means no Ghidra writeback.
- **Frida pool slots ARE needed** for the diff step. The `run_diff_parallel.py` runner acquires them per hook via `scripts/frida_pool.sh acquire`. Sessions don't pre-allocate — they queue. With 4 pool slots and **6 sessions** each verifying ~5 hooks at the verify step, the worst case is ~30 diffs wanting slots near-simultaneously — pool queues them, each diff is ~2s, total drain is ~15s. No need to grow `MAX_SLOTS` past 4 unless you observe sessions stalling on slot acquire.

## End-of-batch merge — invoke the `frida-sweep` skill

For any batch with N≥3 sessions (and definitely for the default N=6), the merge step lives in its own skill: **`frida-sweep`**. Do not inline the merge recipe here.

After the user has run every session in the batch and each session has:
1. Committed its worktree branch (`c3/batch-<id>-s<N>`)
2. Appended a queue row to `re/PROMOTION_QUEUE.md`

…the next step is to invoke `frida-sweep` (Sonnet 4.6) from the main worktree:

> "run the frida sweep" / "drain the promotion queue" / "merge the c3 batch"

That skill:
- Merges every queued branch onto main, resolving the three known text-file conflicts (`build.bat`, `hooks_registry.py`, `CHANGELOG.md`) mechanically.
- Rebuilds the canonical `mashed_re_dev.asi` from the merged state.
- Runs `run_diff_parallel.py` against ALL promoted hooks in the merged build — the integration diff that catches cross-hook regressions (symbol clashes, overlapping JMP patches, struct-definition drift, include ordering).
- Commits the merge only if every hook is GREEN.

A batch that has not been run through `frida-sweep` is **not landed**. Sessions' per-worktree commits are not visible on main until the sweep merges them.

### When `frida-sweep` is NOT needed

- Single-session work — that session ran `re-classify` directly on main (or a one-off branch); no fanout, no merge.
- N=2 batches with strong subsystem separation (different `<Subsystem>/` dirs). Try a plain `git merge` first; only invoke `frida-sweep` if conflicts surface.

For the default N=6 batch: always invoke `frida-sweep`.

## Session block template

The skill emits one of these per session. Substitute placeholders.

```
================================================================================
Session <N> — <subsystem>_c3_<seq>  (C2→C3, <K>-function batch)
================================================================================

Model:       Sonnet 4.6 (claude-sonnet-4-6) — hard cap 8 RVAs (this session: <K>)
Worktree:    .worktrees/c3-batch-<id>-s<N>/  (branch c3/batch-<id>-s<N>)
Skills:      hook-author, diff-original, re-classify
Frida pool:  shared via scripts/frida_pool.sh (no pre-allocation)
Batch id:    c3-batch-<id>

# Mission
Promote <K> C2 functions to C3 by authoring their reimpl in mashed_re_dev.asi,
running run_diff_parallel.py for bit-identical verification, and applying
re-classify transactionally. Stay inside this worktree. Do not touch main.

# Candidates (read each analysis note BEFORE authoring)
| RVA        | Ghidra name      | Analysis note                                | Reimpl name (suggested) | Notes |
|------------|------------------|----------------------------------------------|-------------------------|-------|
| 0x00xxxxxx | FUN_00xxxxxx     | re/analysis/<sub>/<file>.md                  | <ReimplName>            | <leaf? signature? gate flag?> |
| ...        | ...              | ...                                          | ...                     | ...   |

# Pre-flight checklist
1. Confirm worktree binding:  `git worktree list | grep c3-batch-<id>-s<N>`
2. Anchor check:              `sha256sum original/MASHED.exe` matches
                              BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
3. Build baseline:            `mashedmod\build.bat` must succeed on a clean
                              worktree before you add any new hook (catches
                              merge breakage from main).
4. Caller-at-C2+ gate:        For each candidate, check the analysis note's
                              `callers_noted` field. Cross-check each caller
                              RVA in hooks.csv. If any caller is C1 with full
                              C2-quality analysis (mechanical decomp, U-IDs,
                              constants), do an in-session drift-promote
                              C1→C2 of that caller before promoting the leaf.

# Per-function workflow (repeat for each RVA, then commit ONCE at the end)
1. Read re/analysis/<sub>/<rva>.md end-to-end. Confirm:
   - callees_depth1 is [] (or all callees at C2+).
   - signature is one of: float(float), float(pointer), uint32(), void(uint32),
     or another already supported by re/frida/diff_template.js.
   - No unresolved inline [UNCERTAIN] markers in the body.
2. If the signature is NOT in the supported set, STOP and queue a
   note in PROMOTION_QUEUE.md asking the user to extend the harness.
   Do NOT invent a new arg_type in this session.
3. Author the reimpl:
   - File: mashedmod/src/mashed_re/<Subsystem>/<Cluster>.cpp
     (one file per cluster of related hooks, not per RVA; matches the
     existing Math/RwSqrt.cpp pattern).
   - Copy the comment header style from existing C3+ hooks (cite RVA, cite
     disasm bytes, cite the no-guessing source-of-truth lines).
   - extern "C" __declspec(dllexport) <ret> __cdecl <Name>(<args>) { … }
   - RH_ScopedInstall(<Name>, 0x00xxxxxx);
4. Wire build:
   - Append `"%SRC%\<Subsystem>\<Cluster>.cpp" ^` to mashedmod\build.bat.
   - Append a hooks_registry.py entry with the right arg_type and a
     non-trivial input domain (≥10 test vectors for scalar; ≥10 sentinels
     for read_global; cover edge cases — 0, MAX, sign bit, alternating bits).
5. Build: `cmd /c mashedmod\build.bat > log\build_<rva>.txt 2>&1`. Halt on failure.

# Batch verification (run ONCE after all K hooks authored)
1. scripts/frida_pool.sh cleanup
2. py -3.12 re/frida/run_diff_parallel.py <hook1> <hook2> ... <hookK>
3. Expect all GREEN with 100% bit-identical match counts. If any RED:
   debug the failing hook (NOT the others), re-build, re-run.
4. cat log/diff_<hook>.csv for each — confirm match column is all True.

# Promotion transaction (run re-classify ONCE per batch, not per RVA)
Invoke re-classify with all K RVAs in one call. The skill validates the
gates and applies tracker mutations transactionally:
- hooks.csv: dedupe stale rows; write fresh C3 row per RVA.
- STUBS.md: resolve any stubs the new hooks close (S-IDs cited from the
  caller's analysis note).
- CHANGELOG: one line per RVA; lead with the drift-promote line if any.
- Analysis note frontmatter: bump confidence to C3 (or C2 for drift caller).

# Commit policy (this worktree only)
- One commit at the end of the per-function loop (NOT per function — keep
  the worktree git history clean).
- Commit message: `c3: <subsystem> batch <id> session <N> — <K> hooks, evidence cited`
- Do NOT merge to main from this session. Branch merge is the user's call.

# Output (what the next sweep / user-merge consumes)
- Worktree branch c3/batch-<id>-s<N> with one commit on top of main containing:
  - New mashed_re/<Subsystem>/<Cluster>.cpp file(s)
  - build.bat additions
  - hooks_registry.py additions
  - hooks.csv / STUBS.md / CHANGELOG.md updates
  - Analysis note frontmatter bumps
- One row appended to re/PROMOTION_QUEUE.md (relative to project root) of
  the form:
    YYYY-MM-DD  c3-batch-<id>-s<N>  rvas=0x<a>,0x<b>,...  branch=c3/batch-<id>-s<N>  evidence=log/diff_<hook>.csv;...  note=<one-line>

# STOP-AND-ASK if
- Anchor mismatch on MASHED.exe.
- Baseline build fails on clean worktree (main is broken — escalate).
- A candidate's analysis note describes a function very different from
  what hooks.csv says (drift on the note; flag and skip rather than
  promoting).
- More than 2 candidates in this session need caller drift-promote (split
  the session; do the drift-promotes in a dedicated pass).
- Any C2 candidate's analysis note has `[UNCERTAIN]` markers in the body
  that are NOT yet filed in UNCERTAINTIES.md (file them first, then
  decide whether to defer the candidate).
```

## Skill execution workflow

When invoked:

1. **Parse the request** (sessions, per-session cap, subsystem filter, complexity).
2. **Pull C2 candidates** via the `awk` query above, then read enough analysis notes to apply the viability filter. For large filter sets, sample first — don't read 200 notes serially. Use Glob/Grep to surface candidates by markers (`callees_depth1: \[\]` for pure leaves).
3. **Score candidates** by:
   - signature compatibility with the existing harness (highest priority)
   - caller-at-C2+ (yes = +2, drift-eligible = +1, no = exclude)
   - size_bytes (≤50 = +2, ≤200 = +1, >200 = exclude unless special)
   - subsystem cohesion (prefer grouping with same-cluster siblings)
4. **Assign** top-scoring candidates to sessions, respecting the per-session cap. Aim for similar total complexity per session.
5. **Emit `c3_batch_<id>.txt`** at the project root. `<id>` = next unused letter (`a`, `b`, …) — match the existing `batch_h.txt` pattern but in its own namespace.
6. **Report to the user**:
   - File path
   - N sessions × K candidates each (= total to promote)
   - List of candidates included
   - List of viable candidates DEFERRED (so the user knows what's left)
   - One-line invocation reminder: "open N Claude Code sessions, paste session block <i> into session <i>, run."

## Output file naming

- First C3 batch: `c3_batch_a.txt`
- Second:        `c3_batch_b.txt`
- ...
- These live at the **project root** alongside the Ghidra-discovery `batch_<letter>.txt` files. Do not interleave the letter spaces — Ghidra uses `batch_*.txt`, C3 promotions use `c3_batch_*.txt`. Two namespaces, no collision.

## Anti-patterns the skill must reject

- Putting more than 8 RVAs in one session — that's the compaction-risk line.
- Mixing complexity tiers within one session (3 trivial leaves + 1 200-line dispatcher) — uneven session lengths cause the rest of the fanout to wait.
- Including candidates whose analysis note doesn't exist or hasn't been read end-to-end — speculation, not promotion.
- Recommending Opus for a routine *standard*-shape batch when the candidate pool is small (<30 v4-passed) — that's a 3-4× cost increase for no measurable quality gain on mechanical C2→C3 work. Opus is the correct choice only for *high-throughput* (K≥10) batches.
- Emitting a batch that requires sweep coordination as the default — the C3 path should self-classify per-worktree; sweep is the exception.
- Failing to declare the worktree branch name in every session block. Sessions that share branches will collide on commit.

## Default batch + scale guidance

**Default shape: standard, N=6, K=5, Sonnet 4.6** (`c3_batch_<id>.txt` with 6 sessions of 5 candidates each = 30 promotions). Wall time ~45-60 min of human-attended parallel Claude work = roughly 5 hrs serial equivalent. This is the cadence to run weekly until each subsystem's C2 backlog is drained.

**High-throughput shape: N=6, K=10, Opus 4.7 1M** (60 promotions per round). Up-shape when:
- v4-filter-passed candidates ≥ 60 AND
- the user says "high-throughput" / "Opus" / "bigger" / "make it fast"
OR
- token budget is not a constraint AND the candidate pool is large

The high-throughput shape roughly doubles per-round output for ~2-3× the token cost. c3_batch_j was run this way successfully — 60 promotions landed in one parallel-fanout afternoon.

**If a first-ever pilot is requested explicitly**, the user can ask for N=2 K=4 (8 promotions, faster validation). Do not silently shrink the default batch — if candidates run low (fewer than the shape's total target remain in the requested subsystems), produce a partial batch and report exactly which sessions are short.

C2 backlog (snapshot 2026-05-26): frontend 143, render 600+, hud 70, vehicle ~30, audio ~20 → ~800+ promotions remaining. At standard cadence (30/round) that's 27+ rounds; at high-throughput cadence (60/round) it's 13+. The high-throughput shape is strongly recommended for any subsystem with a 100+ C2 backlog.
