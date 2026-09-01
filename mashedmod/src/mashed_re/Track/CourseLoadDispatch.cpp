// Mashed RE — area-track round 1: course-load dispatch cluster (thin verbatim thunks).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Three LOW-RISK thin thunks on/near the Course::Finish (FUN_0040d270) course-load chain.
// Each is a byte-faithful reimplementation that calls the ORIGINAL callees/globals at their
// RVAs (they exist only inside MASHED.exe, so this TU is .asi-only — never the greenfield exe).
//
// Verification: these run at COURSE LOAD, not on demand, so a synthetic path1 re-call is not
// meaningful (the callees open the track .piz / drive load-teardown / render a quad; they
// crash on synthetic menu state). Queued NEEDS-BOOTED-RACE CLUSTER: the PARENT boots one race
// with the .asi live and confirms tracks load identically, gated by the course-load VERIFIER
// (re/frida/scenario_launch.py --assert-course-load).
//
// DEFERRED (heavyweight loaders, do NOT author unverified — parent's acceptance-bar call):
//   FUN_0040cea0 (dirt.piz load + table lookups + apply loop) and FUN_0040d110 (per-player
//   assignment; passes adjacent stack-frame structs to stub callees FUN_0041a8d0 / FUN_004220d0).
//   Author these once the VERIFIER is green so a transcription slip is caught, not shipped.

#include "../Core/HookSystem.h"

#include <cstdint>

namespace {

// --- original callees (RVA-bound; resolved inside MASHED.exe) ---------------
using fn_vu_t   = void (__cdecl*)(std::uint32_t);            // void f(u32)
using fn_vii_t  = void (__cdecl*)(int, int);                // void f(int,int)
using fn_vuu_t  = void (__cdecl*)(std::uint32_t, std::uint32_t); // void f(u32,u32)

// FUN_00426e10 — load-track-by-index consumer (opens track .piz, runs COURSE.LUA).
inline void call_00426e10(std::uint32_t v)      { reinterpret_cast<fn_vu_t>(0x00426e10)(v); }
// FUN_0040d270 — Course::Finish (course load/teardown driver).
inline void call_0040d270(int a, int b)         { reinterpret_cast<fn_vii_t>(0x0040d270)(a, b); }
// FUN_00496c10 — render-quad emitter (void(void); ignores the two forwarded args).
inline void call_00496c10(std::uint32_t a, std::uint32_t b) { reinterpret_cast<fn_vuu_t>(0x00496c10)(a, b); }

// --- original globals -------------------------------------------------------
inline int&          g_005f2770() { return *reinterpret_cast<int*>(0x005f2770u); }          // PTR_PTR_005f2770 (course-handler obj ptr)
inline std::int32_t& g_0063ba7c() { return *reinterpret_cast<std::int32_t*>(0x0063ba7cu); } // DAT_0063ba7c selected course
inline std::int32_t& g_0063ba78() { return *reinterpret_cast<std::int32_t*>(0x0063ba78u); } // DAT_0063ba78 loaded course

} // namespace

// 0x0047b9e0  Thin thunk — forwards (param_1, param_2) to FUN_00496c10 (render-quad
// emitter, void(void) that ignores the args). Byte-faithful forward.
extern "C" __declspec(dllexport) void __cdecl Track_RenderQuadThunk_47b9e0(std::uint32_t param_1, std::uint32_t param_2) {
    call_00496c10(param_1, param_2);  // cited 0x0047b9e0
}
RH_ScopedInstall(Track_RenderQuadThunk_47b9e0, 0x0047b9e0);

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
