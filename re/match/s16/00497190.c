// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00497190 (25 B), h4. */
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
MGLOBALA(char, DAT_007730e4, 0x14, 0x007730e4);
MGLOBALA(char, s_contcfg_d_bin_005d000c, 14, 0x005d000c);
int FUN_004a2b60(char *buf, const char *fmt, ...);
// MATCH: 0x00497190 FUN_00497190
static char *FUN_00497190(int idx)
{
    FUN_004a2b60(DAT_007730e4, s_contcfg_d_bin_005d000c, idx);
    return DAT_007730e4;
}

/* stand-in caller (not the real 0x004971b0 / 0x00497230): keeps the static and its EAX convention */
char *h4_standin_00497190(int idx)
{
    return FUN_00497190(idx);
}
