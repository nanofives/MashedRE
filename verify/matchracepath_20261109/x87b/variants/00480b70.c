// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x00480b70 (h9, by hand): 93.1% (regs 93.1%). Wheel-contact count must be a 4-iteration loop
   (the compiler unrolls it; four ifs give 40%). 20.0 @0x005ccd6c and -20.0 @0x005cd61c are literals.
   Residual (b): in both FUN_0055b650 blocks the original issues the first fld/fmul (vel.x) before the
   integer copy of m->pos and the third fld/fmul before push edx; ours schedules them later. Tried:
   permuter, 7 statement orders, struct vs field copy, float arrays, global constants, inline scale helper,
   pointer temporaries. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
typedef struct V3 { float x, y, z; } V3;
typedef struct Mat { V3 right; int f0c; V3 up; int f1c; V3 at; int f2c; V3 pos; int f3c; } Mat;
typedef struct Whl { int contact; char pad[0xc0]; } Whl;   /* stride 0xc4 */
typedef struct Carb70 {
  char pad000[0x188];
  Whl wheel[4];         /* +0x188 (0x0088173c) */
  char pad498[0x51c];
  V3 v9b4;              /* +0x9b4 (0x00881f68) */
  V3 v9c0;              /* +0x9c0 (0x00881f74) */
  char pad9cc[0x338];
} Carb70;               /* stride 0xd04, base 0x008815b4 */
typedef struct Rec4c { int f0; char b4; char b5; char pad[0x46]; } Rec4c;  /* stride 0x4c */
typedef struct Sel10 { int idx; int pad[3]; } Sel10;
MGLOBALA(Carb70, DAT_008815b4, 16, 0x008815b4);
MGLOBALA(Sel10, DAT_007f1a14, 16, 0x007f1a14);
MGLOBALA(Rec4c, DAT_007f1038, 16, 0x007f1038);
MGLOBALA(int, DAT_006c9a78, 16, 0x006c9a78);
MGLOBAL(V3, DAT_006146fc, 0x006146fc);
int FUN_0057c210(int p0);
void FUN_004c3df0(V3 *p0, V3 *p1, int p2, Mat *p3);
int FUN_0055ad30(int p0, int p1);
void FUN_0055ac00(int p0, int p1, int p2);
void FUN_0046d4a0(Mat **p0, int p1);
int FUN_0046c7b0(int p0);
void FUN_0055b650(int p0, V3 *p1, V3 *p2);
// MATCH: 0x00480b70 FUN_00480b70
void FUN_00480b70(int i)
{
  Rec4c *rec;
  int n;
  int k;
  int obj;
  int geo;
  Mat *m;
  V3 pos;
  V3 vel;

  rec = &DAT_007f1038[DAT_007f1a14[i].idx];
  n = 0;
  for (k = 0; k < 4; k++) {
    if (DAT_008815b4[i].wheel[k].contact != 0) {
      n++;
    }
  }
  obj = FUN_0057c210(DAT_006c9a78[i]);
  m = *(Mat **)(*(int *)(obj + 0x10) + 8);
  FUN_004c3df0(&DAT_008815b4[i].v9b4, &DAT_006146fc, 1, m);
  if (*(int *)(obj + 0x24) == 0 ||
      ((*(unsigned int **)(*(int *)(obj + 0x24) + 0x60))[*(unsigned short *)(obj + 0x20) >> 5] &
       (1 << (*(unsigned short *)(obj + 0x20) & 0x1f))) == 0) {
    FUN_0055ad30(*(int *)(obj + 0x24), obj);
    FUN_0055ac00(*(int *)(obj + 0x24), obj, 1);
  }
  FUN_0046d4a0(&m, i);
  geo = *(int *)(*(int *)(obj + 0x10) + 4);
  if (geo != 0 && FUN_0046c7b0(i) != 0 && n != 0) {
    if (rec->b4) {
      pos.x = m->pos.x;
      pos.y = m->pos.y;
      pos.z = m->pos.z;
      vel.x = DAT_008815b4[i].v9c0.x * 20.0f;
      { float tA_ = DAT_008815b4[i].v9c0.y; vel.y = tA_ * 20.0f; }
      vel.z = DAT_008815b4[i].v9c0.z * 20.0f;
      FUN_0055b650(geo, &vel, &pos);
    }
    if (rec->b5) {
      pos.x = m->pos.x;
      pos.y = m->pos.y;
      pos.z = m->pos.z;
      vel.x = DAT_008815b4[i].v9c0.x * -20.0f;
      { float tA_ = DAT_008815b4[i].v9c0.y; vel.y = tA_ * -20.0f; }
      vel.z = DAT_008815b4[i].v9c0.z * -20.0f;
      FUN_0055b650(geo, &vel, &pos);
    }
  }
}
