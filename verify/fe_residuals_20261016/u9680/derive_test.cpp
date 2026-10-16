
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <float.h>
#include <cstring>
#include <cstdint>
#define MASHED_UNUSED
// 0x004c4eb0
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl
RwMatrixInvert(void* /*out*/, void* /*in*/)
{
    __asm {
        mov    ecx, dword ptr [esp + 8]        // in
        mov    eax, dword ptr [esp + 4]        // out
        fld    dword ptr [ecx + 0x28]
        fmul   dword ptr [ecx + 0x14]
        fld    dword ptr [ecx + 0x24]
        fmul   dword ptr [ecx + 0x18]
        fsubp  st(1), st(0)
        fstp   dword ptr [eax]
        fld    dword ptr [ecx + 0x28]
        fmul   dword ptr [ecx + 0x4]
        fld    dword ptr [ecx + 0x24]
        fmul   dword ptr [ecx + 0x8]
        fsubp  st(1), st(0)
        fchs
        fstp   dword ptr [eax + 0x4]
        fld    dword ptr [ecx + 0x18]
        fmul   dword ptr [ecx + 0x4]
        fld    dword ptr [ecx + 0x14]
        fmul   dword ptr [ecx + 0x8]
        fsubp  st(1), st(0)
        fst    dword ptr [eax + 0x8]
        fld    dword ptr [ecx + 0x10]
        fmul   dword ptr [eax + 0x4]
        fld    dword ptr [ecx + 0x20]
        fmul   st(0), st(2)
        faddp  st(1), st(0)
        fld    dword ptr [eax]
        fmul   dword ptr [ecx]
        faddp  st(1), st(0)
        fstp   dword ptr [esp + 8]
        mov    edx, dword ptr [esp + 8]
        fstp   st(0)
#if 1
        fld1                                    // +1.0 reciprocal numerator. The standalone's .rdata is
                                                // blank-mapped (0x005cc320 reads 0, which zeroed the whole
                                                // inverse, found by D-11072 P1 2026-10-09); fld1 loads the
                                                // identical value 1.0 the float constant 0x3f800000 gives.
#else
        fld    dword ptr ds:[0x005cc320]        // +1.0 reciprocal numerator
#endif
        test   edx, edx
        je     L_skipdiv
        fdiv   dword ptr [esp + 8]              // 1.0 / determinant
    L_skipdiv:
        fld    st(0)
        fmul   dword ptr [eax]
        fst    dword ptr [eax]
        fld    st(1)
        fmul   dword ptr [eax + 0x4]
        fst    dword ptr [esp + 8]
        fstp   dword ptr [eax + 0x4]
        fld    st(1)
        fmul   dword ptr [eax + 0x8]
        fstp   dword ptr [eax + 0x8]
        fld    dword ptr [ecx + 0x28]
        fmul   dword ptr [ecx + 0x10]
        fld    dword ptr [ecx + 0x20]
        fmul   dword ptr [ecx + 0x18]
        fsubp  st(1), st(0)
        fmul   st(0), st(2)
        fchs
        fstp   dword ptr [eax + 0x10]
        fld    dword ptr [ecx + 0x28]
        fmul   dword ptr [ecx]
        fld    dword ptr [ecx + 0x20]
        fmul   dword ptr [ecx + 0x8]
        fsubp  st(1), st(0)
        fmul   st(0), st(2)
        fstp   dword ptr [eax + 0x14]
        fld    dword ptr [ecx + 0x18]
        fmul   dword ptr [ecx]
        fld    dword ptr [ecx + 0x10]
        fmul   dword ptr [ecx + 0x8]
        fsubp  st(1), st(0)
        fmul   st(0), st(2)
        fchs
        fstp   dword ptr [eax + 0x18]
        fld    dword ptr [ecx + 0x24]
        fmul   dword ptr [ecx + 0x10]
        fld    dword ptr [ecx + 0x20]
        fmul   dword ptr [ecx + 0x14]
        fsubp  st(1), st(0)
        fmul   st(0), st(2)
        fstp   dword ptr [eax + 0x20]
        fld    dword ptr [ecx + 0x24]
        fmul   dword ptr [ecx]
        fld    dword ptr [ecx + 0x20]
        fmul   dword ptr [ecx + 0x4]
        fsubp  st(1), st(0)
        fmul   st(0), st(2)
        fchs
        fstp   dword ptr [eax + 0x24]
        fld    dword ptr [ecx + 0x14]
        fmul   dword ptr [ecx]
        fld    dword ptr [ecx + 0x10]
        fmul   dword ptr [ecx + 0x4]
        fsubp  st(1), st(0)
        fmul   st(0), st(2)
        fstp   dword ptr [eax + 0x28]
        fld    dword ptr [eax + 0x20]
        fmul   dword ptr [ecx + 0x38]
        fld    dword ptr [ecx + 0x34]
        fmul   dword ptr [eax + 0x10]
        faddp  st(1), st(0)
        fld    dword ptr [ecx + 0x30]
        fmul   st(0), st(2)
        faddp  st(1), st(0)
        fchs
        fstp   dword ptr [eax + 0x30]
        fstp   st(0)
        fstp   st(0)
        fld    dword ptr [eax + 0x24]
        fmul   dword ptr [ecx + 0x38]
        fld    dword ptr [ecx + 0x34]
        fmul   dword ptr [eax + 0x14]
        faddp  st(1), st(0)
        fld    dword ptr [ecx + 0x30]
        fmul   dword ptr [esp + 8]
        faddp  st(1), st(0)
        fchs
        fstp   dword ptr [eax + 0x34]
        fld    dword ptr [ecx + 0x34]
        fmul   dword ptr [eax + 0x18]
        fld    dword ptr [ecx + 0x30]
        fmul   dword ptr [eax + 0x8]
        faddp  st(1), st(0)
        fld    dword ptr [eax + 0x28]
        fmul   dword ptr [ecx + 0x38]
        mov    dword ptr [eax + 0xc], 0
        faddp  st(1), st(0)
        fchs
        fstp   dword ptr [eax + 0x38]
        ret
    }
}


