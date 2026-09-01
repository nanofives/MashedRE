// Mashed RE - RenderWare string character search (strchr / strrchr).
// Originals (both PURE LEAVES, first-party; each returns a pointer INTO `s` or NULL):
//   0x004d8730  RwStrchr   char* __cdecl(char* s, char c)  first occurrence of c
//   0x004d8750  RwStrrchr  char* __cdecl(char* s, char c)  last  occurrence of c
//
// Registered into the RenderWare string vtable by RwEngineRegisterStringFunctions
// (0x004d8570, C3): strchr at slot +0xe0, strrchr at slot +0xdc (see the MOV
// [reg+off],imm32 table there; siblings of RwStricmp/RwStrupr/RwStrlwr).
//
// Disasm ground truth 0x004d8730..0x004d8749 (RwStrchr):
//   DL=[esp+8]=c; EAX=[esp+4]=s; PUSH ESI; ESI=0
//   loop: CL=*EAX; CMP CL,DL; JZ found(return EAX); INC EAX; TEST CL; JNZ loop
//         EAX=ESI(=0)  (not found -> NULL)
//   found: POP ESI; RET
//   -> like C strchr: matches the terminator when c==0 (returns ptr to the NUL); the
//   `s` pointer is dereferenced with NO null-guard (the callee assumes s != NULL).
//
// Disasm ground truth 0x004d8750..0x004d8769 (RwStrrchr):
//   ECX=[esp+4]=s; PUSH EBX; BL=[esp+0xc]=c (orig [esp+8]); EAX=0 (last match)
//   loop: DL=*ECX; CMP DL,BL; JNZ skip; EAX=ECX (record); skip: INC ECX; TEST DL; JNZ loop
//   POP EBX; RET  (EAX = last match or 0)
//   -> like C strrchr: keeps the LAST match, matches the terminator when c==0.
//
// PURE LEAVES: no callees, no globals, no floating point -> plain-C transcriptions
// are bit-identical by construction. Both return a pointer into the caller's buffer.
#include "../Core/HookSystem.h"

// 0x004d8730
extern "C" __declspec(dllexport) char* __cdecl
RwStrchr(char* s, char c)
{
    for (;;) {
        char cur = *s;
        if (cur == c)
            return s;          // first match (incl. the NUL when c == 0)
        ++s;
        if (cur == 0)
            return 0;          // reached end, not found
    }
}

// 0x004d8750
extern "C" __declspec(dllexport) char* __cdecl
RwStrrchr(char* s, char c)
{
    char* last = 0;
    for (;;) {
        char cur = *s;
        if (cur == c)
            last = s;          // record most-recent match (incl. the NUL when c == 0)
        ++s;
        if (cur == 0)
            return last;       // reached end -> last match or NULL
    }
}

RH_ScopedInstall(RwStrchr, 0x004d8730);
RH_ScopedInstall(RwStrrchr, 0x004d8750);
