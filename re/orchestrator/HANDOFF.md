# Mashed RE orchestrator — resume point (updated 2026-07-31, end of iter21)

MISSION: dual-lane — (A) fix the game per RE_MASTER_PLAN, (B) promote Ghidra functions.

**C1 796 / C2 4006 / C3 879 / C4 185.** Branch `fix/u9025-recharacterise-and-regabi-defects`,
committed through **95d42f4e**, **not pushed** (121 commits ahead of origin/main).
Ledger: 29 promoted / 21 candidate / 8 briefed / 15 blocked.

Resume with `/orchestrate` — it reads `re/orchestrator/state.json`, which is current.
iter21 ran its full 4-cycle budget; hard stop #1 (context) ended it.

---

## START HERE: the ladder is empty of authorable work again — refill it

Cycle 4 consumed both READY rows. `orch.ps1 next` will show `briefed` items, but every one
of them tallied **0 READY** — they are stuck, not actionable. **Refilling is the job**, not a
detour (see the skill's "Refill" note): pick a bucket and brief **>=12 RVAs** on the
read-fleet with `-MaxConcurrent 2..4`. It is off-quota; iter21's 11-RVA brief cost $1.55.

Highest-value refill targets, in order:

1. **Plate `0x005515a0` C1->C2.** It is the sole owner of `0x0052ddc0` and `0x0052df40`, so
   one plate converts both to gate-PASS. Cheapest structural win on the board.
2. **The 3 NEEDS_NEW_HANDLER rows** — `0x004b6b00`, `0x005bfb90`, `0x00407550`. This is the
   **fifth consecutive run** to return that verdict; all four prior ones already existed or
   generalised from one that did. **Check `re/frida/ARG_TYPES.md` before writing anything.**
3. **The 14 NO_OWNER ORPHAN rows** — need a different method than the reference-chain BFS,
   which ran out of edges on them. Do not re-run that method on them.

---

## What iter21 did

### 1. `0x00482900 ReplayGetSize` C3 → C4 (the lift iter20 refused)

The iter20 **refusal was correct**; its **diagnosis was not**, and the difference cost
two boots. The handoff said the `asi:` counters were armed too early, before `dinput8`
loads the `.asi`. Arming them after `wait_phase(1)` changed nothing — still `NOEXPORT`.

Two independent defects, both now fixed (`c555600b`):

1. **`scenario_launch.py:623` sets `MASHED_RE_NO_AUTO_HOOK=1` whenever `--hooks` is
   empty.** Every canonical race run for the lift was **stock original with no hooks
   installed at all**. `armed[orig]` was not a timing artifact — there was nothing
   installed to read. This is documented behaviour ("Empty = stock original"), not a bug,
   but it is a trap: **a scenario_launch run without `--hooks` says nothing about any
   hook.**
2. **The static `Module.findExportByName(moduleName, symbol)` was removed in Frida 17.**
   It threw `TypeError`, the `catch` swallowed it to `null`, and a null is
   indistinguishable from "the .asi is not loaded yet" — exactly the load-order story the
   handoff had primed. The lookup now tries the module-instance API first and, on failure,
   **prints the loaded module list** instead of a bare `NOEXPORT`.

Evidence: `asi:ReplayGetSize = 13` in each of two canonical races run with
`--hooks 0x00482900`, counter on the **.asi export** (reachable only through the installed
JMP). Control `asi:ReplayRecordFrame = 0` — armed at `0x739cce40`, export resolves, but not
in `MASHED_HOOK_ONLY`, so no JMP routes to it. path2 0 FAIL, returns 2572 / 1744 through
the live JMP. Full writeup: `verify/c4_replay_get_size/EVIDENCE.md`.

Also fixed, both `run_verify_hook.py` FAILs the last handoff flagged: *"bytes unchanged
after Module.load"* is now a PASS when the site holds a correct JMP, and *"bad argument
count"* was the path2 dispatcher having no case for `{'scalars': [...]}`. path2 now
forwards `stub_at`/`stub_nargs`/`stub_ret` too — without it, it could never exercise these
rows at all.

### 2. ORPHAN_BLOCK caller-gate: 11 of 27 rows unblocked

`re/orchestrator/ORPHAN_GATE_RESOLUTION.md` + `orphan_owners.tsv`.

**The first method was wrong and is recorded as such.** Walking back from the orphan site
to the nearest preceding defined function resolves all 51 sites and is unsound: site
`00407687` walks back 8 instructions into `FUN_00407640`, but the only reference to its
block start comes from function `00481a30`. **Physical adjacency is not ownership.**

Replaced with a backward BFS over the reference graph that follows chained orphan blocks
and hops through jump-table data slots. Result: **11 PASS**, **2 OWNER_BELOW_C2** (both
owned solely by `0x005515a0` at C1 — one plate converts both), **14 NO_OWNER** (BFS ran out
of edges; unresolved, not disproved — **do not re-screen them with this method**).

An owner here is a **reference-chain witness, not a `function_callers` edge**. It
establishes which function's control flow reaches the block, not the call's arguments.

### 3. `0x00407580` + `0x005b1160` C2 -> C3, both verified in ONE boot

Both from the ORPHAN_BLOCK group unblocked in cycle 2. **Both authored from the raw
listing**, and in both cases the listing carried something the decompiler did not:
`0x005b1160`'s third argument is a **byte** load (`MOV CL,byte ptr [ESP+0xc]` at
`0x005b1172`) though the caller pushes a dword, and its three zero stores run in the order
`+1, +2, +0`.

