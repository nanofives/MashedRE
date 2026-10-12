// Mashed RE — Frontend cursor-navigation reimplementations.
// C2->C3 promotions from the frontend_c0_promote cluster.
//
// Binary anchor: MASHED.exe size=2,846,720
//   sha256=BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
#include "../Core/HookSystem.h"
#include "RowMovers.h"
#include <cstdint>

// ─────────────────────────────────────────────────────────────────────────────
// Globals read/written by this cluster.  All in .bss/.data of MASHED.exe.
// ─────────────────────────────────────────────────────────────────────────────
static constexpr std::uintptr_t kDat_0067f17c = 0x0067f17cu; // row index (track/selection)
static constexpr std::uintptr_t kDat_0067f184 = 0x0067f184u; // column index (option)
static constexpr std::uintptr_t kDat_0067f1a4 = 0x0067f1a4u; // wrap-around flag (zeroed on clamp)
static constexpr std::uintptr_t kDat_0067ea68 = 0x0067ea68u; // multiplayer flag
static constexpr std::uintptr_t kDat_0067e9fc = 0x0067e9fcu; // game mode

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

static inline std::int32_t* gPtr(std::uintptr_t va) {
    return reinterpret_cast<std::int32_t*>(va);
}

// ─────────────────────────────────────────────────────────────────────────────
// The two challenge-row movers, transcribed from the bytes of
// MASHED.exe.unpatched (2026-10-12, capstone over 0x004322c0..0x004323b3 and
// 0x004323c0..0x00432447). One body per RVA (RowMovers.h): the hook export
// below and the standalone (Frontend/MenuNavSM.cpp Nav_MoveCursor) both call
// these with their own RowMoverIo binding.
// ─────────────────────────────────────────────────────────────────────────────
namespace mashed_re {
namespace Frontend {

// 0x004323c0  FUN_004323c0 (MenuCursorBack)
//   0x004323c1  ebx = [0x67f17c] (row)      0x004323c8  ebp = [0x67ea68]
//   0x004323d0  edi = [0x67f184] (sel)
//   top 0x004323d6: dec ebx; store row (0x004323d7); jns past the clamp;
//       clamp 0x004323df: ebx = 0; store row (0x004323e1); store [0x67f1a4]
//       = ebx = 0 (0x004323e7).
//   0x004323ee  FUN_00430830(ebx); != 0 -> return (0x004323f8)
//   0x004323fa  esi = (ebp != 0) * 2 + 1   (setne al; lea eax,[eax+eax+1];
//               eax is the 0 the callee just returned)
//   0x00432405  edi == esi -> 0x0043243c: dec ebx; store row; return.
//               This stores row - 1 unclamped: -1 when the row was 0.
//   walk 0x00432410: dec edi; store sel (0x00432413); edi < esi -> 0x0043242c;
//       FUN_00430910() == 0 -> loop (0x00432422).
//       found:     FUN_0042f6b0() (0x00432424); dec ebx; jmp top
//       exhausted: edi = esi; store sel (0x0043242e); FUN_0042f6b0();
//                  dec ebx (0x00432439); jmp top
//   The retry decrements ebx at 0x00432429/0x00432439 AND again at the top
//   (0x004323d6), so it moves the row down by 2, unlike the DOWN mover's
//   retry (0x00432398 dec + 0x00432300 inc = same row). No row store happens
//   between the walk and the top's store. FUN_0042f6b0 is called on every
//   retry whatever the mode (no mode test, unlike 0x0043237e).
void RowMoveBack(const RowMoverIo& io) {
    std::int32_t ebx = *io.row;                // 0x004323c1
    const std::int32_t ebp = *io.ea68;         // 0x004323c8
    std::int32_t edi = *io.sel;                // 0x004323d0
    for (;;) {
        --ebx;                                 // 0x004323d6
        *io.row = ebx;                         // 0x004323d7
        if (ebx < 0) {                         // 0x004323dd jns
            ebx = 0;                           // 0x004323df
            *io.row = ebx;                     // 0x004323e1
            *io.flag = ebx;                    // 0x004323e7
        }
        if (io.avail(ebx) != 0) return;        // 0x004323ee / 0x004323f8
        const std::int32_t esi = (ebp != 0) ? 3 : 1;   // 0x004323fa..0x00432403
        if (edi == esi) {                      // 0x00432405
            --ebx;                             // 0x0043243c
            *io.row = ebx;                     // 0x0043243d
            return;
        }
        for (;;) {
            --edi;                             // 0x00432410
            *io.sel = edi;                     // 0x00432413
            if (edi < esi) {                   // 0x00432419 jl
                edi = esi;                     // 0x0043242c
                *io.sel = edi;                 // 0x0043242e
                break;
            }
            if (io.optValid() != 0) break;     // 0x0043241b / 0x00432422
        }
        io.modeSync();                         // 0x00432424 / 0x00432434
        --ebx;                                 // 0x00432429 / 0x00432439
    }
}

// 0x004322c0  FUN_004322c0, from 0x004322ea (the caller already ran
// FUN_0042aa00(1) at 0x004322c2 and the slot-kind test
// [0x67ed3c + DAT_0067e9f8*0x40] in 6..7 at 0x004322d8/0x004322e1).
//   0x004322eb  ebx = [0x67f184] (sel)      0x004322f2  ebp = [0x67f17c] (row)
//   0x004322fa  edi = [0x67e9fc] (mode)
//   top 0x00432300: inc ebp; store row (0x00432304); ebp >= 0xd ->
//       ebp = 0xc; store row (0x00432311); [0x67f1a4] = 0 (0x00432317).
//   0x00432322  FUN_00430830(ebp); != 0 -> return (0x0043232c)
//   0x00432332  esi = ([0x67ea68] != 0) * 2 + 1; mode 0xa -> esi = 0xb
//               (0x0043234a); else mode 2 -> esi = 0 (0x00432356). [0x67ea68]
//               is re-read on every pass (0x00432332 is inside the loop).
//   0x00432358  ebx == esi -> 0x0043239e: dec ebp; [0x67f1a4] = 0; store row;
//               return (row back where it was before the inc).
//   walk 0x00432360: dec ebx; store sel (0x00432363); ebx < esi -> 0x00432376
//       (ebx = esi; store sel); else FUN_00430910() == 0 -> loop.
//   0x0043237e  mode 3/4/5 -> FUN_0042f6b0(); edi = [0x67e9fc] (0x00432392)
//   0x00432398  dec ebp; jmp top (retries the same row under the new sel).
void RowMoveFwd(const RowMoverIo& io) {
    std::int32_t ebx = *io.sel;                // 0x004322eb
    std::int32_t ebp = *io.row;                // 0x004322f2
    std::int32_t edi = *io.mode;               // 0x004322fa
    for (;;) {
        ++ebp;                                 // 0x00432300
        *io.row = ebp;                         // 0x00432304
        if (ebp >= 0xd) {                      // 0x00432301 / 0x0043230a jl
            ebp = 0xc;                         // 0x0043230c
            *io.row = ebp;                     // 0x00432311
            *io.flag = 0;                      // 0x00432317
        }
        if (io.avail(ebp) != 0) return;        // 0x00432322 / 0x0043232c
        std::int32_t esi = (*io.ea68 != 0) ? 3 : 1;   // 0x00432332..0x00432346
        if (edi == 0xa)    esi = 0xb;          // 0x0043233f / 0x0043234a
        else if (edi == 2) esi = 0;            // 0x00432351 / 0x00432356
        if (ebx == esi) {                      // 0x00432358
            --ebp;                             // 0x0043239e
            *io.flag = 0;                      // 0x0043239f
            *io.row = ebp;                     // 0x004323a9
            return;
        }
        for (;;) {
            --ebx;                             // 0x00432360
            *io.sel = ebx;                     // 0x00432363
            if (ebx < esi) {                   // 0x00432369 jl
                ebx = esi;                     // 0x00432376
                *io.sel = ebx;                 // 0x00432378
                break;
            }
            if (io.optValid() != 0) break;     // 0x0043236b / 0x00432372
        }
        if (edi == 3 || edi == 4 || edi == 5) {   // 0x0043237e..0x0043238b
            io.modeSync();                     // 0x0043238d
            edi = *io.mode;                    // 0x00432392
        }
        --ebp;                                 // 0x00432398
    }
}

}  // namespace Frontend
}  // namespace mashed_re

