// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046cb30 (126 B): 100.0% (regs 100, ops 100), by hand (h3). Raw did not compile.
   Car record struct array (stride 0xd04): +0x14 int flag, +0x9b0 V3 copied as a struct assignment;
   callee FUN_0047f1e0(float *, uint) cdecl; result scaled by 300.0f (the float at 0x005cd8fc). */
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
typedef struct { float x, y, z; } V3;
typedef struct {
    char p0[0x14];
    int f14;                    /* 0x14 */
    char p1[0x9b0 - 0x18];
    V3 v9b0;                    /* 0x9b0 */
    char p2[0xd04 - 0x9bc];
} Car;                              /* 0xd04 */
MGLOBALA(Car, DAT_008815a0, 16, 0x008815a0);
int FUN_0047f1e0(float *v, uint i);
// MATCH: 0x0046cb30 FUN_0046cb30
undefined4 FUN_0046cb30(float *param_1, uint param_2)
{
  if (param_2 >= 0x10) {
    return 0;
  }
  if (DAT_008815a0[param_2].f14 == 0) {
    *(V3 *)param_1 = DAT_008815a0[param_2].v9b0;
    return 1;
  }
  if (FUN_0047f1e0(param_1, param_2) != 0) {
    param_1[0] = param_1[0] * 300.0f;   /* 300.0f = the float at 0x005cd8fc */
    param_1[1] = param_1[1] * 300.0f;
    param_1[2] = param_1[2] * 300.0f;
    return 1;
  }
  return 0;
}
