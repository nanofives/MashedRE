// Mashed RE - fread-shaped stream read through a vtable slot.
// Original: 0x00550950  FUN_00550950  audio  C2 -> C3   (41 bytes)
//
//     bytes = size * count;
//     got   = (*(fn**)( *(char**)(stream + 0x38) + 0x28 + 8 ))(stream, buf, bytes);
//     return got / size;                       // UNSIGNED divide
//
// The vtable is reached in two steps that the disassembly keeps separate:
// `add eax, 0x28` at 0x00550968 then `call [eax+8]` at 0x0055096d, i.e. the slot
// is at +0x30 from the table base. The port keeps both offsets visible rather
// than folding them to 0x30, because +0x28 is the sub-table and +8 the entry.
//
// DIVIDE BY ZERO IS REACHABLE. `div esi` at 0x00550975 divides by `size` with no
// guard, so size==0 faults in the original exactly as it does here. That is
// preserved deliberately — and it is why no test vector uses size 0.
//
// `div` is UNSIGNED (EDX zeroed at 0x00550973), so the port uses unsigned types
// throughout; a signed divide would differ for got > 0x7fffffff.
//
// Disasm 0x00550950..0x00550978 (stack offsets are post-`push esi`):
//   0x00550950  56             push esi
//   0x00550951  8B 74 24 0C    mov  esi, [esp+0xc]    ; size   (orig [esp+8])
//   0x00550955  8B D6          mov  edx, esi
//   0x00550957  8B 4C 24 14    mov  ecx, [esp+0x14]   ; stream (orig [esp+0x10])
//   0x0055095B  0F AF 54 24 10 imul edx, [esp+0x10]   ; * count(orig [esp+0xc])
//   0x00550960  8B 41 38       mov  eax, [ecx+0x38]   ; vtable base
//   0x00550963  52             push edx               ; bytes
//   0x00550964  8B 54 24 0C    mov  edx, [esp+0xc]    ; buf (orig [esp+4])
//   0x00550968  83 C0 28       add  eax, 0x28         ; sub-table
//   0x0055096B  52             push edx               ; buf
//   0x0055096C  51             push ecx               ; stream
//   0x0055096D  FF 50 08       call dword ptr [eax+8] ; slot +8 of the sub-table
//   0x00550970  83 C4 0C       add  esp, 0xc          ; __cdecl target
//   0x00550973  33 D2          xor  edx, edx
//   0x00550975  F7 F6          div  esi               ; unsigned: got / size
//   0x00550977  5E C3          pop esi; ret
#include "../Core/HookSystem.h"

typedef unsigned int(__cdecl* RwsStreamReadFn)(void* stream, void* buf,
                                               unsigned int bytes);

// 0x00550950
extern "C" __declspec(dllexport) unsigned int __cdecl
RwsStreamRead(void* param_1, unsigned int param_2, unsigned int param_3,
              unsigned char* param_4)
{
    unsigned char* vtable = *(unsigned char**)(param_4 + 0x38);   // 0x00550960
    RwsStreamReadFn fn = *(RwsStreamReadFn*)(vtable + 0x28 + 0x8);// 0x00550968/6d
    const unsigned int got = fn(param_4, param_1, param_2 * param_3);
    return got / param_2;                                          // 0x00550975
}

RH_ScopedInstall(RwsStreamRead, 0x00550950);
