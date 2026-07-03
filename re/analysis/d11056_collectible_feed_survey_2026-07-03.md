# D-11056 collectible-feed survey (claude2, 2026-07-03)

Goal: trace who calls the collectible registrar `FUN_00405780` (0x00405780) so
the standalone's `TrackRenderer::SetCollectibles` / `OnCollect` can be wired
faithfully to populate `DAT_0063a5d0` / `DAT_0063a5d4` (the collectTotal /
collectDone counters that `FUN_00405890` tests for the rule-5 win).

Binary anchor: MASHED.exe SHA-256 (unpatched)
  BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E

## What is already known (from existing plates)

| RVA | Name | Role |
|---|---|---|
| 0x00405780 | FUN_00405780 | Collectible registrar: adds one `0xec`-byte record to `DAT_00639d80`, writes `+0x5c=100.0f` default, variable-length index list at `+0xb4`, increments `DAT_0063a5d0`. **Zero static callers** (U-8997). |
| 0x00405890 | FUN_00405890 | Rule-5 win predicate: `collectDone == collectTotal && collectTotal != 0`. Callers: `FUN_00410510`, `FUN_00410d10`, `FUN_00429e10`, `FUN_00446520`, `FUN_00464a50`. |
| 0x004064c0 | FUN_004064c0 | Per-collectible completion updater: when `(100.0f - field+0x5c)*0.01 >= 1.0`, increments `DAT_0063a5d4`. Caller: `FUN_00406ce0`. |
| 0x004074a0 | FUN_004074a0 | Reset/cleanup: zeroes `DAT_0063a5d0` and `DAT_0063a5d4` via RpClumpDestroy path (per existing plates). |

## The indirect-call problem (U-8997)

`mcp__ghidra__function_callers` on `0x00405780` returns `count: 0` (confirmed in
two independent Ghidra sessions: `bucket_00405400-20260518` and
`race_rules_d1-20260702`). Both existing plates also list `callers_noted: []`.

This means the registrar is **not** called via a direct CALL instruction visible to
Ghidra's static call graph. It is almost certainly invoked via:
- A **function pointer** stored in a vtable or table at track-load time (the
  per-track object-spawn loop iterates a callback table), OR
- A **computed call** (`CALL EAX`/`CALL [EDX+offset]`) from an object-spawn
  dispatcher that is not in the static call graph.

## What the source structure tells us

The standalone exposes:
```cpp
// In mashedmod/src/mashed_re/Race/...
void TrackRenderer::SetCollectibles(int count);   // sets DAT_0063a5d0
void TrackRenderer::OnCollect(int idx);            // increments DAT_0063a5d4
```

These are currently **unfed**: `collectTotal` is never set to a nonzero value in
the standalone, so rule-5 races are perpetually unwinnable (win predicate guard
`if (collectTotal == 0) return false` always fires). The legacy fallback from the
2026-07-03 review (D-11052 resolution) prevents a soft-lock, but rule-5 is still
non-functional.

The real feed wiring must call `FUN_00405780` once per collectible object at
track-load time. The question is: what object-spawn loop calls it?

## Survey of candidate dispatch paths (from sources + existing notes)

### Candidate A — `FUN_00406ce0` (caller of FUN_004064c0)

`FUN_00406ce0` calls `FUN_004064c0` (the per-collectible update tick). It is
therefore the per-tick update dispatcher for all collectible records. Its callee set
(at depth 1) likely includes the per-object update that walks `DAT_00639d80`. If
`FUN_00406ce0` also handles spawn/registration, it may call `FUN_00405780` via a
vtable slot.

- [FABLE]: Pull `mcp__ghidra__function_callers` + `mcp__ghidra__decomp_function`
  on `0x00406ce0` to trace how it iterates the collectible array and whether it
  dispatches registration.

### Candidate B — object spawn loop in track-loader band (0x0047xxxx)

The `COLLI*.BSP` + object table loader (R3, already ported as Track subsystem)
creates game objects for each track item. Collectible objects are distinguished by
their type ID. The spawner likely calls a type-dispatch function that routes to
`FUN_00405780` for collectible types.

The track-loader band around `0x00470000–0x00475000` is where object types are
typically registered. A candidate is any function in this band that calls
`FUN_004a2b60` (the struct/string init copy that FUN_00405780 calls as its first
step — searching callers of `FUN_004a2b60` narrows the set of registrar call
sites).

- [FABLE]: `mcp__ghidra__function_callers` on `0x004a2b60` to find all sites
  that call the struct/string init. Any site that also writes `0x42c80000` to
  a record's `+0x5c` field is a smoking gun for FUN_00405780.
- [FABLE]: PCODE search (`search_pcode` or `reference_to 0x00405780`) in the
  headless project to find computed-call sites that reference the registrar's
  address.

### Candidate C — function-pointer table keyed by object type

The `FUN_00406ae0` mentioned in the `bucket_gameplay_00405400_00407620`
plate (consumer of the `DAT_00639d80` array alongside `FUN_00405f60` /
`FUN_00406410`) may be the per-object-type dispatch that routes to different
handlers including `FUN_00405780`. If there is a table at a known address
(keyed by the collectible type byte), one of its entries IS `&FUN_00405780`.

- [FABLE]: Decompile `FUN_00406ae0` and `FUN_00405f60` / `FUN_00406410` (all
  from `bucket_gameplay_00405400_00407620`) and check whether any uses a
  function-pointer table indexed by object type.

## What the standalone needs (implementation sketch)

Once Fable identifies the call site:

1. At track-load time, for each collectible object in the track's object table:
   - Call `FUN_00405780(param_1, &index_list[0], count)` where:
     - `param_1` = the object descriptor (the `FUN_004a2b60` src argument)
     - `index_list[0..count-1]` = the waypoint/index integers for this object
     - `count` = number of waypoints
   - This increments `DAT_0063a5d0` (collectTotal) automatically.

2. Per-frame, for each collectible in `DAT_00639d80[0..collectTotal-1]`:
   - Call `FUN_004064c0` with `in_EAX = &DAT_00639d80[i]` (this-pointer).
   - This drives the completion fraction and increments `DAT_0063a5d4` when
     a collectible is fully collected.

3. On round reset: call `FUN_004074a0` or zero `DAT_0063a5d0` and `DAT_0063a5d4`
   directly (matching the reset in the existing `RuleEngine.cpp` `Persist` struct).

The standalone already has the `collectTotal` / `collectDone` fields in
`RuleEngine::Persist` mirroring these globals; the feed just needs to populate
them.

## FABLE items (hand-off)

See FABLE HAND-OFF section in the session output.
