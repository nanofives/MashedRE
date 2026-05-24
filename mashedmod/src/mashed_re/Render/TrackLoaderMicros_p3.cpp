// Mashed RE — Render track-loader micro-functions (c3-batch-p session 3).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x00426060  TrackPhysWorld1Get  — 5B; returns DAT_0065742c as uint32
//   0x00426070  TrackPhysWorld2Get  — 5B; returns DAT_00656ee8 as uint32
//   0x004260c0  TrackPhysWorld3Get  — 5B; returns DAT_00657490 as uint32
//   0x00426e00  TrackLoaderFloatGet — 6B; returns DAT_00644368 as x87 double
//   0x00426cd0  TrackSlotArrayReset — 22B; fills 6-dword array at 0x0066d6e4
//                                          with 0xFFFFFFFF; returns 1
//
// Analysis notes:
//   re/analysis/render_promote_c2_track_loader/0x00426060.md
//   re/analysis/render_promote_c2_track_loader/0x00426070.md
//   re/analysis/render_promote_c2_track_loader/0x004260c0.md
//   re/analysis/render_promote_c2_track_loader/0x00426e00.md
//   re/analysis/render_promote_c2_track_loader/0x00426cd0.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// TrackPhysWorld1Get  --  0x00426060
//
// Original: FUN_00426060 (5 bytes, 0x00426060..0x00426064)
// Signature: undefined4 FUN_00426060(void)
//   Returns: DAT_0065742c as unsigned 32-bit value.
//
// Disassembly (cited from 0x00426060 body):
//   MOV EAX, [0x0065742c]   ; load 4-byte global
//   RET
//
// No branches, no side effects, no writes. Pure leaf getter.
//
// Constants:
//   0x0065742c — physics world handle 1 global (called from 6 callers:
//                0x004013f0, 0x0041f8f0, 0x00420050, 0x0047eb30,
//                0x0047fad0, 0x00480340)
//
// Uncertainties: none.
// Anti-island: 6 callers at C2+.
//
// ref: re/analysis/render_promote_c2_track_loader/0x00426060.md
// ---------------------------------------------------------------------------

