// Mashed RE — DirectInput initialization-chain reimplementations.
// Session: ma2-frida-s7   Branch: c3/ma2-frida-s7
//
// First C3 promotions for the input subsystem. Six hooks covering the DirectInput
// init chain, helper leaves and a bare-RET placeholder.
//
// Function map (RVA -> reimpl export):
//   0x00499720  GetInputHinst        — HINSTANCE getter; MOV EAX, [DAT_007e9580]; RET.
//   0x00495530  CreateDInputObject   — IDirectInput8A creation wrapper (calls
//                                       game printf + dinput8!DirectInput8Create
//                                       thunk).
//   0x004955b0  CreateDInputObjectBool — 11-byte bool predicate wrapping the
//                                         above (returns 1 if HRESULT>=0 else 0).
//   0x0045b350  RwInitNullStub        — 1-byte bare RET (0xC3); zero-body
//                                        placeholder called from RW init paths
//                                        (33 callers across binary).
//   0x004b6480  BitArrayClear         — REP STOSD/STOSB leaf clearing first N
//                                        bits of a byte buffer; sub-byte tail
//                                        handled via AND-mask.
//   0x00495830  JoypadStrcpy          — bounds-checked strcpy from joypad-slot
//                                        global to caller buffer; returns 1/0.
//
// Calling convention for all six: __cdecl (C ABI). All `extern "C"` so the
// linker emits unmangled names matching the entries registered with
// RH_ScopedInstall.
//
// CRT/IAT note: 0x00495530 calls two game-internal helpers (the printf wrapper
// at 0x004987b0 and the DirectInput8Create import thunk at 0x0049b300) plus
// kernel32!GetModuleHandleA via the IAT slot at 0x005cc038. To preserve
// bit-identical behaviour without depending on MSVC LTCG folding our calls
// into its own CRT, we route them through naked thunks that push+ret to the
// absolute game VAs, exactly as Save/SettingsConfig.cpp does for the CRT
// FILE* family.

#pragma optimize("", off)

#include "../Core/HookSystem.h"

#include <windows.h>
#include <cstdint>

// ---------------------------------------------------------------------------
// Naked thunks to game-internal functions.
// __declspec(naked) suppresses prologue/epilogue and prevents MSVC LTCG from
// substituting its own symbols.  Each thunk falls through to the absolute
// game VA via push+ret.
// ---------------------------------------------------------------------------

// 0x004987b0  FUN_004987b0 — debug printf wrapper (variadic, __cdecl).
__declspec(naked) static void __cdecl game_printf_dbg(const char* /*fmt*/, ...)
{
    __asm {
        push 0x004987b0
        ret
    }
}

// 0x0049b300  DirectInput8Create thunk inside MASHED.exe (jumps to
// dinput8.dll!DirectInput8Create via the IAT). __stdcall, 5 args.
//   HRESULT __stdcall DirectInput8Create(HINSTANCE hinst, DWORD dwVersion,
//                                        REFIID riidltf, LPVOID* ppvOut,
//                                        LPUNKNOWN punkOuter);
__declspec(naked) static int __stdcall game_dinput8_create(
    void* /*hinst*/, unsigned int /*dwVersion*/, const void* /*riidltf*/,
    void** /*ppvOut*/, void* /*punkOuter*/)
{
    __asm {
        push 0x0049b300
        ret
    }
}

// ---------------------------------------------------------------------------
// Global address constants
// ---------------------------------------------------------------------------

// HINSTANCE storage (loaded by boot; consumed by FUN_00498510 for LoadStringA).
// Cited in: 0x00499720 (single-load getter; 5 bytes)
static constexpr std::uintptr_t kInputHinstAddr   = 0x007e9580;

// IDirectInput8A** output sink (written by DirectInput8Create on success).
// Cited at PUSH in 0x0049553f.
static constexpr std::uintptr_t kDInput8OutAddr   = 0x00771e78;

// IID_IDirectInput8A GUID bytes (16 bytes at this address).
// Confirmed by memory_read in ma1-ghidra-s7 analysis:
//   {BF798030-483A-4DA2-AA99-5D64ED369700}
// Cited at PUSH in 0x00495544.
static constexpr std::uintptr_t kIidDInput8A      = 0x005d0a8c;

