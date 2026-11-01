// MATCH-S05: port body of 0x0041f710 = the byte-matched source re/match/s05/0041f710.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of
// Race/CarRegistrant.cpp, whose verbatim naked copy came from Race/CarRegistrant_naked.inc). exe only, like the old body.
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// The original passes the record in EAX (caller 0x00420870: mov eax,esi at 0x004209ed). VS2022 has no EAX
// convention, so the matched body is the cdecl CR_41f710Body and CR_41f710 (the symbol Race/CarRegistrant.cpp
// calls with the record in EAX) is a register adapter: push the EAX record, call the body, drop the argument.
// EAX/ECX/EDX are clobbered by both, as in the original.
#define MATCH_PORT 1
#define FUN_0041f710 CR_41f710Body
#define FUN_00484cf0 RG_484cf0   // Race/WorldRegistry_naked.inc, the callee the old body called
extern "C" {
#include "../../../../re/match/s05/0041f710.c"
}

// EAX-record entry. Not a body: it only forwards to CR_41f710Body.
extern "C" __declspec(naked) void CR_41f710(void) {
    __asm {
        push eax
        call CR_41f710Body
        add esp, 4
        ret
    }
}
