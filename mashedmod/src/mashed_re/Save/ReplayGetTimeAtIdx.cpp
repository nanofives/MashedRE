// Mashed RE - fetch one replay timestamp by index and decompose it.
// Original: 0x00411530  Replay::GetTimeAtIdx  vehicle  C2 -> C3
// Raw-listing plate: re/analysis/replay_record/0x00411530_RAW.md (orch-iter20)
//
//     if (!param_1) { *param_4 = 0; *param_3 = 0; return; }   // minutes UNWRITTEN
//     Replay::TimeFormat(param_1[0x17c + param_2*4], param_3, param_4, param_5);
//
// THE NULL PATH WRITES SECONDS AND FRACTION, NOT MINUTES. The iter13 handler
// brief recorded "null-path: frac=0 mins=0 secs=unwritten (confirmed)", which is
// backwards on two of the three outputs. The listing writes *param_4 (seconds)
// at 0x00411564 and *param_3 (fraction) at 0x0041156a, and never touches
// param_5 (minutes). A port written from that brief would zero minutes and leave
// seconds dangling - the exact inversion of the original.
//
// PARAM_1 IS A POINTER although hooks.csv and the plate type it `int`; it is
// dereferenced at 0x0041154a. Known pointer-param-described-as-int defect class.
//
// Disasm at 0x00411530..0x00411570 (65 bytes) - full listing in the plate:
//   0x00411530  MOV  EAX,[ESP+4]                   ; param_1 = replay object
//   0x00411534  TEST EAX,EAX
//   0x00411536  JZ   0x0041155c                    ; -> null path
//   0x00411538  MOV  ECX,[ESP+0x14]                ; param_5 (minutes out)
//   0x0041153c  MOV  EDX,[ESP+0x10]                ; param_4 (seconds out)
//   0x00411540  PUSH ECX                           ; TimeFormat arg4
//   0x00411541  MOV  ECX,[ESP+0x10]                ; ESP-4  => orig [ESP+0xc] = param_3
//   0x00411545  PUSH EDX                           ; TimeFormat arg3
//   0x00411546  MOV  EDX,[ESP+0x10]                ; ESP-8  => orig [ESP+8]   = param_2
//   0x0041154a  MOV  EAX,[EAX + EDX*4 + 0x17c]     ; ticks = obj[0x17c + idx*4]
//   0x00411551  PUSH ECX                           ; TimeFormat arg2
//   0x00411552  PUSH EAX                           ; TimeFormat arg1
//   0x00411553  CALL 0x00411350
//   0x00411558  ADD  ESP,0x10                      ; __cdecl, 4 dwords
//   0x0041155c  MOV  ECX,[ESP+0x10]                ; null path: param_4
//   0x00411560  MOV  EDX,[ESP+0xc]                 ;            param_3
//   0x00411564  MOV  dword ptr [ECX],0             ; *seconds = 0
//   0x0041156a  MOV  dword ptr [EDX],0             ; *frac    = 0
//
// THE TWO `[ESP+0x10]` READS ARE NOT THE SAME SLOT. The intervening PUSHes have
// moved ESP by -4 and -8, so they resolve to the original [ESP+0xc] and
// [ESP+8]. Reading them literally would swap two arguments. This is the Ghidra
// pre-branch-args pitfall in its stack-displacement form.
#include "../Core/HookSystem.h"

// 0x00411350 - called by ADDRESS, not by linking to our own ReplayTimeFormat.
// The original CALLs 0x00411350 at 0x00411553, so whichever body is installed
// there (original or our hook) is what both sides of an A/B execute. Linking
// directly to our port instead would make the two sides asymmetric.
typedef void(__cdecl* TimeFormatFn)(unsigned int ticks, float* frac,
                                    int* secs, int* mins);
static const TimeFormatFn TimeFormat = (TimeFormatFn)0x00411350;

// Offset of the tick-timestamp array inside the replay object (0x0041154a).
// [UNCERTAIN] the array length and the bound on param_2 are NOT established -
// there is no bounds check here, so an out-of-range index reads out of bounds.
static const unsigned int kTimeArrayOff = 0x17c;

// 0x00411530
extern "C" __declspec(dllexport) void __cdecl
ReplayGetTimeAtIdx(void* param_1, int param_2, float* param_3,
                   int* param_4, int* param_5)
{
    if (param_1 == 0) {
        // 0x0041155c..0x00411570. Both stores are integer zeros in the
        // original; *param_3 is a float* but is written as a dword 0, which is
        // the bit pattern of 0.0f. Written through a u32 to stay verbatim
        // rather than relying on a float store producing the same bits.
        *param_4 = 0;                               // seconds  (0x00411564)
        *(unsigned int*)param_3 = 0;                // fraction (0x0041156a)
        return;                                     // param_5 (minutes) UNWRITTEN
    }

    // 0x0041154a
    const unsigned int ticks =
        *(const unsigned int*)((const unsigned char*)param_1
                               + kTimeArrayOff + (unsigned int)param_2 * 4u);

    // 0x00411553 - argument order is a clean 1:1 forward (see the plate's
    // push table); the pushes are in reverse order, which is just the ABI.
    TimeFormat(ticks, param_3, param_4, param_5);
}

RH_ScopedInstall(ReplayGetTimeAtIdx, 0x00411530);
