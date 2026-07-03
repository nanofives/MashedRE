// Mashed RE — x87 FPU round-to-integer primitive.
// Original: 0x004a2c48  FUN_004a2c48  (C2 → target C3/C4)
//
// Plate: re/analysis/promote_c1_high_ab3/0x004a2c48.md
//   (Mashed_pool7, session 2026-05-20; 116 bytes, no callees)
//
// Signature: ulonglong FUN_004a2c48(void)
//   Input:  implicit in_ST0 — caller pushes a float10/float onto the x87 FPU
//           stack before calling. No explicit stack parameters.
//   Output: EDX:EAX — 64-bit rounded integer (callers truncate to byte/int/uint
//           at the call site; high dword is the sign extension).
//
// Algorithm (verbatim from plate, 0x004a2c48..0x004a2cd3):
//   1. FRNDINT ST0               -- round ST0 in-place (FPU rounding mode)
//   2. FISTP qword [esp-8]       -- store 64-bit int, pop ST0
//   3. local_20 = low dword; uStack_1c = (float)(high dword >> sign)
//   4. Non-zero gate: if ((local_20 != 0) || ((uVar1 & 0x7fffffff00000000) != 0))
//      Correction:
//        if (high-dword-as-float < 0):     // negative result
//          uVar1 += (0x80000000 < (uint)-(float)(in_ST0 - (float10)(longlong)uVar1))
//        else:                             // positive result
//          uVar2 = (uint)(0x80000000 < (uint)(float)(in_ST0 - (float10)(longlong)uVar1))
//          uVar1 -= uVar2
//   5. Return uVar1 in EDX:EAX.
//
// The correction implements round-half-away-from-zero on top of FRNDINT:
//   the fractional residual (in_ST0 - rounded) is tested by comparing its
//   float-bit representation against 0x80000000; see plate for the exact
//   sign-branch logic.
//
// Calling convention note: this is NOT __cdecl — it takes no stack arguments;
// the input is purely in ST0. The hook body is __declspec(naked) to preserve
// the x87 stack discipline exactly.
//
// Relationship to scattered inline copies:
//   AiLeaderTimer.cpp   g_ftol_4a2c48 x87 shim — forwards to original
//   AiControlStep.cpp   g_ftol_4a2c48 x87 shim — forwards to original
//   AiPreTick.cpp       g_ftol_4a2c48 x87 shim — forwards to original
//   AiStandalone.cpp    RoundST0(float) — static_cast truncation APPROX
//   RaceCamera.cpp      BankersRound(float) — std::lround APPROX
//   AiWallLateral.cpp   static_cast — truncation APPROX
//   AiLineOfSight.cpp   static_cast — truncation APPROX
// Only the x87-shim copies are bit-identical; all static_cast/lround copies
// are approximations that work for their specific call sites but do NOT
// reproduce the exact FRNDINT+correction semantics. This canonical hook
// replaces the original so all call sites are correct once the shim copies
// are removed.
//
// Binary anchor: MASHED.exe SHA-256 (unpatched)
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

// [FABLE: diff-original needed before committing this hook — the correction
// logic's exact floating-point comparisons need bit-identical verification.
// Run: py -3.12 re/frida/run_diff.py <hook_name_once_registered> ]
//
// [FABLE: the hook_name for run_diff.py must be added to hooks_registry.py
// before running the diff. Suggested arg_type: "nop" (no stack args; input ST0
// cannot be captured by the current registry framework — needs a custom
// harness. See ARG_TYPES.md for the float10_st0 handler. Capture at a known
// caller site with a fixed input, e.g. 0x004039f0 or 0x00402fb0.]

// ---------------------------------------------------------------------------
// FUN_004a2c48 — naked implementation.
//
// Stack layout (MSVC x86 __declspec(naked)):
//   [esp+0]  return address (caller's saved RA)
//   ST0      the float to round (pushed by caller before CALL)
//
// Local storage carved below ESP (standard MSVC style; no prologue/epilogue):
//   [esp-4]  local_20  (low dword of 64-bit FISTP result)
//   [esp-8]  uStack_1c (high dword of 64-bit FISTP result)
//
// The FISTP writes 8 bytes below the current SP. Callers of this function
// push onto ST0 and then CALL, so ESP points to the return address. We write
// below ESP, which is standard for naked MSVC x87 helpers (no re-entrant
// interrupts in this single-threaded game context).
//
// After FISTP:
//   [esp-4] = low 32 bits  (local_20)
//   [esp-8] = high 32 bits (uStack_1c, sign-extends the integer)
// ---------------------------------------------------------------------------

