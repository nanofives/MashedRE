// Mashed RE — per-vehicle-slot AABB/OBB corner+midpoint expander (0x0046b1c0).
//
// Zero-callee straight-line PURE LEAF (area-vehicle round 4). Disasm 0x0046b1c0
// (verbatim; log/area-vehicle/disasm_46b1c0.txt).
//
//   int fn(uint slot, float* in6)
//     if (slot > 0xf) return 0;                      // bounds guard
//     rec = 0x008815a0 + slot*0xd04;                 // per-vehicle record
//     writes rec[+0x90 .. +0x134] (42 dwords, 0xa8 bytes) derived ENTIRELY from
//       in6[0..5] (param_2[0..5]) plus consts 0x005cc32c (=0.5f) and 0x005ce034.
//     return 1;
//
// The apparent reads of rec[+0x90..] are reads of this call's OWN writes, not live
// state -> the whole 0xa8-byte output block is a pure function of (slot, in6). The
// write TARGET 0x008815a0+slot*0xd04 is the same 0xd04-stride per-vehicle record ai
// reads at 0x00881f74+veh*0xd04 (velocity, rec+0x944) — a shared struct (see
// re/analysis/vehicle_round3_frontier.md; reported to the CROSS_AREA_BUS) — but the
// COMPUTATION here reads none of ai's fields, so it is synthetically diffable.
//
// Transcribed VERBATIM (a C reimpl would round the x87 midpoint averages
// differently — the original keeps 80-bit intermediates across the interleaved
// FXCH schedule and only rounds on each FSTP). Constants (image .rdata, present when
// injected): 0x005cc32c (0.5f), 0x005ce034. Record base 0x008815a0.
// Callers (caller-half): FUN_004111c0 (util C2), FUN_0040e590 (util C2) — both
//   UNCONDITIONAL_CALL, established via CallersPC.java (log/area-vehicle/callers_46b1c0.txt).
// Binary anchor: MASHED.exe size=2,846,720 sha256=BDCAE093...EFD3C0E
#include "../Core/HookSystem.h"
#include <cstdint>

