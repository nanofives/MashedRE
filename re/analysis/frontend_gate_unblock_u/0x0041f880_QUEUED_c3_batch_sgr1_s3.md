# 0x0041f880 — QUEUED (not promoted) by c3-batch-sgr1-s3, 2026-06-25

**Status:** stays **C2**. Could not be promoted C2→C3 in this batch because the
assigned `arg_type` `void_setter_observe` cannot express this function's contract.
Per the batch calibration + project STOP-AND-ASK rule, queued rather than
force-fit or invented into a new arg_type.

## Why void_setter_observe does not fit

`FUN_0041f880` is a **thiscall struct mutator** (verified from the plate +
disasm), not a scalar→global setter:

- `this` arrives in **ECX/EDI** (register), not as a stack scalar.
- It **reads** flag bits from `this+0x294` (0x10 / 0x20 / 0x400) and selects a
  mode code (0x3b..0x40).
- It **writes** the mode code to `this+0x28c` — a **per-instance struct field**,
  not a fixed global.
- When the guard `DAT_00636acc == 0`, it forwards `this+0x290` (a slot index) +
  the computed sub-value to `FUN_00422aa0` (SlotFieldSet), which writes
  `DAT_0064131c + slot*0xf40`.

The `void_setter_observe` harness (`re/frida/diff_template.js:283`) does exactly:

```js
fn(input >>> 0);                       // input passed as a single scalar arg
return ptr(CONFIG.target_global).readU32();   // read back ONE fixed global
```

It therefore **cannot**:
1. pass a controlled struct pointer as `this` (it passes a scalar),
2. seed `this+0x294` flag bits before the call (no struct-seed step),
3. set `DAT_00636acc=0` (no seed_globals on this path),
4. read back `this+0x28c` (the observable is a per-`this` field, not a fixed
   global the registry can hardcode).

A bit-identity diff under `void_setter_observe` would either be uncontrolled
(both sides read identical garbage flags → spuriously GREEN, zero discriminating
power) or crash (unseeded `this+0x290` slot index → wild SlotFieldSet write).

## Path to promotion (next session)

This is the `struct_call_observe` family (`diff_template.js:940`, already in
`SEEDED_ARG_TYPES`): allocate a struct, seed `+0x294` flags, set the ECX/`this`
convention, observe `+0x28c` (and optionally the SlotFieldSet target). It also
needs `DAT_00636acc` seeded to 0 and the `+0x290` slot pointed at a controlled
SlotFieldSet table slot. Re-queue under a session that authors a
`struct_call_observe` (or fastcall_reg `thiscall_field_get`-style) entry — NOT
`void_setter_observe`. Reimpl file `Gameplay/SlotFlagModeMap.cpp` was NOT
authored (no point until the harness exists).

Callee gate is already satisfied (SlotFieldSet 0x00422aa0 is C3), so only the
verification harness blocks it.
