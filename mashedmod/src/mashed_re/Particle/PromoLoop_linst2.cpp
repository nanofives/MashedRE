// Mashed RE — promote-round lane L-INST, round 2 (installer-caller clause).
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

namespace {

// 0x0049cb00 FUN_0049cb00 (C2): __thiscall, ECX = object. If object+0xc0 is
// non-zero: call [0x005cc2bc](it), then object+0xc0 = 0. Returns nothing used.
typedef void(__thiscall* Fn0049cb00)(void* self);
const Fn0049cb00 s_fn0049cb00 = reinterpret_cast<Fn0049cb00>(0x0049cb00);

// 0x004a1570 FUN_004a1570 (C2): __stdcall, 5 args (callee cleans; no add esp
// after the call at 0x0049e166). Single call site in the image.
typedef int(__stdcall* Fn004a1570)(std::uint32_t, std::uint32_t, std::uint32_t,
                                   std::uint32_t, std::uint32_t);
const Fn004a1570 s_fn004a1570 = reinterpret_cast<Fn004a1570>(0x004a1570);

// 0x004a11c0 FUN_004a11c0 (C2): __stdcall (ret 0xc) bounded 16-bit copy
// (dst, src, n): copies units until a 0 unit or n-1 units, then terminates
// (0x004a11c0..0x004a11f4).
typedef void*(__stdcall* Fn004a11c0)(void* dst, const void* src, std::uint32_t n);
const Fn004a11c0 s_fn004a11c0 = reinterpret_cast<Fn004a11c0>(0x004a11c0);

// One-argument __stdcall COM-style slot (object pushed, ECX not set).
typedef std::uint32_t(__stdcall* ComSlot1Fn)(void* obj);

}  // namespace

// ─────────────────────────────────────────────────────────────────────────────
// ComCs94Field6cNonZero  --  0x0049c900 (41 bytes, 0x0049c900..0x0049c928)
//
//   0x0049c902  mov esi,ecx                  ; this = ECX
//   0x0049c904  lea edi,[esi+0x94]
//   0x0049c90b  call [0x005cc068]            ; EnterCriticalSection(this+0x94)
//   0x0049c911  mov ecx,[esi+0x6c] / setne   ; r = (this+0x6c != 0)
//   0x0049c91e  call [0x005cc06c]            ; LeaveCriticalSection(this+0x94)
//   0x0049c925  mov eax,esi                  ; return r
//
// Installer rows: slot 40 (+0xa0) of vtables 0x005cf9e8 / 0x005cfd70 /
// 0x005d0dc0 / 0x005d0eb0 (store_vptr sites 0x00493c1c, 0x00493c60, 0x00494adc,
// 0x00494b20, 0x0049dda4, 0x0049deae, 0x0049ebc0, 0x0049ec32), boundary resolved.
// ─────────────────────────────────────────────────────────────────────────────
#pragma comment(linker, "/export:ComCs94Field6cNonZero=@ComCs94Field6cNonZero@4")
// 0x0049c900
extern "C" __declspec(dllexport) int __fastcall ComCs94Field6cNonZero(std::uint8_t* self) {
    CRITICAL_SECTION* cs = reinterpret_cast<CRITICAL_SECTION*>(self + 0x94);  // 0x0049c904
    EnterCriticalSection(cs);                                                // 0x0049c90b
    const int r = *reinterpret_cast<std::uint32_t*>(self + 0x6c) != 0;      // 0x0049c911
    LeaveCriticalSection(cs);                                                // 0x0049c91e
    return r;                                                                // 0x0049c925
}
RH_ScopedInstall(ComCs94Field6cNonZero, 0x0049c900);

