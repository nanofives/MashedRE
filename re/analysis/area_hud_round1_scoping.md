# hud area — round 1 scoping (2026-09-01)

Worktree `area-hud`, pool slot `Mashed_pool14`. `/area-round hud` round 1.

## Verdict

**No clean cheap C2->C3 win exists in hud this round.** Same shape as render round-0.
Landed 0 C3. Every top-of-queue candidate is a trap or a heavy asm-exact port. Details
below with per-candidate evidence (NO-GUESSING: nothing forced).

Residue snapshot (`area_residue.py --subsystem hud`): 77 below C3; implemented .cpp = 2,
linked = 2 (both the font traps below); doc-only = 75.

## Why each cheap-looking candidate is not a win

- **00552e40 `FontCtx_FlushMatrix`** (implemented+linked, Frida "10/10 GREEN"): the GREEN is
  `crash_equal_ok` — both sides fault on a NULL camera (`DAT_00912c0c`) at the quiescent menu,
  inside callee `0x004c0ed0` dereferencing `cam+4`, BEFORE FlushMatrix's own compose work runs.
  The matrix-compose semantics are therefore **unverified**; promoting on this diff is a
  documented false-green ([[scratch-field-false-green]]). The old caller-gate refusal now
  arguably passes under the identified-caller clause (callers `00552890/00552920` are C1
  `third-party-library[renderware]` Rt2d with a documented library role), but the crash-equal
  evidence still does not verify the body. **Rejected.**
- **00552b60 `FontSys_InitSeq`** (implemented+linked): Frida diff impossible at quiescent menu
  — 7 alloc/init callees deadlock past the 60 s deadline (TIMEOUT x2, per hooks.csv). Runtime-
  state blocked. **Skipped.**
- **004128f0 / 00413b80 / 00413bb0 / 00413f50** (VehicleIcons sprite-batch module): all `void`,
  side-effecting on globals / GPU render-state / the global vertex array `&DAT_00828320`, or
  register calling conventions (`004128f0` = `in_EAX`/`unaff_EDI` unrecovered `__usercall`).
  None returns a clean value; a bit-identity A/B would be crash-equal / weak = the FlushMatrix
  trap again. **Rejected.**
- **00455b50 / 0047d640 / 0047def0**: 51 / 284 / 414-line per-frame event-marker emitters,
  `__thiscall` (record in EAX) or deeply stateful, 5-9 callees each. Too big/stateful for a
  round-1 diff. **Deferred.**
- **00412cf0** (label-trail record appender): looked clean (8 explicit args, pure function of
  args + `.rdata` consts, gate satisfied: caller `00412f30` C2, callee `Vec3Magnitude` C3). But
  the **Ghidra decomp is materially lossy** (see finding F3) — a faithful port needs asm-exact
  x87-frame work, so it is not a *cheap* win. **Deferred to a dedicated asm-transcription round.**

## Findings (reported to parent for the cross-area bus)

- **F1 (hud->render, module-reclass).** The `0x00553000-0x00557fff` "font-vector 2D" band
  (`00554010/150/200/390`, `00555830`, `00556780`, `00556e40`, `005571c0/e0`, `FontSys_*`) is
  suspected **vendored RenderWare Rt2d** per `module-vendor-doubt` in ~15 plates. Corroboration:
  FlushMatrix's callers `00552890/00552920` are **already** reclassed `third-party-library[renderware]`
  Rt2d. A confirmed Rt2d calibration would `reclass-OUT` ~15 rows hud->library-skip C1, shrinking
  hud residue. Was previously mislabeled `render` (batch_ao). Parent decides on running the
  calibration + one reclass-OUT txn.
- **F2 (hud->frontend, doc-correctness).** `FUN_004a2c48` is `__ftol` (x87 float->int64, **C3**,
  `Math/FPURound.cpp`), NOT "QPC tick" as the `util_c0_promote/0x00412cf0.md` plate labels it.
  Any note keying on the "QPC tick" reading of `004a2c48` is wrong.
- **F3 (hud->gameplay, signature-correction).** `FUN_004726f0` (C3, `gameplay`, plated `void
  FUN_004726f0(float*, float*)`) **returns a float on ST0**: in `00412cf0` the FPU stack is empty
  after `Vec3Magnitude`'s `FSTP` (`0x00412d33`), then `CALL 0x004726f0` (`0x00412d6f`), then
  `FCOM [0x005d757c]` (`0x00412d74`) reads ST0 — that value can only be `004726f0`'s return. Its
  result (clamped `Kc = min(K, eps)`) feeds byte `+0x27` of the record via
  `(u8)__ftol(Kc*Kc * _DAT_005cd04c * <arg>)` — a chain the `00412cf0` decomp drops entirely.
  If the port's `004726f0` is typed `void`, callers depending on the ST0 return break.

## Tooling added

- `re/tools/ghidra_scripts/DisasmPC.java` — dumps raw disassembly (addr : bytes : mnemonic) for
  the function containing an RVA, via `analyzeHeadless` (no MCP). Fills the gap that `DecompPC.java`
  has no disasm mode and MCP `listing_disassemble_function` is blocked on account2. Used above to
  establish F3 authoritatively.
