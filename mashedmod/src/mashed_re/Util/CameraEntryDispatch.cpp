// Mashed RE — CameraEntry::DispatchAll  (0x004464c0, util, C2->C3)
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// void FUN_004464c0(undefined4 param_1) — iterates all active camera-path
// entries (DAT_008964c0, stride 0xd8, count DAT_00898994) and dispatches
// each by the type field at entry+4:
//   type 0 -> FUN_00445aa0(entry, param_1)   [0x004464e0 CALL]
//   type 1 -> FUN_00441d40(entry, param_1)   [0x004464f0 CALL]
//   type 2 -> FUN_00442440(entry, param_1)   [0x00446500 CALL]
// Stride confirmed: ADD ESI,0xd8 at 0x0044650e.
// Entry count re-read each iteration: MOV EAX,[0x00898994] at 0x00446508.
//
// Calling convention: __cdecl.  param_1 at [ESP+0x10] after PUSH EBX/ESI/EDI
// (confirmed: 0x004464ce  MOV EDI,dword ptr [ESP+0x10]).
// Epilogue: POP EDI; POP ESI; POP EBX; RET  (0x00446518..0x0044651b).
//
// Callers (C2): FUN_00446520 (util), FUN_00448700 (vehicle).
// Callees (C2, original): FUN_00445aa0/FUN_00441d40/FUN_00442440.
//   Called as __cdecl pair (entry, param_1); caller cleans 8 bytes via ADD ESP,8.

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x004464c0
// CameraEntry::DispatchAll — dispatch loop over the camera-entry array.
extern "C" __declspec(dllexport) std::uint32_t __cdecl CameraEntryDispatchAll(std::uint32_t param_1)
{
    typedef void (__cdecl *tEntryFn)(std::uint8_t* entry, std::uint32_t p1);

    // 0x004464c0  MOV EAX,[0x00898994]
    std::int32_t count = *reinterpret_cast<std::int32_t*>(0x00898994u);
    // 0x004464c8  TEST EAX,EAX
    // 0x004464ca  JLE  0x0044651a  (skip loop if count <= 0)
    if (count <= 0)
        return static_cast<std::uint32_t>(count);

    // 0x004464d2  MOV ESI,0x8964c0  (base of entry array)
    std::uint8_t* esi = reinterpret_cast<std::uint8_t*>(0x008964c0u);
    int ebx = 0;  // 0x004464c6  XOR EBX,EBX  (loop counter)

    do {
        // 0x004464d7  MOV EAX,[ESI+0x4]  (read type field)
        std::uint32_t type = *reinterpret_cast<std::uint32_t*>(esi + 0x4u);

        // 0x004464da  TEST EAX,EAX
        // 0x004464dc  JNZ  0x004464e8
        if (type == 0u) {
            // 0x004464de  PUSH EDI ; PUSH ESI
            // 0x004464e0  CALL 0x00445aa0
            // 0x004464e5  ADD ESP,0x8
            reinterpret_cast<tEntryFn>(0x00445aa0u)(esi, param_1);
        }

        // 0x004464e8  CMP [ESI+0x4],0x1
        // 0x004464ec  JNZ  0x004464f8
        if (type == 1u) {
            // 0x004464ee  PUSH EDI ; PUSH ESI
            // 0x004464f0  CALL 0x00441d40
            // 0x004464f5  ADD ESP,0x8
            reinterpret_cast<tEntryFn>(0x00441d40u)(esi, param_1);
        }

        // 0x004464f8  CMP [ESI+0x4],0x2
        // 0x004464fc  JNZ  0x00446508
        if (type == 2u) {
            // 0x004464fe  PUSH EDI ; PUSH ESI
            // 0x00446500  CALL 0x00442440
            // 0x00446505  ADD ESP,0x8
            reinterpret_cast<tEntryFn>(0x00442440u)(esi, param_1);
        }

        // 0x00446508  MOV EAX,[0x00898994]  (re-read count each iteration)
        count = *reinterpret_cast<std::int32_t*>(0x00898994u);
        // 0x0044650d  INC EBX
        ebx++;
        // 0x0044650e  ADD ESI,0xd8  (advance to next entry)
        esi += 0xd8u;
        // 0x00446514  CMP EBX,EAX
        // 0x00446516  JL  0x004464d7
    } while (ebx < count);

    // EAX = count at return (last re-read; callers don't use return value)
    return static_cast<std::uint32_t>(count);
}
RH_ScopedInstall(CameraEntryDispatchAll, 0x004464c0);
