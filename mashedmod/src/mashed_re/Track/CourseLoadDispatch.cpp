// Mashed RE — area-track round 1: course-load dispatch cluster (verbatim ports).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Three functions on the Course::Finish (FUN_0040d270) course-load chain. Each is a
// byte-faithful reimplementation that calls the ORIGINAL callees/globals at their RVAs
// (they exist only inside MASHED.exe, so this TU is .asi-only — never the greenfield exe).
//
// Verification model: these run at COURSE LOAD, not on demand, so a synthetic path1
// re-call is not meaningful (FUN_00426e10 opens the track .piz + runs COURSE.LUA; the
// render/piz callees crash on synthetic menu state). They are queued NEEDS-BOOTED-RACE:
// the parent boots one race with the .asi live and confirms tracks load identically.
//
// DEFERRED this round: FUN_0040d110 (0x0040d110) — passes adjacent stack-frame structs
// (&local_18 / &local_20) into stub callees FUN_0041a8d0 / FUN_004220d0; a faithful port
// needs those callees' struct sizes decoded first (over-read risk). Left C2.

#include "../Core/HookSystem.h"

#include <cstdint>

namespace {

// --- original callees (RVA-bound; resolved inside MASHED.exe) ---------------
using fn_vu_t   = void  (__cdecl*)(std::uint32_t);              // void f(u32)
using fn_vi_t   = void  (__cdecl*)(int);                       // void f(int)
using fn_vv_t   = void  (__cdecl*)(void);                      // void f(void)
using fn_vii_t  = void  (__cdecl*)(int, int);                  // void f(int,int)
using fn_viii_t = void  (__cdecl*)(int, int, int);             // void f(int,int,int)
using fn_u_pc_t = std::uint32_t (__cdecl*)(const char*);        // u32 f(const char*)
using fn_i_u_t  = int   (__cdecl*)(std::uint32_t);             // int  f(u32)
using fn_i_uu_t = int   (__cdecl*)(std::uint32_t, std::uint32_t); // int f(u32,u32)
using fn_i_ii_t = int   (__cdecl*)(int, int);                  // int  f(int,int)

// FUN_00426e10 — load-track-by-index consumer (opens track .piz, runs COURSE.LUA).
inline void     call_00426e10(std::uint32_t v) { reinterpret_cast<fn_vu_t>(0x00426e10)(v); }
// FUN_0040d270 — Course::Finish (course load/teardown driver).
inline void     call_0040d270(int a, int b)    { reinterpret_cast<fn_vii_t>(0x0040d270)(a, b); }
// callees of FUN_0040cea0:
inline void     call_004c5c80(int v)           { reinterpret_cast<fn_vi_t>(0x004c5c80)(v); }
inline std::uint32_t call_0042a8d0(const char* s) { return reinterpret_cast<fn_u_pc_t>(0x0042a8d0)(s); }
inline void     call_00495280(std::uint32_t v) { reinterpret_cast<fn_vu_t>(0x00495280)(v); }
inline int      call_00404e00(std::uint32_t v) { return reinterpret_cast<fn_i_u_t>(0x00404e00)(v); }
inline int      call_00404e20(std::uint32_t a, std::uint32_t b) { return reinterpret_cast<fn_i_uu_t>(0x00404e20)(a, b); }
inline int      call_004c5cb0(int a, int b)    { return reinterpret_cast<fn_i_ii_t>(0x004c5cb0)(a, b); }
inline void     call_004952f0()                { reinterpret_cast<fn_vv_t>(0x004952f0)(); }
inline void     call_0041ecc0(int a, int b, int c) { reinterpret_cast<fn_viii_t>(0x0041ecc0)(a, b, c); }

// --- original globals -------------------------------------------------------
inline int&           g_005f2770() { return *reinterpret_cast<int*>(0x005f2770u); }  // PTR_PTR_005f2770 (course-handler obj ptr)
inline std::int32_t&  g_0063ba7c() { return *reinterpret_cast<std::int32_t*>(0x0063ba7cu); }  // DAT_0063ba7c selected course
inline std::int32_t&  g_0063ba78() { return *reinterpret_cast<std::int32_t*>(0x0063ba78u); }  // DAT_0063ba78 loaded course
inline std::int32_t&  g_0063ba90() { return *reinterpret_cast<std::int32_t*>(0x0063ba90u); }  // DAT_0063ba90
inline std::int32_t&  g_0063ba94() { return *reinterpret_cast<std::int32_t*>(0x0063ba94u); }  // DAT_0063ba94

} // namespace

// 0x0040d020  LoadTrackByIndex — double-deref the indexed track-table slot and
// forward to FUN_00426e10. `**(param_1 + param_2*4)`: table holds pointers to
// pointers (see 0x0040d020 plate; identical access shape in 0040cea0/0040d110).
extern "C" __declspec(dllexport) void __cdecl Track_LoadByIndex_40d020(int param_1, int param_2) {
    call_00426e10(**reinterpret_cast<std::uint32_t**>(param_1 + param_2 * 4));  // cited 0x0040d020
}
RH_ScopedInstall(Track_LoadByIndex_40d020, 0x0040d020);

// 0x0040d440  Course::LoadCurrent — apply the selected course value via
// FUN_0040d270(PTR_PTR_005f2770, DAT_0063ba7c), then commit selected->loaded.
extern "C" __declspec(dllexport) void __cdecl Course_LoadCurrent_40d440(void) {
    call_0040d270(g_005f2770(), g_0063ba7c());  // cited 0x0040d440
    g_0063ba78() = g_0063ba7c();                // DAT_0063ba78 = DAT_0063ba7c
}
RH_ScopedInstall(Course_LoadCurrent_40d440, 0x0040d440);

// 0x0040cea0  Vehicle surface setup during track load — opens
// toastart/vehicles/dirt.piz, two table lookups against the indexed track slot,
// stores the two resolved handles at DAT_0063ba90/94, then a 4-iteration apply loop.
extern "C" __declspec(dllexport) void __cdecl Track_VehicleSurfaceSetup_40cea0(int param_1, int param_2, std::uint32_t param_3) {
    int iVar5 = 0;
    int iVar4 = 0;
    call_004c5c80(0);
    std::uint32_t uVar1 = call_0042a8d0("toastart/vehicles/dirt.piz");  // cited 0x0040cea0
    call_00495280(uVar1);
    int iVar2 = call_00404e00(**reinterpret_cast<std::uint32_t**>(param_1 + param_2 * 4));
    int iVar3 = call_00404e20(**reinterpret_cast<std::uint32_t**>(param_1 + param_2 * 4), param_3);
    if (iVar2 != 0) iVar4 = call_004c5cb0(iVar2, 0);
    if (iVar3 != 0) iVar5 = call_004c5cb0(iVar3, 0);
    call_004952f0();
    g_0063ba90() = iVar4;
    g_0063ba94() = iVar5;
    if ((iVar4 != 0) && (iVar5 != 0)) {
        for (int i = 0; i < 4; ++i) call_0041ecc0(i, iVar4, iVar5);
    }
}
RH_ScopedInstall(Track_VehicleSurfaceSetup_40cea0, 0x0040cea0);
