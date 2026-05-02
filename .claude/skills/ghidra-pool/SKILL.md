---
name: ghidra-pool
description: Manage the Ghidra project pool for Mashed RE — acquire/release/create slots on demand. Use this skill any time you need to open a Ghidra project for read or write, especially when multiple Claude sessions might run in parallel. Triggers when the user mentions Ghidra, decompilation, RE work, or asks to "open the project", "look at function X", "decompile Y".
---

# Ghidra Pool — Mashed

Ghidra locks projects with a `.lock` file the moment a session opens them. With multiple Claude sessions or with the headless MCP server running, you need separate clones of the master project. This skill manages that pool **on demand** — slots are created the first time they're needed, not pre-allocated, to save disk (each clone is a full copy of the analyzed `.rep`).

## When to use

- Before any `mcp__ghidra__project_program_open_existing` call.
- When `LockException` appears in MCP output.
- When the user says "open Ghidra", "look at function …", "decompile …", "investigate the binary", or runs `/re`.
- When the user wants to free a slot, refresh slots after edits, or check pool state.

## Architecture (one-time read)

```
C:\Users\maria\Desktop\Proyectos\Mashed\
├── Mashed.gpr / Mashed.rep              # MASTER (GUI-edited; symbols, types, comments live here)
├── Mashed_headless.gpr / Mashed_headless.rep   # OPTIONAL second project for headless writes
└── mashed_pool\                         # CLONES (read-only sessions consume these)
    ├── Mashed_pool0.gpr / .rep / .lock
    ├── Mashed_pool1.gpr / .rep / .lock
    └── ...                              # up to MAX_SLOTS=8, created lazily
```

The Ghidra binary lives in TD5RE (`C:\Users\maria\Desktop\Proyectos\TD5RE\ghidra_12.0.3_PUBLIC`) — **shared, do not duplicate**. Ghidra MCP is also shared from TD5RE (`ghidra-headless-mcp\`).

## Standard acquire/release flow

```bash
# Step 1: acquire (creates slot 0 first call, reuses unlocked slots after)
SLOT=$(bash "C:/Users/maria/Desktop/Proyectos/Mashed/scripts/ghidra_pool.sh" acquire)
echo "Got slot: $SLOT"

# Step 2: open the slot through MCP — ALWAYS read_only=true unless writing
mcp__ghidra__project_program_open_existing
  project_location="C:/Users/maria/Desktop/Proyectos/Mashed/mashed_pool"
  project_name="$SLOT"
  program_name="MASHED.exe"
  read_only=true

# Step 3: do work (decompile, look up functions, etc.)

# Step 4: close + release
mcp__ghidra__program_close
bash "C:/Users/maria/Desktop/Proyectos/Mashed/scripts/ghidra_pool.sh" release ${SLOT#Mashed_pool}
```

## Commands

| Command | What it does |
|---|---|
| `init` | Verifies master exists; first-run setup |
| `acquire` | Prints `Mashed_poolN`. Reuses unlocked slot if any; creates new slot otherwise. Hits `MAX_SLOTS=8` limit. |
| `release [N]` | Clears lock files for slot N, or all slots if N omitted |
| `sync` | Refreshes every unlocked slot from master `.rep` (run this after a GUI editing session) |
| `status` | Prints pool state |
| `cleanup` | Wipes all lock files everywhere (use after crashed sessions) |
| `add [N]` | Pre-creates N new slots immediately |
| `remove <N>` | Deletes slot N from disk (must be unlocked) |

## Decision tree for the agent

1. Is there a `Mashed.gpr` master yet? Check `C:\Users\maria\Desktop\Proyectos\Mashed\Mashed.gpr`.
   - **No** → tell the user to open Ghidra GUI once, create project `Mashed` in `C:\Users\maria\Desktop\Proyectos\Mashed`, import `original\MASHED.exe`, run auto-analysis, save, exit. **Do NOT auto-create the project** — first analysis takes 10–30 minutes and should be supervised.
   - **Yes** → continue.
2. Run `acquire`. If it errors with "all slots locked":
   1. Run `cleanup`.
   2. Run `acquire` again.
3. Open the slot read-only via MCP.
4. After work: `program_close` + `release N`.

## Writing back to master

Slots are read-only by convention. To persist symbol/type changes:

- Option A: open `Mashed.gpr` (the master) directly with `read_only=false` from a single session — never run two writers at once.
- Option B: open `Mashed_headless.gpr` (a second master, dedicated to MCP writes) and merge to `Mashed.gpr` later via Ghidra's project sync. Recommended only once the workflow stabilizes.

After any master write, run `sync` to push changes to all unlocked pool slots.

## NO-GUESSING coupling

This skill is the entry point to deterministic RE work. Whenever you load a slot, the conversation should follow the project's NO-GUESSING rule:

- Report only what decompilation literally shows; cite the address for every constant.
- Never invent meaning for unclear values — report raw hex/decimal and mark `[UNCERTAIN]`.
- Never use words like "probably", "likely", "seems to", "appears to", "I think".

See `CLAUDE.md` for the full rule.
