// Mashed RE — Render track-node leaf functions (c3-batch-o session 1 + c3-batch-p session 5).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x0041e870  TrackNodeRecordScan    — 48B pure leaf; scans 0x48B record array,
//                                         last-match-wins write to DAT_0063d7e4
//   0x0041e9d0  TrackNodeFnPtrGet14    — 8B getter; returns *(DAT_0063d7e4+0x14)
//
// c3-batch-p session 5 additions (render_track_node_remainder):
//   0x0041e9e0  TrackNodeFnPtrGet18    — 8B getter; returns *(DAT_0063d7e4+0x18)
//   0x0041e8c0  TrackNodeDispatch18    — 8B indirect dispatch via *(DAT_0063d7e4+0x18)
//   0x0041e8f0  TrackNodeDispatch24    — 8B indirect dispatch via *(DAT_0063d7e4+0x24)
//   0x0041e9b0  TrackNodeFieldCmp10    — 20B compare *(DAT_0063d7e4+0x10) == param_1
//   0x0041e980  TrackNodeRecordFind    — 38B first-match-return scan of record array
//
// Still deferred (promoted in s1+s2; promoted via track_record_deref harness):
//   0x0041ea90  TrackNodeFnPtrGet44    — 8B getter; returns *(DAT_0063d7e4+0x44)
//   0x0041e8b0  TrackNodeDispatch14    — 8B indirect dispatch via *(DAT_0063d7e4+0x14)
//   0x0041e970  TrackNodeDispatch44    — 8B indirect dispatch via *(DAT_0063d7e4+0x44)
//
// Analysis notes:
//   re/analysis/render_promote_c2_track_node/0x0041e870.md
//   re/analysis/render_promote_c2_track_node/0x0041e8b0.md
//   re/analysis/render_promote_c2_track_node/0x0041e8c0.md
//   re/analysis/render_promote_c2_track_node/0x0041e8f0.md
//   re/analysis/render_promote_c2_track_node/0x0041e970.md
//   re/analysis/render_promote_c2_track_node/0x0041e980.md
//   re/analysis/render_promote_c2_track_node/0x0041e9b0.md
//   re/analysis/render_promote_c2_track_node/0x0041e9d0.md
//   re/analysis/render_promote_c2_track_node/0x0041e9e0.md
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
// TrackNodeFnPtrGet18  --  0x0041e9e0
//
// Original: sub_0041e9e0 (8 bytes, 0x0041e9e0..0x0041e9e7)
// Signature: undefined4 sub_0041e9e0(void)
//   Returns: dword at *(DAT_0063d7e4 + 0x18)
//
// Reads DAT_0063d7e4 (current track descriptor record pointer) and returns
// the dword at offset +0x18 within that record. Used by CAMERA_FN as a
// non-zero guard: `iVar2 = sub_0041e9e0(); if (iVar2 != 0) { ... camera path }`.
// Field +0x18 is a dual-role slot: readable as data here and called as a
// function pointer in TrackNodeDispatch18 (0x0041e8c0). Pattern: nullable fn-ptr
// guard + dispatch, split across two functions.
// Same DAT_0063d7e4 object as TrackNodeFnPtrGet14 (+0x14) and TrackNodeFieldCmp10 (+0x10).
//
// Constants (cited from 0x0041e9e0 body):
//   0x0063d7e4 (at 0x0041e9e3) — global track-descriptor pointer
//   0x18 (24)  (at 0x0041e9e9) — field offset within 0x48-byte record
//
// Uncertainties (non-blocking):
//   U-0554: full layout of DAT_0063d7e4 base object unknown; +0x10/+0x18 confirmed.
//
// Anti-island: callers include FUN_00426ab0 (CAMERA_FN, C2); sibling of TrackNodeDispatch18.
//
// ref: re/analysis/render_promote_c2_track_node/0x0041e9e0.md
// ---------------------------------------------------------------------------

// 0x0041e9e0
extern "C" __declspec(dllexport) std::uint32_t __cdecl TrackNodeFnPtrGet18() {
    // Read the current track-descriptor record pointer from DAT_0063d7e4.  [0x0041e9e3]
    std::uintptr_t record = *reinterpret_cast<std::uintptr_t*>(0x0063d7e4u);
    // Return the dword at record+0x18.  [0x0041e9e9]
    return *reinterpret_cast<std::uint32_t*>(record + 0x18u);
}

RH_ScopedInstall(TrackNodeFnPtrGet18, 0x0041e9e0);