// ─────────────────────────────────────────────────────────────────────────────
// 0x004323c0  MenuCursorBack — hook binding: the image's own globals and the
// image's own callees (called by VA, as before), handed to RowMoveBack.
// Re-bound 2026-10-12 (row-movers). The previous body was a Ghidra-decomp
// transcription with the same final state; it also stored row - 1 between the
// walk and the loop top, which the bytes do not (0x00432429 is a register dec).
// ─────────────────────────────────────────────────────────────────────────────
static int __cdecl HookAvail(int row) {
    typedef int (__cdecl *tFUN_430830)(int);
    return reinterpret_cast<tFUN_430830>(ResolveVA(kRVA_FUN_00430830))(row);
}
static int __cdecl HookOptValid() {
    typedef int (__cdecl *tFUN_430910)();
    return reinterpret_cast<tFUN_430910>(ResolveVA(kRVA_FUN_00430910))();
}
static void __cdecl HookModeSync() {
    typedef void (__cdecl *tFUN_42f6b0)();
    reinterpret_cast<tFUN_42f6b0>(ResolveVA(kRVA_FUN_0042f6b0))();
}

extern "C" __declspec(dllexport) void __cdecl MenuCursorBack() {
    mashed_re::Frontend::RowMoverIo io;
    io.row      = gPtr(kDat_0067f17c);
    io.sel      = gPtr(kDat_0067f184);
    io.mode     = gPtr(kDat_0067e9fc);    // not read by RowMoveBack
    io.flag     = gPtr(kDat_0067f1a4);
    io.ea68     = gPtr(kDat_0067ea68);
    io.avail    = &HookAvail;
    io.optValid = &HookOptValid;
    io.modeSync = &HookModeSync;
    mashed_re::Frontend::RowMoveBack(io);
}
RH_ScopedInstall(MenuCursorBack, 0x004323c0);  // re-enabled 2026-05-24 c3-frontend-b