// ─────────────────────────────────────────────────────────────────────────────
// ComCb00ThenClear4Fields  --  0x0049ea70 (54 bytes, 0x0049ea70..0x0049eaa5)
//
//   0x0049ea72  mov esi,ecx
//   0x0049ea74  call 0x0049cb00              ; ECX = this
//   0x0049ea79  lea edi,[esi+0x94]
//   0x0049ea80  call [0x005cc068]            ; EnterCriticalSection(this+0x94)
//   0x0049ea89  this+0x70 = this+0x74 = this+0xb8 = this+0xbc = 0
//   0x0049ea9b  call [0x005cc06c]            ; LeaveCriticalSection(this+0x94)
//   0x0049eaa2  xor eax,eax                  ; return 0
//
// Installer rows: slot 25 (+0x64) of vtables 0x005cf9e8 / 0x005cfd70 /
// 0x005d0dc0 / 0x005d0eb0 (same eight store_vptr sites), boundary resolved.
// ─────────────────────────────────────────────────────────────────────────────
#pragma comment(linker, "/export:ComCb00ThenClear4Fields=@ComCb00ThenClear4Fields@4")
// 0x0049ea70
extern "C" __declspec(dllexport) int __fastcall ComCb00ThenClear4Fields(std::uint8_t* self) {
    s_fn0049cb00(self);                                                      // 0x0049ea74
    CRITICAL_SECTION* cs = reinterpret_cast<CRITICAL_SECTION*>(self + 0x94);  // 0x0049ea79
    EnterCriticalSection(cs);                                                // 0x0049ea80
    *reinterpret_cast<std::uint32_t*>(self + 0x70) = 0;                     // 0x0049ea89
    *reinterpret_cast<std::uint32_t*>(self + 0x74) = 0;                     // 0x0049ea8c
    *reinterpret_cast<std::uint32_t*>(self + 0xb8) = 0;                     // 0x0049ea8f
    *reinterpret_cast<std::uint32_t*>(self + 0xbc) = 0;                     // 0x0049ea95
    LeaveCriticalSection(cs);                                                // 0x0049ea9b
    return 0;                                                                // 0x0049eaa2
}
RH_ScopedInstall(ComCb00ThenClear4Fields, 0x0049ea70);

// ─────────────────────────────────────────────────────────────────────────────
// ComWait1570StoreField8  --  0x0049e140 (75 bytes, 0x0049e140..0x0049e18c)
//
//   __stdcall (ret 0xc), args p1, p2, p3.
//   0x0049e141  mov edi,[esp+0x10]           ; p3
//   0x0049e145  test edi,edi / jne           ; p3 == NULL -> return 0x80004003
//   0x0049e15b  mov eax,[esi+0x50]           ; esi = p1
//   0x0049e15e  push 0,0,0,p2,[p1+0x50]
//   0x0049e166  call 0x004a1570              ; r
//   0x0049e16b  cmp eax,0x102 / jne
//   0x0049e172  *p3 = [p1+8]; return 0x00040237   (r == 0x102)
//   0x0049e181  *p3 = [p1+8]; return 0            (otherwise)
//
// Installer rows (fn_table, store_mem): slot 7 (+0x1c) of table 0x005cfd30
// (sites 0x00493c22, 0x00493c66, 0x00494ae2, 0x00494b26, 0x0049ebc6, 0x0049ec38)
// and of table 0x005d0d84 (sites 0x0049ddaa, 0x0049deb4), boundary resolved.
// ─────────────────────────────────────────────────────────────────────────────
// 0x0049e140
extern "C" __declspec(dllexport) std::uint32_t __stdcall ComWait1570StoreField8(
        std::uint8_t* p1, std::uint32_t p2, std::uint32_t* p3) {
    if (p3 == nullptr)                                                      // 0x0049e145
        return 0x80004003u;                                                 // 0x0049e149
    const int r = s_fn004a1570(*reinterpret_cast<std::uint32_t*>(p1 + 0x50),
                               p2, 0, 0, 0);                                 // 0x0049e166
    *p3 = *reinterpret_cast<std::uint32_t*>(p1 + 8);                       // 0x0049e172 / 0x0049e181
    return r == 0x102 ? 0x00040237u : 0u;                                  // 0x0049e16b
}
RH_ScopedInstall(ComWait1570StoreField8, 0x0049e140);

