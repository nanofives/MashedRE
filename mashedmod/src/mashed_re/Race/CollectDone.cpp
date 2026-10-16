// Mashed RE - FUN_00405890 (0x00405890) global reader. In BOTH rsp lists (dualcopy4, 2026-10-16).
//
// The computation is Race/CollectDone.h:CollectDoneBody405890. This TU is the original-address reader of it:
//   - mashed_re_dev.asi: installed at 0x00405890 (registry key pred_405890, arg_type two_global_predicate);
//   - mashed_re.exe:     the pickup tick's naked PK_458920 calls it (Gameplay/PickupCollect_naked.inc:25 was `call PK_405890`,
//                        a verbatim naked copy, now deleted). The exe maps the original's data addresses itself.
// Anchored to MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E (original\MASHED.exe.unpatched).
#include "../Core/HookSystem.h"
#include "CollectDone.h"

#include <cstdint>

// 0x00405890
extern "C" __declspec(dllexport) std::uint32_t __cdecl Pred405890(void) {
    const std::uint32_t total = *reinterpret_cast<const std::uint32_t*>(0x0063a5d0u);   // 0x00405890 MOV EAX,[0x0063a5d0]
    const std::uint32_t done  = *reinterpret_cast<const std::uint32_t*>(0x0063a5d4u);   // 0x0040589a MOV EDX,[0x0063a5d4]
    return mashed_re::Race::CollectDoneBody405890(total, done);
}
RH_ScopedInstall(Pred405890, 0x00405890);
