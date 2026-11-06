// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00476c60 (h2): 100%, raw 28.6%. 32-byte struct copy into plugin data +0xd8 (default
   0x006132a4), flags |= 0x100000. Residual: none. */
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
typedef struct { unsigned int v[8]; } SV;
typedef struct { unsigned char pad[0x40]; unsigned int flags; unsigned char pad2[0xd8 - 0x44]; SV m; } PlugD;
typedef struct { int pad; char *obj; } Holder;
MGLOBAL(int, DAT_007dc57c, 0x007dc57c);
MGLOBAL(SV, DAT_006132a4, 0x006132a4);
// MATCH: 0x00476c60 FUN_00476c60
void FUN_00476c60(Holder *param_1, SV *param_2)
{
  if (param_2 == 0) {
    param_2 = &DAT_006132a4;
  }
  (*(PlugD **)(param_1->obj + DAT_007dc57c))->m = *param_2;
  (*(PlugD **)(param_1->obj + DAT_007dc57c))->flags |= 0x100000;
}