// ─────────────────────────────────────────────────────────────────────────────
// ComCopyName30Ref34  --  0x0049ef10 (76 bytes, 0x0049ef10..0x0049ef5d)
//
//   __stdcall (ret 8), args p1, p2.
//   0x0049ef15  p2 == NULL -> return 0x80004003
//   0x0049ef27  src = [p1+0x30]
//   0x0049ef2c  src != 0 -> FUN_004a11c0(p2, src, 0x80)   (0x0049ef2e..0x0049ef35)
//               src == 0 -> word [p2] = 0                  (0x0049ef3c)
//   0x0049ef41  [p2+0x100] = [p1+0x34]
//   0x0049ef4a  obj = [p1+0x34]; obj != 0 -> obj->vtbl[+4](obj), pushed, no ECX
//   0x0049ef58  return 0
//
// Installer rows (fn_table, store_mem): slot 12 (+0x30) of table 0x005cfd30
// (sites 0x00493c22, 0x00493c66, 0x00494ae2, 0x00494b26, 0x0049ebc6, 0x0049ec38)
// and of table 0x005d0d84 (sites 0x0049ddaa, 0x0049deb4), boundary resolved.
// ─────────────────────────────────────────────────────────────────────────────
// 0x0049ef10
extern "C" __declspec(dllexport) std::uint32_t __stdcall ComCopyName30Ref34(
        std::uint8_t* p1, std::uint8_t* p2) {
    if (p2 == nullptr)                                                      // 0x0049ef15
        return 0x80004003u;                                                 // 0x0049ef19
    const void* src = *reinterpret_cast<void**>(p1 + 0x30);                 // 0x0049ef27
    if (src != nullptr)
        s_fn004a11c0(p2, src, 0x80);                                        // 0x0049ef35
    else
        *reinterpret_cast<std::uint16_t*>(p2) = 0;                          // 0x0049ef3c
    void* obj = *reinterpret_cast<void**>(p1 + 0x34);                       // 0x0049ef41
    *reinterpret_cast<void**>(p2 + 0x100) = obj;                            // 0x0049ef44
    if (obj != nullptr) {                                                   // 0x0049ef4d
        void** vtbl = *reinterpret_cast<void***>(obj);                      // 0x0049ef52
        reinterpret_cast<ComSlot1Fn>(vtbl[4 / 4])(obj);                     // 0x0049ef55
    }
    return 0;                                                               // 0x0049ef58
}
RH_ScopedInstall(ComCopyName30Ref34, 0x0049ef10);

// ─────────────────────────────────────────────────────────────────────────────
// ComInfo1cRefCopyName8  --  0x0049f950 (108 bytes, 0x0049f950..0x0049f9bd)
//
//   __stdcall (ret 8), args p1, p2.
//   0x0049f955  p2 == NULL -> return 0x80004003
//   0x0049f967  e = [p1+0x1c]; [p2] = e ? e+0xc : 0       (0x0049f96a..0x0049f975)
//   0x0049f977  e != 0 -> (e+0xc)->vtbl[+4](e+0xc), pushed, no ECX (0x0049f97e..0x0049f985)
//   0x0049f988  src = [p1+8]
//               src != 0 -> FUN_004a11c0(p2+8, src, 0x80)  (0x0049f98f..0x0049f999)
//               src == 0 -> word [p2+8] = 0                (0x0049f9ab)
//   0x0049f99e  [p2+4] = [p1+0x10]; return 0
//
// Installer row (fn_table, store_mem): slot 8 (+0x20) of table 0x005d0d00, site
// 0x0049ccad in FUN_0049cc80 (C2), boundary resolved.
// ─────────────────────────────────────────────────────────────────────────────
// 0x0049f950
extern "C" __declspec(dllexport) std::uint32_t __stdcall ComInfo1cRefCopyName8(
        std::uint8_t* p1, std::uint8_t* p2) {
    if (p2 == nullptr)                                                      // 0x0049f955
        return 0x80004003u;                                                 // 0x0049f959
    std::uint8_t* e = *reinterpret_cast<std::uint8_t**>(p1 + 0x1c);         // 0x0049f967
    *reinterpret_cast<void**>(p2) = e ? e + 0xc : nullptr;                  // 0x0049f975
    if (e != nullptr) {                                                     // 0x0049f97a
        void* obj = e + 0xc;                                                // 0x0049f981
        void** vtbl = *reinterpret_cast<void***>(obj);                      // 0x0049f97e
        reinterpret_cast<ComSlot1Fn>(vtbl[4 / 4])(obj);                     // 0x0049f985
    }
    const void* src = *reinterpret_cast<void**>(p1 + 8);                    // 0x0049f988
    if (src != nullptr)
        s_fn004a11c0(p2 + 8, src, 0x80);                                    // 0x0049f999
    else
        *reinterpret_cast<std::uint16_t*>(p2 + 8) = 0;                      // 0x0049f9ab
    *reinterpret_cast<std::uint32_t*>(p2 + 4) =
        *reinterpret_cast<std::uint32_t*>(p1 + 0x10);                       // 0x0049f99e / 0x0049f9b1
    return 0;                                                               // 0x0049f9a5
}
RH_ScopedInstall(ComInfo1cRefCopyName8, 0x0049f950);
