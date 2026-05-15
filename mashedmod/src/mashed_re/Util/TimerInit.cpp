// Mashed RE — Timer init / utility leaf reimplementations.
// All RVAs verified against MASHED.exe anchor (size 2,846,720; SHA-256 BDCAE093…EFD3C0E).
//
// Functions in this file:
//   0x0041eda0  TimerBitFieldSet    void(int slot, int flag)
//   0x0041f000  TimerSlotDataCopy   void(int slot, int* dst)
//   0x00420d40  TimerArrayClear     void(void)
//   0x00422120  TimerInitLoop       void(void)
//   0x004222c0  TimerInitThunk      void(void)  -- thunk of TimerInitLoop
//   0x00422b10  TimerDwordClear     void(void)
//   0x00425b10  TimerGlobalZero     void(void)

#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>

// ─────────────────────────────────────────────────────────────────────────────
// 0x0041eda0  TimerBitFieldSet  void(int slot, int flag)
//
// Sets or clears bit 3 of the dword at 0x0063dc74 + slot*0x2ac.
//   if flag != 0: dword |= 0x00000008  (set bit 3)
//   if flag == 0: dword &= 0xfffffff7  (clear bit 3)
// ─────────────────────────────────────────────────────────────────────────────
static constexpr std::uintptr_t kTimerBitFieldBase   = 0x0063dc74;
static constexpr std::uint32_t  kTimerSlotStride      = 0x0000002ac;  // 684 bytes

extern "C" __declspec(dllexport) void __cdecl TimerBitFieldSet(int slot, int flag) {
    auto* field = reinterpret_cast<std::uint32_t*>(
        kTimerBitFieldBase + static_cast<std::uint32_t>(slot) * kTimerSlotStride);
    if (flag != 0) {
        *field |= 0x00000008u;
    } else {
        *field &= 0xfffffff7u;
    }
}

RH_ScopedInstall(TimerBitFieldSet, 0x0041eda0);

// ─────────────────────────────────────────────────────────────────────────────
// 0x0041f000  TimerSlotDataCopy  void(int slot, int* dst)
//
// Copies 6 consecutive dwords (24 bytes) from the per-slot source array
// at 0x0063dc10 + slot*0x2ac into the caller-supplied buffer *dst.
// Loop: iVar1 from 6 downto 0 (7 iters but writes 6 dwords — Ghidra loop is
// iVar1 = 6; iVar1 != 0; --iVar1; *(dst++) = *(src++)).
// ─────────────────────────────────────────────────────────────────────────────
static constexpr std::uintptr_t kTimerSlotDataBase = 0x0063dc10;
// Same stride 0x2ac reused (both arrays share stride within the same slot block).

extern "C" __declspec(dllexport) void __cdecl TimerSlotDataCopy(int slot, int* dst) {
    const auto* src = reinterpret_cast<const std::uint32_t*>(
        kTimerSlotDataBase + static_cast<std::uint32_t>(slot) * kTimerSlotStride);
    auto* out = reinterpret_cast<std::uint32_t*>(dst);
    for (int i = 6; i != 0; --i) {
        *out++ = *src++;
    }
}

RH_ScopedInstall(TimerSlotDataCopy, 0x0041f000);

// ─────────────────────────────────────────────────────────────────────────────
// 0x00420d40  TimerStructArrayClear  void(void)
//
// Iterates 6 elements of a 0x24-stride array starting at ptr=0x0063e4c4:
//   per element:
//     FUN_004b6520(ptr-0xc, 8)   — zero 8 bytes at ptr[-3]
//     *(ptr-1) = 0               — dword at ptr-4
//     *ptr = 0                   — dword at ptr
//     *(ptr+1) = 0               — dword at ptr+4
//     *(ptr+2) = 0               — dword at ptr+8
//   ptr += 0x24
// Loop bound: ptr < 0x0063e554.
//
// FUN_004b6520 is a zero-fill helper (ptr, count_bytes). We use memset here.
// Note: name is TimerStructArrayClear to avoid collision with existing TimerArrayClear
// (0x00422b30) in Frontend/TimerReset.cpp.
// ─────────────────────────────────────────────────────────────────────────────
static constexpr std::uintptr_t kTimerArrayBase   = 0x0063e4c4;  // puVar1 start
static constexpr std::uintptr_t kTimerArrayBound  = 0x0063e554;  // exclusive upper bound
static constexpr std::uint32_t  kTimerArrayStride = 0x00000024;  // 36 bytes per element

