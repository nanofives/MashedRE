# Session Rules — Mashed RE first-pass / parallel sweeps

Every RE session (single or parallel) loads this file before any work. The prompt only references it; the rules live here so they stay versioned, diffable, and consistent across sessions.

## Hard rules (non-negotiable)

1. **MCP-down halts the session.** If `mcp__ghidra__health_ping` or any required MCP call fails, stop and surface the error verbatim. Do NOT fall back to `re/prior_art/`, web search, training data, or "what this function probably does." Prior art is for framing questions, never for answering them. Full rule in `.claude/skills/ghidra-pool/SKILL.md` § "MCP availability is required."
2. **Every cited RVA must be verified live** in the current session. Call `mcp__ghidra__function_at` or `mcp__ghidra__listing_code_unit_at` first; if null, halt — do not cite, do not "round" to a nearby valid address.
3. **Every constant** in notes/comments cites the exact Ghidra address it was read at. Sign-sensitive values get raw hex AND signed decimal.
4. **Slot is read-only.** Do not call any write-tool listed in `.claude/skills/ghidra-pool/SKILL.md` § "Write-tool gating." All persistence is via tracker files + `re/analysis/` notes, not via Ghidra symbol writes. Master writes go through the scribe protocol in `.claude/skills/multi-session/SKILL.md`.
5. **No hedging language.** "Probably", "likely", "seems to", "appears to", "I think", "presumably" are forbidden. Mechanical descriptions only. Anything genuinely unclear → `[UNCERTAIN U-NNNN]` marker + an `UNCERTAINTIES.md` row.
6. **No silent recursion.** Stay within the subset the prompt names. Deeper callees go in `DEFERRED.md`, not into this session's notes.
7. **No tracker promotion on first pass.** First-pass sessions write C0/C1 rows only. Promotions to C2+ go through the `re-classify` skill in a separate session, with evidence (Frida diff, behavioral test).

## Session sizing (RVA cap, split protocol)

Sonnet 4.6 has a 200k-token context; auto-compaction kicks in around ~140k. Compaction is **silent** — earlier MCP tool output gets summarized, and the verify-then-cite rule cannot catch a session that doesn't notice its own facts were summarized away. For NO-GUESSING work this is the dominant hallucination risk. The countermeasure is a hard cap on RVAs per session.

### Caps

| Model | Hard cap | Soft target | Why |
|---|---|---|---|
| Sonnet 4.6 (200k context) | **20 RVAs** | 10–15 RVAs | Effective work budget after ~25k startup overhead and ~10k scribe overhead is ~105k. At 3k/RVA average + safety margin against 5–8k spikes on heavy decomps. |
| Opus 4.7 (1M context) | **40 RVAs** | 25–30 RVAs | Context is not the limit — attention/state-tracking is. Past ~30 RVAs, Opus's procedural discipline starts to drift even without compaction. |

### Pre-flight subset-size check (after `function_callees` returns the subset)

```
count = 1 (parent) + len(callees)
if count > <hard_cap>:
    SPLIT IMMEDIATELY — do not "push through" past the cap.
    1. Keep parent + first (hard_cap − 1) callees in this session's bucket.
    2. File the remaining (count − hard_cap) RVAs as a single DEFERRED row.
    3. Surface to user verbatim:
       "Subset of <count> RVAs exceeds the <model> cap of <hard_cap>.
        Splitting: this session takes <parent + first N callees>;
        remaining <M> RVAs filed as DEFERRED-<id> for next session."
    4. Continue the work loop with the truncated subset only.
```

### Heavy-decomp adjustment (mid-loop)

If `decomp_function` for a single RVA returns >5k tokens of output, count that RVA as **2** against the cap (it spent twice the context it should have). When the running total — counting heavy RVAs as 2 — reaches `hard_cap − 2`, stop the work loop early:

1. Commit the partial subset (whatever .md notes + tracker rows you have).
2. File the remaining un-decompiled RVAs as a DEFERRED row.
3. Run the scribe step on the partial subset.
4. End-of-session report includes `early-finish: yes (running count <n> at hard_cap−2; remaining filed as D-NNNN)`.

Better to commit a partial session than to risk compaction-induced hallucinated facts in the second half.

### Split bucket naming

