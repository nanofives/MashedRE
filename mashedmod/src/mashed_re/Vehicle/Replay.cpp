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
constexpr std::uintptr_t kFn_FUN_004987b0 = 0x004987b0;  // debug logger, variadic __cdecl (vsprintf -> OutputDebugStringA)
constexpr std::uintptr_t kFn_printf       = 0x004a2cbd;  // narrow CRT printf (stdout FILE* 0x00616110)

// Format strings the original pushes by address (read from MASHED.exe.unpatched .rdata).
constexpr std::uintptr_t kStr_005ccedc = 0x005ccedc;  // "Time trial recording error time %d\n"
constexpr std::uintptr_t kStr_005ccec4 = 0x005ccec4;  // "Hit intermediate 0 %d\n"
constexpr std::uintptr_t kStr_005ccea4 = 0x005ccea4;  // "Hit intermediate 0 %d:%d:%f\n"
constexpr std::uintptr_t kStr_005cce8c = 0x005cce8c;  // "Hit intermediate 1 %d\n"
constexpr std::uintptr_t kStr_005cce6c = 0x005cce6c;  // "Hit intermediate 1 %d:%d:%f\n"
constexpr std::uintptr_t kStr_005ccf14 = 0x005ccf14;  // "%sReplay%d.rep"
constexpr std::uintptr_t kStr_005cce1c = 0x005cce1c;  // "c:\toast\"
constexpr std::uintptr_t kStr_005cce10 = 0x005cce10;  // "%sAll%d.rep"
constexpr std::uintptr_t kStr_005ccf00 = 0x005ccf00;  // "saving replay %s\n"

// Function-pointer typedefs.
using Fn_void_void_t    = void (__cdecl*)();
using Fn_int_void_t     = int  (__cdecl*)();
using Fn_void_int_t     = void (__cdecl*)(int);
using Fn_int_intint_t   = int  (__cdecl*)(int, int);
using Fn_void_p_t       = void (__cdecl*)(void*);
using Fn_int_p_t        = int  (__cdecl*)(void*);
using Fn_void_u32x5_t   = void (__cdecl*)(std::uint32_t, std::uint32_t, std::uint32_t,
                                          std::uint32_t, std::uint32_t);
using Fn_void_u32p_u32_u32_t = void (__cdecl*)(std::uint32_t, std::uint32_t, int, int);
using Fn_int_pp_t       = int  (__cdecl*)(std::uint32_t*, int);
using Fn_void_iffi_t    = void (__cdecl*)(std::uint32_t, float*, int*, int*);
using Fn_void_pi_t      = void (__cdecl*)(void*, int);
using Fn_varargs_t      = int  (__cdecl*)(const char*, ...);
// RwGlobals (DAT_007d3ff8) +0xc4 holds a sprintf-shaped function pointer. It is a plain
// data slot, not a vtable, and the original calls it __cdecl: two calls of 4 pushes each
// are popped together by 0x00411843 add esp,0x30 (with the 0x004987b0 and 0x00483ca0 pushes).
using Fn_rwsprintf_t    = int  (__cdecl*)(char*, const char*, ...);

