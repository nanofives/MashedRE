// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041f120 by hand (h9): best 89.1% (regs 89.1%, ops 95.7%), 46/46 ins. Raw g2s 45.7%. Moved it: per-player record PSlot at DAT_0063d9e0 (0x2ac bytes; pos V3 +0x258, car +0x29c; Ghidra's DAT_0063dc38/3c/40/7c are record offsets) via a local PSlot*, and the parenthesised (y*y + x*x) + z*z (cl keeps the parenthesised association; without parens it emits z*z first). Residual: one scheduling slot, the original loads s->pos.x before mov ecx,[ecx+4] and does add ecx,0x40 before fsub [ecx]; ours folds fsub [ecx+0x40] first. Tried: local frame/V3 pointers, RwV3dSub macro (expression repeated), static __inline V3Sub/CarPos helpers, a 3-step loop, early return, a car local, permuter (6 rounds, no gain). Class (b). */
/* ---- MATCH-S04 preamble (same text in every re/match/s04 source; not part of any function body) ----
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
typedef struct { V3 right; int f; V3 up; int p1; V3 at; int p2; V3 pos; int p3; } Mtx;
typedef struct { char pad[0x10]; Mtx m; } Frm;
typedef struct {
  int *ev[0x96];      /* +0x000 */
  V3 pos;             /* +0x258 */
  int u264[2];        /* +0x264 */
  char *obj;          /* +0x26c */
  int u270[0xb];      /* +0x270 */
  char *car;          /* +0x29c */
  int u2a0[3];        /* +0x2a0 */
} PSlot;
MGLOBALA(PSlot, DAT_0063d9e0, 4, 0x0063d9e0);
float FUN_004c3ac0(V3 *v);
// MATCH: 0x0041f120 FUN_0041f120
int FUN_0041f120(int param_1, float *param_2, float *param_3)
{
  PSlot *s = &DAT_0063d9e0[param_1];
  V3 d;
  V3 *p;
  if (s->car != 0) {
    p = &(*(Frm **)(s->car + 4))->m.pos;
    d.x = s->pos.x - p->x;
    d.y = s->pos.y - p->y;
    d.z = s->pos.z - p->z;
    if (param_3 != 0)
      *param_3 = (d.y * d.y + d.x * d.x) + d.z * d.z;
    if (param_2 != 0)
      *param_2 = FUN_004c3ac0(&d);
    return 1;
  }
  return 0;
}
