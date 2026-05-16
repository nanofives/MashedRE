// Mashed RE - Frontend menu state machine helpers.
// Analysis notes: re/analysis/frontend_promote_menus_a/
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x0042ae10  MenuReadinessCheckA -- void() readiness check; path A/B char table scan
//   0x0042aeb0  MenuReadinessCheckB -- void() readiness check; variant of A: +1 byte offset, no e7c8 guard
//
// Note: 0x0042ac00 (MenuGroupCount) already implemented in MenuGetters.cpp (prior session).
// Note: 0x0042ac50 (MenuCenterCalc) REFUSED: implicit EAX register arg not supported by
//   NativeFunction harness. D-10639 filed (re/DEFERRED.md): harness needs EAX-in support.
//   (Earlier draft of this comment cited D-10637 which actually tracks a different gap
//   — Interceptor-blocked-by-E9-JMP in PROMOTION_QUEUE c4-batch-a-s16. D-10639 is correct.)
//
// Uncertainties carried (do not block correctness):
//   U-3445 (FUN_0040e470 return semantics), U-3446 (76-byte table entry layout)
//   U-3447 (DAT_007f1a0c mode value 0x1000 meaning), U-3448 (piVar3 += 4 stride)

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// Callee: FUN_0040e470 (C2, RVA 0x0040e470)
// 14-byte getter: returns *(PTR_PTR_005f2770 + param_1*4 + 0x34).
// Used as car-state query; return value compared to 2 in callers.
// Not yet reimplemented; call through to original binary address.
// ref: re/analysis/race_results/0040e470.md
// [UNCERTAIN U-3445]: return semantics (AI-slot state meaning) unresolved.
// ---------------------------------------------------------------------------
typedef std::int32_t (__cdecl *FunSlotStateFn_t)(int param_1);
static constexpr std::uintptr_t kFun0040e470 = 0x0040e470u;
static inline std::int32_t CallFun0040e470(int idx) {
    return reinterpret_cast<FunSlotStateFn_t>(kFun0040e470)(idx);
}

// ---------------------------------------------------------------------------
// MenuReadinessCheckA  --  0x0042ae10
//
// Original: FUN_0042ae10 (156 bytes, 0x0042ae10..0x0042aeac)
// Signature: undefined4 FUN_0042ae10(void)
//   Returns: 0 (not ready) or 1 (ready)
//   Side-effects: none (read-only; no writes to memory)
//
// Guard:
//   if (DAT_0067eab0 != 2 AND DAT_0067e7c8 == 0 AND DAT_00898ab0 != 0): return 0
//
// Path A (when DAT_007f1a0c == 0x1000):
//   pcVar1 = &DAT_007f1502
//   Loop while *(char*)(pcVar1 - 0x4c0) == '\0' OR *pcVar1 != '\0':
//     pcVar1 += 0x4c (stride 76 bytes)
//     if pcVar1 > 0x7f1761: return 0
//   return 1
//
// Path B (when DAT_007f1a0c != 0x1000):
//   piVar3 = &DAT_007f1a14; iVar4 = 0
//   Loop while any condition:
//     *piVar3 == -1, OR
//     FUN_0040e470(iVar4) == 2 [UNCERTAIN U-3445], OR
//     *(char*)(0x007f1042 + *piVar3 * 0x4c) == '\0', OR
//     *(char*)(0x007f1502 + *piVar3 * 0x4c) != '\0':
//     piVar3 += 4; iVar4 += 1  [UNCERTAIN U-3448: int* advance = 16 bytes per step]
//     if piVar3 > 0x7f1a53: return 0
//   return 1
//
// Memory accesses at (all within 0x0042ae10):
//   0x0067eab0 -- mode/state guard (int32, read)
//   0x0067e7c8 -- guard == 0 (int32, read)
//   0x00898ab0 -- guard != 0 (int32, read)
//   0x007f1a0c -- dispatch selector (int32, read); 0x1000 = Path A
//   0x007f1502 -- char table B base (path A)
//   0x007f1042 -- char table A base = 0x7f1502 - 0x4c0 (path A + path B)
//   0x4c (76)  -- table entry stride
//   0x7f1761   -- path A upper bound
//   0x007f1a14 -- path B int* table base
//   0x7f1a53   -- path B upper bound
//
// Callee:
//   0x0040e470 FUN_0040e470 (C2) -- car-state query; return compared to 2 [UNCERTAIN U-3445]
//
// ref: re/analysis/frontend_promote_menus_a/0x0042ae10.md
// U-3445 (FUN_0040e470 semantics), U-3446 (76-byte entry layout),
// U-3447 (0x1000 mode meaning), U-3448 (piVar3 stride)
// ---------------------------------------------------------------------------

