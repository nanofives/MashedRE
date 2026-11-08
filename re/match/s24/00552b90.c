// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 h3 0x00552b90: 100% (regs 100%). Indexed for loop over DAT_00912a84[32]; the raw converter's pointer-compare loop scored 96.9%. */
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
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(int, DAT_00912a84, 32, 0x00912a84);
MGLOBAL(int, DAT_00912b04, 0x00912b04);
MGLOBAL(int, DAT_00913204, 0x00913204);
MGLOBAL(int, DAT_00913208, 0x00913208);
typedef void (__cdecl *fn10c_t)(int);
int __cdecl FUN_004c5770(int);
int __cdecl FUN_00552a60(int);
void __cdecl FUN_00554050(void);
void __cdecl FUN_00555280(void);
void __cdecl FUN_00556ce0(void);
void __cdecl FUN_005571c0(void);
void __cdecl FUN_00557220(void);
// MATCH: 0x00552b90 FUN_00552b90
void FUN_00552b90(void)
{
  int i;
  for (i = 0; i < 32; i++) {
    if (DAT_00912a84[i] != 0) {
      FUN_004c5770(DAT_00912a84[i]);
      DAT_00912a84[i] = 0;
    }
  }
  DAT_00912b04 = 0;
  if (DAT_00913204 != 0) {
    (*(fn10c_t *)(DAT_007d3ff8 + 0x10c))(DAT_00913204);
    DAT_00913204 = 0;
    DAT_00913208 = 0;
  }
  FUN_005571c0();
  FUN_00555280();
  FUN_00556ce0();
  FUN_00554050();
  FUN_00557220();
  FUN_00552a60(0);
}
