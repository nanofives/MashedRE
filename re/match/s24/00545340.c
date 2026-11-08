// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00545340 (h4): 35.8% (regs 45.8%, ops 75.4%), 561 vs 546 instructions. Spline control-point
   setup from param_2 (RwV3d points): count n = numCtrlPoints - 2 (type 1) or - 3 (type 2), RWERROR 0x102 for
   other types. Type 1 filters with the weight table DAT_007dc67c[min(n, 10)] (start/middle/end rows), type 2
   builds two int recurrence tables (a[j] = 4*a[j-1] - a[j-2], seeds 1,3 / 1,5 odd, 1,4 / 1,2 even) and sums
   over neighbours modulo n with scale 1/a[half]; both write ctrlPoints[1..n] and the wrap points. Written from
   the disassembly (raw form did not compile). Moved the score: switch on the type both times, down-counter
   do-while inner loops with walking pointers (a counted for loop is unrolled by 9176: 1202 -> 544
   instructions, 14 -> 31.9), store order / off placement / loop test from the permuter (35.8; its other moves
   broke semantics and were dropped). Residual (b): frame 0x38 vs 0x28 and register allocation (k, i, half in
   memory in ours); the odd/even branch is `je` to even with half computed per branch in the original (that
   form scored 29.1 here, register allocation moved). Out of budget. */
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
typedef struct RwV3d { float x; float y; float z; } RwV3d;
typedef struct RwError { int pluginID; int errorCode; } RwError;
typedef struct Spline545340 {
  int f0;
  int f4;
  int numCtrlPoints;
  int type;
  char pad10[0x28];
  RwV3d ctrlPoints[1];
} Spline545340;
typedef void *(*MallocFn)(unsigned int size, unsigned int hint);
typedef void (*FreeFn)(void *p);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBALA(float *, DAT_007dc67c, 11, 0x007dc67c);
int FUN_004d7ff0(int code, ...);
RwError *FUN_004d8480(RwError *err);
// MATCH: 0x00545340 FUN_00545340
Spline545340 *FUN_00545340(Spline545340 *param_1, RwV3d *param_2)
{
  RwError err;
  int n;
  int k;
  int half;
  int m;
  int i;
  int j;
  int *a;
  float scale;
  float w;
  float c;
  RwV3d sum;
  RwV3d *out;
  float *table;
  float *row;
  float *row2;
  RwV3d *mid;
  RwV3d *p;
  int half2;
  int off;
  int *pb;
  float *wp;
  RwV3d *pp;
  int cnt;
  int idx;

  switch (param_1->type) {
  case 1:
    n = param_1->numCtrlPoints - 2;
    break;
  case 2:
    n = param_1->numCtrlPoints - 3;
    break;
  default:
    err.pluginID = 0x102;
    err.errorCode = FUN_004d7ff0(1);
    FUN_004d8480(&err);
    n = 0;
    break;
  }
  switch (param_1->type) {
  case 1:
    k = n;
    if (k >= 10) k = 10;
    table = DAT_007dc67c[k];
    if (table == 0) {
      err.pluginID = 0x102;
      err.errorCode = FUN_004d7ff0(0x80000016);
      FUN_004d8480(&err);
      return 0;
    }
    half = k / 2;
    mid = param_2 - half;
    row = table;
    row2 = table + (k - n) * k;
    out = &param_1->ctrlPoints[1];
    for (i = 0; i < n; i++) {
      sum.x = DAT_005d757c;
      sum.y = DAT_005d757c;
      sum.z = DAT_005d757c;
      if (i < half) {
        if (k > 0) {
          wp = row;
          pp = param_2;
          cnt = k;
          do {
            cnt--;
            sum.x += *wp * pp->x;
            sum.y += *wp * pp->y;
            sum.z += *wp * pp->z;
            wp++;
            pp++;
          } while (cnt != 0);
        }
      } else if (i < n - half) {
        if (k > 0) {
          pp = mid;
          wp = table + half * k;
          cnt = k;
          do {
            cnt--;
            sum.x += *wp * pp->x;
            sum.y += *wp * pp->y;
            sum.z += *wp * pp->z;
            pp++;
            wp++;
          } while (cnt != 0);
        }
      } else if (k > 0) {
        pp = param_2 + (n - k);
        wp = row2;
        cnt = k;
        do {
          cnt--;
          sum.x += *wp * pp->x;
          sum.y += *wp * pp->y;
          sum.z += *wp * pp->z;
          pp++;
          wp++;
        } while (cnt != 0);
      }
      *out = sum;
      out++;
      mid++;
      row += k;
      row2 += k;
    }
    param_1->ctrlPoints[0] = param_1->ctrlPoints[2];
    param_1->ctrlPoints[n + 1] = param_1->ctrlPoints[n - 1];
    return param_1;
  case 2:
    k = n;
    if (k >= 10) k = 10;
    half = k / 2;
    if (k % 2 == 0) {
      a = (int *)(*(MallocFn *)(DAT_007d3ff8 + 0x108))(half * 4, 0x30102);
      if (a == 0) {
        err.pluginID = 0x102;
        err.errorCode = FUN_004d7ff0(0x80000013, half * 4);
        FUN_004d8480(&err);
        return 0;
      }
      a[0] = 1;
      a[1] = 4;
      for (j = 2; j < half; j++) a[j] = a[j - 1] * 4 - a[j - 2];
      scale = DAT_005cc320 / (float)a[half - 1];
      (*(FreeFn *)(DAT_007d3ff8 + 0x10c))(a);
      m = half + 1;
      a = (int *)(*(MallocFn *)(DAT_007d3ff8 + 0x108))(m * 4, 0x30102);
      off = half * 4;
      if (a == 0) {
        err.pluginID = 0x102;
        err.errorCode = FUN_004d7ff0(0x80000013, m * 4);
        FUN_004d8480(&err);
        return 0;
      }
      a[0] = 1;
      a[1] = 2;
      for (j = 2; j < m; j++) a[j] = a[j - 1] * 4 - a[j - 2];
    } else {
      m = half + 1;
      a = (int *)(*(MallocFn *)(DAT_007d3ff8 + 0x108))(m * 4, 0x30102);
      if (a == 0) {
        err.pluginID = 0x102;
        err.errorCode = FUN_004d7ff0(0x80000013, m * 4);
        FUN_004d8480(&err);
        return 0;
      }
      a[0] = 1;
      a[1] = 3;
      for (j = 2; j < m; j++) a[j] = a[j - 1] * 4 - a[j - 2];
      off = half * 4;
      scale = DAT_005cc320 / (float)a[half];
      (*(FreeFn *)(DAT_007d3ff8 + 0x10c))(a);
      a = (int *)(*(MallocFn *)(DAT_007d3ff8 + 0x108))(m * 4, 0x30102);
      if (a == 0) {
        err.pluginID = 0x102;
        err.errorCode = FUN_004d7ff0(0x80000013, m * 4);
        FUN_004d8480(&err);
        return 0;
      }
      a[0] = 1;
      a[1] = 5;
      for (j = 2; j < m; j++) a[j] = a[j - 1] * 4 - a[j - 2];
    }
    for (j = half - 1; j >= 0; j -= 2) a[j] = -a[j];
    half2 = (k + 1) / 2;
    out = &param_1->ctrlPoints[1];
    for (i = 0; i < n; i++) {
      w = (float)*(int *)((char *)a + off);
      p = &param_2[i % n];
      sum.x = p->x * w;
      sum.y = p->y * w;
      sum.z = p->z * w;
      pb = (int *)((char *)a + off) - 1;
      j = 1;
      if (m > 1) {
        do {
          c = (float)*pb;
          pb--;
          p = &param_2[(j + i) % n];
          j++;
          sum.x += c * p->x;
          sum.y += c * p->y;
          sum.z += c * p->z;
        } while (m > j);
      }
      if (half2 > 1) {
        cnt = half2 - 1;
        idx = n + i - 1;
        pb = (int *)((char *)a + off) - 1;
        do {
          c = (float)*pb;
          p = &param_2[idx % n];
          pb--;
          idx--;
          sum.x += c * p->x;
          sum.y += c * p->y;
          cnt--;
          sum.z += c * p->z;
        } while (cnt != 0);
      }
      out->x = scale * sum.x;
      out->y = scale * sum.y;
      out->z = scale * sum.z;
      out++;
    }
    param_1->ctrlPoints[0] = param_1->ctrlPoints[n];
    param_1->ctrlPoints[n + 1] = param_1->ctrlPoints[1];
    param_1->ctrlPoints[n + 2] = param_1->ctrlPoints[2];
    (*(FreeFn *)(DAT_007d3ff8 + 0x10c))(a);
    return param_1;
  }
  err.pluginID = 0x102;
  err.errorCode = FUN_004d7ff0(1);
  FUN_004d8480(&err);
  return 0;
}
