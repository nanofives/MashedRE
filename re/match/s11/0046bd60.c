// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046bd60 (h2): 100% (regs 100%). Raw 75%. Two early returns (the `a && b` form puts return 0
   at the end), 0xc4-byte wheel array w[4] at +0x1a4 in the 0xd04 record at 0x008815a0. */
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
typedef struct {
    float f0;
    float sum[16];
    undefined1 pad[0xc4 - 4 - 64];
} Wheel_0046bd60;
typedef struct {
    undefined1 pad0[0x1a4];
    Wheel_0046bd60 w[4];
    undefined1 pad1[0xd04 - 0x1a4 - 4 * 0xc4];
} Rec_008815a0;
MGLOBALA(Rec_008815a0, DAT_008815a0, 16, 0x008815a0);

// MATCH: 0x0046bd60 FUN_0046bd60
undefined4 FUN_0046bd60(float *param_1, uint param_2, uint param_3)
{
    if (param_2 >= 0x10) return 0;
    if (param_3 >= 4) return 0;
    *param_1 = DAT_008815a0[param_2].w[param_3].f0;
    return 1;
}
