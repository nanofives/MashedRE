// Mashed RE — util-subsystem C2->C3 cluster (c3-batch-h, session 5).
//
// Eight new pure-leaf / near-leaf util functions from the util_c0_promote
// and timer_d3_cont1_b clusters. Each is ≤ 53 bytes; all writes/reads cite
// the RVA from the analysis note frontmatter.
//
// Candidates landed in this file:
//   0x0042c2f0  SetDat0067ecb8       — game_state_d2  9b setter
//   0x004098b0  LoadingState1Enter   — util_c0_promote  27b 4-global setter
//   0x00409930  LoadingState3Enter   — util_c0_promote  30b 3-global setter
//   0x00409900  LoadingState2Enter   — util_c0_promote  43b wprintf + 3-global setter
//   0x00426c10  TimerDispatch10      — timer_d3_cont1_b  27b conditional dispatch
//   0x00426c30  TimerDispatch30      — timer_d3_cont1_b  22b conditional dispatch
//   0x00426c70  TimerDispatch70      — timer_d3_cont1_b  22b conditional dispatch
//   0x0041cb80  TimerArrayInit46     — timer_d3_cont1_b  53b 46-element init loop
//
// Candidates NOT in this file (already implemented in TimerState.cpp /
// TimerInit.cpp — drift-fix only via re-classify):
//   0x00413f90  TimerGetBasePtr       (TimerState.cpp)
//   0x00422120  TimerInitLoop         (TimerInit.cpp)
//
// Candidates skipped (refused — see PROMOTION_QUEUE note):
//   0x004af31a  — CRT init orphan, no Ghidra function entity, not hookable
//                  via RH_ScopedInstall.
//   0x0041f1e0  — event matrix copy reads game-state-dependent table at
//                  0x0063d9e0 via FUN_004c0ed0; safe diff vector authoring
//                  would require slot-table prep beyond current harness.
#include "../Core/HookSystem.h"

#include <cstdint>

// MATCH-S07: the body of 0x0042c2f0 (SetDat0067ecb8) moved to Match/MatchS07_0042c2f0.cpp,
// which compiles the byte-matched source re/match/s07/0042c2f0.c.

// ─────────────────────────────────────────────────────────────────────────────
// 0x004098b0  LoadingState1Enter  void __cdecl(void)
//
// Pure 4-global setter (no callees, no branches).
//
// Writes (from re/analysis/util_c0_promote/0x004098b0.md):
//   DAT_008a9584 = 1   (loading-state enum)
//   DAT_008a9588 = 1   (state-1 enable flag)
//   DAT_008a95b0 = 0   (counter/flag cleared)
//   DAT_008a95ac = 0   (counter/flag cleared)
//
// Sole caller: FUN_0043dfd0 (main loading dispatcher, 10969 bytes).
// 27-byte body.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl LoadingState1Enter() {
    *reinterpret_cast<std::uint32_t*>(0x008a9584) = 1u;
    *reinterpret_cast<std::uint32_t*>(0x008a9588) = 1u;
    *reinterpret_cast<std::uint32_t*>(0x008a95b0) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008a95ac) = 0u;
}

RH_ScopedInstall(LoadingState1Enter, 0x004098b0);  // re-enabled 2026-05-24 batch-util

// ─────────────────────────────────────────────────────────────────────────────
// 0x00409930  LoadingState3Enter  void __cdecl(void)
//
// Pure 3-global setter (no callees, no branches).
//
// Writes (from re/analysis/util_c0_promote/0x00409930.md):
//   DAT_008a9584 = 3   (loading-state enum)
//   DAT_008a9590 = 1   (state-3 enable flag)
//   DAT_008a95b0 = 0   (counter/flag cleared)
//
// Callers: FUN_0043dfd0 and FUN_0040ab40.
// 30-byte body.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl LoadingState3Enter() {
    *reinterpret_cast<std::uint32_t*>(0x008a9584) = 3u;
    *reinterpret_cast<std::uint32_t*>(0x008a9590) = 1u;
    *reinterpret_cast<std::uint32_t*>(0x008a95b0) = 0u;
}