When a subset splits across sessions, never reuse the same bucket directory name (the per-RVA `.md` files would collide).

- First session: `re/analysis/<base-bucket>/`
- Second session (continuation): `re/analysis/<base-bucket>-cont1/`
- Third: `re/analysis/<base-bucket>-cont2/`, etc.

### DEFERRED row format for splits

```
D-NNNN <comma-separated RVAs> from session <SESSION_SHORT_ID> bucket <base-bucket>
       — pick up as bucket <base-bucket>-cont<N>; same depth, no further recursion.
```

The `from session` and `pick up as` fields make the re-pickup unambiguous: the next session reads the DEFERRED row, knows exactly which RVAs to take and what bucket to put them in.

### Scribe-commit noise

Each session adds two scribe commits (`scribe: claim …` and `scribe: release …`) plus the analysis commit. A 40-RVA tree split into three Sonnet sessions is 9 commits before any sweep is "done." This is by design — the scribe pairs exist for the audit trail in `re/analysis/CHANGELOG.md`. **If you care about a clean log on `main`, squash before merging long-running feature branches** (`git rebase -i` collapsing scribe pairs into the analysis commit per session). Do **not** squash on `main` itself; the audit trail there is load-bearing for resolving "who wrote this plate comment when."

## Startup ritual

a. **Anchor check.** `sha256sum original/MASHED.exe` must equal `BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`. Mismatch → halt; the trackers' RVAs are invalid.

b. **Acquire slot.**
   ```bash
   SLOT=$(bash scripts/ghidra_pool.sh acquire)
   echo "$SLOT" > .pool_slot
   ```

c. **Pre-flight asserts.** `bash scripts/ghidra_assert.sh preflight "$SLOT"` — all three must pass.

d. **Open slot read-only via MCP.**
   ```
   mcp__ghidra__project_program_open_existing
     project_location="C:/Users/maria/Desktop/Proyectos/Mashed/mashed_pool"
     project_name="$SLOT"
     program_name="MASHED.exe"
     read_only=true
   ```

e. **MCP liveness + identity.**
   - `mcp__ghidra__health_ping` → must succeed
   - `mcp__ghidra__ghidra_info` → returned project path must contain `$SLOT`
   - Either fails → halt per rule 1.

## Per-function output format

For each RVA in the session's subset, write `re/analysis/<bucket>/<rva>.md`:

```markdown
---
rva: 0x004axxxx
name_in_ghidra: <ghidra-symbol>
size_bytes: <function_at.size>
confidence: C0-or-C1     # C0 = no behavioral evidence; C1 = structurally read only
callees_depth1:
  - 0x...: <name>
opened_in_slot: <SLOT>
session_date: YYYY-MM-DD
---

## Mechanical description
- Bullets of what the decomp literally does — reads, writes, calls, branches.
- No intent. No "this looks like X."
- Cite the address where each constant appears.

## Constants
| RVA cited at | Raw hex | Signed dec | Notes |
|--------------|---------|------------|-------|
| 0x...        | 0x...   | ...        | only what the listing shows |

## Uncertainties
- [UNCERTAIN U-NNNN] <what is unclear, what evidence is missing>

## Stubs encountered (callees not yet reversed)
- [STUB S-NNNN] 0x... <ghidra-name>
```

## Tracker updates (first-pass only)

Hand-edit only the rows for THIS session's subset. The `re-classify` skill is for promotions, which first-pass does not perform.

- **`hooks.csv`** — one row per RVA: `RVA, ClassName::Method (or sub_<rva>), C0|C1, file=re/analysis/<bucket>/<rva>.md`
- **`STUBS.md`** — append `S-NNNN` rows for unreversed callees encountered.
- **`UNCERTAINTIES.md`** — append `U-NNNN` rows for any `[UNCERTAIN]` markers.
- **`DEFERRED.md`** — append `D-NNNN` rows for RVAs intentionally not recursed into, with the re-pickup condition.

## Shutdown ritual

```bash
mcp__ghidra__program_close
bash scripts/ghidra_pool.sh release "${SLOT#Mashed_pool}"
rm .pool_slot
git add re/analysis/<bucket>/ hooks.csv STUBS.md UNCERTAINTIES.md DEFERRED.md
git commit -m "first pass: <subset description> (slot $SLOT)"
```

