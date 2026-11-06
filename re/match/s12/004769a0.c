// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x004769a0 (h2): 100%, raw 81.8%. The three destination globals 0x00692528/2c/30 are file-static
   (pilot finding 2): only then does cl load all three source words before the stores. static under C, MGLOBAL
   under C++. Residual: none. */
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
#ifndef __cplusplus
static unsigned int DAT_00692528;
static unsigned int DAT_0069252c;
static unsigned int DAT_00692530;
#else
MGLOBAL(unsigned int, DAT_00692528, 0x00692528);
MGLOBAL(unsigned int, DAT_0069252c, 0x0069252c);
MGLOBAL(unsigned int, DAT_00692530, 0x00692530);
#endif
MGLOBALA(unsigned int, DAT_006925a8, 3, 0x006925a8);
// MATCH: 0x004769a0 FUN_004769a0
void FUN_004769a0(unsigned int *param_1)
{
  if (param_1 == 0) {
    param_1 = DAT_006925a8;
  }
  DAT_00692528 = param_1[0];
  DAT_0069252c = param_1[1];
  DAT_00692530 = param_1[2];
}
