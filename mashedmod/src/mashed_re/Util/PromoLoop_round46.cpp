// Mashed RE — promote-round round 46 (pool-manager REMOVE; self-contained list op).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Faithful port of FUN_00485b30 (decomp-verified). Node (pool + slot*0x10):
//   +0 key, +4 prev, +8 next, +0xc slot-id(u16). Manager: +0 count(u16),
//   +4 head, +8 tail, +0x10 slots[]. Three removal cases (head/tail/middle) +
//   not-found. Self-contained (no calls). Diffed via early_window_leaf_diff
//   pool_remove_snapshot (build a list via the original insert, then remove,
//   full-state snapshot). Caller FUN_004852e0 (smplfzx) C2.

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x00485b30  undefined4 FUN(ushort *mgr, int key)
extern "C" __declspec(dllexport) std::uint32_t __cdecl PoolRemove485b30(void* mgr, std::int32_t key) {
    std::uint8_t* m = reinterpret_cast<std::uint8_t*>(mgr);
    const std::uint32_t head = *reinterpret_cast<std::uint32_t*>(m + 4);
    if (head == 0) return 0;

    std::uint8_t* node;  // node to unlink+clear (piVar3)
    if (*reinterpret_cast<std::int32_t*>(head + 0) == key) {          // head->key == key
        const std::uint32_t next = *reinterpret_cast<std::uint32_t*>(head + 8);  // head->next
        if (next == 0) {                                             // single node
            *reinterpret_cast<std::uint16_t*>(m + 4) = 0;
            *reinterpret_cast<std::uint16_t*>(m + 6) = 0;
            *reinterpret_cast<std::uint16_t*>(m + 8) = 0;
            *reinterpret_cast<std::uint16_t*>(m + 0xa) = 0;
        } else {
            *reinterpret_cast<std::uint32_t*>(m + 4) = next;         // head = next
            *reinterpret_cast<std::uint32_t*>(next + 4) = 0;         // next->prev = 0
        }
        node = reinterpret_cast<std::uint8_t*>(head);
    } else {
        const std::uint32_t tail = *reinterpret_cast<std::uint32_t*>(m + 8);
        if (*reinterpret_cast<std::int32_t*>(tail + 0) == key) {      // tail->key == key
            const std::uint32_t prev = *reinterpret_cast<std::uint32_t*>(tail + 4);  // tail->prev
            *reinterpret_cast<std::uint32_t*>(m + 8) = prev;         // tail = prev
            *reinterpret_cast<std::uint32_t*>(prev + 8) = 0;         // prev->next = 0
            node = reinterpret_cast<std::uint8_t*>(tail);
        } else {                                                    // middle search
            std::uint32_t cur = *reinterpret_cast<std::uint32_t*>(head + 8);  // head->next
            if (cur == 0) return 0;
            for (;;) {
                if (cur == tail) return 0;                          // reached tail, not found
                if (*reinterpret_cast<std::int32_t*>(cur + 0) == key) break;
                cur = *reinterpret_cast<std::uint32_t*>(cur + 8);    // cur->next
                if (cur == 0) return 0;
            }
            const std::uint32_t prev = *reinterpret_cast<std::uint32_t*>(cur + 4);
            const std::uint32_t next = *reinterpret_cast<std::uint32_t*>(cur + 8);
            *reinterpret_cast<std::uint32_t*>(prev + 8) = next;      // prev->next = next
            *reinterpret_cast<std::uint32_t*>(next + 4) = prev;      // next->prev = prev
            node = reinterpret_cast<std::uint8_t*>(cur);
        }
    }
    *reinterpret_cast<std::uint16_t*>(m + 0) =
        static_cast<std::uint16_t>(*reinterpret_cast<std::uint16_t*>(m + 0) - 1);  // count--
    *reinterpret_cast<std::uint32_t*>(node + 0) = 0;                 // key = 0
    *reinterpret_cast<std::uint32_t*>(node + 8) = 0;                 // next = 0
    *reinterpret_cast<std::uint32_t*>(node + 4) = 0;                 // prev = 0
    std::int32_t* slots = *reinterpret_cast<std::int32_t**>(m + 0x10);
    slots[*reinterpret_cast<std::uint16_t*>(node + 0xc)] = 0;        // slots[slot-id] = 0
    return 1;
}
RH_ScopedInstall(PoolRemove485b30, 0x00485b30);