template <typename T>
inline T as_str(std::uintptr_t a) {
    return reinterpret_cast<T>(a);
}

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
// Disasm 0x00411580..0x004115b9:
//   0x00411580 mov eax,[DAT_0063bb10]; test; je -> return 0
//   0x0041158d mov edx,[eax+ecx*4+0x17c]; test; je -> return 0
//   0x00411598..0x004115a9 push p4, p3, p2, idx, obj; call 0x00411530
//   0x004115ae add esp,0x14 (5 args: the record pointer IS the first argument)
//   0x004115b1 mov eax,1
// Re-port 2026-10-12: the previous body called 0x00411530 with 4 args and no
// record pointer, so the callee read idx as the record and p2 as idx.
// ─────────────────────────────────────────────────────────────────────────────
// 0x00411580
extern "C" __declspec(dllexport) std::uint32_t __cdecl ReplayGetBestTime(
        std::uint32_t idx, std::uint32_t frac, std::uint32_t secs, std::uint32_t mins) {
    const std::uint32_t best_ptr = l_u32(kDat_0063bb10);
    if (best_ptr == 0u) return 0u;
    const std::uint32_t ckpt = *reinterpret_cast<const std::uint32_t*>(
        best_ptr + kReplayCkpt0_17c + idx * 4u);
    if (ckpt == 0u) return 0u;
    as_fn<Fn_void_u32x5_t>(kFn_FUN_00411530)(best_ptr, idx, frac, secs, mins);  // 0x004115a9
    return 1u;
}
RH_ScopedInstall(ReplayGetBestTime, 0x00411580);  // re-enabled 2026-05-24 c3-vehicle

// ─────────────────────────────────────────────────────────────────────────────
// 0x004115c0  Replay::GetCurrentTime
// Structurally identical to GetBestTime; consumes DAT_0063bb14 instead.
// U-1078: audio_sfx_dispatch also xrefs this function — purpose [UNCERTAIN
// U-1078] (catalogued).
// Disasm 0x004115c0..0x004115f9: same shape as 0x00411580 with DAT_0063bb14;
// 0x004115e8 push eax (record), 0x004115e9 call 0x00411530, 0x004115ee add esp,0x14.
// ─────────────────────────────────────────────────────────────────────────────
// 0x004115c0
extern "C" __declspec(dllexport) std::uint32_t __cdecl ReplayGetCurrentTime(
        std::uint32_t idx, std::uint32_t frac, std::uint32_t secs, std::uint32_t mins) {
    const std::uint32_t cur_ptr = l_u32(kDat_0063bb14);
    if (cur_ptr == 0u) return 0u;
    const std::uint32_t ckpt = *reinterpret_cast<const std::uint32_t*>(
        cur_ptr + kReplayCkpt0_17c + idx * 4u);
    if (ckpt == 0u) return 0u;
    as_fn<Fn_void_u32x5_t>(kFn_FUN_00411530)(cur_ptr, idx, frac, secs, mins);  // 0x004115e9
    return 1u;
}
RH_ScopedInstall(ReplayGetCurrentTime, 0x004115c0);  // re-enabled 2026-05-24 c3-vehicle

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
RH_ScopedInstall(ReplayCleanup, 0x004114e0);  // re-enabled 2026-05-24 c3-vehicle

