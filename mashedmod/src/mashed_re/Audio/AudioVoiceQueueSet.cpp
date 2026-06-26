// Mashed RE — FUN_005a8960 C2->C3 promotion (audio: mutex-guarded voice-queue set).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// 0x005a8960  audio  55 bytes
//   fn(base, val): WaitForSingleObject(DAT_007dcae0,INFINITE);
//                  FUN_005b0dc0(base+0x154, val);  (* q[1]=val; q[0]|=0x80 *)
//                  ReleaseSemaphore(DAT_007dcae0,1,0);
//
//   DAT_007dcae0 @ 0x007dcae0: global audio semaphore HANDLE.
//   FUN_005b0dc0 @ 0x005b0dc0: *(p+4)=val; *p|=0x80  (C3 callee, inlined here).
//   Caller: FUN_0045dc80 (4-slot stream play, C2).

#include "../Core/HookSystem.h"
#include <cstdint>

// 0x005a8960
extern "C" __declspec(dllexport) void __cdecl AudioVoiceQueueSet5a8960(int param_1, std::uint32_t param_2) {
    HANDLE hSem = *reinterpret_cast<HANDLE*>(0x007dcae0u);      // DAT_007dcae0: global audio semaphore
    WaitForSingleObject(hSem, INFINITE);                         // acquire lock
    // FUN_005b0dc0(param_1+0x154, param_2): q[1]=param_2; q[0]|=0x80
    auto* q = reinterpret_cast<std::uint32_t*>(
                  static_cast<std::uint32_t>(param_1) + 0x154u);
    q[1] = param_2;   // *(param_1+0x158) = param_2  [0x005b0dc0: MOV [EAX+4],ECX]
    q[0] |= 0x80u;    // *(param_1+0x154) |= 0x80    [0x005b0dc0: OR CL,0x80;MOV [EAX],ECX]
    ReleaseSemaphore(hSem, 1, nullptr);                          // release lock
}
RH_ScopedInstall(AudioVoiceQueueSet5a8960, 0x005a8960);