extern "C" __declspec(dllexport) void __cdecl TimerStructArrayClear() {
    for (auto* p = reinterpret_cast<std::uint32_t*>(kTimerArrayBase);
         reinterpret_cast<std::uintptr_t>(p) < kTimerArrayBound;
         p = reinterpret_cast<std::uint32_t*>(
             reinterpret_cast<std::uintptr_t>(p) + kTimerArrayStride)) {
        // FUN_004b6520(p - 3, 8): zero 8 bytes at p[-3] (i.e. the 2 dwords at ptr-0xc)
        std::memset(reinterpret_cast<std::uint8_t*>(p) - 0xc, 0, 8);
        // Manual zero writes: p[-1], p[0], p[1], p[2]
        p[-1] = 0u;
        p[0]  = 0u;
        p[1]  = 0u;
        p[2]  = 0u;
    }
}

RH_ScopedInstall(TimerStructArrayClear, 0x00420d40);

// ─────────────────────────────────────────────────────────────────────────────
// 0x00422120  TimerInitLoop  void(void)
//
// Iterates ~4 elements of a 0x208-stride array starting at 0x0063fb90,
// calling FUN_00421c50 each pass (no explicit args visible in decomp):
//   ptr from 0x0063fb90; while ptr < 0x006403b0; ptr += 0x208.
// Iteration count: (0x006403b0 - 0x0063fb90) / 0x208 = 4.
//
// FUN_00421c50 accesses the same global range implicitly (no ptr arg in decomp).
// The callee is classified C2 (drift-promote: C1/unclassified in hooks.csv).
// We call it directly by RVA here.
// ─────────────────────────────────────────────────────────────────────────────
static constexpr std::uintptr_t kTimerInitLoopBase   = 0x0063fb90;
static constexpr std::uintptr_t kTimerInitLoopBound  = 0x006403b0;
static constexpr std::uint32_t  kTimerInitLoopStride = 0x00000208;

// FUN_00421c50 — callee, no visible args in decomp; declared as void(void) cdecl.
// Drift-promote: not yet in hooks.csv; callee-gate permits proceed (≤2 C1/unclassified).
typedef void (__cdecl* TimerInitCallee_t)();
static constexpr std::uintptr_t kTimerInitCallee_RVA = 0x00421c50;

extern "C" __declspec(dllexport) void __cdecl TimerInitLoop() {
    auto callee = reinterpret_cast<TimerInitCallee_t>(kTimerInitCallee_RVA);
    for (std::uintptr_t ptr = kTimerInitLoopBase;
         ptr < kTimerInitLoopBound;
         ptr += kTimerInitLoopStride) {
        callee();
    }
}

RH_ScopedInstall(TimerInitLoop, 0x00422120);

// ─────────────────────────────────────────────────────────────────────────────
// 0x004222c0  TimerInitThunk  void(void)
//
// Confirmed thunk of FUN_00422120 (TimerInitLoop). 4-byte body: unconditional
// branch to 0x00422120. Reimplemented as a direct tail-call.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl TimerInitThunk() {
    TimerInitLoop();
}

RH_ScopedInstall(TimerInitThunk, 0x004222c0);

// ─────────────────────────────────────────────────────────────────────────────
// 0x00422b10  TimerDwordClear  void(void)
//
// Zeroes 0x138 consecutive dwords (0x4e0 = 1248 bytes) starting at 0x008994c0.
// Loop: iVar1 from 0x138 downto 0, decrementing pointer from
//       0x008994c0 + 0x138*4 backwards.
// We implement as forward memset (equivalent result).
// ─────────────────────────────────────────────────────────────────────────────
static constexpr std::uintptr_t kTimerDwordClearBase  = 0x008994c0;
static constexpr std::size_t    kTimerDwordClearBytes = 0x138 * sizeof(std::uint32_t);  // 0x4e0 = 1248

extern "C" __declspec(dllexport) void __cdecl TimerDwordClear() {
    std::memset(reinterpret_cast<void*>(kTimerDwordClearBase), 0, kTimerDwordClearBytes);
}

RH_ScopedInstall(TimerDwordClear, 0x00422b10);

// ─────────────────────────────────────────────────────────────────────────────
// 0x00425b10  TimerGlobalZero  void(void)
//
// Writes 0 to 8 globals at stride 0x4c apart, starting at 0x008992a0.
// Addresses (from Ghidra decomp):
//   0x008992a0, 0x008992ec, 0x00899338, 0x00899384,
//   0x008993d0, 0x0089941c, 0x00899468, 0x008994b4
// ─────────────────────────────────────────────────────────────────────────────
static constexpr std::uintptr_t kTimerGlobalZeroBase   = 0x008992a0;
static constexpr std::uint32_t  kTimerGlobalZeroStride = 0x0000004c;
static constexpr int            kTimerGlobalZeroCount  = 8;

extern "C" __declspec(dllexport) void __cdecl TimerGlobalZero() {
    for (int i = 0; i < kTimerGlobalZeroCount; ++i) {
        auto* field = reinterpret_cast<std::uint32_t*>(
            kTimerGlobalZeroBase + static_cast<std::uint32_t>(i) * kTimerGlobalZeroStride);
        *field = 0u;
    }
}

RH_ScopedInstall(TimerGlobalZero, 0x00425b10);
