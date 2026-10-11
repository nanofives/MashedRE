// Mashed RE — promote-round lane L-INST, round 3 (installer-caller clause).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Every function here is reached only through a vtable or function-table slot.
// Its caller half of the C2->C3 gate is the installer row in
// re/analysis/pointer_callers_2026-10-11.tsv (re/CONFIDENCE.md "Installer-caller
// clause"). An installer names the table. It does not call the function.
//
// Bodies byte-verified with re/tools/disasm_fn.py against
// original\MASHED.exe.unpatched on 2026-10-11. Import slots:
//   0x005cc068 EnterCriticalSection   0x005cc06c LeaveCriticalSection
//
// ASI-only TU: the ports call original-image code at fixed VAs.

#include "../Core/HookSystem.h"

#include <windows.h>
#include <cstdint>
#include <cstring>

namespace {

// Direct callees (all C2 unless noted), at their original-image VAs.
// 0x0049ecc0: __stdcall, 3 args; reached from 0x0049c5d0 by a tail jmp (0x0049c600),
//             the only reference to it in .text.
typedef int(__stdcall* Fn0049ecc0)(void* p1, const void* p2, std::uint32_t p3);
const Fn0049ecc0 s_fn0049ecc0 = reinterpret_cast<Fn0049ecc0>(0x0049ecc0);
// 0x0049ff30 / 0x0049ff60: __stdcall, 1 arg (pushed, no stack adjustment after).
typedef int(__stdcall* FnStd1)(void* p1);
const FnStd1 s_fn0049ff30 = reinterpret_cast<FnStd1>(0x0049ff30);
const FnStd1 s_fn0049ff60 = reinterpret_cast<FnStd1>(0x0049ff60);
// 0x0049cc40: __thiscall(this, v), ret 4: under CS this+0x94, this+0xb0 = v.
typedef void(__thiscall* Fn0049cc40)(void* self, std::uint32_t v);
const Fn0049cc40 s_fn0049cc40 = reinterpret_cast<Fn0049cc40>(0x0049cc40);

typedef int(__thiscall* VcallFn)(void* self);
typedef int(__thiscall* Vcall2Fn)(void* self, const void* a, std::uint32_t b);
typedef std::uint32_t(__stdcall* ComSlot1Fn)(void* obj);

inline void** Vtbl(const void* obj) { return *reinterpret_cast<void** const*>(obj); }

// Enter on construction, Leave on destruction. The two SEH-framed bodies below
// (handler table 0x005ca370, state byte at [esp+0x20]/[esp+0x1c]) release their
// sections on unwind. A scope guard gives the same release order on both the
// normal and the exceptional path.
struct CsLock {
    CRITICAL_SECTION* cs;
    explicit CsLock(void* p) : cs(static_cast<CRITICAL_SECTION*>(p)) { EnterCriticalSection(cs); }
    ~CsLock() { LeaveCriticalSection(cs); }
    CsLock(const CsLock&) = delete;
    CsLock& operator=(const CsLock&) = delete;
};

// The two 16-byte constants 0x0049c5d0 compares against (repe cmpsd, 4 dwords),
// read from original\MASHED.exe.unpatched .rdata:
//   0x005d0bbc: b2 68 a8 56 d4 0a ce 11 b0 3a 00 20 af 0b a7 70
//   0x005d0afc: 80 38 b7 36 c8 c2 cf 11 8b 46 00 80 5f 6c ef 60
const std::uint8_t kConst5d0bbc[16] = {0xb2, 0x68, 0xa8, 0x56, 0xd4, 0x0a, 0xce, 0x11,
                                       0xb0, 0x3a, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70};
const std::uint8_t kConst5d0afc[16] = {0x80, 0x38, 0xb7, 0x36, 0xc8, 0xc2, 0xcf, 0x11,
                                       0x8b, 0x46, 0x00, 0x80, 0x5f, 0x6c, 0xef, 0x60};

}  // namespace

