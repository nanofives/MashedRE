// Mashed RE — slot/handle -> object-field accessors (STATE-lane batch, 2026-07-28).
//
// Three small live-state getters. Each resolves a slot index or handle through
// an ORIGINAL lookup callee and returns a field of the resolved object, so the
// callees stay original and only the outer resolve+field-read is ported.
//
// EVERY instruction below was read from original\MASHED.exe.unpatched with
// capstone (image base 0x00400000), not from a decompiler — plates for two of
// these three carry "Drift-skip: already plated" bodies with no register
// detail, and memory feedback_wrong_plate_propagates_into_ports is explicit
// that a plate's error becomes a runtime bug.
//
// Binary anchor: MASHED.exe size=2,846,720
//   sha256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
#include "../Core/HookSystem.h"
#include <cstdint>

namespace {

// ---------------------------------------------------------------------------
// Original callees (kept ORIGINAL — we port only the outer accessor).
// ---------------------------------------------------------------------------

// 0x0047d150 — bounds-checked slot->object lookup. Verbatim:
//   0047d150 mov eax,[esp+4] / test eax,eax / jl 0x47d15f
//   0047d158 cmp eax,0xc8    / jl 0x47d162
//   0047d15f xor eax,eax     / ret            (out of range -> 0)
// i.e. any index outside [0, 0xc8) returns 0, which is why 0x0044b000 is safe
// to force-call with arbitrary record values.
using SlotLookupFn = int(__cdecl*)(int);
static const SlotLookupFn s_FUN_0047d150 =
    reinterpret_cast<SlotLookupFn>(0x0047d150);

// 0x0057c210 RwpBodyTableLookup (C4 in hooks.csv). Verbatim:
//   0057c210 mov eax,[esp+4] / mov ecx,[0x007dc8d8] / mov eax,[ecx+eax] / ret
// UNBOUNDED: the argument is a BYTE OFFSET added to the live table base at
// 0x007dc8d8. A wild offset faults — see the registry's state_gate/inputs.
using BodyTableLookupFn = int(__cdecl*)(int);
static const BodyTableLookupFn s_FUN_0057c210 =
    reinterpret_cast<BodyTableLookupFn>(0x0057c210);

// 0x004b3f90 — null-guarded clump query; returns 0 for a NULL argument
//   (004b3f97 test ecx,ecx / 004b3f9d je 0x4b3fb6), otherwise walks the clump
//   via 0x004e66d0 with callback 0x004b4070.
using ClumpQueryFn = int(__cdecl*)(int);
static const ClumpQueryFn s_FUN_004b3f90 =
    reinterpret_cast<ClumpQueryFn>(0x004b3f90);

// 10-dword (0x28-byte) record array indexed by 0x0044b000.
constexpr std::uintptr_t kRecordArr_68432c = 0x0068432cu;
constexpr std::uint32_t  kRecordStride     = 0x28u;   // lea eax,[eax+eax*4] then *8
constexpr std::uint32_t  kPositionOff      = 0x30u;   // add eax,0x30
constexpr std::uint32_t  kField18          = 0x18u;   // mov eax,[eax+0x18]
constexpr std::uint32_t  kGeomPtrOff       = 0x04u;   // mov eax,[eax+4]

} // namespace

// ─────────────────────────────────────────────────────────────────────────────
// 0x0044b000  SlotRecordPositionPtr(int slot) -> resolved object + 0x30
//
// Verbatim 0x0044b000..0x0044b01a:
//   0044b000 mov eax,[esp+4]
//   0044b004 lea eax,[eax+eax*4]           ; slot*5
//   0044b007 mov ecx,[eax*8 + 0x68432c]    ; -> record[0] of slot (stride 0x28)
//   0044b00e push ecx
//   0044b00f call 0x47d150
//   0044b014 add esp,4
//   0044b017 add eax,0x30
//   0044b01a ret
//
// NOTE the +0x30 is applied UNCONDITIONALLY, including to the lookup's
// out-of-range 0 — an invalid slot returns 0x30, not 0. That is original
// behaviour and is reproduced verbatim.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl SlotRecordPositionPtr(int slot) {
    const std::uint32_t rec0 = *reinterpret_cast<const std::uint32_t*>(
        kRecordArr_68432c + static_cast<std::uint32_t>(slot) * kRecordStride);
    return static_cast<std::uint32_t>(s_FUN_0047d150(static_cast<int>(rec0)))
           + kPositionOff;
}
RH_ScopedInstall(SlotRecordPositionPtr, 0x0044b000);

// ─────────────────────────────────────────────────────────────────────────────
// 0x00421930  BodyGeometryFirstDword(int handleOff) -> **(body->+4), else 0
//
// Verbatim 0x00421930..0x00421952:
//   00421930 mov eax,[esp+4]
//   00421934 push esi
//   00421935 push eax
//   00421936 xor esi,esi                   ; esi = the 0 returned on every miss
//   00421938 call 0x57c210
//   0042193d add esp,4
//   00421940 test eax,eax     / je 0x42194f
//   00421944 mov eax,[eax+4]              ; geometry/object pointer slot
//   00421947 test eax,eax     / je 0x42194f
//   0042194b mov eax,[eax]                ; first dword of that object
//   0042194d pop esi / ret
//   0042194f mov eax,esi / pop esi / ret  ; miss -> 0
//
// TWO null checks, and the SECOND deref is unguarded past its own check — the
// plate's "return **((iVar1->+0x4))" collapses both levels into one expression;
// the disasm shows they are separate loads with a test between them.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl BodyGeometryFirstDword(int handleOff) {
    const int body = s_FUN_0057c210(handleOff);
    if (body == 0) return 0u;
    const std::uint32_t geom = *reinterpret_cast<const std::uint32_t*>(
        static_cast<std::uintptr_t>(static_cast<std::uint32_t>(body)) + kGeomPtrOff);
    if (geom == 0u) return 0u;
    return *reinterpret_cast<const std::uint32_t*>(
        static_cast<std::uintptr_t>(geom));
}
RH_ScopedInstall(BodyGeometryFirstDword, 0x00421930);

// ─────────────────────────────────────────────────────────────────────────────
// 0x004b4050  ClumpQueryField18(int clump) -> query(clump)->+0x18, else 0
//
// Verbatim 0x004b4050..0x004b406c:
//   004b4050 mov eax,[esp+4] / push esi / push eax / xor esi,esi
//   004b4058 call 0x4b3f90 / add esp,4
//   004b4060 test eax,eax / je 0x4b4069
//   004b4064 mov eax,[eax+0x18] / pop esi / ret
//   004b4069 mov eax,esi / pop esi / ret     ; miss -> 0
//
// ESI is pushed and popped, so the original leaves it untouched for its caller;
// an MSVC __cdecl function preserves ESI by the same contract, so no naked shim
// is needed here (cf. memory feedback_installed_hook_abi_mismatch, where the
// mismatch was a CLOBBER our port introduced, not a preserve it dropped).
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl ClumpQueryField18(int clump) {
    const int obj = s_FUN_004b3f90(clump);
    if (obj == 0) return 0u;
    return *reinterpret_cast<const std::uint32_t*>(
        static_cast<std::uintptr_t>(static_cast<std::uint32_t>(obj)) + kField18);
}
RH_ScopedInstall(ClumpQueryField18, 0x004b4050);
