# Mashed RE orchestrator — resume point (updated 2026-07-31, end of iter20)

MISSION: dual-lane — (A) fix the game per RE_MASTER_PLAN, (B) promote Ghidra functions.

**C1 796 / C2 4008 / C3 878 / C4 184.** Branch `fix/u9025-recharacterise-and-regabi-defects`,
everything committed through **ec92fde2**, **not pushed** (115 commits ahead of origin/main).
Ledger: 27 promoted / 21 candidate / 9 briefed / 16 blocked.

Resume with `/orchestrate` — it reads `re/orchestrator/state.json`, which is current.

---

## START HERE: the C4 lift for `0x00482900` is one small change away

`asi:ExportName` counters exist in `scenario_launch.py` but are armed **at attach time**, which is
too early — `mashed_re_dev.asi` is not loaded yet (`NOEXPORT`). **Arm them after the menu is up**
(after `wait_phase(1, ...)`), then re-run the canonical race. A non-zero `asi:ReplayGetSize` is the
C4 evidence with no inference in it.

While there, two `run_verify_hook.py` FAILs are tool-assumption mismatches, not defects:
- *"bytes unchanged after Module.load"* — expected when `dinput8` already installed the hook.
- *"bad argument count"* — the path2 caller does not understand the `{'scalars': [...]}` test shape
  used by `ptr_seed_observe` / `stub_dispatch_observe`.

---

## The C4 trap this session walked into — do not repeat

A canonical race showed `0x00482900` firing **13× per run over three runs**. That is **not** C4
evidence: an install probe reported `armed[orig]`, so those were entries into the **original**
function. Counting a hooked RVA proves the *function* ran, never that *our port* ran.

The failed inference: `run_verify_hook` showed the JMP live in **its** process (opcode `0xE9`,
rel32 `0x7358c97b` → `0x73a0f280` = the reimpl), and both tools spawn the same binary the same way.
**Two boots of the same binary are not one observation.** Also: read install bytes **before**
`Interceptor.attach` — attach patches the site, so a later read returns Frida's trampoline.

Memory: `feedback_rva_count_is_not_proof_our_port_ran`.

## `0x00411350` / `0x00411530` cannot be lifted by a race scenario

Both fired **zero** times in every canonical race, while sibling `0x00411600 ReplayRecordFrame`
fired 1738/1856/1774 in the *same* runs — the control that makes the zero an absence, not a dead
probe. They are reachable only when a recorded time is **displayed** (results / time-trial UI),
which the launcher cannot currently reach. A C4 attempt needs that scenario built first.

---

## Standing rules re-confirmed this session (all measured, not stylistic)

1. **Pre-flight before every boot**: `py -3.12 scripts/orch_preflight.py <hook>...`.
2. **Author 3-4 rows, verify in ONE `state_batch` boot.** Three hooks were verified in 0.1 s of
   in-race window this session.
3. **NEEDS_NEW_HANDLER is a hypothesis about the handler inventory, not a fact.** Four consecutive
   runs (16, 19, 20, 20) found the proposed handler already existed or generalised from one that
   did. Check `re/frida/ARG_TYPES.md` before writing anything.
4. **A decompiler summary is not evidence about x87.** Three times this session a summary asserted
   an x87 fact the raw listing refuted. `FUN_004a2c48()` with empty parens says nothing about
   arity — memory `feedback_ftol_empty_parens_says_nothing_about_arity`.
5. **A discriminator must be checked against the truncation, not the operand.** A U-9035 vector
   picked because its operands differed was degenerate: both quotients truncated to 3576.

---

## Lane choice for the next substantial run

The synthetic-leaf lane is thin. Gate pool = **88 rows**: 43 MUTATOR_LANE, 29 NEEDS_GHIDRA,
11 NEEDS_NEW_HANDLER, 1 READY (`0x005aeed0`, deferred by directive). Options:

- **(a)** Plate the **27 ORPHAN_BLOCK** rows — their callback blocks are not wrapped as functions,
  so they cannot be gated at all. Unblocks the largest stuck group.
- **(b)** **Mutator lane** — 43 rows, only 2 AB_READY. Do **not** re-pick the 7 AB_NONDET rows;
  6 of them write heap allocation addresses.
- **(c)** Build a **results/time-trial scenario** in the launcher — unblocks the two Replay rows
  above and any other UI-reachable-only code.

## Hygiene

- **Ghidra pool slot 5 holds a leaked `.lock~`** from a failed MCP open (`rm` → "Device or resource
  busy"). Known in-JVM leak; survives deleting the file and clears only when the MCP JVM restarts.
  **Slot 9 is analyzed, free, and was released cleanly** — use it. Slot 4 is a stale broken clone.
- `scenario_launch.py` occasionally fails its first attach ("could not attach"); a plain retry
  worked. Not investigated.
- All PIDs spawned this session were killed by PID; `original/` intact; no worktrees created.
