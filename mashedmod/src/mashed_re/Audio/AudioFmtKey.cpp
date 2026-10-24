// Mashed RE — Audio/AudioFmtKey.cpp
// 0x005adf30 AudioFmtKeyCompare, split out of Audio/AudioRws.cpp (audiolink slice 4) so that Audio/RwsFmt.cpp can link
// into the exe without the rest of AudioRws.cpp. In both rsp files. Pure: no address, no callee.
//
// Bytes (verify/audiolink_20261023/disasm_slice4.txt): cmp esi,edi / jne (0x005adf3a, equal pointers -> 0); then
// mov ecx,0x10 / repe cmpsb (0x005adf43..0x005adf4a, unsigned byte compare, DF assumed clear); on a difference
// sbb eax,eax / sbb eax,-1 (0x005adf4e / 0x005adf50) = -1 when the first differing byte of param_1 is below, +1 above.
#include "../Core/HookSystem.h"
#include "AudioFmtKey.h"

// ---------------------------------------------------------------------------
// 0x005adf30  FUN_005adf30 -- 16-byte format descriptor lexicographic memcmp
//
// Signature: int FUN_005adf30(byte* param_1, byte* param_2)
// Compares two 16-byte audio format keys byte-by-byte.
// Returns:
//   0  if all 16 bytes are equal (or param_1 == param_2).
//  -1  if first differing byte: *param_1 < *param_2.
//  +1  if first differing byte: *param_1 > *param_2.
//
// Functionally equivalent to a sign-normalised memcmp(param_1, param_2, 16)
// returning exactly -1/0/+1 (not arbitrary negative/positive like memcmp).
//
// Body: 0x005adf30--0x005adf55 (0x26 bytes). No callees. Pure.
// ---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int __cdecl AudioFmtKeyCompare(
        const uint8_t* param_1, const uint8_t* param_2)
{
    // Pointer-identity short-circuit (matches 0x005adf30 entry test).
    if (param_1 == param_2) return 0;

    int count = 16;
    bool equal = true;
    bool less  = false;
    while (equal && count > 0) {
        // audiolink slice 4: repe cmpsb faults on the ES:[EDI] byte (param_2) before DS:[ESI] (param_1); measured with
        // two unmapped key pointers (PREREG_slice4.md amendment 1). Volatile loads keep that order.
        const std::uint8_t b = *static_cast<const volatile std::uint8_t*>(param_2);
        const std::uint8_t a = *static_cast<const volatile std::uint8_t*>(param_1);
        less  = (a < b);
        equal = (a == b);
        ++param_1;
        ++param_2;
        --count;
    }
    if (!equal) {
        // Reproduce original: (1 - (uint)bVar3) - (uint)(bVar3 != 0)
        // less==true  -> 1 - 1 - 1 = -1
        // less==false -> 1 - 0 - 0 = +1
        return less ? -1 : 1;
    }
    return 0;
}

RH_ScopedInstall(AudioFmtKeyCompare, 0x005adf30);  // re-enabled 2026-05-24 c3-audio-a
