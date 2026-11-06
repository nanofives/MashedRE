// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x004972b0 (85 B Ghidra size, 88 B real), h4. Matched: match.py --bytes reports the whole obj symbol
   BYTE-IDENTICAL (88 B); the per-instruction score is 91.7% only because Ghidra's size stops before the
   pop esi / pop ebp / ret epilogue. First hand pass, typed arrays. */
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
MGLOBALA(unsigned char, DAT_007730d4, 0x48, 0x007730d4);
MGLOBALA(float, DAT_0077311c, 8, 0x0077311c);
MGLOBALA(unsigned char, DAT_0077313c, 0x20, 0x0077313c);
int FUN_00495790(void);
int FUN_004957a0(int idx, void *a, float *axes, void *b, unsigned char *buttons);
void FUN_004b6520(unsigned char *buf, int n);
int FUN_00496100(unsigned char *buf);
// MATCH: 0x004972b0 FUN_004972b0
void FUN_004972b0(void)
{
    int n = FUN_00495790();
    int i;
    for (i = 0; i < n; i++) {
        FUN_004957a0(i, 0, &DAT_0077311c[i * 2], 0, &DAT_007730d4[i * 4]);
    }
    FUN_004b6520(DAT_0077313c, 0x20);
    FUN_00496100(DAT_0077313c);
}
