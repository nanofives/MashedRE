// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00425fa0 (125 B), h9 by hand: 100.0% (regs 100.0%). Raw 56.1%. Same 8 x 0x4c table (MGLOBALA at
   0x00899260), indexed by param_1; param_2 is a {V3 center; float radius} filled by a struct copy of record +0x34
   and the radius at +0x28 of FUN_004b3f90's result (1 arg; flag byte +0x4c bit 2 -> FUN_004e5fc0 first). */
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
typedef struct { float x; float y; float z; } V3;
typedef struct { V3 center; float radius; } Sphere;
typedef struct {
  int pad00[7];          /* +0x00 */
  Sphere bsphere;        /* +0x1c */
  int pad2c[8];          /* +0x2c */
  unsigned char iflags;  /* +0x4c */
} AtomicB;
typedef struct {
  void *clump;     /* +0x00 */
  void *body;      /* +0x04 */
  void *part0;     /* +0x08 */
  void *part1;     /* +0x0c */
  int pad10[9];    /* +0x10 */
  V3 pos;          /* +0x34 */
  float time;      /* +0x40 */
  void *anim;      /* +0x44 */
  int pad48;       /* +0x48 */
} AnimObj;
MGLOBALA(AnimObj, DAT_00899260, 8, 0x00899260);
AtomicB *FUN_004b3f90(void *clump);
void FUN_004e5fc0(AtomicB *a);
// MATCH: 0x00425fa0 FUN_00425fa0
void FUN_00425fa0(int param_1, Sphere *param_2)
{
  if (DAT_00899260[param_1].anim != 0 && DAT_00899260[param_1].clump != 0) {
    param_2->center = DAT_00899260[param_1].pos;
    if ((FUN_004b3f90(DAT_00899260[param_1].clump)->iflags & 2) != 0) {
      FUN_004e5fc0(FUN_004b3f90(DAT_00899260[param_1].clump));
    }
    param_2->radius = FUN_004b3f90(DAT_00899260[param_1].clump)->bsphere.radius;
  }
}
