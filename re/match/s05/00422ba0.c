// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00422ba0 (1071 B): best 92.2% exact (regs 92.9, ops 100.0) by hand + match_permute. Raw g2s 31.1%.
   Walks the 256-entry event ring backwards from DAT_007e9de0-1 while the stamp DAT_007ea1e4[k] equals
   DAT_007f1030 (at most 0x100 entries), dispatching on DAT_007e9de4[k]: 10 -> FUN_0044e0a0(&local record of
   0x30 + V3 from the 0x40-stride ring at 0x007ec9e4 + who + two dwords of the 0x20-stride ring at 0x007ea9e4),
   9 -> FUN_004219f0; then, for who = DAT_007ea5e4[k]-0xff00 < 0x10: 8, 6, 4, 3 (if chain; a switch on type gave
   59-82%). Type 6's colour test is a switch (0xff0000ff/0xffff0080 share a body). V3 temporaries are filled
   field by field (a struct copy gave lea-based loads). Per-player 0x138-stride records at 0x008995b0.
   Note: the float distance in type 3 is spilled to [esp+0x10], the slot that also holds k; the original does
   the same, so this is the compiler's packing, not a port error.
   Residual (b): every remaining difference is one stack slot: the out-parameter st of type 6's FUN_0046cbb0 call
   sits at esp+0x14 (before the V3 temporary) in the original and after stop in ours, shifting v/type/stop by 4.
   Tried: declaration order (both directions), renaming, block-scoping st/x6c, block-scoping v (frame grows to
   0xb0), switch forms; none moved the slot. Variants of the V3 fill order score up to 93.6% under the masked
   compare but load the wrong fields (relocheck BAD); x,y,z order is the one with relocheck OK. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
