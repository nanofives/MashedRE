---
description: Run one round of the per-system-area RE + parity loop for $ARGUMENTS (a canonical hooks.csv subsystem, e.g. render).
---

# /area-round $ARGUMENTS

One iteration of the area loop for the **$ARGUMENTS** subsystem. Full protocol
(both roles, coverage math, blockers): **`re/AREA_LOOP.md`** — read it if this is
your first round. This command is the CHILD iteration; the parent orchestrator
protocol (fleet spawn, bus, scheduling, sweeps) also lives in that file.

Constraints (non-negotiable, this config): single account **account2**, single
machine, Ghidra via `analyzeHeadless` + `DecompPC.java` against your own pool slot
— **never Ghidra MCP** (blocked on account2). Star topology: cross-area findings
go to the **parent** via `send_to_session`, never edit `re/CROSS_AREA_BUS.md` yourself.

Steps (abort the round if the gate fails):

0. **Gate** — worktree is yours + `.pool_slot` held; exe anchor SHA matches; a
   baseline `mashedmod\build.bat` succeeds; `scripts/ghidra_assert.sh preflight`.
1. **Scope** — `py -3.12 re/tools/area_residue.py --subsystem $ARGUMENTS --out queue.tsv`;
   take the top K cheapest-win-first; read the ledger's `$ARGUMENTS` section — its
   open visual defects must be closed or re-filed, not dropped.
2. **Work each candidate** — headless decode → author (`hook-author`) → **link the
   `.cpp` into `build.bat`/`asi_sources.rsp`** → `build.bat` → `run_diff.py` +
   `run_verify_hook.py` → on GREEN `re-classify` to C3 and append to `re/PROMOTION_QUEUE.md`.
3. **Cross-area finding** — struct offset / global / shared RVA / dispatcher touching
   another area → `send_to_session` the parent (kind + anchor + one-line claim). Keep working.
4. **Parity** (visual areas) — run the `re/parity/recipes.toml` capture pair + diff by
   hand → `py -3.12 re/tools/parity_scoreboard.py record --recipe <id> --round <N> --from out.txt`
   (BLOCKED recipes like T-ARCTIC → `--blocked`).
5. **Ledger** — append a round row to the `$ARGUMENTS` section; update the dry streak.
   2 consecutive dry rounds → MINED-OUT, tell the parent. COVERED only when residue<C3
   is 0 on linked terms, STUBS section empty, every parity recipe GREEN or BLOCKED.

Under `/loop /area-round $ARGUMENTS` this repeats; the parent decides when to stop.
