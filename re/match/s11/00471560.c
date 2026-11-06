// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00471560 (h6): 100% (regs 100%), first try; name table at 0x00690a30, 20 entries of 0x84 (char name[0x80], int value).
   strstr is the CRT function at 0x004a3860 (cdecl). */
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
char *strstr(const char *, const char *);
typedef struct Name84 {
    char name[0x80]; /* +0x00 */
    int value;       /* +0x80 */
} Name84;
MGLOBALA(Name84, DAT_00690a30, 20, 0x00690a30);
// MATCH: 0x00471560 FUN_00471560
int FUN_00471560(char *param_1)
{
    int i;
    for (i = 0; i < 20; i++) {
        if (strstr(DAT_00690a30[i].name, param_1) != 0)
            return i + 2000;
    }
    return -1;
}
