// MATCH-T01G-FU: port bodies of 0x004111c0, 0x0040fc00 and 0x004102f0 = the byte-matched source re/match/t01g/004111c0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = BYTE-IDENTICAL for all three).
// New bodies, callerless, no RH_ScopedInstall (no old install existed). 0x004103a0 is a HELD copy in the source: its
// port body is SubStateHeldCountdown5_C (Race/SubStateMachine.cpp), bound below.
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).

#define MATCH_PORT 1
#define FUN_004111c0 FUN_004111c0
#define FUN_0040fc00 FUN_0040fc00
#define FUN_004102f0 FUN_004102f0
#define FUN_0040dbd0 SubStateEnter4
#define FUN_0040e350 GetRenderSubMode
#define FUN_004103a0 SubStateHeldCountdown5_C
#define FUN_0041cc50 HudSlotUpdateCc50
#define FUN_0041d930 HudSlideBillboardTick
#define FUN_0041da90 DeltaTimeOutGet
#define FUN_00426ba0 HudDrawEnabled
#define FUN_004292c0 UtilWritePtr
#define FUN_0042f500 GetDat0067ea64
#define FUN_0042f6a0 GetRaceSubMode
#define FUN_00448700 VehicleSeedWritePair
#define FUN_00467210 FollowTargetField10
#define FUN_0046baa0 VehiclePhysicsStateReset
#define FUN_0046c730 EntityDamageStateGet
#define FUN_0046c750 EntityVelocityCounterGet
#define FUN_0046c7b0 VehicleSlotGetter
#define FUN_0046d780 LaunchRevRelease46d780
#define FUN_0046d7f0 BoundedTableSignClamp46d7f0
#define FUN_0055dec0 UtilDeref55dec0
#define FUN_00411d60 ReplayCheckTimer