`0x005b1160` is the strong row: 6 vectors, 6 distinct fingerprints, and the **0xAA buffer
seed is load-bearing** — three of its five writes store zero, so against a zero-filled
buffer a port that omitted them would have passed.

`0x00407580` is the **weaker** row and its note says so: a pure read of a live global, so
only the address computation is under test. Its sentinel was deliberately set to the
**array itself** (`0x00639dc4`) so an unpopulated array refuses the run rather than scoring
a column of zeros GREEN — `selected_copter_field60` documents that same array as
unpopulated in Quick Battle, and the snapshot confirmed the first 8 dwords **are** zero.
10 seeds gave 2 distinct values (indices 12/16/24 -> `0xbf800000`). A real discriminator,
but thinner than a fingerprint-per-vector row.

**Pre-flight failed both rows on the first pass** — `ring_header_init` lacked `observe_ret`
and neither had a `scenario_sentinel`. Two boots saved by a millisecond check.

---

## Standing rules (all measured; unchanged)

1. **Pre-flight before every boot**: `py -3.12 scripts/orch_preflight.py <hook>...`.
2. **Author 3-4 rows, verify in ONE `state_batch` boot.**
3. **NEEDS_NEW_HANDLER is a hypothesis about the handler inventory, not a fact** — now 5
   consecutive runs. Check `re/frida/ARG_TYPES.md` before writing anything.
4. **A decompiler summary is not evidence about x87.** Read the raw listing.
5. **A discriminator must be checked against the truncation, not the operand.**
6. **NEW (iter21): a harness reading of "absent" has two causes — the thing is absent, or
   the probe is broken — and they look identical.** `NOEXPORT` meant "the API you called no
   longer exists", not "not loaded". Make probes report *why* they failed, not just that
   they did.

## Lane choice for the next substantial run

- **(a)** ~~Author the 2 READY rows~~ — **done in iter21 cycle 4**; both promoted C2→C3.
- **(b)** Plate `0x005515a0` C1→C2, which converts `0x0052ddc0` and `0x0052df40` to PASS.
  **This is now the cheapest win** — see START HERE.
- **(c)** Build a **results/time-trial scenario** in the launcher. Still the only route to
  `0x00411350` / `0x00411530`, which fired zero times in every race while sibling
  `0x00411600` fired ~1800 in the same runs. **Now cheaper than it was**: the asi-export
  counter works, so reachability can be measured directly instead of inferred.
- **(d)** Mutator lane — 43 rows plus 5 more from iter21. Do **not** re-pick the 7 AB_NONDET
  rows; 6 of them write heap addresses.

## Hygiene

- **Pool slots 5 and 10 hold leaked `.lock~`** (in-JVM leak from a failed MCP open; `rm`
  reports "Device or resource busy"; clears only when the MCP JVM restarts). Slot 10 was
  leaked **this session** — `ghidra_pool.ps1 acquire` handed it out and the open failed
  with `LockException`. **Slot 9 works and was used and released cleanly.** Slot 4 is a
  stale broken clone.
- `mashed_pool/Mashed_poolN/` **subdirectory** projects are stale 0-byte duplicates — open
  the root-level `mashed_pool/Mashed_poolN.gpr` only.
- All MASHED PIDs spawned this session were killed by the harness; `original/` intact; no
  worktrees created.
