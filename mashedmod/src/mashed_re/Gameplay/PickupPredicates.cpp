// Mashed RE — two pickup-lane predicates: ONE body each for both build targets (D4-DC-INC-batch3, 2026-11-01).
//
// Before this file each RVA had two bodies, the dual-copy class (ROADMAP D4, re/tools/dual_copy_allowlist.txt):
//   Util/PromoLoop_sessionB.cpp                          .asi-only, installed, the body hooks.csv's evidence covers
//   PK_4576b0 (Gameplay/PickupTick_naked.inc)            exe-only, private verbatim naked copy called by PK_459000
//   PK_45c330 (Gameplay/PickupCollect_naked.inc)         exe-only, private verbatim naked copy called by PK_458920
// This TU is in BOTH exe_sources.rsp and asi_sources.rsp. The bodies moved here. RH_ScopedInstall installs them in
// the .asi and is a no-op in the exe. The exe's naked callers now `call` them (re/tools/naked_specs/pickups_m2a.json,
// pickups_m3.json).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"
#include "PickupPredicates.h"

#include <cstdint>

#ifdef MASHED_STANDALONE
extern "C" int __cdecl PK_SlotEmpty(int v);                      // Gameplay/PickupTick.cpp (exe-only TU)
extern "C" { void* s_fn_45bff0 = reinterpret_cast<void*>(&PK_SlotEmpty); }
#else
extern "C" { void* s_fn_45bff0 = reinterpret_cast<void*>(0x0045bff0u); }   // the original inside MASHED.exe
#endif

// MATCH-S09: the body of 0x004576b0 (AnyActive4576b0) moved to Match/MatchS09_004576b0.cpp,
// which compiles the byte-matched source re/match/s09/004576b0.c.

// 0x0045c330 FUN_0045c330 (gameplay) — PredNot45c330.
//   bool f(int idx): return Pred45bff0(idx)==0;  (NEG/SBB/INC = "x==0 ? 1 : 0")
//   Disasm (0x0045c330..0x0045c342) verbatim. The callee FUN_0045bff0 is taken per target through s_fn_45bff0:
//   the original in the .asi (was `mov eax, 045BFF0h / call eax`), PK_SlotEmpty in the exe (was PK_45c330's
//   `call PK_SlotEmpty`). EAX is the callee's return either way, so the dropped `mov eax, imm` changes no live value.
extern "C" __declspec(dllexport) __declspec(naked) int __cdecl PredNot45c330(int /*idx*/)
{
    __asm {
        mov   eax, dword ptr [esp+4]
        push  eax
        call  dword ptr [s_fn_45bff0]       ; Pred45bff0
        add   esp, 4
        neg   eax
        sbb   eax, eax
        inc   eax
        ret
    }
}
RH_ScopedInstall(PredNot45c330, 0x0045c330);
