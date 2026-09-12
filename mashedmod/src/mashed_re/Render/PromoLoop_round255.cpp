// Mashed RE — promote-round round 255 (two callback walkers).
//
// Functions in this file:
//   0x004d8060  RwPluginListDispatch3 — walks the plugin list at +0x14 and calls
//                each node's callback with THREE arguments.
//   0x004f0d80  D3D9DeclElementForAll — walks a counted stride-0xc array and calls
//                a CALLER-SUPPLIED callback until it returns 0.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// RwPluginListDispatch3  --  0x004d8060
//
// int fn(int registry /*[esp+4]*/, uint32 arg /*[esp+8]*/)
//
// Disassembly 0x004d8060..0x004d8088:
//   push ebx / mov ebx,[esp+8]        ; registry
//   push esi / mov esi,[ebx+0x14]     ; head  <- +0x14
//   test esi,esi / je end
//   push edi / mov edi,[esp+0x14]     ; arg
// loop:
//   mov eax,[esi+4]  / mov ecx,[esi]  ; node[1], node[0]
//   push eax / push ecx / push edi    ; -> callee(arg, node[0], node[1])
//   call dword ptr [esi+0x24]         ; callback <- +0x24
//   mov esi,[esi+0x34]                ; next     <- +0x34
//   add esp,0xc                       ; __cdecl, THREE args
//   test esi,esi / jne loop
//   ...returns registry
//
// CAREFUL — THIS IS THE SIBLING OF 0x004d8090 (promoted r253) AND EVERY OFFSET
// DIFFERS. Confusing the two is the realistic porting error here:
//                    0x004d8090          0x004d8060 (this one)
//   list head        registry+0x10       registry+0x14
//   next link        node+0x30           node+0x34
//   callback slot    node+0x28           node+0x24
//   callback arity   4 (a, b, n0, n1)    3 (arg, n0, n1)
// The A/B therefore seeds the 0x004d8090 offsets too and requires they stay
// untouched, so a port that reused the sibling's constants fails.
//
// Terminated by a NULL next pointer, not by a sentinel address.
// 32 callers, including FUN_004c0c20 (C2).
// ---------------------------------------------------------------------------

// 0x004d8060
extern "C" __declspec(dllexport) int __cdecl RwPluginListDispatch3(
    int registry, std::uint32_t arg)
{
    for (std::uint32_t* node = *reinterpret_cast<std::uint32_t**>(registry + 0x14);
         node != nullptr;
         node = reinterpret_cast<std::uint32_t*>(node[0xd])) {   // +0x34
        reinterpret_cast<void(__cdecl*)(std::uint32_t, std::uint32_t, std::uint32_t)>(
            node[9])(arg, node[0], node[1]);                     // +0x24
    }
    return registry;
}

RH_ScopedInstall(RwPluginListDispatch3, 0x004d8060);

// ---------------------------------------------------------------------------
// D3D9DeclElementForAll  --  0x004f0d80
//
// int fn(int obj /*[esp+4]*/, code* cb /*[esp+8]*/, uint32 user /*[esp+0xc]*/)
//
// Disassembly 0x004f0d80..0x004f0db3:
//   mov ebx,[esp+8]                 ; obj   (after push ebx)
//   mov eax,[ebx+0xc]               ; byte offset
//   xor esi,esi / mov si,[ebx+4]    ; count, ZERO-EXTENDED 16-bit
//   lea edi,[eax+ebx+0x10]          ; cursor = obj + offset + 0x10
//   test esi,esi / je end           ; count 0 -> return immediately
// loop:
//   mov ebp,[esp+0x1c]              ; user
//   push ebp / push ebx / push edi  ; -> cb(cursor, obj, user)
//   dec esi
//   call dword ptr [esp+0x24]       ; the CALLBACK ARGUMENT, called straight
//                                   ; from its stack slot
//   add esp,0xc                     ; __cdecl, three args
//   test eax,eax / je end           ; EARLY EXIT when the callback returns 0
//   add edi,0xc                     ; stride 0xc
//   test esi,esi / ...
//   ...returns obj in every case
//
// THREE THINGS A PORT CAN GET WRONG, all covered by the vectors:
//   1. the count is a ZERO-EXTENDED 16-bit field, not a 32-bit one;
//   2. the cursor base is obj + *(int*)(obj+0xc) + 0x10, and the 0x10 is easy
//      to drop;
//   3. the loop EARLY-EXITS on a zero return and still returns obj, so a port
//      that ran to completion would produce the same return value and differ
//      only in the number of callback invocations.
//
// The callee is reached through a function pointer PASSED AS AN ARGUMENT, which
// re/CONFIDENCE.md's indirect-dispatch clause calls the "a fortiori" case — and
// its requirement is met rather than merely invoked: the A/B plants a recorder
// in that argument slot, so the dispatch itself is what is being compared.
//
// Sole caller FUN_004f36c0 (C2).
// ---------------------------------------------------------------------------

// 0x004f0d80
extern "C" __declspec(dllexport) int __cdecl D3D9DeclElementForAll(
    std::int32_t obj, void* cb, std::uint32_t user)
{
    std::uint32_t remaining = *reinterpret_cast<std::uint16_t*>(obj + 4);  // zero-extended
    std::int32_t cursor = *reinterpret_cast<std::int32_t*>(obj + 0xc) + 0x10 + obj;

    while (remaining != 0) {
        remaining = remaining - 1;
        const int r = reinterpret_cast<int(__cdecl*)(std::int32_t, std::int32_t, std::uint32_t)>(
            cb)(cursor, obj, user);
        if (r == 0) {
            break;          // early exit, still returns obj
        }
        cursor = cursor + 0xc;
    }
    return obj;
}

RH_ScopedInstall(D3D9DeclElementForAll, 0x004f0d80);
