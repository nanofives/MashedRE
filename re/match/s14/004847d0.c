// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 (h1) 0x004847d0: 34.9% (regs 37.3%). Builds the 11 x 11 grid of 0x1b8-byte cells at 0x006cfcb8
   (same Cell layout as 0x00484ac0.c: count 0x190, RwBBox sup 0x194 / inf 0x1a0, key, mixed, full).
   18.0f (0x005ce178) is a compiler literal (fld dir; fmul [k]); dot product written z + x + y to get the
   original y, x, z load order. 0x004c39b0 = vector normalize (out, in), result discarded.
   Residual, class (b): the original strength-reduces every cell field address into its own pointer
   induction variable (10 of them: 3 in EBX/EBP/EDI, 7 spilled, each `add 0x1b8` per inner step) and keeps
   `dir` on the stack; our cl derives all fields from one IV, so the frame (0x24 vs 0x44), register
   assignment and the whole loop tail differ. Tried: grid[5-i][5-j], separate up-counters grid[a][b],
   flat index, a Cell pointer per iteration, permuter (one statement move, kept). Source form that gives
   one IV per field not found. */
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
typedef struct Cell {
  void *items[100];            /* 0x000 */
  int count;                   /* 0x190 */
  V3 sup;                      /* 0x194 */
  V3 inf;                      /* 0x1a0 */
  int key;                     /* 0x1ac */
  int mixed;                   /* 0x1b0 */
  int full;                    /* 0x1b4 */
} Cell;                        /* 0x1b8 */
typedef Cell CellRow[11];
MGLOBAL(V3 *, DAT_006dccb0, 0x006dccb0);
MGLOBALA(CellRow, DAT_006cfcb8, 11, 0x006cfcb8);
MGLOBAL(int, DAT_006e70d4, 0x006e70d4);
MGLOBAL(float, DAT_005cc350, 0x005cc350);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cf298, 0x005cf298);
MGLOBAL(float, DAT_005cf294, 0x005cf294);
int FUN_00467210(int a);
V3 *FUN_00443090(void);
float FUN_004c39b0(V3 *out, V3 *in);

// MATCH: 0x004847d0 FUN_004847d0
void FUN_004847d0(void)
{
  V3 *dir;
  V3 *org;
  int i, j, a, b;
  float x, px, pz;
  V3 v;
  V3 p;

  dir = (V3 *)(FUN_00467210(0) + 0x20);
  org = FUN_00443090();
  DAT_006dccb0 = org;
  for (i = 5, a = 0; i > -6; i--, a++) {
    x = (float)i * DAT_005cc350;
    for (j = 5, b = 0; j > -6; j--, b++) {
      p.z = (float)j * DAT_005cc350;
      v.x = dir->x * 18.0f + p.x;
      p.x = x;
      v.y = dir->y * 18.0f;
      v.z = dir->z * 18.0f + p.z;
      if (v.x != DAT_005d757c || v.y != DAT_005d757c || v.z != DAT_005d757c) {
        FUN_004c39b0(&v, &v);
      }
      DAT_006cfcb8[a][b].full = 0;
      if (v.z * dir->z + v.x * dir->x + v.y * dir->y < DAT_005cf298) {
        DAT_006cfcb8[a][b].full = 1;
      } else {
        px = p.x + DAT_006dccb0->x;
        pz = p.z + DAT_006dccb0->z;
        DAT_006cfcb8[a][b].inf.x = px - DAT_005cf294;
        DAT_006cfcb8[a][b].inf.y = DAT_006dccb0->y;
        DAT_006cfcb8[a][b].inf.z = pz - DAT_005cf294;
        DAT_006cfcb8[a][b].sup.x = DAT_005cf294 + px;
        DAT_006cfcb8[a][b].sup.y = DAT_006dccb0->y;
        DAT_006cfcb8[a][b].sup.z = DAT_005cf294 + pz;
        DAT_006cfcb8[a][b].count = 0;
        DAT_006cfcb8[a][b].key = 0;
        DAT_006cfcb8[a][b].mixed = 0;
      }
    }
  }
  DAT_006e70d4 = 1;
}
