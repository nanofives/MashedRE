// Mashed RE: stand-in promotion round 4 (plan item #3 step 2, 2026-10-11).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Two RenderWare pipeline builders (caller 0x00496e40 PCVS_Init) that differ only in the fragment-node getter
// (0x004eb3c0 / 0x004eb9d0; the other three differing instructions are rel8 targets of the same branches). Every callee
// is called at its original address; the pipeline allocations go through the RwGlobals slots (heap stand-in).
//
//   +0x02  call 0x004d4170            edi = pipe            test / je -> return 0
//   +0x0d  call 0x004d4dd0 (edi)      esi = locked          test / je -> destroy
//   +0x1c  call <node getter>         eax = node
//   +0x27  call 0x004d4f90 (esi, 0, eax, 0)
//   +0x2d  call 0x004d4380 (eax)      test / je -> destroy
//   +0x39  return edi
//   destroy: call 0x004d41e0 (edi), return 0
//   All calls cdecl, caller-cleaned (add esp, 4 after 0x004d4dd0; add esp, 0x14 after 0x004d4380 for both calls).

#include "../Core/HookSystem.h"

#include <cstdint>

// MATCH-S16: the body of 0x00496d60 (PipelineFragment4eb3c0Create) moved to Match/MatchS16_00496d60.cpp,
// which compiles the byte-matched source re/match/s16/00496d60.c.

// MATCH-S16: the body of 0x00496db0 (PipelineFragment4eb9d0Create) moved to Match/MatchS16_00496db0.cpp,
// which compiles the byte-matched source re/match/s16/00496db0.c.
