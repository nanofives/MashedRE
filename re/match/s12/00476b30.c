// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00476b30 (h2): 100%, raw 86.7%. Typed chain: RW plugin data at (obj + *0x007dc57c), +0xc4
   set from param (default 0x00613290), mirrored into (**obj->+0x18->+0x20)->+4 when non-null, flags |= 0x2000.
   Residual: none. */
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
typedef struct { unsigned char pad[0x40]; unsigned int flags; unsigned char pad2[0x80]; unsigned int c4; } PlugD;
typedef struct { int pad; int f4; } Mat;
typedef struct { unsigned char pad[0x20]; Mat **pm; } Sub18;
typedef struct { unsigned char pad[0x18]; Sub18 *s; } Obj;
typedef struct { int pad; Obj *obj; } Holder;
MGLOBAL(int, DAT_007dc57c, 0x007dc57c);
MGLOBAL(unsigned int, DAT_00613290, 0x00613290);
// MATCH: 0x00476b30 FUN_00476b30
void FUN_00476b30(Holder *param_1, unsigned int *param_2)
{
  Mat *m;

  if (param_2 == 0) {
    param_2 = &DAT_00613290;
  }
  (*(PlugD **)((char *)param_1->obj + DAT_007dc57c))->c4 = *param_2;
  m = *param_1->obj->s->pm;
  if (m != 0) {
    m->f4 = (*(PlugD **)((char *)param_1->obj + DAT_007dc57c))->c4;
  }
  (*(PlugD **)((char *)param_1->obj + DAT_007dc57c))->flags |= 0x2000;
}
