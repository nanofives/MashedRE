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
| Ghidra headless via subprocess (`xtwin.py`, `analyzeHeadless.bat`) | ✅ launcher present | slow (~30–60s each), single-writer |
| **Ghidra MCP (`mcp__ghidra__*`)** | ❌ **unavailable** | off the managed MCP allowlist; ToolSearch returns nothing |
| Figma / claude.ai connectors | ❌ need OAuth | non-interactive session can't run the auth flow |

**One hard blocker:** interactive Ghidra MCP is off-allowlist, so the `scribe-transcriber` /
`leaf-decoder` agents and any live MCP decomp chain-chasing are dead on account2. Decomp is
still *obtainable* the slow way through `xtwin.py` / headless subprocess — usable for reading a
specific twin, not for bulk fan-out.

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
- Decomp via `xtwin.py` / headless subprocess (no MCP) — single-writer, slow.
- Frida diffs — installed, but spawning MASHED + hot-path rules apply; case-by-case, not batch.

**Out of reach on account2 — queue for account3:**
- Interactive Ghidra-MCP discovery / scribe batches.
- `re-classify` **C4 promotions** (need live Frida canonical-scenario evidence).
- Figma / claude.ai connector work.

## Rule of thumb

When account3 is down: keep momentum on **Lane A audits + Lane B build/tool/quality passes**;
queue anything needing Ghidra-MCP or C4 evidence for account3. This is also the CLAUDE.md cost
lever — read-heavy exploration is exactly what account2 is for.

Never fabricate command output, web results, or test results to paper over a blocked capability;
STOP and flag it instead.
