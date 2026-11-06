// MATCH-S09-FU: port body of 0x00458920 = the byte-matched source re/match/s09/00458920.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the exe (it replaces the verbatim naked PK_458920 that
// re/tools/naked_specs/pickups_m3.json generated into Gameplay/PickupCollect_naked.inc). Exe only, as that copy was.
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
//
// Callers: Gameplay/PickupTick.cpp Pickups_Callback (the world registry's callback, Race/WorldRegistry.cpp
// RG_CbDispatch for target 0x00458920) and the env-gated replay PickupCb_MaybeReplay (MASHED_PKCB_REPLAY).
// Callees bound to the symbols the naked copy called (pickups_m3.json "calls"):
//   0x00405890 Pred405890 (Match/MatchS01.cpp)        0x00407580 SubsystemARecordKey (Gameplay/SubsystemARecordKey.cpp)
//   0x0040e350 PK_SubMode (Gameplay/PickupTick.cpp)    0x0045c330 PredNot45c330 (Gameplay/PickupPredicates.cpp)
//   0x004588c0 PK_Burst, 0x0045c010 PK_Grant, 0x00465ca0 PK_Sfx (Gameplay/PickupTick.cpp)
//   0x00458570 (HELD in the source): its port body is the naked PK_458570 (register arguments: EAX = index,
//     ESI = out, record on the stack, 0x004589bc..0x004589dd). MatchS09_00458920_Call458570 below loads those
//     registers and calls it through PK_458570_w, as the naked caller did.
// .rdata: the source reads 1.0f at 0x005ce548 (`fsub [0x005ce548]` at 0x00458979, the arm entered from 0x0045896d / 0x004589af).
// The exe's arena holds zero there (exe_main.cpp g_b17_low_arena_pad), so this TU seeds the original's value
// 0x3f800000 at static init. No other exe code reads 0x005ce548 (the naked tick reads Gameplay/PickupTick.cpp
// g_pk_c_5ce548, its own copy).
#include <cstdint>

#define MATCH_PORT 1
#define FUN_00458920 PK_458920
#define FUN_00405890 Pred405890
#define FUN_00407580 SubsystemARecordKey
#define FUN_0040e350 PK_SubMode
#define FUN_0045c330 PredNot45c330
#define FUN_004588c0 PK_Burst
#define FUN_0045c010 PK_Grant
#define FUN_00465ca0 PK_Sfx
#define FUN_00458570 MatchS09_00458920_Call458570
extern "C" {
#include "../../../../re/match/s09/00458920.c"
}

extern "C" void PK_458570_w();   // Gameplay/PickupTick.cpp: witness, then jmp PK_458570 (Gameplay/PickupCollect_naked.inc)

// The held callee's forwarder (not a body of 0x00458570). cdecl (out, idx, rec) -> EAX = idx, ESI = out, rec pushed;
// the callee's ST0 result is returned unchanged. EBX/EBP/ESI/EDI are saved here.
extern "C" __declspec(naked) float MatchS09_00458920_Call458570(V3_00458920* /*out*/, int /*idx*/, int* /*rec*/) {
    __asm {
        push ebp
        push ebx
        push edi
        push esi
        mov  esi, dword ptr [esp + 20]
        mov  eax, dword ptr [esp + 24]
        push dword ptr [esp + 28]
        call PK_458570_w
        add  esp, 4
        pop  esi
        pop  edi
        pop  ebx
        pop  ebp
        ret
    }
}

namespace {
struct SeedRdata005ce548 {
    SeedRdata005ce548() {
        __try {
            *reinterpret_cast<volatile std::uint32_t*>(0x005ce548u) = 0x3f800000u;   // 1.0f, the original's .rdata value
        } __except (1 /* EXCEPTION_EXECUTE_HANDLER */) {
        }
    }
} s_seedRdata005ce548;
}  // namespace
