// Mashed RE — scenario-attach batch sa1 session 1 (HUD leaf C2->C3).
//
// Binary anchor:
//   original\MASHED.exe.unpatched  SHA-256
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//
// Hooks in this file:
//   0x00413bc0  HudUvRect413bc0 — index->UV-rect leaf (ESI=idx, ECX=out*)
//
// Slate audit (re/analysis/scenario_attach_sa1_viability_2026-06-15.md): the
// other four sa1 candidates are NOT promotable with the current harness and were
// NOT authored — 0x0041d870 / 0x0041e850 are pure vtable[0x48] draw dispatchers
// (FUN_0041d410 / FUN_0041e630) with no memory observable; 0x00413bb0 is a
// destructive destructor (FUN_004768c0, double-free under A/B); 0x00413b80 is a
// heavy RW initializer (FUN_004770c0) storing non-deterministic heap handles.
// Only 0x00413bc0 is a clean deterministic leaf. It needed a new arg_type
// (esi_idx_ecx_outbuf4) because int_outbuf4 passes args cdecl-on-stack and
// fastcall_reg only seeds ECX/EDX — neither sets ESI. User-approved building it.

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// 0x00413bc0  HudUvRect413bc0
//
// Original (0x00413bc0..0x00413c6c, 173 bytes). __fastcall-hybrid:
//   index in ESI, out-pointer in ECX. Writes 4 floats (a UV rect) to
//   [ECX], [ECX+4], [ECX+8], [ECX+0xc]. Register-only, plain RET (no stack args).
//
// cell = *(float*)0x005cd060 = 0x3e800000 = 0.25f  (read from .rdata; next bytes
//        spell "Vehi…" = the "VehicleIcons" string at 0x5cd064).
//
//   ESI < 0  ||  ESI >= 5      -> { 0.0, 0.0, 1.0, 1.0 }           (full-texture quad)
//   ESI == 3                   -> { 0.255, 0.255, 0.995, 0.995 }   (0x3e828f5c / 0x3f7eb852, inset)
//   else (ESI in {0,1,2,4})    -> 4-wide atlas grid, c = ESI & 3, r = ESI >> 2:
//                                 { c*cell, r*cell, (c+1)*cell, (r+1)*cell }
//
// The original computes the grid UV for every in-range ESI, then OVERWRITES it
// with the inset when ESI==3 (JNZ after CMP ESI,3 skips the override otherwise).
// We branch to the inset directly for ESI==3 — same final output buffer.
//
// FP bit-identity: the grid path reproduces the orig's exact x87 ops
//   FILD [int] ; FMUL dword ptr ds:[0x005cd060] ; FSTP [dst]
// (FILD/FMUL/FSTP of the same int by the same .rdata const), so even the 80-bit
// intermediate rounding matches. ESI>=0 in the grid path, so the orig's
// CDQ/AND EDX,3/ADD round-toward-zero adjust is a no-op here (plain SAR).
//
// ref: re/analysis/bucket_00412130/0x00413bc0.md ; listing 0x00413bc0..0x00413c6c
// ---------------------------------------------------------------------------

// MATCH-S03 (2026-10-31): the body is the matched source re/match/s03/h2_00413bc0.c (byte-identical
// under re/tools/match/match.py, cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1), replacing the naked-asm
// transcription. There FUN_00413bc0 is a file-static callee (the period compiler passes out in ECX and
// idx in ESI); the naked shim below is only the register entry the .asi installs.
#define MATCH_PORT 1
#define DAT_005cd060 (*reinterpret_cast<const float*>(0x005cd060u))
extern "C" {
// 0x00413bc0  FUN_00413bc0
#include "../../../../re/match/s03/h2_00413bc0.c"
}
#undef DAT_005cd060
#undef MATCH_PORT

extern "C" __declspec(dllexport) __declspec(naked) void __cdecl HudUvRect413bc0()
{
    __asm {
        push esi                     // idx (arg2)
        push ecx                     // out (arg1)
        call FUN_00413bc0
        add  esp, 8
        ret
    }
}

RH_ScopedInstall(HudUvRect413bc0, 0x00413bc0);
