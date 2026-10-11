// Mashed RE — promote-round lane L-INST, round 1 (installer-caller clause).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Every function here is reached only through a vtable slot. Its caller-half of
// the C2->C3 gate is the installer row in
// re/analysis/pointer_callers_2026-10-11.tsv (re/CONFIDENCE.md "Installer-caller
// clause"). An installer names the table the function sits in. It does NOT call
// the function, and nothing in this file claims it does.
//
// Bodies byte-verified with re/tools/disasm_fn.py against
// original\MASHED.exe.unpatched on 2026-10-11. Import slots resolved from the
// anchored PE's import table:
//   0x005cc068 EnterCriticalSection   0x005cc06c LeaveCriticalSection
//   0x005cc074 ResetEvent
//
// ASI-only TU: the ports call original-image code at fixed VAs (0x004a2310),
// which the standalone exe does not map.

#include "../Core/HookSystem.h"

#include <windows.h>
#include <cstdint>

namespace {

// 0x004a2310 FUN_004a2310 (C2): __thiscall, ECX = object. Bytes 0x004a2310..
// 0x004a233e: EnterCriticalSection(ECX+0x1c); +0x38/+0x3c/+0x40/+0x44 = 0;
// +0x48 = 1; LeaveCriticalSection(ECX+0x1c); return 0.
typedef int(__thiscall* Fn004a2310)(void* obj);
const Fn004a2310 s_fn004a2310 = reinterpret_cast<Fn004a2310>(0x004a2310);

typedef int(__thiscall* VcallArg1Fn)(void* self, int arg);
typedef int(__thiscall* VcallFn)(void* self);
typedef int(__stdcall* ComSlotFn)(void* obj, std::uint32_t arg);

}  // namespace

// 0x0049c800 Zero49c800 (C3, Util/PromoLoop_sessionB.cpp): naked, ECX = this,
// writes this+0x68 = 0.
extern "C" void __cdecl Zero49c800(void);

// ─────────────────────────────────────────────────────────────────────────────
// ComInner50Vcall28  --  0x0049c750 (27 bytes, 0x0049c750..0x0049c76b)
//
//   0x0049c751  mov esi,ecx                  ; this = ECX (__fastcall/__thiscall)
//   0x0049c753  mov ecx,[esi+0x50]
//   0x0049c756  test ecx,ecx / je 0x0049c75f
//   0x0049c75a  call 0x004a2310              ; ECX = this+0x50 (register arg the
//                                            ; decompiler shows as "no args")
//   0x0049c75f  mov eax,[esi]                ; vtable
//   0x0049c761  push 1 / mov ecx,esi
//   0x0049c765  call [eax+0x28]              ; slot 10, __thiscall(this, 1)
//   0x0049c768  xor eax,eax                  ; return 0
//
// Installer rows: slot 34 (+0x88) of tables 0x005cf9e8 / 0x005cfd70 /
// 0x005d0dc0 / 0x005d0eb0 (installer sites 0x00493c1c, 0x00493c60, 0x00494adc,
// 0x00494b20, 0x0049dda4, 0x0049deae, 0x0049ebc0, 0x0049ec32), boundary resolved.
// ─────────────────────────────────────────────────────────────────────────────
#pragma comment(linker, "/export:ComInner50Vcall28=@ComInner50Vcall28@4")
extern "C" __declspec(dllexport) int __fastcall ComInner50Vcall28(std::uint8_t* self) {
    void* inner = *reinterpret_cast<void**>(self + 0x50);           // 0x0049c753
    if (inner != nullptr)                                           // 0x0049c756
        s_fn004a2310(inner);                                        // 0x0049c75a
    void** vtbl = *reinterpret_cast<void***>(self);                 // 0x0049c75f
    reinterpret_cast<VcallArg1Fn>(vtbl[0x28 / 4])(self, 1);         // 0x0049c765
    return 0;                                                       // 0x0049c768
}
RH_ScopedInstall(ComInner50Vcall28, 0x0049c750);

