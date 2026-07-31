// Mashed RE - replay buffer size from a duration and a divisor.
// Original: 0x00482900  FUN_00482900  vehicle  C2 -> C3
// Plate: re/analysis/bucket_vehicle_004820e0_00485420/00482900.md (corrected
//        2026-07-31; the pre-correction plate was materially wrong — see below)
//
//     frames = (int)(param_1 * 60.0f / param_2);   // via __ftol
//     size   = frames * 0x24 + 0x19c;
//     printf("Replay size is %d\n", size);
//     return size;
//
// THIS FUNCTION TAKES TWO ARGUMENTS. Ghidra types it `int FUN_00482900(void)`
// and the original plate recorded that as fact, with the __ftol call described
// as a "checkpoint/frame count getter". Both were wrong. The arguments are
// consumed ONLY by x87 instructions — `FLD [ESP+4]` and `FIDIV [ESP+0xc]` — and
// Ghidra cannot see x87 register arguments, so it reported the whole function
// as void-parameter. A port written from that plate would have been
// `int f(void)` calling a getter that does not exist.
//
// FUN_004a2c48 is `__ftol`, the MSVC CRT x87 round-to-int64 helper, already C3
// and byte-identical at Math/FPURound.cpp (hooks.csv:254). It takes ST0. It is
// NOT ported again here; the port leaves the conversion to the compiler's own
// float->int cast, which on this /arch-less x87 build emits the same __ftol
// call. [UNCERTAIN] whether MSVC emits a literal `call __ftol` here or an
// inline FISTP — the A/B compares the RETURNED SIZE, which is identical either
// way, so this does not affect acceptance. It would matter for a bit-identity
// claim about the instruction stream, which is not claimed.
//
// The meaning of the two arguments is NOT established. `param_1 * 60.0f` is
// consistent with a seconds-to-frames conversion at 60 fps and `param_2` with a
// stride/divisor, but nothing in this body names either, so no semantic is
// assigned. Resolve from the three call sites, not from here.
//
// Disasm at 0x00482900..0x0048292f (47 bytes):
//   0x00482900  D9 44 24 04          FLD   float ptr [ESP+4]      ; param_1 FLOAT
//   0x00482904  56                   PUSH  ESI
//   0x00482905  D8 0D 28 C7 5C 00    FMUL  float ptr [0x005cc728] ; * 60.0f
//   0x0048290B  DA 74 24 0C          FIDIV dword ptr [ESP+0xc]    ; / param_2 INT
//   0x0048290F  E8 34 03 02 00       CALL  0x004a2c48             ; __ftol
//   0x00482914  8D 34 C0             LEA   ESI,[EAX+EAX*8]        ; EAX*9
//   0x00482917  8D 34 B5 9C 01 00 00 LEA   ESI,[ESI*4+0x19c]      ; *4 +412
//   0x0048291E  56                   PUSH  ESI
//   0x0048291F  68 4C F2 5C 00       PUSH  0x5cf24c               ; format string
//   0x00482924  E8 87 5E 01 00       CALL  0x004987b0             ; logger
//   0x00482929  83 C4 08             ADD   ESP,8
//   0x0048292C  8B C6                MOV   EAX,ESI
//   0x0048292E  5E                   POP   ESI
//   0x0048292F  C3                   RET
//
// `FIDIV` (integer divide), not `FDIV`, is what makes param_2 an int. And the
// `*36` is the LEA pair `(EAX*9)*4`, a strength-reduced multiply, not an IMUL.
//
// [ESP+0xc] at 0x0048290b is the SECOND argument: the PUSH ESI at 0x00482904
// has already moved ESP down 4, so [ESP+0xc] is the original [ESP+8].
#include "../Core/HookSystem.h"

// 0x004987b0 — printf-style logger (variadic; declared with the one argument
// this call site actually passes beyond the format).
typedef int(__cdecl* LogPrintfFn)(const char* fmt, int value);
static const LogPrintfFn LogPrintf = (LogPrintfFn)0x004987b0;

// 0x00482900
extern "C" __declspec(dllexport) int __cdecl
ReplayGetSize(float param_1, int param_2)
{
    // 0x00482900..0x0048290f — x87 chain then __ftol.
    // Intermediates are `double`, not `float`, deliberately. The original does
    // the whole chain in 80-bit x87 registers: FLD widens param_1, FMUL widens
    // the 60.0f operand, and FIDIV widens the INTEGER divisor — none of it is
    // rounded back to 32-bit float in between. Writing `(float)param_2` would
    // round the divisor to a 24-bit mantissa and diverge for |param_2| > 2^24.
    // double (53-bit) represents every int32 exactly, so it reproduces FIDIV's
    // operand faithfully and, on this x87 build, is itself evaluated in the
    // 80-bit registers. [UNCERTAIN] whether every intermediate rounds
    // identically to the original's register chain; the A/B compares the
    // RETURNED INT, and a divergence would have to survive truncation to show
    // up — no bit-identity claim is made about the instruction stream.
    const int frames = (int)((double)param_1 * 60.0 / (double)param_2);
    // 0x00482914/0x00482917 — LEA pair: (frames*9)*4 + 412  ==  frames*36 + 412
    const int size = frames * 0x24 + 0x19c;
    LogPrintf((const char*)0x005cf24c, size);   // 0x0048291f/0x00482924
    return size;                                 // 0x0048292c
}

RH_ScopedInstall(ReplayGetSize, 0x00482900);