// 0x0049c690 ComField14Get (C3, Particle/ParticleLeaves_ad4.cpp): returns this+0x14.
extern "C" std::uint32_t __fastcall ComField14Get(const std::uint8_t* thisptr);

// ─────────────────────────────────────────────────────────────────────────────
// ComQiTwoConstsV24  --  0x0049c5d0 (73 bytes, 0x0049c5d0..0x0049c618)
//
//   __stdcall p1, p2, p3 (ret 0xc).
//   0x0049c5d6  repe cmpsd p2 vs 0x005d0bbc (4 dwords) -> equal: 0x0049c605
//   0x0049c5e8  repe cmpsd p2 vs 0x005d0afc (4 dwords) -> equal: 0x0049c605
//   0x0049c5fa  neither: jmp 0x0049ecc0 with the caller's (p1, p2, p3) in place
//   0x0049c605  ecx = p1; push p3; push p2; call [vtbl(p1)+0x24]   (__thiscall)
//               its EAX is the return
//
// Installer rows: slot 0 (+0x0) of vtable 0x005d0dc0, store_vptr sites 0x0049dda4
// (FUN_0049dd60) and 0x0049deae (FUN_0049de90), boundary resolved.
// ─────────────────────────────────────────────────────────────────────────────
// 0x0049c5d0
extern "C" __declspec(dllexport) int __stdcall ComQiTwoConstsV24(
        void* p1, const std::uint8_t* p2, std::uint32_t p3) {
    if (std::memcmp(p2, kConst5d0bbc, 16) == 0 ||                    // 0x0049c5e4
        std::memcmp(p2, kConst5d0afc, 16) == 0)                       // 0x0049c5f6
        return reinterpret_cast<Vcall2Fn>(Vtbl(p1)[0x24 / 4])(p1, p2, p3);  // 0x0049c611
    return s_fn0049ecc0(p1, p2, p3);                                  // 0x0049c600
}
RH_ScopedInstall(ComQiTwoConstsV24, 0x0049c5d0);

// ─────────────────────────────────────────────────────────────────────────────
// ComLockedFf30V84V64  --  0x0049cd70 (158 bytes, 0x0049cd70..0x0049ce0c)
//
//   __stdcall p1 (ret 4). inner = [p1+0xcc], re-read before every use.
//   0x0049cd94  EnterCriticalSection(inner+0x7c)
//   0x0049cda4  EnterCriticalSection(inner+0x94)
//   0x0049cdc5  FUN_0049ff30(p1)                       (__stdcall, result unused)
//   0x0049cdd2  inner->vtbl[+0x84]()                   (__thiscall, result unused)
//   0x0049cde4  LeaveCriticalSection(inner+0x94)
//   0x0049cdee  r = inner->vtbl[+0x64]()               (__thiscall)
//   0x0049cdf4  LeaveCriticalSection(inner+0x7c); return r
//
// Installer row: slot 15 (+0x3c) of function table 0x005d0d00, store_mem site
// 0x0049ccad in FUN_0049cc80 (C2), boundary resolved.
// ─────────────────────────────────────────────────────────────────────────────
// 0x0049cd70
extern "C" __declspec(dllexport) int __stdcall ComLockedFf30V84V64(std::uint8_t* p1) {
    CsLock outer(*reinterpret_cast<std::uint8_t**>(p1 + 0xcc) + 0x7c);      // 0x0049cd94
    {
        CsLock in(*reinterpret_cast<std::uint8_t**>(p1 + 0xcc) + 0x94);     // 0x0049cda4
        s_fn0049ff30(p1);                                                  // 0x0049cdc5
        void* o = *reinterpret_cast<void**>(p1 + 0xcc);                    // 0x0049cdca
        reinterpret_cast<VcallFn>(Vtbl(o)[0x84 / 4])(o);                   // 0x0049cdd2
    }                                                                      // 0x0049cde4
    void* o = *reinterpret_cast<void**>(p1 + 0xcc);                        // 0x0049cde6
    return reinterpret_cast<VcallFn>(Vtbl(o)[0x64 / 4])(o);                // 0x0049cdee
}                                                                          // 0x0049cdf4
RH_ScopedInstall(ComLockedFf30V84V64, 0x0049cd70);