// ─────────────────────────────────────────────────────────────────────────────
// ComInner50Vcall70  --  0x0049c770 (26 bytes, 0x0049c770..0x0049c789)
//
// Same as 0x0049c750 except the dispatch:
//   0x0049c77f  mov eax,[esi] / mov ecx,esi
//   0x0049c783  call [eax+0x70]              ; slot 28, __thiscall(this), no push
//   0x0049c786  xor eax,eax                  ; return 0
//
// Installer rows: slot 30 (+0x78) of tables 0x005cf9e8 / 0x005cfd70 /
// 0x005d0dc0 / 0x005d0eb0 (same eight installer sites), boundary resolved.
// ─────────────────────────────────────────────────────────────────────────────
#pragma comment(linker, "/export:ComInner50Vcall70=@ComInner50Vcall70@4")
extern "C" __declspec(dllexport) int __fastcall ComInner50Vcall70(std::uint8_t* self) {
    void* inner = *reinterpret_cast<void**>(self + 0x50);           // 0x0049c773
    if (inner != nullptr)                                           // 0x0049c776
        s_fn004a2310(inner);                                        // 0x0049c77a
    void** vtbl = *reinterpret_cast<void***>(self);                 // 0x0049c77f
    reinterpret_cast<VcallFn>(vtbl[0x70 / 4])(self);                // 0x0049c783
    return 0;                                                       // 0x0049c786
}
RH_ScopedInstall(ComInner50Vcall70, 0x0049c770);

// ─────────────────────────────────────────────────────────────────────────────
// ComSlot68DispatchReset  --  0x0049c810 (48 bytes, 0x0049c810..0x0049c83f)
//
//   0x0049c814  mov edi,[esi+0x68]           ; v = this+0x68
//   0x0049c817  test edi,edi / je 0x0049c82c
//   0x0049c81b  mov eax,[esi+0x18]           ; obj = this+0x18
//   0x0049c81e  mov ecx,[eax]
//   0x0049c820  push edi / push eax
//   0x0049c822  call [ecx+0x18]              ; obj->vtbl[6](obj, v), args on the
//                                            ; stack, no ECX setup (__stdcall)
//   0x0049c825  mov ecx,esi
//   0x0049c827  call 0x0049c800              ; this+0x68 = 0
//   0x0049c82c  push [esi+0x54]
//   0x0049c830  call [0x005cc074]            ; ResetEvent(this+0x54)
//   0x0049c836  xor eax,eax / test edi,edi / sete al   ; return v == 0
//
// The return tests the value read at 0x0049c814, before 0x0049c800 zeroes it.
//
// Installer rows: slot 27 (+0x6c) of tables 0x005cf9e8 / 0x005cfd70 /
// 0x005d0dc0 / 0x005d0eb0 (same eight installer sites), boundary resolved.
// ─────────────────────────────────────────────────────────────────────────────
#pragma comment(linker, "/export:ComSlot68DispatchReset=@ComSlot68DispatchReset@4")
extern "C" __declspec(dllexport) int __fastcall ComSlot68DispatchReset(std::uint8_t* self) {
    const std::uint32_t v = *reinterpret_cast<std::uint32_t*>(self + 0x68);   // 0x0049c814
    if (v != 0) {                                                             // 0x0049c817
        void* obj = *reinterpret_cast<void**>(self + 0x18);                   // 0x0049c81b
        void** vtbl = *reinterpret_cast<void***>(obj);                        // 0x0049c81e
        reinterpret_cast<ComSlotFn>(vtbl[0x18 / 4])(obj, v);                  // 0x0049c822
        reinterpret_cast<void(__fastcall*)(void*)>(&Zero49c800)(self);        // 0x0049c827
    }
    ResetEvent(*reinterpret_cast<HANDLE*>(self + 0x54));                      // 0x0049c830
    return v == 0 ? 1 : 0;                                                    // 0x0049c836
}
RH_ScopedInstall(ComSlot68DispatchReset, 0x0049c810);

