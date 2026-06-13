// Mashed RE — promote-round round 56 (one per-vehicle vec3 getter + three 2-index wheel getters).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// All four are __cdecl, self-contained (zero callees), byte-verified from the listing.
//
// FUN_0046bd20 (0x0046bd20): per-vehicle vec3 getter `u32 fn(out_ptr, idx)`.
//   idx [ESP+8], CMP 0x10 / JC ; out [ESP+4] ; IMUL 0xd04 ;
//   MOV [EAX+0x8820a0]->out[0], +0x8820a4->out[1], +0x8820a8->out[2] ; MOV EAX,1
//   (base 0x008820a0, n=3, stride 0xd04, bound 0x10) — same shape as round 54.
//
// FUN_0046d320/d360/bd60: 2-index wheel getters `u32 fn(out_ptr, i1, i2)`.
//   i1 [ESP+8], CMP 0x10 / JNC oob ; i2 [ESP+0xc], CMP 4 / JC body (else oob->0) ;
//   IMUL EAX,i1,0x11 ; ADD EAX,i2 ; out [ESP+4] ; IMUL EAX,0xc4 ;
//   MOV [EAX+base]->*out ; MOV EAX,1
//   Byte-verified base:  0046d320: 0x00881790   0046d360: 0x00881738   0046bd60: 0x00881744

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0046bd20 — vec3 getter (out, idx)
extern "C" __declspec(dllexport) std::uint32_t __cdecl VehTbl8820a0Get3(std::uint32_t* out, std::uint32_t idx) {
    if (idx > 0xfu) return 0;                                   // CMP 0x10 / JC ; XOR EAX,EAX
    std::uint32_t b = idx * 0xd04u;                             // IMUL 0xd04
    out[0] = *reinterpret_cast<std::uint32_t*>(0x008820a0u + b);
    out[1] = *reinterpret_cast<std::uint32_t*>(0x008820a4u + b);
    out[2] = *reinterpret_cast<std::uint32_t*>(0x008820a8u + b);
    return 1;                                                   // MOV EAX,1
}
RH_ScopedInstall(VehTbl8820a0Get3, 0x0046bd20);

// 2-index wheel getter: *out = *(u32*)(Base + (i1*0x11 + i2)*0xc4); i1>=0x10 || i2>=4 -> return 0.
template <std::uint32_t Base>
static inline std::uint32_t Idx2WheelGet(std::uint32_t* out, std::uint32_t i1, std::uint32_t i2) {
    if (i1 >= 0x10u || i2 >= 4u) return 0;                      // CMP 0x10/JNC ; CMP 4/JC
    *out = *reinterpret_cast<std::uint32_t*>(Base + (i1 * 0x11u + i2) * 0xc4u);  // IMUL 0x11 ; ADD ; IMUL 0xc4
    return 1;
}

// 0x0046d320
extern "C" __declspec(dllexport) std::uint32_t __cdecl Idx2Wheel881790Get(std::uint32_t* out, std::uint32_t i1, std::uint32_t i2) { return Idx2WheelGet<0x00881790u>(out, i1, i2); }
RH_ScopedInstall(Idx2Wheel881790Get, 0x0046d320);

// 0x0046d360
extern "C" __declspec(dllexport) std::uint32_t __cdecl Idx2Wheel881738Get(std::uint32_t* out, std::uint32_t i1, std::uint32_t i2) { return Idx2WheelGet<0x00881738u>(out, i1, i2); }
RH_ScopedInstall(Idx2Wheel881738Get, 0x0046d360);

// 0x0046bd60
extern "C" __declspec(dllexport) std::uint32_t __cdecl Idx2Wheel881744Get(std::uint32_t* out, std::uint32_t i1, std::uint32_t i2) { return Idx2WheelGet<0x00881744u>(out, i1, i2); }
RH_ScopedInstall(Idx2Wheel881744Get, 0x0046bd60);
