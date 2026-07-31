// Mashed RE - table dispatch through an 8-byte-entry function table.
// Originals:
//   0x005b10a0  FUN_005b10a0  table ptr at holder+0x4   (37 bytes)
//   0x005b10e0  FUN_005b10e0  table ptr at holder+0x8   (37 bytes)
// Both: audio, C2 -> C3.
//
// The two are BYTE-IDENTICAL apart from a single displacement byte:
//   0x005b10a8  8B 48 04   mov ecx,[eax+4]
//   0x005b10e8  8B 48 08   mov ecx,[eax+8]
//
//     entry = *(holder + K) + idx*8;          // K = 4 or 8
//     entry[0]( a1, *(unsigned short*)(entry + 4), a4 );
//
// Each 8-byte table entry carries a function pointer at +0 and a 16-bit datum
// at +4 that is passed as the MIDDLE argument of the dispatched call. Nothing
// here names that datum, so it is reported as a raw u16 and given no meaning.
//
// ARGUMENT ORDER - read from the stack, not from the decompiler. The two
// PUSHes before the third read shift ESP, so `mov eax,[esp+0xc]` at 0x005b10bb
// reaches the ORIGINAL [esp+4], i.e. the FIRST argument. Working it through:
//   [esp+0x04] a1      -> pushed LAST, so it is the dispatched call's arg 1
//   [esp+0x08] holder  -> the table holder
//   [esp+0x0c] idx     -> entry index, stride 8
//   [esp+0x10] a4      -> pushed FIRST, so it is the dispatched call's arg 3
// Both this function and the dispatched target are __cdecl (`add esp,0xc` at
// 0x005b10c2 cleans the three pushed arguments here; the caller cleans ours).
//
// Disasm at 0x005b10a0..0x005b10c5 (0x005b10e0 identical but for the one byte):
//   0x005b10a0  8B 44 24 08         mov  eax, [esp+8]          ; holder
//   0x005b10a4  8B 54 24 0C         mov  edx, [esp+0xc]        ; idx
//   0x005b10a8  8B 48 04            mov  ecx, [eax+4]          ; table base
//   0x005b10ab  33 C0               xor  eax, eax              ; zero-extend
//   0x005b10ad  66 8B 44 D1 04      mov  ax, [ecx+edx*8+4]     ; u16 at entry+4
//   0x005b10b2  8D 0C D1            lea  ecx, [ecx+edx*8]      ; &entry
//   0x005b10b5  8B 54 24 10         mov  edx, [esp+0x10]       ; a4
//   0x005b10b9  52                  push edx                   ; arg3 = a4
//   0x005b10ba  50                  push eax                   ; arg2 = aux16
//   0x005b10bb  8B 44 24 0C         mov  eax, [esp+0xc]        ; = orig [esp+4] = a1
//   0x005b10bf  50                  push eax                   ; arg1 = a1
//   0x005b10c0  FF 11               call dword ptr [ecx]       ; entry[0]
//   0x005b10c2  83 C4 0C            add  esp, 0xc
//   0x005b10c5  C3                  ret
//
// The XOR/MOV AX pair zero-extends the 16-bit datum into a full dword, so the
// dispatched call receives it as a zero-extended u32, never sign-extended.
#include "../Core/HookSystem.h"

// Each table entry: [0] = target, [4] = u16 datum, [6] = padding.
struct AudioDispatchEntry {
    void(__cdecl* fn)(unsigned int a1, unsigned int aux, unsigned int a4);
    unsigned short aux;
    unsigned short pad;
};

// 0x005b10a0
extern "C" __declspec(dllexport) void __cdecl
AudioTableDispatchAt4(unsigned int a1, unsigned char* holder,
                      unsigned int idx, unsigned int a4)
{
    AudioDispatchEntry* table = *(AudioDispatchEntry**)(holder + 0x4);
    AudioDispatchEntry* entry = table + idx;          // stride 8 = sizeof(entry)
    entry->fn(a1, (unsigned int)entry->aux, a4);      // aux zero-extended
}

// 0x005b10e0
extern "C" __declspec(dllexport) void __cdecl
AudioTableDispatchAt8(unsigned int a1, unsigned char* holder,
                      unsigned int idx, unsigned int a4)
{
    AudioDispatchEntry* table = *(AudioDispatchEntry**)(holder + 0x8);
    AudioDispatchEntry* entry = table + idx;
    entry->fn(a1, (unsigned int)entry->aux, a4);
}

RH_ScopedInstall(AudioTableDispatchAt4, 0x005b10a0);
RH_ScopedInstall(AudioTableDispatchAt8, 0x005b10e0);
