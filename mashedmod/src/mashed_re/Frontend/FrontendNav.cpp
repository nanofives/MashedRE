// Mashed RE — Frontend cursor-navigation reimplementations.
// C2->C3 promotions from the frontend_c0_promote cluster.
//
// Binary anchor: MASHED.exe size=2,846,720
//   sha256=BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
#include "../Core/HookSystem.h"
#include <cstdint>

// ─────────────────────────────────────────────────────────────────────────────
// Globals read/written by this cluster.  All in .bss/.data of MASHED.exe.
// ─────────────────────────────────────────────────────────────────────────────
static constexpr std::uintptr_t kDat_0067f17c = 0x0067f17cu; // row index (track/selection)
static constexpr std::uintptr_t kDat_0067f184 = 0x0067f184u; // column index (option)
static constexpr std::uintptr_t kDat_0067f1a4 = 0x0067f1a4u; // wrap-around flag (zeroed on clamp)
static constexpr std::uintptr_t kDat_0067ea68 = 0x0067ea68u; // multiplayer flag

// Callee RVAs — called directly into the original MASHED.exe image.
// 0x00430830  FUN_00430830  availability check by game mode / slot  (C2)
// 0x0042f6b0  FUN_0042f6b0  option-index -> game-mode writer        (C2)
// 0x00430910  FUN_00430910  option-validity check                   (C2)
static constexpr std::uintptr_t kRVA_FUN_00430830 = 0x00430830u;
static constexpr std::uintptr_t kRVA_FUN_0042f6b0 = 0x0042f6b0u;
static constexpr std::uintptr_t kRVA_FUN_00430910 = 0x00430910u;

// ─────────────────────────────────────────────────────────────────────────────
// ResolveVA: return a function pointer from an absolute virtual address.
// The "RVAs" throughout this project are actually absolute VAs (PE32 with
// preferred base 0x00400000, loaded without relocation).
// ─────────────────────────────────────────────────────────────────────────────
static inline void* ResolveVA(std::uintptr_t va) {
    return reinterpret_cast<void*>(va);
}

// Global-access helpers — typed references into MASHED.exe's data segment.
static inline std::int32_t& gRow()  { return *reinterpret_cast<std::int32_t*>(kDat_0067f17c); }
static inline std::int32_t& gCol()  { return *reinterpret_cast<std::int32_t*>(kDat_0067f184); }
static inline std::int32_t& gFlag() { return *reinterpret_cast<std::int32_t*>(kDat_0067f1a4); }
static inline std::int32_t  gMode() { return *reinterpret_cast<const std::int32_t*>(kDat_0067ea68); }

// ─────────────────────────────────────────────────────────────────────────────
// 0x004323c0  MenuCursorBack
//
// Backward 2D cursor navigation.
// Decrements DAT_0067f17c (row) and finds the previous valid option by
// scanning backward via FUN_00430830 / FUN_00430910, then syncs game-mode
// via FUN_0042f6b0.  Writes DAT_0067f17c, DAT_0067f184, DAT_0067f1a4.
//
// Ghidra decomp at 0x004323c0..0x00432431 (135 bytes):
//
//   iVar1 = DAT_0067ea68;          // mode/MP flag
//   iVar5 = DAT_0067f184;          // current option index (column)
//   do {                           // outer: scan rows backward
//     DAT_0067f17c -= 1;
//     if (DAT_0067f17c < 0) { DAT_0067f17c = 0; DAT_0067f1a4 = 0; }
//     iVar2 = DAT_0067f17c;
//     iVar3 = FUN_00430830(DAT_0067f17c);   // row available?
//     if (iVar3 != 0) return;
//     iVar3 = (iVar1 != 0) ? 2+1 : 1;      // min option: 3 if MP else 1
//     if (iVar5 == iVar3) { DAT_0067f17c = iVar2 - 1; return; }
//     do {                         // inner: scan options backward
//       iVar5 -= 1;
//       if (iVar5 < iVar3) {
//         DAT_0067f184 = iVar3;
//         FUN_0042f6b0();
//         DAT_0067f17c = iVar2 - 1;
//         iVar5 = iVar3;
//         break;                   // goto outer loop top
//       }
//       DAT_0067f184 = iVar5;
//       iVar4 = FUN_00430910();
//     } while (iVar4 == 0);
//     if (iVar4 != 0) {
//       FUN_0042f6b0();
//       DAT_0067f17c = iVar2 - 1;
//     }
//   } while (true);
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl MenuCursorBack() {
    typedef int  (__cdecl *tFUN_430830)(int);
    typedef void (__cdecl *tFUN_42f6b0)();
    typedef int  (__cdecl *tFUN_430910)();

    const auto FUN_00430830 = reinterpret_cast<tFUN_430830>(ResolveVA(kRVA_FUN_00430830));
    const auto FUN_0042f6b0 = reinterpret_cast<tFUN_42f6b0>(ResolveVA(kRVA_FUN_0042f6b0));
    const auto FUN_00430910 = reinterpret_cast<tFUN_430910>(ResolveVA(kRVA_FUN_00430910));

    const std::int32_t iVar1 = gMode();   // DAT_0067ea68 (MP flag); read once at entry
    std::int32_t       iVar5 = gCol();    // DAT_0067f184 (option index); read once at entry

    // Outer do-while: search backward through rows.
    for (;;) {
        gRow() -= 1;
        if (gRow() < 0) {
            gRow()  = 0;
            gFlag() = 0;  // DAT_0067f1a4 = 0
        }
        const std::int32_t iVar2 = gRow();  // save updated row

        // Is this row available?
        const std::int32_t iVar3_avail = FUN_00430830(gRow());
        if (iVar3_avail != 0) return;

        // Minimum valid option: 3 (multiplayer) or 1 (single player).
        const std::int32_t iVar3_min = (iVar1 != 0) ? (2 + 1) : 1;
        if (iVar5 == iVar3_min) {
            gRow() = iVar2 - 1;
            return;
        }

        // Inner do-while: search backward through options at this row.
        // Exit conditions:
        //   (a) iVar5 < iVar3_min  — wrap: set col to min, call f6b0, adjust row, reset iVar5
        //   (b) FUN_00430910 != 0  — valid option: call f6b0, adjust row, continue outer
        std::int32_t iVar4 = 0;
        bool inner_exhausted = false;
        do {
            iVar5 -= 1;
            if (iVar5 < iVar3_min) {
                gCol()  = iVar3_min;
                FUN_0042f6b0();
                gRow()  = iVar2 - 1;
                iVar5   = iVar3_min;
                inner_exhausted = true;
                break;
            }
            gCol() = iVar5;
            iVar4  = FUN_00430910();
        } while (iVar4 == 0);

        if (!inner_exhausted) {
            // iVar4 != 0: valid option found in inner loop.
            FUN_0042f6b0();
            gRow() = iVar2 - 1;
        }
        // In both branches: continue outer loop.
    }
}
// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(MenuCursorBack, 0x004323c0);
