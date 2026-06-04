// Mashed RE — Util leaf functions, c3_batch_ab session 6 (audio/util batch).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// All seven reimplementations below are byte-faithful ports of disassembly read
// directly from the anchored binary (capstone, 2026-06-04). Each function's
// exact instruction stream is reproduced in its header comment; the C body is a
// 1:1 transcription. NO-GUESSING: every constant is cited to the instruction
// that emits it.
//
// Candidates (c3_batch_ab.txt session 6):
//   0x004425d0  FUN_004425d0  — void(void): zero elem[0] of stride-0xD8 table @0x008964c0
//   0x0045c480  FUN_0045c480  — void(void): zero 12 scalars + 36-dword block @0x0088f5e0
//   0x0046b4f0  FUN_0046b4f0  — int(out,o,i): bounds-checked 3-dword copy from table @0x008815a0
//   0x004904d0  FUN_004904d0  — void(void): zero elem[0] of stride-0x50 table @0x0086a4a0
//   0x004d8560  FUN_004d8560  — int(void): return 1
//   0x0055dec0  FUN_0055dec0  — int(int*): return *p  (cdecl stack arg)
//   0x004a1790  FUN_004a1790  — uint __fastcall(void**): COM Release thunk (ECX)
//
// Source analysis notes:
//   re/analysis/timer_d3_cont1_b/0x004425d0.md
//   re/analysis/timer_d2_cont1/0x0045c480.md
//   re/analysis/profile_career_d3/FUN_0046b4f0.md
//   re/analysis/timer_d3_cont2/0x004904d0.md
//   re/analysis/timer_d3_cont2/0x004d8560.md
//   re/analysis/timer_d3_cont2/0x0055dec0.md
//   re/analysis/video_mci/0x004a1790.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// FUN_004425d0  --  0x004425d0   (void(void) global-table zeroer A)
//
// Disasm (anchored binary, 0x004425d0..0x004425fc):
//   b8 c0648900   mov  eax, 0x008964c0          ; table base
//   eb 09         jmp  0x4425e0                 ; (alignment nops 0x4425d7..0x4425df)
//  loop @0x4425e0:
//   c7 00 00000000 mov  dword ptr [eax], 0       ; zero elem[0] of current entry
//   05 d8000000   add  eax, 0xd8                 ; stride = 0xD8 bytes (216) per entry
//   3d c07f8900   cmp  eax, 0x00897fc0           ; loop sentinel
//   7c ee         jl   0x4425e0                  ; signed: continue while eax < 0x897fc0
//   c705 8c898900 00000000  mov dword ptr [0x0089898c], 0   ; trailing scalar
//   c3            ret
//
// Zeroes the first dword of each 0xD8-byte entry across [0x008964c0, 0x00897fc0),
// then zeroes the standalone scalar at 0x0089898c. No params, no callees.
// ---------------------------------------------------------------------------

// 0x004425d0
extern "C" __declspec(dllexport) void __cdecl UtilZeroTable8964c0() {
    for (std::uint32_t a = 0x008964c0u; (std::int32_t)a < 0x00897fc0; a += 0xD8u) {
        *reinterpret_cast<std::uint32_t*>(a) = 0u;   // mov [eax],0 ; add eax,0xd8 ; cmp/jl
    }
    *reinterpret_cast<std::uint32_t*>(0x0089898cu) = 0u;  // mov [0x0089898c],0
}

RH_ScopedInstall(UtilZeroTable8964c0, 0x004425d0);

