// Mashed RE — AI spline-bank switcher (FUN_00417180): ONE body for both build targets (fwd494fd0, 2026-10-16).
//
// Before this file the RVA had two bodies, which is the dual-copy class (re/analysis/DUAL_COPY_AUDIT_2026-09-29.md,
// ROADMAP D4 item 2):
//   Ai/AiPreTick.cpp     .asi-only, installed, the verbatim port the C3 evidence covers (A/B driver AbAiBank,
//                        live race: rule0/track0 2305+9473 calls, 0 mismatches, all four branch families natural)
//   Ai/AiStandalone.cpp  exe+asi, `BankSwitch`, the exe's private copy (audit verdict UNREVIEWED)
// The two were compared line by line on 2026-10-16 and the state machine is the same one: same addresses (0x0089a500
// request, 0x0089a504 timer, 0x0089a4cc line type, 0x0089a4d0 spline index, 0x008022ac / 0x00801ca0 / 0x00802ec4 +
// idx*0x204 bank counts, 0x007f1008 tick, 0x0089a368 flag, 0x007f0ff8 / 3000, 0x0063bd90 dedup), same branch order,
// same read-once / re-read pattern, same 9000 cap. This TU keeps the AiPreTick.cpp text (the body the evidence cites)
// and puts the two things that cannot be the same in both targets behind two thunks below.
//
// The two target-specific bindings (the audit's P4: arithmetic core shared, thunks per target):
//   SplineBankReset()  FUN_00414030(-1). The original's -1 path is `mov eax,0x8032d4 / mov [eax],0x3e8 /
//                      add eax,0x14 / cmp eax,0x803324 / jl` (0x00414039..0x0041404e, read from
//                      original/MASHED.exe.unpatched): 1000 into FOUR slots, 0x008032d4 + i*0x14, i = 0..3. The inline
//                      loop below is that, so it needs no original code and is the same in both targets. (hooks.csv's
//                      "resets all 5 slots" for FUN_00414030 is wrong: 0x00803324 - 0x008032d4 = 0x50 = 4 * 0x14.)
//   RandUnit()         FUN_00472650(0, 1.0f). .asi: the ORIGINAL function (the process shares its ring with the
//                      original's other callers). exe: Ai_RandRange(0, 1), the verbatim FUN_00534870 / FUN_00534990
//                      port in AiStandalone.cpp, which has no entropy input (see the note there) but whose CALL INDEX
//                      cannot match the original's. Same law, same distribution, not the same draw.
//
// The probability gate reads `_DAT_005cc32c`, which is 0.5f in the original image; the image-pad is zero-filled in the
// standalone, so it is a literal here (AiStandalone.cpp's kBankRandGate says the same).
//
// FPU protocol: FUN_00472650 returns float10 in ST0, consumed by the caller's FCOM, so the .asi binding declares it
// float-returning (PC=24 value-identical), exactly as the AiPreTick.cpp port did.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched; prologue 0x00417180: 53 55 56 8b 74 24 10)

#include "../Core/HookSystem.h"
#include "AiState.h"
#include "AiStandalone.h"

#include <cstdint>

using namespace Ai;

namespace {

constexpr float kBankRandGate = 0.5f;   // _DAT_005cc32c

// FUN_00414030(-1): 1000 into the four per-vehicle spline-continuity slots (0x00414039..0x0041404e).
inline void SplineBankReset()
{
    for (std::uintptr_t p = 0x008032d4u; p < 0x00803324u; p += 0x14u)
        I32(p) = 1000;
}

#ifdef MASHED_STANDALONE
inline float RandUnit() { return Ai_RandRange(0.0f, 1.0f); }
#else
typedef float (__cdecl* fn_rand_t)(int, std::uint32_t);
inline float RandUnit() { return reinterpret_cast<fn_rand_t>(0x00472650)(0, 0x3f800000u); }  // float10-ST0
#endif

} // namespace

