# Uncertainties

Every `[UNCERTAIN]` marker dropped in an analysis note or comment gets one row here. Uncertainties are not bugs — they are explicit holes in our knowledge with a path-to-resolution. Knowledge gaps without rows here are **invisible** and forbidden.

A function cannot reach C3 while it has unresolved uncertainties **of type semantic or structural** (see Type column). Cosmetic uncertainties (e.g., "name doesn't match RW convention exactly") may stay open through C4 if scoped tightly.

## Active uncertainties

| ID | Type | Where | Statement | Evidence missing | Path to resolution | Blocks |
|----|------|-------|-----------|------------------|--------------------|--------|
| U-0001 | structural | 0x004a4bb7 (entry) | Four bytes written to `local_114.szCSDVersion[0x7c..0x7f]` (0xcf,0x4b,0x4a,0x00) before GetVersionExA — purpose unclear | Instruction-level listing at these offsets | Read listing at those instructions; check if compiler generates them as stack-init or SEH-related | none |
| U-0002 | semantic | 0x004a4bb7 (entry) | `local_20` boolean set from multi-step module-header parse (0x5a4d/0x4550/0x10b or 0x20b check, then dword at offset 0x3a or 0x3e) | Instruction-level listing to confirm exact field offsets and which data-directory entry is read | Read listing at PE-parse block; cross-check against known PE32 optional header layout at C2+ | none |
| U-0003 | provenance | 0x004a31f3 (FUN_004a31f3) | `PTR_FUN_00616044` global pointer to function — called if non-null, but what writes it before this call is not visible | Cross-references to 0x00616044 showing writer(s) | mcp__ghidra__reference_to 0x00616044 to find initialization site | none |
| U-0004 | structural | 0x004a31f3 (FUN_004a31f3) | `LAB_004a78f4` (0x004a78f4) passed to `_atexit`; Ghidra labels it LAB_ not FUN_; one byte past end of FUN_004a78b0 | Listing at 0x004a78f4 to determine if this is dead code, intra-function label, or undefined function start | mcp__ghidra__listing_code_unit_at 0x004a78f4 + context read | none |
| U-0005 | structural | 0x004a78b0 (FUN_004a78b0) | Loop bounds `for (p = &DAT_005e7b84; p < &DAT_005e7b84; ...)` — start and end are identical address; body never executes as decompiled | Instruction-level listing at loop bounds to check whether two distinct pointers were collapsed or table is genuinely empty | mcp__ghidra__listing_disassemble_function 0x004a78b0 or listing_code_units_list around 0x005e7b84 | none |
| U-0006 | structural | 0x004aa3fe (__heap_init) | Reads `in_stack_00000004` (stack +4) as implicit parameter to determine HeapCreate first arg; not visible as named param in signature | Instruction-level listing to identify source of stack-offset read | mcp__ghidra__listing_code_unit_at around 0x004aa3fe body start | none |

## Resolved (audit trail)

| ID | Type | Resolved date | Resolution |
|----|------|---------------|------------|
|    |      |               |            |

## Types

- **semantic** — what does this value mean? (e.g., "is +0x14 a velocity or an acceleration?")
- **structural** — what is the shape of this struct/buffer?
- **environmental** — does this depend on game state we don't track? (RNG seed, frame parity, save flag)
- **provenance** — is this code from RW SDK or game-specific?
- **cosmetic** — naming/style; doesn't block correctness

## Conventions

- ID format: `U-NNNN`, monotonic, never reused.
- Every `[UNCERTAIN]` in source/notes must cite a U-NNNN.
- Resolution must include the cited evidence (RVA, log line, diff output).
