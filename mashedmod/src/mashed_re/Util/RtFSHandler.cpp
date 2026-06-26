// Mashed RE — RtFSHandler IsEOF predicate.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file:
//   0x005514e0  RtFSHandler::IsEOF  — EOF predicate; compares 64-bit position
//                                     vs 64-bit file size in the slot struct.
//
// Analysis:
//   re/analysis/bucket_util_00095280_0040e460/001514e0.md
//   re/analysis/piz_fsmanager_handler/REPORT.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// RtFSHandler::IsEOF  --  0x005514e0
//
// Original: FUN_005514e0 (39 bytes, 0x005514e0..0x00551507)
// Calling convention: __cdecl (stack arg; plain RET — no stack cleanup by callee).
// Signature: int __cdecl FUN_005514e0(int* slot)
//
// Body (verbatim from disasm at 0x005514e0..0x00551507):
//   MOV EAX, [ESP+0x4]      ; EAX = slot ptr (arg1)
//   PUSH ESI
//   MOV ECX, [EAX+0x8]      ; posLow  = slot[2]
//   MOV EDX, [EAX+0xc]      ; posHigh = slot[3]
//   MOV ESI, [EAX+0x4]      ; sizeHigh = slot[1]
//   CMP ECX, -1             ; if posLow == 0xffffffff -> EOF sentinel
//   JZ  LAB_00551501        ; -> return 1
//   CMP EDX, ESI            ; signed compare posHigh vs sizeHigh
//   JG  LAB_00551501        ; posHigh > sizeHigh -> EOF
//   JNZ LAB_005514fd        ; posHigh != sizeHigh (i.e. < sizeHigh) -> not EOF
//   CMP ECX, [EAX]          ; posLow vs sizeLow (slot[0])
//   JGE LAB_00551501        ; posLow >= sizeLow -> EOF
//   LAB_005514fd: XOR EAX,EAX / POP ESI / RET   -> return 0
//   LAB_00551501: MOV EAX,1 / POP ESI / RET     -> return 1
//
// Slot struct layout (4-byte fields at indices 0..3):
//   slot[0] = +0x0 : sizeLow  (low  32-bits of file size; set by Open/GetFileSize)
//   slot[1] = +0x4 : sizeHigh (high 32-bits of file size)
//   slot[2] = +0x8 : posLow   (low  32-bits of current position; SetFilePointer)
//   slot[3] = +0xc : posHigh  (high 32-bits of current position)
//   -1 (0xffffffff) posLow sentinel = SetFilePointer error; treated as EOF.
//
// Returns:
//   0 — not EOF: position (64-bit signed) < size (64-bit)
//   1 — EOF or invalid position (posLow sentinel, posHigh > sizeHigh,
//       or posHigh == sizeHigh && posLow >= sizeLow)
//
// Uncertainties:
//   U-5901 — function boundary absent in pool0 clone (created via function_create
//             during analysis; see 001514e0.md); non-blocking.
//
// ref: re/analysis/bucket_util_00095280_0040e460/001514e0.md
// ---------------------------------------------------------------------------

// 0x005514e0
extern "C" __declspec(dllexport) int __cdecl RtFSHandler_IsEOF(int* slot) {
    // posLow  = slot[2]: current position low 32-bits.  [EAX+0x8] @ 0x005514e5
    // posHigh = slot[3]: current position high 32-bits. [EAX+0xc] @ 0x005514e8
    // sizeHigh = slot[1]: file size high 32-bits.       [EAX+0x4] @ 0x005514eb
    int posLow   = slot[2];
    int posHigh  = slot[3];
    int sizeHigh = slot[1];

    // 0x005514ee: CMP ECX,-1 / JZ LAB_00551501
    // posLow == -1 (0xffffffff) is the SetFilePointer error sentinel -> EOF.
    if (posLow == -1)
        return 1;

    // 0x005514f3: CMP EDX,ESI / JG LAB_00551501
    // posHigh > sizeHigh (signed) -> position is past end -> EOF.
    if (posHigh > sizeHigh)
        return 1;

    // 0x005514f7: JNZ LAB_005514fd
    // posHigh != sizeHigh means posHigh < sizeHigh (signed) -> not EOF.
    if (posHigh != sizeHigh)
        return 0;

    // posHigh == sizeHigh: compare low halves.
    // 0x005514f9: CMP ECX,[EAX] (posLow vs slot[0]=sizeLow) / JGE LAB_00551501
    if (posLow >= slot[0])
        return 1;

    // 0x005514fd: XOR EAX,EAX -> return 0 (not EOF: position < size).
    return 0;
}

RH_ScopedInstall(RtFSHandler_IsEOF, 0x005514e0);
