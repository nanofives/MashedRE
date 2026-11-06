// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x004669b0: 84.4% (regs 90.6%), raw g2s 54.5%. Calls and string args match (strings as 128-byte
   MGLOBALA buffers, 0x80 apart). The zero stores at 0x008aa300 (3 ints) and 0x00690490 (3 ints) are inlined
   memsets (#pragma intrinsic): that is where the scratch-register zero after the calls comes from (plain int
   stores hoist the zero into esi across the calls and push esi for the call args: 54.5%; natural-order memset
   source 78.1%). Store order below is the permuter's (all independent global stores, same behaviour).
   Residual (b): the original uses one zero register (ecx) for both memsets and 0x008aa30c and holds 1 in eax for
   DAT_006904e8, with DAT_006900d0 = 0 as an immediate; ours gives the second memset its own edx and stores 1 as
   an immediate. Tried: int/float types per group, third-arg types and spellings of the call zero, chained
   assignments, the store or call block as static __inline helpers, file-static globals, a 16-byte memset
   over 0x008aa300..0x008aa30c, plain stores for either group, permuter (180 + 368 compiles). */
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
MGLOBALA(int, DAT_008aa300, 3, 0x008aa300);
MGLOBAL(int, DAT_006900d0, 0x006900d0);
MGLOBAL(int, DAT_0068fccc, 0x0068fccc);
MGLOBAL(int, DAT_008aa30c, 0x008aa30c);
MGLOBAL(int, DAT_006904e8, 0x006904e8);
MGLOBAL(float, DAT_0068fcbc, 0x0068fcbc);
MGLOBALA(int, DAT_00690490, 3, 0x00690490);
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
void FUN_004669b0(void)
{
  FUN_004625b0(DAT_007f0f60);
  FUN_00429290(s_soundstuff_006036e8,s_soundstuff_006036e8,0);
  FUN_004627f0();
  FUN_00429290(s_more_soundstuff_00603768,s_more_soundstuff_00603768,0);
  FUN_00429290(s_even_more_soundstuff_006037e8,s_even_more_soundstuff_006037e8,0);
  FUN_0045d460();
  DAT_0068fccc = 1000;
  memset(DAT_008aa300, 0, sizeof(DAT_008aa300));
  DAT_006904e8 = 1;
  DAT_0068fcbc = 1.0f;
  DAT_008aa30c = 0;
  DAT_006900d0 = 0;
  memset(DAT_00690490, 0, sizeof(DAT_00690490));
}
