// Mashed RE — vehicle float field -> int out-param (0x0046cc10).
//
// Survivor of three screens plus hand-reading. Of the five race-gated `direct`
// candidates: 0x005b0f40 pops a live queue, 0x004e4320 stores through
// DAT_007d716c + arg (mutates the game), 0x0041f290 indexes unbounded and
// inherits a conditional side effect through 0x004c0ed0. This one is
// bounds-checked, deterministic, and writes only to the CALLER'S buffer.
//
// Transcribed from capstone disasm of original\MASHED.exe.unpatched.
// Binary anchor: MASHED.exe size=2,846,720
//   sha256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
#include "../Core/HookSystem.h"
#include <cstdint>

namespace {

// Per-vehicle record stride and the field this reads. 0x0d04 is the vehicle
// struct size (memory project_wsa_a1_vehicle_struct); the array base used here
// is 0x00881f80, which is DAT_008815a0 + 0x9e0, i.e. field +0x9e0 of record 0.
constexpr std::uintptr_t kFieldBase_881f80 = 0x00881f80u;
constexpr std::uint32_t  kVehicleStride    = 0x0d04u;
constexpr std::uint32_t  kMaxIndex         = 0x10u;

// 0x004a2c48 __ftol — MSVC's float->long helper. Takes its operand in ST0 and
// returns in EAX, which no C declaration can express, so this loads the float
// and TAIL-JUMPS into it: `push imm / ret` sets EIP without disturbing the
// return address, so __ftol returns straight to our caller with EAX set. Same
// shape as the existing ValidateHandle thunk in Physics/SmplFzxStateBlock.cpp.
//
// Calling the ORIGINAL rather than using a C cast matters: C truncation and
// __ftol's x87 rounding-mode-dependent conversion are not guaranteed to agree,
// and this project builds x87 without /arch:SSE2 precisely so such things stay
// bit-identical (memory project_wsa2_rwmath_bitident).
__declspec(naked) int __cdecl FtolFrom(const float* /*src*/) {
    __asm {
        mov eax, [esp+4]
        fld dword ptr [eax]
        push 0x004a2c48
        ret
    }
}

} // namespace

// ─────────────────────────────────────────────────────────────────────────────
// 0x0046cc10  VehicleFloatFieldAsInt(int* out, unsigned idx) -> 1 ok / 0 range
//
// Verbatim 0x0046cc10..0x0046cc38:
//   0046cc10 mov eax,[esp+8]              ; idx  (SECOND argument)
//   0046cc14 cmp eax,0x10
//   0046cc17 jb  0x46cc1c                 ; UNSIGNED compare
//   0046cc19 xor eax,eax / ret            ; out of range -> 0, *out UNTOUCHED
//   0046cc1c imul eax,eax,0xd04
//   0046cc22 fld dword ptr [eax+0x881f80]
//   0046cc28 call 0x4a2c48                ; __ftol -> EAX
//   0046cc2d mov ecx,[esp+4]              ; out
//   0046cc31 mov [ecx],eax
//   0046cc33 mov eax,1 / ret
//
// Note the out-of-range path leaves *out ALONE — it does not zero it. The diff
// poisons the out slot to 0xCCCCCCCC before each call, so that "no write" is
// observable rather than silently matching a stale value.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) int __cdecl VehicleFloatFieldAsInt(int* out,
                                                                    unsigned int idx) {
    if (idx >= kMaxIndex) return 0;
    const float* src = reinterpret_cast<const float*>(
        kFieldBase_881f80 + static_cast<std::uintptr_t>(idx) * kVehicleStride);
    *out = FtolFrom(src);
    return 1;
}
RH_ScopedInstall(VehicleFloatFieldAsInt, 0x0046cc10);
