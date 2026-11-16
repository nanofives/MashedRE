// FLAGS: /O2 /Ob1 /G5
/* PORT-TOP-AUDIOINIT-SPLIT-5A9970 0x005aa510: the callback 0x005aa480 passes to 0x005aa560 as param_5
   (re/match/s30/005aa480.c line 59). Written by hand from MASHED.exe.unpatched 0x005aa510..0x005aa515 (6 B):
   mov eax,0x7dccf8 / ret. 0x005aa560 calls it with four arguments (cdecl, caller pops) and reads EAX. */
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
MGLOBAL(undefined1, DAT_007dccf8, 0x007dccf8);
// MATCH: 0x005aa510 FUN_005aa510
int FUN_005aa510(void)
{
  return (int)&DAT_007dccf8;
}
