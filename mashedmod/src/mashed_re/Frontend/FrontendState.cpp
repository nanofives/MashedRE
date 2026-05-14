// Mashed RE — Frontend state accessor functions.
// Three pure-leaf C2->C3 promotions from the frontend_c0_promote cluster.
// All originals are pure leaves (callees_depth1: []).
// See re/analysis/frontend_c0_promote/ for per-function analysis notes.
//
// Binary anchor: MASHED.exe size=2,846,720
//   SHA-256 (unpatched): BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (patched binary SHA differs — .unpatched preserves the anchor)
//
// Candidate 0x0042f020 (VehicleFlagClear) REFUSED: __fastcall with implicit
// EAX register arg is not supported by the Frida NativeFunction harness
// (which uses 'mscdecl' calling convention only). Filed as [UNCERTAIN U-3594].
#include "../Core/HookSystem.h"
#include <cstdint>

// ─────────────────────────────────────────────────────────────────────────────
// 0x00430b60  MenuSlotCount
//
// Ghidra decomp (frontend_c0_promote/0x00430b60.md):
//   cVar1 = (DAT_007f1a14 != -1)
//   if (DAT_007f1a24 != -1): cVar1 += 1
//   if (DAT_007f1a34 != -1): cVar1 += 1
//   if (DAT_007f1a44 != -1): cVar1 += 1
//   return cVar1
//
// Counts non-(-1) entries among four slot globals spaced 0x10 bytes apart.
// Return range: 0..4.  Pure read; no side-effects.
//
// Global addresses cited at (analysis note):
//   0x00430b62  DAT_007f1a14  slot 0
//   0x00430b6c  DAT_007f1a24  slot 1
//   0x00430b76  DAT_007f1a34  slot 2
//   0x00430b80  DAT_007f1a44  slot 3
// Sentinel 0xffffffff (-1) marks absent slot.
// [UNCERTAIN U-3600] Semantic meaning of these globals not traced.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) char __cdecl MenuSlotCount() {
    // 0x00430b62
    char cVar1 = (*reinterpret_cast<const std::int32_t*>(0x007f1a14) != -1) ? 1 : 0;
    // 0x00430b6c
    if (*reinterpret_cast<const std::int32_t*>(0x007f1a24) != -1) cVar1 += 1;
    // 0x00430b76
    if (*reinterpret_cast<const std::int32_t*>(0x007f1a34) != -1) cVar1 += 1;
    // 0x00430b80
    if (*reinterpret_cast<const std::int32_t*>(0x007f1a44) != -1) cVar1 += 1;
    return cVar1;
}
RH_ScopedInstall(MenuSlotCount, 0x00430b60);

// ─────────────────────────────────────────────────────────────────────────────
// 0x0042f6b0  MenuModeSync
//
// Ghidra decomp (frontend_c0_promote/0x0042f6b0.md):
//   switch (DAT_0067f184) {
//     case 0:   DAT_0067e9fc = 2;  break;
//     case 1:   DAT_0067e9fc = 3;  break;
//     case 2:   DAT_0067e9fc = 4;  break;
//     case 3:   DAT_0067e9fc = 6;  break;
//     case 5:   DAT_0067e9fc = 5;  break;
//     case 8:   DAT_0067e9fc = 7;  break;
//     case 9:   DAT_0067e9fc = 8;  break;
//     case 10:  DAT_0067e9fc = 9;  break;
//     case 11:  DAT_0067e9fc = 10; break;
//     default:  /* no write */     break;
//   }
//
// Switch base at 0x0042f6b8.  Cases 4, 6, 7 absent — default is no-op.
// [UNCERTAIN U-3595] Exact semantic mapping (menu index → game-mode code) not confirmed.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl MenuModeSync() {
    // 0x0042f6b8
    const std::int32_t mode = *reinterpret_cast<const std::int32_t*>(0x0067f184);
    std::int32_t& dest = *reinterpret_cast<std::int32_t*>(0x0067e9fc);
    switch (mode) {
        case 0:  dest = 2;  break;  // 0x0042f6bc
        case 1:  dest = 3;  break;  // 0x0042f6c6
        case 2:  dest = 4;  break;  // 0x0042f6d0
        case 3:  dest = 6;  break;  // 0x0042f6da
        case 5:  dest = 5;  break;  // 0x0042f6e4
        case 8:  dest = 7;  break;  // 0x0042f6ee
        case 9:  dest = 8;  break;  // 0x0042f6f8
        case 10: dest = 9;  break;  // 0x0042f702
        case 11: dest = 10; break;  // 0x0042f70c
        default: break;             // no write — falls through
    }
}
RH_ScopedInstall(MenuModeSync, 0x0042f6b0);

