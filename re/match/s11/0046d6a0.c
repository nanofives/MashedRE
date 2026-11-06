// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046d6a0 (helper h4): 100% (regs 100%). Getter: dword at vehicle record DAT_008815a0 +0xb0c, early return on index >= 0x10. */
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
typedef struct { float x, y, z; } V3_0046d6a0;
typedef struct { float m[16]; } Mat_0046d6a0;
typedef struct {
  undefined1 pad0[0xb0c];
  int f; /* +0xb0c */
  undefined1 padb10[0x1f4];
} Rec_0046d6a0;
MGLOBALA(Rec_0046d6a0, DAT_008815a0, 16, 0x008815a0);
// MATCH: 0x0046d6a0 FUN_0046d6a0
int FUN_0046d6a0(int *param_1,unsigned int param_2)
{
  if (param_2 >= 0x10) return 0;
  *param_1 = DAT_008815a0[param_2].f;
  return 1;
}
