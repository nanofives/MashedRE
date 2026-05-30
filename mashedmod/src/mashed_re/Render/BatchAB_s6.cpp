// Mashed RE — Render/BatchAB_s6.cpp
// RenderWare plugin extension-slot lookup.
//
// Covers:
//   0x004d7db0  RwPluginExtensionSlotGet  — walks *(param_1+0x10) linked list;
//               returns node[0] on id match or 0xffffffff on miss.
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// RENDER LIVE-STATE REFUSAL applied:
//   0x004d7430  FUN_004d7430  — REFUSED: writes to dirty-state byte queue
//               DAT_007d5168[DAT_007d6c14] (render pipeline command queue);
//               live-state write during synthetic Frida A/B corrupts queue counter.
//   0x004d7ac0  FUN_004d7ac0  — REFUSED: writes to dirty-state byte queue
//               DAT_007d5168[DAT_007d6c14]; same live-state write hazard.
//   0x004d7bc0  FUN_004d7bc0  — REFUSED: writes to dirty-state byte queue
//               DAT_007d5168[DAT_007d6c14]; same live-state write hazard.
//   0x004d7c10  FUN_004d7c10  — REFUSED: writes to dirty-state byte queue
//               DAT_007d5168[DAT_007d6c14]; same live-state write hazard.
//
// Analysis refs:
//   re/analysis/render_6_c1_to_c2_s3/004d7db0.md
//   re/analysis/bucket_004d7ac0/0x004d7db0.md

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// RwPluginExtensionSlotGet  --  0x004d7db0
//
// Original: FUN_004d7db0 (33 bytes, 0x004d7db0..0x004d7dd1).
// Decompiled (verbatim, Ghidra):
//   undefined4 FUN_004d7db0(int param_1, int param_2) {
//     undefined4 *puVar1;
//     puVar1 = *(undefined4**)(param_1 + 0x10);   // 0x004d7db4: list-head offset +0x10
//     while (puVar1 != (undefined4*)NULL) {
//         if (puVar1[2] == param_2) {              // 0x004d7dbb: key at node+0x08
//             return *puVar1;                      // 0x004d7dc0: return node[0] (base offset)
//         }
//         puVar1 = (undefined4*)puVar1[0xc];       // 0x004d7dc6: next at node+0x30
//     }
//     return 0xffffffff;                           // 0x004d7dcb: not-found sentinel
//   }
//
// RW plugin extension-slot lookup by id.
// param_1 — pointer to a type-descriptor object (e.g. &DAT_00618664 = RpAtomic anchor).
// param_2 — plugin id to search for.
//   *(param_1 + 0x10): head of intrusive singly-linked extension list.
//   node[0] (offset +0x00): base offset of extension data for this plugin.
//   node[2] (offset +0x08): plugin id.
//   node[0xc] (offset +0x30): next node pointer.
//   Returns node[0] on match, 0xffffffff (raw: 0xffffffff = -1 signed) on miss.
//
// Leaf function — no callees. Anti-island gate: pure reader of passed-in pointer.
// Callers: FUN_004c2dc0 (0x004c2dc0, Lua, C2), FUN_004e7e10 (0x004e7e10, C1).
// RW analog: RwPluginRegistryGetPluginOffset.
// ---------------------------------------------------------------------------

// 0x004d7db0
extern "C" __declspec(dllexport) std::uint32_t __cdecl RwPluginExtensionSlotGet(
    std::int32_t param_1, std::int32_t param_2)
{
    // 0x004d7db4: read list head at *(param_1 + 0x10)
    const std::uint32_t* puVar1 =
        *reinterpret_cast<const std::uint32_t* const*>(
            reinterpret_cast<const std::uint8_t*>(
                static_cast<std::uintptr_t>(param_1)) + 0x10u);

    while (puVar1 != nullptr) {
        // 0x004d7dbb: compare node[2] (offset +0x08) against param_2
        if (static_cast<std::int32_t>(puVar1[2]) == param_2) {
            // 0x004d7dc0: return node[0] — extension base offset
            return puVar1[0];
        }
        // 0x004d7dc6: advance via node[0xc] (offset +0x30)
        puVar1 = reinterpret_cast<const std::uint32_t*>(
            static_cast<std::uintptr_t>(puVar1[0xc]));
    }

    // 0x004d7dcb: not-found sentinel — raw 0xffffffff
    return 0xffffffffu;
}

RH_ScopedInstall(RwPluginExtensionSlotGet, 0x004d7db0);  // c3-batch-ab-s6
