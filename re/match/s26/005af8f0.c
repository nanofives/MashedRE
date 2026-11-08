// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 h1 0x005af8f0 (268 B). Best 90.8% (regs 90.8%), residual class (a). ops 100% (98/98 ins, same block order): only 8 branch targets differ: cl cross-jumps the fail blocks to the default-case copy at +0x1f while the original jumps to its own copy near the end; switch(state){2,3->fallthrough 4,default} + gotos, tried tail-forms/break forms. */
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
typedef long LONG;
typedef struct St { unsigned int a; unsigned int b; int state; int c; int pad4; int p14; int pad[8]; int e38; } St;
void FUN_005af260(St *p0);
int FUN_005afa00(St *p0, int p1);
int FUN_005afcf0(St *p0, int *p1);
void FUN_005b09c0(St *p0, int *p1);
int FUN_005b0bb0(int p0, unsigned int p1, int p2);
int FUN_005b1110(int p0, int p1, int p2);
__declspec(dllimport) LONG __stdcall InterlockedExchange(LONG * p0, LONG p1);
// MATCH: 0x005af8f0 FUN_005af8f0
void FUN_005af8f0(St *param_1,int *param_2,int param_3)
{
  int flag = 0;
  int r;
  switch (param_1->state) {
  case 2:
    if (*(int *)(param_3 + 8) == 0) goto done6;
    InterlockedExchange((LONG *)&param_1->a, 0);
    InterlockedExchange((LONG *)&param_1->b, 1);
    param_1->e38 = 1;
    r = FUN_005afa00(param_1, *(int *)(param_3 + 8));
    if (r == 0) goto fail;
    param_1->state = 3;
    return;
  case 3:
    if (param_1->a > 0) goto fail;
    if (param_1->b > 0) return;
    param_1->c = FUN_005b1110(param_1->p14, *(int *)(param_1->p14 + 0x58), 2);
    if (FUN_005afcf0(param_1, param_2) == 0) goto fail;
    param_1->state = 4;
    /* fall through */
  case 4:
    if (param_1->a > 0) {
      flag = 1;
      param_1->state = 6;
    }
    if (!(param_1->b > 0)) {
      param_2[1] = FUN_005b0bb0(param_3 + 0x18, 0xc0, param_1->c);
      if (param_2[1] == 0) goto fail;
      FUN_005b09c0(param_1, param_2);
      param_1->state = 5;
    }
    if (flag == 0) return;
    goto fail;
  default:
    FUN_005af260(param_1);
    param_1->state = 6;
    return;
  }
fail:
  FUN_005af260(param_1);
done6:
  param_1->state = 6;
}