RH_ScopedInstall(LoadingState3Enter, 0x00409930);  // re-enabled 2026-05-24 batch-util

// ─────────────────────────────────────────────────────────────────────────────
// 0x00409900  LoadingState2Enter  void __cdecl(void)
//
// State-setter for loading state 2. Emits a debug printf then writes 3 globals.
//
// Verified asm bytes (from MASHED.exe at 0x00409900..0x0040992a):
//   68 28 CB 5C 00          PUSH 0x005ccb28              ; "Load start\n" (ASCII)
//   E8 B3 93 09 00          CALL 0x00442cbd              ; printf (statically linked)
//   83 C4 04                ADD  ESP, 4
//   C7 05 84 95 8A 00 02 00 00 00   MOV [0x008a9584], 2
//   C7 05 8C 95 8A 00 01 00 00 00   MOV [0x008a958c], 1
//   C7 05 B0 95 8A 00 00 00 00 00   MOV [0x008a95b0], 0
//   C3                              RET
//
// Note: re/analysis/util_c0_promote/0x00409900.md describes the callee as
// "FID_conflict__wprintf" but the string bytes at 0x005ccb28 are ASCII
// ("Load start\n"), and the call is a direct CALL (not an IAT indirect),
// so the actual callee is printf (statically linked CRT at 0x00442cbd).
// The Ghidra FID match was wrong; the byte-level analysis is correct.
//
// Sole caller: FUN_0043dfd0.  43-byte body.
//
// We trampoline to the original printf entry to preserve identical
// stdout side-effects without depending on which CRT is linked in our DLL.
// ─────────────────────────────────────────────────────────────────────────────
typedef int (__cdecl* printf_t)(const char*, ...);

// BUGFIX 2026-07-27 — DIGIT TRANSPOSITION. This read `0x00442cbd`; the actual CALL
// target at 0x00409905 is `0x004a2cbd` (`a2` was typed as `42`). The cited call site in
// the comment was correct all along — only the transcribed address was wrong.
// `0x00442cbd` is +0x3d into FUN_00442c80, so calling it executed mid-instruction garbage,
// the same class as the 0x004a3220 crash fixed in 7518a088 (0x00408a70). Ground truth:
//   0x00409900 PUSH 0x5ccb28 / 0x00409905 CALL 0x004a2cbd / 0x0040990a ADD ESP,4
// 0x004a2cbd is the narrow CRT printf (stdout FILE* at 0x00616110) — the same callee
// 0x00408a70 uses. Found by the systematic call-target audit, not by a crash: this hook
// (LoadingState2Enter @ 0x00409900) is on the loading path, which the menu-window
// acceptance runs never exercise.
static constexpr std::uintptr_t kPrintfFn_VA   = 0x004a2cbdu;  // CALL target at 0x00409905
static constexpr std::uintptr_t kLoadStartStr  = 0x005ccb28u;  // PUSH operand at 0x00409901

extern "C" __declspec(dllexport) void __cdecl LoadingState2Enter() {
    const auto printf_fn = reinterpret_cast<printf_t>(kPrintfFn_VA);
    printf_fn(reinterpret_cast<const char*>(kLoadStartStr));
    *reinterpret_cast<std::uint32_t*>(0x008a9584) = 2u;
    *reinterpret_cast<std::uint32_t*>(0x008a958c) = 1u;
    *reinterpret_cast<std::uint32_t*>(0x008a95b0) = 0u;
}

RH_ScopedInstall(LoadingState2Enter, 0x00409900);  // re-enabled 2026-05-24 batch-util

// ─────────────────────────────────────────────────────────────────────────────
// 0x00426c10  TimerDispatch10  void __cdecl(void)
//
// Conditional dispatcher (27 bytes).
// Body (from re/analysis/timer_d3_cont1_b/0x00426c10.md):
//   if (FUN_0041ea40() != 0) {
//       FUN_0041e920(0x00646e58);   // global addr passed as arg
//       FUN_00480100();
//   }
//
// FUN_0041ea40, FUN_0041e920, FUN_00480100 are not yet reversed (C0).
// We call originals via function-pointer trampolines so the chain
// proceeds bit-identically.
//
// Sole caller: FUN_004111c0 (init).
// ─────────────────────────────────────────────────────────────────────────────

