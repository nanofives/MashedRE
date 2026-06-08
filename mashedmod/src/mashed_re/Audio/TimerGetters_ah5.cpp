// Mashed RE - c3_batch_ah s5: audio root/timer getters + linear->log leaf.
// Bit-faithful ports of audio-module pure leaves. Decomp pool14 2026-06-08
// (matches re/analysis/bucket_audio_005a7b60_005ab620/{005aad40,005ab070}.md
//  and bucket_audio_005ab710_005af040/0x005aeb90.md).
//
// DEFERRED in this session (not authored here):
//   0x005ab010 AudioElapsedTicks      - reads a MOVING counter (FUN_005ab040)
//       minus a baseline global; read_global seeds one global but cannot
//       capture a moving-counter diff deterministically -> racy/non-det.
//   0x005aaff0 AudioTimerBaselineInit - writes counter@now into dcdf0 and 0
//       into dcdf4; void_write_observe on dcdf4 is degenerate (always 0),
//       on dcdf0 is racy (counter moves between orig/reimpl calls).
//   (2026-06-08 harness/scenario-attach) the three voice field-getters below
//   are now promotable via the new thiscall_field_get arg_type (synthetic
//   struct-seed, plain menu-attach) — authored + registered here.
#include "../Core/HookSystem.h"
#include <cstdint>

// 0x005aad40  FUN_005aad40  (6 bytes)  undefined4(void)
// MOV EAX,[0x007dcd20]; RET. Audio root/system object pointer getter.
extern "C" __declspec(dllexport) uint32_t __cdecl AudioRootGet(void) {
    return *reinterpret_cast<volatile uint32_t*>(0x007dcd20);
}

RH_ScopedInstall(AudioRootGet, 0x005aad40);  // c3_batch_ah s5

// 0x005ab070  FUN_005ab070  (21 bytes)  undefined4(void)
// uVar1 = DAT_007dcdf8; if (DAT_007dce00 != 1) uVar1 = 1000; return uVar1.
// Timer tick-rate getter; literal 1000 fallback when mode flag != 1.
extern "C" __declspec(dllexport) uint32_t __cdecl AudioTimerRateGet(void) {
    uint32_t uVar1 = *reinterpret_cast<volatile uint32_t*>(0x007dcdf8);
    if (*reinterpret_cast<volatile uint32_t*>(0x007dce00) != 1) {
        uVar1 = 1000;
    }
    return uVar1;
}

RH_ScopedInstall(AudioTimerRateGet, 0x005ab070);  // c3_batch_ah s5

// 0x005aeb90  FUN_005aeb90  (97 bytes)  uint(float)
// Linear amplitude -> log (millibel-style) integer, clamped to <= 0.
//   if (x < [0x005cc990]) return 0xffffd8f0;            (-10000 floor)
//   r = TRUNC( log10(x) * [0x005cd0b8] );                (x87: FLDLG2/FYL2X/FMUL)
//   return (r > 0) ? 0 : r;                              (AND with (SETG-1))
// NOTE: the Ghidra C shows `x * 0.30103 * scale`, but the disassembly is
//   FLDLG2 + FYL2X = log10(x); the *0.30103 is a decompiler artifact.
//   FISTP runs with the control word OR'd 0x0c00 = round-toward-zero (trunc).
// Reproduced verbatim in inline x87 asm for bit-exact match (x86 /MSVC).
extern "C" __declspec(dllexport) uint32_t __cdecl AudioLinearToLog(float param_1) {
    uint32_t result;
    float    scaled;       // post-FMUL log10(x)*scale (was [ESP+0x4])
    uint16_t cwSaved;      // saved control word    (was [ESP+0x2])
    uint16_t cwTrunc;      // modified control word  (was [ESP+0x10])
    int32_t  rounded;      // FISTP target           (was [ESP+0x8])
    __asm {
        fld     dword ptr [param_1]
        mov     edx, 0x005cc990
        fcomp   dword ptr [edx]
        fnstsw  ax
        test    ah, 0x5
        jp      compute
        mov     eax, 0xffffd8f0
        mov     result, eax
        jmp     done
    compute:
        fldlg2
        fld     dword ptr [param_1]
        fyl2x
        mov     edx, 0x005cd0b8
        fmul    dword ptr [edx]
        fstp    dword ptr [scaled]
        fnstcw  word ptr [cwSaved]
        fld     dword ptr [scaled]
        mov     ax, word ptr [cwSaved]
        or      ah, 0x0c
        mov     word ptr [cwTrunc], ax
        fldcw   word ptr [cwTrunc]
        fistp   dword ptr [rounded]
        fldcw   word ptr [cwSaved]
        mov     eax, dword ptr [rounded]
        xor     ecx, ecx
        test    eax, eax
        setg    cl
        dec     ecx
        and     eax, ecx
        mov     result, eax
    done:
    }
    return result;
}

RH_ScopedInstall(AudioLinearToLog, 0x005aeb90);  // c3_batch_ah s5

// Voice field-getter triplet. __cdecl, single struct/this pointer on the stack
// (disasm: MOV EAX,[ESP+4]; MOV EAX,[EAX+off]; RET), NOT an ECX thiscall.
// Verbatim leaf: return *(uint32_t*)((char*)this + off).
// (harness/scenario-attach 2026-06-08; verified pool14.)

// 0x005a89a0  FUN_005a89a0  (11 bytes)  undefined4(this)  -> *(this+0xD6C)
extern "C" __declspec(dllexport) uint32_t __cdecl VoiceFieldD6cGet(void* self) {
    return *reinterpret_cast<uint32_t*>(reinterpret_cast<char*>(self) + 0xD6C);
}
RH_ScopedInstall(VoiceFieldD6cGet, 0x005a89a0);

// 0x005a89b0  FUN_005a89b0  (11 bytes)  undefined4(this)  -> *(this+0xD70)
extern "C" __declspec(dllexport) uint32_t __cdecl VoiceFieldD70Get(void* self) {
    return *reinterpret_cast<uint32_t*>(reinterpret_cast<char*>(self) + 0xD70);
}
RH_ScopedInstall(VoiceFieldD70Get, 0x005a89b0);

// 0x005a89c0  FUN_005a89c0  (11 bytes)  undefined4(this)  -> *(this+0xD74)
extern "C" __declspec(dllexport) uint32_t __cdecl VoiceFieldD74Get(void* self) {
    return *reinterpret_cast<uint32_t*>(reinterpret_cast<char*>(self) + 0xD74);
}
RH_ScopedInstall(VoiceFieldD74Get, 0x005a89c0);

// 0x005ab370  FUN_005ab370  (7 bytes)  undefined4(this)  -> *(this+0x10)
// Same __cdecl single-stack-ptr field-getter shape (MOV EAX,[ESP+4];
// MOV EAX,[EAX+0x10]; RET, pool14). Audio channel-meta accessor.
extern "C" __declspec(dllexport) uint32_t __cdecl ChannelField10Get(void* self) {
    return *reinterpret_cast<uint32_t*>(reinterpret_cast<char*>(self) + 0x10);
}
RH_ScopedInstall(ChannelField10Get, 0x005ab370);
