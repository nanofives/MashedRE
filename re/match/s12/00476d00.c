// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 h8 0x00476d00: 100% (regs 100%), 75/75 instructions. Raw converter 44.2%.
   Fixes: the pool record (+0x08 capacity, +0x0c count, +0x10..+0x28 seven array pointers) as a struct and each array
   element as a struct of its dword size (12/8/4/64/4-float/16/32 B), so every copy is one struct assignment from the
   template globals (64 and 32 B copies give the rep movsd, the others mov pairs; 0x006924d8 is float: fld/fstp).
   That gave 98.7%: in the +0x14 block the template dword loaded before the count. The last swap was pilot finding 2:
   DAT_006924dc is a file-static global (static under C, MGLOBAL under C++). Making any of the other six template
   globals static changes nothing (tested one by one), so only DAT_006924dc carries the evidence. */
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
typedef struct { undefined4 a[3]; } V12;
typedef struct { undefined4 a[2]; } V8;
typedef struct { undefined4 a[4]; } V16;
typedef struct { undefined4 a[8]; } V32;
typedef struct { undefined4 a[16]; } V64;
typedef struct {
  int f0;
  int f4;
  int cap;
  int count;
  V12 *p10;
  V8 *p14;
  undefined4 *p18;
  V64 *p1c;
  float *p20;
  V16 *p24;
  V32 *p28;
} Pool;
MGLOBAL(float, DAT_006924d8, 0x006924d8);
#ifndef __cplusplus
static V8 DAT_006924dc;
#else
MGLOBAL(V8, DAT_006924dc, 0x006924dc);
#endif
MGLOBAL(V64, DAT_006924e8, 0x006924e8);
MGLOBAL(V12, DAT_00692528, 0x00692528);
MGLOBAL(V32, DAT_00692534, 0x00692534);
MGLOBAL(undefined4, DAT_00692554, 0x00692554);
MGLOBAL(V16, DAT_00692598, 0x00692598);
// MATCH: 0x00476d00 FUN_00476d00
void FUN_00476d00(Pool *param_1)
{
  if (param_1->count < param_1->cap) {
    if (param_1->p10 != 0) param_1->p10[param_1->count] = DAT_00692528;
    if (param_1->p14 != 0) param_1->p14[param_1->count] = DAT_006924dc;
    if (param_1->p18 != 0) param_1->p18[param_1->count] = DAT_00692554;
    if (param_1->p1c != 0) param_1->p1c[param_1->count] = DAT_006924e8;
    if (param_1->p20 != 0) param_1->p20[param_1->count] = DAT_006924d8;
    if (param_1->p24 != 0) param_1->p24[param_1->count] = DAT_00692598;
    if (param_1->p28 != 0) param_1->p28[param_1->count] = DAT_00692534;
    param_1->count++;
  }
}