// ===========================================================================
// 0x00417180  AiBankSwitch(vehicle)
// ===========================================================================
extern "C" __declspec(dllexport)
void __cdecl AiBankSwitch(int veh)
{
    const std::uintptr_t s = static_cast<std::uintptr_t>(veh) * 0x74u;

    if (I32(0x0089a500u + s) != 0) {                       // 0x0041718a switch request
        int t = I32(0x0089a4ccu + s);                      // 0x004171a5 (read ONCE into ECX)
        I32(0x0089a504u + s) = 1;                          // 0x004171ad start timer
        if (t == 2) {                                      // 0x004171ab slow lines
            int idx = I32(0x0089a4d0u + s) + 1;            // 0x004171b5
            I32(0x0089a4d0u + s) = idx;                    // 0x004171c0
            if (idx > 2) I32(0x0089a4d0u + s) = 0;         // 0x004171c8
            SplineBankReset();                             // 0x004171d0
        }
        if (t == 1) {                                      // 0x004171d8 inside lines
            int idx = I32(0x0089a4d0u + s) + 1;            // 0x004171e1
            I32(0x0089a4d0u + s) = idx;                    // 0x004171ea
            if (idx > 2 || I32(0x008022acu + static_cast<std::uintptr_t>(idx) * 0x204u) < 4) { // 0x004171f0/0x004171f8
                I32(0x0089a4ccu + s) = 2;                  // 0x00417200
                I32(0x0089a4d0u + s) = 0;                  // 0x00417206
            }
        }
        if (I32(0x0089a4ccu + s) == 0) {                   // 0x0041720c race lines (RE-READS memory)
            int idx = I32(0x0089a4d0u + s) + 1;            // 0x00417214
            I32(0x0089a4d0u + s) = idx;                    // 0x0041721d
            if (idx > 2 || I32(0x00801ca0u + static_cast<std::uintptr_t>(idx) * 0x204u) < 4) { // 0x00417223/0x0041722b
                I32(0x0089a4ccu + s) = 1;                  // 0x00417233
                I32(0x0089a4d0u + s) = 0;                  // 0x00417239
            }
            SplineBankReset();                             // 0x00417241 (called on BOTH outcomes)
        }
        if (I32(0x0089a4ccu + s) == 3) {                   // 0x00417249 cheat lines (RE-READS memory)
            int idx = I32(0x0089a4d0u + s) + 1;            // 0x00417252
            I32(0x0089a4d0u + s) = idx;                    // 0x0041725b
            if (idx > 2 || I32(0x00802ec4u + static_cast<std::uintptr_t>(idx) * 0x204u) < 4) { // 0x00417261/0x00417269
                I32(0x0089a4ccu + s) = 0;                  // 0x00417271
                I32(0x0089a4d0u + s) = 0;                  // 0x00417277
            }
            SplineBankReset();                             // 0x0041727f (called on BOTH outcomes)
        }
        I32(0x0089a500u + s) = 0;                          // 0x00417287 clear request
    }

    // switch timer (0x0041728d)
    int timer = I32(0x0089a504u + s);
    if (timer != 0 && timer < 9000) {                      // 0x00417293/0x00417297
        I32(0x0089a504u + s) = timer + I32(0x007f1008u);   // 0x0041729e..0x004172a7
        return;                                            // 0x004172b0
    }
    I32(0x0089a504u + s) = 0;                              // 0x004172b1

    // line-type reset by debug flag (0x004172b7)
    if (I32(0x0089a368u) == 0) I32(0x0089a4ccu + s) = 0;   // 0x004172b7/0x004172bf
    if (I32(0x0089a368u) == 2) I32(0x0089a4ccu + s) = 0;   // 0x004172c5/0x004172cd
    if (I32(0x0089a368u) == 1) {                           // 0x004172d3
        if (I32(0x0089a4ccu + s) != 2) SplineBankReset();  // 0x004172db/0x004172e5
        I32(0x0089a4ccu + s) = 2;                          // 0x004172ed
    }

    // low-frequency random spline variation (0x004172f3)
    int period = I32(0x007f0ff8u) / 3000;                  // 0x004172f9 (signed /3000 idiom)
    if ((period & 0xf) == 0xf) {                           // 0x0041730e/0x00417311
        if (I32(0x0063bd90u) != period) {                  // 0x00417316 dedup
            float rnd = RandUnit();                        // 0x00417324 FUN_00472650(0, 1.0f)
            if (rnd < kBankRandGate) {                     // 0x00417329 (TEST 0x5/JP-skip) vs _DAT_005cc32c = 0.5f
                I32(0x0089a4d0u + s) = I32(0x0089a4d0u + s) + 1; // 0x00417339
                SplineBankReset();                         // 0x00417348
            }
        }
        I32(0x0063bd90u) = period;                         // 0x00417350
    } else {
        I32(0x0063bd90u) = 0;                              // 0x0041735e
    }
}
RH_ScopedInstall(AiBankSwitch, 0x00417180);