// ─────────────────────────────────────────────────────────────────────────────
// 0x00430910  MenuOptionSlotGet
//
// Ghidra decomp (frontend_c0_promote/0x00430910.md):
//   iVar1 = DAT_0067e9fc - 3;              // initial value (default return)
//   switch (DAT_0067e9fc) {                 // 0x00430924
//     case 3: case 4: case 5:
//       if (DAT_0067f184 in {0,3,4,6,7,8,9,10,11}) return 0;
//       break;
//     case 6: case 7: case 8: case 9:
//       if (DAT_0067f184 in {0,1,2,4,5,6,7}) return 0;
//       break;
//     default:
//       return iVar1;   // DAT_0067e9fc - 3
//   }
//   // table read (after exclusions)
//   return (&DAT_007f0a40)[DAT_0067f184 + DAT_0067f17c * 0xc];  // 0x004309a0
//
// Globals:
//   0x00430913  DAT_0067e9fc  game mode
//   0x004309a0  DAT_007f0a40  2D option-slot table base; row stride 0xc
//   DAT_0067f17c  row index (track/selection index)
//   DAT_0067f184  column index (option index)
// [UNCERTAIN U-3598] Exclusion set semantics not confirmed.
// [UNCERTAIN U-3599] Table element type / per-cell semantics unknown.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) int __cdecl MenuOptionSlotGet() {
    // 0x00430913
    const std::int32_t gameMode   = *reinterpret_cast<const std::int32_t*>(0x0067e9fc);
    const std::int32_t optionIdx  = *reinterpret_cast<const std::int32_t*>(0x0067f184);
    const std::int32_t rowIdx     = *reinterpret_cast<const std::int32_t*>(0x0067f17c);

    // iVar1 initialised to DAT_0067e9fc - 3 (default path)
    int iVar1 = gameMode - 3;

    switch (gameMode) {  // 0x00430924
        case 3:
        case 4:
        case 5:
            // Exclusion set for cases 3-5: {0,3,4,6,7,8,9,10,11}
            if (optionIdx == 0  || optionIdx == 3  || optionIdx == 4  ||
                optionIdx == 6  || optionIdx == 7  || optionIdx == 8  ||
                optionIdx == 9  || optionIdx == 10 || optionIdx == 11) {
                return 0;
            }
            break;
        case 6:
        case 7:
        case 8:
        case 9:
            // Exclusion set for cases 6-9: {0,1,2,4,5,6,7}
            if (optionIdx == 0 || optionIdx == 1 || optionIdx == 2 ||
                optionIdx == 4 || optionIdx == 5 || optionIdx == 6 ||
                optionIdx == 7) {
                return 0;
            }
            break;
        default:
            return iVar1;  // DAT_0067e9fc - 3
    }

    // Table read (after exclusions): 0x004309a0
    // DAT_007f0a40 is a 2D table; row stride = 0xc DWORDs (but byte-indexed: DAT_0067f184 + DAT_0067f17c * 0xc)
    iVar1 = reinterpret_cast<const std::int32_t*>(0x007f0a40)[optionIdx + rowIdx * 0xc];
    return iVar1;
}
RH_ScopedInstall(MenuOptionSlotGet, 0x00430910);
