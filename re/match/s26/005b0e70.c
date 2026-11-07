// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 h1 0x005b0e70 (71 B). Best 100.0% (regs 100.0%), exact. switch(mode){1->3,2->6,3->4} then cnt++ and callback; dec-chain from switch. */
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
typedef struct CB { void (*fn)(int, int); int arg; } CB;
typedef struct St { char pad[0x20]; int mode; int cnt; int state; CB *cb; } St;
typedef struct Obj { char pad[0x20]; St *st; } Obj;
// MATCH: 0x005b0e70 FUN_005b0e70
void FUN_005b0e70(Obj *param_1)
{
  St *s = param_1->st;
  CB *cb;
  if (s->state != 1) {
    switch (s->mode) {
    case 1: s->state = 3; break;
    case 2: s->state = 6; break;
    case 3: s->state = 4; break;
    }
    s->cnt++;
  }
  cb = s->cb;
  if (cb != 0) {
    cb->fn((int)param_1, cb->arg);
  }
}