After the commit, immediately attempt the **post-analysis scribe step** (see below).

## Post-analysis scribe step

Immediately after the analysis commit, every session attempts to push its findings into the master
Ghidra project. This is a serial, immediate write — no polling, no retry loop.

### Session-short-id (used in flag files, commit messages, and CHANGELOG rows)

Construct **once at session start**, before any other scribe-related step, and reuse for every flag/commit/log entry in the session:

```bash
SESSION_SHORT_ID="<bucket>-$(date -u +%Y%m%d-%H%M)"
# Example for the entry_callees bucket: entry_callees-20260502-1437
```

Format: `<bucket>-YYYYMMDD-HHMM`, UTC. Deterministic, no guessing, collision-free in practice (collisions only on same-bucket same-minute, which the lock check would catch anyway). Never invent a different format — `<sessionid>` and `<session-short-id>` are aliases for this one value.

### Lock check (do this first, before opening anything)

```bash
bash scripts/ghidra_assert.sh scribe-check
```

- **Flag absent** → proceed with the steps below.
- **Flag present** (`master.WIP-*` exists) → **stop and surface verbatim to the user:**

  > "Master is locked by `<flag-file-name>`. Scribe step skipped.
  > When the lock is released, re-run the scribe step manually for this subset:
  > bucket=`<bucket>`, RVAs: `<comma-separated list from this session>`."

  Do **not** poll. Do **not** retry automatically. Wait for the user to say "retry."

### What to write at C1

Only these write tools are appropriate for a first-pass C1 scribe session:

