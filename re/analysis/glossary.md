# Glossary

Standard vocabulary for Mashed RE work. One source of truth so terminology doesn't drift across sessions / sub-agents / docs.

## Project shape

| Term | Meaning |
|---|---|
| **Greenfield / Source port** | The end state: a standalone `mashed_re.exe` that loads Mashed's data files and runs without `MASHED.exe`. The shipping target. |
| **Dev harness** | The temporary infrastructure used during development: `mashed_re_dev.asi` injected via Ultimate-ASI-Loader (`d3d9.dll`) into the live `MASHED.exe` for behavioral diff verification. Not shipped. |
| **Hook mode / Standalone mode** | The two build configurations of `mashed_re`. Hook mode produces `mashed_re_dev.asi`; standalone mode produces `mashed_re.exe`. Same source tree. |

## Reverse engineering

| Term | Meaning |
|---|---|
| **RVA** | Relative virtual address — the offset of a function/datum from the binary's image base. We cite them as `0x00xxxxxx`. Anchor: the SHA-256 of `original\MASHED.exe`. |
| **Hook** | A `0xE9 <rel32>` JMP overwritten onto the original function's first 5 bytes, redirecting execution to our reimplementation. |
| **Inline-JMP installer** | The ~200-line custom hook installer (gta-reversed-style) that writes the JMP, manages permissions, and tracks toggleable hooks. |
| **Toggleable hook** | A hook whose enabled/disabled state can be flipped at runtime. Required for `diff-original` A/B testing. |
| **Frida** | Dynamic instrumentation toolkit. We use it to attach to a running process and trace function args/return/memory at specific RVAs. |

## Confidence

| Term | Meaning |
|---|---|
| **C0..C4** | The 5-level confidence rubric. C0 = unknown, C1 = located, C2 = disassembled, C3 = understood, C4 = verified. See `re/CONFIDENCE.md`. |
| **F-DoD / S-DoD / P-DoD** | Definition of Done at function / subsystem / project levels. See `ROADMAP.md`. |
| **Promotion / Demotion** | Moving a function up or down the C0..C4 ladder. Only the `re-classify` skill mutates these. |

## Bookkeeping

| Term | Meaning |
|---|---|
| **`hooks.csv`** | Single source of truth for every reversed function. One row per RVA. |
| **Stub (S-NNNN)** | A placeholder for a not-yet-reversed callee. Inline as `// STUB S-NNNN`. Tracked in `STUBS.md`. Blocks S-DoD until cleared. |
| **Uncertainty (U-NNNN)** | An explicit knowledge hole. Inline as `[UNCERTAIN U-NNNN]`. Tracked in `UNCERTAINTIES.md`. Blocks C3 if semantic or structural. |
| **Deferred (D-NNNN)** | Work explicitly out of scope for now. Tracked in `DEFERRED.md`. Has a re-pickup condition. |
| **NO-GUESSING rule** | The discipline forbidding "probably/likely/seems"; require RVA-cited evidence. See `CLAUDE.md`. |

## Workflow

| Term | Meaning |
|---|---|
| **Worktree** | A git worktree under `.worktrees/<name>/`, branched off `main`, bound to its own Ghidra pool slot. Used for isolated subsystem sweeps or experiments. |
| **Pool slot** | A read-only clone of `Mashed.gpr` at `mashed_pool/Mashed_poolN.{gpr,rep}`. Locked while a session has the program open. |
| **Master** | The canonical `Mashed.gpr` / `Mashed.rep`. The only place symbols/types/comments are authored. |
| **Scenario** | A reproducible sequence of game inputs (e.g., "skip intros, race lap 1 of Egypt with vehicle Bullet"). Frida diffs reference scenarios. Stored under `verify/scenarios/`. |
| **Subsystem ownership** | Coordination convention: while a worktree owns a subsystem, only it can promote functions there. Tracked in `re/analysis/SESSION_OWNERSHIP.md`. |

## Asset formats

| Term | Meaning |
|---|---|
| **`.piz`** | Mashed's custom asset archive. Header `PIZ\0` + v3 + count + 2KB pad; per-entry 116B name + 4B offset + 4B length + 4B id; data 2KB-aligned. Raw-byte offsets confirmed. |
| **`.RWS`** | RenderWare 3.7 binary stream — audio dictionaries in Mashed's case. Standard format; tools like RWS Reader and RW QuickBMS work. |
| **`.TXD`** | RenderWare texture dictionary. Standard format. |

## Tools

| Term | Meaning |
|---|---|
| **Ghidra MCP** | `mrphrazer/ghidra-headless-mcp` — the Model Context Protocol server giving the agent access to Ghidra's analysis. 212 tools. Wired in `.mcp.json`. |
| **Frida MCP** | `0xhackerfren/frida-game-hacking-mcp` — MCP server for Frida memory scan / hook / modify operations. Wired in `.mcp.json`. |
| **Ultimate-ASI-Loader** | ThirteenAG's d3d9-proxy DLL that loads `*.asi` files at process startup. The dev-harness loader. |
| **`analyzeHeadless`** | Ghidra's command-line entry point. Used to bootstrap the master project and to run scripts like `count_functions.py`. |
| **`build.bat` (TBD)** | The MSVC x86 build script for `mashed_re.exe` and `mashed_re_dev.asi`. Calls `vcvars32.bat`, runs `cl` + `link`. Not yet authored (Phase 4). |

## Git / multi-session

| Term | Meaning |
|---|---|
| **`nanofives`** | The git/GitHub identity used for every commit on this repo. Configured local-scope. |
| **Tracker** | Any of `hooks.csv`, `STUBS.md`, `UNCERTAINTIES.md`, `DEFERRED.md`. Mutated only via `re-classify`. |
| **Stop-and-ask** | The mandatory rule: when a request is ambiguous, an action is destructive, or evidence for a confidence promotion is missing — halt and ask the user, do not infer. See CLAUDE.md "Operating principles". |
