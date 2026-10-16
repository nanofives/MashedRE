// Mashed RE - FUN_00405890 (0x00405890): the rule-5 "all collectibles done" predicate, ONE body for both targets.
//
// dualcopy4 (2026-10-16, ROADMAP D4). The RVA had three bodies: Util/PromoLoop_round78.cpp:Pred405890 (.asi, installed),
// Race/RuleEngine.cpp:CollectAllDone (exe, over the host-side Persist struct) and Gameplay/PickupCollect_naked.inc:PK_405890
// (exe, a verbatim naked copy). They computed the same thing from three data sources. The computation is this function; each
// source is a thin reader (CollectDone.cpp:Pred405890 reads the two globals, RuleEngine.cpp:CollectAllDone reads Persist).
//
// Original bytes (0x00405890..0x004058a9, 26 bytes, anchored MASHED.exe.unpatched):
//   0x00405890  MOV EAX,[0x0063a5d0]          ; total
//   0x00405895  TEST EAX,EAX / 0x00405897 JNZ -> 0x0040589a ; total == 0 -> RET with EAX = 0 (0x00405899)
//   0x0040589a  MOV EDX,[0x0063a5d4]          ; done
//   0x004058a0  XOR ECX,ECX / CMP EDX,EAX / SETZ CL / MOV EAX,ECX / RET   ; done == total ? 1 : 0
#pragma once

#include <cstdint>

namespace mashed_re {
namespace Race {

inline std::uint32_t CollectDoneBody405890(std::uint32_t total, std::uint32_t done) {
    if (total == 0u) return 0u;              // 0x00405895 TEST / 0x00405897 JNZ not taken -> 0x00405899 RET (EAX == 0)
    return done == total ? 1u : 0u;          // 0x004058a0..0x004058a7 XOR / CMP / SETZ / MOV
}

}  // namespace Race
}  // namespace mashed_re
