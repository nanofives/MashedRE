// Mashed RE — Ghost render-state setup (0x00411ce0).
//
// area-vehicle round 6 (Group-R). Disasm 0x00411ce0 (verbatim;
// log/area-vehicle/disasm_411ce0.txt):
//
//   if (GetRaceSubMode() == 2) {                       // CALL 0x0042f6a0; SUB EAX,2; JNZ
//       dev = *(void**)0x007d3ff8;                     // MOV EAX,[0x7d3ff8]
//       (*(void(__cdecl*)(u32,u32))(dev+0x20))(0xa, 5);   // 5x RwRenderStateSet(state,value)
//       ... (0xb,6) (8,1) (6,1) (0x14,2)                  // pushed value then state -> cdecl(state,value)
//       if (*(u32*)0x0063bb10 != 0 || *(u32*)0x0063bb0c != 0) FUN_0041a960();
//       if (*(u32*)0x0063bb28 != 0 && *(u32*)0x0063bb10 != 0) FUN_00483a70(*(u32*)0x0063bb10);
//   }
//
// The five (state,value) pairs are FIXED CONSTANTS emitted through the RW device
// vtable slot [*0x007d3ff8 + 0x20]; the witness is their exact ordered sequence
// (arg_type render_state_seq_observe records them). The two trailing calls are
// gated by the replay/ghost record pointers 0x0063bb10/0c/28 (0 at menu-attach, so
// the diff keeps them 0 and neither fires). Non-leaf; gate GetRaceSubMode
// (0x0042f6a0) is util C3, so the callee-half is satisfied; the vtable dispatch is
// an indirect call off runtime data (0x007d3ff8) and is exercised by the recorder.
//
// Globals accessed via `*(volatile T*)ADDR` (a real memory load, not the naked
// `mov r,[imm]` immediate trap flagged by area/frontend). Fn-ptr callees declared
// with their real return types (int gate, void dispatch/callees) — no float10, no
// x87-stack leak ([[x87-st0-float10-fnptr-void-leak]]).
// Callers (caller-half): FUN_00410b30 (render C2, UNCONDITIONAL_CALL) via CallersPC.java.
// Binary anchor: MASHED.exe size=2,846,720 sha256=BDCAE093...EFD3C0E
#include "../Core/HookSystem.h"
#include <cstdint>

namespace {
using Fn_getmode_t = int (__cdecl*)();
using Fn_setrs_t   = void (__cdecl*)(std::uint32_t, std::uint32_t);
using Fn_void_t    = void (__cdecl*)();
using Fn_u32_t     = void (__cdecl*)(std::uint32_t);

// 0x0042f6a0 GetRaceSubMode (util C3) -> DAT_0067e9fc.
Fn_getmode_t const s_GetRaceSubMode = reinterpret_cast<Fn_getmode_t>(0x0042f6a0u);
// 0x0041a960 (vehicle C2), 0x00483a70 (render C2) — conditional replay/ghost callees.
Fn_void_t    const s_fn_0041a960    = reinterpret_cast<Fn_void_t>(0x0041a960u);
Fn_u32_t     const s_fn_00483a70    = reinterpret_cast<Fn_u32_t>(0x00483a70u);

constexpr std::uintptr_t kDeviceGlobal = 0x007d3ff8u;   // RW device object pointer
constexpr std::uintptr_t kBB10 = 0x0063bb10u;
constexpr std::uintptr_t kBB0c = 0x0063bb0cu;
constexpr std::uintptr_t kBB28 = 0x0063bb28u;

inline std::uint32_t g32(std::uintptr_t a) {
    return *reinterpret_cast<volatile std::uint32_t*>(a);
}
} // namespace

// ─────────────────────────────────────────────────────────────────────────────
// 0x00411ce0  GhostSetupRender
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl GhostSetupRender() {
    if (s_GetRaceSubMode() != 2) {
        return;
    }
    void* dev = *reinterpret_cast<void**>(kDeviceGlobal);
    Fn_setrs_t set = *reinterpret_cast<Fn_setrs_t*>(
                         reinterpret_cast<char*>(dev) + 0x20);
    set(0xau, 5u);
    set(0xbu, 6u);
    set(0x8u, 1u);
    set(0x6u, 1u);
    set(0x14u, 2u);
    if (g32(kBB10) != 0 || g32(kBB0c) != 0) {
        s_fn_0041a960();
    }
    if (g32(kBB28) != 0 && g32(kBB10) != 0) {
        s_fn_00483a70(g32(kBB10));
    }
}
RH_ScopedInstall(GhostSetupRender, 0x00411ce0);