// 0x00426060
extern "C" __declspec(dllexport) std::uint32_t __cdecl TrackPhysWorld1Get()
{
    return *reinterpret_cast<std::uint32_t*>(0x0065742cu);
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(TrackPhysWorld1Get, 0x00426060);

// ---------------------------------------------------------------------------
// TrackPhysWorld2Get  --  0x00426070
//
// Original: FUN_00426070 (5 bytes, 0x00426070..0x00426074)
// Signature: undefined4 FUN_00426070(void)
//   Returns: DAT_00656ee8 as unsigned 32-bit value.
//
// Disassembly (cited from 0x00426070 body):
//   MOV EAX, [0x00656ee8]   ; load 4-byte global
//   RET
//
// No branches, no side effects, no writes. Pure leaf getter.
//
// Constants:
//   0x00656ee8 — physics world handle 2 global (called from 5 callers:
//                0x0040d270, 0x0041f8f0, 0x0047d640, 0x0047def0,
//                0x00480340)
//
// Uncertainties: none.
// Anti-island: 5 callers at C2+.
//
// ref: re/analysis/render_promote_c2_track_loader/0x00426070.md
// ---------------------------------------------------------------------------

// 0x00426070
extern "C" __declspec(dllexport) std::uint32_t __cdecl TrackPhysWorld2Get()
{
    return *reinterpret_cast<std::uint32_t*>(0x00656ee8u);
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(TrackPhysWorld2Get, 0x00426070);

// ---------------------------------------------------------------------------
// TrackPhysWorld3Get  --  0x004260c0
//
// Original: FUN_004260c0 (5 bytes, 0x004260c0..0x004260c4)
// Signature: undefined4 FUN_004260c0(void)
//   Returns: DAT_00657490 as unsigned 32-bit value.
//
// Disassembly (cited from 0x004260c0 body):
//   MOV EAX, [0x00657490]   ; load 4-byte global
//   RET
//
// No branches, no side effects, no writes. Pure leaf getter.
//
// Constants:
//   0x00657490 — physics world handle 3 global (called from 2 callers:
//                0x00420050, 0x00481a30)
//
// Uncertainties (non-blocking):
//   U-3653: semantics of DAT_00657490 not established in this session.
//           Evidence needed: cross-reference all stores to 0x00657490.
//           Does not block C3 — structural identity is fully confirmed.
//
// Anti-island: 2 callers at C2+.
//
// ref: re/analysis/render_promote_c2_track_loader/0x004260c0.md
// ---------------------------------------------------------------------------

// 0x004260c0
extern "C" __declspec(dllexport) std::uint32_t __cdecl TrackPhysWorld3Get()
{
    return *reinterpret_cast<std::uint32_t*>(0x00657490u);
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(TrackPhysWorld3Get, 0x004260c0);

// ---------------------------------------------------------------------------
// TrackLoaderFloatGet  --  0x00426e00
//
// Original: FUN_00426e00 (6 bytes, 0x00426e00..0x00426e05)
// Signature: float10 FUN_00426e00(void)
//   Returns: DAT_00644368 via x87 FPU (fld qword; ret).
//
// Disassembly (cited from 0x00426e00 body):
//   FLD QWORD PTR [0x00644368]   ; load 8-byte IEEE-754 double onto ST0
//   RET                          ; return with value in ST0 (x87 float10 convention)
//
// No branches, no side effects, no writes. Pure leaf getter.
//
// NOTE: The original uses x87 `fld qword` — it loads an 8-byte (double) value
// from 0x00644368 and leaves it on ST0. Ghidra annotates the return type as
// `float10` (80-bit extended). In practice the loaded value is an IEEE-754
// double (8 bytes) that the x87 FPU silently extends to 80 bits on load.
// Our C++ reimpl reads the same address as `double` and returns it via the
// x87-based `__cdecl` double return convention (ST0), which is structurally
// identical. Frida cannot capture 80-bit x87 returns reliably; the harness
// uses ret='void' + arg_type='none' (voidMatch).
//
// Constants:
//   0x00644368 — track-related float (8 bytes, adjacent to record count at
//                0x00644370); semantics uncertain (U-3656, non-blocking).
//
// Uncertainties (non-blocking):
//   U-3656: semantics of DAT_00644368 not established. Does not block C3 —
//           structural identity (fld qword [imm32]; ret) is fully confirmed.
//
// Anti-island: 1 caller (FUN_0041f8f0, C2+).
//
// ref: re/analysis/render_promote_c2_track_loader/0x00426e00.md
// ---------------------------------------------------------------------------

// 0x00426e00
extern "C" __declspec(dllexport) double __cdecl TrackLoaderFloatGet()
{
    // Load the 8-byte (double) value from DAT_00644368, matching `fld qword` / ret.
    return *reinterpret_cast<double*>(0x00644368u);
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(TrackLoaderFloatGet, 0x00426e00);

// ---------------------------------------------------------------------------
// TrackSlotArrayReset  --  0x00426cd0
//
// Original: FUN_00426cd0 (22 bytes, 0x00426cd0..0x00426ce5)
// Signature: undefined4 FUN_00426cd0(void)
//   Returns: 1
//
// Algorithm (cited from 0x00426cd0 body):
//   1. puVar2 = &DAT_0066d6e4                          [entry]
//   2. Loop 6 times: *puVar2 = 0xffffffff; puVar2++   [body]
//      Covers 0x0066d6e4..0x0066d6fb (6 × 4 = 24 bytes)
//   3. Return 1                                        [exit]
//
// The 6-dword array at 0x0066d6e4 is the track-slot-ID array. Writing
// 0xFFFFFFFF (= -1 as int32) marks all 6 slots as "invalid" / empty.
// Called early in TRACK_LOAD_FN (from FUN_00426e10) to reset slots before
// loading a new track.
//
// Constants (all cited from 0x00426cd0 body):
//   0x0066d6e4 — track slot[0]  (first of 6 consecutive dwords)
//   0x0066d6e8 — track slot[1]
//   0x0066d6ec — track slot[2]
//   0x0066d6f0 — track slot[3]
//   0x0066d6f4 — track slot[4]
//   0x0066d6f8 — track slot[5]
//   0xFFFFFFFF — invalid sentinel (−1 as signed int32)
//
// Uncertainties: none.
// Anti-island: caller is FUN_00426e10 (C2+).
//
// ref: re/analysis/render_promote_c2_track_loader/0x00426cd0.md
// ---------------------------------------------------------------------------

// 0x00426cd0
extern "C" __declspec(dllexport) std::uint32_t __cdecl TrackSlotArrayReset()
{
    // Write 0xFFFFFFFF to 6 consecutive dwords starting at DAT_0066d6e4.
    // [0x00426cd0 body: puVar2 = &DAT_0066d6e4; loop 6 times]
    std::uint32_t* slot = reinterpret_cast<std::uint32_t*>(0x0066d6e4u);
    slot[0] = 0xFFFFFFFFu;  // track slot[0]  [0x0066d6e4]
    slot[1] = 0xFFFFFFFFu;  // track slot[1]  [0x0066d6e8]
    slot[2] = 0xFFFFFFFFu;  // track slot[2]  [0x0066d6ec]
    slot[3] = 0xFFFFFFFFu;  // track slot[3]  [0x0066d6f0]
    slot[4] = 0xFFFFFFFFu;  // track slot[4]  [0x0066d6f4]
    slot[5] = 0xFFFFFFFFu;  // track slot[5]  [0x0066d6f8]
    return 1u;
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(TrackSlotArrayReset, 0x00426cd0);
