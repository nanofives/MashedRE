// Mashed RE — promote-round round 45 (pool-manager INSERT; self-contained list op).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Faithful port of FUN_00485a70 (decomp-verified). Manager (byte offsets):
//   +0 count(u16), +2 cap(u16), +4 head(ptr), +8 tail(ptr), +0xc pool(ptr),
//   +0x10 slots(int[] ptr). Node (pool + slot*0x10): +0 key, +4 link-a, +8 link-b,
//   +0xc slot-id(u16). No function calls (self-contained). Diffed via
//   early_window_leaf_diff pool_insert_snapshot (reset+call+full-state snapshot).
//   Caller FUN_004850e0 (smplfzx) C2.

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x00485a70  undefined2 FUN(ushort *mgr, undefined4 key)
extern "C" __declspec(dllexport) std::uint16_t __cdecl PoolInsert485a70(void* mgr, std::uint32_t key) {
    std::uint8_t* m = reinterpret_cast<std::uint8_t*>(mgr);
    const std::uint16_t cap   = *reinterpret_cast<std::uint16_t*>(m + 2);
    const std::uint16_t count = *reinterpret_cast<std::uint16_t*>(m + 0);
    if (cap <= count) return 0xffffu;                                  // full

    std::int32_t* slots = *reinterpret_cast<std::int32_t**>(m + 0x10);
    std::uint16_t slot = cap;
    bool found = false;
    if (cap != 0) {
        do {
            slot = static_cast<std::uint16_t>(slot - 1);
            if (slots[slot] == 0) { found = true; break; }
        } while (slot != 0);
    }
    if (!found) slot = 0xffffu;

    std::uint8_t* pool = *reinterpret_cast<std::uint8_t**>(m + 0xc);
    std::uint8_t* node = pool + static_cast<std::uint32_t>(slot) * 0x10u;
    slots[slot] = 1;
    *reinterpret_cast<std::uint16_t*>(node + 0xc) = slot;             // node slot-id
    *reinterpret_cast<std::uint16_t*>(m + 0) = static_cast<std::uint16_t>(count + 1);  // count++
    *reinterpret_cast<std::uint32_t*>(node + 0) = key;               // node key

    const std::uint32_t head = *reinterpret_cast<std::uint32_t*>(m + 4);
    const std::uint32_t nodeAddr = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(node));
    if (head == 0) {                                                 // empty list
        *reinterpret_cast<std::uint32_t*>(m + 4) = nodeAddr;         // head = node
        *reinterpret_cast<std::uint32_t*>(node + 4) = 0;
        *reinterpret_cast<std::uint32_t*>(*reinterpret_cast<std::uint32_t*>(m + 4) + 8) = 0;  // head[+8]=0
        return *reinterpret_cast<std::uint16_t*>(node + 0xc);
    }
    const std::uint32_t tail = *reinterpret_cast<std::uint32_t*>(m + 8);
    if (tail == 0) {                                                 // one node -> becomes tail
        *reinterpret_cast<std::uint32_t*>(m + 8) = nodeAddr;         // tail = node
        *reinterpret_cast<std::uint32_t*>(node + 8) = 0;
        *reinterpret_cast<std::uint32_t*>(*reinterpret_cast<std::uint32_t*>(m + 8) + 4) = *reinterpret_cast<std::uint32_t*>(m + 4);  // tail[+4]=head
        *reinterpret_cast<std::uint32_t*>(*reinterpret_cast<std::uint32_t*>(m + 4) + 8) = *reinterpret_cast<std::uint32_t*>(m + 8);  // head[+8]=tail
        return *reinterpret_cast<std::uint16_t*>(node + 0xc);
    }
    *reinterpret_cast<std::uint32_t*>(node + 4) = *reinterpret_cast<std::uint32_t*>(m + 8);  // node[+4]=tail
    *reinterpret_cast<std::uint32_t*>(*reinterpret_cast<std::uint32_t*>(m + 8) + 8) = nodeAddr;  // tail[+8]=node
    *reinterpret_cast<std::uint32_t*>(m + 8) = nodeAddr;             // tail = node
    *reinterpret_cast<std::uint32_t*>(node + 8) = 0;
    return *reinterpret_cast<std::uint16_t*>(node + 0xc);
}
RH_ScopedInstall(PoolInsert485a70, 0x00485a70);
