// Mashed RE: Vtbl5cf9e8ScalarDeletingDtor, the MSVC scalar deleting destructor in slot 3 (+0xc) of vtable 0x005cf9e8, at 0x004943d0 (plan item #3, heap/import stand-ins, 2026-10-11).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// 0x004943d0  FUN_004943d0, 30 B, __thiscall(this, flags), returns this:
//   0x004943d0  push esi
//   0x004943d1  mov esi, ecx
//   0x004943d3  call 0x00493c60          destructor body (this in ECX, no stack args)
//   0x004943d8  test byte [esp+8], 1     flags bit 0
//   0x004943dd  je 0x004943e8
//   0x004943df  push esi
//   0x004943e0  call 0x004a3bda          `jmp 0x004a460d` (_free), cdecl, caller pops
//   0x004943e5  add esp, 4
//   0x004943e8  mov eax, esi             return this
//   0x004943eb  ret 4
// 0x00493c60 stores five vtable pointers (0x005cf9e8 at +0x0, 0x005cfd30 +0xc, 0x005cf9d0 +0x10, 0x005d0e88 +0xe0,
// 0x005d0e74 +0xe4) and tail-jumps to 0x0049ebc0, inside the directshow-baseclasses band (hooks.csv). That chain calls
// Enter/Leave/DeleteCriticalSection, CloseHandle, timeEndPeriod, InterlockedDecrement, FreeLibrary and _free
// (0x0049cbd0, 0x0049c990, 0x0049de90, 0x004a1180, 0x004a01b0, 0x004a1000), so this row is A/B-testable only under
// the heap + import stand-ins (re/frida/js/standins.js). The port calls 0x00493c60 itself: it is C2 and not ported.
//
// Negative controls (dev .asi only; MASHED_NEGCTL selects one, read once at DLL load):
//   dtor4943d0_skipfree     flags bit 0 set but no free                      -> free log loses `_free:OBJ`
//   dtor4943d0_wrongfree    frees through RwGlobals+0x10c (rwfree) instead   -> free log tag `rwfree`
//   dtor4943d0_extraimport  adds CloseHandle(0x1234) through the exe IAT slot 0x005cc088 (the slot 0x004a1187 uses)

#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace {
using DtorBodyFn = void(__fastcall*)(void* self, void* edx);  // 0x00493c60: ECX = this, no stack args
using CrtFreeFn = void(__cdecl*)(void* p);                    // 0x004a3bda -> 0x004a460d _free
using RwFreeFn = void(__cdecl*)(void* p);                     // *(0x007d3ff8) + 0x10c rwfree
using CloseHandleFn = int(__stdcall*)(void* h);               // exe IAT slot 0x005cc088

int NegCtl() {
    const char* v = std::getenv("MASHED_NEGCTL");
    if (!v) return 0;
    if (std::strcmp(v, "dtor4943d0_skipfree") == 0) return 1;
    if (std::strcmp(v, "dtor4943d0_wrongfree") == 0) return 2;
    if (std::strcmp(v, "dtor4943d0_extraimport") == 0) return 3;
    return 0;
}
// Read once at DLL load, never inside the body: getenv takes the .asi CRT's environment lock, and the import
// stand-in records that EnterCriticalSection / LeaveCriticalSection pair through the .asi IAT as two extra calls
// (the first run of this port read RED 6/6 on exactly that, 2026-10-11).
const int s_negctl = NegCtl();
}  // namespace

// 0x004943d0
extern "C" __declspec(dllexport) void* __fastcall Vtbl5cf9e8ScalarDeletingDtor(void* self, void* /*edx*/, std::uint32_t flags) {
    const int neg = s_negctl;
    reinterpret_cast<DtorBodyFn>(0x00493c60u)(self, nullptr);  // 0x004943d3
    if (neg == 3) {
        CloseHandleFn ch = *reinterpret_cast<CloseHandleFn*>(0x005cc088u);
        ch(reinterpret_cast<void*>(0x1234u));
    }
    if (flags & 1u) {                                           // 0x004943d8
        if (neg == 1) {
        } else if (neg == 2) {
            const std::uintptr_t rwg = *reinterpret_cast<const std::uintptr_t*>(0x007d3ff8u);
            (*reinterpret_cast<RwFreeFn*>(rwg + 0x10cu))(self);
        } else {
            reinterpret_cast<CrtFreeFn>(0x004a3bdau)(self);     // 0x004943e0
        }
    }
    return self;                                                // 0x004943e8
}
RH_ScopedInstall(Vtbl5cf9e8ScalarDeletingDtor, 0x004943d0);
