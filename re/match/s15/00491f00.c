// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 h6 0x00491f00: 88.3% (regs 89.4%). This file also holds 0x00491e50 (static callee, buffer in EAX and source entry in EDI, both picked by the parameter order; 100%, its only caller is 0x00491f00). FILE-STATIC global: DAT_00771724 is static under C (pilot finding 2: the original reloads DAT_007dc57c after the store but keeps DAT_00771724 in ecx; 85.6% -> 88.3%), MGLOBAL under C++. Hand: Spark (0x20) and SparkSrc (0x38) structs, lock outputs as {ptr, stride} structs, live-count local n (the original keeps DAT_00771720 in esi through the loop). Tried: e by index / by pointer walk, age as ++ / local / re-read, static on the other globals (no change), the permuter (89.9% only by moving e's assignment below its first use, rejected as wrong). Residual (b): x87 operand order on d[0]/d[2] (original fld pos then fsubr e->a), age kept in two registers (edx stored, eax compared). */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
  int active;
  int age;
  float pos[3];
  float vel[3];
} Spark;
typedef struct {
  float a[3];
  float r;
  float b[3];
  float c[3];
  float d[3];
  int f34;
} SparkSrc;
typedef struct {
  float *p;
  int stride;
} LockPos;
typedef struct {
  unsigned char *p;
  int stride;
} LockCol;
MGLOBAL(Spark *, DAT_00869ca0, 0x00869ca0);
MGLOBAL(int, DAT_00771718, 0x00771718);
MGLOBAL(int, DAT_0077171c, 0x0077171c);
MGLOBAL(int, DAT_00771720, 0x00771720);
#ifndef __cplusplus
static int DAT_00771724;
#else
MGLOBAL(int, DAT_00771724, 0x00771724);
#endif
MGLOBALA(SparkSrc, DAT_00771558, 8, 0x00771558);
MGLOBAL(float, DAT_005cf694, 0x005cf694);
MGLOBAL(float, DAT_005ce18c, 0x005ce18c);
MGLOBAL(float, DAT_005cc9c0, 0x005cc9c0);
MGLOBAL(int, DAT_007dc57c, 0x007dc57c);
float FUN_00472650(float lo, float hi);
float FUN_004c3ac0(float *v);
int FUN_004c1b40(int p0, float *p1);
int FUN_00487e20(int p0, void *p1, int p2, int p3);
void FUN_00487e30(int p0);
// MATCH: 0x00491e50 FUN_00491e50
static void FUN_00491e50(Spark *p, SparkSrc *src)
{
  int i;
  float vx;
  float vz;
  for (i = 0; i < 0x200; p++, i++) {
    if (p->active == 0) {
      p->active = 1;
      p->age = 0;
      vx = FUN_00472650(src->b[0], src->c[0]);
      vz = FUN_00472650(src->b[2], src->c[2]);
      p->vel[0] = vx;
      p->vel[1] = FUN_00472650(src->b[1], src->c[1]);
      p->vel[2] = vz;
      p->pos[0] = FUN_00472650(-src->r, src->r) + src->a[0];
      p->pos[1] = src->a[1];
      p->pos[2] = FUN_00472650(-src->r, src->r) + src->a[2];
      return;
    }
  }
}
// MATCH: 0x00491f00 FUN_00491f00
void FUN_00491f00(int param_1)
{
  Spark *buf;
  Spark *p;
  SparkSrc *e;
  int i;
  int a;
  int n;
  int age;
  float *mp;
  float d[3];
  LockPos uv;
  LockPos pos;
  LockCol col;
  float f;

  buf = DAT_00869ca0;
  if (DAT_0077171c != 0) {
    for (i = 0; i < DAT_00771718; i++) {
      e = &DAT_00771558[i];
      mp = *(float **)(param_1 + 4);
      d[0] = e->a[0] - mp[0x10];
      d[1] = e->a[1] - mp[0x11];
      d[2] = e->a[2] - mp[0x12];
      if (FUN_004c3ac0(d) < DAT_005cf694 && FUN_004c1b40(param_1, e->d) == 2) {
        FUN_00491e50(buf, e);
      }
    }
  }
  DAT_0077171c++;
  DAT_00771720 = 0;
  FUN_00487e20(DAT_00771724, &pos, 1, 0x40000000);
  FUN_00487e20(DAT_00771724, &col, 2, 0x40000000);
  FUN_00487e20(DAT_00771724, &uv, 4, 0x40000000);
  if (pos.p != 0 && col.p != 0 && uv.p != 0) {
    n = DAT_00771720;
    p = buf;
    for (i = 0x200; i != 0; i--, p++) {
      if (p->active == 1) {
        n++;
        p->pos[0] += p->vel[0];
        age = p->age + 1;
        p->age = age;
        p->pos[1] += p->vel[1];
        p->pos[2] += p->vel[2];
        if (age < 0x40) {
          pos.p[0] = p->pos[0];
          pos.p[1] = p->pos[1];
          pos.p[2] = p->pos[2];
          pos.p = (float *)((char *)pos.p + pos.stride);
          col.p[0] = (unsigned char)(p->age / 2 - 0x40);
          col.p[1] = (unsigned char)(p->age / 2 + 0x40);
          col.p[2] = 8;
          a = 0xff - p->age * 4;
          if (a < 0) {
            a = 0;
          } else if (a > 0xff) {
            a = 0xff;
          }
          col.p[3] = (unsigned char)a;
          col.p += col.stride;
          f = (float)p->age * DAT_005ce18c + DAT_005cc9c0;
          uv.p[0] = f;
          uv.p[1] = f;
          uv.p = (float *)((char *)uv.p + uv.stride);
        } else {
          p->active = 0;
          p->age = 0;
          n--;
        }
      }
    }
    DAT_00771720 = n;
    FUN_00487e30(DAT_00771724);
  }
  *(unsigned int *)(*(int *)(DAT_007dc57c + DAT_00771724) + 0x40) |= 0x800000;
  *(int *)(*(int *)(DAT_007dc57c + DAT_00771724) + 4) = DAT_00771720;
}
