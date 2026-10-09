// Mashed RE — D-11073 leg 1b: FUN_004111c0 phases 4 and 5. See SubStateMachine.cpp.
#pragma once

extern "C" __declspec(dllexport) void __cdecl SubStateEnter4(void);          // 0x0040dbd0
extern "C" void __cdecl SubStateHeldCountdown5_C(int units);                 // 0x004103a0 body
extern "C" __declspec(dllexport) void __cdecl SubStateHeldCountdown5(void);  // 0x004103a0 (EAX)
extern "C" __declspec(dllexport) void __cdecl SubStateRollCountdown414180(void); // 0x00414180

#ifdef MASHED_STANDALONE
// Exe-side bindings for the callees the exe cannot reach at their RVA. Set by TrackRenderer.
struct SubStateHost {
    int           (*car_alive)(int v);       // FUN_0046c7b0 -> s_host.car_alive (NAMED DEVIATION)
    void          (*ai_tick)();              // FUN_00418860 -> AiBridgeSnapshot + Ai_Standalone_Tick
    unsigned char (*player_accel_byte)();    // v == 0 accel byte for FUN_0046d7f0 (stand-in binding)
};
void SubState_SetHost(const SubStateHost& h);
#endif
