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

// MATCH-S05: the body of 0x00420d40 (TimerStructArrayClear) moved to Match/MatchS05_00420d40.cpp,
// which compiles the byte-matched source re/match/s05/00420d40.c.

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

// MATCH-S05: the body of 0x004222c0 (TimerInitThunk) moved to Match/MatchS05_004222c0.cpp,
// which compiles the byte-matched source re/match/s05/004222c0.c.

// MATCH-S05: the body of 0x00422b10 (TimerDwordClear) moved to Match/MatchS05_00422b10.cpp,
// which compiles the byte-matched source re/match/s05/00422b10.c.

// MATCH-S05: the body of 0x00425b10 (TimerGlobalZero) moved to Match/MatchS05_00425b10.cpp,
// which compiles the byte-matched source re/match/s05/00425b10.c.

// frida-sweep-20260516-0008: c3-batch-e-s12 added duplicate impls for
// 0x00422b10 (TimerArrayZero), 0x00425b10 (PlayerSlotZero), 0x004222c0
// (TimerInitThunk).  The duplicates have been removed under first-wins
// (TimerDwordClear / TimerGlobalZero / TimerInitThunk above already cover
// those RVAs).  See git history.

// MATCH-S04: the body of 0x0041cbc0 (FloatTableInit) moved to Match/MatchS04_0041cbc0.cpp,
// which compiles the byte-matched source re/match/s04/0041cbc0.c.

