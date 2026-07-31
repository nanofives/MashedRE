# ORPHAN_BLOCK caller-gate resolution (orch-iter21 cycle 2)

27 rows in `caller_gate_144.tsv` carry verdict `ORPHAN_BLOCK_CALLERS`: every call site
reaching them lies in code Ghidra never wrapped as a function, so `function_callers`
returns 0 and the C3 caller-gate cannot be evaluated at all. Memory
`feedback_unwrapped_blocks_invisible_to_callers`.

## Method, and the first method that was WRONG

The obvious approach — walk backwards from the orphan site to the nearest preceding
defined function — resolved all 51 sites and is **not sound**. Control case: site
`00407687` walks back 8 instructions into `FUN_00407640`, but the only reference to its
block start `00407670` comes from function `00481a30`. Physical adjacency is not
ownership; the orphan block simply sits in the gap after an unrelated function's body.
The walkback table was discarded.

What was used instead is a backward BFS over the reference graph from the block start:
- source inside a function -> that function is an OWNER
- source is orphan CODE -> continue from *its* block start (chained orphan blocks)
- source is DATA -> follow references to the data address (a jump-table slot, which is
  how the indirect `JMP` in the real owner is reached)

Block start = walk back until the previous instruction is a flow terminator (RET or
unconditional JMP) or is inside a function.

Raw output: `re/orchestrator/orphan_owners.tsv` (site, block, hops, owners).

## Result

| verdict | rows | meaning |
|---|---|---|
| PASS | 11 | at least one owner at C2+; caller-gate is now evaluable and passes |
| OWNER_BELOW_C2 | 2 | sole owner `0x005515a0` is C1 — promoting it unblocks both |
| NO_OWNER | 14 | BFS terminated in data/orphan with no function source |

**PASS (11)** — rva -> owner(conf):

    0x004b6b00 (3)   -> 004b6640:C2
    0x00407580 (17)  -> 00459000:C2
    0x00471430 (22)  -> 00426340,0045bae0,0047a020,0047a0f0,0047a130 (all C2)
    0x0047cde0 (27)  -> 0045bba0:C2
    0x005b1160 (29)  -> 005b74c0:C2, 005bce80:C2
    0x005bfb90 (30)  -> 005be260:C2
    0x005bfa20 (32)  -> 005be260:C2
    0x00407550 (35)  -> 00481a30:C2
    0x004722e0 (37)  -> 00472380:C2
    0x00497470 (62)  -> 00498510:C2
    0x00497b10 (74)  -> 004982a0:C2

**OWNER_BELOW_C2 (2)**: `0x0052ddc0` (35) and `0x0052df40` (45), both owned solely by
`0x005515a0` at C1. One C1->C2 plate on that function converts both to PASS.

## Scope of the claim

An owner here is established by a **reference chain**, not by Ghidra's `function_callers`.
The chain is structural — direct jumps and jump-table slots — so it is evidence, not
inference, but it is a *different* witness from a normal caller edge and the gate note on
each row should say so. It establishes which function's control flow reaches the block;
it does not by itself establish the call's arguments.

The 14 NO_OWNER rows are not disproved, only unresolved: the BFS ran out of reference
edges. They stay blocked and should not be re-screened by this method.
