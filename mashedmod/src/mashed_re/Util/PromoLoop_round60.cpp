// Mashed RE — promote-round round 60 (4-way pointer-global selector).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// FUN_0045c640 (0x0045c640): __cdecl, self-contained. Sets two pointer-globals
// 0x0088fbc4 / 0x0088fbc8 to constant image VAs selected by param_1 (DEC/JZ chain).
// Byte-verified:
//   8b442404 48 7445   MOV EAX,[ESP+4] ; DEC ; JZ case1
//   48 742d            DEC ; JZ case2
//   48 7415            DEC ; JZ case3
//   default: MOV [0x0088fbc4],0x88f6b0 ; MOV [0x0088fbc8],0x88eba0 ; RET
//   case3:   MOV [0x0088fbc4],0x88eb80 ; MOV [0x0088fbc8],0x88f6c0 ; RET
//   case2:   MOV [0x0088fbc4],0x88f0b0 ; MOV [0x0088fbc8],0x88f0e0 ; RET
//   case1:   MOV [0x0088fbc4],0x88e67c ; MOV [0x0088fbc8],0x88e680 ; RET

#include "../Core/HookSystem.h"

#include <cstdint>

// BUGFIX 2026-07-27 — CRASHER (installed-hook ABI mismatch, the 0x0047bda0 class).
// The C body above was VALUE-correct but REGISTER-wrong. The original touches ONLY EAX
// (`MOV EAX,[ESP+4]` + three `DEC EAX`); compiler-generated C is free to clobber EAX, ECX
// and EDX as caller-saved scratch. Its sole caller FUN_0045c6b0 is NOT hooked and carries
// its loop index in EDX straight across the call:
//   0x0045c6b1 33d2            XOR EDX,EDX          ; index = 0
//   0x0045c6b4 52              PUSH EDX
//   0x0045c6b5 e886ffffff      CALL 0x0045c640      ; <- must preserve EDX
//   0x0045c6c6 c70495c0f08800  MOV [EDX*4 + 0x0088f0c0],0
//   0x0045c711 42              INC EDX / CMP EDX,4 / JL 0x0045c6b4
// With the C hook installed EDX came back as 0x0088fbc8, and
//   0x0088fbc8*4 + 0x0088f0c0 = 0x02acdfe0
// which is exactly the observed write-AV address. Stock 2/2 clean, hooked 2/2 AV with
// byte-identical register state. Transcribed verbatim so only EAX is disturbed.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Sel88fbc4(std::int32_t /*p1*/) {
    __asm {
        mov  eax, dword ptr [esp+4]
        dec  eax
        je   L_S60_C1
        dec  eax
        je   L_S60_C2
        dec  eax
        je   L_S60_C3
        mov  dword ptr ds:[088FBC4h], 088F6B0h      // default
        mov  dword ptr ds:[088FBC8h], 088EBA0h
        ret
    L_S60_C3:
        mov  dword ptr ds:[088FBC4h], 088EB80h
        mov  dword ptr ds:[088FBC8h], 088F6C0h
        ret
    L_S60_C2:
        mov  dword ptr ds:[088FBC4h], 088F0B0h
        mov  dword ptr ds:[088FBC8h], 088F0E0h
        ret
    L_S60_C1:
        mov  dword ptr ds:[088FBC4h], 088E67Ch
        mov  dword ptr ds:[088FBC8h], 088E680h
        ret
    }
}
RH_ScopedInstall(Sel88fbc4, 0x0045c640);
