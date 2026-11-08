// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 h2 0x004b47e0 (159 B): by hand, 100%. Keyframe lookup: t <= DAT_005d757c -> first key, t >= end ->
   last key, else walk to the bracketing pair, interpolate (FUN_00532b80) into a local, then FUN_00532a60.
   Key: one walking pointer k (keys) plus a trailing cur/prev pair; the loop tests k->time (76.7% with a
   single cur++ pointer, which also tail-merges the first two calls). */
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
typedef struct { int f0; float time; int pad[7]; } Key_4b47e0;
typedef struct { int f0; int count; int f8; float end; Key_4b47e0 *keys; } Anim_4b47e0;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
void FUN_00532a60(void *p0, Key_4b47e0 *p1);
void FUN_00532b80(Key_4b47e0 *p0, Key_4b47e0 *p1, Key_4b47e0 *p2, float p3, int p4);
// MATCH: 0x004b47e0 FUN_004b47e0
void FUN_004b47e0(void *param_1, Anim_4b47e0 *param_2, float param_3)
{
  Key_4b47e0 *prev;
  Key_4b47e0 *cur;
  Key_4b47e0 *k;
  int n;
  Key_4b47e0 tmp;

  k = param_2->keys;
  n = param_2->count;
  if (param_3 <= DAT_005d757c) {
    FUN_00532a60(param_1, k);
    return;
  }
  if (param_3 >= param_2->end) {
    FUN_00532a60(param_1, &k[n - 1]);
    return;
  }
  cur = k;
  do {
    k++;
    prev = cur;
    cur = k;
  } while (param_3 > k->time);
  FUN_00532b80(&tmp, prev, k, param_3, 0);
  FUN_00532a60(param_1, &tmp);
}
