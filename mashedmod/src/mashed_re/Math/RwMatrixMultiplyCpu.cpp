// Mashed RE — U-9151: a standalone CPU implementation of the RenderWare device
// matrix multiply, so `RwMatrixRotate` combine modes 1/2 work in `mashed_re.exe`.
//
// WHY. `RwMatrixMultiply` 0x004c4600 is a DISPATCHER, not the math. Disassembled from
// the anchored `original/MASHED.exe.unpatched`
// (`py -3.12 re/tools/disasm_fn.py 0x004c4600 0x004c4680`, RETs at 0x004c463c /
// 0x004c4654 / 0x004c466e):
//
//   0x004c4600  mov  ecx, [0x007d4028]            ; device-table byte offset
//   0x004c460c  mov  ebp, [0x007d3ff8]            ; device-table runtime base
//   0x004c461a  mov  eax, [ecx+ebp+4]             ; caps word
//   0x004c4622  and  eax, 0x20000                 ; the IDENTITY bit
//   0x004c4627  test ebx, eax                     ; ebx = A->flags  ([edx+0xc])
//   0x004c4629  je   ...                          ; A identity -> rep movsd B -> out
//   0x004c463d  test edi, eax                     ; edi = B->flags ([esi+0xc])
//   0x004c463f  je   ...                          ; B identity -> rep movsd A -> out
//   0x004c465c  call [ecx+ebp+8]                  ; the DEVICE multiply (out, A, B)
//   0x004c4663  and  edi, ebx / mov [esi+0xc],edi ; out->flags = A->flags & B->flags
//
// Those two absolute addresses are mapped in the injected dev `.asi` and NOT in the
// standalone, so with `orient` bound the exe's first airborne A6b dereferenced
// unmapped memory and exited 0xC0000005 on 3 runs of 3
// (`verify/d2_reopen_20260929/solo_a6bfix{1,2,3}/PROVENANCE.txt`; U-9151, and
// `re/analysis/D2_REOPEN_2026-09-29.md` §4.6).
//
// WHICH FUNCTION THE POINTER HOLDS — MEASURED, not guessed. Read live off the running
// anchored original with `re/frida/scenario_launch.py --peek`, during a race
// (`--track 3 --mode 10 --cars 4 --car 0 --hold 20`), two samples 3.7 s apart and
// identical:
//
//   [0x007d4028]            = 0x0000014c
//   [0x007d3ff8]            = 0x02d28d80        (heap; varies per run)
//   [devBase+0]             = 0x007d4004
//   [devBase+4]             = 0x00020000        <- confirms the 0x20000 identity mask
//   [devBase+8]             = 0x005cb2a0        <- THE MULTIPLY
//
// So the device multiply is the in-image routine at **0x005cb2a0**, `RET` at
// 0x005cb402. Transcribed verbatim below.
//
// SIGNATURE, from the prologue (`0x005cb2a0..0x005cb2ac`):
//   cdecl, three stack args. [ebp+8] -> ECX = out, [ebp+0xc] -> EAX = A,
//   [ebp+0x10] -> EBX = B. No return value; EBP/EAX/EBX/ECX saved and restored.
//   `out = A * B` in RenderWare's row-vector convention, 4 rows of 3 at stride 0x10.
//
// WHY NAKED x87 AND NOT PLAIN C++. Every row accumulates in the x87 80-bit stack and
// rounds to float32 ONCE, at the `fstp`. A C++ `a0*b0 + a1*b4 + a2*b8` built with
// /arch:SSE2 rounds at each step. The sibling CPU stand-in in
// `Math/RwV3dTransformPointsCPU.cpp` took the plain-C++ route and its own measurement
// records the cost: "9/10 bit-identical, the 10th off by 1 ULP ... a summation-order
// difference". A 1-ULP drift is not acceptable in a matrix that is preconcatenated
// every airborne frame, so this is the verbatim transcription instead. Recipe:
// memory `render-cheapest-wins-are-rw-math-leaves` and `Math/RwMatrixRotateInner.cpp`.
//
// Anchored MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.
#include <cstdint>
#include <cstring>

