// Mashed RE — synthetic harness stubs (feature/harness-arg-types).
//
// These are NOT production reimplementations. Each export forwards directly to
// the original RVA via a function-pointer cast. The purpose is to exercise the
// four new diff_template.js arg_types end-to-end:
//
//   eax_implicit_ptr / eax_implicit_int   (D-11010 blocker)
//   vec3_global_mul_observe              (D-11011 blocker)
//   fmt_desc_pair_compare                (c3-batch-i-s2 audio fmt refusals)
//
// Because the reimpl just JMPs to the original, the orig-vs-reimpl A/B diff
// is bit-identical by construction. That tests the harness machinery (input
// packing, EAX seeding, global save/restore, fingerprinting, result capture)
// independently of any actual reverse-engineering work.
//
// DO NOT promote these RVAs to C3 on the strength of this scaffold's diff
// results — a true C3 promotion needs a real implementation. These stubs
// exist solely to validate the new arg_types.

#include <cstdint>

// ── EAX-implicit harness target  (D-11010) ─────────────────────────────────
// Original: 0x00497190 FUN_00497190 — sprintf("contcfg%d.bin"); returns ptr.
// The function reads its slot index from EAX (Ghidra `in_EAX`). Our stub
// just JMPs to the original RVA; EAX is preserved by the naked JMP.
__declspec(naked) void HarnessEaxImplicitPtrImpl() {
    __asm {
        mov edx, 0x00497190
        jmp edx
    }
}
extern "C" __declspec(dllexport) void HarnessEaxImplicitPtrStub() {
    HarnessEaxImplicitPtrImpl();
}

// Synthetic int-implicit target — same prologue, different RVA.
// We reuse 0x00497190 (same target) so the diff is symmetric; the difference
// is only how the harness packs the input (pointer-as-EAX vs int-as-EAX).
__declspec(naked) void HarnessEaxImplicitIntImpl() {
    __asm {
        mov edx, 0x00497190
        jmp edx
    }
}
extern "C" __declspec(dllexport) void HarnessEaxImplicitIntStub() {
    HarnessEaxImplicitIntImpl();
}

// ── vec3_global_mul_observe harness target  (D-11011) ──────────────────────
// Original: 0x0046c570 FUN_0046c570 — scales 3 floats at
//   DAT_00881f50/54/58 + param_1 * 0x341 * 4   by _DAT_005ce264.
// Returns constant 1.
// Stub: plain JMP to the original. Standard cdecl(int).
__declspec(naked) int HarnessVec3GlobalMulImpl(int /*idx*/) {
    __asm {
        mov edx, 0x0046c570
        jmp edx
    }
}
extern "C" __declspec(dllexport) int HarnessVec3GlobalMulStub(int idx) {
    return HarnessVec3GlobalMulImpl(idx);
}

// ── fmt_desc_pair_compare harness target  (c3-batch-i-s2) ──────────────────
// Original: 0x005ac5f0 FUN_005ac5f0 — fmt-desc equality predicate.
//   Signature: int fn(int* a, int* b)  → 0/1.
__declspec(naked) int HarnessFmtDescPairImpl(int* /*a*/, int* /*b*/) {
    __asm {
        mov edx, 0x005ac5f0
        jmp edx
    }
}
extern "C" __declspec(dllexport) int HarnessFmtDescPairStub(int* a, int* b) {
    return HarnessFmtDescPairImpl(a, b);
}
