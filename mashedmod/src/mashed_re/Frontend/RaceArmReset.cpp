// Mashed RE — C2->C3 promotion: FUN_00433240 RaceArmReset (subsystem: frontend).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Analysis:
//   re/analysis/bucket_0041dc30/0x00433240.md
//   re/analysis/frontend_c1_to_c2_followup_s2/FUN_00433240.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// RaceArmReset  --  0x00433240   (subsystem: frontend)
//
// Original: FUN_00433240 (0x00433240..0x00433298, 0x59 bytes incl. ret)
// Bytes (byte-verified in original\MASHED.exe.unpatched 2026-06-25,
//        file offset 0x33240 = RVA - 0x400000):
//   A1 A4 EC 67 00            mov  eax,[0x0067eca4]        ; one-shot gate
//   85 C0                     test eax,eax
//   75 4F                     jne  0x00433298              ; gate set -> ret
//   C7 05 A4 EC 67 00 01..    mov  dword [0x0067eca4],1    ; latch
//   C7 05 B0 EC 67 00 21..    mov  dword [0x0067ecb0],0x21
//   E8 1E FD FC FF            call 0x00402f80
//   E8 39 A1 FF FF            call 0x0042d3a0
//   8B 44 24 04              mov  eax,[esp+4]             ; param_1 (cdecl)
//   C7 05 44 E8 67 00 00..    mov  dword [0x0067e844],0
//   C7 05 F8 E9 67 00 00..    mov  dword [0x0067e9f8],0
//   C7 05 14 E9 67 00 00..    mov  dword [0x0067e914],0
//   A3 AC EC 67 00            mov  [0x0067ecac],eax        ; = param_1
//   C7 05 84 EA 67 00 01..    mov  dword [0x0067ea84],1
//   C3                        ret                          ; plain ret = __cdecl
// Signature: void __cdecl FUN_00433240(undefined4 param_1)
//
// Mechanical behavior (cited from body above): one-shot gate on
// DAT_0067eca4. When the gate is open (== 0) the body latches the gate,
// seeds DAT_0067ecb0 = 0x21, runs the two no-arg initializers FUN_00402f80
// (const-init 0x00636ae8..0x00636af4) and FUN_0042d3a0 (zero-fill
// 0x0067ed78..0x0067f0bc), clears three state words, stores param_1 into
// DAT_0067ecac, and sets DAT_0067ea84 = 1.
//
// Constants / globals (cited from function body at 0x00433240):
//   0x0067eca4 — one-shot gate flag        (0x00433240 / 0x00433249)
//   0x0067ecb0 — mode/state seed = 0x21    (0x00433253)
//   0x00402f80 — initializer callee        (0x0043325d)
//   0x0042d3a0 — initializer callee        (0x00433262)
//   0x0067e844 — cleared                   (0x0043326b)
//   0x0067e9f8 — cleared                   (0x00433275)
//   0x0067e914 — cleared                   (0x0043327f)
//   0x0067ecac — stores param_1            (0x00433289)
//   0x0067ea84 — set to 1                  (0x0043328e)
//
// Callers: FUN_00403250, FUN_00492770.
//
// Uncertainties (non-blocking, data-semantic only):
//   U-4255: param_1 type/semantics (stored into 0x0067ecac).
// ---------------------------------------------------------------------------

// 0x00433240
extern "C" __declspec(dllexport) void __cdecl RaceArmReset(std::uint32_t param_1) {
    // one-shot gate at 0x0067eca4 cited at 0x00433240 body.
    if (*reinterpret_cast<const std::uint32_t*>(0x0067eca4u) != 0u) {
        return;
    }
    *reinterpret_cast<std::uint32_t*>(0x0067eca4u) = 1u;        // 0x00433249
    *reinterpret_cast<std::uint32_t*>(0x0067ecb0u) = 0x21u;     // 0x00433253
    reinterpret_cast<void(__cdecl*)()>(0x00402f80u)();          // 0x0043325d
    reinterpret_cast<void(__cdecl*)()>(0x0042d3a0u)();          // 0x00433262
    *reinterpret_cast<std::uint32_t*>(0x0067e844u) = 0u;        // 0x0043326b
    *reinterpret_cast<std::uint32_t*>(0x0067e9f8u) = 0u;        // 0x00433275
    *reinterpret_cast<std::uint32_t*>(0x0067e914u) = 0u;        // 0x0043327f
    *reinterpret_cast<std::uint32_t*>(0x0067ecacu) = param_1;   // 0x00433289
    *reinterpret_cast<std::uint32_t*>(0x0067ea84u) = 1u;        // 0x0043328e
}

RH_ScopedInstall(RaceArmReset, 0x00433240);
