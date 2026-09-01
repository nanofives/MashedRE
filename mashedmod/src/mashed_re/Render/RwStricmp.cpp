// Mashed RE - RenderWare case-insensitive string compare (stricmp).
// Original: 0x004d8680  FUN_004d8680  render  C2 -> C3
//
//   int __cdecl RwStricmp(char* s1 /*[esp+4]*/, char* s2 /*[esp+8]*/)
//
// Disasm ground truth (0x004d8680..0x004d86c7, 72 bytes):
//   PUSH ESI; ESI=[esp+8]=s1; if ESI==0 -> XOR EAX,EAX; POP; RET (return 0)
//   EDX=[esp+0xc]=s2;         if EDX==0 -> XOR EAX,EAX; POP; RET (return 0)
//   loop: AL=*s1; CL=*s2
//     CMP AL,0x41 (JL skip signed); CMP AL,0x5A (JG skip signed); ADD AL,0x20
//     CMP CL,0x41 (JL skip signed); CMP CL,0x5A (JG skip signed); ADD CL,0x20
//     CMP AL,CL; JNZ diff
//     INC ESI; INC EDX; TEST AL,AL; JZ ret0; TEST CL,CL; JNZ loop; fall -> ret0
//   ret0:  XOR EAX,EAX; POP ESI; RET
//   diff:  MOVSX ECX,CL; MOVSX EAX,AL; SUB EAX,ECX; POP ESI; RET
//
// The range test uses SIGNED byte compares (JL/JG), so bytes >= 0x80 (negative as
// signed char) are NOT folded -- matched exactly by `signed char` in [ 'A' , 'Z' ].
// The result is MOVSX-extended: (int)(signed char)a_folded - (int)(signed char)c_folded.
//
// PURE LEAF: no callees, no globals, no live state, no floating point. A plain-C
// integer transcription is bit-identical by construction (no x87/SSE rounding to
// diverge). Stored at the RenderWare string vtable slot +0xf0 (see the
// PromoLoop_sessionB dispatch table entry {0xf0, 0x4d8680}).
#include "../Core/HookSystem.h"

// 0x004d8680
extern "C" __declspec(dllexport) int __cdecl
RwStricmp(const char* s1, const char* s2)
{
    if ((s1 != 0) && (s2 != 0)) {
        for (;;) {
            signed char a = (signed char)*s1;
            signed char c = (signed char)*s2;
            if ((a > '@') && (a < '[')) a = (signed char)(a + ' ');   // 'A'..'Z' -> +0x20
            if ((c > '@') && (c < '[')) c = (signed char)(c + ' ');
            if (a != c)
                return (int)a - (int)c;
            ++s1;
            ++s2;
            if (a == 0) break;   // equal chars and end-of-string reached -> 0
            if (c == 0) break;
        }
    }
    return 0;
}

RH_ScopedInstall(RwStricmp, 0x004d8680);