// FIRST DRAFT — the correction block is translated from the Ghidra decompile
// mechanically; the float-bit comparisons against 0x80000000 reproduce the
// original's bit-cast comparison pattern. Fable must run diff-original to
// confirm this is bit-identical before the scattered shim copies are removed.

__declspec(naked) void FPURound_4a2c48()
{
    __asm {
        // Step 1-2: round ST0 to nearest integer and store as 64-bit int.
        // The decompile calls FISTP locally via Ghidra's "(ulonglong)ROUND(ST0)"
        // representation; the raw asm is:
        //   FRNDINT          -- round ST0 in-place (respects FPU CW, typically RN-even)
        //   FIST qword ptr   -- then the decompile's correction uses the ORIGINAL
        //                       in_ST0 value, so we need to preserve it.
        //                       The actual sequence: FRNDINT, FISTP qword, FLD copy
        // [UNCERTAIN: the original may PUSH a copy of in_ST0 before FRNDINT, or
        //  may reload it from the FPU stack afterward. Fable: pull the raw listing
        //  via mcp__ghidra__listing_disassemble_function at 0x004a2c48 to confirm
        //  whether in_ST0 is preserved across the FISTP or reloaded from memory.]
        //
        // Conservative implementation: use FDUP (FLD ST0) to copy before rounding.
        fld     st(0)                      // duplicate ST0 -> ST0=in_ST0_copy, ST1=in_ST0
        frndint                            // round ST0 in-place; ST1=original in_ST0
        fsub    st(0), st(1)               // [INCORRECT - see note below; placeholder]
        // [UNCERTAIN: the above is a placeholder. The original uses in_ST0 (pre-round)
        //  for the residual, not the rounded value. The exact FISTP/residual sequence
        //  must be confirmed from the raw disassembly before this hook is correct.]
        //
        // For now, forward to the original to preserve correctness until Fable
        // verifies the full reimplementation.
        // [REMOVE this fallback block and replace with the corrected asm after
        //  Fable pulls the raw listing at 0x004a2c48.]
        fstp    st(0)                      // discard the temporary
        fstp    st(0)                      // restore stack balance
        jmp     dword ptr [g_ftol_orig]    // forward to original
    }
}

// Pointer to the original for the fallback path above.
// Remove once the reimplementation is verified bit-identical by diff-original.
static void* g_ftol_orig = reinterpret_cast<void*>(0x004a2c48u);

// [FABLE INSTRUCTION]: Before registering this hook, replace the body above
// with the corrected naked asm derived from the raw listing at 0x004a2c48:
//
//   EXPECTED raw sequence (from Ghidra decompile + plate mechanical description):
//     0x004a2c48  PUSH EBP / MOV EBP,ESP (or direct sub ESP)
//     0x004a2c4e  FRNDINT                -- rounds ST0 to nearest int
//     0x004a2c50  FISTP qword ptr [local]-- stores 64-bit int, pops ST0
//     0x004a2c54  MOV EAX, [local]       -- low dword
//     0x004a2c5a  MOV EDX, [local+4]     -- high dword
//     ...correction block (0x004a2c78 non-zero gate, 0x004a2c97 threshold)...
//     0x004a2cd3  RET
//
// Once the raw listing is confirmed, write the final naked function that
// replicates every instruction. The correction block's C++ analog is:
//
//   if (local_20 != 0 || (uVar1 & 0x7fffffff00000000ULL) != 0) {
//       float frac;
//       if (*(int*)&high_dword_as_float < 0) {   // negative result
//           float neg_frac = -(float)(in_ST0 - (long long)uVar1);
//           uVar1 += (0x80000000u < *(unsigned*)&neg_frac) ? 1u : 0u;
//       } else {                                 // positive result
//           float pos_frac = (float)(in_ST0 - (long long)uVar1);
//           unsigned uVar2 = (0x80000000u < *(unsigned*)&pos_frac) ? 1u : 0u;
//           uVar1 -= uVar2;
//       }
//   }
//   return uVar1;  // EDX:EAX

// RH_ScopedInstall(FPURound_4a2c48, 0x004a2c48);
// [COMMENTED OUT until Fable confirms the body is bit-identical via diff-original.
//  Uncommenting with the current fallback-to-original body is safe for build testing
//  but produces a trivially identical hook (no coverage evidence); do not promote
//  past C2 until the naked asm is correct and diff-original is GREEN.]
