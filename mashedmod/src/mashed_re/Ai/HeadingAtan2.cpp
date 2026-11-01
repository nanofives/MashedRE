// Mashed RE - Heading angle atan2 -> game-angle units.
// Original: 0x004233e0  FUN_004233e0  ai  C2 -> C3
//
// float FUN_004233e0(float param_1, float param_2): 2-arg x87 atan2-style heading
// converter. When param_2 != 0 it computes FPATAN(param_1/param_2), scales/offsets
// into game-angle units via the .rdata constants below, wraps into a range, and
// handles the param_2<0 half-plane; the param_2==0 case returns one of two axis
// constants by the sign of param_1; param_1==0 && param_2==0 returns 0. Used by the
// AI spline curvature / heading path. Pure x87 leaf (only .rdata constants; no live
// globals) -> the Frida diff runs standalone-force-called against the module.
//
// The registry drives this through the `float3_scalar_ret` handler with a DUMMY
// third float: the target is a 2-arg __cdecl (caller cleans the stack), so the
// extra pushed dword at [esp+0xc] is ignored. Test vector = [param_1, param_2, 0].
//
// BIT-IDENTITY: the original is FPATAN + a chain of fld/fmul/fsub/fcomp keeping the
// result in 80-bit ST0; a portable atan2/atanf reimpl will NOT match. This is a
// __declspec(naked) verbatim transcription of 0x004233e0..0x0042347b. Constants
// (bits copied from MASHED's image into the s_k_* globals, see below):
//   0x005d757c = 0x00000000 (+0.0)   - the zero threshold (FCOMP)
//   0x005ccae0 (double)              - radians->game-units scale (FMUL m64)
//   0x005cd09c                       - offset / half-turn term
//   0x005ccac4                       - full-turn wrap modulus
//   0x005ccad0 / 0x005cd324          - +axis / -axis constants (param_2==0 case)
// __cdecl -> plain RET; the result is left in ST0 (float10; the harness truncates
// to f32 for the fingerprint).
#include "../Core/HookSystem.h"

#include <cstdint>

// D4-DC-INC-ai (2026-11-01): this is now the ONE body for 0x004233e0 in both targets. The exe ran a second,
// private verbatim copy (AL_4233e0 in Ai/AiLeafTargets_naked.inc, called by FUN_00443440's transcription
// at 0x00443775 / 0x004439dd / 0x00443c0c); it was the same 44 instructions with the same branch targets, and
// read the constants from exe-safe copies, because the standalone maps the original's .rdata blank (memory
// naked-asm-rdata-reads-zero-in-exe). This body read them at their original VAs, which is right only inside
// MASHED.exe. It now reads the copies below in both targets. Each holds the bits of the original address,
// re-read from original/MASHED.exe.unpatched 2026-11-01, so the .asi result cannot change:
namespace {
std::uint32_t s_k_5d757c = 0x00000000u;              // 0x005d757c  0.0
std::uint64_t s_k_5ccae0 = 0x404ca5dcc0000000ull;     // 0x005ccae0  57.295799255371094 (double)
std::uint32_t s_k_5cd09c = 0x43340000u;              // 0x005cd09c  180.0
std::uint32_t s_k_5ccac4 = 0x43b40000u;              // 0x005ccac4  360.0
std::uint32_t s_k_5ccad0 = 0x42b40000u;              // 0x005ccad0  90.0
std::uint32_t s_k_5cd324 = 0x43870000u;              // 0x005cd324  270.0
}  // namespace

// 0x004233e0
extern "C" __declspec(dllexport) __declspec(naked) float __cdecl
HeadingAtan2ToGameAngle(float /*p1*/, float /*p2*/, float /*dummy*/)
{
    __asm {
        fld    dword ptr [esp + 0x8]          // param_2
        fcomp  dword ptr [s_k_5d757c]
        fld    dword ptr [esp + 0x4]          // param_1
        fnstsw ax
        test   ah, 0x44
        jnp    L_special
        fdiv   dword ptr [esp + 0x8]          // param_1 / param_2
        fld1
        fpatan
        fmul   qword ptr [s_k_5ccae0]
        fchs
        fsubr  dword ptr [s_k_5cd09c]
        fld    dword ptr [esp + 0x8]          // param_2
        fcomp  dword ptr [s_k_5d757c]
        fnstsw ax
        test   ah, 0x41
        jnz    L_a
        fadd   dword ptr [s_k_5cd09c]
    L_a:
        fcom   dword ptr [s_k_5ccac4]
        fnstsw ax
        test   ah, 0x1
        jnz    L_end
        fsub   dword ptr [s_k_5ccac4]
    L_end:
        fld    dword ptr [esp + 0x4]          // param_1
        fcomp  dword ptr [s_k_5d757c]
        fnstsw ax
        test   ah, 0x44
        jp     L_ret
        fld    dword ptr [esp + 0x8]          // param_2
        fcomp  dword ptr [s_k_5d757c]
        fnstsw ax
        test   ah, 0x44
        jp     L_ret
        fstp   st(0)
        fld    dword ptr [s_k_5d757c]      // both zero -> 0.0
    L_ret:
        ret
    L_special:
        fcomp  dword ptr [s_k_5d757c]
        fnstsw ax
        test   ah, 0x5
        jp     L_sp2
        fld    dword ptr [s_k_5cd324]
        jmp    L_end
    L_sp2:
        fld    dword ptr [s_k_5ccad0]
        jmp    L_end
    }
}

RH_ScopedInstall(HeadingAtan2ToGameAngle, 0x004233e0);
