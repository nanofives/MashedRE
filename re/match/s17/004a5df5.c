// FLAGS: /O1
/* MATCH-S17 0x004a5df5: CRT testfdiv.c _ms_p5_test_fdiv: dRslt = 4195835.0 - (4195835.0 / 3145727.0) * 3145727.0,
   return dRslt > 1.0. Constants: 3145727.0 at 0x005d1520, 4195835.0 at 0x005d1518, 1.0 at 0x005cd0c8.
   Best 58.3% (regs 58.3%). Residual class (c): the CRT source computes dRslt in an inline __asm block (fld dividend /
   fdiv divisor / fmul divisor / fsubr dividend / fstp dRslt, hence the EBP frame and the memory-operand sequence),
   so the FDIV test is not folded or rescheduled. From C (volatile locals, both declaration orders) cl 13 keeps one
   value in st(0) and uses fsubp. */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
   C under the DDK (match.py), C++ under VS2022 when the port includes this file (MATCH_PORT). */
#ifndef MATCH_PREAMBLE
#define MATCH_PREAMBLE
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10;
#ifndef __cplusplus
typedef unsigned char bool;
#endif
#ifdef __cplusplus
/* port: a global is a reference to its original address (the exe maps the original data range) */
#define MGLOBAL(T, n, a) static T &n = *(T *)(a)
#define MGLOBALA(T, n, N, a) static T (&n)[N] = *(T (*)[N])(a)
#else
#define MGLOBAL(T, n, a) extern T n
#define MGLOBALA(T, n, N, a) extern T n[N]
#endif
#endif
/* ---- end preamble ---- */
// MATCH: 0x004a5df5 FUN_004a5df5
int FUN_004a5df5(void)
{
    volatile double dTestDividend;
    volatile double dTestDivisor;
    volatile double dRslt;

    dTestDivisor = 3145727.0;
    dTestDividend = 4195835.0;
    dRslt = dTestDividend - (dTestDividend / dTestDivisor) * dTestDivisor;
    return (dRslt > 1.0);
}
