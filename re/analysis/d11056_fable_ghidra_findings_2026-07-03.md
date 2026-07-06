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

## Resolved 2026-07-04 (Ghidra, Mashed_pool14): rule-5 reads the copter array exclusively

`FUN_00405890` (rule-5 win predicate) decompiles to:
```c
bool FUN_00405890(void)
{
  if (DAT_0063a5d0 == 0) { return false; }
  return DAT_0063a5d4 == DAT_0063a5d0;
}
```
Both operands are `DAT_0063a5d0`/`DAT_0063a5d4` — the copter/waypoint-object counters.

`reference_to 0x0063a5d0` (20 xrefs) confirms the only writers are `FUN_00405780` (0x00405879, the
registrar's `DAT_0063a5d0 = DAT_0063a5d0 + 1`) and `FUN_004074a0` (0x004074fa, wipe-all reset to 0
at track load). `FUN_00405780`'s decompile confirms it writes the copter record into the
`DAT_00639d80`-based array (stride `0xec`, indexed by the pre-increment value of `DAT_0063a5d0`) and
then increments `DAT_0063a5d0` — i.e. `DAT_0063a5d0` is the **total registered copter count**.

`FUN_004064c0` (the per-object per-frame completion tick over that same array, `this`-pointer
`in_EAX`) contains the sole writer of `DAT_0063a5d4`: `DAT_0063a5d4 = DAT_0063a5d4 + 1;` gated on
`*(int*)(in_EAX+0x48) == 0 && _DAT_005cc320 <= local_1c` (first-time completion of that copter's
waypoint list). So `DAT_0063a5d4` is the **completed-copter count**, incremented once per copter
object as it finishes its route.

**`KTC_AddPickUp` (0x0047b7d0) does NOT feed this pair.** Disassembly of its body (it is not a bound
Ghidra FUNCTION object, so `decomp_function` errors — same artifact as `KTC_NewCopter`) shows it
tokenizes 5 floats via `CALL 0x004b7090`/`FSTP`, then calls **`CALL 0x00405730`** — a different
registrar entirely. `FUN_00405730`'s decompile:
```c
void FUN_00405730(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)
{
  int iVar1 = (&DAT_0063a478)[param_1];
  int iVar3 = (iVar1 + param_1 * 10) * 0xc;
  undefined4 uVar2 = FUN_00458e00(param_2,param_3);
  *(undefined4 *)(&DAT_0063a228 + iVar3) = uVar2;
  (&DAT_0063a478)[param_1] = iVar1 + 1;
  *(undefined4 *)(&DAT_0063a220 + iVar3) = param_4;
  *(undefined4 *)(&DAT_0063a224 + iVar3) = param_4;
}
```
It writes into `DAT_0063a220`/`0x0063a224`/`0x0063a228`/`0x0063a478` — an address range disjoint from
`DAT_00639d80`/`DAT_0063a5d0`/`DAT_0063a5d4`. `KTC_AddPickUp` objects are confirmed to be an
unrelated pool that never reaches the rule-5 predicate.

**Conclusion:** the rule-5 `collectTotal`/`collectDone` pair is fed exclusively by the
`KTC_NewCopter` chain (`FUN_00405780` registrar incrementing `DAT_0063a5d0`; `FUN_004064c0`
per-object completion tick incrementing `DAT_0063a5d4`). `KTC_AddPickUp` is out of scope for rule-5.
Closes U-8997; unblocks D-11056.

### Definitive xref closure (both arrays, per the reader-survey's minimal check list)

`reference_to 0x0063a5d0` (20 xrefs) and `reference_to 0x0063a5d4` (5 xrefs) exhaustively
account for every write:
- `DAT_0063a5d0`: written only by `FUN_00405780` (0x00405786/0x004057e4 read, 0x00405879
  `+= 1`) and `FUN_004074a0` (0x004074fa, full wipe to 0).
- `DAT_0063a5d4`: written only by `FUN_004064c0` (0x004066ea read, 0x00406703 `+= 1`, the
  completion tick) and two reset sites — `FUN_004074a0` (0x00407500, full wipe) and a
  **newly-decompiled `FUN_004073b0`** (0x0040748c): a per-object field reset over the
  `DAT_00639d80` array (indices `0..DAT_0063a5d0-1`, i.e. it does NOT touch the total count)
  that ends with `DAT_0063a5d4 = 0; DAT_0063a5d8 = 0;`. This is a **per-round soft reset**
  (re-arm completion state, keep the registered-object count) as distinct from
  `FUN_004074a0`'s full wipe (new track/race) — structurally the same two-tier reset the
  standalone now implements (`SetRaceRule` = full `SetCollectibles(total)`; `StartRound` =
  `collectDone`/per-copter `done` soft reset only). No unexplained writer exists for either
  counter; the "which array" question is closed with full xref coverage, not just call-graph
  coverage (which is known to miss unbound-function edges per the `KTC_NewCopter` case above).

## Fix path
Standalone wiring: run the `KTC_NewCopter` handler logic per matching command while parsing the
track's KTC script — for each `KTC_NewCopter` object, register it (name + waypoint-index list) into
the standalone's collectible-count state, incrementing `collectTotal`; increment `collectDone` when a
copter's route completes (mirrors `FUN_004064c0`'s first-time-completion gate). `KTC_AddPickUp` is not
part of this feed and needs no changes for rule-5.
