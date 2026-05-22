---
rva: 0x004ecaee
name_in_ghidra: FUN_004ecaee
size_bytes: 12
confidence: C1
callees_depth1: [FUN_004fbe7a, PTR_FUN_006187a8]
callers_noted: []
opened_in_slot: Mashed_pool11
session_date: 2026-05-19
subsystem_observed: d3dx9-delay-load-thunk
---

## Mechanical description
- Delay-load forwarder. Variadic: no arg-shuffle (zero stack adjustment), JMPs through `PTR_FUN_006187a8` after init.
- Pattern variant: lacks the explicit arg pass-through of larger thunks, suggesting a 0-arg or this-call-only target.

## Constants
| Address | Value | Note |
|---------|-------|------|
| 0x004ecaf3 | `PTR_FUN_006187a8` | import slot |

## Uncertainties
- [UNCERTAIN] D3DX9 export — could be `D3DXVec3Normalize` or any short signature.

## Stubs encountered
- [STUB] FUN_004fbe7a, PTR_FUN_006187a8.
