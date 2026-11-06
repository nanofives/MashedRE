// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00452170 (h1): 100% (regs 100%). Raw converter 39.4%: the two copies are 64-byte struct assignments between MGLOBAL matrices, strings as MGLOBALA, the printf at 0x004a2cbd called by its FUN_ name. Residual: none. */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef unsigned int undefined4;
typedef struct { float m[16]; } M16_00452170;
MGLOBAL(int, DAT_005f978c, 0x005f978c);
MGLOBAL(int, DAT_005f9790, 0x005f9790);
MGLOBAL(M16_00452170, DAT_00684c58, 0x00684c58);
MGLOBAL(M16_00452170, DAT_00684c98, 0x00684c98);
MGLOBAL(M16_00452170, DAT_00684cd8, 0x00684cd8);
MGLOBAL(M16_00452170, DAT_00684de8, 0x00684de8);
MGLOBALA(char, s_we_have_reset_the_pontoon_005ce3a0, 27, 0x005ce3a0);
MGLOBALA(char, s_Reset_with_matrix_005ce38c, 18, 0x005ce38c);
int FUN_004a2cbd(char *p0, ...);
void FUN_004518f0(char *p0, M16_00452170 *p1);
void FUN_0047d180(int p0, M16_00452170 *p1);
// MATCH: 0x00452170 FUN_00452170
undefined4 FUN_00452170(undefined4 param_1)
{
  DAT_00684cd8 = DAT_00684de8;
  DAT_00684c98 = DAT_00684c58;
  FUN_0047d180(DAT_005f978c,&DAT_00684de8);
  FUN_004a2cbd(s_we_have_reset_the_pontoon_005ce3a0);
  FUN_004518f0(s_Reset_with_matrix_005ce38c,&DAT_00684de8);
  FUN_0047d180(DAT_005f9790,&DAT_00684c58);
  return param_1;
}
