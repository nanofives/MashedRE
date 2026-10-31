// Mashed RE — MATCH-S01 (2026-10-31): port bodies compiled from the byte-matched sources in re/match/s01/.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Each re/match/s01/*.c file is byte-identical to the original under re/tools/match/match.py (cl 13.00.9176 /
// c2 13.00.9178, /O2 /GS /Ob1, C, x87). This TU compiles the SAME files as the port body of those RVAs, in both
// targets. The function bodies are untouched; only the declaration environment differs: with MASHED_RE_PORT
// defined, each file skips its own extern / prototype block and gets the one below instead:
//   * defined functions FUN_<rva>  -> the port's exported names (callers and RH_ScopedInstall unchanged);
//   * globals DAT_<addr>           -> lvalues at the original address (the exe maps those pages, exe_main.cpp);
//   * callees                      -> exactly what the replaced bodies called: absolute original addresses
//                                     (the exe reaches them through Compat/StandaloneRvaThunks), or a port
//                                     function where the replaced body called one by name.
// x87 (x87_tus.txt): the matched code is x87 code. build_objs.ps1 takes only .cpp entries, so the .c files are
// compiled here as C++ inside extern "C".
//
// Replaced bodies (removed from their TUs, installs moved here):
//   0x00401340 CupSpinSpeedAndColor     HUD/ScenarioWriters_sa2s2.cpp
//   0x00401570 EntryTableScanByKey      Frontend/SmallLeaves_t1.cpp
//   0x00401ee0 EntityTableSelectUpdate  Frontend/MenuLeaves_af4.cpp
//   0x004026d0 BootQueueFlush           Boot/BootLowRvaCluster.cpp
//   0x00402f50 BootDefaultParamsInit    Boot/BootLowRvaCluster.cpp
//   0x00402f80 CupFloatInit             HUD/Cluster_v2.cpp
//   0x00405890 Pred405890               Race/CollectDone.cpp
//   0x00407a60 UtilTableInit8a9620      Util/UtilLeaves_ac.cpp
//   0x00408a50 PerCarRaceProgressGet    Frontend/Leaves.cpp
//   0x00408ad0 RaceScoreFloatGetBySlot  Frontend/SmallLeaves_t1.cpp
//   0x00409290 SnapNodeToSafeStartLines Race/RoundGridSnap.cpp
//   0x00407600 PlayerPositionPtr        Render/SlotObjectAccessors.cpp (ShadowAB wrapper retired, MATCH-L69-SWAP)
// Evidence: verify/matchs01_20261031/RESULT_MATCH-S01.md.

#include "../Core/HookSystem.h"

#include <cstdint>
#include <string.h>

extern "C" std::uint32_t __cdecl GetDat0066d6e0();                                            // 0x00426bc0
extern "C" void __cdecl GetTableEntry0066d658(int i, std::uint16_t* lo, std::uint16_t* hi);  // 0x00426bd0

#define MASHED_RE_PORT 1

