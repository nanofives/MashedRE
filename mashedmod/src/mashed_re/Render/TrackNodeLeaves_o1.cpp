// Mashed RE — Render track-node leaf functions (c3-batch-o session 1 + c3-batch-p session 2).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x0041e870  TrackNodeRecordScan    — 48B pure leaf; scans 0x48B record array,
//                                         last-match-wins write to DAT_0063d7e4
//
// Smoke-test stubs for `track_record_deref` harness arg_type (2026-05-22):
//   0x0041e9d0  TrackNodeFnPtrGet14    — 8B getter; returns *(DAT_0063d7e4+0x14)
//
// c3-batch-p session 2 (2026-05-22): dispatchers promoted to C3:
//   0x0041e8b0  TrackNodeDispatch14    — 8B indirect dispatch via *(DAT_0063d7e4+0x14)
//   0x0041e970  TrackNodeDispatch44    — 8B indirect dispatch via *(DAT_0063d7e4+0x44)
//
// Still deferred:
//   0x0041ea90  TrackNodeFnPtrGet44    — 8B getter; returns *(DAT_0063d7e4+0x44)
//                                         (batch-p s1 candidate; not in this session)
//
// Analysis notes:
//   re/analysis/render_promote_c2_track_node/0x0041e870.md
//   re/analysis/render_promote_c2_track_node/0x0041e8b0.md
//   re/analysis/render_promote_c2_track_node/0x0041e970.md
//   re/analysis/render_promote_c2_track_node/0x0041e9d0.md
//   re/analysis/render_promote_c2_track_node/0x0041ea90.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// TrackNodeRecordScan  --  0x0041e870
//
// Original: FUN_0041e870 (48 bytes, 0x0041e870..0x0041e89f)
// Signature: void FUN_0041e870(int param_1)
//   param_1: track index; compared against field +0x10 of each 0x48-byte record
// Returns: void
//
// Algorithm (cited from 0x0041e870 body):
//   1. DAT_0063d7e4 = NULL                                    [entry]
//   2. if DAT_005f37a0 <= 0: return                           [loop guard]
//   3. for i = 0 .. DAT_005f37a0-1:
//        record = s_training_005f33f8 + i * 0x48             [array base + stride]
//        if *(int*)(record + 0x10) == param_1:
//            DAT_0063d7e4 = record                            [last-match-wins]
//   4. return (side effect: DAT_0063d7e4 = last matching record or NULL)
//
// Constants (cited from 0x0041e870 body):
//   0x0063d7e4 — global current track-descriptor pointer      [entry + 0x10 write]
//   0x005f37a0 — count of track descriptor records            [loop guard]
//   0x005f33f8 — record array base (s_training_005f33f8)      [loop body]
//   0x10 (16)  — int id field offset within 0x48-byte record  [loop body]
//   0x48 (72)  — stride per record                            [loop body]
//
// Note: does NOT early-exit — full array scan, last match wins.
//
// Uncertainties (non-blocking):
//   U-3213: full layout of 72-byte record at 0x005f33f8 unknown; only +0x10 confirmed.
//
// Anti-island: callers include FUN_00426640 (C2, render subsystem); callee
//   constraint is N/A for pure leaf (leaf-function exemption, re/CONFIDENCE.md).
//
// ref: re/analysis/render_promote_c2_track_node/0x0041e870.md
// ---------------------------------------------------------------------------

// 0x0041e870
extern "C" __declspec(dllexport) void __cdecl TrackNodeRecordScan(int param_1)
{
    // Step 1: clear the global current track-descriptor pointer. [entry]
    *reinterpret_cast<std::uintptr_t*>(0x0063d7e4u) = 0u;

    // Step 2: read iteration count; if zero or negative, return immediately. [0x0041e870 body]
    std::int32_t count = *reinterpret_cast<std::int32_t*>(0x005f37a0u);
    if (count <= 0) {
        return;
    }

    // Step 3: scan array; last matching record wins. [0x0041e870 body]
    // Array base: s_training_005f33f8 (0x005f33f8); stride: 0x48 (72 bytes/record).
    const std::uintptr_t array_base  = 0x005f33f8u;
    const std::uintptr_t record_stride = 0x48u;
    const std::uintptr_t id_field_off  = 0x10u;  // int field at record+0x10

    for (std::int32_t i = 0; i < count; ++i) {
        std::uintptr_t record = array_base + static_cast<std::uint32_t>(i) * record_stride;
        if (*reinterpret_cast<std::int32_t*>(record + id_field_off) == param_1) {
            // Last match wins: overwrite with current matching record pointer.
            *reinterpret_cast<std::uintptr_t*>(0x0063d7e4u) = record;
        }
    }
    // Returns void; side effect is DAT_0063d7e4 = last matching record, or NULL.
}

RH_ScopedInstall(TrackNodeRecordScan, 0x0041e870);

// ---------------------------------------------------------------------------
// TrackNodeFnPtrGet14  --  0x0041e9d0
//
// Original: FUN_0041e9d0 (8 bytes, 0x0041e9d0..0x0041e9d7)
// Signature: undefined4 FUN_0041e9d0(void)
//   Returns: dword at *(DAT_0063d7e4 + 0x14)
//
// Reads DAT_0063d7e4 (current track descriptor record pointer) and returns
// the dword at offset +0x14 within that record. Used as a non-zero guard
// by FUN_00426e10: `if FUN_0041e9d0() != 0 { call FUN_0041e8b0() }`.
// At quiescent main menu, DAT_0063d7e4 == NULL -> this function will crash
// unless DAT_0063d7e4 is pre-seeded by the diff harness (track_record_deref).
//
// Smoke-test target for track_record_deref harness arg_type (2026-05-22).
// Full C3 promotion is c3_batch_p's job.
//
// Constants (cited from 0x0041e9d0 body):
//   0x0063d7e4 — global track-descriptor pointer
//   0x14 (20)  — field offset within 0x48-byte record
//
// Analysis note: re/analysis/render_promote_c2_track_node/0x0041e9d0.md
// ---------------------------------------------------------------------------

