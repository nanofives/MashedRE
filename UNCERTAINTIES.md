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
| U-0007 | structural | 0x00402750 sub_00402750 | Vtable call at offset 0xe4 from *(int*)DAT_00771a0c; type of object at DAT_00771a0c unknown; exact instruction addresses for vtable dereference and constants 0xe4, 0x5, 0x19, 0x18 not pinpointed | decomp + listing_code_unit_at within 0x402750–0x402a21 for vtable block | Decomp FUN_004caea0 (0x004caea0) to determine return type; listing at call site | C3 |
| U-0008 | semantic | 0x00402750 sub_00402750 | Arg 0x3e99999a to FUN_00431b40: raw hex 0x3e99999a; as IEEE 754 single = 0.300000012; whether calling convention treats this as float or int not determinable | Signature of FUN_00431b40 | Decomp FUN_00431b40 (0x00431b40) | none |
| U-0009 | structural | 0x004924f0 sub_004924f0 | Inner switch (cases 0–11) uses pointer arithmetic mixing iVar2/iVar3 with fixed data addresses; exact effective addresses per iteration not pinpointed | listing_code_unit_at calls within 0x4924f0–0x49268d | mcp__ghidra__listing_code_units_list on function body; manual offset trace | none |
| U-0010 | structural | 0x004c5930 sub_004c5930 | Structure layout of param_1: offsets +0x8, +0x10, +0x14 accessed; linked-list shape inferred from pointer equality and double-unlink ops; struct type unknown | Type analysis of param_1 callers | Decomp callers (FUN_00402750 at 0x004b3d80 return path; FUN_00402a40 at DAT_00636ac8 path) | C3 |
| U-0011 | structural | 0x004c5930 sub_004c5930 | DAT_007d3ff8 (0x007d3ff8) and DAT_007d4054 (0x007d4054) used as indirect bases for field and vtable access at +0x11c; types and relationship not determinable from this decomp | Callers and writers of these two data addresses | mcp__ghidra__reference_to 0x007d3ff8 + 0x007d4054 | C3 |

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
