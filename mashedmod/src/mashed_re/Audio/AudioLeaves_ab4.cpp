// Mashed RE — Audio C2->C3 leaf batch (c3-batch-ab session 4).
//
// SHA-256 anchor: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//                 (preserved in original\MASHED.exe.unpatched)
//
// Six audio-cluster leaves from bucket re/analysis/bucket_audio_005bf4d0_005c9770/:
//   0x005bf660  Audio3DwordZero       — __cdecl  void(ptr): zero 3 consecutive dwords
//   0x005bfcc0  AudioCounterPairGet   — __stdcall int(ptr,u64*,u64*): two counters, 64-bit out
//   0x005c7500  AudioMixerRateCompute — __cdecl  void(ptr): fixed-point (int<<32)/divisor
//   0x005c75b0  AudioVoiceField8cGet  — __cdecl  u32(ptr): return *(ptr+0x8c)
//   0x005c9380  AudioBitBufSizeCalc   — __cdecl  u32(bits): max(1,bits>>3)+0xc
//   0x005c9770  AudioPcmPackSaturate  — __cdecl  void(i16*,i32*,count): saturating i32->i16 pack
//
// Every reimpl matches the original's calling convention (verified from the
// disassembly) so the inline-JMP install is ABI-correct, and is bit-identity
// verified via re/frida/run_diff_warm.py against the original.
#include "../Core/HookSystem.h"
#include <cstdint>

// ─────────────────────────────────────────────────────────────────────────────
// 0x005bf660  FUN_005bf660  Audio3DwordZero
// 1-arg __cdecl leaf. Zeroes three consecutive 4-byte slots of param_1.
//
// ASM (0x005bf660..0x005bf66e):
//   MOV EAX,[ESP+4]        ; param_1 (stack -> __cdecl)
//   XOR ECX,ECX
//   MOV [EAX],ECX          ; param_1[0] = 0
//   MOV [EAX+4],ECX        ; param_1[1] = 0
//   MOV [EAX+8],ECX        ; param_1[2] = 0
//   RET                    ; no immediate -> __cdecl
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport)
void __cdecl Audio3DwordZero(uint32_t* param_1) {
    param_1[0] = 0u;
    param_1[1] = 0u;
    param_1[2] = 0u;
}

// ─────────────────────────────────────────────────────────────────────────────
// 0x005bfcc0  FUN_005bfcc0  AudioCounterPairGet
// 3-arg __stdcall (RET 0xc). Reads the 32-bit fields at +0x15c and +0x160 of
// param_1 and writes two 64-bit outputs (high dword forced 0):
//   *param_2 = (u64) *(param_1+0x15c)
//   *param_3 = (u64)((u32)*(param_1+0x15c) - (u32)*(param_1+0x160))   (high=0)
//   return 0
//
// ASM (0x005bfcc0..0x005bfcf0):
//   MOV EAX,[ESP+4]   ; param_1
//   MOV ECX,[ESP+8]   ; param_2 (out1)
//   PUSH ESI
//   MOV EDX,[EAX+0x15c]
//   MOV [ECX],EDX     ; out1.lo = +0x15c
//   XOR EDX,EDX
//   MOV [ECX+4],EDX   ; out1.hi = 0
//   MOV ECX,[EAX+0x15c]
//   MOV ESI,[EAX+0x160]
//   MOV EAX,[ESP+0x10]; param_3 (out2)  (after PUSH ESI; orig [ESP+0xc])
//   SUB ECX,ESI       ; (+0x15c) - (+0x160)
//   POP ESI
//   MOV [EAX],ECX     ; out2.lo = difference
//   MOV [EAX+4],EDX   ; out2.hi = 0
//   XOR EAX,EAX       ; return 0
//   RET 0xc           ; callee cleans 12 bytes -> __stdcall
//
// The high dword of out2 is unconditionally 0 (EDX), so this is NOT a true
// 64-bit subtract — the difference is the 32-bit wrap of the two counters.
// ref: re/analysis/bucket_audio_005bf4d0_005c9770/0x005bfcc0.md
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport)
int __stdcall AudioCounterPairGet(uint32_t* param_1, uint64_t* param_2, uint64_t* param_3) {
    const uint32_t c0 = param_1[0x15c / 4];   // *(param_1 + 0x15c)
    const uint32_t c1 = param_1[0x160 / 4];   // *(param_1 + 0x160)
    *param_2 = static_cast<uint64_t>(c0);             // lo=c0, hi=0
    *param_3 = static_cast<uint64_t>(c0 - c1);        // lo=c0-c1 (u32 wrap), hi=0
    return 0;
}

