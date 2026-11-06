// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x00487e40 (h4): 100%. Index into an array of 0x38-byte records (imul 0x38); the raw converter's int-array form with *0xe scored 83.3%. The record base is written as 0x0086ae34 (the lowest address the function touches); fields +0x00 and +0x18 cleared. */
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
typedef struct S38 {
    int f00, f04, f08, f0c, f10, f14, f18, f1c, f20, f24, f28, f2c, f30, f34;
} S38;
MGLOBALA(S38, DAT_0086ae34, 128, 0x0086ae34);
MGLOBAL(int, DAT_007030b4, 0x007030b4);

// MATCH: 0x00487e40 FUN_00487e40
int FUN_00487e40(int param_1)
{
    if (param_1 >= 0x80) {
        return 0;
    }
    DAT_0086ae34[param_1].f00 = 0;
    DAT_0086ae34[param_1].f18 = 0;
    DAT_007030b4 = 0;
    return 1;
}
