// Mashed RE - Audio music group utilities.
// All functions are from the audio_music_* clusters (Ghidra analysis notes).
//
// Source RVAs and asm annotations are cited per function below.
#include "../Core/HookSystem.h"
#include <cstdint>
#include <cstring>

// 0x005baf00  FUN_005baf00  (62 bytes)
// Music channel group volume setter.
//
// Signature (listing-verified): void FUN_005baf00(ptr param_1, float param_2)
//
// ASM key (all offsets listing-verified):
//   005baf04: FLD float ptr [ESP+0x8]      ; load param_2 (float)
//   005baf08: MOV EAX,[EDX+0xc]            ; EAX = circular list head
//   005baf0b: LEA ECX,[EDX+0xc]            ; ECX = sentinel = &param_1+0xc
//   005baf0e: FSTP float ptr [EDX+0x38]    ; param_1+0x38 = param_2 (volume)
//   ; loop: 005baf16..005baf2a
//   005baf16: FLD [EAX+0x1c]               ; FP load (no-op; compiler artifact U-2212)
//   005baf19: MOV EDI,[EAX+0x14]           ; EDI = node->flags
//   005baf1c: OR  EDI,0x40                  ; set bit 6 (dirty flag)
//   005baf1f: FSTP [EAX+0x1c]              ; store back (value unchanged)
//   005baf22: MOV [EAX+0x14],EDI           ; write flags back
//   005baf25: MOV EAX,[EAX]                ; EAX = node->next
//   005baf28: CMP EAX,ECX                  ; while EAX != sentinel
//   005baf2a: JNZ loop
//   ; after loop:
//   005baf2c: MOV EAX,[EDX+0x11c]          ; secondary object ptr
//   005baf32: TEST EAX,EAX                  ; if non-zero:
//   005baf34: JZ  exit
//   005baf36: MOV ECX,[ESP+0x8]            ; param_2 as raw dword
//   005baf3a: MOV [EAX+0x30],ECX           ; secondary+0x30 = param_2 raw bits
//   exit: RET
//
// Uncertainties (in ## Uncertainties of analysis note — not inline gates):
//   U-2212: FLD+FSTP of node+0x1c is a no-op (listing-confirmed); modelled here
//           as a volatile touch which preserves value identity.
//   U-2213: node+0x14 bit-6 semantics are unresolved; bit is set per listing.
//   U-2214: secondary ptr at param_1+0x11c semantics unresolved; propagation modelled.
extern "C" __declspec(dllexport) void __cdecl MusicGroupVolumeSet(
        int* param_1, float param_2) {
    // Write volume to struct+0x38.
    // param_1 is treated as int* for offset arithmetic; offset 0x38 / 4 = 14.
    *reinterpret_cast<float*>(reinterpret_cast<char*>(param_1) + 0x38) = param_2;

    // Walk circular linked list at param_1+0xc.
    // Sentinel: pointer to param_1+0xc itself.
    auto* const sentinel = reinterpret_cast<int**>(
            reinterpret_cast<char*>(param_1) + 0x0c);
    auto* node = reinterpret_cast<int**>(*sentinel);

    while (node != sentinel) {
        // Volatile FLD+FSTP of node+0x1c (compiler artifact; value unchanged — U-2212).
        volatile float* fp1c = reinterpret_cast<volatile float*>(
                reinterpret_cast<char*>(node) + 0x1c);
        volatile float tmp = *fp1c;
        *fp1c = tmp;

        // Set bit 6 of node+0x14 (dirty flag — U-2213).
        auto* flags_ptr = reinterpret_cast<unsigned int*>(
                reinterpret_cast<char*>(node) + 0x14);
        *flags_ptr |= 0x40u;

        // Advance: node = *node (first dword is next pointer).
        node = reinterpret_cast<int**>(*reinterpret_cast<void**>(node));
    }

    // Propagate volume to secondary object if present.
    // Reads param_1+0x11c; if non-zero, copies param_2 raw bits to secondary+0x30.
    auto* const secondary = reinterpret_cast<char*>(
            *reinterpret_cast<void**>(
                reinterpret_cast<char*>(param_1) + 0x11c));
    if (secondary) {
        // Propagate param_2 raw bits (not re-loaded via FPU — verified as MOV at 0x005baf36).
        std::uint32_t raw_bits;
        static_assert(sizeof(raw_bits) == sizeof(param_2), "float must be 32-bit");
        std::memcpy(&raw_bits, &param_2, sizeof(raw_bits));
        *reinterpret_cast<std::uint32_t*>(secondary + 0x30) = raw_bits;
    }
}

RH_ScopedInstall(MusicGroupVolumeSet, 0x005baf00);  // re-enabled 2026-05-24 phase-a1 music_vol_set GREEN