typedef struct { float x, y, z; } V3_ba0;
typedef struct { int m[12]; } M30_ba0;
typedef struct { int a, b, c; int pad[5]; } Ev20_ba0;            /* stride 0x20 at 0x007ea9e4 */
typedef struct {
  union {
    M30_ba0 mat;
    struct { float f0, f4; int f8, fc; V3_ba0 p10; int f1c, f20; V3_ba0 p24; } s;
  } u;
  V3_ba0 p30;
  int f3c;
} Ev40_ba0;                                                        /* stride 0x40 at 0x007ec9e4 */
typedef struct { M30_ba0 mat; V3_ba0 pos; unsigned int who; int a; int c; } Ev10_ba0;
typedef struct {
  int hits;                /* +0x00 (0x008995b0) */
  int pad1[6];
  int stamp[4];            /* +0x1c */
  float dist[4];           /* +0x2c */
  int other;               /* +0x3c */
  int t;                   /* +0x40 */
  int flag;                /* +0x44 */
  int rest[0x3c];
} P_ba0;                                                           /* stride 0x138 */
MGLOBAL(int, DAT_007e9de0, 0x007e9de0);
MGLOBALA(int, DAT_007e9de4, 256, 0x007e9de4);
MGLOBALA(int, DAT_007ea1e4, 256, 0x007ea1e4);
MGLOBALA(int, DAT_007ea5e4, 256, 0x007ea5e4);
MGLOBALA(Ev20_ba0, DAT_007ea9e4, 256, 0x007ea9e4);
MGLOBALA(Ev40_ba0, DAT_007ec9e4, 256, 0x007ec9e4);
MGLOBAL(int, DAT_007f1030, 0x007f1030);
MGLOBAL(int, DAT_007f0ff8, 0x007f0ff8);
MGLOBALA(P_ba0, DAT_008995b0, 4, 0x008995b0);
int FUN_0040e470(int i);
void FUN_00417370(int i);
void FUN_004219f0(int a);
void FUN_0044e0a0(Ev10_ba0 *e);
int FUN_0046cbb0(unsigned int i, int *a, int *b);
int FUN_0046cbe0(unsigned int i, int a, int b);
void FUN_0047cea0(int a, V3_ba0 *v);
int FUN_00480720(int i);
void FUN_00480d60(int i, int a);
void FUN_004922e0(int i, int a, int b, int c);
float FUN_004c3ac0(V3_ba0 *v);
// MATCH: 0x00422ba0 FUN_00422ba0
int FUN_00422ba0(void)
{
  int k, cnt, stop, stamp, i, type, st, x6c;
  unsigned int who;
  int arr1[4];
  int arr2[4];
  Ev10_ba0 ev;
  V3_ba0 v;
  float d;

  for (i = 0; i < 4; i++)
    FUN_0046cbb0(i, &arr1[i], &arr2[i]);
  k = DAT_007e9de0 - 1;
  if (k < 0)
    k = 0xff;
  stamp = DAT_007f1030;
  cnt = 0x100;
  stop = 0;
  do {
    if (stop)
      break;
    if (DAT_007ea1e4[k] == stamp) {
      who = DAT_007ea5e4[k] - 0xff00;
      type = DAT_007e9de4[k];
      if (type == 10) {
        ev.a = DAT_007ea9e4[k].a;
        ev.c = DAT_007ea9e4[k].c;
        ev.who = who;
        ev.mat = DAT_007ec9e4[k].u.mat;
        ev.pos = DAT_007ec9e4[k].p30;
        FUN_0044e0a0(&ev);
      } else if (type == 9) {
        FUN_004219f0(DAT_007ea9e4[k].b);
      }
      if (who < 0x10) {
        if (type == 8) {
          v.y = DAT_007ec9e4[k].u.s.f4 * 0.005f;
          v.x = 0.0f;
          v.z = 0.0f;
          FUN_0047cea0(DAT_007ea9e4[k].b, &v);
        } else if (type == 6) {
          int a = DAT_007ea9e4[k].a;
          FUN_0046cbb0(who, &st, &x6c);
          switch (a) {
          case 0xff0000ff:
          case 0xffff0080:
            if (st == 0 || st == 1) {
              FUN_0046cbe0(who, 2, 0);
              FUN_004922e0(who, 2, 0x1e, 0xff);
            }
            break;
          case 0xff010101:
            if (st == 0)
              FUN_0046cbe0(who, 1, 0);
            break;
          }
        } else if (type == 4) {
          int a = DAT_007ea9e4[k].a;
          int b = DAT_007ea9e4[k].b;
          FUN_004922e0(a, 1, 2, 0xff);
          FUN_004922e0(b, 1, 2, 0xff);
          if (arr1[a] == 0 && arr1[b] == 0) {
            int t = DAT_007f0ff8;
            DAT_008995b0[a].other = b + 1000;
            DAT_008995b0[a].t = t;
            DAT_008995b0[b].t = t;
            DAT_008995b0[b].other = a + 1000;
            DAT_008995b0[a].stamp[b] = t;
            DAT_008995b0[b].stamp[a] = t;
            v.x = DAT_007ec9e4[k].u.s.p24.x;
            v.y = DAT_007ec9e4[k].u.s.p24.y;
            v.z = DAT_007ec9e4[k].u.s.p24.z;
            d = FUN_004c3ac0(&v);
            DAT_008995b0[a].dist[b] = d;
            DAT_008995b0[b].dist[a] = d;
            DAT_008995b0[a].flag = 0;
            DAT_008995b0[b].flag = DAT_007ea9e4[k].c == 1;
            if (FUN_0040e470(a) != 1)
              FUN_00417370(a);
            if (FUN_0040e470(b) != 1)
              FUN_00417370(b);
          }
        } else if (type == 3) {
          DAT_008995b0[who].hits++;
          v.x = DAT_007ec9e4[k].u.s.p10.x;
          v.y = DAT_007ec9e4[k].u.s.p10.y;
          v.z = DAT_007ec9e4[k].u.s.p10.z;
          d = FUN_004c3ac0(&v);
          if (d > 100.0f)
            FUN_004922e0(who, 1, 2, 0xff);
          if (d > 2500.0f) {
            FUN_00480720(who);
            FUN_00480d60(who, 7);
          }
        }
      }
    } else {
      stop = 1;
    }
    k--;
    if (k < 0)
      k = 0xff;
  } while (--cnt > 0);
  return 1;
}