// ─────────────────────────────────────────────────────────────────────────────
// ComLockedV88ThenFf60  --  0x0049ce10 (146 bytes, 0x0049ce10..0x0049cea2)
//
//   __stdcall p1 (ret 4). inner = [p1+0xcc], re-read before every use.
//   0x0049ce33  EnterCriticalSection(inner+0x7c)
//   0x0049ce44  EnterCriticalSection(inner+0x94)
//   0x0049ce6c  r = inner->vtbl[+0x88]()               (__thiscall)
//   0x0049ce74  r >= 0 -> r = FUN_0049ff60(p1)         (__stdcall)
//   0x0049ce87  LeaveCriticalSection(inner+0x94)
//   0x0049ce8a  LeaveCriticalSection(inner+0x7c); return r
//
// Installer row: slot 16 (+0x40) of function table 0x005d0d00, store_mem site
// 0x0049ccad in FUN_0049cc80 (C2), boundary resolved.
// ─────────────────────────────────────────────────────────────────────────────
// 0x0049ce10
extern "C" __declspec(dllexport) int __stdcall ComLockedV88ThenFf60(std::uint8_t* p1) {
    CsLock outer(*reinterpret_cast<std::uint8_t**>(p1 + 0xcc) + 0x7c);      // 0x0049ce33
    CsLock in(*reinterpret_cast<std::uint8_t**>(p1 + 0xcc) + 0x94);         // 0x0049ce44
    void* o = *reinterpret_cast<void**>(p1 + 0xcc);                        // 0x0049ce5f
    int r = reinterpret_cast<VcallFn>(Vtbl(o)[0x88 / 4])(o);               // 0x0049ce6c
    if (r >= 0)                                                            // 0x0049ce76
        r = s_fn0049ff60(p1);                                              // 0x0049ce79
    return r;                                                              // 0x0049ce87..0x0049ce8a
}
RH_ScopedInstall(ComLockedV88ThenFf60, 0x0049ce10);

// ─────────────────────────────────────────────────────────────────────────────
// ComState14V7cSetB0  --  0x0049e600 (55 bytes, 0x0049e600..0x0049e638)
//
//   __thiscall this, one stack arg that the body never reads (ret 4).
//   0x0049e603  this+0x60 = 0
//   0x0049e60a  s = FUN_0049c690(this)                 (= this+0x14)
//   0x0049e60f  s != 2 -> FUN_0049cc40(this, 1); return 0     (0x0049e62c)
//   0x0049e616  r = this->vtbl[+0x7c]()  (ECX still = this after 0x0049c690)
//   0x0049e61b  r < 0  -> return r
//   0x0049e621  FUN_0049cc40(this, 0); return 0
//
// Exported __fastcall (self, EDX unused, arg): ECX + one callee-cleaned stack
// dword, the same register/stack contract as the original __thiscall.
//
// Installer rows: slot 37 (+0x94) of vtables 0x005cf9e8 / 0x005cfd70 /
// 0x005d0dc0 / 0x005d0eb0 (store_vptr sites 0x00493c1c, 0x00493c60, 0x00494adc,
// 0x00494b20, 0x0049dda4, 0x0049deae, 0x0049ebc0, 0x0049ec32), boundary resolved.
// ─────────────────────────────────────────────────────────────────────────────
#pragma comment(linker, "/export:ComState14V7cSetB0=@ComState14V7cSetB0@12")
// 0x0049e600
extern "C" __declspec(dllexport) int __fastcall ComState14V7cSetB0(
        std::uint8_t* self, std::uint32_t /*edx*/, std::uint32_t /*unused*/) {
    *reinterpret_cast<std::uint32_t*>(self + 0x60) = 0;                    // 0x0049e603
    if (ComField14Get(self) != 2) {                                        // 0x0049e60a..0x0049e612
        s_fn0049cc40(self, 1);                                             // 0x0049e62e
        return 0;                                                          // 0x0049e633
    }
    const int r = reinterpret_cast<VcallFn>(Vtbl(self)[0x7c / 4])(self);   // 0x0049e616
    if (r < 0)                                                             // 0x0049e61b
        return r;
    s_fn0049cc40(self, 0);                                                 // 0x0049e621
    return 0;                                                              // 0x0049e626
}
RH_ScopedInstall(ComState14V7cSetB0, 0x0049e600);

