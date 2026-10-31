// Mashed RE — timer slot init setters.
// 0x0041d820  FUN_0041d820  timer_d3_cont1_b  C2 (promote_c2_util_timer-20260513)
// 0x0041e130  FUN_0041e130  timer_d3_cont1_b  C2 (promote_c2_util_timer-20260513)
#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>

// MATCH-S04: the body of 0x0041d820 (TimerSlotClear) moved to Match/MatchS04_0041d820.cpp,
// which compiles the byte-matched source re/match/s04/0041d820.c.

// MATCH-S04: the body of 0x0041e130 (TimerTrackSetter) moved to Match/MatchS04_0041e130.cpp,
// which compiles the byte-matched source re/match/s04/0041e130.c.

// MATCH-S04: the body of 0x0041eda0 (TimerBitFieldSet) moved to Match/MatchS04_0041eda0.cpp,
// which compiles the byte-matched source re/match/s04/0041eda0.c.

// MATCH-S04: the body of 0x0041f000 (TimerSlotDataCopy) moved to Match/MatchS04_0041f000.cpp,
// which compiles the byte-matched source re/match/s04/0041f000.c.

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

RH_ScopedInstall(TimerStructArrayClear, 0x00420d40);  // re-enabled 2026-05-24 c3-safe

// ─────────────────────────────────────────────────────────────────────────────
// 0x00422120  TimerInitLoop  void(void)
//
// Iterates 4 elements of a 0x208-stride array starting at 0x0063fb90,
// calling FUN_00421c50 each pass.  ptr from 0x0063fb90 advances by 0x208 per
// iteration while ptr < 0x006403b0.
// Iteration count: (0x006403b0 - 0x0063fb90) / 0x208 = 4.
//
// Exact asm (0x00422120 – 0x0042213c, 29 bytes):
//   0x00422120  PUSH ESI                       ; save callee-saved ESI
//   0x00422121  MOV  ESI, 0x0063fb90           ; loop ptr
//   0x00422126  MOV  EAX, ESI                  ; <-- callee arg via EAX (non-standard)
//   0x00422128  CALL 0x00421c50
//   0x0042212d  ADD  ESI, 0x208
//   0x00422133  CMP  ESI, 0x006403b0
//   0x00422139  JL   0x00422126
//   0x0042213b  POP  ESI
//   0x0042213c  RET
//
// Non-standard ABI for the callee (FUN_00421c50): it takes its per-element
// pointer in EAX, not on the stack.  FUN_00421c50's first executable line is
// MOV ESI, EAX, after which it touches param+0x10, param+0xf0, param+0xf4
// (verified: FUN_00421c10 dereferences at those offsets).
//
// A plain C++ `callee()` cannot set EAX deterministically — under MSVC the
// in-register arg is whatever happens to be in EAX at the call site, which
// is undefined behaviour and causes FUN_00421c50 / FUN_00421c10 to read/write
// garbage memory.  We therefore replicate the original prologue/epilogue
// exactly via __declspec(naked) + inline-asm, matching the original
// instruction stream byte-for-byte.  This is the canonical pattern for
// ESI/EAX-passing callee chains in this codebase; future RVAs that hit the
// same impedance (e.g. 0x0041cb80) should adopt the same approach.
//
// ref: re/analysis/timer_d3_cont1_b/0x00422120.md
// ─────────────────────────────────────────────────────────────────────────────
// 0x00422120
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl TimerInitLoop() {
    __asm {
        push    esi                         // 0x00422120: PUSH ESI
        mov     esi, 0x0063fb90             // 0x00422121: loop base
    timer_init_loop_body:
        mov     eax, esi                    // 0x00422126: arg-in-EAX for FUN_00421c50
        mov     ecx, 0x00421c50             // direct absolute-addr call (orig used
        call    ecx                         //   E8 rel32, equivalent end-state here)
        add     esi, 0x00000208             // 0x0042212d: advance ptr
        cmp     esi, 0x006403b0             // 0x00422133: loop bound (exclusive)
        jl      timer_init_loop_body        // 0x00422139: JL back to MOV EAX,ESI
        pop     esi                         // 0x0042213b: POP ESI
        ret                                 // 0x0042213c: RET
    }
}

RH_ScopedInstall(TimerInitLoop, 0x00422120);  // re-enabled 2026-05-24 c3-safe

// ─────────────────────────────────────────────────────────────────────────────
// 0x004222c0  TimerInitThunk  void(void)
//
// Confirmed thunk of FUN_00422120 (TimerInitLoop). 4-byte body: unconditional
// branch to 0x00422120. Reimplemented as a direct tail-call.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl TimerInitThunk() {
    TimerInitLoop();
}

RH_ScopedInstall(TimerInitThunk, 0x004222c0);  // re-enabled 2026-05-24 phase-a1 write@+0xf0 GREEN

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

RH_ScopedInstall(TimerDwordClear, 0x00422b10);  // re-enabled 2026-05-24 phase-a1 real-GREEN

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

RH_ScopedInstall(TimerGlobalZero, 0x00425b10);  // re-enabled 2026-05-24 phase-a1 real-GREEN

// frida-sweep-20260516-0008: c3-batch-e-s12 added duplicate impls for
// 0x00422b10 (TimerArrayZero), 0x00425b10 (PlayerSlotZero), 0x004222c0
// (TimerInitThunk).  The duplicates have been removed under first-wins
// (TimerDwordClear / TimerGlobalZero / TimerInitThunk above already cover
// those RVAs).  See git history.

// MATCH-S04: the body of 0x0041cbc0 (FloatTableInit) moved to Match/MatchS04_0041cbc0.cpp,
// which compiles the byte-matched source re/match/s04/0041cbc0.c.