// ---------------------------------------------------------------------------
// TrackNodeDispatch18  --  0x0041e8c0
//
// Original: sub_0041e8c0 (8 bytes, 0x0041e8c0..0x0041e8c7)
// Signature: void sub_0041e8c0(void)
//   Returns: void (indirect tail-call; no explicit return)
//
// Reads DAT_0063d7e4 (current track descriptor record pointer); reads the
// dword at record+0x18 as a function pointer; calls through it (indirect
// tail-call dispatch). Ghidra: "Could not recover jumptable at 0x0041e8c6;
// Too many branches" / "Treating indirect jump as call."
// Called from FUN_00426ab0 (CAMERA_FN) only when TrackNodeFnPtrGet18() != 0
// (guard checks the same +0x18 field non-null). No static callees; dispatch
// is runtime-resolved via the function pointer.
// Sibling of TrackNodeDispatch14 (0x0041e8b0, +0x14) and TrackNodeDispatch24 (0x0041e8f0, +0x24).
//
// Constants (cited from 0x0041e8c0 body):
//   0x0063d7e4 (at 0x0041e8c2) — global track-descriptor pointer
//   0x18 (24)  (at 0x0041e8c6) — fn-ptr field offset within 0x48-byte record
//
// Uncertainties (non-blocking for C3):
//   U-0550: DAT_0063d7e4 struct type and owner unknown.
//   U-0551: actual arg list unconfirmed (Ghidra shows 0 params due to jumptable confusion).
//
// Diff strategy: track_record_deref + crash_equal_ok=True.
//   At quiescent main-menu state, a fake 0x48B record is injected with the
//   fn-ptr field (0x18) set to sentinel values. Both original and reimpl call
//   through the fn-ptr; if sentinel is not a valid fn address, both crash
//   identically → crash_equal_ok=True → GREEN.
//
// Anti-island: callers include FUN_00426ab0 (C2); sibling of TrackNodeFnPtrGet18.
//
// ref: re/analysis/render_promote_c2_track_node/0x0041e8c0.md
// ---------------------------------------------------------------------------

// 0x0041e8c0
extern "C" __declspec(dllexport) void __cdecl TrackNodeDispatch18() {
    // Read the current track-descriptor record pointer from DAT_0063d7e4.  [0x0041e8c2]
    std::uintptr_t record = *reinterpret_cast<std::uintptr_t*>(0x0063d7e4u);
    // Read fn-ptr at record+0x18 and call through it.  [0x0041e8c6]
    typedef void (__cdecl *FnPtr_t)();
    FnPtr_t fn = *reinterpret_cast<FnPtr_t*>(record + 0x18u);
    fn();
}

RH_ScopedInstall(TrackNodeDispatch18, 0x0041e8c0);

// ---------------------------------------------------------------------------
// TrackNodeDispatch24  --  0x0041e8f0
//
// Original: FUN_0041e8f0 (8 bytes, 0x0041e8f0..0x0041e8f7)
// Signature: void FUN_0041e8f0(void)
//   Returns: void (indirect tail-call; no explicit return)
//
// Reads DAT_0063d7e4 (current track descriptor record pointer); reads the
// dword at record+0x24 as a function pointer; calls through it (indirect
// dispatch). Ghidra: "Could not recover jumptable at 0x0041e8f6. Too many
// branches" / "Treating indirect jump as call."
// Part of the same DAT_0063d7e4 fn-ptr dispatch family:
//   +0x14 → TrackNodeDispatch14 (FUN_0041e8b0)
//   +0x18 → TrackNodeDispatch18 (sub_0041e8c0)
//   +0x24 → TrackNodeDispatch24 (this function)
//   +0x44 → TrackNodeDispatch44 (FUN_0041e970)
//
// Constants (cited from 0x0041e8f0 body):
//   0x0063d7e4 (at 0x0041e8f0) — global track-descriptor pointer
//   0x24 (36)                  — fn-ptr field offset within 0x48-byte record
//
// Uncertainties (non-blocking for C3):
//   U-3128: dispatch target at 0x0041e8f6 is runtime-resolved; Ghidra failed to
//            recover jumptable. Actual callee set unknown without runtime trace.
//
// Diff strategy: track_record_deref + crash_equal_ok=True (same as TrackNodeDispatch18).
//
// Anti-island: sibling cluster of TrackNodeDispatch14/18/44 (all C2/C3).
//
// ref: re/analysis/render_promote_c2_track_node/0x0041e8f0.md
// ---------------------------------------------------------------------------

// 0x0041e8f0
extern "C" __declspec(dllexport) void __cdecl TrackNodeDispatch24() {
    // Read the current track-descriptor record pointer from DAT_0063d7e4.  [0x0041e8f0]
    std::uintptr_t record = *reinterpret_cast<std::uintptr_t*>(0x0063d7e4u);
    // Read fn-ptr at record+0x24 and call through it.  [dispatch]
    typedef void (__cdecl *FnPtr_t)();
    FnPtr_t fn = *reinterpret_cast<FnPtr_t*>(record + 0x24u);
    fn();
}

RH_ScopedInstall(TrackNodeDispatch24, 0x0041e8f0);