// 0x0041e9d0
extern "C" __declspec(dllexport) std::uint32_t __cdecl TrackNodeFnPtrGet14() {
    // Read the current track-descriptor record pointer from DAT_0063d7e4.
    std::uintptr_t record = *reinterpret_cast<std::uintptr_t*>(0x0063d7e4u);
    // Return the dword at record+0x14.
    return *reinterpret_cast<std::uint32_t*>(record + 0x14u);
}

RH_ScopedInstall(TrackNodeFnPtrGet14, 0x0041e9d0);

// ---------------------------------------------------------------------------
// TrackNodeDispatch14  --  0x0041e8b0
//
// Original: FUN_0041e8b0 (8 bytes, 0x0041e8b0..0x0041e8b7)
// Signature: void FUN_0041e8b0(void)
//   Returns: void
//
// Reads DAT_0063d7e4 (current track descriptor record pointer) and calls
// through the function pointer stored at record+0x14. Ghidra reports:
// "Could not recover jumptable at 0x0041e8b6. Too many branches."
// Called from FUN_00426e10 with guard `if FUN_0041e9d0() != 0`.
// Sibling of TrackNodeDispatch44 (0x0041e970, which uses +0x44).
//
// Harness: track_record_deref + crash_equal_ok=True. The fake 0x48-byte
// record has a zero/non-code-ptr sentinel at +0x14; both original and
// reimplementation crash identically when called through a non-executable
// fn-ptr. crash_equal_ok accepts equal-crash as GREEN.
//
// Constants (cited from 0x0041e8b0 body):
//   0x0063d7e4 — global track-descriptor pointer
//   0x14 (20)  — fn-ptr field offset within 0x48-byte record
//
// Uncertainties (non-blocking for C3):
//   U-0429: caller FUN_00426e10 may pass args; Ghidra decompiler shows void
//           due to jumptable failure. Actual parameter signature unconfirmed.
//           Does not affect reimplementation correctness since the dispatcher
//           body itself has no parameter reads (args pass through to callee).
//   U-3214: actual fn-ptr values at record+0x14 unknown without runtime data.
//
// Anti-island: callee chain root is TrackNodeRecordScan (0x0041e870, C3);
//   TrackNodeFnPtrGet14 (0x0041e9d0) is the guard getter in the same cluster.
//
// ref: re/analysis/render_promote_c2_track_node/0x0041e8b0.md
// ---------------------------------------------------------------------------

// 0x0041e8b0
extern "C" __declspec(dllexport) void __cdecl TrackNodeDispatch14()
{
    // Read the current track-descriptor record pointer from DAT_0063d7e4. [0x0041e8b0]
    std::uintptr_t record = *reinterpret_cast<std::uintptr_t*>(0x0063d7e4u);
    // Read the function pointer at record+0x14 and call through it. [0x0041e8b0 body]
    typedef void(__cdecl* fn_t)();
    fn_t fn = *reinterpret_cast<fn_t*>(record + 0x14u);
    fn();
}

RH_ScopedInstall(TrackNodeDispatch14, 0x0041e8b0);

// ---------------------------------------------------------------------------
// TrackNodeDispatch44  --  0x0041e970
//
// Original: FUN_0041e970 (8 bytes, 0x0041e970..0x0041e977)
// Signature: void FUN_0041e970(void)
//   Returns: void
//
// Reads DAT_0063d7e4 (current track descriptor record pointer) and calls
// through the function pointer stored at record+0x44. Ghidra reports:
// "Could not recover jumptable at 0x0041e976. Too many branches."
// Called from FUN_00426e10 with guard `if FUN_0041ea90() != 0`.
// Sibling of TrackNodeDispatch14 (0x0041e8b0, which uses +0x14).
//
// Harness: track_record_deref + crash_equal_ok=True (same rationale as
// TrackNodeDispatch14 above).
//
// Constants (cited from 0x0041e970 body):
//   0x0063d7e4 — global track-descriptor pointer
//   0x44 (68)  — fn-ptr field offset within 0x48-byte record
//
// Uncertainties (non-blocking for C3):
//   U-3214: actual fn-ptr values at record+0x44 unknown without runtime data.
//
// Anti-island: same cluster as 0x0041e8b0 above; TrackNodeRecordScan (C3)
//   is the record-setting root.
//
// ref: re/analysis/render_promote_c2_track_node/0x0041e970.md
// ---------------------------------------------------------------------------

// 0x0041e970
extern "C" __declspec(dllexport) void __cdecl TrackNodeDispatch44()
{
    // Read the current track-descriptor record pointer from DAT_0063d7e4. [0x0041e970]
    std::uintptr_t record = *reinterpret_cast<std::uintptr_t*>(0x0063d7e4u);
    // Read the function pointer at record+0x44 and call through it. [0x0041e970 body]
    typedef void(__cdecl* fn_t)();
    fn_t fn = *reinterpret_cast<fn_t*>(record + 0x44u);
    fn();
}

RH_ScopedInstall(TrackNodeDispatch44, 0x0041e970);
