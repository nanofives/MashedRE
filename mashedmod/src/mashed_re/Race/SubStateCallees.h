// Mashed RE — D-11073 leg 1a: exe-side substrate for the race sub-state machine's
// pre-race phases (FUN_004111c0 states 4 and 5). See SubStateCallees.cpp.
#pragma once

#include <cstdint>

// MASHED_SUBSTATE (default OFF). Gates the two timer writes below. Read once.
bool SubState_Enabled();

// PARTIAL of FUN_0040fc00: the step store at 0x0040fc57,
// _DAT_007f100c = (float)units * _DAT_005cc948. Writes only when SubState_Enabled().
void SubState_StoreStep(int units);

// PARTIAL of FUN_0041d930: its tail 0x0041da5f..0x0041da76,
// DAT_0063d584 += 1; DAT_0063d588 += _DAT_007f100c. Unconditional; the caller gates it.
// HudSlideBillboardTick (HUD/ScenarioLeaves_sa2.cpp, whole 0x0041d930) calls this as its
// last statement, so the tail exists once.
extern "C" void __cdecl HudSlideBillboardTimerTail(void);

// Moved callee bodies (one body per RVA, both targets).
extern "C" __declspec(dllexport) void          __cdecl    Clear63d584Pair(void);                    // 0x0041d910
extern "C" __declspec(dllexport) void          __cdecl    DeltaTimeOutGet(std::uint32_t* param_1);  // 0x0041da90
extern "C" __declspec(dllexport) void          __fastcall Copy41ae20(char* self);                   // 0x0041ae20
extern "C" __declspec(dllexport) void          __cdecl    Update41b520(void);                       // 0x0041b520
extern "C" __declspec(dllexport) std::uint32_t __cdecl    VehiclePhysicsStateReset(std::uint32_t param_1); // 0x0046baa0
extern "C" __declspec(dllexport) std::uint32_t __cdecl    EntityVelocityCounterGet(int param_1);    // 0x0046c750
extern "C" __declspec(dllexport) std::uint32_t __cdecl    EntityDamageStateGet(int param_1);        // 0x0046c730
