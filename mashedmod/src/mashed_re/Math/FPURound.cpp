// Mashed RE — x87 FPU round-to-integer primitive (the MSVC __ftol helper).
// Original: 0x004a2c48  FUN_004a2c48  (verbatim port; C2 -> C3 via diff-original)
//
// Plate: re/analysis/promote_c1_high_ab3/0x004a2c48.md
//   (raw listing re-pulled 2026-07-03, pool13; body 0x004a2c48..0x004a2cbc, 37 insns)
//
// Signature: ulonglong FUN_004a2c48(void)
//   Input:  implicit ST0 — the caller pushes a float onto the x87 stack before
//           CALL (this is how MSVC emits __ftol). No explicit stack parameters.
//   Output: EDX:EAX — 64-bit rounded integer (callers truncate to byte/int/uint
//           at the call site; high dword is the sign extension).
//
// Algorithm (VERBATIM from the raw listing — note: there is NO FRNDINT; the
// earlier plate description was wrong):
//   1. FLD  ST0                        -- duplicate x
//   2. FST  dword [esp+0x18]           -- keep a single-precision copy of x (sign source)
//   3. FISTP qword [esp+0x10]          -- round(x) per the FPU control word, store i64, pop
//   4. FILD qword [esp+0x10]           -- reload rounded: ST0 = (double)rounded, ST1 = x
//   5. EDX = float-bits of x; EAX = low dword of rounded.
//   6. Non-zero gate: if low dword == 0, jump to the high-dword check; if that is
//      also zero (mod sign bit), drain the x87 stack and return {EAX=0, EDX=high}.
//   7. Correction (round-half-away-from-zero on top of the CW rounding):
//        FSUBP -> residual = x - rounded.
//        residual's single-precision bits go through (ADD 0x7fffffff) to set CF
//        iff residual != 0; the sign of x (EDX) selects +1 (ADC, negative x) or
//        -1 (SBB, positive x) applied to EDX:EAX. For negative x the residual
//        bits are XOR'd with 0x80000000 first (sign flip).
//   8. Return EDX:EAX.
//
// Calling convention: NOT __cdecl — no stack arguments; input is purely ST0.
// The body is __declspec(naked) and reproduces the original's exact frame
// (PUSH EBP / MOV EBP,ESP / SUB ESP,0x20 / AND ESP,-16) and x87-stack discipline
// (the two trailing FSTPs on the zero path drain ST0/ST1). RH_ScopedInstall
// patches an inline JMP at 0x004a2c48 -> this function, so every __ftol call site
// in the game routes here; the whole function is reimplemented (no trampoline).
//
// Relationship to scattered inline copies (remove once this hook is verified):
//   AiLeaderTimer.cpp / AiControlStep.cpp / AiPreTick.cpp
//                       g_ftol_4a2c48 x87 shim — forwards to original; can be
//                       replaced with a direct call to this port.
//   AiStandalone.cpp    RoundST0(float)   — static_cast truncation APPROX
//   RaceCamera.cpp      BankersRound(float) — std::lround APPROX
//   AiWallLateral.cpp / AiLineOfSight.cpp  — static_cast — truncation APPROX
// Only the x87-shim copies are bit-identical; the static_cast/lround copies are
// approximations that work for their specific call sites but do NOT reproduce the
// exact CW-round + residual-correction semantics.
//
// Binary anchor: MASHED.exe SHA-256 (unpatched)
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

// ---------------------------------------------------------------------------
// FUN_004a2c48 — naked implementation, transcribed instruction-for-instruction
// from the raw listing at 0x004a2c48..0x004a2cbc.
//
// Aligned-frame locals (after AND ESP,-16):
//   [esp+0x10] qword rounded int  (low +0x10, high +0x14)
//   [esp+0x18] single copy of x   (sign source)
//   [esp+0x00] single residual scratch
// ---------------------------------------------------------------------------
__declspec(naked) void FPURound_4a2c48()
{
    __asm {
        push    ebp
        mov     ebp, esp
        sub     esp, 20h
        and     esp, 0FFFFFFF0h
        fld     st(0)                     // dup input x: ST0=x, ST1=x
        fst     dword ptr [esp+18h]       // single copy of x (sign source)
        fistp   qword ptr [esp+10h]       // round(x) per FPU CW -> i64, pop
        fild    qword ptr [esp+10h]       // ST0 = (double)rounded, ST1 = x
        mov     edx, dword ptr [esp+18h]  // EDX = float-bits of x (sign)
        mov     eax, dword ptr [esp+10h]  // EAX = low dword of rounded
        test    eax, eax
        jz      zero_path
    do_correction:
        fsubp   st(1), st(0)              // residual = x - rounded, pop -> ST0=residual
        test    edx, edx
        jns     positive_path
        // negative x: flip residual sign bit, then +1 on residual != 0
        fstp    dword ptr [esp]           // store residual (single), pop
        mov     ecx, dword ptr [esp]
        xor     ecx, 80000000h
        add     ecx, 7FFFFFFFh            // CF = (residual != 0)
        adc     eax, 0
        mov     edx, dword ptr [esp+14h]
        adc     edx, 0
        jmp     done
    positive_path:
        fstp    dword ptr [esp]
        mov     ecx, dword ptr [esp]
        add     ecx, 7FFFFFFFh            // CF = (residual != 0)
        sbb     eax, 0                    // -1 on residual != 0
        mov     edx, dword ptr [esp+14h]
        sbb     edx, 0
        jmp     done
    zero_path:
        mov     edx, dword ptr [esp+14h]  // high dword
        test    edx, 7FFFFFFFh
        jnz     do_correction             // high nonzero -> correct via residual
        fstp    dword ptr [esp+18h]       // drain x87 ST0
        fstp    dword ptr [esp+18h]       // drain x87 ST1
    done:
        leave
        ret
    }
}

// Inline-JMP hook at 0x004a2c48 -> FPURound_4a2c48 (registered on DLL load;
// patched by HookSystem::InstallAll; runtime-toggleable for A/B via the harness).
RH_ScopedInstall(FPURound_4a2c48, 0x004a2c48);