// Callees with no port body in both targets (or an incompatible one): raw-address forwarders, same prototype as the
// matched source, no RVA comment (in the .asi that address carries any installed JMP; the exe body never runs).
extern "C" void MatchT01G_004111c0_Call00403250(int p0) {
    reinterpret_cast<void (__cdecl*)(int)>(0x00403250u)(p0);
}
#define FUN_00403250 MatchT01G_004111c0_Call00403250
extern "C" void MatchT01G_004111c0_Call004039f0(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x004039f0u)();
}
#define FUN_004039f0 MatchT01G_004111c0_Call004039f0
extern "C" int MatchT01G_004111c0_Call00405460(unsigned int p0) {
    return reinterpret_cast<int (__cdecl*)(unsigned int)>(0x00405460u)(p0);
}
#define FUN_00405460 MatchT01G_004111c0_Call00405460
extern "C" void MatchT01G_004111c0_Call00406ce0(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x00406ce0u)();
}
#define FUN_00406ce0 MatchT01G_004111c0_Call00406ce0
extern "C" void MatchT01G_004111c0_Call0040ce00(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x0040ce00u)();
}
#define FUN_0040ce00 MatchT01G_004111c0_Call0040ce00
extern "C" void MatchT01G_004111c0_Call0040d470(int p0) {
    reinterpret_cast<void (__cdecl*)(int)>(0x0040d470u)(p0);
}
#define FUN_0040d470 MatchT01G_004111c0_Call0040d470
extern "C" void MatchT01G_004111c0_Call0040da50(unsigned int p0) {
    reinterpret_cast<void (__cdecl*)(unsigned int)>(0x0040da50u)(p0);
}
#define FUN_0040da50 MatchT01G_004111c0_Call0040da50
extern "C" void MatchT01G_004111c0_Call0040dba0(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x0040dba0u)();
}
#define FUN_0040dba0 MatchT01G_004111c0_Call0040dba0
extern "C" void MatchT01G_004111c0_Call0040dd80(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x0040dd80u)();
}
#define FUN_0040dd80 MatchT01G_004111c0_Call0040dd80
extern "C" void MatchT01G_004111c0_Call0040ddf0(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x0040ddf0u)();
}
#define FUN_0040ddf0 MatchT01G_004111c0_Call0040ddf0
extern "C" void MatchT01G_004111c0_Call0040de00(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x0040de00u)();
}
#define FUN_0040de00 MatchT01G_004111c0_Call0040de00
extern "C" void MatchT01G_004111c0_Call0040e560(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x0040e560u)();
}
#define FUN_0040e560 MatchT01G_004111c0_Call0040e560
extern "C" void MatchT01G_004111c0_Call0040e590(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x0040e590u)();
}
#define FUN_0040e590 MatchT01G_004111c0_Call0040e590
extern "C" void MatchT01G_004111c0_Call0040feb0(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x0040feb0u)();
}
#define FUN_0040feb0 MatchT01G_004111c0_Call0040feb0
extern "C" void MatchT01G_004111c0_Call00410130(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x00410130u)();
}
#define FUN_00410130 MatchT01G_004111c0_Call00410130
extern "C" void MatchT01G_004111c0_Call00410860(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x00410860u)();
}
#define FUN_00410860 MatchT01G_004111c0_Call00410860
extern "C" void MatchT01G_004111c0_Call00410ab0(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x00410ab0u)();
}
#define FUN_00410ab0 MatchT01G_004111c0_Call00410ab0
extern "C" void MatchT01G_004111c0_Call00411170(unsigned int p0) {
    reinterpret_cast<void (__cdecl*)(unsigned int)>(0x00411170u)(p0);
}
#define FUN_00411170 MatchT01G_004111c0_Call00411170
extern "C" void MatchT01G_004111c0_Call00411ae0(int p0, unsigned int p1) {
    reinterpret_cast<void (__cdecl*)(int, unsigned int)>(0x00411ae0u)(p0, p1);
}
#define FUN_00411ae0 MatchT01G_004111c0_Call00411ae0
extern "C" void MatchT01G_004111c0_Call00412f30(int p0) {
    reinterpret_cast<void (__cdecl*)(int)>(0x00412f30u)(p0);
}
#define FUN_00412f30 MatchT01G_004111c0_Call00412f30
extern "C" void MatchT01G_004111c0_Call00413f20(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x00413f20u)();
}
#define FUN_00413f20 MatchT01G_004111c0_Call00413f20
extern "C" void MatchT01G_004111c0_Call00418860(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x00418860u)();
}
#define FUN_00418860 MatchT01G_004111c0_Call00418860
extern "C" void MatchT01G_004111c0_Call004189a0(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x004189a0u)();
}
#define FUN_004189a0 MatchT01G_004111c0_Call004189a0
extern "C" void MatchT01G_004111c0_Call0041a250(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x0041a250u)();
}
#define FUN_0041a250 MatchT01G_004111c0_Call0041a250
extern "C" void MatchT01G_004111c0_Call0041b540(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x0041b540u)();
}
#define FUN_0041b540 MatchT01G_004111c0_Call0041b540
extern "C" void MatchT01G_004111c0_Call0041c090(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x0041c090u)();
}
#define FUN_0041c090 MatchT01G_004111c0_Call0041c090
extern "C" void MatchT01G_004111c0_Call0041c180(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x0041c180u)();
}
#define FUN_0041c180 MatchT01G_004111c0_Call0041c180
extern "C" void MatchT01G_004111c0_Call0041d830(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x0041d830u)();
}
#define FUN_0041d830 MatchT01G_004111c0_Call0041d830
extern "C" void MatchT01G_004111c0_Call0041dca0(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x0041dca0u)();
}
#define FUN_0041dca0 MatchT01G_004111c0_Call0041dca0
extern "C" void MatchT01G_004111c0_Call0041e6c0(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x0041e6c0u)();
}
#define FUN_0041e6c0 MatchT01G_004111c0_Call0041e6c0
extern "C" void MatchT01G_004111c0_Call004215a0(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x004215a0u)();
}
#define FUN_004215a0 MatchT01G_004111c0_Call004215a0
extern "C" void MatchT01G_004111c0_Call004222d0(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x004222d0u)();
}
#define FUN_004222d0 MatchT01G_004111c0_Call004222d0
extern "C" int MatchT01G_004111c0_Call00425a40(unsigned int p0) {
    return reinterpret_cast<int (__cdecl*)(unsigned int)>(0x00425a40u)(p0);
}
#define FUN_00425a40 MatchT01G_004111c0_Call00425a40
extern "C" void MatchT01G_004111c0_Call00425d50(unsigned int p0) {
    reinterpret_cast<void (__cdecl*)(unsigned int)>(0x00425d50u)(p0);
}
#define FUN_00425d50 MatchT01G_004111c0_Call00425d50
extern "C" void MatchT01G_004111c0_Call00426630(int p0) {
    reinterpret_cast<void (__cdecl*)(int)>(0x00426630u)(p0);
}
#define FUN_00426630 MatchT01G_004111c0_Call00426630
extern "C" void MatchT01G_004111c0_Call004266f0(int p0) {
    reinterpret_cast<void (__cdecl*)(int)>(0x004266f0u)(p0);
}
#define FUN_004266f0 MatchT01G_004111c0_Call004266f0
extern "C" void MatchT01G_004111c0_Call00426ab0(int p0, unsigned int p1) {
    reinterpret_cast<void (__cdecl*)(int, unsigned int)>(0x00426ab0u)(p0, p1);
}
#define FUN_00426ab0 MatchT01G_004111c0_Call00426ab0
extern "C" void MatchT01G_004111c0_Call00426c90(int p0) {
    reinterpret_cast<void (__cdecl*)(int)>(0x00426c90u)(p0);
}
#define FUN_00426c90 MatchT01G_004111c0_Call00426c90
extern "C" void MatchT01G_004111c0_Call00429310(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x00429310u)();
}
#define FUN_00429310 MatchT01G_004111c0_Call00429310
extern "C" void MatchT01G_004111c0_Call00429840(int p0) {
    reinterpret_cast<void (__cdecl*)(int)>(0x00429840u)(p0);
}
#define FUN_00429840 MatchT01G_004111c0_Call00429840
extern "C" int MatchT01G_004111c0_Call00429860(void) {
    return reinterpret_cast<int (__cdecl*)(void)>(0x00429860u)();
}
#define FUN_00429860 MatchT01G_004111c0_Call00429860
extern "C" int MatchT01G_004111c0_Call00430820(void) {
    return reinterpret_cast<int (__cdecl*)(void)>(0x00430820u)();
}
#define FUN_00430820 MatchT01G_004111c0_Call00430820
extern "C" void MatchT01G_004111c0_Call004430a0(int p0) {
    reinterpret_cast<void (__cdecl*)(int)>(0x004430a0u)(p0);
}
#define FUN_004430a0 MatchT01G_004111c0_Call004430a0
extern "C" int MatchT01G_004111c0_Call004430b0(void) {
    return reinterpret_cast<int (__cdecl*)(void)>(0x004430b0u)();
}
#define FUN_004430b0 MatchT01G_004111c0_Call004430b0
extern "C" void MatchT01G_004111c0_Call0045b350(int p0) {
    reinterpret_cast<void (__cdecl*)(int)>(0x0045b350u)(p0);
}
#define FUN_0045b350 MatchT01G_004111c0_Call0045b350
extern "C" void MatchT01G_004111c0_Call0045bba0(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x0045bba0u)();
}
#define FUN_0045bba0 MatchT01G_004111c0_Call0045bba0
extern "C" void MatchT01G_004111c0_Call00475c10(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x00475c10u)();
}
#define FUN_00475c10 MatchT01G_004111c0_Call00475c10
extern "C" void MatchT01G_004111c0_Call00476440(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x00476440u)();
}
#define FUN_00476440 MatchT01G_004111c0_Call00476440
extern "C" void MatchT01G_004111c0_Call004777d0(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x004777d0u)();
}
#define FUN_004777d0 MatchT01G_004111c0_Call004777d0
extern "C" void MatchT01G_004111c0_Call00477920(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x00477920u)();
}
#define FUN_00477920 MatchT01G_004111c0_Call00477920
extern "C" void MatchT01G_004111c0_Call00485070(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x00485070u)();
}
#define FUN_00485070 MatchT01G_004111c0_Call00485070
extern "C" void MatchT01G_004111c0_Call00488e70(int p0) {
    reinterpret_cast<void (__cdecl*)(int)>(0x00488e70u)(p0);
}
#define FUN_00488e70 MatchT01G_004111c0_Call00488e70
extern "C" void MatchT01G_004111c0_Call00490380(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x00490380u)();
}
#define FUN_00490380 MatchT01G_004111c0_Call00490380
extern "C" void MatchT01G_004111c0_Call00490500(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x00490500u)();
}
#define FUN_00490500 MatchT01G_004111c0_Call00490500
extern "C" int MatchT01G_004111c0_Call00496930(int p0) {
    return reinterpret_cast<int (__cdecl*)(int)>(0x00496930u)(p0);
}
#define FUN_00496930 MatchT01G_004111c0_Call00496930
#pragma comment(linker, "/EXPORT:FUN_004111c0=_FUN_004111c0")
#pragma comment(linker, "/EXPORT:FUN_0040fc00=_FUN_0040fc00")
#pragma comment(linker, "/EXPORT:FUN_004102f0=_FUN_004102f0")
extern "C" {
#include "../../../../re/match/t01g/004111c0.c"
}
