// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046c7d0 (852 B): 100.0% (regs 100, ops 100), relocheck OK, by hand (h3). Raw 31.0%.
   What moved it: car record as DAT_008815a0[i].field (no Car* local: the original addresses
   [edi + 0x881f..]), V3 struct copy of +0x9d4, one float h reused for heading / limit / angle (all three
   live in the param_1 home slot), a separate t for the |4*b| clamp so it stays on the x87 stack,
   n = (int)FUN_00408a50(i) as its own statement, and cl 13.00 reassociating one-line float expressions:
   the dot product, y * (float)param_2, * (1/60) and * K are split into separate statements (relocheck
   caught the 1/60 / K order, which the masked byte score cannot see).
   param_2 is unsigned (fild + 2^32 fixup). acos = FUN_004a3384 (plain CRT entry, pilot finding 5).
   Constants (read from the image): 0x005ccae0 = 57.295799255371094 (double), 0x005ccad8 = 90.0 (double),
   0x005ccad0 = 90.0f, 0x005ccac4 = 360, 0x005cc72c = 30, 0x005cc728 = 60, 0x005ccacc = 45,
   0x005cea58 = 0x392ec33e, 0x005cc574 = 2, 0x005cd694 = 0x3c888889, 0x005cc35c = 4, 0x005cd984 = 0x3cb60b61. */
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
typedef struct { float m[16]; } Mtx;   /* 0x40 */
typedef struct {
    char p0[0x10];
    int f10;                        /* 0x10 */
    char p1[0x928 - 0x14];
    Mtx mt[2];                      /* 0x928 */
    int mi;                         /* 0x9a8 */
    char p2[0x9d4 - 0x9ac];
    V3 v9d4;                        /* 0x9d4 */
    char p3[0x9e4 - 0x9e0];
    float f9e4;                     /* 0x9e4 */
    char p4[0x9f0 - 0x9e8];
    int f9f0;                       /* 0x9f0 */
    char p5[0xd04 - 0x9f4];
} Car;                              /* 0xd04 */
typedef struct { char c[0x4c]; } InBlk;
typedef struct { int blk; int p[3]; } CarIn;   /* 0x10 */
MGLOBALA(Car, DAT_008815a0, 16, 0x008815a0);
MGLOBALA(InBlk, DAT_007f1038, 32, 0x007f1038);
MGLOBAL(InBlk, DAT_007f19b8, 0x007f19b8);
MGLOBALA(CarIn, DAT_007f1a14, 16, 0x007f1a14);
MGLOBAL(V3, DAT_006146f0, 0x006146f0);
MGLOBAL(V3, DAT_006146fc, 0x006146fc);
MGLOBAL(float, DAT_00613134, 0x00613134);
int FUN_0040e350(void);
float FUN_004c39b0(V3 *out, V3 *in);
double FUN_004a3384(double x);   /* acos, CRT stack-argument entry */
float FUN_00408a50(int i);
int FUN_004093b0(int n, float *a, float *b);
Mtx *FUN_004c4d20(Mtx *m, V3 *axis, float angle, int mode);

// MATCH: 0x0046c7d0 FUN_0046c7d0
int FUN_0046c7d0(uint param_1, uint param_2)
{
  char *p;
  V3 v;
  float a, b, h, d, y, r, t;
  int n;

  if (param_1 >= 0x10) {
    return 0;
  }
  p = DAT_007f1038[DAT_007f1a14[param_1].blk].c;
  if (DAT_008815a0[param_1].f9f0 != 0) {
    p = DAT_007f19b8.c;
  }
  if (DAT_008815a0[param_1].f10 != 0) {
    p = DAT_007f19b8.c;
  }
  if (FUN_0040e350() != 6 && FUN_0040e350() != 0xb && FUN_0040e350() != 10) {
    p = DAT_007f19b8.c;
  }
  if (p[0] == 0 && p[1] == 0) {
    v = DAT_008815a0[param_1].v9d4;
    if (v.x != 0.0f || v.z != 0.0f) {
      v.y = 0.0f;
      FUN_004c39b0(&v, &v);
      d = DAT_006146f0.y * v.y + DAT_006146f0.x * v.x;
      d = d + DAT_006146f0.z * v.z;
      if (d < -1.0f) {
        d = -1.0f;
      } else if (d > 1.0f) {
        d = 1.0f;
      }
      h = (float)(-(FUN_004a3384(d) * 57.295799255371094 - 90.0)) + 90.0f;
      v.x = DAT_006146f0.z * DAT_008815a0[param_1].v9d4.y - DAT_006146f0.y * DAT_008815a0[param_1].v9d4.z;
      v.y = DAT_006146f0.x * DAT_008815a0[param_1].v9d4.z - DAT_006146f0.z * DAT_008815a0[param_1].v9d4.x;
      v.z = DAT_006146f0.y * DAT_008815a0[param_1].v9d4.x - DAT_006146f0.x * DAT_008815a0[param_1].v9d4.y;
      if (v.y < 0.0f) {
        h = 360.0f - h;
      }
      n = (int)FUN_00408a50(param_1);
      FUN_004093b0(n, &a, &b);
      h = h - a;
      while (h < 0.0f) {
        h = h + 90.0f;
      }
      while (h > 90.0f) {
        h = h - 90.0f;
      }
      if (h < 30.0f) {
        y = -h;
      } else if (h < 60.0f) {
        y = 45.0f - h;
      } else {
        y = 90.0f - h;
      }
      h = DAT_008815a0[param_1].f9e4 * 0.00016666666f;
      if (h > 2.0f) {
        h = 2.0f;
      }
      if (y > h) {
        y = h;
      }
      h = -h;
      if (y < h) {
        y = h;
      }
      r = y * (float)param_2;
      r = r * 0.016666668f; r = r * DAT_00613134;
      t = b * 4.0f;
      if (t < 0.0f) {
        t = -t;
      }
      if (t > 45.0f) {
        t = 45.0f;
      }
      h = r * (45.0f - t) * 0.022222223f;
      if (h != 0.0f) {
        FUN_004c4d20(&DAT_008815a0[param_1].mt[DAT_008815a0[param_1].mi], &DAT_006146fc, h, 1);
      }
    }
  }
  return 1;
}