// MATCH-S06: the body of 0x00426c10 (TimerDispatch10) moved to Match/MatchS06_00426c10.cpp,
// which compiles the byte-matched source re/match/s06/00426c10.c.

// ─────────────────────────────────────────────────────────────────────────────
// 0x00426c30  TimerDispatch30  void __cdecl(void)
//
// Conditional dispatcher (22 bytes), sibling of 0x00426c10.
//
// Verified asm bytes at 0x00426c30..0x00426c46:
//   E8 FB 7D FF FF          CALL 0x0041ea30           ; gate
//   85 C0                   TEST EAX, EAX
//   74 0D                   JZ   0x00426c46
//   C7 44 24 04 58 6E 64 00 MOV  [ESP+4], 0x00646e58
//   E9 CA 7C FF FF          JMP  0x0041e910           ; tail-call with arg 0x00646e58
//   C3                      RET
//
// Note: re/analysis/timer_d3_cont1_b/0x00426c30.md describes the action as
// "calls FUN_0041e910() with no arguments" — that is INCORRECT. The asm
// writes 0x00646e58 into the first stack arg slot before the tail-JMP, so
// the call is FUN_0041e910(0x00646e58). Sibling 0x00426c70 has the same
// arg-write pattern.
//
// FUN_0041ea30, FUN_0041e910 not yet reversed. Sole caller: FUN_004111c0.
// ─────────────────────────────────────────────────────────────────────────────

// MATCH-S06: the body of 0x00426c30 (TimerDispatch30) moved to Match/MatchS06_00426c30.cpp,
// which compiles the byte-matched source re/match/s06/00426c30.c.

// ─────────────────────────────────────────────────────────────────────────────
// 0x00426c70  TimerDispatch70  void __cdecl(void)
//
// Conditional dispatcher (22 bytes), sibling of 0x00426c10 and 0x00426c30.
//
// Verified asm bytes at 0x00426c70..0x00426c86:
//   E8 DB 7D FF FF          CALL 0x0041ea50           ; gate
//   85 C0                   TEST EAX, EAX
//   74 0D                   JZ   0x00426c86
//   C7 44 24 04 58 6E 64 00 MOV  [ESP+4], 0x00646e58
//   E9 AA 7C FF FF          JMP  0x0041e930           ; tail-call with arg 0x00646e58
//   C3                      RET
//
// Note: re/analysis/timer_d3_cont1_b/0x00426c70.md describes the action as
// "calls FUN_0041e930() with no arguments" — that is INCORRECT (same
// pattern as 0x00426c30). The asm writes 0x00646e58 into the first stack
// arg slot before the tail-JMP.
//
// FUN_0041ea50, FUN_0041e930 not yet reversed. Sole caller: FUN_004111c0.
// ─────────────────────────────────────────────────────────────────────────────

// MATCH-S06: the body of 0x00426c70 (TimerDispatch70) moved to Match/MatchS06_00426c70.cpp,
// which compiles the byte-matched source re/match/s06/00426c70.c.

// 0x0041cb80  TimerArrayInit46 — REFUSED (not bit-identical reachable).
//
// Asm verified at 0x0041cb80..0x0041cbb4. The per-element loop body at
// 0x0041cba0 is `CALL 0x0041c380` with ESI live as the current element
// pointer (ESI loaded at 0x0041cb95). FUN_0041c380 is not yet reversed,
// but the existence of `MOV ESI` immediately before the loop indicates a
// register-passed implicit pointer. A C++ reimpl that calls FUN_0041c380()
// via function-pointer cannot reliably set ESI to the per-element ptr,
// so the side-effects diverge from the original.
//
// Same impedance as 0x00422120 (TimerInitLoop) which uses MOV EAX, ESI
// to pass the pointer in EAX before each callee CALL. Both refused
// until FUN_0041c380 / FUN_00421c50 are reversed and the implicit-pointer
// register can either be eliminated or implemented via inline-asm wrapper.