static float DeriveMenuMeasureDiv() {
    const float vw   = 0.8f;                      // camera +0x68 / +0x6c = 0x3f4ccccd
    const float W    = vw + vw;                   // DAT_00912b08 (0x00552a99)
    const float H    = vw + vw;                   // DAT_00912b0c
    const float s    = 1.0f;                      // DAT_00912be0 (0x00552c48)
    const float half = 0.5f;                      // [0x005cc32c]
    const float m    = (W < H) ? W : H;           // 0x00552ebc..0x00552ece
    const uint32_t kPos0 = 0x00000000u, kNeg0 = 0x80000000u;
    uint32_t b18[16] = { 0, kPos0, kPos0, kPos0, kNeg0, 0, kPos0, kPos0,
                              kNeg0, kPos0, 0x3f800000u, kPos0, 0, 0, kPos0, kPos0 };
    const float c[16] = { -m, 0.f, 0.f, 0.f, 0.f, m, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f,
                          W * s * half, -(H * s * half), 0.f, 0.f };
    float in[16], inv[16] = {};
    std::memcpy(in, b18, sizeof(in));
    in[0] = c[0]; in[5] = c[5]; in[12] = c[12]; in[13] = c[13];
    RwMatrixInvert(inv, in);                      // 0x004c4eb0 (flag word 0 -> cofactor path)
    const float m00 = inv[0], m30 = inv[12];
    const float x0 = (W * s) * (-half), x1 = (W * s) * half;   // 0x00552920 corners
    const float a = x0 * m00 + m30, b = x1 * m00 + m30;        // FUN_004dc370 / 0x004c3d90
    const float lo = (a < b) ? a : b, hi = (a < b) ? b : a;
    return hi - lo;                               // FUN_00553cf0: max - min
}
static uint32_t H2(float f){uint32_t u;memcpy(&u,&f,4);return u;}
int main(){ for(int pc=0;pc<2;pc++){unsigned cw;_controlfp_s(&cw,pc==0?_PC_24:_PC_64,_MCW_PC);
 float d=DeriveMenuMeasureDiv(); printf("%s derived div bits=%08x\n",pc==0?"PC_24":"PC_64",H2(d)); } }
