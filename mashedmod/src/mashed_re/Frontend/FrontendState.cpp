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
#include "RowMovers.h"
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
RH_ScopedInstall(MenuSlotCount, 0x00430b60);  // re-enabled 2026-05-24 c3-frontend-b

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
RH_ScopedInstall(MenuModeSync, 0x0042f6b0);  // re-enabled 2026-05-24 c3-frontend-b

// ─────────────────────────────────────────────────────────────────────────────
// 0x00430910  MenuOptionSlotGet
//
// Re-transcribed from the bytes 2026-10-12 (row-movers; capstone over
// 0x00430910..0x00430999, MASHED.exe.unpatched):
//   0x00430910  eax = [0x67e9fc] (mode); add eax,-3; cmp eax,6; ja 0x00430999
//               -> return eax, i.e. mode - 3 (non-zero for every mode outside
//               3..9, e.g. -1 for mode 2, 7 for mode 10).
//   0x0043091d  byte table 0x004309a4 = {0,0,0,1,1,1,1}; jump table 0x0043099c
//               = {0x0043092b, 0x0043095a}: modes 3/4/5 -> A, modes 6..9 -> B.
//   A 0x0043092b: sel = [0x67f184]; sel in {0,3,4,6,7,8,9,10,11} -> 0
//               (0x00430930..0x00430955, then 0x0043097f je for 10 and
//               0x00430981 cmp 0xb for 11).
//   B 0x0043095a: sel in {0,1,2,5,6,7,4,11} -> 0 (0x0043095f..0x0043097c,
//               then the shared 0x0043097f je for 4 and 0x00430981 for 11).
//   0x00430986  return dword [0x7f0a40 + (row*0xc + sel)*4], row = [0x67f17c]
//               (lea ecx,[ecx+ecx*2]; lea edx,[eax+ecx*4]; [edx*4+0x7f0a40]).
// The previous port (from the Ghidra decomp) omitted sel 11 from set B, so in
// modes 6..9 with sel 11 it read the table where the original returns 0.
// [UNCERTAIN U-3598] Exclusion set semantics not confirmed.
// [UNCERTAIN U-3599] Table element type / per-cell semantics unknown.
// ─────────────────────────────────────────────────────────────────────────────
int mashed_re::Frontend::MenuOptionSlotGetFor(std::int32_t gameMode,
                                              std::int32_t optionIdx,
                                              std::int32_t rowIdx) {
    const std::int32_t iVar1 = gameMode - 3;   // 0x00430915
    switch (gameMode) {
        case 3: case 4: case 5:                // case A 0x0043092b
            if (optionIdx == 0 || optionIdx == 3  || optionIdx == 4  ||
                optionIdx == 6 || optionIdx == 7  || optionIdx == 8  ||
                optionIdx == 9 || optionIdx == 10 || optionIdx == 11)
                return 0;                      // 0x00430934
            break;
        case 6: case 7: case 8: case 9:        // case B 0x0043095a
            if (optionIdx == 0 || optionIdx == 1 || optionIdx == 2 ||
                optionIdx == 4 || optionIdx == 5 || optionIdx == 6 ||
                optionIdx == 7 || optionIdx == 11)
                return 0;                      // 0x00430934
            break;
        default:
            return iVar1;                      // 0x0043091b ja 0x00430999
    }
    // 0x00430986..0x00430992
    return reinterpret_cast<const std::int32_t*>(0x007f0a40)[optionIdx + rowIdx * 0xc];
}

// 0x00430910 — hook binding: the image's DAT_0067e9fc / DAT_0067f184 / DAT_0067f17c.
extern "C" __declspec(dllexport) int __cdecl MenuOptionSlotGet() {
    return mashed_re::Frontend::MenuOptionSlotGetFor(
        *reinterpret_cast<const std::int32_t*>(0x0067e9fc),
        *reinterpret_cast<const std::int32_t*>(0x0067f184),
        *reinterpret_cast<const std::int32_t*>(0x0067f17c));
}
RH_ScopedInstall(MenuOptionSlotGet, 0x00430910);  // re-enabled 2026-05-24 c3-frontend-b