// Log strings (cited in 0x00495530 / 0x00495560).
//   "Creating DInput object\n\0"  at 0x005cfeb4
//   "Failed\n\0"                  at 0x005cfeac
static constexpr std::uintptr_t kLogStrCreating   = 0x005cfeb4;
static constexpr std::uintptr_t kLogStrFailed     = 0x005cfeac;

// Joypad-array bounds + base + stride. Cited in 0x00495830:
//   DAT_00772fac  : joypad count (signed int)
//   DAT_00771eb4  : per-slot base; stride 0x448 bytes (cited at 0x00495843).
static constexpr std::uintptr_t kJoypadCountAddr  = 0x00772fac;
static constexpr std::uintptr_t kJoypadArrayBase  = 0x00771eb4;
static constexpr std::size_t    kJoypadSlotStride = 0x448;

// ---------------------------------------------------------------------------
// 0x00499720  GetInputHinst — undefined4(void)
// ---------------------------------------------------------------------------
// Pure leaf, 5 bytes:
//   MOV EAX, dword ptr [0x007e9580]
//   RET
// Returns the HINSTANCE stored at kInputHinstAddr. Consumer FUN_00498510 passes
// the value as LoadStringA's first argument.
//
extern "C" __declspec(dllexport) std::uint32_t __cdecl GetInputHinst() {
    return *reinterpret_cast<const std::uint32_t*>(kInputHinstAddr);
}

RH_ScopedInstall(GetInputHinst, 0x00499720);

// ---------------------------------------------------------------------------
// 0x00495530  CreateDInputObject — undefined4(void)
// ---------------------------------------------------------------------------
// IDirectInput8A interface creation wrapper. 70 bytes in original.
//
// Steps (analysis note 00495530.md):
//   1. Log "Creating DInput object\n".
//   2. hinst = GetModuleHandleA(NULL).
//   3. hr = DirectInput8Create(hinst, 0x800, &IID_IDirectInput8A,
//                              &DAT_00771e78, NULL).
//   4. if (hr < 0) { log "Failed\n"; return 0; }
//   5. return 1;
//
// DAT_00771e78 is the output IDirectInput8A* sink (resolved U-0267 in analysis).
//
extern "C" __declspec(dllexport) std::uint32_t __cdecl CreateDInputObject() {
    game_printf_dbg(reinterpret_cast<const char*>(kLogStrCreating));

    HINSTANCE hinst = GetModuleHandleA(nullptr);
    int hr = game_dinput8_create(
        hinst,
        0x0800u,
        reinterpret_cast<const void*>(kIidDInput8A),
        reinterpret_cast<void**>(kDInput8OutAddr),
        nullptr);

    if (hr < 0) {
        game_printf_dbg(reinterpret_cast<const char*>(kLogStrFailed));
        return 0;
    }
    return 1;
}

RH_ScopedInstall(CreateDInputObject, 0x00495530);

// ---------------------------------------------------------------------------
// 0x004955b0  CreateDInputObjectBool — bool(void)
// ---------------------------------------------------------------------------
// 11-byte predicate wrapper:
//   return FUN_00495530() != 0;
// Pure thin wrapper. Promoted to C2 in ma1-ghidra-s3.
//
extern "C" __declspec(dllexport) std::uint32_t __cdecl CreateDInputObjectBool() {
    return CreateDInputObject() != 0 ? 1u : 0u;
}

RH_ScopedInstall(CreateDInputObjectBool, 0x004955b0);

// ---------------------------------------------------------------------------
// 0x0045b350  RwInitNullStub — void(void)
// ---------------------------------------------------------------------------
// 1-byte bare RET (0xC3). Single instruction: RET. No prologue, no operands.
// 33 callers across the binary (verified function_callers in ma1-ghidra-s3
// session). Functions as a null/placeholder callback in RW init paths.
//
extern "C" __declspec(dllexport) void __cdecl RwInitNullStub() {
    // Intentionally empty — mirror of the 0xC3 RET in the original.
}

RH_ScopedInstall(RwInitNullStub, 0x0045b350);

