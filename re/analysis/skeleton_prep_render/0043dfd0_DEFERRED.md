---
rva: 0x0043dfd0
name: FUN_0043dfd0
size_bytes: 10969
confidence_target: C2
status: DEFERRED — decomp exceeds 300-line C2 mechanical scope
callees_depth1: 63 (too many to enumerate; MCP confirms via function_callees)
callers_noted: [00492d30 (TimerTick)]
opened_in_slot: Mashed_pool10
session_date: 2026-05-22
---

## Deferral reason

Decomp output is 10,969 bytes / 63 callees. The body spans 0x0043dfd0–0x00440aa9 (~2,777 bytes of machine code). The Ghidra decompiler emits ~64KB of C text — far beyond the 300-line C2 mechanical cap for this session.

This function was previously noted in hooks.csv as "10969 bytes; 63 callees; decomp too large; C1 via caller-chain FUN_00492d30". The C1 classification stands. C2 promotion requires a dedicated large-body session.

## What is known (from hooks.csv notes + decomp header scan)

- Caller: `FUN_00492d30` (game-state tick / TimerTick).
- Contains: race-mode input dispatch, scoreboard display, player-slot state machine, Lua-driven powerup init, camera target switch, race-mode transition sequencing.
- Contains `float10` FPU usage (x87 `fVar20`) confirmed from decomp.
- Contains `thunk_FUN_00493f70` and `thunk_FUN_00494480` and `thunk_FUN_00494a80` calls (video/hardware gate).

## Next-step to unblock

Dedicated large-body C2 session (one RVA, ~1 hour). Use `decomp_function` with chunked reads; enumerate 63 callees in batches.