// ─────────────────────────────────────────────────────────────────────────────
// 0x00411600  Replay::RecordFrame  (REPLAY_FN: per-frame writer)
// Re-ported from the bytes 2026-10-12 (disasm 0x00411600..0x0041174d). Frame:
// sub esp,0x10 = four dword locals: L0 float frac, L1 int secs, L2 int mins
// (0x00411350 outs) and L3 (the 0x0046d4a0 out).
//   0x00411603 call 0x0042f6a0; 0x00411608 sub eax,2; jne -> return
//   0x00411611 DAT_0063bb14 == DAT_0063bb10 -> 0x0041162a call 0x004987b0
//              ("Time trial recording error time %d\n" @0x005ccedc, DAT_007f0ff4)
//   0x0041163b call 0x0046d4a0(&L3, 0): stores ONE dword, 0x00881ec8 +
//              idx*0xd04 + DAT_00881f48[idx*0xd04]*0x40 (0x0046d4ac..0x0046d4c6)
//   0x00411653 call 0x004829d0(DAT_0063bb14 (re-read 0x00411648), L3, param_1, 10)
//   0x0041165c DAT_0063bb20 = 0
//   0x00411666 call 0x00407a00(0, 0); nonzero and [DAT_0063bb14+0x17c] == 0 ->
//     0x00411681 [+0x17c] = param_1; 0x0041168d esi = [DAT_0063bb14+0x17c]
//     0x004116a3 call 0x00411350(esi, &L0, &L1, &L2)
//     0x004116ae printf("Hit intermediate 0 %d\n" @0x005ccec4, esi)
//     0x004116cd printf("Hit intermediate 0 %d:%d:%f\n" @0x005ccea4, L2, L1, (double)L0)
//   0x004116d9 call 0x00407a00(0, 1); same with +0x180 and 0x005cce8c / 0x005cce6c.
// printf is the narrow CRT printf 0x004a2cbd, not wprintf. The previous port passed the
// ADDRESS of a zeroed 0x4c-byte local as FUN_004829d0 arg 2 instead of the pointer value
// FUN_0046d4a0 stores there, and dropped the logger call, the 0x00411350 calls and both
// printf pairs.
// ── Globals ─────────────────────────────────────────────────────────────────
// 0x00411600
extern "C" __declspec(dllexport) void __cdecl ReplayRecordFrame(int param_1) {
    if (as_fn<Fn_int_void_t>(kFn_FUN_0042f6a0)() != 2) return;          // 0x00411608

    if (l_u32(kDat_0063bb14) == l_u32(kDat_0063bb10)) {                   // 0x00411616
        as_fn<Fn_varargs_t>(kFn_FUN_004987b0)(as_str<const char*>(kStr_005ccedc),
                                              l_u32(kDat_007f0ff4));      // 0x0041162a
    }

    // L3: 0x0046d4a0 writes it unconditionally for index 0 (0x0046d4a4 cmp eax,0x10; jb).
    std::uint32_t vehPtr = 0u;
    as_fn<Fn_int_pp_t>(kFn_FUN_0046d4a0)(&vehPtr, 0);                     // 0x0041163b
    as_fn<Fn_void_u32p_u32_u32_t>(kFn_FUN_004829d0)(l_u32(kDat_0063bb14), vehPtr,
                                                     param_1, 10);        // 0x00411653
    *g_u32(kDat_0063bb20) = 0u;                                           // 0x0041165c

    struct Ckpt { std::ptrdiff_t off; int k; std::uintptr_t fmt1; std::uintptr_t fmt2; };
    static const Ckpt kCkpt[2] = {
        { kReplayCkpt0_17c, 0, kStr_005ccec4, kStr_005ccea4 },            // 0x00411666..0x004116d2
        { kReplayCkpt1_180, 1, kStr_005cce8c, kStr_005cce6c },            // 0x004116d9..0x00411745
    };
    for (const Ckpt& c : kCkpt) {
        if (as_fn<Fn_int_intint_t>(kFn_FUN_00407a00)(0, c.k) == 0) continue;
        const std::uint32_t rec = l_u32(kDat_0063bb14);
        if (*reinterpret_cast<const std::uint32_t*>(rec + c.off) != 0u) continue;
        *reinterpret_cast<std::uint32_t*>(rec + c.off) = static_cast<std::uint32_t>(param_1);
        const std::uint32_t t = *reinterpret_cast<const std::uint32_t*>(l_u32(kDat_0063bb14) + c.off);
        float frac; int secs; int mins;
        as_fn<Fn_void_iffi_t>(kFn_FUN_00411350)(t, &frac, &secs, &mins);
        as_fn<Fn_varargs_t>(kFn_printf)(as_str<const char*>(c.fmt1), t);
        // fld dword L0 / fstp qword: exact float -> double widening.
        as_fn<Fn_varargs_t>(kFn_printf)(as_str<const char*>(c.fmt2), mins, secs,
                                        static_cast<double>(frac));
    }
}
RH_ScopedInstall(ReplayRecordFrame, 0x00411600);  // re-enabled 2026-05-24 c3-vehicle

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
RH_ScopedInstall(ReplayStartLap, 0x00411750);  // re-enabled 2026-05-24 c3-vehicle

