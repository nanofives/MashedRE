// Mashed RE - walk a singly-linked chain, invoking a callback per node.
// Original: 0x005af200  FUN_005af200  audio  C2 -> C3   (34 bytes)
//
//     do { next = *(node + 0x38); fn(node, user); node = next; } while (next);
//
// THE ORDER MATTERS AND IS EASY TO GET WRONG. `next` is read BEFORE the callback
// runs (0x005af20f, ahead of the call at 0x005af214), so a callback that unlinks
// or frees its own node still leaves the walk with a valid successor. Reading
// the link after the call would be a different function.
//
// It is a do-while, not a while: the callback fires on the head unconditionally,
// with NO null check on the head pointer. Passing null faults in the original,
// and does here too.
//
// The termination test is on `next`, so the final node — the one whose +0x38 is
// zero — IS visited.
//
// The callback arrives as a plain argument (`call ebx` at 0x005af214), not
// through any table, and is __cdecl: the caller cleans with `add esp,8`.
//
// Disasm 0x005af200..0x005af222 (offsets shown post-push where relevant):
//   0x005af200  8B 44 24 04    mov  eax, [esp+4]     ; head
//   0x005af204  53             push ebx
//   0x005af205  8B 5C 24 0C    mov  ebx, [esp+0xc]   ; fn   (orig [esp+8])
//   0x005af209  56 57          push esi; push edi
//   0x005af20B  8B 7C 24 18    mov  edi, [esp+0x18]  ; user (orig [esp+0xc])
//   0x005af20F  8B 70 38       mov  esi, [eax+0x38]  ; next  <-- BEFORE the call
//   0x005af212  57             push edi              ; user
//   0x005af213  50             push eax              ; node
//   0x005af214  FF D3          call ebx              ; fn(node, user)
//   0x005af216  83 C4 08       add  esp, 8           ; __cdecl callback
//   0x005af219  8B C6          mov  eax, esi         ; node = next
//   0x005af21B  85 F6          test esi, esi
//   0x005af21D  75 F0          jnz  0x005af20f
//   0x005af21F  5F 5E 5B C3    pop edi/esi/ebx; ret
#include "../Core/HookSystem.h"

typedef void(__cdecl* AudioChainCbFn)(void* node, void* user);

// 0x005af200
extern "C" __declspec(dllexport) void __cdecl
AudioChainWalk(unsigned char* param_1, AudioChainCbFn param_2, void* param_3)
{
    unsigned char* node = param_1;
    for (;;) {
        unsigned char* next = *(unsigned char**)(node + 0x38);  // 0x005af20f
        param_2(node, param_3);                                 // 0x005af214
        node = next;                                            // 0x005af219
        if (next == 0)                                          // 0x005af21b/1d
            break;
    }
}

RH_ScopedInstall(AudioChainWalk, 0x005af200);
