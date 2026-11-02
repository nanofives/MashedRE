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
#include "../Core/ShadowAB.h"
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

} // namespace

// MATCH-S13: the body of 0x0047d150 (SlotObjectField8) moved to Match/MatchS13_0047d150.cpp,
// which compiles the byte-matched source re/match/s13/0047d150.c.

// ─────────────────────────────────────────────────────────────────────────────
// MATCH-S08: the body of 0x0044b000 (SlotRecordPositionPtr) moved to Match/MatchS08_0044b000.cpp,
// which compiles the byte-matched source re/match/s08/0044b000.c. Its ShadowAB wrapper (TT-11) and the
// static _impl are retired, as for 0x00407600 (MATCH-L69-SWAP): the matched body reads the same record
// (0x0068432c + slot*0x28), calls 0x0047d150 through its original address (as s_FUN_0047d150 did) and adds 0x30.
// ─────────────────────────────────────────────────────────────────────────────

// ─────────────────────────────────────────────────────────────────────────────
// MATCH-L69-SWAP (L-69 a, 2026-10-31): the body of 0x00407600 (PlayerPositionPtr) moved to
// Match/MatchS01.cpp, which compiles the byte-matched source re/match/s01/00407600.c. Its ShadowAB
// wrapper (TT-11, re/tools/shadow_gen.py) is retired: the RVA anchor sat on the wrapper, so the matched
// source could not be the RVA's body with it in place. The matched body reads the same record
// (0x00639d80 + i*0xec + 0x44 = 0x00639dc4 + i*0xec), calls 0x0047d150 and adds 0x30, as the old one.
// Callers in the original: 0x00446520, 0x00464a50 (byte-accurate E8 scan); none in mashed_re.exe.
// ─────────────────────────────────────────────────────────────────────────────

// MATCH-S05: the body of 0x00421930 (BodyGeometryFirstDword) moved to Match/MatchS05_00421930.cpp,
// which compiles the byte-matched source re/match/s05/00421930.c. Its ShadowAB wrapper (TT-11) is retired,
// as for 0x00407600 (MATCH-L69-SWAP): the RVA anchor sat on the wrapper. The matched body calls 0x0057c210
// through its original address, as the replaced body did (s_FUN_0057c210).

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
extern "C" __declspec(dllexport) std::uint32_t __cdecl ClumpQueryField18(int clump);
static std::uint32_t __cdecl ClumpQueryField18_impl(int clump) {
    const int obj = s_FUN_004b3f90(clump);
    if (obj == 0) return 0u;
    return *reinterpret_cast<const std::uint32_t*>(
        static_cast<std::uintptr_t>(static_cast<std::uint32_t>(obj)) + kField18);
}
// --- 0x004b4050 shadow A/B (TT-11) -- GENERATED by re/tools/shadow_gen.py -----------
// In-process original-vs-port comparison at the real call site; arm with
// MASHED_SHADOW_AB=1 (or --hooks in scenario_launch.py). Results: shadow_ab.log.
extern "C" __declspec(dllexport) std::uint32_t __cdecl ClumpQueryField18(int clump) {
    SHADOW_AB_COUNTER(ab, "ClumpQueryField18", 0x004b4050u, ShadowAB::kPhaseRace);
    return ShadowAB::Run(ab, ClumpQueryField18_impl, clump);
}
RH_ScopedInstall(ClumpQueryField18, 0x004b4050);
