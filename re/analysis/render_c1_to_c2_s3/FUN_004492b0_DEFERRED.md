# FUN_004492b0 — SkyDomeRender — DEFERRED (no Ghidra function)

RVA: 0x004492b0  
U-id: U-4376  

## Status

Ghidra has no function defined at 0x004492b0 (`function_at` and `decomp_function` both return
"no function found"). The address falls inside a gap between FUN_004491e0 (body_end 0x004492ac)
and the next defined function.

## Existing C1 note (hooks.csv row 805)

"SKY_FN; 0x004492b0-0x0044942a; positions sky clumps at camera via FUN_004491e0; disables
z-write/fog; RpClumpRender([ESI+0x100f0]); unanalyzed in Ghidra needs function_create"

## Required action for C2

Run `function_create` at 0x004492b0 in Ghidra (or `listing_disassemble_seed` + `function_create`),
then re-run decomp_function. Defer to SCRIBE_QUEUE for master-project update.

## Uncertainty

[UNCERTAIN] Whether 0x004492b0 is a valid function entry or alignment padding. Evidence missing:
no disassembly available via MCP on read-only pool clone.
