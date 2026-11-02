// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045d7a0 (h3): 52.6% (regs 68.4%, ops 91.1%). Raw converter 15.4%. Structure: a 16-dword
   matrix local copied from FUN_00467210(0) (rep movsd) and reset to identity (flags |= 0x20003) when
   FUN_004728a0 fails; position from FUN_004417e0/FUN_00472820; delta to the smoothed position 0x00690490, length
   via FUN_004c3ac0, > 10.0f (@0x005cc55c) moves it by 0.5f (@0x005cc32c) of the delta, else snaps; then five
   writes into the object at DAT_0068f61c (+0x28 pos |1, +0x4c at + cross into +0x58 |4, +0x40 up + cross |8,
   +0x40/+0x4c/+0x58 from the matrix |0xc, +0x34 zero vector |2), written as __inline helpers. Identity-store
   and pos-zero order from the permuter (stores in that order in the original). Remaining: (1) the original does
   each flags update as or [eax+0x70],k (memory RMW, no value carried between helpers) while 9176 carries or
   reloads the flags in a register; (2) the cross product in the original addresses .x through the copy pointer
   ([ecx]/[edx]) and .y/.z as [eax+off], ours through the helper's own pointer; (3) the original loads
   DAT_0068f620 once into edx/ecx/esi after FUN_004c3ac0 and reuses it in both branches and for +0x28, ours
   reloads (extra push ebx). Tried: helpers vs open-coded vs macro cross, pointer-copy helper, volatile flags
   (66.3% but wrong instruction forms, rejected), struct vs per-component copies of 0x00690490/0x0068f634,
   permuter 300 s (48.4 -> 52.6). Residual class (b), (1)/(2) may be (a) inliner alias handling, not proven. */
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
typedef struct {
  float x;
  float y;
  float z;
} V3_0045d7a0;
typedef struct {
  V3_0045d7a0 right;
  unsigned int flags;
  V3_0045d7a0 up;
  int pad1;
  V3_0045d7a0 at;
  int pad2;
  V3_0045d7a0 pos;
  int pad3;
} M_0045d7a0;
typedef struct {
  char pad0[0x28];
  V3_0045d7a0 pos;
  V3_0045d7a0 v34;
  V3_0045d7a0 v40;
  V3_0045d7a0 v4c;
  V3_0045d7a0 v58;
  int pad64[3];
  unsigned int flags;
} F_0045d7a0;
MGLOBAL(F_0045d7a0 *, DAT_0068f61c, 0x0068f61c);
MGLOBAL(V3_0045d7a0, DAT_0068f620, 0x0068f620);
MGLOBAL(V3_0045d7a0, DAT_0068f634, 0x0068f634);
MGLOBAL(V3_0045d7a0, DAT_00690490, 0x00690490);
M_0045d7a0 *FUN_00467210(int p0);
int FUN_004728a0(M_0045d7a0 *p0);
void FUN_004417e0(V3_0045d7a0 *p0);
int FUN_00472820(V3_0045d7a0 *p0);
float FUN_004c3ac0(V3_0045d7a0 *p0);
static __inline void Cross_0045d7a0(V3_0045d7a0 *o, V3_0045d7a0 *a, V3_0045d7a0 *b)
{
  o->x = b->z * a->y - b->y * a->z;
  o->y = a->z * b->x - b->z * a->x;
  o->z = a->x * b->y - a->y * b->x;
}
static __inline void SetPos_0045d7a0(F_0045d7a0 *f, V3_0045d7a0 *v)
{
  f->pos = *v;
  f->flags |= 1;
}
static __inline void SetAt_0045d7a0(F_0045d7a0 *f, V3_0045d7a0 *v)
{
  f->v4c = *v;
  Cross_0045d7a0(&f->v58, &f->v40, &f->v4c);
  f->flags |= 4;
}
static __inline void SetUp_0045d7a0(F_0045d7a0 *f, V3_0045d7a0 *v)
{
  f->v40 = *v;
  Cross_0045d7a0(&f->v58, &f->v40, &f->v4c);
  f->flags |= 8;
}
static __inline void SetAxes_0045d7a0(F_0045d7a0 *f, M_0045d7a0 *m)
{
  f->v40 = m->up;
  f->v4c = m->at;
  f->v58 = m->right;
  f->flags |= 0xc;
}
static __inline void SetV34_0045d7a0(F_0045d7a0 *f, V3_0045d7a0 *v)
{
  f->v34 = *v;
  f->flags |= 2;
}
// MATCH: 0x0045d7a0 FUN_0045d7a0
void FUN_0045d7a0(void)
{
  V3_0045d7a0 d;
  V3_0045d7a0 zero;
  F_0045d7a0 *f;
  M_0045d7a0 m;
  V3_0045d7a0 pos;

  zero.x = 0.0f;
  zero.y = 0.0f;
  zero.z = 0.0f;
  m = *FUN_00467210(0);
  if (FUN_004728a0(&m) == 0) {
    m.right.x = 1.0f;
    m.up.x = 0.0f;
    m.right.z = 0.0f;
    m.right.y = 0.0f;
    m.at.y = 0.0f;
    m.at.x = 0.0f;
    m.up.z = 0.0f;
    m.pos.z = 0.0f;
    m.pos.y = 0.0f;
    m.pos.x = 0.0f;
    m.at.z = 1.0f;
    m.up.y = 1.0f;
    m.flags |= 0x20003;
  }
  FUN_004417e0(&pos);
  if (FUN_00472820(&pos) == 0) {
    pos.y = 0.0f;
    pos.z = 0.0f;
    pos.x = 0.0f;
  }
  d.x = pos.x - DAT_00690490.x;
  DAT_0068f620 = pos;
  d.y = pos.y - DAT_00690490.y;
  d.z = pos.z - DAT_00690490.z;
  if (FUN_004c3ac0(&d) > 10.0f) {
    DAT_0068f634.x = d.x * 0.5f + DAT_00690490.x;
    DAT_00690490.x = DAT_0068f634.x;
    DAT_0068f634.y = d.y * 0.5f + DAT_00690490.y;
    DAT_00690490.y = DAT_0068f634.y;
    DAT_0068f634.z = d.z * 0.5f + DAT_00690490.z;
    DAT_00690490.z = DAT_0068f634.z;
  }
  else {
    DAT_0068f634 = DAT_0068f620;
    DAT_00690490 = DAT_0068f620;
  }
  f = DAT_0068f61c;
  SetPos_0045d7a0(f, &DAT_0068f620);
  SetAt_0045d7a0(f, &m.at);
  SetUp_0045d7a0(f, &m.up);
  SetAxes_0045d7a0(f, &m);
  SetV34_0045d7a0(f, &zero);
}
