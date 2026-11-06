// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046bd20 (h2): 100% (regs 100%). Raw 86.7%. Early return for the bound, typed 0xd04 record
   array at 0x008815a0 with a vec3 at +0xb00. */
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
    undefined1 pad0[0xb00];
    float v[3];
    undefined1 pad1[0xd04 - 0xb00 - 12];
} Rec_008815a0;
MGLOBALA(Rec_008815a0, DAT_008815a0, 16, 0x008815a0);

// MATCH: 0x0046bd20 FUN_0046bd20
undefined4 FUN_0046bd20(float *param_1, uint param_2)
{
    if (param_2 >= 0x10) return 0;
    param_1[0] = DAT_008815a0[param_2].v[0];
    param_1[1] = DAT_008815a0[param_2].v[1];
    param_1[2] = DAT_008815a0[param_2].v[2];
    return 1;
}