// ─────────────────────────────────────────────────────────────────────────────
// ComInnerD8Vcall8cThen9fc80  --  0x0049ceb0 (31 bytes, 0x0049ceb0..0x0049cece)
//
//   0x0049ceb1  mov esi,ecx                  ; this = ECX
//   0x0049ceb3  mov ecx,[esi+0xd8]           ; inner = this+0xd8
//   0x0049ceb9  mov eax,[ecx]
//   0x0049cebb  call [eax+0x8c]              ; r = inner->vtbl[35](), __thiscall,
//                                            ; ECX = inner, no stack args
//   0x0049cec1  test eax,eax / jl 0x0049cecd ; r < 0 -> return r
//   0x0049cec5  mov ecx,esi / pop esi
//   0x0049cec8  jmp 0x0049fc80               ; tail call FUN_0049fc80(ECX = this),
//                                            ; its EAX is the return
//
// Installer row: slot 11 (+0x2c) of table 0x005d0d48, installer site 0x0049cca7
// in FUN_0049cc80 (C2), store_vptr, boundary resolved.
// ─────────────────────────────────────────────────────────────────────────────
namespace {
// 0x0049fc80 FUN_0049fc80 (C2): __thiscall, ECX = this; returns EAX.
typedef int(__thiscall* Fn0049fc80)(void* self);
const Fn0049fc80 s_fn0049fc80 = reinterpret_cast<Fn0049fc80>(0x0049fc80);
}  // namespace

#pragma comment(linker, "/export:ComInnerD8Vcall8cThen9fc80=@ComInnerD8Vcall8cThen9fc80@4")
extern "C" __declspec(dllexport) int __fastcall ComInnerD8Vcall8cThen9fc80(std::uint8_t* self) {
    void* inner = *reinterpret_cast<void**>(self + 0xd8);            // 0x0049ceb3
    void** vtbl = *reinterpret_cast<void***>(inner);                 // 0x0049ceb9
    const int r = reinterpret_cast<VcallFn>(vtbl[0x8c / 4])(inner);  // 0x0049cebb
    if (r < 0)                                                       // 0x0049cec3
        return r;
    return s_fn0049fc80(self);                                       // 0x0049cec8
}
RH_ScopedInstall(ComInnerD8Vcall8cThen9fc80, 0x0049ceb0);

// ─────────────────────────────────────────────────────────────────────────────
// ComObj8Vcall10StoreC  --  0x0049f2e0 (29 bytes, 0x0049f2e0..0x0049f2fc)
//
//   0x0049f2e1  mov esi,[esp+8]              ; p = stack arg (__stdcall, ret 4)
//   0x0049f2e5  mov ecx,[esi+8]              ; obj = p+8
//   0x0049f2e8  mov dword [esi+4],0          ; p+4 = 0
//   0x0049f2ef  mov eax,[ecx]
//   0x0049f2f1  call [eax+0x10]              ; r = obj->vtbl[4](), ECX = obj,
//                                            ; no stack args
//   0x0049f2f4  mov [esi+0xc],eax            ; p+0xc = r
//   0x0049f2f7  xor eax,eax                  ; return 0
//   0x0049f2fa  ret 4
//
// Installer rows: slot 5 (+0x14) of table 0x005d0f94, installer sites 0x0049f210
// (FUN_0049f210) and 0x004a0c1d (FUN_004a0c10), store_vptr, boundary resolved.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) int __stdcall ComObj8Vcall10StoreC(std::uint8_t* p) {
    void* obj = *reinterpret_cast<void**>(p + 8);                    // 0x0049f2e5
    *reinterpret_cast<std::uint32_t*>(p + 4) = 0;                    // 0x0049f2e8
    void** vtbl = *reinterpret_cast<void***>(obj);                   // 0x0049f2ef
    const int r = reinterpret_cast<VcallFn>(vtbl[0x10 / 4])(obj);    // 0x0049f2f1
    *reinterpret_cast<int*>(p + 0xc) = r;                            // 0x0049f2f4
    return 0;                                                        // 0x0049f2f7
}
RH_ScopedInstall(ComObj8Vcall10StoreC, 0x0049f2e0);
