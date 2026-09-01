// Mashed RE - RenderWare in-place ASCII case conversion (strupr / strlwr).
// Originals (both PURE LEAVES, first-party -- NOT the vendored CRT; each null-guards):
//   0x004d86d0  RwStrupr  void __cdecl(char* s)   fold 'a'..'z' -> upper (SUB 0x20)
//   0x004d8700  RwStrlwr  void __cdecl(char* s)   fold 'A'..'Z' -> lower (ADD 0x20)
//
// Both are registered into the RenderWare string vtable by
// RwEngineRegisterStringFunctions (0x004d8570, C3): strupr at slot +0xf8, strlwr at
// slot +0xfc (see the MOV [reg+off],imm32 table there; sibling of RwStricmp @+0xf0).
//
// Disasm ground truth 0x004d86d0..0x004d86f9 (RwStrupr):
//   EAX=[esp+4]=s; if s==0 RET; CL=*s; if *s==0 RET; EDX=s
//   loop: CL=*EDX; CMP CL,0x61 (JL skip); CMP CL,0x7a (JG skip); SUB CL,0x20; MOV [EDX],CL
//         skip: CL=[EDX+1]; INC EDX; TEST CL; JNZ loop
//   RET (void)
// RwStrlwr (0x004d8700..0x004d8729) is byte-identical with the range 0x41..0x5a and
// ADD 0x20 instead. Range checks are SIGNED byte compares (JL/JG), so bytes >= 0x80
// are never folded -- matched by `signed char` here.
//
// PURE LEAF: no callees, no globals, no floating point -> a plain-C in-place
// transcription is bit-identical by construction. void return; the observable is the
// mutated buffer.
#include "../Core/HookSystem.h"

// 0x004d86d0
extern "C" __declspec(dllexport) void __cdecl
RwStrupr(char* s)
{
    if ((s == 0) || (*s == 0))
        return;
    char* p = s;
    do {
        signed char c = (signed char)*p;
        if ((c >= 'a') && (c <= 'z'))
            *p = (char)(c - 0x20);
        char n = p[1];
        ++p;
        if (n == 0)
            break;
    } while (1);
}

// 0x004d8700
extern "C" __declspec(dllexport) void __cdecl
RwStrlwr(char* s)
{
    if ((s == 0) || (*s == 0))
        return;
    char* p = s;
    do {
        signed char c = (signed char)*p;
        if ((c >= 'A') && (c <= 'Z'))
            *p = (char)(c + 0x20);
        char n = p[1];
        ++p;
        if (n == 0)
            break;
    } while (1);
}

RH_ScopedInstall(RwStrupr, 0x004d86d0);
RH_ScopedInstall(RwStrlwr, 0x004d8700);
