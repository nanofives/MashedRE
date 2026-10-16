
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <float.h>
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


static uint32_t H(float f){uint32_t u;memcpy(&u,&f,4);return u;}
static float F(uint32_t u){float f;memcpy(&f,&u,4);return f;}
int main(){
  for(int pc=0;pc<2;pc++){
    unsigned cw; _controlfp_s(&cw, pc==0?_PC_24:_PC_64, _MCW_PC);
    float in[16]={0},out[16]={0};
    in[0]=F(0xbfcccccd); in[5]=F(0x3fcccccd); in[10]=1.0f; in[12]=F(0x3f4ccccd); in[13]=F(0xbf4ccccd); in[15]=0;
    RwMatrixInvert(out,in);
    printf("%s:", pc==0?"PC_24":"PC_64"); for(int i=0;i<16;i++) printf(" %x",H(out[i])); printf("\n");
  }
}