// ─────────────────────────────────────────────────────────────────────────────
// ComReleaseAcGate78  --  0x0049e640 (117 bytes, 0x0049e640..0x0049e6b4)
//
//   __fastcall this.
//   0x0049e643  o = this+0xac; o -> o->vtbl[+8](o) (pushed, __stdcall), this+0xac = 0
//   0x0049e65d  a = this+0x78; [a+0x18] == 0 -> return 1
//   0x0049e66e  this+0x14 != 0 && byte [a+0x25] == 0 -> return 0x80040224
//   0x0049e683  FUN_0049cc40(this, 0)
//   0x0049e690  this->vtbl[+0x64](); this->vtbl[+0x70]()    (__thiscall)
//   0x0049e69a  s = this+0x14 (read after those calls); this+0x60 = 0
//   0x0049e6a5  s == 2 -> this->vtbl[+0x80]()
//   0x0049e6b1  return 0
//
// Installer rows: slot 35 (+0x8c) of vtables 0x005cf9e8 / 0x005cfd70 /
// 0x005d0dc0 / 0x005d0eb0 (same eight store_vptr sites), boundary resolved.
// ─────────────────────────────────────────────────────────────────────────────
#pragma comment(linker, "/export:ComReleaseAcGate78=@ComReleaseAcGate78@4")
// 0x0049e640
extern "C" __declspec(dllexport) int __fastcall ComReleaseAcGate78(std::uint8_t* self) {
    void* o = *reinterpret_cast<void**>(self + 0xac);                       // 0x0049e643
    if (o != nullptr) {
        reinterpret_cast<ComSlot1Fn>(Vtbl(o)[8 / 4])(o);                    // 0x0049e650
        *reinterpret_cast<std::uint32_t*>(self + 0xac) = 0;                 // 0x0049e653
    }
    const std::uint8_t* a = *reinterpret_cast<std::uint8_t**>(self + 0x78); // 0x0049e65d
    if (*reinterpret_cast<const std::uint32_t*>(a + 0x18) == 0)             // 0x0049e660
        return 1;                                                           // 0x0049e667
    if (*reinterpret_cast<std::uint32_t*>(self + 0x14) != 0 &&              // 0x0049e66e
        a[0x25] == 0)                                                       // 0x0049e675
        return static_cast<int>(0x80040224u);                               // 0x0049e67c
    s_fn0049cc40(self, 0);                                                  // 0x0049e687
    reinterpret_cast<VcallFn>(Vtbl(self)[0x64 / 4])(self);                  // 0x0049e690
    reinterpret_cast<VcallFn>(Vtbl(self)[0x70 / 4])(self);                  // 0x0049e697
    const std::uint32_t s = *reinterpret_cast<std::uint32_t*>(self + 0x14); // 0x0049e69a
    *reinterpret_cast<std::uint32_t*>(self + 0x60) = 0;                     // 0x0049e69e
    if (s == 2)                                                             // 0x0049e6a5
        reinterpret_cast<VcallFn>(Vtbl(self)[0x80 / 4])(self);              // 0x0049e6ab
    return 0;                                                               // 0x0049e6b1
}
RH_ScopedInstall(ComReleaseAcGate78, 0x0049e640);
