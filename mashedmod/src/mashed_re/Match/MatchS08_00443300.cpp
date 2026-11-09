// MATCH-S08: port body of 0x00443300 = the byte-matched source re/match/s08/00443300.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Ai/AiSplineEval.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
//
// PORT-AISPLINE-DDKORDER (2026-11-09, PREREG verify/portstate_20261109/PREREG_AISPLINE_DDKORDER.md): the matched
// source compiles as AiSplineEval_x87_match. VS2022 /fp:precise sums it in source order, while the DDK reassociated
// the outer sum (0x004433b9..0x004433c7), so the port's values differ from the original's (PORT-X87-PC24-AUDIT
// finding (a)). AiSplineEval_x87_ddk evaluates the DDK's order. Default ON since L-104 (a) (2026-11-09);
// MASHED_NO_AISPLINE_DDK=1 opts out and selects the matched body.

#include <cstdlib>
#include <cstring>

#define MATCH_PORT 1
#define FUN_00443300 AiSplineEval_x87_match
#pragma comment(linker, "/EXPORT:AiSplineEval_x87=_AiSplineEval_x87")
extern "C" {
#include "../../../../re/match/s08/00443300.c"
}

namespace {

// One axis of 0x00443300 in the original's order, one float operation per assignment so each intermediate rounds
// to float (equal to the original's PC_24 rounding in ST, and independent of this thread's CW).
// x axis 0x00443383..0x004433cf, z axis 0x004433d2..0x00443427 (same ops, +4).
float DdkAxis(float p0, float p1, float p2, float p3, float t, float t2, float t3)
{
    float a = p0 + p0;          // 0x00443385 fadd st0,st0
    float m = p1 * 5.0f;        // 0x00443389 fmul [0x005cc358]
    a = a - m;                  // 0x0044338f fsubp
    m = p2 * 4.0f;              // 0x00443393 fmul [0x005cc35c]
    a = a + m;                  // 0x00443399 faddp
    a = a - p3;                 // 0x0044339b fsub p3
    a = a * t2;                 // 0x0044339e fmul st2 (t2)
    float b = p1 * 3.0f;        // 0x004433a2 fmul [0x005cc31c]
    b = b - p0;                 // 0x004433a8 fsub p0
    m = p2 * 3.0f;              // 0x004433ac fmul [0x005cc31c]
    b = b - m;                  // 0x004433b2 fsubp
    b = b + p3;                 // 0x004433b4 fadd p3
    b = b * t3;                 // 0x004433b7 fmul st2 (t3)
    float s = a + b;            // 0x004433b9 faddp
    m = p2 - p0;                // 0x004433bd fsub p0
    m = m * t;                  // 0x004433bf fmul st4 (t)
    s = s + m;                  // 0x004433c1 faddp
    m = p1 + p1;                // 0x004433c5 fadd st0,st0
    s = s + m;                  // 0x004433c7 faddp
    return s * 0.5f;            // 0x004433c9 fmul [0x005cc32c]
}

bool DdkOn()
{
    static int on = -1;
    if (on < 0) {
        const char* e = std::getenv("MASHED_NO_AISPLINE_DDK");
        on = (e && std::strcmp(e, "1") == 0) ? 0 : 1;
    }
    return on == 1;
}

}  // namespace

// 0x00443300 in the DDK's evaluation order (default ON, opt-out MASHED_NO_AISPLINE_DDK=1). Index wrap and clamp as the matched source.
extern "C" void AiSplineEval_x87_ddk(Path_00443300* param_1, int param_2, float param_3, float* param_4)
{
    int i = param_2 - 1;
    if (i < 0) {
        i += param_1->count;
    }
    const Pt_00443300* p0 = &param_1->pt[i];
    i++;
    if (i >= param_1->count) {
        i -= param_1->count;
    }
    const Pt_00443300* p1 = &param_1->pt[i];
    i++;
    if (i >= param_1->count) {
        i -= param_1->count;
    }
    const Pt_00443300* p2 = &param_1->pt[i];
    i++;
    if (i >= param_1->count) {
        i -= param_1->count;
    }
    const Pt_00443300* p3 = &param_1->pt[i];
    float t = param_3;
    if (t > 1.0f) {
        t = 1.0f;
    } else if (t < 0.0f) {
        t = 0.0f;
    }
    float t2 = t * t;           // 0x00443379
    float t3 = t2 * t;          // 0x00443381
    param_4[0] = DdkAxis(p0->x, p1->x, p2->x, p3->x, t, t2, t3);
    param_4[1] = DdkAxis(p0->z, p1->z, p2->z, p3->z, t, t2, t3);
}

// 0x00443300: the symbol every caller and the .asi export use. Knob OFF = the matched source.
extern "C" void AiSplineEval_x87(Path_00443300* param_1, int param_2, float param_3, float* param_4)
{
    if (DdkOn()) {
        AiSplineEval_x87_ddk(param_1, param_2, param_3, param_4);
    } else {
        AiSplineEval_x87_match(param_1, param_2, param_3, param_4);
    }
}