// ---------------------------------------------------------------------------
// FUN_0045c480  --  0x0045c480   (void(void) scatter zeroer B)
//
// Disasm (anchored binary, 0x0045c480..0x0045c4d2):
//   33c0          xor eax, eax                   ; eax = 0
//   33c9          xor ecx, ecx                   ; ecx = 0
//   890d a0f08800 mov [0x0088f0a0], ecx
//   a3 b0f68800   mov [0x0088f6b0], eax
//   a3 7ce68800   mov [0x0088e67c], eax
//   a3 b0f08800   mov [0x0088f0b0], eax
//   a3 80eb8800   mov [0x0088eb80], eax
//   890d a4f08800 mov [0x0088f0a4], ecx
//   a3 e0a28a00   mov [0x008aa2e0], eax
//   890d a8f08800 mov [0x0088f0a8], ecx
//   57            push edi
//   a3 e4a28a00   mov [0x008aa2e4], eax
//   890d acf08800 mov [0x0088f0ac], ecx
//   a3 e8a28a00   mov [0x008aa2e8], eax
//   b9 24000000   mov ecx, 0x24                  ; 36 dwords
//   bf e0f58800   mov edi, 0x0088f5e0           ; block base
//   f3ab          rep stosd                      ; zero [0x0088f5e0 .. +0x90)
//   a3 eca28a00   mov [0x008aa2ec], eax
//   5f            pop edi
//   c3            ret
//
// Zeroes 12 individual dword globals plus a 36-dword (0x90-byte) block at
// 0x0088f5e0. Value written is always 0 (eax==ecx==0). No params, no callees.
// ---------------------------------------------------------------------------

// 0x0045c480
extern "C" __declspec(dllexport) void __cdecl UtilZeroScatter45c480() {
    *reinterpret_cast<std::uint32_t*>(0x0088f0a0u) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x0088f6b0u) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x0088e67cu) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x0088f0b0u) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x0088eb80u) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x0088f0a4u) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008aa2e0u) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x0088f0a8u) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008aa2e4u) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x0088f0acu) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008aa2e8u) = 0u;
    // rep stosd: 0x24 (36) dwords from 0x0088f5e0.
    std::uint32_t* blk = reinterpret_cast<std::uint32_t*>(0x0088f5e0u);
    for (int i = 0; i < 0x24; ++i) blk[i] = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008aa2ecu) = 0u;
}

RH_ScopedInstall(UtilZeroScatter45c480, 0x0045c480);

// ---------------------------------------------------------------------------
// FUN_0046b4f0  --  0x0046b4f0   (int(out, outerIdx, innerIdx) table reader)
//
// Disasm (anchored binary, 0x0046b4f0..0x0046b537):
//   8b4c2408      mov ecx, [esp+8]               ; ecx = param_2 (outer index)
//   83f910        cmp ecx, 0x10
//   7309          jae 0x46b502                   ; unsigned: outer >= 16 -> fail
//   8b44240c      mov eax, [esp+0xc]             ; eax = param_3 (inner index)
//   83f812        cmp eax, 0x12
//   7c03          jl  0x46b505                   ; signed: inner < 18 -> ok
//  fail @0x46b502:
//   33c0          xor eax, eax                   ; return 0
//   c3            ret
//  ok @0x46b505:
//   69c9 040d0000 imul ecx, ecx, 0xd04           ; outer * 0xD04 (row byte stride)
//   8d544018      lea  edx, [eax + eax*2 + 0x18] ; inner*3 + 0x18  (dword index)
//   81c1 a0158800 add  ecx, 0x008815a0           ; ecx = table base + outer*0xD04
//   56            push esi
//   8b3491        mov  esi, [ecx + edx*4]        ; src dword #0 = base[ inner*3 + 0x18 ]
//   8b542408      mov  edx, [esp+8]              ; edx = param_1 (out ptr) (esp shifted by push)
//   8d0440        lea  eax, [eax + eax*2]        ; inner*3
//   8d0481        lea  eax, [ecx + eax*4]        ; eax = &base[ inner*3 ]  (within row)
//   8932          mov  [edx], esi                ; out[0] = src #0
//   8b4864        mov  ecx, [eax + 0x64]         ; src #1 = base[inner*3] + 0x64
//   894a04        mov  [edx+4], ecx              ; out[1]
//   8b4068        mov  eax, [eax + 0x68]         ; src #2 = base[inner*3] + 0x68
//   894208        mov  [edx+8], eax              ; out[2]
//   b801000000    mov  eax, 1                    ; return 1
//   5e            pop esi
//   c3            ret
//
// Element address algebra (all cited above):
//   row   = 0x008815a0 + outer * 0xD04
//   out[0] = *(row + (inner*3 + 0x18)*4)
//   out[1] = *(row + inner*3*4 + 0x64)
//   out[2] = *(row + inner*3*4 + 0x68)
// The +0x18 dword (=0x60 bytes) and the +0x64/+0x68 byte offsets reproduce the
// original's two distinct base pointers exactly (one dword-indexed, one byte-
// indexed off the same row). Reads a static data table; both orig and reimpl
// observe identical table contents -> bit-identical. (U-2193: table semantics
// unconfirmed; non-blocking — does not affect offset/bounds correctness.)
// ---------------------------------------------------------------------------