// ---------------------------------------------------------------------------
// 0x004b6480  BitArrayClear — void(byte* buf, uint count_bits)
// ---------------------------------------------------------------------------
// Zeros the first `count_bits` bits (LSB-first) of the byte array at `buf`.
// 88 bytes in original.
//
// Algorithm (analysis note 004b6480.md):
//   if (count_bits >= 8):
//     full_bytes = count_bits >> 3
//     REP STOSD  (full_bytes >> 2)        — 4-byte chunks
//     REP STOSB  (full_bytes & 3)         — 1-byte tail of the byte chunk
//     count_bits -= full_bytes * 8        — remaining sub-byte bits
//     buf        += full_bytes
//   if (count_bits != 0):
//     mask = 0
//     for k = count_bits .. 1:
//       mask |= 1 << ((k - 1) & 0x1f)
//     *buf &= ~mask
//
// Constants (all cited in original):
//   0x8  gate threshold (0x004b6484)
//   0x3  bits→bytes shift (0x004b6494)
//   0x5  bits→dwords shift (0x004b649c)
//   0x3  leftover-bytes mask (0x004b64a7)
//   0x1f shift-amount mask (0x004b64c5)
//
extern "C" __declspec(dllexport) void __cdecl BitArrayClear(
    std::uint8_t* buf, std::uint32_t count_bits)
{
    if (static_cast<std::int32_t>(count_bits) >= 8) {
        std::uint32_t full_bytes = count_bits >> 3;          // bits → bytes

        // REP STOSD: write (count_bits >> 5) dwords of zero.
        std::uint32_t dword_count = count_bits >> 5;
        std::uint8_t* p = buf;
        for (std::uint32_t i = dword_count; i != 0; --i) {
            *reinterpret_cast<std::uint32_t*>(p) = 0;
            p += 4;
        }

        // REP STOSB: write (full_bytes & 3) leftover bytes inside the dword
        // boundary.
        std::uint32_t byte_tail = full_bytes & 0x3u;
        for (std::uint32_t i = byte_tail; i != 0; --i) {
            *p = 0;
            p += 1;
        }

        // Advance state for the sub-byte tail loop.
        count_bits = count_bits - full_bytes * 8u;
        buf        = buf + full_bytes;
    }

    if (count_bits != 0) {
        std::uint32_t mask = 0;
        std::uint32_t k    = count_bits;
        do {
            k -= 1;
            mask |= (1u << (k & 0x1fu));
            count_bits -= 1;
        } while (count_bits != 0);
        *buf = static_cast<std::uint8_t>(*buf & ~static_cast<std::uint8_t>(mask));
    }
}

RH_ScopedInstall(BitArrayClear, 0x004b6480);

// ---------------------------------------------------------------------------
// 0x00495830  JoypadStrcpy — int(int slot_idx, int dst_ptr)
// ---------------------------------------------------------------------------
// Bounds-checked strcpy from the start of joypad-slot `slot_idx` to caller
// buffer `dst_ptr` (passed as raw int — same as the original's signature).
// Returns 1 on success, 0 on early-out.
//
// Decomp (analysis note 00495830.md, 52 bytes):
//   if (slot_idx < DAT_00772fac && dst_ptr != 0) {
//     src = &DAT_00771eb4 + slot_idx * 0x448;
//     do {
//       *(char*)(dst_ptr + (src - original_src)) = *src;
//       src += 1;
//     } while (last byte != '\0');
//     return 1;
//   }
//   return 0;
//
// The original uses an offset trick (param_2 - (int)pcVar2) so the loop can
// use a single induction pointer for both src reads and dst writes; that's a
// compiler optimisation, not a semantic requirement.  We restore the natural
// shape: walk src and dst in parallel until the NUL is copied.
//
extern "C" __declspec(dllexport) std::int32_t __cdecl JoypadStrcpy(
    std::int32_t slot_idx, std::int32_t dst_ptr)
{
    const std::int32_t count =
        *reinterpret_cast<const std::int32_t*>(kJoypadCountAddr);

    if (slot_idx >= count) return 0;
    if (dst_ptr == 0)      return 0;

    const char* src =
        reinterpret_cast<const char*>(kJoypadArrayBase) +
        static_cast<std::size_t>(slot_idx) * kJoypadSlotStride;
    char* dst = reinterpret_cast<char*>(static_cast<std::uintptr_t>(dst_ptr));

    char c;
    do {
        c = *src;
        *dst = c;
        ++src;
        ++dst;
    } while (c != '\0');

    return 1;
}

RH_ScopedInstall(JoypadStrcpy, 0x00495830);
