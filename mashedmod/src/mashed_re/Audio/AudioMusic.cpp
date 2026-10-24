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
// audiolink 2026-10-23: the two FLD m32 / FSTP m32 pairs (0x005baf04 / 0x005baf0e, 0x005baf16 / 0x005baf1f) are
// copies through the x87 register, and FLD m32 quiets a signaling NaN (sets mantissa bit 22) with the invalid
// exception masked. Every other float32 value (denormals included) comes back bit-identical, whatever the
// precision control. The port copies the bits and applies that one change.
static std::uint32_t X87CopyF32(std::uint32_t b) {
    if ((b & 0x7f800000u) == 0x7f800000u && (b & 0x007fffffu) != 0u) b |= 0x00400000u;
    return b;
}

extern "C" __declspec(dllexport) void __cdecl MusicGroupVolumeSet(
        int* param_1, float param_2) {
    // Write volume to struct+0x38 (FLD [esp+8] / FSTP [edx+0x38]).
    std::uint32_t vol_bits;
    std::memcpy(&vol_bits, &param_2, sizeof(vol_bits));
    *reinterpret_cast<std::uint32_t*>(reinterpret_cast<char*>(param_1) + 0x38) = X87CopyF32(vol_bits);

    // Walk circular linked list at param_1+0xc.
    // Sentinel: pointer to param_1+0xc itself.
    auto* const sentinel = reinterpret_cast<int**>(
            reinterpret_cast<char*>(param_1) + 0x0c);
    auto* node = reinterpret_cast<int**>(*sentinel);

    while (node != sentinel) {
        // FLD+FSTP of node+0x1c: the value comes back unchanged except a signaling NaN is quieted (U-2212).
        volatile std::uint32_t* p1c = reinterpret_cast<volatile std::uint32_t*>(
                reinterpret_cast<char*>(node) + 0x1c);
        const std::uint32_t b1c = *p1c;

        // Set bit 6 of node+0x14 (dirty flag — U-2213).
        auto* flags_ptr = reinterpret_cast<unsigned int*>(
                reinterpret_cast<char*>(node) + 0x14);
        const std::uint32_t fl = *flags_ptr | 0x40u;   // 0x005baf19 / 0x005baf1c
        *p1c = X87CopyF32(b1c);                         // 0x005baf1f
        *flags_ptr = fl;                                // 0x005baf22

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
