// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x004709a0 (h5): best 55.7% (regs 62.9%). Raw 29.1%.
   Signature is (float dt, int ctx): ctx is a second stack argument Ghidra missed (only read to pass in EAX to
   0x0046f6c0). Five callees take register arguments (pilot finding 1, static callees of this TU):
   0x0046e9e0 car ESI / current matrix EDI / previous matrix EBX + (dt, surf) on the stack; 0x0046f6c0 ctx EAX /
   car EDI + flag; 0x00469aa0 car ESI; 0x0046ef70 car EDI + matrix; 0x00469df0 car ECX / other car EAX + (j, n).
   This file holds STAND-INS for all five (no MATCH lines, under #ifndef MATCH_PORT; prototypes under MATCH_PORT),
   shaped only so cl 9176 gives each that convention: the register order follows use weight inside the callee
   (heaviest -> ESI, next -> EDI, then EBX); a parameter whose register is later reused for another variable
   arrives in EAX/ECX and is copied. Real bodies: 0046e9e0.c, 0046f6c0.c, 0046ef70.c (this slice); 0x00469aa0 and
   0x00469df0 are not in this slice. Moved the score: stand-in conventions (36.9 -> 55.7), `rad * rad > sum`
   (R squared first, then dz, dy, dx as in the original).
   Residual (b): register allocation. The original keeps the do-loop counter n in EBP and the "no collision" flag,
   surf and i in memory, so the two calls at the top run before the callee-saved pushes and `push 1` is a literal;
   ours gives EBP to the flag (and caches i in EBP in the outer region), so n is spilled. Tried: Ghidra's literal
   while(true)/break form (54.6), flag set before/after the first call, `n++`/flag order, `== 0` test, all
   declaration orders (no effect). The permuter reached 69.0% only with nonsense pointer compares that change
   register pressure (not kept). */
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
typedef struct Surf_004709a0 { int v[19]; } Surf_004709a0;
MGLOBALA(char, DAT_008815a0, 0xd040, 0x008815a0);
MGLOBAL(int, DAT_007f0fdc, 0x007f0fdc);
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBAL(int, DAT_007f0fd4, 0x007f0fd4);
MGLOBALA(int, DAT_007f1a14, 64, 0x007f1a14);
MGLOBALA(Surf_004709a0, DAT_007f1038, 32, 0x007f1038);
MGLOBAL(Surf_004709a0, DAT_007f19b8, 0x007f19b8);
MGLOBAL(float, DAT_005cc950, 0x005cc950);
int FUN_0040e350(void);
int FUN_0040e340(void);
void FUN_00467300(int car);

#ifndef MATCH_PORT
/* Stand-ins (no MATCH line) for this TU's static callees, shaped only so cl 9176 gives each the register
   convention the original call sites use. The real bodies: 0046e9e0.c, 0046f6c0.c, 0046ef70.c; 0x00469aa0 and
   0x00469df0 are not in this slice. */
extern int g_stub_004709a0[8];
extern void stub_call_004709a0(int a, int b);
extern int stub_call2_004709a0(int a, int b);

/* 0x0046e9e0: car in ESI, current matrix in EDI, previous matrix in EBX, (dt, surf) on the stack */
static void FUN_0046e9e0(float dt, Surf_004709a0 *surf, char *car, float *cur, float *prev)
{
  stub_call_004709a0(*(int *)(car + 0x9b0), *(int *)(car + 0x9d0));
  stub_call_004709a0(*(int *)(car + 0x9b4), *(int *)(car + 0x9d4));
  stub_call_004709a0(*(int *)(car + 0x9b8), (int)cur[2]);
  prev[0] = cur[4] + dt;
  stub_call_004709a0(*(int *)(car + 0x9bc), (int)cur[3]);
  prev[1] = cur[5] + dt;
  stub_call_004709a0(*(int *)(car + 0x9c0), (int)cur[7]);
  prev[2] = cur[8] + dt;
  stub_call_004709a0(*(int *)(car + 0x9c4), *(int *)(car + 0x9c8));
  stub_call_004709a0(surf->v[0], *(int *)(car + 0x9cc));
}

/* 0x0046f6c0: ctx in EAX, car in EDI, flag on the stack */
static void FUN_0046f6c0(int flag, int *ctx, char *car)
{
  int k;
  float *q = (float *)(car + *(int *)(car + 0x9ac) * 0x40 + 0x928);
  g_stub_004709a0[0] = 0;
  stub_call_004709a0((int)q, 1);
  stub_call_004709a0((int)q, 2);
  g_stub_004709a0[1] = (int)ctx;
  stub_call_004709a0(ctx[2], 0);
  for (k = 0; k < 4; k++) {
    stub_call_004709a0((int)q, k);
    stub_call_004709a0(k, *(int *)(car + 0x198));
    stub_call_004709a0(k, *(int *)(car + 0x19c));
    stub_call_004709a0(k, *(int *)(car + 0x1a0));
    stub_call_004709a0(k, *(int *)(car + 0x1a4));
    stub_call_004709a0(0, *(int *)(car + 0x1a8));
  }
  if (flag)
    stub_call_004709a0(*(int *)(car + 0x9e0), 2);
}

/* 0x00469aa0: car in ESI */
static int FUN_00469aa0(char *car)
{
  stub_call_004709a0(*(int *)(car + 0x9ac), 0);
  stub_call_004709a0(*(int *)(car + 0x9a8), 0);
  return *(int *)(car + 0x198);
}

/* 0x0046ef70: car in EDI, matrix on the stack */
static void FUN_0046ef70(float *m, char *car)
{
  int k;
  for (k = 0; k < 4; k++) {
    stub_call_004709a0(k, *(int *)(car + 0x198));
    stub_call_004709a0(k, *(int *)(car + 0x25c));
    stub_call_004709a0(k, *(int *)(car + 0x320));
    stub_call_004709a0(k, k);
  }
  stub_call_004709a0((int)m[0], *(int *)(car + 0x3e4));
}

/* 0x00469df0: car in ECX, other car in EAX, (j, n) on the stack */
static int FUN_00469df0(int j, int n, char *car, char *other)
{
  int k;
  int r = 0;
  int *p;
  int *q;
  stub_call_004709a0(*(int *)(car + 0x9a8), *(int *)(other + 0x9a8));
  p = (int *)(car + *(int *)(car + 0x9ac));
  q = (int *)(other + *(int *)(other + 0x9ac));
  for (k = 0; k < 2; k++) {
    r += stub_call2_004709a0(p[k], q[k]);
    r += stub_call2_004709a0(p[k + 2], q[k + 2]);
    r += stub_call2_004709a0(p[k + 4], q[k + 4]);
  }
  return r + j + n;
}
#else
void FUN_0046e9e0(float dt, Surf_004709a0 *surf, char *car, float *cur, float *prev);
void FUN_0046f6c0(int flag, int *ctx, char *car);
int FUN_00469aa0(char *car);
void FUN_0046ef70(float *m, char *car);
int FUN_00469df0(int j, int n, char *car, char *other);
#endif

// MATCH: 0x004709a0 FUN_004709a0
void FUN_004709a0(float dt, int ctx)
{
  int mode;
  int ncars;
  int i;
  int j;
  int n;
  int ok;
  int r;
  char *car;
  float *cur;
  float *prev;
  float *a;
  float *b;
  float rad;
  float dx, dy, dz;
  Surf_004709a0 *surf;

  mode = FUN_0040e350();
  ncars = FUN_0040e340();
  if (DAT_007f0fdc != 0) {
    DAT_007f0fdc = (int)((float)DAT_007f0fdc - dt);
    if (DAT_007f0fdc < 0)
      DAT_007f0fdc = 0;
  }
  for (i = 0; i < 16; i++) {
    car = DAT_008815a0 + i * 0xd04;
    surf = &DAT_007f1038[DAT_007f1a14[i * 4]];
    if (*(int *)(car + 0x9f0) != 0)
      surf = &DAT_007f19b8;
    if (*(int *)(car + 0x10) != 0)
      surf = &DAT_007f19b8;
    if (FUN_0040e350() != 6 && FUN_0040e350() != 11 && FUN_0040e350() != 10)
      surf = &DAT_007f19b8;
    if (*(int *)(car + 0x14) != 0)
      continue;
    if (*(int *)(car + 4) == 0 && *(int *)(car + 0x10) != 1)
      continue;
    cur = (float *)(car + *(int *)(car + 0x9a8) * 0x40 + 0x928);
    prev = (float *)(car + *(int *)(car + 0x9ac) * 0x40 + 0x928);
    if (*(int *)(car + 0x9f0) == 0xff)
      continue;
    n = 0;
    do {
      if (n >= 2)
        goto next;
      ok = 1;
      FUN_0046e9e0(dt, surf, car, cur, prev);
      FUN_0046f6c0(1, (int *)ctx, car);
      r = FUN_00469aa0(car);
      if (*(int *)(car + 0x9ec) != 0) {
        FUN_0046ef70(prev, car);
        if (r != 0) {
          n++;
          continue;
        }
      }
      for (j = i + 1; j < 16; j++) {
        if (j < ncars && (*(int *)(DAT_008815a0 + j * 0xd04 + 4) != 0 || *(int *)(DAT_008815a0 + j * 0xd04 + 0x10) == 1)) {
          a = (float *)(car + *(int *)(car + 0x9a8) * 0x40 + 0x958);
          b = (float *)(DAT_008815a0 + j * 0xd04 + *(int *)(DAT_008815a0 + j * 0xd04 + 0x9a8) * 0x40 + 0x958);
          rad = (*(float *)(DAT_008815a0 + j * 0xd04 + 0x4a4) + *(float *)(car + 0x4a4)) * DAT_005cc950;
          dx = a[0] - b[0];
          dy = a[1] - b[1];
          dz = a[2] - b[2];
          if (rad * rad > dx * dx + dy * dy + dz * dz
              && (mode == 6 || mode == 7 || mode == 10 || mode == 11)
              && FUN_00469df0(j, n, car, DAT_008815a0 + j * 0xd04)) {
            n++;
            ok = 0;
            if (DAT_007f0fd0 == 1 && DAT_007f0fdc == 0) {
              if (i == DAT_007f0fd4)
                FUN_00467300(j);
              else if (j == DAT_007f0fd4)
                FUN_00467300(i);
            }
          }
        }
      }
    } while (!ok);
    *(int *)(car + 0x9ac) = *(int *)(car + 0x9a8);
    *(int *)(car + 0x9a8) = (*(int *)(car + 0x9a8) - 1) & 1;
next:;
  }
}
