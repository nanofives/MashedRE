// MATCH-S06: port body of 0x0042ac00 = the byte-matched source re/match/s06/0042ac00.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuGetters.cpp).
// The matched 0x0042ac00 is `static` (the DDK passes the list in EDX). Under VS2022 it is the plain cdecl
// MenuGroupCountBody (MATCH_STATIC); MenuGroupCount below is the EDX entry (the RH_ScopedInstall target).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042ac00 MenuGroupCountBody
extern "C" {
#include "../../../../re/match/s06/0042ac00.c"
}

// EDX-list entry, __fastcall(ECX unused, EDX list) as the old body, so the export keeps its decorated name
// @MenuGroupCount@8 (the registry's export). Not a body: it only forwards to MenuGroupCountBody.
extern "C" __declspec(dllexport) int __fastcall MenuGroupCount(int /*ecx_unused*/, int* list) {
    return MenuGroupCountBody(list);
}
RH_ScopedInstall(MenuGroupCount, 0x0042ac00);