// 0x0042ae10
extern "C" __declspec(dllexport) int __cdecl MenuReadinessCheckA() {
    // Guard: if (mode != 2 AND e7c8 == 0 AND 898ab0 != 0): not ready
    std::int32_t mode     = *reinterpret_cast<std::int32_t*>(0x0067eab0u);
    std::int32_t guard2   = *reinterpret_cast<std::int32_t*>(0x0067e7c8u);
    std::int32_t guard3   = *reinterpret_cast<std::int32_t*>(0x00898ab0u);

    if (mode != 2 && guard2 == 0 && guard3 != 0) {
        return 0;
    }

    std::int32_t dispatch = *reinterpret_cast<std::int32_t*>(0x007f1a0cu);

    if (dispatch == 0x1000) {
        // Path A: stride-0x4c char scan at 0x7f1502
        // Table A base: pcVar1 - 0x4c0 = 0x7f1042
        // Table B base: pcVar1         = 0x7f1502
        const std::int8_t* pcVar1 = reinterpret_cast<const std::int8_t*>(0x007f1502u);

        while (true) {
            // Loop condition: table A entry is '\0' OR table B entry != '\0'
            if (*reinterpret_cast<const std::int8_t*>(
                    reinterpret_cast<std::uintptr_t>(pcVar1) - 0x4c0u) == '\0'
                || *pcVar1 != '\0') {
                // Advance by stride 0x4c
                pcVar1 += 0x4c;
                if (reinterpret_cast<std::uintptr_t>(pcVar1) > 0x007f1761u) {
                    return 0;
                }
            } else {
                // Both conditions false: table A non-'\0' AND table B == '\0'
                return 1;
            }
        }
    } else {
        // Path B: car-index + FUN_0040e470 check
        const std::int32_t* piVar3 = reinterpret_cast<const std::int32_t*>(0x007f1a14u);
        int iVar4 = 0;

        while (true) {
            // Loop condition: any of four predicates true → keep scanning
            bool cond = (*piVar3 == -1)
                     || (CallFun0040e470(iVar4) == 2)  // [UNCERTAIN U-3445]
                     || (*reinterpret_cast<const std::int8_t*>(
                             0x007f1042u + static_cast<std::uint32_t>(*piVar3) * 0x4cu) == '\0')
                     || (*reinterpret_cast<const std::int8_t*>(
                             0x007f1502u + static_cast<std::uint32_t>(*piVar3) * 0x4cu) != '\0');

            if (!cond) {
                return 1;
            }

            // Advance: piVar3 += 4 on int* = +16 bytes [UNCERTAIN U-3448]
            piVar3 += 4;
            iVar4 += 1;
            if (reinterpret_cast<std::uintptr_t>(piVar3) > 0x007f1a53u) {
                return 0;
            }
        }
    }
}

RH_ScopedInstall(MenuReadinessCheckA, 0x0042ae10);

