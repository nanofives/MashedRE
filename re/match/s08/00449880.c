// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00449880: 100% (regs 100%) by hand (h9). Raw 66.7%. A float local (global + 1.0f literal, .rdata
   0x005cc320) stored into a 256-float array by an int loop: the compiler emits rep stosd through the spilled
   float. */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
MGLOBAL(float, DAT_005f96d0, 0x005f96d0);
MGLOBALA(float, DAT_00683ec8, 0x100, 0x00683ec8);
// MATCH: 0x00449880 FUN_00449880
void FUN_00449880(void)
{
  float v;
  int i;

  v = DAT_005f96d0 + 1.0f;
  for (i = 0; i < 0x100; i++) {
    DAT_00683ec8[i] = v;
  }
}
