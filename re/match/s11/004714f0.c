// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x004714f0 (h6): 100% (regs 100%), first try; inlined strcpy intrinsic; name table at 0x00690a30, 20 entries of 0x84 (char name[0x80], int value),
   count at 0x00691484. */
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
char *strcpy(char *, const char *);
#pragma intrinsic(strcpy)
typedef struct Name84 {
    char name[0x80]; /* +0x00 */
    int value;       /* +0x80 */
} Name84;
MGLOBALA(Name84, DAT_00690a30, 20, 0x00690a30);
MGLOBAL(int, DAT_00691484, 0x00691484);
// MATCH: 0x004714f0 FUN_004714f0
void FUN_004714f0(char *param_1, int param_2)
{
    strcpy(DAT_00690a30[DAT_00691484].name, param_1);
    DAT_00690a30[DAT_00691484].value = param_2;
    DAT_00691484++;
}
