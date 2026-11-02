// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 h4 0x00490380: 100% (regs 100%, ops 100%), whole symbol byte-identical (272 B, relocations masked);
   by hand from the decomp (raw converter 24.1%). Moved the score: the 30-record array indexed as
   DAT_0086a4a0[i].field (a record pointer local biases esi by -0x18); p->t += dt and later reads of
   DAT_0086a4a0[i].t (the original spills the CSE'd value: fst [esp+0xc]); the four .rdata floats written as
   literals 1.1f (0x005ccabc), 0.5f (0x005cc32c), 6.0f (0x005cd0a0), 0.0f (0x005d757c): as named globals the
   x87 operand order of the 1.1f multiply flips. sin is the fsin intrinsic. Residual: none. */
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
typedef struct { float x, y, z; } RwV3d;
typedef struct {
  int active;      /* +0x00 */
  char *atomic;    /* +0x04 */
  int pad08;       /* +0x08 */
  int obj;         /* +0x0c */
  RwV3d pos;       /* +0x10 */
  RwV3d vel;       /* +0x1c */
  RwV3d scale;     /* +0x28 */
  float f34;       /* +0x34 */
  int f38;         /* +0x38 */
  int f3c;         /* +0x3c */
  RwV3d axis;      /* +0x40 */
  float t;         /* +0x4c */
} Spark0086a4a0;
MGLOBAL(float, DAT_007f100c, 0x007f100c);
MGLOBALA(Spark0086a4a0, DAT_0086a4a0, 30, 0x0086a4a0);
double sin(double);
#pragma intrinsic(sin)
int FUN_004c1340(char *frame, RwV3d *v, int op);
int FUN_004c13e0(char *frame, RwV3d *v, int op);
int FUN_004c1520(char *frame, RwV3d *axis, float angle, int op);
// MATCH: 0x00490380 FUN_00490380
void FUN_00490380(void)
{
  int i;
  float dy;
  char *frame;

  for (i = 0; i < 30; i++) {
    if (DAT_0086a4a0[i].active) {
      DAT_0086a4a0[i].pos.x = DAT_0086a4a0[i].vel.x + DAT_0086a4a0[i].pos.x;
      DAT_0086a4a0[i].pos.z = DAT_0086a4a0[i].vel.z + DAT_0086a4a0[i].pos.z;
      DAT_0086a4a0[i].t += DAT_007f100c;
      if (DAT_0086a4a0[i].active == 3) {
        dy = DAT_0086a4a0[i].f34 = DAT_0086a4a0[i].f34 * 1.1f;
        if (DAT_0086a4a0[i].t >= 0.5f) DAT_0086a4a0[i].active = 0;
      } else {
        dy = sin(DAT_0086a4a0[i].t * 6.0f) * DAT_0086a4a0[i].vel.y;
        if (DAT_0086a4a0[i].active == 1 && dy < 0.0f) DAT_0086a4a0[i].active = 2;
        if (DAT_0086a4a0[i].active == 2) {
          if (dy > DAT_0086a4a0[i].f34) {
            DAT_0086a4a0[i].active = 3;
            DAT_0086a4a0[i].t = 0;
            dy = -DAT_0086a4a0[i].vel.y;
          }
          DAT_0086a4a0[i].f34 = dy;
        }
      }
      DAT_0086a4a0[i].pos.y += dy;
      DAT_0086a4a0[i].f3c += DAT_0086a4a0[i].f38;
      frame = *(char **)(DAT_0086a4a0[i].atomic + 4);
      FUN_004c13e0(frame, &DAT_0086a4a0[i].scale, 0);
      FUN_004c1520(frame, &DAT_0086a4a0[i].axis, (float)DAT_0086a4a0[i].f3c, 2);
      FUN_004c1340(frame, &DAT_0086a4a0[i].pos, 2);
    }
  }
}
