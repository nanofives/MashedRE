# D-11056 — Fable Ghidra findings (2026-07-03)

Companion to `d11056_collectible_feed_survey_2026-07-03.md` (the claude2 survey). This records the
Ghidra-MCP results the survey handed off to Fable. Read-only pool13; no master writes.

## The call site (the "zero static callers" was a Ghidra artifact)
`reference_to 0x00405780` returns exactly ONE xref: an `UNCONDITIONAL_CALL` at **0x0047b793**
(`E8 e89ff8ff` = `CALL 0x00405780`). `function_callers` reported 0 only because the routine containing
that call — starting at **0x0047b720** — was never bound to a Ghidra FUNCTION object, so the static
call-graph edge is missing though the CALL instruction exists.

Call-site prologue (0x0047b788–0x0047b798): the routine tokenizes a string arg, copies a name via a
byte loop, converts tokens to ints (`CALL 0x004b7090` + `CALL 0x004a2c48` __ftol), builds an index
array, then:
```
PUSH EBP            ; count  = token count
LEA EDX,[ESP+0x14]; PUSH EDX   ; int*  = &index_array
LEA EAX,[ESP+0x2c]; PUSH EAX   ; char* = &name_buffer
CALL 0x00405780                ; the registrar
```
So **`FUN_00405780(char* name, int* indexList, int count)`** — matches the survey's implementation sketch.

## Dispatch mechanism: string-keyed callback registry (NOT a type-indexed fn-ptr table)
`reference_to 0x0047b720` → one DATA ref at **0x00440fc5**:
`PUSH 0x47b720 (handler) ; PUSH 0x5cda9c (key str) ; CALL 0x0047b980 (register)`.
An installer block (0x00440f90–0x00440fe6) registers a family of named track-script (KTC) handlers:

| key str | handler | calls 0x00405780? |
|---|---|---|
| `KTC_AddPickUp` (0x5cda8c) | 0x0047b7d0 | **NO** |
| `KTC_NewCopter` (0x5cda9c) | **0x0047b720** | **YES** |
| `SetCopter` (0x5cdaac) | 0x0047a320 | no |
| `RainSetDirection` (0x5cdab8) | 0x00491a70 | no |
| `RainSetCameraScale` (0x5cdacc) | 0x00491a10 | no |

At track-load the KTC parser looks up the command name and invokes the handler through the stored
fn-ptr. **The `KTC_NewCopter` command is the sole feeder of `FUN_00405780`.**

## Correction to the survey premise (important)
`DAT_00639d80` / `DAT_0063a5d0` is the **copter / moving-waypoint-object array**, not a generic
"pickup" list. `KTC_AddPickUp` is a *separate* handler (0x0047b7d0) that does **not** call
`FUN_00405780`. `FUN_00405780` registers one copter object (name + waypoint-index list) into
`DAT_00639d80` and increments `DAT_0063a5d0`.

`0x00406ce0` is a **pure per-frame tick** over the `DAT_0063a5d0` records (calls `FUN_004064c0`
completion-updater + spline/render updates); it never calls `FUN_00405780` and never increments
`DAT_0063a5d0`. Spawn/registration happens solely at KTC-parse time via 0x0047b720.

## Open question (blocks the fix decision) — [UNCERTAIN]
Does the **rule-5 win predicate's `collectTotal`** count the copter objects fed by `KTC_NewCopter`
(→ `DAT_0063a5d0`), or the `KTC_AddPickUp` objects (a different array)? The only feeder of
`DAT_0063a5d0` via `FUN_00405780` is `KTC_NewCopter`; whether rule-5 reads that array or the pickup
array is unresolved. **Next step (Fable/Ghidra):** find what the rule-5 SegmentCheck/CollectAllDone
path reads for its total, and whether `KTC_AddPickUp` (0x0047b7d0) populates that same array.

## Fix path (once the array question is resolved)
Standalone wiring options: (a) run the `KTC_NewCopter` (and/or `KTC_AddPickUp`) handler per matching
command while parsing the track's KTC script; or (b) call `FUN_00405780(name, idxArray, count)` (and
the pickup registrar) directly per object during track load. Feed `collectTotal` from whichever array
the rule-5 predicate actually reads.
