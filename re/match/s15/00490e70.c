// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x00490e70 (h5, by hand): 100% (regs 100%), 215 B, byte-identical. The zero stores to 0x00771538/0x0077153c/0x006146e0 are float 0.0f: as int 0 the compiler shares xor eax,eax across them and the byte 0 store. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
MGLOBAL(unsigned char, DAT_00616030, 0x00616030);
MGLOBAL(unsigned char, DAT_00616031, 0x00616031);
MGLOBAL(unsigned char, DAT_00616032, 0x00616032);
MGLOBAL(unsigned char, DAT_00616033, 0x00616033);
MGLOBAL(unsigned char, DAT_00616034, 0x00616034);
MGLOBAL(unsigned char, DAT_00616035, 0x00616035);
MGLOBAL(unsigned char, DAT_00616036, 0x00616036);
MGLOBAL(unsigned char, DAT_00616037, 0x00616037);
MGLOBAL(float, DAT_006146c0, 0x006146c0);
MGLOBAL(float, DAT_006146c4, 0x006146c4);
MGLOBAL(float, DAT_00771538, 0x00771538);
MGLOBAL(float, DAT_0077153c, 0x0077153c);
MGLOBAL(float, DAT_006146c8, 0x006146c8);
MGLOBAL(float, DAT_006146cc, 0x006146cc);
MGLOBAL(float, DAT_006146d0, 0x006146d0);
MGLOBAL(float, DAT_006146d4, 0x006146d4);
MGLOBAL(float, DAT_006146d8, 0x006146d8);
MGLOBAL(float, DAT_006146dc, 0x006146dc);
MGLOBAL(float, DAT_006146e0, 0x006146e0);
MGLOBAL(float, DAT_006146e4, 0x006146e4);
MGLOBAL(float, DAT_006146b0, 0x006146b0);
MGLOBAL(float, DAT_006146b4, 0x006146b4);
MGLOBAL(float, DAT_006146b8, 0x006146b8);
MGLOBAL(float, DAT_006146bc, 0x006146bc);
// MATCH: 0x00490e70 FUN_00490e70
void FUN_00490e70(void)
{
  DAT_00616030 = 0x78;
  DAT_00616031 = 0x80;
  DAT_00616032 = 0x88;
  DAT_00616033 = 0x40;
  DAT_00616034 = 0x78;
  DAT_00616035 = 0x80;
  DAT_00616036 = 0x88;
  DAT_00616037 = 0;
  DAT_006146c0 = -4.0f;
  DAT_006146c4 = 4.0f;
  DAT_00771538 = 0.0f;
  DAT_0077153c = 0.0f;
  DAT_006146c8 = -4.0f;
  DAT_006146cc = 4.0f;
  DAT_006146d0 = 0.5f;
  DAT_006146d4 = 1.1f;
  DAT_006146d8 = 0.7f;
  DAT_006146dc = 1.0f;
  DAT_006146e0 = 0.0f;
  DAT_006146e4 = 0.1f;
  DAT_006146b4 = 0.25f;
  DAT_006146b0 = 0.25f;
  DAT_006146b8 = 3.0f;
  DAT_006146bc = 6.0f;
}
