// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047f6d0 (h7): 97.9% (regs 97.9%, ops 100%). Raw 43.6%. RpCollisionTriangle-style callback
   (5 stack args, returns arg 3); 12-byte struct copies for the three vertices and the normal; range test
   `== 1 || (> 4 && <= 6)`. Residual (b): the bit-index temp is spilled into arg 5's home slot ([esp+0x28]),
   the original uses arg 3's ([esp+0x20]). Tried: reusing param_1/param_4 as the temp (moves it to their slots),
   local copies of arg 3 / arg 5, signed temp, float arg 4, permuter (175 compiles, no change). */
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
typedef struct { float x, y, z; } V3;
typedef struct { V3 p; int pad0c[3]; unsigned int idx; V3 *a; V3 *b; V3 *c; } Tri;
typedef struct { V3 a; int pad0c; V3 b; int pad1c; V3 c; int pad2c; V3 p; int pad3c; int geo; unsigned int idx; unsigned int mat; } Out4c;
typedef struct { Out4c *base; int count; int pad08[2]; int **tab; } OutList;
MGLOBAL(int, DAT_00773920, 0x00773920);
int **FUN_00561ea0(int p0);
// MATCH: 0x0047f6d0 FUN_0047f6d0
Tri *FUN_0047f6d0(int param_1, int param_2, Tri *param_3, int param_4, OutList *param_5)
{
  Out4c *o;
  unsigned int idx;
  int **t;
  int i;

  o = &param_5->base[param_5->count];
  param_5->count = param_5->count + 1;
  o->a = *param_3->a;
  o->b = *param_3->b;
  o->c = *param_3->c;
  o->p = param_3->p;
  o->geo = param_2;
  o->idx = param_3->idx;
  if ((DAT_00773920 == 1) || ((DAT_00773920 > 4) && (DAT_00773920 <= 6))) {
    idx = param_3->idx;
    t = FUN_00561ea0(param_2);
    param_5->tab = t;
    o->mat = ((unsigned int)(*t)[idx >> 3] >> ((idx & 7) << 2)) & 0xf;
  }
  if ((param_2 != 0) && ((i = param_3->idx) >= 0) && (i < (int)*(unsigned short *)(param_2 + 0x84))) {
    o->idx = o->idx | ((unsigned int)*(unsigned short *)(*(int *)(param_2 + 4) + 6 + i * 8) +
                        (unsigned int)*(unsigned short *)(param_2 + 0x80)) << 16;
  }
  return param_3;
}
