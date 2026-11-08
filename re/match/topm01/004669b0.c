// FLAGS: /O2 /GS /Ob1
/* TOPM01 0x004669b0: 100.0% (32/32 ins), --bytes BYTE-IDENTICAL (160 B, relocations masked), relocheck OK.
   Started from re/match/s10/004669b0.c (84.4%). What closed it:
   (1) return 1: the listing leaves eax = 1 at the ret 0x00466a4f (mov eax,1 at 0x00466a0f, stored to
       0x006904e8 at 0x00466a2e). An int return of 1 makes the compiler share the constant in eax. The one
       caller, 0x004028db, does not read eax after the call (next instruction 0x004028e0 is another call).
   (2) 0x00690490..0x00690498 as float[3] stored with 0.0f: the binary reads them with fsub/fadd dword at
       0x0045d85c/0x0045d87b/0x0045d896 and 0x0045d8d1/0x0045d8f1/0x0045d911. Float 0.0f stores take the
       known-zero ecx and need no second memset (s10's residual edx).
   (3) 0x008aa300 as int[4] with one 16-byte memset: 0x008aa30c is the [3] slot (0x008aa300 is indexed
       by esi*4 at 0x00463883/0x004638e7/0x00463931/0x004639db/0x00463ae5).
   (4) DAT_006900d0 = 0 and DAT_0068fccc = 1000 written before the memset: a zero store placed before the
       memset stays an immediate (0x00466a14), one after it is turned into the ecx store.
   Tried on the way: types of DAT_006900d0 (int/uint/long/float/pointer), chained and plain int stores
   (zero hoisted into esi, 58-66%), loops, static __inline helpers, return through a local or the global,
   flags /G3../G7 /GB /Ob2 /Ox, a 120-variant order search with a 12-byte memset (best 96.9%). */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
MGLOBAL(int, DAT_007f0f60, 0x007f0f60);
MGLOBALA(int, DAT_008aa300, 4, 0x008aa300);
MGLOBAL(int, DAT_006900d0, 0x006900d0);
MGLOBAL(int, DAT_0068fccc, 0x0068fccc);
MGLOBAL(int, DAT_006904e8, 0x006904e8);
MGLOBAL(float, DAT_0068fcbc, 0x0068fcbc);
MGLOBALA(float, DAT_00690490, 3, 0x00690490);
MGLOBALA(char, s_soundstuff_006036e8, 128, 0x006036e8);
MGLOBALA(char, s_more_soundstuff_00603768, 128, 0x00603768);
MGLOBALA(char, s_even_more_soundstuff_006037e8, 128, 0x006037e8);
#ifdef __cplusplus
extern "C"
#endif
void *memset(void *p0, int p1, unsigned int p2);
#pragma intrinsic(memset)
void FUN_00429290(char *p0, char *p1, int p2);
void FUN_0045d460(void);
void FUN_004625b0(int p0);
void FUN_004627f0(void);
// MATCH: 0x004669b0 FUN_004669b0
int FUN_004669b0(void)
{
  FUN_004625b0(DAT_007f0f60);
  FUN_00429290(s_soundstuff_006036e8,s_soundstuff_006036e8,0);
  FUN_004627f0();
  FUN_00429290(s_more_soundstuff_00603768,s_more_soundstuff_00603768,0);
  FUN_00429290(s_even_more_soundstuff_006037e8,s_even_more_soundstuff_006037e8,0);
  FUN_0045d460();
  DAT_006900d0 = 0;
  DAT_0068fccc = 1000;
  memset(DAT_008aa300, 0, 16);
  DAT_006904e8 = 1;
  DAT_0068fcbc = 1.0f;
  DAT_00690490[0] = 0.0f;
  DAT_00690490[1] = 0.0f;
  DAT_00690490[2] = 0.0f;
  return 1;
}
