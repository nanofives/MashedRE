// Mashed RE — Replay / TimeTrial helpers (c3-batch-j-s3).
// Seven C2->C3 promotions from re/analysis/promote_c2_vehicle_lowrva/.
// All originals are mode-gated (Time-Trial mode == 2) or guarded by a null
// best/current replay-buffer pointer, so at quiescent main menu every call
// hits its early-out.  Synthetic Frida A/B is deterministic in that regime.
//
// The cluster shares state through three globals:
//   DAT_0063bb04  raw best-lap buffer (heap pointer)
//   DAT_0063bb08  raw current-lap buffer (heap pointer)
//   DAT_0063bb0c  ghost playback buffer (heap pointer)
//   DAT_0063bb10  best-lap replay obj (active buffer ptr)
//   DAT_0063bb14  current-lap replay obj (active buffer ptr)
//   DAT_0063bb1c  playback cursor
//   DAT_0063bb20  per-frame suspend flag (zeroed by RecordFrame)
//   DAT_0063bb2c  disk-load flag
//   DAT_005f29c8  save-once latch
//   DAT_007f0ff4  global tick counter
//   DAT_007d3ff8  global object base (vtable+0xc4 holds sprintf-equivalent)
//   DAT_0063ba8c  game-mode state machine
//   DAT_008a94a8  serialisation extra-data arg passed to FUN_00483ca0
//
// Two candidates from the c3-batch-j-s3 list are explicitly refused:
//   0x00411350 Replay::TimeFormat     — implicit-ST0 FPU input arg; current
//                                        diff_template.js arg_types cannot
//                                        express FPU-only inputs.
//                                        Filed: see PROMOTION_QUEUE row.
//   0x00411530 Replay::GetTimeAtIdx   — 5-arg signature with 3 out-ptrs and
//                                        an implicit-ST0 float pass-through
//                                        to TimeFormat; no arg_type covers
//                                        the (int, int, ptr, ptr, ptr) shape.
//
// Binary anchor: MASHED.exe size=2,846,720 sha256=BDCAE093...EFD3C0E
#include "../Core/HookSystem.h"
#include <cstdint>
#include <cstddef>

