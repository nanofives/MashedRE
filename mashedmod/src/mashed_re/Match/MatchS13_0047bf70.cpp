// MATCH-S13: port body of 0x0047bc90, 0x0047bcc0 and 0x0047bf70 = the byte-matched source re/match/s13/0047bf70.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100% / --bytes BYTE-IDENTICAL).
// The same source text is the only body of these three RVAs in the targets that compile this TU
// (0x0047bc90 Match47bc90 + Match47bc90_RegAbi moved out of Util/PromoLoop_sessionB.cpp; 0x0047bcc0
// TriEdgePairsBuildBody + TriEdgePairsBuild moved out of Track/PromoLoop_standins_r2.cpp; 0x0047bf70 had no
// port body and keeps FUN_0047bf70). 0x0047be80 is HELD in the source (86.4%): the port calls the original.
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// The original passes 0x0047bc90's args in ESI/EDX and 0x0047bcc0's triangle count in EAX. VS2022 has no
// such convention, so the matched bodies are cdecl and the installed/exported entries Match47bc90_RegAbi and
// TriEdgePairsBuild are register adapters (not bodies: they only forward).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0047bc90 Match47bc90
#define FUN_0047bcc0 TriEdgePairsBuildBody
#define FUN_0047be80 MatchS13_0047bf70_Call0047be80
#define FUN_004c0ed0 MatchS13_0047bf70_Call004c0ed0
#define FUN_004c3d90 MatchS13_0047bf70_Call004c3d90
#define FUN_004b53b0 MatchS13_0047bf70_Call004b53b0
#pragma comment(linker, "/EXPORT:Match47bc90=_Match47bc90")
#pragma comment(linker, "/EXPORT:Match47bc90_RegAbi=_Match47bc90_RegAbi")
#pragma comment(linker, "/EXPORT:TriEdgePairsBuild=_TriEdgePairsBuild")
#pragma comment(linker, "/EXPORT:FUN_0047bf70=_FUN_0047bf70")

// Raw-address forwarders: the port has no body of these callees, so they call the original image.
// 0x0047be80 takes ECX = normals out, EDX = triangles, EBX = vertices, stack = count (call site 0x0047bfba..0x0047bfc3,
// caller cleans the stack). EBX is saved here.
struct RwV3d;
extern "C" __declspec(naked) void __cdecl MatchS13_0047bf70_Call0047be80(struct RwV3d *out, unsigned short *tris, struct RwV3d *verts, int ntri)
{
    __asm {
        push ebx
        mov  ecx, dword ptr [esp + 8]
        mov  edx, dword ptr [esp + 12]
        mov  ebx, dword ptr [esp + 16]
        push dword ptr [esp + 20]
        mov  eax, 0x0047be80
        call eax
        add  esp, 4
        pop  ebx
        ret
    }
}
extern "C" int __cdecl MatchS13_0047bf70_Call004c0ed0(int p)
{
    return reinterpret_cast<int(__cdecl *)(int)>(0x004c0ed0u)(p);
}
extern "C" int __cdecl MatchS13_0047bf70_Call004c3d90(void *a, void *b, int c, int d)
{
    return reinterpret_cast<int(__cdecl *)(void *, void *, int, int)>(0x004c3d90u)(a, b, c, d);
}
extern "C" void __cdecl MatchS13_0047bf70_Call004b53b0(void *a, void *b, int c)
{
    reinterpret_cast<void(__cdecl *)(void *, void *, int)>(0x004b53b0u)(a, b, c);
}

extern "C" {
#include "../../../../re/match/s13/0047bf70.c"
}

// 0x0047bc90 register entry (ESI = a, EDX = b -> EAX). The original writes only EAX/ECX and preserves EDX;
// its caller 0x0047bcc0 keeps its loop pointer in EDX across the call (0x0047bd9b..0x0047bdc9), so EDX is
// saved here (EDX FIX 2026-07-27, the old ~71 s AV). Not a body: it only forwards to Match47bc90.
extern "C" __declspec(naked) void __cdecl Match47bc90_RegAbi(void)
{
    __asm {
        push edx
        push edx
        push esi
        call Match47bc90
        add  esp, 8
        pop  edx
        ret
    }
}
RH_ScopedInstall(Match47bc90_RegAbi, 0x0047bc90);

// 0x0047bcc0 entry: EAX = n, [esp+4] = out, [esp+8] = tris. Not a body: it only forwards to TriEdgePairsBuildBody.
extern "C" __declspec(naked) void TriEdgePairsBuild()
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        push eax
        call TriEdgePairsBuildBody
        add  esp, 12
        ret
    }
}
RH_ScopedInstall(TriEdgePairsBuild, 0x0047bcc0);
