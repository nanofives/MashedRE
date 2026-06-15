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

// 0x00413bc0
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl HudUvRect413bc0()
{
    __asm {
        // ESI = index, ECX = out pointer (set by caller / harness trampoline).
        test esi, esi
        jl   default_quad                       // ESI < 0
        cmp  esi, 5
        jge  default_quad                       // ESI >= 5
        cmp  esi, 3
        je   inset_quad                          // ESI == 3 -> inset override

        // ── grid case (ESI in {0,1,2,4}) ──────────────────────────────────
        mov  eax, esi
        and  eax, 3                              // col = ESI & 3
        mov  edx, esi
        sar  edx, 2                              // row = ESI >> 2 (ESI>=0)
        push edx                                 // [esp+4] scratch = row
        push eax                                 // [esp]   scratch = col
        // out[0] = col * cell
        fild dword ptr [esp]
        fmul dword ptr ds:[0x005cd060]
        fstp dword ptr [ecx]
        // out[1] = row * cell
        fild dword ptr [esp+4]
        fmul dword ptr ds:[0x005cd060]
        fstp dword ptr [ecx+4]
        // out[2] = (col+1) * cell
        mov  eax, [esp]
        inc  eax
        mov  [esp], eax
        fild dword ptr [esp]
        fmul dword ptr ds:[0x005cd060]
        fstp dword ptr [ecx+8]
        // out[3] = (row+1) * cell
        mov  eax, [esp+4]
        inc  eax
        mov  [esp+4], eax
        fild dword ptr [esp+4]
        fmul dword ptr ds:[0x005cd060]
        fstp dword ptr [ecx+0xc]
        add  esp, 8
        ret

    inset_quad:
        mov  eax, 0x3f7eb852                     // 0.995f
        mov  dword ptr [ecx],     0x3e828f5c     // 0.255f
        mov  dword ptr [ecx+4],   0x3e828f5c     // 0.255f
        mov  dword ptr [ecx+8],   eax
        mov  dword ptr [ecx+0xc], eax
        ret

    default_quad:
        mov  eax, 0x3f800000                     // 1.0f
        mov  dword ptr [ecx],     0
        mov  dword ptr [ecx+4],   0
        mov  dword ptr [ecx+8],   eax
        mov  dword ptr [ecx+0xc], eax
        ret
    }
}

RH_ScopedInstall(HudUvRect413bc0, 0x00413bc0);
