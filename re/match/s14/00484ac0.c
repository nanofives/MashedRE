// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 (h1) 0x00484ac0: 70.3% (regs 70.3%). File also holds the static register-arg callee 0x00484a50
   (100%, EAX = grid cell, ECX = object; two separate early-return tests give the original block order).
   0x00484ac0 itself takes its object on the stack (Ghidra's unaff_* came from the _ftol calls).
   0x004a3090 is the CRT floor entry (double arg on the stack), declared as FUN_004a3090(double).
   0x005cf28c (0.2222f) is a compiler literal (literal gives fld o->r; fmul [k], the global form loads the
   constant first); 0x005cf290 / 0x005cf288 / 0x005cda64 stay globals (as literals they reorder loads).
   Residual, class (b): (1) original loads the object into EBP before the null test; (2) original keeps
   (24.75 + org.z) on the x87 stack after the dz store and pops it (fsubr st(1); fstp st(0)), we fold it;
   (3) stack slots: fx / z1 share [esp+0x34], ours differ; (4) the outer loop entry is `jmp` + `lea ecx,[ecx]`
   padding in the original. Tried: in-place `+=`/`-=` on the copied vector, separate ox/oz temps (best),
   a second vector for the offsets, nested-if vs early-return, operand orders, literals vs globals,
   permuter (2 runs; one "best" variant moved `org.z -= o->z` after its use and was rejected as wrong). */
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
typedef struct V3 { float x, y, z; } V3;
struct Cell;
typedef struct Obj {
  float x, y, z, r;            /* 0x00 */
  int key;                     /* 0x10 */
  int pad14[4];                /* 0x14 */
  struct Cell *cells[16];      /* 0x24 */
  int ncells;                  /* 0x64 */
} Obj;
typedef struct Cell {
  Obj *items[100];             /* 0x000 */
  int count;                   /* 0x190 */
  int pad194[6];               /* 0x194 */
  int key;                     /* 0x1ac */
  int mixed;                   /* 0x1b0 */
  int full;                    /* 0x1b4 */
} Cell;                        /* 0x1b8 */
typedef Cell CellRow[11];
MGLOBAL(V3 *, DAT_006dccb0, 0x006dccb0);
MGLOBALA(CellRow, DAT_006cfcb8, 11, 0x006cfcb8);
MGLOBAL(float, DAT_005cf290, 0x005cf290);
MGLOBAL(float, DAT_005cf288, 0x005cf288);
MGLOBAL(float, DAT_005cda64, 0x005cda64);
double FUN_004a3090(double x);

// MATCH: 0x00484a50 FUN_00484a50
static int FUN_00484a50(Cell *c, Obj *o)
{
  if (c->full != 0) {
    return 0;
  }
  if (c->count >= 100) {
    return 0;
  }
  {
    c->items[c->count] = o;
    c->count++;
    if (c->mixed == 0) {
      if (c->key == 0) {
        c->key = o->key;
      }
      if (c->key != o->key) {
        c->mixed = 1;
        c->key = o->key;
      }
    }
    o->cells[o->ncells] = c;
    o->ncells++;
    return 1;
  }
}

// MATCH: 0x00484ac0 FUN_00484ac0
int FUN_00484ac0(Obj *o)
{
  V3 org, d;
  float ox, oz, fx, fz, r;
  int ix, iz, x0, x1, z0, z1, i, j;

  if (DAT_006dccb0 == 0) {
    return 0;
  }
  org = *DAT_006dccb0;
  ox = DAT_005cf290 + org.x;
  oz = DAT_005cf290 + org.z;
  org.x = ox - o->x;
  org.z = oz - o->z;
  fx = (org.x * DAT_005cf288) * DAT_005cda64;
  ix = (int)FUN_004a3090(fx);
  fz = (org.z * DAT_005cf288) * DAT_005cda64;
  iz = (int)FUN_004a3090(fz);
  if (ix >= 0 && ix < 11 && iz >= 0 && iz < 11 && FUN_00484a50(&DAT_006cfcb8[ix][iz], o)) {
  r = 0.222222222f * o->r;
  x0 = (int)(fx - r);
  x1 = (int)(fx + r);
  z0 = (int)(fz - r);
  z1 = (int)(fz + r);
  if (x0 < 0) x0 = 0;
  if (x1 > 10) x1 = 10;
  if (z0 < 0) z0 = 0;
  if (z1 > 10) z1 = 10;
  for (i = x0; i <= x1; i++) {
    for (j = z0; j <= z1; j++) {
      if (i != ix || j != iz) {
        FUN_00484a50(&DAT_006cfcb8[i][j], o);
      }
    }
  }
    return 1;
  }
  return 0;
}
