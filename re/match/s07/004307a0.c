// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x004307a0: 97.1% (regs 97.1%) by hand (raw 28.3%). 0x005cc728 is the literal 60.0f (written as a
   literal: a global operand made the compiler swap the fmul operands). Source order .b + .c gives the
   original's fadd order c then b (relocheck OK). Residual (a): the original schedules fcomp [limit] before
   pop esi, ours after it; same slot, same compare. Tried: if/else, return of the compare, ?: , > with swapped
   returns, limit >= total, a float or int temp for the seconds term, a70 returning double / long double (worse),
   permuter (85 compiles, no change). */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
typedef struct { float a; float b; float c; } Lap004307a0;
MGLOBAL(int, DAT_0067f17c, 0x0067f17c);
MGLOBALA(Lap004307a0, DAT_00614718, 1, 0x00614718);
float FUN_00429a70(int p0);
int FUN_00429a80(int p0);
int FUN_00429a90(int p0);
// MATCH: 0x004307a0 FUN_004307a0
int FUN_004307a0(void)
{
  float limit;
  limit = DAT_00614718[DAT_0067f17c].a * 60.0f + DAT_00614718[DAT_0067f17c].b + DAT_00614718[DAT_0067f17c].c;
  if (FUN_00429a70(0) + (float)(FUN_00429a80(0) * 60 + FUN_00429a90(0)) <= limit) {
    return 1;
  }
  return 0;
}
