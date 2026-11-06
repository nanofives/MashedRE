// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045cb20 (h2): 100% (regs 100%). Raw 75.4%. The file also holds its callees 0x0045c4e0 (100%),
   0x0045c510 (100%, h1's source with the arg typed as the 40-byte record) and 0x0045c550 (semantic body, no
   MATCH line, see below), all non-static and defined before the caller. With every callee body in the TU the
   compiler knows the three calls do not write DAT_0088fbc8, so the natural source (DAT_0088fbc8 passed directly
   to each call) is CSE'd into EBX after the n < 0x20 test, as in the original. With 0x0045c550 only declared the
   best was 90.8% (a `list` local: 90.6%, regs swapped or load hoisted, permuter no gain). */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
typedef struct { int f0; int f4; int f8; int fc[7]; } Rec28;
typedef struct { int pad00; int f04; int pad08; int state; int pad10; int f14; int pad18[4]; int f28; } Obj;
MGLOBAL(int *, DAT_0088fbc4, 0x0088fbc4);
MGLOBAL(Rec28 *, DAT_0088fbc8, 0x0088fbc8);
MGLOBALA(Rec28, DAT_005fcb50, 1, 0x005fcb50);
MGLOBALA(Obj *, DAT_0088f680, 4, 0x0088f680);
void FUN_0045c640(int param_1);
int FUN_0045caf0(int param_1, int param_2);
// MATCH: 0x0045c4e0 FUN_0045c4e0
int FUN_0045c4e0(int param_1, Rec28 *param_2, int param_3)
{
  int i;
  for (i = 0; i < param_3; i++) {
    if (param_2[i].f0 == param_1)
      return i;
  }
  return -1;
}
// MATCH: 0x0045c510 FUN_0045c510
int FUN_0045c510(int id, Rec28 *arg)
{
    int i;
    Obj *o;
    for (i = 0; i < 4; i++) {
        o = DAT_0088f680[i];
        if (o != 0 && o->state == 1 && o->f28 == id && arg->f4 == i)
            return 1;
    }
    return 0;
}
/* 0x0045c550: semantic body only (no MATCH line; helper h1 owns its match). Its body must be in this TU and
   must not write globals for 0x0045cb20 to match: swap in h1's matched source when it lands. */
int FUN_0045c550(int param_1, Rec28 *param_2)
{
  int a[20];
  int b[20];
  int c[20];
  int n;
  int k;
  Obj *o;
  Obj **p;

  if (DAT_005fcb50[param_1].fc[6] == 0)
    return 0;
  n = 0;
  do {
    param_1 = DAT_005fcb50[param_1].fc[6];
    c[n] = param_1;
    if (param_1 == 0x25) {
      b[n] = b[n - 1];
      a[n] = a[n - 1];
    } else if (n > 0) {
      b[n] = a[n - 1];
      a[n] = b[n - 1];
    } else {
      b[0] = param_2->f8;
      a[0] = param_2->f4;
    }
    n++;
  } while (DAT_005fcb50[param_1].fc[6] != 0);
  for (p = DAT_0088f680; p < DAT_0088f680 + 4; p++) {
    o = *p;
    if (o != 0 && o->state == 1) {
      for (k = 0; k < n; k++) {
        if (o->f28 == c[k] && o->f04 == b[k] && o->f14 == a[k])
          return 1;
      }
    }
  }
  return 0;
}

// MATCH: 0x0045cb20 FUN_0045cb20
void FUN_0045cb20(int param_1, int param_2, int param_3)
{
  int n;

  if (FUN_0045caf0(param_2, param_3) == 0) {
    FUN_0045c640(param_2);
    n = *DAT_0088fbc4;
    if (n < 0x20 && FUN_0045c510(param_1, DAT_0088fbc8) != 1 && FUN_0045c550(param_1, DAT_0088fbc8) != 1 &&
        FUN_0045c4e0(param_1, DAT_0088fbc8, n) == -1) {
      DAT_0088fbc8[n] = DAT_005fcb50[param_1];
      DAT_0088fbc8[*DAT_0088fbc4].f8 = param_3;
      DAT_0088fbc8[*DAT_0088fbc4].f4 = param_2;
      (*DAT_0088fbc4)++;
    }
  }
}