// ---------------------------------------------------------------------------
// TrackNodeFieldCmp10  --  0x0041e9b0
//
// Original: sub_0041e9b0 (20 bytes, 0x0041e9b0..0x0041e9c3)
// Signature: int sub_0041e9b0(int param_1)
//   param_1: value compared against *(int*)(DAT_0063d7e4+0x10)
//   Returns: 1 if equal, 0 otherwise
//
// Reads DAT_0063d7e4 (current track descriptor record pointer); reads the
// int at record+0x10; compares to param_1; returns 1 (equal) or 0 (not equal).
// No side effects; no calls; no writes. Pure comparison function.
// Called from FUN_00426ab0 (CAMERA_FN); return value appears discarded in
// decomp per U-0553 (may be a decompiler artifact at the call site).
// Same DAT_0063d7e4 object as TrackNodeFnPtrGet18 (+0x18) and TrackNodeFnPtrGet14 (+0x14).
//
// Constants (cited from 0x0041e9b0 body):
//   0x0063d7e4 (at 0x0041e9b3) — global track-descriptor pointer
//   0x10 (16)  (at 0x0041e9ba) — int field offset within 0x48-byte record
//
// Uncertainties (non-blocking for C3):
//   U-0552: DAT_0063d7e4+0x10 semantics (int field identity) unknown.
//   U-0553: CAMERA_FN discards the bool return; actual consumer unconfirmed.
//
// Anti-island: callers include FUN_00426ab0 (C2); siblings TrackNodeFnPtrGet14/18.
//
// ref: re/analysis/render_promote_c2_track_node/0x0041e9b0.md
// ---------------------------------------------------------------------------

// 0x0041e9b0
extern "C" __declspec(dllexport) int __cdecl TrackNodeFieldCmp10(int param_1) {
    // Read the current track-descriptor record pointer from DAT_0063d7e4.  [0x0041e9b3]
    std::uintptr_t record = *reinterpret_cast<std::uintptr_t*>(0x0063d7e4u);
    // Read int at record+0x10; compare to param_1.  [0x0041e9ba]
    std::int32_t field_val = *reinterpret_cast<std::int32_t*>(record + 0x10u);
    // Return 1 if equal, 0 otherwise.
    return (field_val == param_1) ? 1 : 0;
}

RH_ScopedInstall(TrackNodeFieldCmp10, 0x0041e9b0);

// ---------------------------------------------------------------------------
// TrackNodeRecordFind  --  0x0041e980
//
// Original: FUN_0041e980 (38 bytes, 0x0041e980..0x0041e9a5)
// Signature: char* FUN_0041e980(int param_1)
//   param_1: track index; compared against field +0x10 of each 0x48-byte record
//   Returns: pointer to first matching record base, or NULL if no match
//
// Iterates over the record array at s_training_005f33f8 (0x005f33f8),
// stride 0x48, count from DAT_005f37a0 (0x005f37a0). For each record, if
// *(int*)(record+0x10) == param_1: returns that record pointer immediately
// (early-exit on first match). Returns NULL if no match found.
// CONTRAST with TrackNodeRecordScan (0x0041e870): that scans ALL records and
// writes the LAST match to DAT_0063d7e4 (global side effect). This function:
//   - exits on FIRST match,
//   - returns pointer (does NOT write any global),
//   - is called by FUN_00426e10 which uses the result as a char* base ptr.
//
// Same scan infrastructure as TrackNodeRecordScan: DAT_005f37a0 count,
// s_training_005f33f8 array, stride 0x48, field +0x10. Different semantic:
// find-first vs write-last.
//
// Constants (cited from 0x0041e980 body):
//   0x005f37a0 — count of track descriptor records  [loop guard]
//   0x005f33f8 — record array base (s_training_005f33f8)  [loop body]
//   0x10 (16)  — int id field offset within 0x48-byte record  [loop body]
//   0x48 (72)  — stride per record  [loop body]
//
// Uncertainties (non-blocking for C3):
//   U-3213: full layout of 72-byte record at 0x005f33f8 unknown; only +0x10 confirmed.
//            (Shared with TrackNodeRecordScan; same uncertainty, same evidence gap.)
//
// Anti-island: callers include FUN_00426e10 (C2, render subsystem); callee
//   constraint N/A (pure leaf, leaf-function exemption, re/CONFIDENCE.md).
//   Shares scan globals with TrackNodeRecordScan (C3 seed, same file) — cluster passes.
//
// ref: re/analysis/render_promote_c2_track_node/0x0041e980.md
// ---------------------------------------------------------------------------

// 0x0041e980
extern "C" __declspec(dllexport) char* __cdecl TrackNodeRecordFind(int param_1) {
    // Read iteration count; if zero or negative, return NULL immediately.  [loop guard]
    std::int32_t count = *reinterpret_cast<std::int32_t*>(0x005f37a0u);
    if (count <= 0) {
        return nullptr;
    }

    // Scan array; return FIRST matching record pointer (early-exit).
    // Array base: s_training_005f33f8 (0x005f33f8); stride: 0x48 (72 bytes/record).
    const std::uintptr_t array_base    = 0x005f33f8u;
    const std::uintptr_t record_stride = 0x48u;
    const std::uintptr_t id_field_off  = 0x10u;  // int field at record+0x10

    for (std::int32_t i = 0; i < count; ++i) {
        std::uintptr_t record = array_base + static_cast<std::uint32_t>(i) * record_stride;
        if (*reinterpret_cast<std::int32_t*>(record + id_field_off) == param_1) {
            // First match: return immediately (no global write).
            return reinterpret_cast<char*>(record);
        }
    }
    return nullptr;
}

RH_ScopedInstall(TrackNodeRecordFind, 0x0041e980);
