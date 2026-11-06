// Mashed RE — cluster wf_cfd622ea-c14-4, promote-round (C2->C3).
//
// Binary anchor:
//   original\MASHED.exe.unpatched  size 2,846,720
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//
// ============================================================
// 0x00472820  Vec3IsValidNonTiny  (util) — 121 bytes
// ============================================================
//
// float3 validator: returns 1 if all 3 components finite AND mag^2 > threshold.
// Returns 0 if any component is infinity/NaN OR mag^2 <= threshold.
//
// Signature: int __cdecl FUN_00472820(float* vec)
//   arg0 = vec  : pointer to 3-element float array [x, y, z]
//   return      : 1 = valid (finite + non-tiny), 0 = invalid
//
// Caller:  0x0045d7a0 FUN_0045d7a0 (util, C2)
// Callee:  0x004a3907 FUN_004a3907 (msvc-crt-main library, C2)
//          - 20-byte inline _finite check: (*(uint16_t*)((char*)&d+6) & 0x7ff0) != 0x7ff0
//          - Inspects exponent bits of the promoted double; 0x7ff0 = all 11 exp bits set
//            (infinity/NaN); setne cl = 1 if finite, 0 if infinity/NaN.
//
// Threshold at 0x005ce54c (.rdata, always valid): float value 1.0e-6f (0x358637bd).
// Function returns 1 only when mag^2 = x*x + y*y + z*z > 1.0e-6f.
//
// Disasm (0x00472820..0x00472898, 121 bytes):
//   push esi
//   mov esi,[esp+8]               ; vec = arg0
//   fld [esi]  → fstp qword[esp] → call 0x4a3907 → test eax → je returns0  ; x finite?
//   fld [esi+4]→ fstp qword[esp] → call 0x4a3907 → test eax → je returns0  ; y finite?
//   fld [esi+8]→ fstp qword[esp] → call 0x4a3907 → test eax → je returns0  ; z finite?
//   fld [esi+4] ; y          fld [esi] ; x         fld [esi+8] ; z
//   fld st(2)   ; y copy     fmul st(3) ; y*y      fld st(2) ; x copy
//   fmul st(3)  ; x*x        faddp      ; x*x+y*y  fld st(1) ; z copy
//   fmul st(2)  ; z*z        faddp      ; total mag^2
//   fcomp dword [0x005ce54c]            ; compare mag^2 with 1.0e-6f threshold
//   fstp st(0) ; pop z                  fnstsw ax
//   fstp st(0) ; pop x                  test ah, 0x41   ; C0 | C3 set -> below/equal
//   fstp st(0) ; pop y                  jnp returns0    ; mag^2 <= threshold -> 0
//   mov eax,1; pop esi; ret             ; mag^2 > threshold -> returns 1
//   returns0: xor eax,eax; pop esi; ret

#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>

// MATCH-S11: the body of 0x00472820 (Vec3IsValidNonTiny) moved to Match/MatchS11_00472820.cpp,
// which compiles the byte-matched source re/match/s11/00472820.c.