// ─────────────────────────────────────────────────────────────────────────────
// 0x005c7500  FUN_005c7500  AudioMixerRateCompute
// 1-arg __cdecl. Computes a 64-bit fixed-point value and stores it at +0x80.
//
// ASM (0x005c7500..0x005c7564):
//   FLD  [ESI+0x38]            ; ST = (float)*(p+0x38)
//   FMUL [ESI+0x34]            ; ST = *(p+0x38) * *(p+0x34)
//   FSTP [ESP+8]              ; round product to float32
//   FNSTCW / OR AH,0xc / FLDCW; set rounding = toward zero (truncate)
//   FISTP [ESP+0xc]          ; ip = (int32) trunc(product)
//   FLDCW                     ; restore control word
//   EAX = [ESI+0x9c]          ; nested ptr
//   ECX = [EAX+0x18]          ; divisor (32-bit)
//   __aulldiv( dividend = (lo=0, hi=ip),   divisor = (lo=ECX, hi=0) )
//        -> EDX:EAX = ((u32)ip << 32) / divisor   (unsigned 64-bit)
//   [ESI+0x80] = EAX ; [ESI+0x84] = EDX           ; store 64-bit quotient
//   MOV word [ESI+0x80],0                          ; clear low 16 bits
//   RET                                            ; __cdecl
//
// The FISTP truncates toward zero (OR AH,0xc -> RC=11b), matching a C
// float->int cast. The 64-bit dividend is the integer placed in the HIGH
// dword (low dword 0): dividend = (u32)ip << 32.
// [UNCERTAIN U-7208] fixed-point format of the +0x80 result / units of the
//   divisor are data-semantic; the arithmetic is fully determinate.
// ref: re/analysis/bucket_audio_005bf4d0_005c9770/0x005c7500.md
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport)
void __cdecl AudioMixerRateCompute(uint8_t* param_1) {
    const float f34 = *reinterpret_cast<const float*>(param_1 + 0x34);
    const float f38 = *reinterpret_cast<const float*>(param_1 + 0x38);
    const float product = f38 * f34;                 // rounds to float32 (matches FSTP)
    const int32_t ip = static_cast<int32_t>(product);// truncate toward zero (FISTP RC=chop)

    const uint32_t  nested  = *reinterpret_cast<const uint32_t*>(param_1 + 0x9c);
    const uint32_t  divisor = *reinterpret_cast<const uint32_t*>(nested + 0x18);

    const uint64_t dividend = static_cast<uint64_t>(static_cast<uint32_t>(ip)) << 32;
    const uint64_t quotient = dividend / static_cast<uint64_t>(divisor);

    *reinterpret_cast<uint64_t*>(param_1 + 0x80) = quotient;
    *reinterpret_cast<uint16_t*>(param_1 + 0x80) = 0u;  // clear low 16 bits
}

// ─────────────────────────────────────────────────────────────────────────────
// 0x005c75b0  FUN_005c75b0  AudioVoiceField8cGet
// 1-arg __cdecl getter: return *(uint32*)(param_1 + 0x8c).
//
// ASM (0x005c75b0..0x005c75ba):
//   MOV EAX,[ESP+4]      ; param_1 (__cdecl)
//   MOV EAX,[EAX+0x8c]
//   RET
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport)
uint32_t __cdecl AudioVoiceField8cGet(uint8_t* param_1) {
    return *reinterpret_cast<const uint32_t*>(param_1 + 0x8c);
}