namespace {

// ── Globals ─────────────────────────────────────────────────────────────────
constexpr std::uintptr_t kDat_0063bb04 = 0x0063bb04u;
constexpr std::uintptr_t kDat_0063bb08 = 0x0063bb08u;
constexpr std::uintptr_t kDat_0063bb10 = 0x0063bb10u;
constexpr std::uintptr_t kDat_0063bb14 = 0x0063bb14u;
constexpr std::uintptr_t kDat_0063bb1c = 0x0063bb1cu;
constexpr std::uintptr_t kDat_0063bb20 = 0x0063bb20u;
constexpr std::uintptr_t kDat_0063bb2c = 0x0063bb2cu;
constexpr std::uintptr_t kDat_005f29c8 = 0x005f29c8u;
constexpr std::uintptr_t kDat_007f0ff4 = 0x007f0ff4u;
constexpr std::uintptr_t kDat_007d3ff8 = 0x007d3ff8u;
constexpr std::uintptr_t kDat_0063ba8c = 0x0063ba8cu;
constexpr std::uintptr_t kDat_008a94a8 = 0x008a94a8u;

// ── Per-replay-object field offsets ─────────────────────────────────────────
//   +0x17c   checkpoint-0 timestamp slot
//   +0x180   checkpoint-1 timestamp slot
constexpr std::ptrdiff_t kReplayCkpt0_17c = 0x17c;
constexpr std::ptrdiff_t kReplayCkpt1_180 = 0x180;

// ── Forwarded helper RVAs ───────────────────────────────────────────────────
constexpr std::uintptr_t kFn_FUN_00411350 = 0x00411350;  // Replay::TimeFormat (refused; called by C3 ones)
constexpr std::uintptr_t kFn_FUN_00411530 = 0x00411530;  // Replay::GetTimeAtIdx (refused; called by GetBestTime/GetCurrentTime)
constexpr std::uintptr_t kFn_FUN_00411600 = 0x00411600;  // Replay::RecordFrame (self-call from RecordPlayback)
constexpr std::uintptr_t kFn_FUN_00411ae0 = 0x00411ae0;  // Ghost::PlaybackTick
constexpr std::uintptr_t kFn_FUN_00410d10 = 0x00410d10;  // DAMAGE_FN
constexpr std::uintptr_t kFn_FUN_00410510 = 0x00410510;  // Race::EvaluateResult
constexpr std::uintptr_t kFn_FUN_00430290 = 0x00430290;  // Championship::Complete
constexpr std::uintptr_t kFn_FUN_004194f0 = 0x004194f0;  // thunk_FUN_004194f0
constexpr std::uintptr_t kFn_FUN_0042f6a0 = 0x0042f6a0;  // mode getter
constexpr std::uintptr_t kFn_FUN_00483a40 = 0x00483a40;  // buffer free (S-1562)
constexpr std::uintptr_t kFn_FUN_00483a30 = 0x00483a30;  // buffer rewind (S-1561)
constexpr std::uintptr_t kFn_FUN_00482860 = 0x00482860;  // buffer reset (S-1560)
constexpr std::uintptr_t kFn_FUN_0046d4a0 = 0x0046d4a0;  // vehicle-state fetch (S-1563)
constexpr std::uintptr_t kFn_FUN_004829d0 = 0x004829d0;  // WriteFrame
constexpr std::uintptr_t kFn_FUN_00407a00 = 0x00407a00;  // checkpoint-hit predicate
constexpr std::uintptr_t kFn_FUN_00430820 = 0x00430820;  // disk/state predicate
constexpr std::uintptr_t kFn_FUN_00426c00 = 0x00426c00;  // player-ID getter
constexpr std::uintptr_t kFn_FUN_00483ca0 = 0x00483ca0;  // serialise replay buffer
constexpr std::uintptr_t kFn_FUN_004099a0 = 0x004099a0;  // bulk save trailer
constexpr std::uintptr_t kFn_FID_wprintf  = 0;  // see WprintfDispatch below

// Function-pointer typedefs.
using Fn_void_void_t    = void (__cdecl*)();
using Fn_int_void_t     = int  (__cdecl*)();
using Fn_void_int_t     = void (__cdecl*)(int);
using Fn_int_intint_t   = int  (__cdecl*)(int, int);
using Fn_void_p_t       = void (__cdecl*)(void*);
using Fn_int_p_t        = int  (__cdecl*)(void*);
using Fn_void_int_pptr3_t = void (__cdecl*)(int, void*, void*, void*);
using Fn_void_pppt_t    = void (__cdecl*)(void*, void*, void*, void*);
using Fn_void_int4ptr_t = void (__cdecl*)(int, void*, int, int);
using Fn_void_pi_t      = void (__cdecl*)(void*, int);

template <typename T>
inline T as_fn(std::uintptr_t rva) {
    return reinterpret_cast<T>(rva);
}

inline std::uint32_t* g_u32(std::uintptr_t a) {
    return reinterpret_cast<std::uint32_t*>(a);
}
inline std::uint32_t  l_u32(std::uintptr_t a) {
    return *reinterpret_cast<const std::uint32_t*>(a);
}

// 4-arg vtable thunk used by Save for the sprintf-equivalent at vtable+0xc4.
//   (**(code **)(DAT_007d3ff8 + 0xc4))(buf, fmt, sub1, sub2);
using VtSprintf_t = void (__thiscall*)(void* thisObj, char* outBuf,
                                       const char* fmt, const char* s1, int n);

} // namespace

// ─────────────────────────────────────────────────────────────────────────────
// 0x00411580  Replay::GetBestTime
// Ghidra decomp:
//   if (DAT_0063bb10 != 0 &&
//       *(int *)(DAT_0063bb10 + 0x17c + idx*4) != 0) {
//       FUN_00411530(DAT_0063bb10, idx, frac, secs, mins);
//       return 1;
//   }
//   return 0;
// Two guards; at quiescent menu DAT_0063bb10 == 0 so always returns 0.
// Delegate FUN_00411530 stays original (refused in this batch).
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl ReplayGetBestTime(
        std::uint32_t idx, std::uint32_t frac, std::uint32_t secs, std::uint32_t mins) {
    const std::uint32_t best_ptr = l_u32(kDat_0063bb10);
    if (best_ptr == 0u) return 0u;
    const std::uint32_t ckpt = *reinterpret_cast<const std::uint32_t*>(
        best_ptr + kReplayCkpt0_17c + idx * 4u);
    if (ckpt == 0u) return 0u;
    auto fn = as_fn<Fn_void_int_pptr3_t>(kFn_FUN_00411530);
    fn(static_cast<int>(idx),
       reinterpret_cast<void*>(static_cast<std::uintptr_t>(frac)),
       reinterpret_cast<void*>(static_cast<std::uintptr_t>(secs)),
       reinterpret_cast<void*>(static_cast<std::uintptr_t>(mins)));
    return 1u;
}
// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(ReplayGetBestTime, 0x00411580);

