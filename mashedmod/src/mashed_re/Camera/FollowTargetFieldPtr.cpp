// Mashed RE — follow-target field pointers (0x004671d0 / 0x00467210).
//
// The only two of the ten in-race SAFE candidates that are PURE READS. Of the
// rest: 0x004219c0 writes DAT_006403b0 and drives a register-convention callee,
// 0x00442a20 stores into the object AND memcpys into live state, 0x00485070
// clears three globals, 0x0045d3f0 sets DAT_006904ec, 0x0045cc50 is void and
// calls a setter eight times. Screens narrowed the pool; reading decided it.
//
// Every instruction below read from capstone disasm of MASHED.exe.unpatched.
// Binary anchor: MASHED.exe size=2,846,720
//   sha256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
#include "../Core/HookSystem.h"
#include <cstdint>

namespace {

// Both callees are ZERO-ARGUMENT global reads — verified, because the call sites
// push nothing and that is exactly the shape that hides an argument-convention
// bug (memory feedback_ghidra_prebranch_args):
//   0042b930  mov eax,[0x0067ecb0] / ret     MenuAlphaGet      (C4)
//   0042f510  mov eax,[0x0067f190] / ret     Vehicle0HandleGet (C3)
using GlobalGetFn = std::uint32_t(__cdecl*)();
static const GlobalGetFn s_FUN_0042b930 = reinterpret_cast<GlobalGetFn>(0x0042b930);
static const GlobalGetFn s_FUN_0042f510 = reinterpret_cast<GlobalGetFn>(0x0042f510);

constexpr std::uintptr_t kDefaultHolder_6905b0 = 0x006905b0u;
constexpr std::uint32_t  kModeSelector         = 3u;

// Shared body of the twins. Verbatim control flow of 0x004671d0, which differs
// from 0x00467210 only in the final displacement (0x40 vs 0x10):
//   004671d0 call 0x42b930 / cmp eax,3 / jne 0x4671f9
//   004671da cmp [esp+4],-1 / jne 0x4671ed
//   004671e1 mov eax,[0x6905b0] / mov eax,[eax+4] / add eax,0x40 / ret
//   004671ed call 0x42f510   / mov eax,[eax+4] / add eax,0x40 / ret
//   004671f9 mov ecx,[0x6905b0]/ mov eax,[ecx+4] / add eax,0x40 / ret
// Note the two default arms (0x4671e1 and 0x4671f9) are the SAME computation
// reached by different routes — the original does not share them, and neither
// path writes anything.
inline std::uint32_t FollowTargetField(int sel, std::uint32_t fieldOff) {
    const std::uint32_t holder =
        (s_FUN_0042b930() == kModeSelector && sel != -1)
            ? s_FUN_0042f510()
            : *reinterpret_cast<const std::uint32_t*>(kDefaultHolder_6905b0);
    return *reinterpret_cast<const std::uint32_t*>(
               static_cast<std::uintptr_t>(holder) + 4u) + fieldOff;
}

} // namespace

// ─────────────────────────────────────────────────────────────────────────────
// 0x004671d0  FollowTargetField40(int sel) -> *(holder->+4) + 0x40
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl FollowTargetField40(int sel) {
    return FollowTargetField(sel, 0x40u);
}
RH_ScopedInstall(FollowTargetField40, 0x004671d0);

// ─────────────────────────────────────────────────────────────────────────────
// 0x00467210  FollowTargetField10(int sel) -> *(holder->+4) + 0x10
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl FollowTargetField10(int sel) {
    return FollowTargetField(sel, 0x10u);
}
RH_ScopedInstall(FollowTargetField10, 0x00467210);
