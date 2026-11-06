// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x00486460 (h4): 100%. Writes one 0x90 record of the 100-entry array at 0x006fa5d0 (same layout as 0x00486f90's second array: 0x14 header + 0x7c inner record) through a pointer to the inner record; the position is a 3-float struct copy. Converter 30.8%. */
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
typedef struct V3 { float x, y, z; } V3;
typedef struct S7c {
    V3 pos;
    int f0c, f10, f14;
    float f18;
    int f1c, f20, f24, f28;
    float f2c;
    int f30[16];
    int f70, f74, f78;
} S7c;
typedef struct S90 {
    V3 *f00;
    int f04, f08, f0c, f10;
    S7c in;
} S90;
MGLOBALA(S90, DAT_006fa5d0, 100, 0x006fa5d0);
MGLOBAL(int, DAT_0070303c, 0x0070303c);
float FUN_00472650(float lo, float hi);

// MATCH: 0x00486460 FUN_00486460
void FUN_00486460(V3 *param_1, int param_2)
{
    S7c *p;
    DAT_006fa5d0[DAT_0070303c].f00 = param_1;
    p = &DAT_006fa5d0[DAT_0070303c].in;
    p->f78 = 0;
    p->f70 = 1;
    p->f20 = 0;
    p->pos = *param_1;
    p->f14 = param_2;
    p->f0c = 0;
    p->f18 = FUN_00472650(0.6f, 1.3f);
    p->f1c = 0;
    p->f74 = 0;
    p->f2c = 50.0f;
    DAT_0070303c++;
    if (DAT_0070303c >= 100) {
        DAT_0070303c = 0;
    }
}
