// Mashed RE - decompose a replay tick count into minutes / seconds / fraction.
// Original: 0x00411350  Replay::TimeFormat  vehicle  C2 -> C3
// Raw-listing plate: re/analysis/replay_record/0x00411350_RAW.md (orch-iter20)
//
//     v        = (double)(uint32)param_1 * (1/3000);
//     *param_3 = (int)v;                    // truncation, via __ftol
//     *param_2 = (float)(v - (int)v);       // fractional second
//     *param_4 = 0;
//     while (*param_3 > 59) { (*param_4)++; *param_3 -= 60; }
//
// THIS FUNCTION HAS NO x87 INPUT ARGUMENT, contrary to every prior description
// of it. The plate, the iter13 handler brief and the hooks.csv note all claimed
// an "implicit-ST0 FPU input arg (param_1 raw_ticks pushed as float)", citing
// the decompiler's `extraout_ST0`, and the row was REFUSED for promotion in
// c3-batch-j-s3 on that basis. The raw listing shows:
//
//     0x00411354  FILD dword ptr [ESP+4]
//
// an INTEGER load from the STACK. `extraout_ST0` is an artifact of Ghidra
// failing to model the FLD ST0 duplication at 0x00411368 against __ftol's pop -
// not evidence of an incoming argument. All three artefacts were written from
// decompiler output; none quoted a listing. Same defect class as 0x00482900.
//
// PARAM_1 IS UNSIGNED. `FILD` loads a signed int32, and the
// TEST/JGE/FADD 2^32 at 0x00411358..0x0041135c is the canonical MSVC
// unsigned-to-float fixup: a negative signed reading is corrected by adding
// 2^32. Typing it `int` breaks for every value >= 0x80000000.
//
// Disasm at 0x00411350..0x004113a6 (87 bytes) - full listing in the plate:
//   0x00411350  MOV   EAX,[ESP+4]              ; param_1 as an integer
//   0x00411354  FILD  dword ptr [ESP+4]        ; ST0 = (int32)param_1
//   0x00411358  TEST  EAX,EAX
//   0x0041135a  JGE   0x00411362
//   0x0041135c  FADD  float ptr [0x005cc94c]   ; += 4294967296.0 (2^32)
//   0x00411362  FMUL  float ptr [0x005cc948]   ; *= 3.3333333e-4 (1/3000)
//   0x00411368  FLD   ST0                      ; duplicate v
//   0x0041136a  CALL  0x004a2c48               ; __ftol -> EAX = trunc(v)
//   0x0041137b  FILD  dword ptr [ESP+4]        ; ST0 = (float)trunc(v), ST1 = v
//   0x00411385  FSUBR ST0,ST1                  ; ST0 = v - trunc(v)
//   0x00411387  FSTP  float ptr [EDX]          ; *param_2 = fraction
//   0x00411389  MOV   dword ptr [EAX],0        ; *param_4 = 0
//   0x00411391  CMP   EDX,0x3b                 ; signed compare vs 59
//   0x0041139c  ADD   EDX,-0x3c                ; -= 60
//
// x87 balance: FILD(+1) FLD ST0(+1) __ftol(-1) FILD(+1) FSUBR(0) FSTP[EDX](-1)
// FSTP ST0(-1) = 0. Nothing is left on the stack at RET; the trailing
// `FSTP ST0` at 0x00411394 discards the surviving copy of v and is not a store.
#include "../Core/HookSystem.h"

// 0x005cc948 - the tick->second scale. This is a MULTIPLY BY THE RECIPROCAL,
// not a divide by 3000: `FMUL float ptr [0x005cc948]`. Writing `/ 3000.0` would
// be a different operation and would not reproduce the original's bits. The
// literal below round-trips to exactly 0x39AEC33E, the dword at that address.
static const float kTicksToSeconds = 3.3333333e-4f;   // 0x39AEC33E @ 0x005cc948

// 0x00411350
extern "C" __declspec(dllexport) void __cdecl
ReplayTimeFormat(unsigned int param_1, float* param_2, int* param_3, int* param_4)
{
    // 0x00411350..0x00411362. The cast from `unsigned` is what emits the
    // FILD + TEST/JGE + FADD 2^32 fixup; casting from `int` would emit the bare
    // FILD and silently diverge on the whole upper half of the domain.
    const double v = (double)param_1 * kTicksToSeconds;

    // 0x0041136a - truncation toward zero (__ftol), NOT round-to-nearest.
    const int whole = (int)v;

    *param_3 = whole;                       // 0x0041137f
    *param_2 = (float)(v - (double)whole);  // 0x00411385/0x00411387
    *param_4 = 0;                           // 0x00411389

    // 0x0041138f..0x004113a4. Signed compare against 59, and the original
    // RE-READS *param_3 from memory on every iteration (0x0041139a) rather than
    // caching it - transcribed as written.
    while (*param_3 > 59) {
        (*param_4)++;
        *param_3 = *param_3 - 60;
    }
}

RH_ScopedInstall(ReplayTimeFormat, 0x00411350);
