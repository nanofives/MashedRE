// Mashed RE - Frontend menus_a mixed reimplementations.
// Analysis notes:
//   re/analysis/frontend_promote_menus_a/0x0042a940.md
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// Forward declarations for callees (not yet fully reimplemented; we call originals)
// ---------------------------------------------------------------------------

// 0x0040ce80  FUN_0040ce80  — double-deref field reader at PTR_PTR_005f2770
// Analysis: re/analysis/promote_c1_low_ab1/0x0040ce80.md  (C2 note)
// Signature: undefined4 FUN_0040ce80(int param_1)
// Pure read: *(PTR_PTR_005f2770[param_1] + 4). No side-effects.
static auto* const s_FUN_0040ce80 =
    reinterpret_cast<std::uint32_t(__cdecl*)(int)>(0x0040ce80);

// ---------------------------------------------------------------------------
// MenuTableSearch  --  0x0042a940
//
// Original: FUN_0042a940 (61 bytes, 0x0042a940..0x0042a97d)
// Signature: undefined4 FUN_0042a940(undefined4 param_1)
//   param_1: car slot index passed to FUN_0040ce80 as lookup key
// Returns: 0 if not found; value field (+8) from stride-3 table at 0x005f6748 on match.
// Side-effects: none (read-only; writes no globals, no memory)
//
// Algorithm (cited from 0x0042a940 body):
//   1. key = FUN_0040ce80(param_1)       [0x0042a944]
//   2. pos = 0
//   3. entry0 = *(int32_t*)(0x005f6748)  [0x0042a944]
//   4. Loop:
//      a. if entry0 == -1: return 0      [0x0042a94c — sentinel 0xffffffff]
//      b. if entry0 == key: break        [match found]
//      c. entry0 = *(int32_t*)(0x005f6754 + pos*4)  [0x0042a953]
//      d. pos += 3                        [0x0042a955 — stride 3 int32 units]
//   5. return *(undefined4*)(0x005f6750 + pos*4)  [0x0042a966 — value at +8 relative to key]
//
// Table layout (stride 3 int32, base 0x005f6748):
//   Entry N: key at 0x005f6748+N*12; +4 field (unknown, never read here);
//            value at 0x005f6750+N*12
//
// Uncertainties (do not block reimplementation):
//   U-3434: FUN_0040ce80 semantics unknown (pure mechanical double-deref).
//   U-3435: +4 field per stride-3 entry never accessed; role unknown.
//
// ref: re/analysis/frontend_promote_menus_a/0x0042a940.md
// ---------------------------------------------------------------------------

// 0x0042a940
extern "C" __declspec(dllexport) std::uint32_t __cdecl MenuTableSearch(
    std::uint32_t param_1)
{
    // Step 1: compute search key via callee FUN_0040ce80.
    // Cited at 0x0042a944: calls 0x0040ce80(param_1).
    int key = static_cast<int>(s_FUN_0040ce80(static_cast<int>(param_1)));

    // Step 2: initialise table position counter (int units) and load first key.
    // 0x005f6748: first key in stride-3 table (cited at 0x0042a944).
    int pos = 0;
    int entry = *reinterpret_cast<std::int32_t*>(0x005f6748u);

    // Step 3: linear table scan.
    while (true) {
        // End-of-table sentinel: -1 (0xffffffff). Cited at 0x0042a94c.
        if (entry == -1) {
            return 0u;
        }
        // Key match: break to return value.
        if (entry == key) {
            break;
        }
        // Advance to next entry: next key is at 0x005f6754 + pos*4 (cited at 0x0042a953).
        entry = *reinterpret_cast<std::int32_t*>(0x005f6754u + static_cast<unsigned>(pos) * 4u);
        // Stride is 3 int32 units (cited at 0x0042a955).
        pos += 3;
    }

    // Step 4: return value field at 0x005f6750 + pos*4 (cited at 0x0042a966).
    return *reinterpret_cast<std::uint32_t*>(0x005f6750u + static_cast<unsigned>(pos) * 4u);
}

RH_ScopedInstall(MenuTableSearch, 0x0042a940);  // re-enabled 2026-05-24 c3-frontend-a