// ─────────────────────────────────────────────────────────────────────────────
// 0x0046b1c0  VehicleSlotAabbExpand
//   int VehicleSlotAabbExpand(unsigned slot, float* in6)
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) __declspec(naked) int __cdecl VehicleSlotAabbExpand(
        unsigned /*slot*/, float* /*in6*/) {
    __asm {
        mov  eax, dword ptr [esp+4]              // slot
        sub  esp, 0Ch                            // 0xc bytes local scratch
        cmp  eax, 10h
        jc   L_VSAE_OK
        xor  eax, eax
        add  esp, 0Ch
        ret
    L_VSAE_OK:
        mov  ecx, dword ptr [esp+14h]            // in6 (param_2, post-sub)
        imul eax, eax, 0D04h
        mov  edx, dword ptr [ecx]
        add  eax, 8815A0h
        mov  dword ptr [eax+90h], edx
        mov  edx, dword ptr [ecx+10h]
        mov  dword ptr [eax+94h], edx
        fld  dword ptr [eax+90h]
        mov  edx, dword ptr [ecx+8]
        mov  dword ptr [eax+98h], edx
        mov  edx, dword ptr [ecx+0Ch]
        mov  dword ptr [eax+9Ch], edx
        mov  edx, dword ptr [ecx+10h]
        fadd dword ptr [eax+9Ch]
        mov  dword ptr [eax+0A0h], edx
        mov  edx, dword ptr [ecx+8]
        mov  dword ptr [eax+0A4h], edx
        fld  dword ptr [eax+0A0h]
        mov  edx, dword ptr [ecx]
        fadd dword ptr [eax+94h]
        mov  dword ptr [eax+0A8h], edx
        fld  dword ptr [eax+98h]
        mov  edx, dword ptr [ecx+10h]
        fadd dword ptr [eax+0A4h]
        mov  dword ptr [eax+0ACh], edx
        mov  edx, dword ptr [ecx+14h]
        mov  dword ptr [eax+0B0h], edx
        mov  edx, dword ptr [ecx+0Ch]
        mov  dword ptr [eax+0B4h], edx
        mov  edx, dword ptr [ecx+10h]
        mov  dword ptr [eax+0B8h], edx
        mov  edx, dword ptr [ecx+14h]
        mov  dword ptr [eax+0BCh], edx
        mov  edx, dword ptr [ecx]
        mov  dword ptr [eax+0C0h], edx
        mov  edx, dword ptr [ecx+4]
        mov  dword ptr [eax+0C4h], edx
        mov  edx, dword ptr [ecx+8]
        mov  dword ptr [eax+0C8h], edx
        mov  edx, dword ptr [ecx+0Ch]
        mov  dword ptr [eax+0CCh], edx
        mov  edx, dword ptr [ecx+4]
        mov  dword ptr [eax+0D0h], edx
        mov  edx, dword ptr [ecx+8]
        mov  dword ptr [eax+0D4h], edx
        mov  edx, dword ptr [ecx]
        mov  dword ptr [eax+0D8h], edx
        mov  edx, dword ptr [ecx+4]
        mov  dword ptr [eax+0DCh], edx
        mov  edx, dword ptr [ecx+14h]
        mov  dword ptr [eax+0E0h], edx
        mov  edx, dword ptr [ecx+0Ch]
        mov  dword ptr [eax+0E4h], edx
        mov  edx, dword ptr [ecx+4]
        fstp dword ptr [esp+8]
        fxch
        mov  dword ptr [eax+0E8h], edx
        fmul dword ptr ds:[05CC32Ch]
        mov  ecx, dword ptr [ecx+14h]
        mov  dword ptr [eax+0ECh], ecx
        fstp dword ptr [eax+0F0h]
        fmul dword ptr ds:[05CC32Ch]
        fstp dword ptr [eax+0F4h]
        fld  dword ptr [esp+8]
        fmul dword ptr ds:[05CC32Ch]
        fstp dword ptr [eax+0F8h]
        fld  dword ptr [eax+9Ch]
        fadd st(0), st(0)
        fld  dword ptr [eax+0A0h]
        fadd st(0), st(0)
        fld  dword ptr [eax+0A4h]
        fadd st(0), st(0)
        fstp dword ptr [esp+8]
        fxch
        fadd dword ptr [eax+0B4h]
        fstp dword ptr [esp]
        fadd dword ptr [eax+0B8h]
        fld  dword ptr [esp+8]
        fadd dword ptr [eax+0BCh]
        fld  dword ptr [esp]
        fmul dword ptr ds:[05CE034h]
        fstp dword ptr [eax+0FCh]
        fxch
        fmul dword ptr ds:[05CE034h]
        fstp dword ptr [eax+100h]
        fmul dword ptr ds:[05CE034h]
        fstp dword ptr [eax+104h]
        fld  dword ptr [eax+0B4h]
        fadd st(0), st(0)
        fld  dword ptr [eax+0B8h]
        fadd st(0), st(0)
        fld  dword ptr [eax+0BCh]
        fadd st(0), st(0)
        fstp dword ptr [esp+8]
        fxch
        fadd dword ptr [eax+9Ch]
        fstp dword ptr [esp]
        fadd dword ptr [eax+0A0h]
        fld  dword ptr [esp+8]
        fadd dword ptr [eax+0A4h]
        fld  dword ptr [esp]
        fmul dword ptr ds:[05CE034h]
        fstp dword ptr [eax+108h]
        fxch
        fmul dword ptr ds:[05CE034h]
        fstp dword ptr [eax+10Ch]
        fmul dword ptr ds:[05CE034h]
        fstp dword ptr [eax+110h]
        fld  dword ptr [eax+0B4h]
        fadd dword ptr [eax+0A8h]
        fld  dword ptr [eax+0B8h]
        fadd dword ptr [eax+0ACh]
        fld  dword ptr [eax+0BCh]
        fadd dword ptr [eax+0B0h]
        fstp dword ptr [esp+8]
        fxch
        fmul dword ptr ds:[05CC32Ch]
        fstp dword ptr [eax+114h]
        fmul dword ptr ds:[05CC32Ch]
        fstp dword ptr [eax+118h]
        fld  dword ptr [esp+8]
        fmul dword ptr ds:[05CC32Ch]
        fstp dword ptr [eax+11Ch]
        fld  dword ptr [eax+0A8h]
        fadd st(0), st(0)
        fld  dword ptr [eax+0ACh]
        fadd st(0), st(0)
        fld  dword ptr [eax+0B0h]
        fadd st(0), st(0)
        fstp dword ptr [esp+8]
        fxch
        fadd dword ptr [eax+90h]
        fstp dword ptr [esp]
        fadd dword ptr [eax+94h]
        fld  dword ptr [esp+8]
        fadd dword ptr [eax+98h]
        fld  dword ptr [esp]
        fmul dword ptr ds:[05CE034h]
        fstp dword ptr [eax+120h]
        fxch
        fmul dword ptr ds:[05CE034h]
        fstp dword ptr [eax+124h]
        fmul dword ptr ds:[05CE034h]
        fstp dword ptr [eax+128h]
        fld  dword ptr [eax+90h]
        fadd st(0), st(0)
        fld  dword ptr [eax+94h]
        fadd st(0), st(0)
        fld  dword ptr [eax+98h]
        fadd st(0), st(0)
        fstp dword ptr [esp+8]
        fxch
        fadd dword ptr [eax+0A8h]
        fstp dword ptr [esp]
        fadd dword ptr [eax+0ACh]
        fld  dword ptr [esp+8]
        fadd dword ptr [eax+0B0h]
        fld  dword ptr [esp]
        fmul dword ptr ds:[05CE034h]
        fstp dword ptr [eax+12Ch]
        fxch
        fmul dword ptr ds:[05CE034h]
        fstp dword ptr [eax+130h]
        fmul dword ptr ds:[05CE034h]
        fstp dword ptr [eax+134h]
        mov  eax, 1
        add  esp, 0Ch
        ret
    }
}
RH_ScopedInstall(VehicleSlotAabbExpand, 0x0046b1c0);