// ---------------------------------------------------------------------------
// MenuReadinessCheckB  --  0x0042aeb0
//
// Original: FUN_0042aeb0 (156 bytes, 0x0042aeb0..0x0042af4c)
// Signature: undefined4 FUN_0042aeb0(void)
//   Returns: 0 (not ready) or 1 (ready)
//   Side-effects: none (read-only)
//
// Structural variant of FUN_0042ae10 (MenuReadinessCheckA). Differences:
//   1. Guard omits DAT_0067e7c8 == 0 check: only (eab0 != 2 AND 898ab0 != 0)
//   2. Table bases shifted +1: 0x7f1503 (vs 0x7f1502), 0x7f1043 (vs 0x7f1042)
//   3. Path A upper bound 0x7f1762 (vs 0x7f1761)
//   All other logic identical.
//
// Guard:
//   if (DAT_0067eab0 != 2 AND DAT_00898ab0 != 0): return 0
//   (No DAT_0067e7c8 check.)
//
// Path A (when DAT_007f1a0c == 0x1000):
//   pcVar1 = &DAT_007f1503  (table B+1 base)
//   pcVar1 - 0x4c0 = 0x007f1043  (table A+1 base)
//   Loop while *(char*)(pcVar1 - 0x4c0) == '\0' OR *pcVar1 != '\0':
//     pcVar1 += 0x4c
//     if pcVar1 > 0x7f1762: return 0
//   return 1
//
// Path B (when DAT_007f1a0c != 0x1000):
//   piVar3 = &DAT_007f1a14; iVar4 = 0
//   Loop while any:
//     *piVar3 == -1, OR FUN_0040e470(iVar4) == 2, OR
//     *(char*)(0x007f1043 + *piVar3 * 0x4c) == '\0', OR
//     *(char*)(0x007f1503 + *piVar3 * 0x4c) != '\0':
//     piVar3 += 4; iVar4 += 1
//     if piVar3 > 0x7f1a53: return 0
//   return 1
//
// Memory accesses at (all within 0x0042aeb0):
//   0x0067eab0 -- mode guard checked != 2
//   0x00898ab0 -- guard checked != 0
//   0x007f1a0c -- dispatch selector (0x1000 = Path A)
//   0x007f1503 -- char table B+1 base (path A)
//   0x007f1043 -- char table A+1 base = 0x7f1503 - 0x4c0 (path A + path B)
//   0x4c (76)  -- entry stride
//   0x7f1762   -- path A upper bound
//   0x007f1a14 -- path B int* base
//   0x7f1a53   -- path B upper bound
//
// Callee:
//   0x0040e470 FUN_0040e470 (C2) -- [UNCERTAIN U-3445]
//
// ref: re/analysis/frontend_promote_menus_a/0x0042aeb0.md
// U-3445 (FUN_0040e470 semantics shared with A), U-3446 (+1 byte column rel. A)
// ---------------------------------------------------------------------------

// 0x0042aeb0
extern "C" __declspec(dllexport) int __cdecl MenuReadinessCheckB() {
    // Guard: if (mode != 2 AND 898ab0 != 0): not ready  (no e7c8 guard)
    std::int32_t mode  = *reinterpret_cast<std::int32_t*>(0x0067eab0u);
    std::int32_t guard = *reinterpret_cast<std::int32_t*>(0x00898ab0u);

    if (mode != 2 && guard != 0) {
        return 0;
    }

    std::int32_t dispatch = *reinterpret_cast<std::int32_t*>(0x007f1a0cu);

    if (dispatch == 0x1000) {
        // Path A: table B+1 base = 0x7f1503; table A+1 base = 0x7f1043
        const std::int8_t* pcVar1 = reinterpret_cast<const std::int8_t*>(0x007f1503u);

        while (true) {
            if (*reinterpret_cast<const std::int8_t*>(
                    reinterpret_cast<std::uintptr_t>(pcVar1) - 0x4c0u) == '\0'
                || *pcVar1 != '\0') {
                pcVar1 += 0x4c;
                if (reinterpret_cast<std::uintptr_t>(pcVar1) > 0x007f1762u) {
                    return 0;
                }
            } else {
                return 1;
            }
        }
    } else {
        // Path B: same table base as A but +1 byte column
        const std::int32_t* piVar3 = reinterpret_cast<const std::int32_t*>(0x007f1a14u);
        int iVar4 = 0;

        while (true) {
            bool cond = (*piVar3 == -1)
                     || (CallFun0040e470(iVar4) == 2)  // [UNCERTAIN U-3445]
                     || (*reinterpret_cast<const std::int8_t*>(
                             0x007f1043u + static_cast<std::uint32_t>(*piVar3) * 0x4cu) == '\0')
                     || (*reinterpret_cast<const std::int8_t*>(
                             0x007f1503u + static_cast<std::uint32_t>(*piVar3) * 0x4cu) != '\0');

            if (!cond) {
                return 1;
            }

            piVar3 += 4;
            iVar4 += 1;
            if (reinterpret_cast<std::uintptr_t>(piVar3) > 0x007f1a53u) {
                return 0;
            }
        }
    }
}

RH_ScopedInstall(MenuReadinessCheckB, 0x0042aeb0);