// 0x0046b4f0
extern "C" __declspec(dllexport) int __cdecl UtilTableRead46b4f0(
        std::uint32_t* out, std::uint32_t outerIdx, std::uint32_t innerIdx) {
    if (outerIdx >= 0x10u) return 0;                    // cmp 0x10 / jae
    if ((std::int32_t)innerIdx >= 0x12) return 0;       // cmp 0x12 / jl (signed)
    // row base = 0x008815a0 + outer*0xD04, addressed as a byte pointer.
    std::uint8_t* row = reinterpret_cast<std::uint8_t*>(0x008815a0u + outerIdx * 0xD04u);
    // out[0] = *(uint32*)(row + (inner*3 + 0x18)*4)
    out[0] = *reinterpret_cast<std::uint32_t*>(row + (innerIdx * 3u + 0x18u) * 4u);
    // &base[inner*3] as a byte pointer for the +0x64/+0x68 reads.
    std::uint8_t* p = row + (innerIdx * 3u) * 4u;
    out[1] = *reinterpret_cast<std::uint32_t*>(p + 0x64u);
    out[2] = *reinterpret_cast<std::uint32_t*>(p + 0x68u);
    return 1;
}

RH_ScopedInstall(UtilTableRead46b4f0, 0x0046b4f0);

// ---------------------------------------------------------------------------
// FUN_004904d0  --  0x004904d0   (void(void) global-table zeroer C)
//
// Disasm (anchored binary, 0x004904d0..0x004904f0):
//   b8 a0a48600   mov  eax, 0x0086a4a0          ; table base
//   eb 09         jmp  0x4904e0                 ; (alignment nops 0x4904d7..0x4904df)
//  loop @0x4904e0:
//   c7 00 00000000 mov dword ptr [eax], 0        ; zero elem[0] of current entry
//   83c0 50       add  eax, 0x50                 ; stride = 0x50 bytes (80) per entry
//   3d 00ae8600   cmp  eax, 0x0086ae00           ; loop sentinel
//   7c f0         jl   0x4904e0                  ; signed: continue while eax < 0x86ae00
//   c3            ret
//
// Zeroes the first dword of each 0x50-byte entry across [0x0086a4a0, 0x0086ae00)
// = 30 entries. No params, no callees.
// ---------------------------------------------------------------------------

// 0x004904d0
extern "C" __declspec(dllexport) void __cdecl UtilZeroTable86a4a0() {
    for (std::uint32_t a = 0x0086a4a0u; (std::int32_t)a < 0x0086ae00; a += 0x50u) {
        *reinterpret_cast<std::uint32_t*>(a) = 0u;
    }
}

RH_ScopedInstall(UtilZeroTable86a4a0, 0x004904d0);

// ---------------------------------------------------------------------------
// FUN_004d8560  --  0x004d8560   (int(void) constant)
//
// Disasm (anchored binary, 0x004d8560..0x004d8565):
//   b801000000    mov  eax, 1
//   c3            ret
//
// Always returns 1. No params, no globals, no callees.
// ---------------------------------------------------------------------------