// ─────────────────────────────────────────────────────────────────────────────
// 0x005c9380  FUN_005c9380  AudioBitBufSizeCalc
// 1-arg __cdecl pure size calc: (bits >> 3), clamp-to-1 if zero, + 0xc.
//
// ASM (0x005c9380..0x005c9391):
//   MOV EAX,[ESP+4]
//   SHR EAX,3            ; logical shift (unsigned /8)
//   JNZ +5
//   MOV EAX,1            ; clamp 0 -> 1
//   ADD EAX,0xc          ; + 12-byte header
//   RET
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport)
uint32_t __cdecl AudioBitBufSizeCalc(uint32_t bits) {
    uint32_t bytes = bits >> 3;     // logical shift
    if (bytes == 0u) {
        bytes = 1u;
    }
    return bytes + 0xcu;
}

// ─────────────────────────────────────────────────────────────────────────────
// 0x005c9770  FUN_005c9770  AudioPcmPackSaturate
// 3-arg __cdecl saturating int32 -> int16 PCM pack (mixer output conversion).
//   param_1 = int16* dst, param_2 = int32* src, param_3 = count.
// Per source sample s (int32):
//   s <  -0x800000   -> 0x8000  (int16 min)
//   s <   0x7fff01   -> (int16)((u32)s >> 8)
//   else             -> 0x7fff  (int16 max)
// Hand-unrolled: main loop (count>>1 iters) packs 4 samples/iter; tail loop
// packs 2 samples/iter for the odd remainder. Net total = 2*count samples.
// Ported verbatim from the decompilation to preserve write ordering / count.
// ref: re/analysis/bucket_audio_005bf4d0_005c9770/0x005c9770.md
// ─────────────────────────────────────────────────────────────────────────────
static inline uint16_t PcmSat(int32_t s) {
    if (s < -0x800000) {
        return 0x8000u;
    } else if (s < 0x7fff01) {
        return static_cast<uint16_t>(static_cast<uint32_t>(s) >> 8);
    }
    return 0x7fffu;
}

extern "C" __declspec(dllexport)
void __cdecl AudioPcmPackSaturate(uint16_t* param_1, int32_t* param_2, uint32_t param_3) {
    uint32_t uVar2 = 0u;
    for (uint32_t uVar5 = param_3 >> 1; uVar5 != 0u; uVar5 = uVar5 - 1u) {
        param_1[0] = PcmSat(param_2[0]);
        param_1[1] = PcmSat(param_2[1]);
        param_1[2] = PcmSat(param_2[2]);
        param_1[3] = PcmSat(param_2[3]);
        param_2 = param_2 + 4;
        param_1 = param_1 + 4;
        uVar2 = param_3 >> 1;
    }
    if (uVar2 * 2u < param_3) {
        int32_t iVar4 = static_cast<int32_t>(param_3 + uVar2 * (uint32_t)(-2));
        do {
            param_1[0] = PcmSat(param_2[0]);
            param_1[1] = PcmSat(param_2[1]);
            param_2 = param_2 + 2;
            param_1 = param_1 + 2;
            iVar4 = iVar4 - 1;
        } while (iVar4 != 0);
    }
}

// ─── Hook registration ───────────────────────────────────────────────────────
// All six are bit-identity verified leaves (run_diff_warm.py, c3-batch-ab-s4).
RH_ScopedInstall(Audio3DwordZero,       0x005bf660);
RH_ScopedInstall(AudioCounterPairGet,   0x005bfcc0);
RH_ScopedInstall(AudioMixerRateCompute, 0x005c7500);
RH_ScopedInstall(AudioVoiceField8cGet,  0x005c75b0);
RH_ScopedInstall(AudioBitBufSizeCalc,   0x005c9380);
RH_ScopedInstall(AudioPcmPackSaturate,  0x005c9770);
