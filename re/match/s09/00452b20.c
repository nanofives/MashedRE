// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00452b20: 100% by hand, first natural form (unsigned param_2 to float, two explicit
   hierarchy-reset blocks sharing one zero V3 local). */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef struct { float x, y, z; } V3_452b20;
typedef struct { V3_452b20 a; int pad0c; V3_452b20 b; int pad1c; } El_452b20;   /* 0x20 bytes */
typedef struct { int pad[2]; El_452b20 *arr; } S_452b20;                     /* +8 = array */
typedef struct { int pad[4]; S_452b20 *s; } R_452b20;                        /* +0x10 */
typedef struct { R_452b20 *r; int idx; } P_452b20;
typedef struct { int pad; P_452b20 *p; } Q_452b20;                           /* +4 */
typedef struct { int pad[4]; Q_452b20 *q; } B_452b20;                        /* +0x10 */
typedef struct { float m[16]; } M16_452b20;
MGLOBAL(int, DAT_0088ff44, 0x0088ff44);
MGLOBAL(int, DAT_005f978c, 0x005f978c);
MGLOBAL(int, DAT_005f9790, 0x005f9790);
MGLOBAL(int, DAT_00684c50, 0x00684c50);
MGLOBALA(undefined, DAT_00684d18, 1, 0x00684d18);
MGLOBAL(M16_452b20, DAT_00684cd8, 0x00684cd8);
MGLOBAL(M16_452b20, DAT_00684c98, 0x00684c98);
MGLOBALA(int, DAT_0088ff60, 8, 0x0088ff60);
void FUN_00452790(int p);
void FUN_00451640(float f);
void FUN_004522e0(int a, int b, float f);
int FUN_004671a0(int a);
void FUN_0047cbb0(int a);
void FUN_0047d150(int a);
void FUN_00451d80(int a, uint b);
void FUN_00451a50(int a, M16_452b20 *b, M16_452b20 *c);
void FUN_0047d180(int a, M16_452b20 *m);
int FUN_0047d130(int a);
B_452b20 *FUN_0057c210(int a);
int FUN_004751d0(int a, int b);
void FUN_00475060(int a, int b);
void FUN_00452680(int p, float f);
void FUN_00452440(int a);
// MATCH: 0x00452b20 FUN_00452b20
void FUN_00452b20(int param_1, int param_2)
{
  float f;
  int h;
  B_452b20 *b;
  P_452b20 *p;
  V3_452b20 zero;
  int *pp;
  FUN_00452790(param_1);
  f = (float)(uint)param_2 * 0.00033333333f;
  FUN_00451640(f);
  FUN_004522e0(DAT_0088ff44, *(int *)(param_1 + 0x10010), f);
  FUN_0047cbb0(FUN_004671a0(0));
  FUN_0047d150(DAT_005f978c);
  zero.x = 0.0f;
  zero.y = 0.0f;
  zero.z = 0.0f;
  FUN_00451d80((int)DAT_00684d18, param_2);
  FUN_00451a50((int)DAT_00684d18, &DAT_00684cd8, &DAT_00684cd8);
  FUN_0047d180(DAT_005f978c, &DAT_00684cd8);
  h = FUN_0047d130(DAT_005f978c);
  if (h != 0) {
    b = FUN_0057c210(h);
    if (b != 0) {
      p = b->q->p;
      if (p != 0) {
        p->r->s->arr[p->idx].a = zero;
        p->r->s->arr[p->idx].b = zero;
      }
    }
  }
  FUN_00451a50((int)DAT_00684d18, &DAT_00684c98, &DAT_00684c98);
  FUN_0047d180(DAT_005f9790, &DAT_00684c98);
  h = FUN_0047d130(DAT_005f9790);
  if (h != 0) {
    b = FUN_0057c210(h);
    if (b != 0) {
      p = b->q->p;
      if (p != 0) {
        p->r->s->arr[p->idx].a = zero;
        p->r->s->arr[p->idx].b = zero;
      }
    }
  }
  FUN_00475060(*(int *)(param_1 + 0x10110), FUN_004751d0(DAT_00684c50, param_2 * 2));
  FUN_00452680(param_1, f);
  pp = DAT_0088ff60;
  do {
    FUN_00452440(*pp);
    pp++;
  } while ((int)pp < 0x88ff80);
}
