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
// [D-11073 leg 2, 2026-10-09] Now in BOTH targets. Exe arm: type 0 -> CamEntryType0Step445aa0
// (Race/CameraEntries.cpp); types 1/2 (FUN_00441d40 / FUN_00442440) are NOT called in the exe:
// visual only, neither writes the camera struct (BRIEF_D11073 §1), and neither has an exe body.
// The type field is RE-READ before each test, as the bytes do ([ESI+4] at 0x004464d7, 0x004464e8,
// 0x004464f8); the earlier copy cached it.
// Callees (C2, original): FUN_00445aa0/FUN_00441d40/FUN_00442440.
//   Called as __cdecl pair (entry, param_1); caller cleans 8 bytes via ADD ESP,8.

#include "../Core/HookSystem.h"
#include "../Race/CameraEntries.h"

#include <cstdint>

// [a2-camera 2026-10-12] Dev .asi arm is now NAKED and byte-for-byte the original stream.
// Defect it replaces (re/analysis/negctl/followup_20261012/a2-camera_RESULT.md): MSVC compiled the C
// loop with a 16-byte frame (PUSH EBX/EBP/ESI/EDI; EBP held 0x898994), the original keeps 12
// (PUSH EBX @0x004464c5, ESI @0x004464cc, EDI @0x004464cd). Every callee therefore ran 4 bytes lower
// on the stack and copied different uninitialized stack dwords into the RwMatrix pad slots of the
// entry (entry+0x68 / +0x78 / +0x88). Live race, 7 entries (types 0,1,1,1,2,2,3): a uniform +4 frame
// alone reproduced the port's diff exactly (e0+0x88 via 0x00445aa0, e4/e5+0x68 via 0x00442440);
// the port's EAX/ECX/EDX/EBP values at each CALL changed nothing. No callee port is involved: the
// .asi arm always called the ORIGINAL 0x00445aa0 / 0x00441d40 / 0x00442440.
// The standalone arm keeps the C body (callees are not at their RVA there).

#ifndef MASHED_STANDALONE
static void* const s_call_00445aa0 = reinterpret_cast<void*>(0x00445aa0u);
static void* const s_call_00441d40 = reinterpret_cast<void*>(0x00441d40u);
static void* const s_call_00442440 = reinterpret_cast<void*>(0x00442440u);

// 0x004464c0
// CameraEntry::DispatchAll — dispatch loop over the camera-entry array.
// Returns EAX as the bytes leave it: the count read at 0x004464c0 (count <= 0) or re-read at 0x00446508.
extern "C" __declspec(dllexport) __declspec(naked) std::uint32_t __cdecl CameraEntryDispatchAll(std::uint32_t /*param_1*/)
{
    __asm {
        mov  eax, dword ptr ds:[898994h]    // 0x004464c0
        push ebx                            // 0x004464c5
        xor  ebx, ebx                       // 0x004464c6
        test eax, eax                       // 0x004464c8
        jle  L_END                          // 0x004464ca
        push esi                            // 0x004464cc
        push edi                            // 0x004464cd
        mov  edi, dword ptr [esp+10h]       // 0x004464ce  param_1
        mov  esi, 8964C0h                   // 0x004464d2  entry array base
    L_LOOP:
        mov  eax, dword ptr [esi+4]         // 0x004464d7  type
        test eax, eax                       // 0x004464da
        jne  L_T1                           // 0x004464dc
        push edi                            // 0x004464de
        push esi                            // 0x004464df
        call dword ptr [s_call_00445aa0]    // 0x004464e0  CALL 0x00445aa0
        add  esp, 8                         // 0x004464e5
    L_T1:
        cmp  dword ptr [esi+4], 1           // 0x004464e8
        jne  L_T2                           // 0x004464ec
        push edi                            // 0x004464ee
        push esi                            // 0x004464ef
        call dword ptr [s_call_00441d40]    // 0x004464f0  CALL 0x00441d40
        add  esp, 8                         // 0x004464f5
    L_T2:
        cmp  dword ptr [esi+4], 2           // 0x004464f8
        jne  L_NEXT                         // 0x004464fc
        push edi                            // 0x004464fe
        push esi                            // 0x004464ff
        call dword ptr [s_call_00442440]    // 0x00446500  CALL 0x00442440
        add  esp, 8                         // 0x00446505
    L_NEXT:
        mov  eax, dword ptr ds:[898994h]    // 0x00446508  count re-read
        inc  ebx                            // 0x0044650d
        add  esi, 0D8h                      // 0x0044650e  stride 0xd8
        cmp  ebx, eax                       // 0x00446514
        jl   L_LOOP                         // 0x00446516
        pop  edi                            // 0x00446518
        pop  esi                            // 0x00446519
    L_END:
        pop  ebx                            // 0x0044651a
        ret                                 // 0x0044651b
    }
}
#else
// 0x004464c0
// CameraEntry::DispatchAll — dispatch loop over the camera-entry array.
extern "C" __declspec(dllexport) std::uint32_t __cdecl CameraEntryDispatchAll(std::uint32_t param_1)
{
    // 0x004464c0  MOV EAX,[0x00898994]
    std::int32_t count = *reinterpret_cast<std::int32_t*>(0x00898994u);
    // 0x004464c8  TEST EAX,EAX / 0x004464ca  JLE 0x0044651a
    if (count <= 0)
        return static_cast<std::uint32_t>(count);

    std::uint8_t* esi = reinterpret_cast<std::uint8_t*>(0x008964c0u);   // 0x004464d2
    int ebx = 0;                                                       // 0x004464c6
    do {
        // 0x004464d7..0x004464e5  type 0 -> CALL 0x00445aa0
        if (*reinterpret_cast<std::uint32_t*>(esi + 0x4u) == 0u)
            CamEntryType0Step445aa0(esi, reinterpret_cast<std::int32_t*>(param_1));
        // 0x004464e8 / 0x004464f8: types 1 (0x00441d40) and 2 (0x00442440) have no exe body; see header.
        count = *reinterpret_cast<std::int32_t*>(0x00898994u);           // 0x00446508
        ebx++;                                                         // 0x0044650d
        esi += 0xd8u;                                                  // 0x0044650e
    } while (ebx < count);                                             // 0x00446514 / 0x00446516
    return static_cast<std::uint32_t>(count);
}
#endif
RH_ScopedInstall(CameraEntryDispatchAll, 0x004464c0);
