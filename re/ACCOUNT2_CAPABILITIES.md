# account2 (Accenture worker) — what it can drive on Mashed

**Purpose:** keep the Mashed RE project moving when the personal account (account3) is
unavailable. This is an *empirical* capability matrix — every row was verified by actually
running the probe in a live account2 session, not read off policy.

- **Probed:** 2026-07-23 (session on `CLAUDE_CONFIG_DIR = C:\Users\maria\.claude-account2`).
- **Policy source of truth:** `CLAUDE.local.md` (managed-config mirror, v27). This doc records
  what actually executed; where the two disagree, re-probe before trusting either.
- Re-verify after any managed-policy change (watched by `scripts/watch-accenture-policy.ps1`).

## Capability matrix (verified)

| Capability | Result | Evidence from probe |
|---|---|---|
| Read / Grep / Glob, read-only git (`log/status/diff/show`) | ✅ works | `git log` ran clean |
| Python 3.12 + RE tooling | ✅ works | `piz_extract.py --help` ran; `py -3.12` = 3.12.10 |
| Frida | ✅ installed | `import frida` → 17.9.3 |
| Write / Edit files | ✅ works | wrote scratchpad + this doc, no denial |
| Builds (`mashedmod\build.bat`, cl.exe) | ✅ reachable | vcvars/cl path intact (may prompt once) |
| WebSearch / WebFetch | ✅ works | live search returned RenderWare results (was doc-flagged "unconfirmed") |
| Ghidra headless via subprocess (`decomp_pc.py`, `xtwin.py`, `analyzeHeadless.bat`) | ✅ **verified end-to-end** | 2026-08-12: 120-function sweep in **9.2 s**; ~8 s fixed + ~0.075 s/fn decomp (measured, see below) |
| **Ghidra MCP (`mcp__ghidra__*`)** | ❌ **unavailable** | off the managed MCP allowlist; ToolSearch returns nothing |
| Figma / claude.ai connectors | ❌ need OAuth | non-interactive session can't run the auth flow |

**One hard blocker (narrowed 2026-08-12):** Ghidra MCP is off-allowlist, so the
`scribe-transcriber` / `leaf-decoder` agents — which are *defined* in terms of `mcp__ghidra__*`
tools — cannot run on account2, and neither can interactive turn-by-turn MCP chain-chasing.

**But PC decomp itself is no longer a bottleneck.** `re/tools/decomp_pc.py` +
`re/tools/ghidra_scripts/DecompPC.java` drive `analyzeHeadless` against a read-only pool clone
and batch **N addresses per invocation**, emitting JSON (signature, size, decomp, callees,
callers, xrefs, strings).

**Measured 2026-08-12** (don't reuse the "~30–60 s" figure — that describes `xtwin.py` on the
larger `Mashed_Console` project, and does *not* transfer here):

| run | wall time |
|---|---|
| 1 fn + decomp | 8.1 s |
| 20 fns + decomp | 9.6 s |
| 120 fns, xrefs+strings, no decomp | 9.2 s |

≈ **8 s fixed** (JVM + project open) **+ ~0.075 s per decompiled function**. This kills the old
"not for bulk fan-out" caveat outright — a 120-address sweep costs about what one address does.
Bulk decode legs and leaf pre-screens are fully viable on account2. What stays mildly awkward is
*iterative* chasing, since each hop pays the 8 s open again; `--callees`/`--callers`/`--xrefs`
fetch a whole frontier in one pass to avoid that.

> **Gotcha for anyone extending this:** never pass a comma-separated string as a script arg.
> `analyzeHeadless` is a `.bat` and cmd.exe splits argv on commas, so `"decomp,callers"` arrives
> as `"decomp"` and the remaining modes vanish **silently** — no error, just missing output.
> Pass separate tokens. (Cost a debug cycle on 2026-08-12.)

`xtwin.py` remains the Xbox-twin path (`Mashed_Console`, single-writer — don't run two at once).
Both are *static* witnesses only: per CLAUDE.md they never substitute for a Frida diff and cannot
move a C-level on their own.

**Not a bug:** `MASHED.exe` SHA `1110BFB5…` ≠ the CLAUDE.md anchor `BDCAE093…`. That is
expected — boot patches are applied and the anchor is preserved on `MASHED.exe.unpatched`.
Do not "fix" it.

## Work lanes on account2

**Lane A — pure-text, zero-risk, high volume (the sweet spot):**
- Source audits of `mashedmod\src\mashed_re\` — call-site maps, subsystem inventories,
  "which hooks touch X", dead-stub hunts.
- Tracker analysis over `hooks.csv` (5,896 rows) + `STUBS.md` / `UNCERTAINTIES.md` /
  `DEFERRED.md` — summaries, backlog ranking, inconsistency finds. (Reads only; mutations
  still go through the `re-classify` skill.)
- Cross-reference `re\prior_art\` (SciLor) and existing `re\analysis\` plates vs the port.
- Draft analysis notes / plates **from decomp already captured** in `re/analysis/`.

**Lane B — runs locally, needs execution but works here:**
- `mashedmod\build.bat` compile checks (catch regressions).
- Python RE tools: `piz_extract`, txd/rws dumpers, save editors, `drawlist_diff.py`,
  `imgdiff.py`, `nav_coverage.py`.
- Code quality on already-written hooks (`/code-review`, `/simplify`).
- Web research for RenderWare / DirectX / file-format questions.

**Lane C — possible but slow / careful:**
- **Batched PC decomp via `re\tools\decomp_pc.py`** (no MCP) — pool-slot, `-readOnly`; batch all
  addresses into one call. Good for bulk decode legs and leaf pre-screens; poor for iterative
  chain-chasing.
- Xbox-twin decomp via `xtwin.py` — single-writer, slow, one function per run.
- Frida diffs — installed, but spawning MASHED + hot-path rules apply; case-by-case, not batch.

**Out of reach on account2 — queue for account3:**
- The `scribe-transcriber` / `leaf-decoder` agents (MCP-defined) and interactive MCP
  chain-chasing. Note the *decomp* they'd produce is now obtainable via `decomp_pc.py`; it's the
  agent plumbing that's blocked, not the data.
- `re-classify` **C4 promotions** (need live Frida canonical-scenario evidence).
- Figma / claude.ai connector work.

## Rule of thumb

When account3 is down: keep momentum on **Lane A audits + Lane B build/tool/quality passes**;
queue anything needing Ghidra-MCP or C4 evidence for account3. This is also the CLAUDE.md cost
lever — read-heavy exploration is exactly what account2 is for.

Never fabricate command output, web results, or test results to paper over a blocked capability;
STOP and flag it instead.
