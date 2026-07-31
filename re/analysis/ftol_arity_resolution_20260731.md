# `__ftol` arity-suspect rows — RESOLVED (2026-07-31, orch-iter20)

The iter18 `__ftol` plate audit rated two rows **ARITY_SUSPECT / arity_risk HIGH**: they declare
`(void)` parameters while calling `FUN_004a2c48` (`__ftol`) with *no float setup visible in the
decompile*. The audit was explicit that it could only rate **risk**, never confirm a defect,
because none of the 24 plates it read contained raw disassembly.

Both are now decoded from the listing. **Both ratings are FALSE ALARMS — the `(void)` parameter
lists are correct.** In each case the `__ftol` operand comes from a **global**, which the
decompiler simply did not surface into its C output. "No float setup visible" was a statement
about the decompile, not about the function.

Source: Ghidra `Mashed_pool9`, read-only, session closed cleanly. Anchor BDCAE093.

---

## 0x00423040 — `void FUN_00423040(void)` — CONFIRMED no arguments

The audit's concern was that *"the very first two decompile operations are both FUN_004a2c48
calls at 0x00423048 and 0x0042304e … Those calls must consume floats from ST0/ST1 that the
decompile cannot see"*, concluding *"the true arity is likely 2 float parameters"* and *"a port
from this plate as `void()` would silently drop two arguments."*

The actual first instructions are:

```
00423040  d9055c1a7f00      FLD   float ptr [0x007f1a5c]     ; <-- GLOBAL
00423046  53                PUSH  EBX
00423047  d80d00ca5c00      FMUL  float ptr [0x005cca00]
0042304d  56                PUSH  ESI
0042304e  57                PUSH  EDI
0042304f  e8f4fb0700        CALL  0x004a2c48                 ; __ftol
00423054  d905581a7f00      FLD   float ptr [0x007f1a58]     ; <-- GLOBAL
0042305a  d80d00ca5c00      FMUL  float ptr [0x005cca00]
00423067  e8dcfb0700        CALL  0x004a2c48                 ; __ftol
```

The float setup is present and explicit — `FLD` from `0x007f1a5c` and `0x007f1a58`. Note also
that the audit's cited call addresses (0x00423048, 0x0042304e) are **wrong**: the calls are at
0x0042304f and 0x00423067; 0x0042304e is the `PUSH EDI`. That is the same off-by-a-few-bytes
citation error found on the secondary `0x00482900` plate in iter17.

- No `[ESP+N]` read anywhere resolves above the pushed registers, so there are no stack args.
- Plain `RET` at 0x00423212 / 0x00423245 / 0x00423259 / 0x0042326b (four exit paths).
- `EAX` is not set on a consistent path before the RETs — return unused.

**Mechanically:** reads globals `0x007f1a5c` / `0x007f1a58` (floats), scales both by
`[0x005cca00]`, converts each with `__ftol` into `ESI = -20 - n1` and `EDI = -12 - n2`. Then
three byte-flag-gated blocks (`0x007f1042`, `0x007f1076`, gated further on
`[0x007f1a54] == 0 && [0x007f1a64] == 4`) call `0x00417450` and `0x00417530`. Then four
near-identical key-repeat blocks keyed on `0x007f1044/45/46/47`, each managing a counter pair
(`0x006440fc`/`0x00644108`, `0x0064410c`/`0x006440f4`, `0x006440ec`/`0x00644104`,
`0x006440f0`/`0x006440f8`) with the constants 1, 10 and 2, and nudging the float globals by
`±[0x005cc564]`.

**NOT a C3 batch candidate:** 555 bytes, writes at least eight globals, `WRITES_GLOBAL` safety
class. It belongs in the mutator lane, not the synthetic-leaf lane. Recorded here so the arity
question is closed and the row is not re-screened for it.

---

## 0x004325c0 — `int FUN_004325c0(void)` — CONFIRMED no arguments, and NOT void-return

```
004325c0  a1a4ec6700        MOV   EAX,[0x0067eca4]           ; <-- GLOBAL
004325c5  83ec08            SUB   ESP,0x8                    ; 8 bytes of locals
004325c8  83f804            CMP   EAX,0x4
004325cb  7c08              JL    0x004325d5
004325cd  d90574c55c00      FLD   float ptr [0x005cc574]     ; <-- .rdata constant
004325d5  d90520c35c00      FLD   float ptr [0x005cc320]     ; <-- .rdata constant
```

Stack accounting: `SUB ESP,8` plus four `PUSH`es (EBX/EBP/ESI/EDI) puts ESP 24 bytes below
entry, so a first argument would live at `[ESP+0x1c]`. **The only stack reads in the body are
`[ESP+0x10]` and `[ESP+0x14]`**, which fall inside the 8-byte local area — they are scratch used
for `FILD`/`FIADD` int-to-float staging at 0x004326db..0x00432712. Nothing at `[ESP+0x1c]` or
above is ever read. `ADD ESP,8` then plain `RET` at 0x004327f3 confirms __cdecl with no args.

**The return is NOT void.** `MOV EAX,[ESP+0x10]` at 0x004327e6 loads the local initialised to 1
at 0x004325e5 and cleared to 0 at 0x0043260d, and returns it. So `undefined4 (void)` — the
plate's return type was right; only the "arity HIGH risk" was wrong.

The `__ftol` operands here are `[0x007f1004]` (a global) multiplied by `ST1` (the constant
`FLD`'d in the prologue) and by one of `[0x005cc9fc]` / `[0x005cd904]` / `[0x005ccd04]` /
`[0x005cd8f8]` — again all globals and .rdata, never arguments.

**Mechanically:** loops `ESI` over the table `0x00898ad0 .. 0x008990e8` at **stride 0x34**,
switching on `[ESI-0x10]` against the tags `0xff000000`, `0xff040000`, `0xff100000`,
`0xff110000`, `0xff120000`, `0xff130000`, `0xff230000`; per entry it accumulates a scaled
delta into `[ESI]`, clamps at `0x190`/`0x1ff`, and updates a state word at `[ESI-0xc]`
(values 0/1/2/0x1000). Calls `0x0042ac50` twice.

**NOT a C3 batch candidate:** 563 bytes, table-walking global mutator.

---

## What this says about the audit method

The audit did exactly the right thing by rating these HIGH-risk rather than claiming a defect —
its own note said *"If a raw listing is available it should be checked before treating it as
CLEAN."* That check has now run, and both cleared.

The generalisable point is that **`FUN_004a2c48()` rendered with empty parentheses says nothing
about the caller's arity.** `__ftol` takes ST0, so its operand can come from a stack argument
(`0x00482900`: `FLD [ESP+4]`), from the function's own computation (`0x004150e0`: `p[1]*4.0`),
or from a global (`0x00423040`: `FLD [0x007f1a5c]`). Only the first is an arity defect, and only
a listing distinguishes them. A screen that flags every `(void)`-plus-`__ftol` plate will keep
producing this false-positive class.