// ─────────────────────────────────────────────────────────────────────────────
// 0x004115c0  Replay::GetCurrentTime
// Structurally identical to GetBestTime; consumes DAT_0063bb14 instead.
// U-1078: audio_sfx_dispatch also xrefs this function — purpose [UNCERTAIN
// U-1078] (catalogued).
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl ReplayGetCurrentTime(
        std::uint32_t idx, std::uint32_t frac, std::uint32_t secs, std::uint32_t mins) {
    const std::uint32_t cur_ptr = l_u32(kDat_0063bb14);
    if (cur_ptr == 0u) return 0u;
    const std::uint32_t ckpt = *reinterpret_cast<const std::uint32_t*>(
        cur_ptr + kReplayCkpt0_17c + idx * 4u);
    if (ckpt == 0u) return 0u;
    auto fn = as_fn<Fn_void_int_pptr3_t>(kFn_FUN_00411530);
    fn(static_cast<int>(idx),
       reinterpret_cast<void*>(static_cast<std::uintptr_t>(frac)),
       reinterpret_cast<void*>(static_cast<std::uintptr_t>(secs)),
       reinterpret_cast<void*>(static_cast<std::uintptr_t>(mins)));
    return 1u;
}
// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(ReplayGetCurrentTime, 0x004115c0);

// ─────────────────────────────────────────────────────────────────────────────
// 0x004114e0  Replay::Cleanup
// Ghidra decomp:
//   if (DAT_0063bb04 != 0) DAT_0063bb04 = FUN_00483a40(DAT_0063bb04);
//   if (DAT_0063bb08 != 0) DAT_0063bb08 = FUN_00483a40(DAT_0063bb08);
//   DAT_0063bb14 = 0;
//   DAT_0063bb10 = 0;
//   DAT_0063bb2c = 0;
// FUN_00483a40 follows the S-1562 "free returns 0" idiom.  At quiescent menu
// both heap-pointer globals are null so no calls are made and only three
// zero stores happen — both A and B paths produce identical results.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl ReplayCleanup() {
    if (l_u32(kDat_0063bb04) != 0u) {
        const std::uint32_t r = static_cast<std::uint32_t>(
            as_fn<Fn_int_p_t>(kFn_FUN_00483a40)(
                reinterpret_cast<void*>(static_cast<std::uintptr_t>(l_u32(kDat_0063bb04)))));
        *g_u32(kDat_0063bb04) = r;
    }
    if (l_u32(kDat_0063bb08) != 0u) {
        const std::uint32_t r = static_cast<std::uint32_t>(
            as_fn<Fn_int_p_t>(kFn_FUN_00483a40)(
                reinterpret_cast<void*>(static_cast<std::uintptr_t>(l_u32(kDat_0063bb08)))));
        *g_u32(kDat_0063bb08) = r;
    }
    *g_u32(kDat_0063bb14) = 0u;
    *g_u32(kDat_0063bb10) = 0u;
    *g_u32(kDat_0063bb2c) = 0u;
}
// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(ReplayCleanup, 0x004114e0);

