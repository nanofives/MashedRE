// Mashed RE — Input subsystem Wave 3 mixed-candidate cluster.
// Session: wave3-s4   Branch: c3-sweep/wave3-s4
//
// One input function authored to C2 quality:
//   0x004972b0  JoypadAxisSnapshot  — per-frame joypad axis + keyboard read
//
// Analysis note: re/analysis/promote_c2_dinput_init/004972b0.md
//
// Binary anchor:
//   original\MASHED.exe  size 2,846,720
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// Callee prototypes (addresses Ghidra-cited in 004972b0.md)
// ---------------------------------------------------------------------------

// 0x00495790  FUN_00495790 — joypad count getter; returns DAT_00772fac (int).
// Note: C1 plate exists at re/analysis/input_dinput_d2/00495790.md;
//       tracker-gap D-10770; function is a 5-byte leaf getter.
typedef int (__cdecl* FnGetJoypadCount)();

// 0x004957a0  FUN_004957a0 — per-slot axis calibration read.
// Signature: void(int slot, int unk0, undefined* out_A, int unk1, undefined* out_B)
// Writes 8 bytes to out_A (DAT_0077311c[slot*8]) and 4 bytes to out_B (DAT_007730d4[slot*4]).
// Note: C1 plate re/analysis/input_dinput_d2/004957a0.md; tracker-gap D-10771.
typedef void (__cdecl* FnAxisCalibGet)(int slot, int unk0, void* out_a,
                                       int unk1, void* out_b);

// 0x004b6520  ZeroFillWrapper — zero N bytes starting at ptr.
// util/TimerSlot.cpp, hooks.csv row 1059, C2.
typedef void (__cdecl* FnZeroFill)(void* dst, int byte_count);

// 0x00496100  FUN_00496100 — keyboard device state reader.
// Reads keyboard device state into the 32-byte bitmap at the supplied ptr.
// Note: C1 plate re/analysis/input_dinput_d2/00496100.md; tracker-gap D-10772.
typedef void (__cdecl* FnKeyboardRead)(void* bitmap_buf);

static inline int         CallGetJoypadCount()  { return reinterpret_cast<FnGetJoypadCount>(0x00495790u)(); }
static inline void        CallAxisCalibGet(int s, int u0, void* a, int u1, void* b)
                          { reinterpret_cast<FnAxisCalibGet>(0x004957a0u)(s, u0, a, u1, b); }
static inline void        CallZeroFill(void* d, int n) { reinterpret_cast<FnZeroFill>(0x004b6520u)(d, n); }
static inline void        CallKeyboardRead(void* p)    { reinterpret_cast<FnKeyboardRead>(0x00496100u)(p); }

// ===========================================================================
// 0x004972b0  JoypadAxisSnapshot
//
// Per-frame joypad axis snapshot + keyboard read.
// Called once per frame from the input update path.
//
// Body 0x004972b0–0x00497307, 88 bytes.
//
// Sequence:
//   1. count = FUN_00495790()          — read joypad count
//   2. if count > 0:
//        for slot in 0..count-1:
//          FUN_004957a0(slot, 0,
//                       &DAT_0077311c + slot*8,    // 8-byte axis-cal "B" row
//                       0,
//                       &DAT_007730d4 + slot*4)    // 4-byte axis-cal "A" row
//   3. FUN_004b6520(&DAT_0077313c, 0x20)           — zero 32-byte keyboard bitmap
//   4. FUN_00496100(&DAT_0077313c)                 — read keyboard state into bitmap
//
// Globals:
//   DAT_00772fac — joypad count (read-only via FUN_00495790)
//   DAT_0077311c — per-joypad 8-byte axis-cal "B" output; stride 8
//   DAT_007730d4 — per-joypad 4-byte axis-cal "A" output; stride 4
//   DAT_0077313c — 32-byte keyboard state bitmap; zeroed then filled
//
// No return value. No branches other than the loop gate.
//
// [UNCERTAIN] markers: none in the C2 analysis note.
// ===========================================================================

// 0x004972b0
extern "C" __declspec(dllexport) void __cdecl JoypadAxisSnapshot() {
    const int count = CallGetJoypadCount();  // 0x004972bc: CALL 0x00495790
    if (count > 0) {
        // Array bases (cited in 004972b0.md)
        std::uint8_t* pAxisB = reinterpret_cast<std::uint8_t*>(0x0077311cu); // stride 8
        std::uint8_t* pAxisA = reinterpret_cast<std::uint8_t*>(0x007730d4u); // stride 4
        for (int slot = 0; slot < count; ++slot) {
            // 0x004972d5: CALL 0x004957a0 with slot, 0, puVar2, 0, puVar4
            CallAxisCalibGet(slot, 0, pAxisB, 0, pAxisA);
            pAxisA += 4; // stride 4 per slot (cited at 004972b0.md)
            pAxisB += 8; // stride 8 per slot
        }
    }
    // 0x004972f5: zero 0x20 bytes at DAT_0077313c
    void* const kbBuf = reinterpret_cast<void*>(0x0077313cu);
    CallZeroFill(kbBuf, 0x20);
    // 0x004972ff: read keyboard device state into the zero'd buffer
    CallKeyboardRead(kbBuf);
}

RH_ScopedInstall(JoypadAxisSnapshot, 0x004972b0);
