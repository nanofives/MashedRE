// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d7ac0 (h1, by hand): 100.0% exact, regs 100.0% (raw 36.8%). Toggle of cached render states 7 and
   0x17 through the deferred render-state set (value into {value,changed} 0x007d57f8[st], st appended to 0x007d5168
   when not yet pending), written as a local macro (#undef after the function). Returns 1 on every path (eax=1 in
   the original). relocheck OK, C++ ok. */
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
typedef struct { int value; int changed; } FUN_004d7ac0_cache;
MGLOBAL(int, DAT_007d6aec, 0x007d6aec);
MGLOBAL(int, DAT_007d6ae8, 0x007d6ae8);
MGLOBAL(uint, DAT_007d6c14, 0x007d6c14);
MGLOBALA(int, DAT_007d5168, 210, 0x007d5168);
MGLOBALA(FUN_004d7ac0_cache, DAT_007d57f8, 210, 0x007d57f8);
#define FUN_004d7ac0_RS(st, v)                     \
  do {                                             \
    DAT_007d57f8[st].value = (v);                  \
    if (!DAT_007d57f8[st].changed) {               \
      DAT_007d57f8[st].changed = 1;                \
      DAT_007d5168[DAT_007d6c14++] = (st);         \
    }                                              \
  } while (0)
// MATCH: 0x004d7ac0 FUN_004d7ac0
int FUN_004d7ac0(int param_1)

{
  if (param_1 != 0) {
    if (DAT_007d6aec == 0) {
      if (DAT_007d6ae8 == 0) {
        FUN_004d7ac0_RS(7, 1);
      }
      FUN_004d7ac0_RS(0x17, 4);
      DAT_007d6aec = 1;
    }
  }
  else {
    if (DAT_007d6aec != 0) {
      if (DAT_007d6ae8 == 0) {
        FUN_004d7ac0_RS(7, 0);
      }
      FUN_004d7ac0_RS(0x17, 8);
      DAT_007d6aec = 0;
    }
  }
  return 1;
}
#undef FUN_004d7ac0_RS