// 0x004d8560
extern "C" __declspec(dllexport) int __cdecl UtilReturnOne4d8560() {
    return 1;   // mov eax,1 ; ret
}

RH_ScopedInstall(UtilReturnOne4d8560, 0x004d8560);

// ---------------------------------------------------------------------------
// FUN_0055dec0  --  0x0055dec0   (int(int*) pointer deref — cdecl stack arg)
//
// Disasm (anchored binary, 0x0055dec0..0x0055dec6):
//   8b442404      mov  eax, [esp+4]              ; eax = param_1 (pointer, on stack)
//   8b00          mov  eax, [eax]                ; eax = *param_1
//   c3            ret
//
// Reads the first dword arg as a pointer and returns the dword it points at.
// NOTE: param arrives on the STACK ([esp+4]) — this is __cdecl, NOT an
// EAX-implicit register leaf (the c3_batch_ab suggestion 'eax_implicit_ptr' was
// disconfirmed by the bytes above). Verified via vec3_ptr (harness passes a
// seeded buffer pointer as the single arg and compares the returned dword).
// ---------------------------------------------------------------------------

// 0x0055dec0
extern "C" __declspec(dllexport) std::uint32_t __cdecl UtilDeref55dec0(const std::uint32_t* p) {
    return *p;   // mov eax,[esp+4] ; mov eax,[eax] ; ret
}

RH_ScopedInstall(UtilDeref55dec0, 0x0055dec0);

// ---------------------------------------------------------------------------
// FUN_004a1790  --  0x004a1790   (COM Release thunk — __fastcall ECX)   *CANARY*
//
// Disasm (anchored binary, 0x004a1790..0x004a179c):
//   8b01          mov  eax, [ecx]                ; eax = *pp  (interface pointer)
//   85c0          test eax, eax
//   7406          je   0x4a179c                  ; if interface == NULL -> ret
//   8b08          mov  ecx, [eax]                ; ecx = vtable (first dword of interface)
//   50            push eax                        ; push 'this'
//   ff5108        call dword ptr [ecx+8]          ; vtable[2] = Release (this on stack)
//   c3            ret
//
// __fastcall: the single argument arrives in ECX (a pointer-to-interface-
// pointer). Loads *pp; if non-null, calls vtable slot 2 (IUnknown::Release,
// offset 8) with 'this' pushed on the stack. On the NULL path EAX = *pp = 0 is
// the return value.
//
// *** fastcall_reg CANARY *** — this hook validates the new fastcall_reg
// arg_type in diff_template.js. The harness seeds ECX with a ZEROED 64-byte
// scratch buffer, so *pp == 0 -> the je is taken -> both orig and reimpl no-op
// and return 0 (proves ECX marshalling + callee-clean stack balance). The
// vtable call path is NOT exercised by the canary (no live COM interface).
//
// Exported __fastcall, so MSVC decorates the symbol as @ComReleaseThunk@4; the
// linker pragma re-exports the undecorated name the Frida harness resolves
// (same technique as MenuGroupCount @ 0x0042ac00).
// ---------------------------------------------------------------------------

#pragma comment(linker, "/export:ComReleaseThunk=@ComReleaseThunk@4")

// 0x004a1790
extern "C" __declspec(dllexport) std::uint32_t __fastcall ComReleaseThunk(void** pp) {
    void* itf = *pp;                                  // mov eax,[ecx]
    if (itf) {                                        // test/je
        void** vtbl = *reinterpret_cast<void***>(itf);            // mov ecx,[eax]
        // push eax ; call [ecx+8]  — 'this' passed on the stack (stdcall-style Release).
        reinterpret_cast<void(__stdcall*)(void*)>(vtbl[2])(itf);
    }
    return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(itf));  // EAX = *pp on null path
}

RH_ScopedInstall(ComReleaseThunk, 0x004a1790);