namespace mashed_re {
namespace Math {

// 0x005cb2a0 — the RenderWare device matrix multiply, verbatim.
// Every instruction below is the instruction at the cited address, in order.
__declspec(naked) void __cdecl RwMatrixMultiplyCPU(float* /*out*/,
                                                   const float* /*A*/,
                                                   const float* /*B*/)
{
    __asm {
        push ebp                        // 0x005cb2a0
        mov  ebp, esp                   // 0x005cb2a1
        push eax                        // 0x005cb2a3
        push ebx                        // 0x005cb2a4
        push ecx                        // 0x005cb2a5
        mov  ebx, dword ptr [ebp+0x10]  // 0x005cb2a6  B
        mov  eax, dword ptr [ebp+0x0c]  // 0x005cb2a9  A
        mov  ecx, dword ptr [ebp+0x08]  // 0x005cb2ac  out
        // ---- row 0 ----
        fld  dword ptr [eax+8]          // 0x005cb2af
        fld  dword ptr [eax+4]          // 0x005cb2b2
        fld  dword ptr [eax]            // 0x005cb2b5
        fld  dword ptr [ebx]            // 0x005cb2b7
        fmul st(0), st(1)               // 0x005cb2b9  fmul st(1)
        fld  dword ptr [ebx+0x10]       // 0x005cb2bb
        fmul st(0), st(3)               // 0x005cb2be  fmul st(3)
        faddp st(1), st(0)              // 0x005cb2c0
        fld  dword ptr [ebx+0x20]       // 0x005cb2c2
        fmul st(0), st(4)               // 0x005cb2c5  fmul st(4)
        faddp st(1), st(0)              // 0x005cb2c7
        fld  dword ptr [ebx+4]          // 0x005cb2c9
        fmul st(0), st(2)               // 0x005cb2cc  fmul st(2)
        fxch st(1)                      // 0x005cb2ce
        fstp dword ptr [ecx]            // 0x005cb2d0
        fld  dword ptr [ebx+0x14]       // 0x005cb2d2
        fmul st(0), st(3)               // 0x005cb2d5
        faddp st(1), st(0)              // 0x005cb2d7
        fld  dword ptr [ebx+0x24]       // 0x005cb2d9
        fmul st(0), st(4)               // 0x005cb2dc
        faddp st(1), st(0)              // 0x005cb2de
        fld  dword ptr [ebx+8]          // 0x005cb2e0
        fmulp st(2), st(0)              // 0x005cb2e3
        fstp dword ptr [ecx+4]          // 0x005cb2e5
        fld  dword ptr [ebx+0x18]       // 0x005cb2e8
        fmulp st(2), st(0)              // 0x005cb2eb
        faddp st(1), st(0)              // 0x005cb2ed
        fld  dword ptr [ebx+0x28]       // 0x005cb2ef
        fmulp st(2), st(0)              // 0x005cb2f2
        faddp st(1), st(0)              // 0x005cb2f4
        // ---- row 1 ----
        fld  dword ptr [eax+0x18]       // 0x005cb2f6
        fxch st(1)                      // 0x005cb2f9
        fld  dword ptr [eax+0x14]       // 0x005cb2fb
        fxch st(1)                      // 0x005cb2fe
        fld  dword ptr [eax+0x10]       // 0x005cb300
        fxch st(1)                      // 0x005cb303
        fstp dword ptr [ecx+8]          // 0x005cb305
        fld  dword ptr [ebx]            // 0x005cb308
        fmul st(0), st(1)               // 0x005cb30a
        fld  dword ptr [ebx+0x10]       // 0x005cb30c
        fmul st(0), st(3)               // 0x005cb30f
        faddp st(1), st(0)              // 0x005cb311
        fld  dword ptr [ebx+0x20]       // 0x005cb313
        fmul st(0), st(4)               // 0x005cb316
        faddp st(1), st(0)              // 0x005cb318
        fld  dword ptr [ebx+4]          // 0x005cb31a
        fmul st(0), st(2)               // 0x005cb31d
        fxch st(1)                      // 0x005cb31f
        fstp dword ptr [ecx+0x10]       // 0x005cb321
        fld  dword ptr [ebx+0x14]       // 0x005cb324
        fmul st(0), st(3)               // 0x005cb327
        faddp st(1), st(0)              // 0x005cb329
        fld  dword ptr [ebx+0x24]       // 0x005cb32b
        fmul st(0), st(4)               // 0x005cb32e
        faddp st(1), st(0)              // 0x005cb330
        fld  dword ptr [ebx+8]          // 0x005cb332
        fmulp st(2), st(0)              // 0x005cb335
        fstp dword ptr [ecx+0x14]       // 0x005cb337
        fld  dword ptr [ebx+0x18]       // 0x005cb33a
        fmulp st(2), st(0)              // 0x005cb33d
        faddp st(1), st(0)              // 0x005cb33f
        fld  dword ptr [ebx+0x28]       // 0x005cb341
        fmulp st(2), st(0)              // 0x005cb344
        faddp st(1), st(0)              // 0x005cb346
        // ---- row 2 ----
        fld  dword ptr [eax+0x28]       // 0x005cb348
        fxch st(1)                      // 0x005cb34b
        fld  dword ptr [eax+0x24]       // 0x005cb34d
        fxch st(1)                      // 0x005cb350
        fld  dword ptr [eax+0x20]       // 0x005cb352
        fxch st(1)                      // 0x005cb355
        fstp dword ptr [ecx+0x18]       // 0x005cb357
        fld  dword ptr [ebx]            // 0x005cb35a
        fmul st(0), st(1)               // 0x005cb35c
        fld  dword ptr [ebx+0x10]       // 0x005cb35e
        fmul st(0), st(3)               // 0x005cb361
        faddp st(1), st(0)              // 0x005cb363
        fld  dword ptr [ebx+0x20]       // 0x005cb365
        fmul st(0), st(4)               // 0x005cb368
        faddp st(1), st(0)              // 0x005cb36a
        fld  dword ptr [ebx+4]          // 0x005cb36c
        fmul st(0), st(2)               // 0x005cb36f
        fxch st(1)                      // 0x005cb371
        fstp dword ptr [ecx+0x20]       // 0x005cb373
        fld  dword ptr [ebx+0x14]       // 0x005cb376
        fmul st(0), st(3)               // 0x005cb379
        faddp st(1), st(0)              // 0x005cb37b
        fld  dword ptr [ebx+0x24]       // 0x005cb37d
        fmul st(0), st(4)               // 0x005cb380
        faddp st(1), st(0)              // 0x005cb382
        fld  dword ptr [ebx+8]          // 0x005cb384
        fmulp st(2), st(0)              // 0x005cb387
        fstp dword ptr [ecx+0x24]       // 0x005cb389
        fld  dword ptr [ebx+0x18]       // 0x005cb38c
        fmulp st(2), st(0)              // 0x005cb38f
        faddp st(1), st(0)              // 0x005cb391
        fld  dword ptr [ebx+0x28]       // 0x005cb393
        fmulp st(2), st(0)              // 0x005cb396
        faddp st(1), st(0)              // 0x005cb398
        // ---- row 3 (translation: the B translation row IS added) ----
        fld  dword ptr [eax+0x38]       // 0x005cb39a
        fxch st(1)                      // 0x005cb39d
        fld  dword ptr [eax+0x34]       // 0x005cb39f
        fxch st(1)                      // 0x005cb3a2
        fld  dword ptr [eax+0x30]       // 0x005cb3a4
        fxch st(1)                      // 0x005cb3a7
        fstp dword ptr [ecx+0x28]       // 0x005cb3a9
        fld  dword ptr [ebx]            // 0x005cb3ac
        fmul st(0), st(1)               // 0x005cb3ae
        fld  dword ptr [ebx+0x10]       // 0x005cb3b0
        fmul st(0), st(3)               // 0x005cb3b3
        faddp st(1), st(0)              // 0x005cb3b5
        fld  dword ptr [ebx+0x20]       // 0x005cb3b7
        fmul st(0), st(4)               // 0x005cb3ba
        faddp st(1), st(0)              // 0x005cb3bc
        fld  dword ptr [ebx+0x30]       // 0x005cb3be
        faddp st(1), st(0)              // 0x005cb3c1
        fld  dword ptr [ebx+4]          // 0x005cb3c3
        fmul st(0), st(2)               // 0x005cb3c6
        fxch st(1)                      // 0x005cb3c8
        fstp dword ptr [ecx+0x30]       // 0x005cb3ca
        fld  dword ptr [ebx+0x14]       // 0x005cb3cd
        fmul st(0), st(3)               // 0x005cb3d0
        faddp st(1), st(0)              // 0x005cb3d2
        fld  dword ptr [ebx+0x24]       // 0x005cb3d4
        fmul st(0), st(4)               // 0x005cb3d7
        faddp st(1), st(0)              // 0x005cb3d9
        fld  dword ptr [ebx+0x34]       // 0x005cb3db
        faddp st(1), st(0)              // 0x005cb3de
        fld  dword ptr [ebx+8]          // 0x005cb3e0
        fmulp st(2), st(0)              // 0x005cb3e3
        fstp dword ptr [ecx+0x34]       // 0x005cb3e5
        fld  dword ptr [ebx+0x18]       // 0x005cb3e8
        fmulp st(2), st(0)              // 0x005cb3eb
        faddp st(1), st(0)              // 0x005cb3ed
        fld  dword ptr [ebx+0x28]       // 0x005cb3ef
        fmulp st(2), st(0)              // 0x005cb3f2
        faddp st(1), st(0)              // 0x005cb3f4
        fld  dword ptr [ebx+0x38]       // 0x005cb3f6
        faddp st(1), st(0)              // 0x005cb3f9
        fstp dword ptr [ecx+0x38]       // 0x005cb3fb
        pop  ecx                        // 0x005cb3fe
        pop  ebx                        // 0x005cb3ff
        pop  eax                        // 0x005cb400
        pop  ebp                        // 0x005cb401
        ret                             // 0x005cb402
    }
}

// 0x004c4600 — the dispatcher, with the device call replaced by the CPU multiply.
// The two identity short-circuits and the `out->flags = A->flags & B->flags` tail are
// the original's (addresses in the header). `kIdentityMask` is the measured
// `[devBase+4]` = 0x00020000, which is also the `and eax, 0x20000` literal at
// 0x004c4622 — two independent witnesses for the same constant.
void* RwMatrixMultiplyCPUDispatch(void* out, const void* A, const void* B)
{
    const std::uint32_t kIdentityMask = 0x00020000u;
    std::uint32_t fa, fb;
    std::memcpy(&fa, static_cast<const char*>(A) + 0x0c, 4);   // 0x004c4612
    std::memcpy(&fb, static_cast<const char*>(B) + 0x0c, 4);   // 0x004c461f
    // `test ebx, eax / je` falls through when the bit is SET, so a SET bit means
    // "this matrix is the identity" and the product is just the other one.
    if ((fa & kIdentityMask) != 0u) {                          // 0x004c4627/29
        std::memcpy(out, B, 0x40);                             // 0x004c4636 rep movsd 0x10
        return out;
    }
    if ((fb & kIdentityMask) != 0u) {                          // 0x004c463d/3f
        std::memcpy(out, A, 0x40);                             // 0x004c464e rep movsd 0x10
        return out;
    }
    RwMatrixMultiplyCPU(static_cast<float*>(out),
                        static_cast<const float*>(A),
                        static_cast<const float*>(B));         // 0x004c465c
    const std::uint32_t fo = fb & fa;                          // 0x004c4663
    std::memcpy(static_cast<char*>(out) + 0x0c, &fo, 4);       // 0x004c4665
    return out;                                                // 0x004c4668 mov eax, esi
}

} // namespace Math
} // namespace mashed_re

// C-linkage export so the substitute can be measured against the ORIGINAL device
// routine through the existing lane (`re/frida/run_diff.py`, arg_type
// `matrix_multiply`, hook `rw_matrix_multiply_cpu`) rather than argued about. The
// original side of that diff is 0x005cb2a0 itself, read out of the live device table.
extern "C" __declspec(dllexport)
void __cdecl RwMatrixMultiplyCPU_C(void* out, const void* A, const void* B)
{
    mashed_re::Math::RwMatrixMultiplyCPU(static_cast<float*>(out),
                                         static_cast<const float*>(A),
                                         static_cast<const float*>(B));
}