// ─────────────────────────────────────────────────────────────────────────────
// 0x004117b0  Replay::Save
// Re-ported from the bytes 2026-10-12 (disasm 0x004117b0..0x00411865). /GS frame:
// sub esp,0x44, 0x40-byte buffer at [esp], cookie at [esp+0x40].
//   0x004117c0 DAT_005f29c8 != 0 -> return (no latch write)
//   0x004117cd DAT_0063bb10 == 0 -> latch; 0x004117d6 call 0x00430820; != 0 -> latch
//   0x004117e0 esi = DAT_007d3ff8 + 0xc4 (RwGlobals slot address, read BEFORE the getter)
//   0x004117ec call 0x00426c00
//   0x00411801 call [esi](buf, "%sReplay%d.rep" @0x005ccf14, "c:\toast\" @0x005cce1c, id)
//   0x0041180d call 0x004987b0("saving replay %s\n" @0x005ccf00, buf)
//   0x0041181e call 0x00483ca0(DAT_0063bb10 (re-read 0x00411812), 0x008a94a8)
//   0x0041183d call [DAT_007d3ff8 (re-read 0x00411823) + 0xc4]
//              (buf, "%sAll%d.rep" @0x005cce10, "c:\toast\" @0x005cce1c, 0x10b)
//   0x00411843 add esp,0x30  -> the +0xc4 slot is __cdecl (caller pops)
//   0x00411846 call 0x004099a0
//   0x0041184c DAT_005f29c8 = 1
// The previous port called the +0xc4 slot as __thiscall (no caller cleanup: ESP drifted
// 0x20 and the process died at the /GS check), passed its own string literals, guarded
// DAT_007d3ff8 against null (the original does not), and dropped the 0x004987b0 call.
// ── Globals ─────────────────────────────────────────────────────────────────
// 0x004117b0
extern "C" __declspec(dllexport) void __cdecl ReplaySave() {
    if (*g_u32(kDat_005f29c8) != 0u) return;                              // 0x004117c7
    if (l_u32(kDat_0063bb10) != 0u &&                                     // 0x004117d4
        as_fn<Fn_int_void_t>(kFn_FUN_00430820)() == 0) {                  // 0x004117dd
        char buf[0x40];
        const std::uintptr_t slot = l_u32(kDat_007d3ff8) + 0xc4u;         // 0x004117e6
        const int id = as_fn<Fn_int_void_t>(kFn_FUN_00426c00)();          // 0x004117ec
        (*reinterpret_cast<const Fn_rwsprintf_t*>(slot))(
            buf, as_str<const char*>(kStr_005ccf14),
            as_str<const char*>(kStr_005cce1c), id);                      // 0x00411801
        as_fn<Fn_varargs_t>(kFn_FUN_004987b0)(as_str<const char*>(kStr_005ccf00), buf);  // 0x0041180d
        as_fn<Fn_void_pi_t>(kFn_FUN_00483ca0)(
            reinterpret_cast<void*>(static_cast<std::uintptr_t>(l_u32(kDat_0063bb10))),
            static_cast<int>(kDat_008a94a8));                             // 0x0041181e
        (*reinterpret_cast<const Fn_rwsprintf_t*>(l_u32(kDat_007d3ff8) + 0xc4u))(
            buf, as_str<const char*>(kStr_005cce10),
            as_str<const char*>(kStr_005cce1c), 0x10b);                   // 0x0041183d
        as_fn<Fn_void_void_t>(kFn_FUN_004099a0)();                        // 0x00411846
    }
    *g_u32(kDat_005f29c8) = 1u;                                           // 0x0041184c
}
RH_ScopedInstall(ReplaySave, 0x004117b0);  // re-enabled 2026-05-24 c3-vehicle

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
RH_ScopedInstall(TimeTrialRecordPlayback, 0x00411170);  // re-enabled 2026-05-24 c3-vehicle