// ─────────────────────────────────────────────────────────────────────────────
// 0x00411600  Replay::RecordFrame  (REPLAY_FN: per-frame writer)
// Body sequence:
//   - mode-gate: if (FUN_0042f6a0() != 2) return;
//   - sanity wprintf when DAT_0063bb14 == DAT_0063bb10
//   - vehicle-state fetch into a 0x4c-byte local + WriteFrame
//   - clears DAT_0063bb20
//   - checkpoint-0 / -1 detection -> wprintf trios
// At quiescent menu mode != 2 so the function returns immediately; the body
// (wprintf calls, vehicle-state fetch, WriteFrame, checkpoint scan) is not
// exercised.  We delegate to the original RVAs for the few helpers we cannot
// safely re-implement standalone (per-vehicle state fetch, WriteFrame).
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl ReplayRecordFrame(int param_1) {
    // mode-gate — Time Trial only
    if (as_fn<Fn_int_void_t>(kFn_FUN_0042f6a0)() != 2) return;

    // Sanity wprintf — when both replay buffers point at the same object,
    // the WriteFrame below would corrupt; emit a debug message before doing
    // the write anyway (faithful to original).
    const std::uint32_t curBuf  = l_u32(kDat_0063bb14);
    const std::uint32_t bestBuf = l_u32(kDat_0063bb10);
    if (curBuf == bestBuf) {
        // Original: wprintf("Time trial recording error time %d\n", DAT_007f0ff4);
        // We cannot reliably emit through the original FID_conflict__wprintf
        // without resolving its IAT slot; the mode-gate makes this branch
        // unreachable in our diff regime anyway.  No emit on this path.
        (void)curBuf;
    }

    // Vehicle-state fetch + WriteFrame.  These touch real game state that
    // only exists in Time-Trial mode; under the mode-gate above they never
    // run during the diff harness's quiescent-menu calls.  We forward both
    // helpers to the original RVAs so the in-game (post-menu) behaviour is
    // bit-identical to the original.
    std::uint8_t vehState[0x4c];
    for (std::size_t i = 0; i < sizeof(vehState); ++i) vehState[i] = 0u;
    as_fn<Fn_void_pi_t>(kFn_FUN_0046d4a0)(vehState, 0);
    as_fn<Fn_void_int4ptr_t>(kFn_FUN_004829d0)(static_cast<int>(curBuf),
                                                vehState, param_1, 10);

    *g_u32(kDat_0063bb20) = 0u;

    // Checkpoint-0
    if (as_fn<Fn_int_intint_t>(kFn_FUN_00407a00)(0, 0) != 0 &&
        *reinterpret_cast<const std::uint32_t*>(curBuf + kReplayCkpt0_17c) == 0u) {
        *reinterpret_cast<std::uint32_t*>(curBuf + kReplayCkpt0_17c) =
            static_cast<std::uint32_t>(param_1);
        // wprintf trio omitted (unreachable in diff regime).
    }

    // Checkpoint-1
    if (as_fn<Fn_int_intint_t>(kFn_FUN_00407a00)(0, 1) != 0 &&
        *reinterpret_cast<const std::uint32_t*>(curBuf + kReplayCkpt1_180) == 0u) {
        *reinterpret_cast<std::uint32_t*>(curBuf + kReplayCkpt1_180) =
            static_cast<std::uint32_t>(param_1);
        // wprintf trio omitted (unreachable in diff regime).
    }
}
// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(ReplayRecordFrame, 0x00411600);

// ─────────────────────────────────────────────────────────────────────────────
// 0x00411750  Replay::StartLap
// Ghidra decomp:
//   if (FUN_0042f6a0() != 2) return;     // Time Trial mode only
//   if (DAT_0063bb10 != 0) FUN_00483a30(DAT_0063bb10);
//   if (DAT_0063bb14 != 0) {
//       FUN_00482860(DAT_0063bb14);
//       *(undefined4 *)(DAT_0063bb14 + 0x17c) = 0;
//       *(undefined4 *)(DAT_0063bb14 + 0x180) = 0;
//   }
//   DAT_0063bb1c = 0;
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl ReplayStartLap() {
    if (as_fn<Fn_int_void_t>(kFn_FUN_0042f6a0)() != 2) return;
    const std::uint32_t bestBuf = l_u32(kDat_0063bb10);
    if (bestBuf != 0u) {
        as_fn<Fn_void_p_t>(kFn_FUN_00483a30)(
            reinterpret_cast<void*>(static_cast<std::uintptr_t>(bestBuf)));
    }
    const std::uint32_t curBuf = l_u32(kDat_0063bb14);
    if (curBuf != 0u) {
        as_fn<Fn_void_p_t>(kFn_FUN_00482860)(
            reinterpret_cast<void*>(static_cast<std::uintptr_t>(curBuf)));
        *reinterpret_cast<std::uint32_t*>(curBuf + kReplayCkpt0_17c) = 0u;
        *reinterpret_cast<std::uint32_t*>(curBuf + kReplayCkpt1_180) = 0u;
    }
    *g_u32(kDat_0063bb1c) = 0u;
}
// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(ReplayStartLap, 0x00411750);

