// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00463640 (h7, by hand): 94.3% (regs 94.9%, ops 99.5%), raw 44.5%. DAT_0068f640 is an 8-byte record
   array (object pointer + dword; +0x20 is record a+4); DAT_00604570 is a 0x128-byte record array (int at +0,
   float at +0x10); float constants are source literals (1500, 2000, 250, 200, 100, 1000, 10, 1/9000, double
   2.286f/0.7f, 0.01, 0.35, 1e-5, 3.5, 10000).
   What moved it: first switch with the 0/4/5/11 cases as separate labels-with-bodies (gives the plain dword
   table, not a byte-indexed one); case 6 written with its two stores swapped so the compiler does not merge it
   with the 2/8/9 block (the original keeps both blocks, 0x004636ad and 0x004636c3, with the stores in cat-then-
   min order: every same-order spelling tried, including literal forms, goto, and all case groupings/orders,
   merges them); rnd() call inline in case 0 with the DAT_008aa300 store after; the gear-step tests as
   `state != 2` first; the third test reads DAT_006904b4[param_1] directly (no CSE of *rpm - old) written
   `x - old < -T`; `old` loaded after `base` so it takes the dead param_1 slot.
   Residual (b): the case-6 store pair order (2 insns) and, at 0x00463a1f, the state/gear register choice
   (ours: state in ecx, gear*4 via lea eax; original: gear in ecx via shl, state in eax, fld/fsub scheduled first);
   the rest is jump-offset fallout. */
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
typedef struct Obj00463640 {
  char pad0[0x30];
  int h30;
  unsigned char f34;
  char pad35[7];
  float f3c;
} Obj00463640;
typedef struct { Obj00463640 *p; int q; } Rec0068f640;
typedef struct { int base; char pad4[0xc]; float f10; char pad14[0x114]; } Rec00604570;
typedef struct { int idx; char pad4[0xc]; } Rec007f1a14;
typedef struct { char flag; char pad1[0x4b]; } Rec007f103c;
MGLOBAL(int, DAT_006130bc, 0x006130bc);
MGLOBAL(int, DAT_0069055c, 0x0069055c);
MGLOBAL(float, DAT_006130c4, 0x006130c4);
MGLOBAL(float, DAT_006130c0, 0x006130c0);
MGLOBALA(float, DAT_006904b4, 1, 0x006904b4);
MGLOBALA(Rec00604570, DAT_00604570, 1, 0x00604570);
MGLOBALA(Rec0068f640, DAT_0068f640, 1, 0x0068f640);
MGLOBALA(int, DAT_0069054c, 1, 0x0069054c);
MGLOBALA(int, DAT_0069052c, 1, 0x0069052c);
MGLOBALA(int, DAT_008aa300, 1, 0x008aa300);
MGLOBALA(Rec007f1a14, DAT_007f1a14, 1, 0x007f1a14);
MGLOBALA(Rec007f103c, DAT_007f103c, 1, 0x007f103c);
MGLOBALA(float, DAT_00613020, 4, 0x00613020);
MGLOBALA(float, DAT_00613030, 4, 0x00613030);
MGLOBALA(float, DAT_00613044, 4, 0x00613044);
MGLOBALA(float, DAT_00613058, 4, 0x00613058);
MGLOBALA(float, DAT_0061306c, 4, 0x0061306c);
MGLOBALA(float, DAT_00613080, 4, 0x00613080);
MGLOBALA(float, DAT_00613094, 4, 0x00613094);
MGLOBALA(float, DAT_006130a8, 4, 0x006130a8);
MGLOBALA(float, DAT_006130c8, 4, 0x006130c8);
double sin(double);
#pragma intrinsic(sin)
int FUN_0040e350(void);
int FUN_0041efc0(int p0);
float FUN_00420d80(int p0);
void FUN_0045daf0(int p0, int p1);
void FUN_0045e0f0(int p0, float p1);
int FUN_0046c7b0(int p0);
int FUN_0046cc10(int *p0, int p1);
int FUN_0046d6d0(float *p0, int p1);
float FUN_00472650(float lo, float hi);
void FUN_005a66d0(Obj00463640 *p0, int p1);
void FUN_005a6dc0(int p0, int p1, int p2, float p3);