| Tool | When | Content |
|---|---|---|
| `comment_set` (plate) | Every analyzed `FUN_XXXXXXXX` | First bullet of the .md's `## Mechanical description`, copied **verbatim**, prefixed with `[C1 YYYY-MM-DD] `. **Truncation rule:** if the prefixed text exceeds 120 chars, cut at the last word boundary that fits and append `…`. Never paraphrase, reword, or summarize to compress — truncation is the only compression operation allowed. |
| `bookmark_add` | Every analyzed RVA | Category `first-pass`, description `C1 YYYY-MM-DD session:<bucket>`. |
| `function_rename` | Only if **all three** hold: (a) Ghidra still shows `FUN_XXXXXXXX` for this RVA; (b) `mcp__ghidra__comment_get` for this RVA returns a comment whose text begins with `Library Function:` (Ghidra's FidDB match marker); (c) that comment names a single concrete symbol. | Rename to **exactly** the symbol Ghidra named in the `Library Function:` comment. No prefix, no suffix, no namespace decoration. If conditions (a)–(c) are not all met, **skip the rename** for this RVA — do not invent a name from any other source. |

**Detecting a library-match (procedural):**

```text
comment = mcp__ghidra__comment_get(rva, comment_type="plate" or "pre")
if comment is None or not comment.startswith("Library Function:"):
    skip rename
else:
    name = comment.split(":", 1)[1].strip().split()[0]    # first token after the colon
    mcp__ghidra__function_rename(rva, name)
```

If the FidDB match is ambiguous (e.g., "Library Function: foo OR bar"), treat as skip — never pick one.

**Not allowed at C1 scribe:** type annotations, struct layouts, parameter renames, calling-convention changes, custom symbol creation, namespace moves. Those require C2+ confidence.

### Scribe execution

`SESSION_SHORT_ID` is the value built once at session start (see "Session-short-id" above). Reuse it everywhere below — never construct a different id mid-scribe.

```
1. bash scripts/ghidra_assert.sh scribe-check              # must be clear
2. echo "master.WIP-$SESSION_SHORT_ID" > "master.WIP-$SESSION_SHORT_ID"
   git add "master.WIP-$SESSION_SHORT_ID"
   git commit -m "scribe: claim $SESSION_SHORT_ID"
3. mcp__ghidra__project_program_open_existing
     project_location="C:/Users/maria/Desktop/Proyectos/Mashed"
     project_name="Mashed"
     program_name="MASHED.exe"
     read_only=false
4. For each RVA in the session subset — in address order, one at a time:
     a. mcp__ghidra__function_at <rva>     ← confirm non-null before any write
     b. mcp__ghidra__comment_set           ← plate comment (apply truncation rule above)
     c. mcp__ghidra__bookmark_add          ← first-pass bookmark
     d. mcp__ghidra__comment_get <rva>     ← check for "Library Function:" prefix
        → if matches, mcp__ghidra__function_rename to the FidDB-supplied name
        → if no match or ambiguous, skip rename — never invent a name
5. mcp__ghidra__program_save                ← NOT program_save_as
6. mcp__ghidra__program_close
7. bash scripts/ghidra_pool.sh sync
8. git rm "master.WIP-$SESSION_SHORT_ID"
   git commit -m "scribe: release $SESSION_SHORT_ID"
```

**Flag-file contract:**

- File name: `master.WIP-$SESSION_SHORT_ID` at repo root.
- File contents: the literal string `master.WIP-$SESSION_SHORT_ID` (so `cat master.WIP-*` identifies the holder unambiguously). No newlines required, no trailing whitespace.
- Lifetime: created in step 2, removed in step 8. Any `master.WIP-*` file present outside an active scribe step is stale → user runs `git rm` and resolves manually.

**Branch-locality caveat (current limitation):** the flag is committed to the local branch. Two sessions on different worktrees / branches cannot see each other's flags until they fetch + rebase. **For solo or sequential master-edit work this is fine.** For parallel-fanout master writes this is a hole — TODO before fanout, the flag check needs to consult `origin/main` over the network. Document this in `re/analysis/CHANGELOG.md` if you ever run two scribe sessions in parallel — they may collide silently.

If any MCP call in step 4 fails: halt, surface the error, do NOT continue to the next RVA. The partial
writes already made are fine — they are safe to leave. The user decides whether to retry from the
failed RVA or abandon the scribe session.

The scribe step adds two rows to `re/analysis/CHANGELOG.md` (claim + release). Format:

```
YYYY-MM-DD <session-short-id> scribe-claim  bucket=<bucket> rvas=<count>
YYYY-MM-DD <session-short-id> scribe-release bucket=<bucket> writes=<count> errors=<count>
```

## End-of-session report (one message to user)

- Slot held: `<SLOT>`
- Functions covered: `<count>` RVAs
- New `hooks.csv` rows: `<count>` at C0/C1
- New STUBS rows: `S-NNNN..S-NNNN` (`<count>`)
- New UNCERTAINTIES rows: `U-NNNN..U-NNNN` (`<count>`)
- New DEFERRED rows: `D-NNNN..` (`<count>` RVAs)
- MCP calls that failed: list verbatim, or "none"
- Observations that warrant a follow-up question (not guesses): list, or "none"

## Reference materials (external — outside this repo)

These references live under `C:\Users\maria\Desktop\Proyectos\re-references\` so their licenses don't bleed into Mashed's tree. They are **framing references only** — they let you ask better questions about what MCP is showing you, never substitutes for what MCP shows.

### `re-references/gta-reversed-modern/`

Reverse-engineering of GTA: San Andreas. Pinned to commit `b53415bde650ed9af94212f77e8d8d842696ecb2` (2026-04-24). **License: GPL-3.0 — read, do not paste into Mashed's tree.**

Mashed and SA share RenderWare 3.x. RW struct layouts and API signatures are stable across RW 3.x titles; **everything else is SA-specific and does not transfer.**

| When MCP shows … | Read … |
|---|---|
| A function accessing fields that look like an RW frame tree (parent / child / next pointers + 4×4 LTM matrix at fixed offset) | `source/game_sa/RenderWare/rw/rwcore.h` (RwFrame, RwMatrix) |
| Atomic linked-list iteration over geometry / material structures | `source/game_sa/RenderWare/rw/rpworld.h` (RpAtomic, RpClump, RpGeometry, RpMaterial) |
| Skinning / bone matrix updates per draw | `source/game_sa/RenderWare/rw/rpskin.h`, `rphanim.h` |
| Animation playback against an RW-style anim hierarchy | `source/game_sa/RenderWare/rw/rtanim.h` |
| RWS audio middleware calls | (no useful gta-reversed analogue — SA uses different audio) |
| D3D9 calls wrapped through RW resource handles | `source/game_sa/RenderWare/D3DResourceSystem.{h,cpp}` |
| You're about to author a hook and want to know what `RH_ScopedInstall` expands to | `source/HookSystem.h` |
| You're aggregating per-class `InjectHooks()` calls into a master | `source/InjectHooksMain.cpp` |

**Forbidden uses of this reference:**

- Citing **any** RVA from gta-reversed-modern in Mashed trackers, source comments, or notes. Every address there is for SA's `gta_sa.exe`. Citing one in Mashed is a critical hallucination — equivalent to inventing it.
- Pasting source verbatim into `mashedmod/`. GPL-3.0 → would force Mashed onto GPL-3.0.
- Using SA-specific class names (`CPed`, `CVehicle`, `CCarCtrl`, `CEntity`, `CWanted`, `CWorld`, etc.) for Mashed code. Mashed's gameplay code is Supersonic's, not Rockstar's. The class hierarchy does not match.
- Treating "gta-reversed says function X does Y" as evidence about what a Mashed function does. It is evidence only about RW-side semantics; the surrounding Mashed code must be verified through MCP independently.

**Permitted uses:**

- Reading struct field layouts and confirming MCP-observed offsets against the documented RW layout.
- Reading function signatures to predict the calling convention and parameter types of an RW API call you see in MCP.
- Studying the hook-installation pattern when authoring `InjectHooks()` for a Mashed class.
- Cross-checking your re-authored RW header in `mashedmod/deps/RenderWare/` for layout errors after you derive it from MCP.

The full curation manifest for gta-reversed-modern is at `C:\Users\maria\Desktop\Proyectos\re-references\README.md`. Read it once before your first hook-authoring session.

### `re-references/librw/` — **first reference to consult on any RW question**

Clean-room MIT-licensed RenderWare 3 re-implementation by aap. Pinned to commit `1252b90e03fa89d5d15e9d58166b2f34eb19643f`.

**This is the most useful RW reference we have**, and it has the loosest license (MIT — vendoring snippets is fine with attribution). Where gta-reversed-modern shows *one game's specific RW usage*, librw documents *the engine itself* — what RW APIs guarantee, what struct invariants hold, how the atomic / frame / world pipelines are shaped.

| When MCP shows … | Read in librw … |
|---|---|
| Any function whose name starts with `Rw*`, `Rp*`, `Rt*` (or whose decomp body matches that pattern with stripped names) | `src/` first — the engine core |
| RW calls that touch the D3D9 backend (texture upload, vertex buffer, rasterizer state) | `src/d3d9/` — Mashed's runtime backend |
| A struct with a tagged-section header (4-byte section ID, 4-byte size, 4-byte version) | `src/base.cpp` and `src/streamread.cpp` — RW chunk reader |
| Frame-tree traversal (parent/child/next pointers, LTM matrix updates) | `src/baseframe.cpp` |
| Atomic / clump iteration | `src/baseworld.cpp` |
| Texture dictionary or texture loading | `src/basetexture.cpp` and librwgta's `tools/storiesconv/` for TXD parsing |

**Permitted uses for Mashed (broader than gta-reversed-modern):**

- **Vendor struct definitions directly** into `mashedmod/deps/RenderWare/` with MIT attribution preserved in the file header.
- **Vendor utility functions** (matrix math, frame walkers) with attribution.
- **Use as the canonical reference** when re-deriving an RW struct layout from MCP — if MCP shows a 0x40-byte struct that looks like an `RpAtomic`, librw's `RpAtomic` definition is your cross-check.

**Hard rule unchanged:** librw tells you what RW APIs *should* do; only MCP tells you what Mashed actually does. A field offset matching librw's layout is evidence; "this function is probably an RW frame walker because librw has one" is not.

### `re-references/librwgta/` — RW format parsers (DFF, TXD, IFP)

Pinned to `eb06ee545af46298696532b6f394cf83ac1337c3`. MIT. Same author as librw.

The TXD parser in `tools/storiesconv/` is the cleanest open-source RW texture-dictionary reader. **Use it when Mashed-side TXD work starts** — Mashed uses RW TXD for textures per CLAUDE.md, so this is a direct match. DFF parser may apply if MCP reveals Mashed loads RW DFF models (check first).

### `re-references/criterion-rwg37/` — official Criterion RW 3.7.0.2 PC source tree (authoritative)

The Criterion-internal engine source code for RW 3.7.0.2 PC (2005-12-8 build). Pinned to commit `43f26d89b2fd4c01b70712b55bcc7f375af4ca09`. Proprietary — **study, never paste into Mashed's tree.**

This is **the definitive answer** for any RW engine semantics question. Where librw is a clean re-implementation and gta-reversed-modern is one game's RE, this tree is the original Criterion code. Use it when:

- librw and MCP disagree on what an RW API does — this tree breaks the tie.
- You need to know precisely how `RwFrameUpdateObjects` / `RpAtomicRender` / `RwRasterLock` / etc. work, including edge cases.
- You're trying to understand the **D3D9 backend** specifically — `core/driver/d3d9/` is the most directly applicable subtree because Mashed runs DX9.0c.
- You need authoritative struct field semantics (read `core/src/baframe.c`, `world/baclump.c`, etc., paired with MCP-observed offsets).

**Version caveat:** this is 3.7.0.2 (~2005). Mashed (2004) is plausibly RW 3.5 or 3.6. Struct layouts are stable across the RW 3.x line but minor field additions exist between minors. **Always cross-check field offsets against MCP before trusting "this matches Criterion 3.7.0.2."** If MCP shows a struct that's smaller or has fewer fields than the 3.7.0.2 layout, you're seeing an earlier minor — file an `[UNCERTAIN U-NNNN]` and note the discrepancy rather than forcing the larger layout onto Mashed.

**Forbidden uses (in addition to general "no paste"):**

- Citing any "address" or RVA from this tree. It has none — every "address" would be invented.
- Treating its function bodies as evidence about what *Mashed* does. They are evidence about what *RenderWare 3.7.0.2 PC* does. Mashed may differ.

### Reference priority order (when MCP shows an unfamiliar RW pattern)

1. **librw** — first read. Engine semantics in modern, readable C++ with comments. MIT — may vendor pieces with attribution.
2. **`re-references/criterion-rwg37/`** — definitive ground truth for engine behavior. Read after librw to confirm semantics, especially for D3D9 backend questions or when librw is ambiguous. **Study only, never vendor.**
3. **gta-reversed-modern** `source/game_sa/RenderWare/` — for game-side RW *usage patterns*. GPL-3.0, study only.
4. **GTAModding wiki** (gtamods.com) — for byte-layout questions on RW containers (RWS audio, TXD, DFF chunk headers). Bookmark, not a clone.
5. **librwgta** `tools/storiesconv/` — parser code for TXD/DFF/IFP. MIT, may vendor.
6. **MASHED.exe via MCP** — the final word, always. References 1–5 let you frame better questions; only MCP is evidence about Mashed.

**Conflict-resolution rules:**

- librw vs. criterion-rwg37 → trust criterion-rwg37 (it's the original).
- librw / criterion-rwg37 vs. gta-reversed-modern → trust the engine references (1, 2) for engine APIs; trust gta-reversed-modern only for game-side patterns it specifically reverses.
- Any reference vs. MCP → **MCP wins.** File the discrepancy as `[UNCERTAIN U-NNNN]` and continue the session against what MCP shows. Do not "correct" MCP-observed code to match a reference — the binary is the truth.

## Recommended model per session type

Every RE-session prompt **must** declare its recommended model in the first line of the prompt body, in this format:

```
Recommended model: <claude-sonnet-4-6 | claude-opus-4-7> at <minimal|low|medium|high> thinking. Reason: <one short clause>.
```

The user reads this line before launching the session and picks the model accordingly. The recommendation is a guideline, not a lock; the user can override.

| Session type | Recommended model | Thinking | RVA cap (hard / soft) | Why |
|---|---|---|---|---|
| First-pass discovery (mechanical decomp reads, C0/C1 only) | Sonnet 4.6 | medium | 20 / 10–15 | NO-GUESSING work is procedural; thinking only needs to fund self-discipline checks. |
| Parallel fanout sweeps (subsystem coverage, C0/C1 only) | Sonnet 4.6 | medium | 20 / 10–15 | Same as above; cost dominates at scale. |
| Confidence promotion (`re-classify` C2+, evidence weighing) | Opus 4.7 | high | 10 / 3–5 | Per-RVA evidence work is heavy; promotions are load-bearing. |
| Hook authoring (`hook-author`, RH_ScopedInstall placement) | Opus 4.7 | medium | 5 / 1–3 | One class per session typical; calling-convention quirks need attention. |
| `diff-original` verification (Frida script + trace reading) | Sonnet 4.6 | medium | n/a (per hook, not per RVA) | One hook per session; bounded by Frida output, not RVA count. |
| Multi-session conflict resolution (tracker merges with diverging facts) | Opus 4.7 | high | n/a (bounded by conflict set) | Architecture / fact-merge judgment calls. |
| Subsystem ownership planning (`SESSION_OWNERSHIP.md` carving) | Opus 4.7 | medium | n/a (one-shot) | Architectural decision; cheap to do once well. |
| `.piz` / `.rws` / `.txd` file-format work (pure parser code) | Sonnet 4.6 | low | n/a (per format, not per RVA) | Well-defined formats; deterministic transformations. |
| Master-Ghidra scribe sessions (writing back symbols/types beyond C1) | Opus 4.7 | medium | 15 / 5–10 | Last-writer-wins on master; errors are sticky. |
| Question-answering against trackers / notes (no MCP, no decomp) | Sonnet 4.6 | low | n/a | Plain reading + summarization. |

The "n/a" sessions are bounded by their own scope (one hook, one format, one merge), not by RVA count. The RVA-capped sessions hard-fail-and-split if the subset returned by `function_callees` exceeds the cap; see § "Session sizing (RVA cap, split protocol)" above.

**When to escalate** (override Sonnet → Opus for a session that would normally be Sonnet):
- The subset includes a function the trackers already mark `[UNCERTAIN]` — judging what evidence would resolve it benefits from Opus reasoning.
- The subset crosses subsystem boundaries (e.g., entry function calling into both audio init and renderer init) — boundary classification is a judgment call.
- The subset is the **first** session of a new pattern (first hook of a class, first promotion of the day) — the output sets the pattern for follow-on Sonnet sessions, so spend once on quality.

**When to de-escalate** (override Opus → Sonnet for a session that would normally be Opus):
- Almost never. The cost delta is small enough that the Opus default stands when the rubric calls for it.

## Model-specific guardrails

These rules apply to all models, but Sonnet (and any model below Opus on procedural-discipline benchmarks) drifts from them more often. When running a session on Sonnet, the prompt must inline the four "Sonnet halts" listed below in its body — referencing this file alone is not sufficient, because Sonnet skims linked files after the first read.

**Sonnet halts (must appear verbatim in any Sonnet-bound prompt):**

1. **MCP-down = stop.** If `mcp__ghidra__health_ping` fails or any required MCP call returns an error, halt and surface the error verbatim. Do not retry against `re/prior_art/`, web search, or your own knowledge of similar binaries. Never say "based on what I know about RenderWare/Mashed/this kind of function …" — you do not know this binary; only MCP does.
2. **Verify-then-cite.** Before any RVA appears in conversation, in a note, or in a tracker row, you must have called `mcp__ghidra__function_at` or `mcp__ghidra__listing_code_unit_at` for that exact address in this session and gotten a non-null result. No "rounding" to a nearby valid address.
3. **No hedging, no inference, no naming from names.** Forbidden words: "probably", "likely", "seems to", "appears to", "I think", "presumably", "must be", "is meant to", "is responsible for". A function symbol named `InitGameState` is not evidence that the function initializes game state — it is evidence that someone called the symbol that. Describe what the decomp does, mechanically, and stop.
4. **One function at a time.** Complete the full per-function loop (function_at → decomp_function → function_callees → write the note → update trackers for that one RVA) before starting the next. Do not batch decomps. Do not write five notes and then update trackers in bulk — you will lose state.

**Sonnet anti-patterns to refuse:**

- Trailing summary at end of session ("in summary, this subsystem handles …"). The end-of-session report is the structured one in this file; nothing else.
- Filling in a function's "purpose" when the decomp is hard. Hard decomp → `[UNCERTAIN U-NNNN]` + UNCERTAINTIES.md row. Not a guess.
- Recursing into depth-2 callees because "they were short." Depth-2 goes in DEFERRED.md, full stop.
- Self-correcting a wrong RVA citation by silently swapping it. If you cited an RVA that turns out null, halt and surface the error — do not quietly fix it.
- **Continuing past the hard cap because "we're almost done."** The cap is the cap. At RVA 19 of a 22-RVA subset, file the remaining 3 as DEFERRED, commit, run scribe, end the session. Pushing through to "finish" is exactly when compaction triggers and earlier facts vanish silently. See § "Session sizing (RVA cap, split protocol)."
- **Reusing a base-bucket name across split sessions.** `re/analysis/entry_callees/` and a continuation session writing to the same dir is a `.md`-file collision. Use `entry_callees-cont1`, `-cont2`, etc.

**Opus-bound prompts** can omit the inlined Sonnet halts; the SESSION_RULES.md reference is enough.

## Parallel-fanout scribe-queue pattern

When more than one analysis session runs in parallel, **none of them may attempt the post-analysis scribe step directly**. The branch-local `master.WIP-*` flag is invisible across worktrees that haven't fetched+rebased, so two parallel claimants would both see "no flag" and both write to master, last-writer-wins. Until the scribe-check is made `origin/main`-aware, the workaround is:

### For each analysis session in the fanout

Replace § "Post-analysis scribe step" with the following, only for the duration of the fanout:

1. Complete the analysis loop and commit + push the analysis commit normally.
2. **Do NOT** run `scribe-check`. **Do NOT** create a `master.WIP-*` flag. **Do NOT** open the master writable.
3. Instead, append one row to `re/SCRIBE_QUEUE.md` (under the "Queued" heading):
   ```
   YYYY-MM-DD  $SESSION_SHORT_ID  bucket=<bucket>  rvas=<rva1>,<rva2>,...,<rvaN>
   ```
4. `git add re/SCRIBE_QUEUE.md && git commit -m "queue: <bucket> for sweep ($SESSION_SHORT_ID)"` and push.
5. End-of-session report includes one extra line: `Scribe outcome: queued for sweep (see re/SCRIBE_QUEUE.md row $SESSION_SHORT_ID).`

### One follow-up sweep session (Opus)

After all parallel analysis sessions have committed and pushed, a single Opus sweep session:

1. `git pull --rebase` to gather every queued row.
2. Reads `re/SCRIBE_QUEUE.md`, processes each "Queued" row in order.
3. Runs the standard scribe step from § "Post-analysis scribe step" **once**, with a single `master.WIP-*` flag covering the whole sweep. Per-bucket CHANGELOG rows are still written (one claim line + one release line per drained bucket, plus one outer scribe-claim/release pair for the sweep itself).
4. Moves processed rows from "Queued" to "Drained" in `SCRIBE_QUEUE.md` after each bucket completes its writes.
5. Reports any per-RVA write failures; halts the whole sweep on the first failure (do not skip and continue).

### Why a queue and not a polling lock

- Polling burns tokens.
- Sequential per-session scribe attempts on `main` would still race on flag claim if all sessions have `git pull --rebase` lag.
- A queue file is monotonically appended; rebases compose cleanly because no two sessions touch the same row.
- One sweep session means one master open/close cycle and one sync — minimal master churn, maximum auditability.

## ID range allocation (parallel-fanout only)

When multiple analysis sessions run in parallel and all may file `U-NNNN` / `D-NNNN` / `S-NNNN` rows, **the prompt for each session pre-allocates an ID range**. The session never goes outside its allocation. If a session would exhaust its range, it halts and asks the user for an extension — does not silently overrun.

Format in each prompt's "Subset" section:

```
ID-range: U=<start>..<end>  D=<start>..<end>  S=<start>..<end>
```

End-of-session report must include `IDs used: U-XXXX..U-XXXX (count), D-XXXX..D-XXXX (count), S-XXXX..S-XXXX (count)`.

## When to halt and ask

- Anchor SHA-256 mismatch.
- Pre-flight assert failure (slot, staleness, scribe).
- Any MCP call failure.
- A function whose decomp cannot be described mechanically without inference — file `[UNCERTAIN]` and continue, or halt and ask if it blocks the subset.
- Tracker conflict on `git pull --rebase` that touches the same RVA.
