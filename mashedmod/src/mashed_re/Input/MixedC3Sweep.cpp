// Mashed RE — Input mixed-cluster C3 sweep (wave1-s6).
//
// SHA-256 anchor: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//                 (preserved in original\MASHED.exe.unpatched)
//
// ─── 0x004960e0  FUN_004960e0 (WM_ACTIVATE vtable dispatcher) — DEFERRED ────
//   Callee gate blocks C2->C3:
//     All callees are indirect via vtable (no static RVAs resolvable).
//     The analysis note (re/analysis/window_msgpump/004960e0.md) explicitly states:
//       "Zero new stubs (vtable targets cannot be filed as stubs without their RVAs)."
//       "a future C3 of this function requires the vtable target identities to be
//        resolved at runtime first."
//     [UNCERTAIN U-0648]: DAT_00772fb8 object type unknown; vtable slot identities
//       at +0x1c and +0x20 require Frida runtime resolution.
//   The C2->C3 callee gate cannot be met: "at least one callee at C2 or higher"
//   requires at least one statically-resolved callee. With all callees indirect and
//   unresolved, the gate cannot be verified.
//   Re-pickup condition: Frida runtime resolution of DAT_00772fb8 and its vtable
//   slots +0x1c/+0x20 per U-0648's stated path; bring resolved callees to C2+.
//   Analysis: re/analysis/window_msgpump/004960e0.md

// This file is intentionally empty of implementations.
// It is not compiled into the ASI (no entry in build.bat).