// MATCH: 0x00463640 FUN_00463640
void FUN_00463640(int param_1)
{
  int kind;
  int notalive;
  int a;
  int st;
  int g;
  float *rpm;
  float speed;
  float old;
  float base;
  float v;
  float r;
  float x;

  kind = FUN_0041efc0(param_1) / 6;
  speed = FUN_00420d80(param_1);
  notalive = FUN_0046c7b0(param_1) ^ 1;
  DAT_006130bc = 1;
  switch (kind) {
  case 0:
    DAT_0069055c = 3;
    DAT_006130c4 = 1500.0f;
    break;
  case 4:
    DAT_0069055c = 3;
    DAT_006130c4 = 1500.0f;
    break;
  case 5:
    DAT_0069055c = 3;
    DAT_006130c4 = 1500.0f;
    break;
  case 11:
    DAT_0069055c = 3;
    DAT_006130c4 = 1500.0f;
    break;
  case 1:
  case 3:
    DAT_0069055c = 1;
    DAT_006130c4 = 2000.0f;
    break;
  case 6:
    DAT_006130c4 = 1500.0f;
    DAT_0069055c = 2;
    break;
  case 2:
  case 8:
  case 9:
    DAT_0069055c = 2;
    DAT_006130c4 = 1500.0f;
    break;
  default:
    DAT_0069055c = 0;
    DAT_006130c4 = 2000.0f;
    break;
  }
  DAT_006130c4 = FUN_00472650(0.0f, 250.0f) + DAT_006130c4;
  base = (float)param_1 * 200.0f + DAT_00604570[DAT_0069055c].f10;
  a = DAT_00604570[DAT_0069055c].base + param_1;
  old = DAT_006904b4[param_1];
  rpm = &DAT_006904b4[param_1];
  if ((DAT_0068f640[a].p->f34 & 2) == 0) {
    FUN_005a66d0(DAT_0068f640[a].p, 1);
    if (DAT_006130bc != 0) {
      FUN_005a66d0(DAT_0068f640[a + 4].p, 1);
    }
  }
  FUN_0045daf0(a, param_1);
  if (DAT_006130bc != 0) {
    FUN_0045daf0(a + 4, param_1);
  }
  FUN_0046d6d0(&v, param_1);
  FUN_0046cc10(&st, param_1);
  switch (DAT_0069054c[param_1]) {
  case 0:
    if (*rpm >= DAT_0061306c[DAT_0069052c[param_1]] &&
        DAT_007f103c[DAT_007f1a14[param_1].idx].flag != 0) {
      DAT_0069054c[param_1] = 1;
    }
    if (*rpm < DAT_006130a8[DAT_0069052c[param_1]]) {
      DAT_0069054c[param_1] = 2;
    }
    if (v < 100.0f) {
      DAT_0069054c[param_1] = 3;
      DAT_0069052c[param_1] = 0;
    }
    if (st != 4) {
      DAT_0069054c[param_1] = 4;
      DAT_0069052c[param_1] = 0;
    }
    *rpm = v * DAT_006130c8[DAT_0069052c[param_1]] + FUN_00472650(0.0f, 100.0f) + 1000.0f;
    DAT_008aa300[param_1] = 0;
    break;
  case 1:
    if (*rpm <= DAT_00613080[DAT_0069052c[param_1]]) {
      g = DAT_0069052c[param_1] + 1;
      DAT_0069052c[param_1] = g;
      DAT_0069054c[param_1] = 0;
      if (g >= 4) {
        DAT_0069054c[param_1] = 0;
        DAT_0069052c[param_1] = 0;
      }
    }
    DAT_008aa300[param_1] = 1;
    *rpm = DAT_00613080[DAT_0069052c[param_1]];
    break;
  case 2:
    if (*rpm >= DAT_00613094[DAT_0069052c[param_1]]) {
      DAT_0069052c[param_1] = DAT_0069052c[param_1] - 1;
      DAT_0069054c[param_1] = 0;
    }
    DAT_008aa300[param_1] = 2;
    *rpm = DAT_00613094[DAT_0069052c[param_1]];
    break;
  case 3:
    if (v > 100.0f) {
      DAT_0069054c[param_1] = 0;
    }
    g = FUN_0040e350();
    if (DAT_007f103c[DAT_007f1a14[param_1].idx].flag != 0 && notalive == 0 && g != 7) {
      *rpm = DAT_00613030[0] + *rpm;
    }
    else {
      *rpm = *rpm - DAT_00613044[0];
    }
    break;
  case 4:
    if (st == 4) {
      DAT_0069054c[param_1] = 0;
    }
    if (notalive != 0) {
      DAT_0069054c[param_1] = 300;
    }
    DAT_006130c0 = 10000.0f;
    *rpm = DAT_00613030[0] + DAT_00613030[0] + *rpm;
    break;
  default:
    *rpm = 0.0f;
    DAT_008aa300[param_1] = 0;
    DAT_0069052c[param_1] = 0;
    if (notalive == 0) {
      DAT_0069054c[param_1] = 0;
    }
    break;
  }
  if (*rpm > DAT_006130c0) {
    *rpm = DAT_006130c0;
  }
  if (*rpm < DAT_006130c4) {
    *rpm = DAT_006130c4;
  }
  g = DAT_0069052c[param_1];
  r = *rpm;
  if (DAT_0069054c[param_1] != 2) {
    if (*rpm - old > DAT_00613030[g]) {
      r = (float)param_1 * 10.0f + DAT_00613030[g] + old;
    }
  }
  else {
    if (*rpm - old > DAT_00613058[g]) {
      r = old + DAT_00613058[g];
    }
  }
  if (DAT_006904b4[param_1] - old < -DAT_00613044[g]) {
    r = old - DAT_00613044[g];
  }
  *rpm = r;
  x = (float)(sin((r - 1000.0f) * (1.0f / 9000.0f)) * 2.286f - 0.7f);
  if (speed > 0.0f) {
    x = speed * 0.01f + x;
  }
  if (DAT_008aa300[param_1] == 0) {
    if (x < 0.0f) {
      x = 0.0f;
    }
    else if (x > 1.0f) {
      x = 1.0f;
    }
  }
  else {
    x = 1.0f;
  }
  if (DAT_006130bc != 0) {
    if (DAT_008aa300[param_1] != 0) {
      FUN_0045e0f0(a + 4, x * 0.35f + DAT_00613020[DAT_0069055c]);
    }
    else {
      FUN_0045e0f0(a + 4, r * 1e-05f + DAT_00613020[DAT_0069055c]);
    }
  }
  DAT_0068f640[a].p->f3c = *rpm * 3.5f + base;
  if (DAT_0068f640[a].p->h30 != 0) {
    FUN_005a6dc0(DAT_0068f640[a].p->h30, 4, 1, *rpm * 3.5f + base);
  }
  FUN_0045e0f0(a, *rpm * 1e-05f + DAT_00613020[DAT_0069055c]);
  if (DAT_006130bc != 0) {
    DAT_0068f640[a + 4].p->f3c = *rpm * 3.5f + base;
    if (DAT_0068f640[a + 4].p->h30 != 0) {
      FUN_005a6dc0(DAT_0068f640[a + 4].p->h30, 4, 1, *rpm * 3.5f + base);
    }
  }
}
