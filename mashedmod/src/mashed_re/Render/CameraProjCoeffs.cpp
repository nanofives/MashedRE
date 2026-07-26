// Mashed RE - Camera projection Z-coefficient recompute.
// Original: 0x004c1a70  FUN_004c1a70  render  C2 -> C3
//
// void FUN_004c1a70(camera* param_1): recomputes the two view-space depth
// coefficients written to param_1[+0x8c] (slope) and param_1[+0x90] (bias) from
// the projection type at +0x14, the two clip values at +0x80/+0x84, and the live
// view-bounds pair read off the global device struct DAT_007d3ff8 (+0x18/+0x1c).
// Callers: 5 (e.g. RwCameraSetProjection wrapper 0x004c1c10, world-sector setter
// 0x004c1b10). Not a pure leaf: it dereferences the live global DAT_007d3ff8, so
// the Frida diff must run against the live game (menu-state camera is sufficient).
//
// BIT-IDENTITY: every intermediate stays in the 80-bit x87 stack in the original
// (Ghidra renders them as float10); a plain-C /arch:SSE2 reimpl would round each
// intermediate to 24-bit mantissa and diverge by ULPs in the +0x8c/+0x90 stores.
// This is a __declspec(naked) verbatim transcription of the 0x95-byte body
// (0x004c1a70..0x004c1b05). Constants read live from MASHED's image:
//   0x005cc320 = 0x3f800000 (+1.0)   - reciprocal numerator (perspective branch)
//   0x005cd03c                       - view-bounds interpolation factor
//   0x005cc32c                       - bias scale
// Local frame: SUB ESP,8 gives two f32 scratch slots [esp] and [esp+4]; after the
// prologue param_1 sits at [esp+0xc]. __cdecl: caller cleans -> plain RET.
#include "../Core/HookSystem.h"

// 0x004c1a70
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl
CameraRecomputeProjCoeffs(void* /*camera*/)
{
    __asm {
        sub    esp, 8
        mov    eax, dword ptr ds:[0x007d3ff8]
        fld    dword ptr [eax + 0x18]
        fstp   dword ptr [esp + 0x4]
        fld    dword ptr [eax + 0x1c]
        mov    eax, dword ptr [esp + 0xc]     // param_1 (camera)
        mov    ecx, dword ptr [eax + 0x14]    // projection type
        fstp   dword ptr [esp]
        fld    dword ptr [eax + 0x84]
        sub    ecx, 2
        jz     L_proj2
        fdivr  dword ptr ds:[0x005cc320]      // 1.0 / clip@+0x84
        fld    dword ptr [eax + 0x80]
        fdivr  dword ptr ds:[0x005cc320]      // 1.0 / clip@+0x80
        jmp    L_after
    L_proj2:
        fld    dword ptr [eax + 0x80]
    L_after:
        fld    dword ptr [esp]
        fsub   dword ptr [esp + 0x4]
        fmul   dword ptr ds:[0x005cd03c]
        fld    dword ptr [esp]
        fsub   st(0), st(1)
        fstp   dword ptr [esp]
        fadd   dword ptr [esp + 0x4]
        fld    dword ptr [esp]
        fsub   st(0), st(1)
        fld    st(3)
        fsub   st(0), st(3)
        fdivp  st(1), st(0)
        fst    dword ptr [eax + 0x8c]         // slope
        fld    dword ptr [esp]
        fadd   st(0), st(2)
        fxch   st(3)
        fadd   st(0), st(4)
        fmul   st(0), st(1)
        fsubp  st(3), st(0)
        fxch   st(2)
        fmul   dword ptr ds:[0x005cc32c]
        fstp   dword ptr [eax + 0x90]         // bias
        fstp   st(1)
        fstp   st(0)
        fstp   st(0)
        add    esp, 8
        ret
    }
}

RH_ScopedInstall(CameraRecomputeProjCoeffs, 0x004c1a70);
