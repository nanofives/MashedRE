// Mashed RE — Audio/AudioLinearToLog.cpp
// 0x005aeb90 split out of the asi-only Audio/TimerGetters_ah5.cpp (D4-AUDIO-VOLTODS, W0-2) so it links into the exe. In
// both rsp files. Body unchanged except the two .rdata operands, which the exe reads from private copies (below).
// Plate: re/analysis/bucket_audio_005ab710_005af040/0x005aeb90.md. Direct callers (E8 scan of MASHED.exe.unpatched):
// 0x005b8a1b, 0x005b8f62, 0x005b955f, 0x005b96b7. At 0x005b8f62 the result is pushed into `call [edx+0x3c]` on
// [esi+0xdc] (0x005b8f74), vtable offset 0x3c = slot 15, which is SetVolume in IDirectSoundBuffer's layout (the object's
// type is not read here).
// In the exe AudioEngine's VolToDs calls it under MASHED_VOLTODS_ORIG=1 (default OFF).
#include "../Core/HookSystem.h"
#include <cstdint>

#ifdef MASHED_STANDALONE
// 0x005cc990 (.rdata) = 0x3727c5ac (1e-5f), 0x005cd0b8 (.rdata) = 0x44fa0000 (2000.0f), read from MASHED.exe.unpatched
// (verify/audiovoltods_20261024/consts.txt).
extern "C" const uint32_t g_audioLogFloor_005cc990 = 0x3727c5acu;
extern "C" const uint32_t g_audioLogScale_005cd0b8 = 0x44fa0000u;
#endif

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
#ifdef MASHED_STANDALONE
        lea     edx, g_audioLogFloor_005cc990
#else
        mov     edx, 0x005cc990
#endif
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
#ifdef MASHED_STANDALONE
        lea     edx, g_audioLogScale_005cd0b8
#else
        mov     edx, 0x005cd0b8
#endif
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
