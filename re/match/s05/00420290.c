// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00420290 by hand (h2): best 86.7% (regs 86.7%, ops 91.1%), 45/45 ins. Raw g2s 30.9%.
   Moved it: per-player record PSlot at 0x0063d9e0 (0x2ac bytes; +0x274 float, +0x294 flags), one float local f
   reused for the lerp start and the FUN_0041f120 distance out-param (cl puts it in the dead param_1 slot, as the
   original does), and float literals for the pooled constants (100.0 at 0x005cc568, 0.0 at 0x005d757c, 0.01 at
   0x005cc328, 1.0 at 0x005cc320): with literals cl folds the second compare away on the 100 path, as the original.
   Residuals: (1) the original places the f=100 block out of line after the ret (jnp forward + jmp back); ours keeps
   it inline after the 0-clamp (tried 13 clamp spellings: if/else, nested ternaries, !(100<=f), f<100, goto,
   MIN-then-MAX macro, extern consts); (2) fmul [param_2] is scheduled one push later in ours (tried 4 lerp
   spellings). Permuter (6 rounds) no gain. Class (b). */
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
typedef struct { float x, y, z; } V3;
typedef struct {
  int *ev[0x96];      /* +0x000 */
  V3 pos;             /* +0x258 */
  int u264[2];        /* +0x264 */
  char *obj;          /* +0x26c */
  int u270;           /* +0x270 */
  float f274;         /* +0x274 */
  int u278[7];        /* +0x278 */
  unsigned int flags; /* +0x294 */
  int u298;           /* +0x298 */
  char *car;          /* +0x29c */
  int u2a0[3];        /* +0x2a0 */
} PSlot;
MGLOBALA(PSlot, DAT_0063d9e0, 4, 0x0063d9e0);
int FUN_0041f120(int param_1, float *param_2, float *param_3);
float FUN_00426de0(void);
float FUN_00426df0(void);
// MATCH: 0x00420290 FUN_00420290
void FUN_00420290(int param_1, float param_2)
{
  PSlot *s = &DAT_0063d9e0[param_1];
  float f;
  f = FUN_00426de0();
  s->f274 = (FUN_00426df0() - f) * param_2 + f;
  f = 0.0f;
  FUN_0041f120(param_1, 0, &f);
  if (!(100.0f <= f)) { if (!(f >= 0.0f)) f = 0.0f; } else f = 100.0f;
  s->flags |= 2;
  s->f274 *= 1.0f - f * 0.01f;
}

