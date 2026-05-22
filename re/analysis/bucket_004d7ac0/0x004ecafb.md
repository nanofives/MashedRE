---
rva: 0x004ecafb
name_in_ghidra: thunk_FUN_004ecaee
size_bytes: 6
confidence: C1
callees_depth1: [PTR_FUN_006187a8]
callers_noted: []
opened_in_slot: Mashed_pool11
session_date: 2026-05-19
subsystem_observed: d3dx9-delay-load-thunk
---

## Mechanical description
- Pure thunk: `JMP dword ptr [0x006187a8]` (6 bytes).
- Bypasses the FUN_004fbe7a init step that FUN_004ecaee performs.

## Constants
| Address | Value | Note |
|---------|-------|------|
| 0x004ecafd | `[0x006187a8]` | indirect target |

## Uncertainties
- None.

## Stubs encountered
- [STUB] PTR_FUN_006187a8 (D3DX9 export).
