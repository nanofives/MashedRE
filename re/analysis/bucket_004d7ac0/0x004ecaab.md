---
rva: 0x004ecaab
name_in_ghidra: FUN_004ecaab
size_bytes: 66
confidence: C1
callees_depth1: []
callers_noted: []
opened_in_slot: Mashed_pool11
session_date: 2026-05-19
subsystem_observed: rw-math-vec2-baryc
---

## Mechanical description
- Vec2 barycentric interpolation: `P = V0 + (V1 - V0) * f + (V2 - V0) * g` where `param_5 = f` and `param_6 = g`.
- Inputs `param_2/3/4` are V0/V1/V2 Vec2 (param_2 = V0 — base).
- Output Vec2 in `param_1[0..1]`.

## Constants
| Address | Value | Note |
|---------|-------|------|
| none (pure arithmetic) | | |

## Uncertainties
- None — canonical barycentric formula.

## Stubs encountered
- None.
