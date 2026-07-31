// Mashed RE - one-slot COM-style vtable thunk.
// Original: 0x004cbb50  FUN_004cbb50  render  C2 -> C3   (10 bytes)
//
//     (*(void**)((*(char**)obj) + 8))(obj);   // and pass the result straight out
//
// NOTE THE MISSING STACK CLEANUP. The call at 0x004cbb57 is followed directly
// by RET — there is no `add esp,4` — so the dispatched target is __stdcall
// (callee-cleans), while this thunk itself is __cdecl with one argument. Get
// that backwards and the stack is off by four on every call.
//
// There is also no `mov` between the call and the RET, so whatever the target
// leaves in EAX becomes this function's return value. The port returns it
// explicitly; the register passthrough is not something C can express directly.
//
// Slot +8 is the third vtable entry. Nothing here names it, so no COM identity
// (Release/AddRef/etc.) is asserted — it is reported as "vtable slot +8".
//
// Disasm 0x004cbb50..0x004cbb5a:
//   0x004cbb50  8B 44 24 04    mov  eax, [esp+4]     ; obj
//   0x004cbb54  50             push eax              ; arg = obj
//   0x004cbb55  8B 08          mov  ecx, [eax]       ; vtable = *obj
//   0x004cbb57  FF 51 08       call dword ptr [ecx+8]
//   0x004cbb5A  C3             ret                   ; no cleanup -> stdcall target
#include "../Core/HookSystem.h"

typedef int(__stdcall* RwComSlot2Fn)(void* self);

// 0x004cbb50
extern "C" __declspec(dllexport) int __cdecl
RwComReleaseThunk(void* param_1)
{
    unsigned char* vtable = *(unsigned char**)param_1;        // 0x004cbb55
    RwComSlot2Fn fn = *(RwComSlot2Fn*)(vtable + 0x8);         // 0x004cbb57
    return fn(param_1);                                       // EAX flows out
}

RH_ScopedInstall(RwComReleaseThunk, 0x004cbb50);
