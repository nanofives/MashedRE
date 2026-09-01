// TextCtrlCodeRemap — 0x004277a0
//
// One-file class per gta-reversed convention. Reimplementation of FUN_004277a0.
//
// Original (0x004277a0..0x00427832, 147 bytes). Register calling convention:
//   EAX = source pointer to a length-prefixed uint16 array: [count, ch0, ch1, ...]
//   EBX = destination pointer (uint16 array)
// No stack args, plain RET. Preserves EBX (only read), pushes/pops ESI and EDI.
//
// Behaviour (verbatim from the disassembly at 0x004277a0):
//   count = *(int16*)EAX        (mov si,[eax])
//   src  += 1 word              (add eax,2 -> points at ch0)
//   if count <= 0: dst[(int16)count] = 0; return.   (test si,si / jle 0x427828)
//   else loop i=0..count-1:
//     c = src[i]                (mov ax,[edx+ecx], with edx=src-dst, ecx=dst+i*2)
//     remap control codes (else pass through):
//        8 -> 0x81   9 -> 0x7f   0x0a -> 0x81   0x0b -> 0x8d
//        0x0c -> 0x80   0x0d -> 0x87   0x0e -> 0x8f
//     dst[i] = c                (mov [ecx],ax ; add ecx,2 ; dec edi ; jne)
//   dst[(int16)count] = 0       (movsx eax,si ; mov word ptr [ebx+eax*2],0)
//   return.
//
// Pure integer transform (no x87), no callees (pure leaf), no globals. The naked
// body below is a verbatim transcription of the original instruction stream, so
// the destination buffer is bit-identical by construction.
//
// Callers: MenuMenusBA (0x004282a0, C3), MenuMenusBB (0x00427ad0, C3).
// ref: re/analysis/promote_c1_low_ab1/0x004277a0.md ;
//      re/analysis/frontend_004277a0_c3_plan.md ; listing 0x004277a0..0x00427832
//
// U-9065 (2026-09-01): this TU is the SOLE installer at 0x004277a0. The dev-only
// EBX==0 boot guard formerly in Compat/IntroTextNullGuard.cpp (a SECOND
// RH_ScopedInstall that WON the site and shadowed this reimpl -> path2 measured
// 0/2 install) is folded in below, gated `#ifndef MASHED_STANDALONE`. The stock
// fn NULL-writes out[count] when a system-DLL intro-text caller passes EBX=0 ~90 s
// into the modded boot -> AV; the dev .asi needs that one case skipped to reach the
// menu for runtime verification. The shipping greenfield exe (MASHED_STANDALONE)
// never hooks MASHED, so the stock crash cannot occur there -> guard compiled out,
// leaving the pure verbatim body. For every valid caller (EBX!=0) and the path1
// harness the guard falls straight through, so the destination stays bit-identical.
// ---------------------------------------------------------------------------

#include "../Core/HookSystem.h"

// 0x004277a0
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl TextCtrlCodeRemap()
{
    __asm {
        // EAX = src (length-prefixed u16), EBX = dst  (set by caller / harness).
#ifndef MASHED_STANDALONE
        // U-9065 dev-only boot guard: skip the broken EBX==0 (NULL out buffer) case.
        test ebx, ebx
        jnz  do_work
        ret
    do_work:
#endif
        push esi
        mov  si, word ptr [eax]                  // count
        add  eax, 2                              // src -> first char
        test si, si
        jle  terminate_only                      // count <= 0

        mov  edx, eax
        push edi
        mov  ecx, ebx                            // ecx = dst cursor
        sub  edx, ebx                            // edx = src - dst (so [edx+ecx] = src[i])
        movzx edi, si                            // loop counter = count

    remap_loop:
        mov  ax, word ptr [edx + ecx]            // c = src[i]
        cmp  ax, 8
        jne  c9
        mov  eax, 0x81
        jmp  store
    c9:
        cmp  ax, 9
        jne  ca
        mov  eax, 0x7f
        jmp  store
    ca:
        cmp  ax, 0xa
        jne  cb
        mov  eax, 0x81
        jmp  store
    cb:
        cmp  ax, 0xb
        jne  cc
        mov  eax, 0x8d
        jmp  store
    cc:
        cmp  ax, 0xc
        jne  cd
        mov  eax, 0x80
        jmp  store
    cd:
        cmp  ax, 0xd
        jne  ce
        mov  eax, 0x87
        jmp  store
    ce:
        cmp  ax, 0xe
        jne  store
        mov  eax, 0x8f
    store:
        mov  word ptr [ecx], ax                  // dst[i] = c
        add  ecx, 2
        dec  edi
        jne  remap_loop

        movsx eax, si
        pop  edi
        mov  word ptr [ebx + eax*2], 0           // dst[count] = 0
        pop  esi
        ret

    terminate_only:
        movsx ecx, si
        mov  word ptr [ebx + ecx*2], 0           // dst[count] = 0  (count <= 0)
        pop  esi
        ret
    }
}

RH_ScopedInstall(TextCtrlCodeRemap, 0x004277a0);
