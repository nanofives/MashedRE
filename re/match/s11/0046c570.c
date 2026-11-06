// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046c570 (75 B): 100.0% (regs 100, ops 100), by hand (h3). Raw 6.2%.
   Car *c = &DAT_008815a0[i] (stride 0xd04), c->v[0..2] *= 0.85f (the float at 0x005ce264). */
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
    char p0[0x9b0];
    float v9b0[3];                  /* 0x9b0 */
    char p1[0xd04 - 0x9bc];
} Car;                              /* 0xd04 */
MGLOBALA(Car, DAT_008815a0, 16, 0x008815a0);
// MATCH: 0x0046c570 FUN_0046c570
undefined4 FUN_0046c570(int param_1)
{
  Car *c = &DAT_008815a0[param_1];
  c->v9b0[0] = c->v9b0[0] * 0.85f;   /* 0.85f = the float at 0x005ce264 */
  c->v9b0[1] = c->v9b0[1] * 0.85f;
  c->v9b0[2] = c->v9b0[2] * 0.85f;
  return 1;
}
