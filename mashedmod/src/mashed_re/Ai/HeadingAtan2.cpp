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
// (read live from MASHED's image):
//   0x005d757c = 0x00000000 (+0.0)   - the zero threshold (FCOMP)
//   0x005ccae0 (double)              - radians->game-units scale (FMUL m64)
//   0x005cd09c                       - offset / half-turn term
//   0x005ccac4                       - full-turn wrap modulus
//   0x005ccad0 / 0x005cd324          - +axis / -axis constants (param_2==0 case)
// __cdecl -> plain RET; the result is left in ST0 (float10; the harness truncates
// to f32 for the fingerprint).
#include "../Core/HookSystem.h"

// 0x004233e0
extern "C" __declspec(dllexport) __declspec(naked) float __cdecl
HeadingAtan2ToGameAngle(float /*p1*/, float /*p2*/, float /*dummy*/)
{
    __asm {
        fld    dword ptr [esp + 0x8]          // param_2
        fcomp  dword ptr ds:[0x005d757c]
        fld    dword ptr [esp + 0x4]          // param_1
        fnstsw ax
        test   ah, 0x44
        jnp    L_special
        fdiv   dword ptr [esp + 0x8]          // param_1 / param_2
        fld1
        fpatan
        fmul   qword ptr ds:[0x005ccae0]
        fchs
        fsubr  dword ptr ds:[0x005cd09c]
        fld    dword ptr [esp + 0x8]          // param_2
        fcomp  dword ptr ds:[0x005d757c]
        fnstsw ax
        test   ah, 0x41
        jnz    L_a
        fadd   dword ptr ds:[0x005cd09c]
    L_a:
        fcom   dword ptr ds:[0x005ccac4]
        fnstsw ax
        test   ah, 0x1
        jnz    L_end
        fsub   dword ptr ds:[0x005ccac4]
    L_end:
        fld    dword ptr [esp + 0x4]          // param_1
        fcomp  dword ptr ds:[0x005d757c]
        fnstsw ax
        test   ah, 0x44
        jp     L_ret
        fld    dword ptr [esp + 0x8]          // param_2
        fcomp  dword ptr ds:[0x005d757c]
        fnstsw ax
        test   ah, 0x44
        jp     L_ret
        fstp   st(0)
        fld    dword ptr ds:[0x005d757c]      // both zero -> 0.0
    L_ret:
        ret
    L_special:
        fcomp  dword ptr ds:[0x005d757c]
        fnstsw ax
        test   ah, 0x5
        jp     L_sp2
        fld    dword ptr ds:[0x005cd324]
        jmp    L_end
    L_sp2:
        fld    dword ptr ds:[0x005ccad0]
        jmp    L_end
    }
}

RH_ScopedInstall(HeadingAtan2ToGameAngle, 0x004233e0);