extern "C" {

// ---- defined functions -> port export names ---------------------------------------------------------------
#define FUN_00401340 CupSpinSpeedAndColor
#define FUN_00401570 EntryTableScanByKey
#define FUN_00401ee0 EntityTableSelectUpdate
#define FUN_004026d0 BootQueueFlush
#define FUN_00402f50 BootDefaultParamsInit
#define FUN_00402f80 CupFloatInit
#define FUN_00405890 Pred405890
#define FUN_00407a60 UtilTableInit8a9620
#define FUN_00408a50 PerCarRaceProgressGet
#define FUN_00408ad0 RaceScoreFloatGetBySlot
#define FUN_00409290 SnapNodeToSafeStartLines
#define FUN_00407600 PlayerPositionPtr
__declspec(dllexport) void __cdecl CupSpinSpeedAndColor(void);
__declspec(dllexport) void __cdecl EntryTableScanByKey(int key);
__declspec(dllexport) void __cdecl EntityTableSelectUpdate(int key);
__declspec(dllexport) void __cdecl BootQueueFlush(int n);
__declspec(dllexport) void __cdecl BootDefaultParamsInit(void);
__declspec(dllexport) void __cdecl CupFloatInit(void);
__declspec(dllexport) int __cdecl Pred405890(void);
__declspec(dllexport) void __cdecl UtilTableInit8a9620(void);
__declspec(dllexport) float __cdecl PerCarRaceProgressGet(int i);
__declspec(dllexport) float __cdecl RaceScoreFloatGetBySlot(int i);
__declspec(dllexport) void __cdecl SnapNodeToSafeStartLines(int* v);
__declspec(dllexport) char* __cdecl PlayerPositionPtr(int i);

// ---- globals at their original addresses ----------------------------------------------------------------
#define DAT_007f0ff0 (*(int*)0x007f0ff0u)
#define DAT_00636564 (*(void**)0x00636564u)
#define DAT_005cc328 (*(const float*)0x005cc328u)
#define DAT_00636574 (*(float*)0x00636574u)
#define DAT_00636578 ((Entry636578*)0x00636578u)
#define DAT_00636ac0 (*(Entry636578**)0x00636ac0u)
#define DAT_00636ae8 (*(Block636ae8*)0x00636ae8u)
#define DAT_0063a5d0 (*(int*)0x0063a5d0u)
#define DAT_0063a5d4 (*(int*)0x0063a5d4u)
#define DAT_008a9620 ((CarBlk8a9620*)0x008a9620u)
#define DAT_00639d80 ((Rec639d80*)0x00639d80u)

// ---- callees: what the replaced bodies called ------------------------------------------------------------
#define FUN_0042f6a0 ((int(__cdecl*)(void))0x0042f6a0u)
#define FUN_004b4080 ((Obj4b4080*(__cdecl*)(void*))0x004b4080u)
#define FUN_00401da0 ((void(__cdecl*)(void))0x00401da0u)
#define FUN_004e6680 ((void(__cdecl*)(Entry636578*))0x004e6680u)
#define FUN_004671a0 ((void*(__cdecl*)(int))0x004671a0u)
#define FUN_004c19f0 ((void(__cdecl*)(void*))0x004c19f0u)
#define FUN_004c1a00 ((int(__cdecl*)(void*))0x004c1a00u)
#define FUN_004c1bb0 ((void(__cdecl*)(void*, Rgba4026d0*, int))0x004c1bb0u)
#define FUN_004c1be0 ((void(__cdecl*)(void*, int, int))0x004c1be0u)
#define FUN_0047d150 ((char*(__cdecl*)(int))0x0047d150u)   // 0x00407600's callee, as the replaced body called it
#define FUN_00426bc0() ((int)GetDat0066d6e0())
#define FUN_00426bd0(i, lo, hi) GetTableEntry0066d658((i), (std::uint16_t*)(lo), (std::uint16_t*)(hi))

#include "../../../../re/match/s01/00401340.c"
#include "../../../../re/match/s01/00401570.c"
#include "../../../../re/match/s01/004026d0.c"
#include "../../../../re/match/s01/00402f50.c"
#include "../../../../re/match/s01/00405890.c"
#include "../../../../re/match/s01/00407a60.c"
#include "../../../../re/match/s01/00409290.c"
#include "../../../../re/match/s01/00407600.c"

}  // extern "C"

RH_ScopedInstall(CupSpinSpeedAndColor, 0x00401340);
RH_ScopedInstall(EntryTableScanByKey, 0x00401570);
RH_ScopedInstall(EntityTableSelectUpdate, 0x00401ee0);
RH_ScopedInstall(BootQueueFlush, 0x004026d0);
RH_ScopedInstall(BootDefaultParamsInit, 0x00402f50);
RH_ScopedInstall(CupFloatInit, 0x00402f80);
RH_ScopedInstall(Pred405890, 0x00405890);
RH_ScopedInstall(UtilTableInit8a9620, 0x00407a60);
RH_ScopedInstall(PerCarRaceProgressGet, 0x00408a50);
RH_ScopedInstall(RaceScoreFloatGetBySlot, 0x00408ad0);
RH_ScopedInstall(SnapNodeToSafeStartLines, 0x00409290);
RH_ScopedInstall(PlayerPositionPtr, 0x00407600);