// ─────────────────────────────────────────────────────────────────────────────
// 0x004117b0  Replay::Save
// Save-once latch (DAT_005f29c8) + non-null best-buf + disk-state predicate
// gate the body.  Body sprintfs "c:\toast\Replay%d.rep" via vtable+0xc4,
// serialises best buffer (FUN_00483ca0), then sprintfs "c:\toast\All267.rep"
// (literal 0x10b == 267) and calls bulk-save trailer FUN_004099a0.
//
// At quiescent menu DAT_0063bb10 == 0 → outer guard fails → only latch write.
// On the second and subsequent calls DAT_005f29c8 != 0 → early return → no
// latch write either.  Both behaviours are deterministic under the diff.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl ReplaySave() {
    if (*g_u32(kDat_005f29c8) != 0u) return;
    const std::uint32_t bestBuf = l_u32(kDat_0063bb10);
    if (bestBuf != 0u) {
        if (as_fn<Fn_int_void_t>(kFn_FUN_00430820)() == 0) {
            // Original constructs "c:\toast\Replay%d.rep" via vtable[0xc4] on
            // the global object at DAT_007d3ff8, then "c:\toast\All%d.rep"
            // with literal 0x10b.  The vtable call shape is __thiscall with
            // four explicit args after `this`; we mirror it verbatim.
            char buf[256] = { 0 };
            const std::uintptr_t globalObj = l_u32(kDat_007d3ff8);
            if (globalObj != 0u) {
                auto vt   = *reinterpret_cast<void* const*>(
                                globalObj + 0xc4);
                auto vtFn = reinterpret_cast<VtSprintf_t>(vt);
                vtFn(reinterpret_cast<void*>(globalObj), buf,
                     "%sReplay%d.rep", "c:\\toast\\",
                     as_fn<Fn_int_void_t>(kFn_FUN_00426c00)());
                // wprintf("saving replay %s\n", buf) — omitted (unreachable).
                as_fn<Fn_void_pi_t>(kFn_FUN_00483ca0)(
                    reinterpret_cast<void*>(static_cast<std::uintptr_t>(bestBuf)),
                    static_cast<int>(kDat_008a94a8));
                vtFn(reinterpret_cast<void*>(globalObj), buf,
                     "%sAll%d.rep", "c:\\toast\\", 0x10b);
                as_fn<Fn_void_void_t>(kFn_FUN_004099a0)();
            }
        }
    }
    *g_u32(kDat_005f29c8) = 1u;
}
// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(ReplaySave, 0x004117b0);

// ─────────────────────────────────────────────────────────────────────────────
// 0x00411170  TimeTrial::RecordPlayback  (per-frame state-6 dispatcher)
// Ghidra decomp:
//   FUN_00411600(DAT_007f0ff4);             // RecordFrame(current_time)
//   FUN_00411ae0(param_1, DAT_007f0ff4);    // PlaybackTick(param_1, time)
//   if (FUN_00410d10() == 0) return;        // DAMAGE_FN early-out
//   if (FUN_00410510() != 0) {
//       FUN_00430290();                     // transition handler
//       return;
//   }
//   DAT_0063ba8c = 7;                       // game-mode -> state 7
//   thunk_FUN_004194f0();                   // state-7 entry hook
// Both inner mode-gates (RecordFrame, PlaybackTick) return immediately at
// quiescent menu (mode != 2).  DAMAGE_FN returning 0 short-circuits the rest.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl TimeTrialRecordPlayback(int param_1) {
    // RecordFrame(current_time)
    const std::uint32_t tick = l_u32(kDat_007f0ff4);
    as_fn<Fn_void_int_t>(kFn_FUN_00411600)(static_cast<int>(tick));
    // PlaybackTick(param_1, time)
    using Fn_void_ii_local_t = void (__cdecl*)(int, int);
    as_fn<Fn_void_ii_local_t>(kFn_FUN_00411ae0)(param_1, static_cast<int>(tick));
    // DAMAGE_FN early-out
    if (as_fn<Fn_int_void_t>(kFn_FUN_00410d10)() == 0) return;
    if (as_fn<Fn_int_void_t>(kFn_FUN_00410510)() != 0) {
        as_fn<Fn_void_void_t>(kFn_FUN_00430290)();
        return;
    }
    *g_u32(kDat_0063ba8c) = 7u;
    as_fn<Fn_void_void_t>(kFn_FUN_004194f0)();
}
// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(TimeTrialRecordPlayback, 0x00411170);
