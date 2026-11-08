// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00549970 h1: best 53.6% (regs 78.6%, ops 92.9%). Residue: ours keeps zero in esi (CSE with the final g750=0 store) where orig does xor eax,eax + immediate final store. Tried chained assign, else-form, ||-form. Class (b). */
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
MGLOBAL(int, DAT_007dc73c, 0x007dc73c);
MGLOBAL(int, DAT_007dc740, 0x007dc740);
MGLOBAL(int, DAT_007dc744, 0x007dc744);
MGLOBAL(int, DAT_007dc748, 0x007dc748);
MGLOBAL(int, DAT_007dc74c, 0x007dc74c);
MGLOBAL(int, DAT_007dc750, 0x007dc750);
unsigned char * FUN_004cbb60(void);
int FUN_00549a50(void);
int FUN_005c9d00(void);
// MATCH: 0x00549970 FUN_00549970
int FUN_00549970(void)
{
  unsigned char *p;
  int f;
  DAT_007dc73c = 0;
  DAT_007dc740 = 0;
  DAT_007dc744 = 0;
  DAT_007dc748 = 0;
  DAT_007dc73c = FUN_00549a50();
  DAT_007dc740 = FUN_005c9d00();
  DAT_007dc744 = FUN_005c9d00();
  DAT_007dc748 = FUN_005c9d00();
  p = FUN_004cbb60();
  f = p[0x1e] & 1;
  DAT_007dc74c = f;
  if (f) {
    DAT_007dc750 = 1;
    if ((*(unsigned int *)(p + 0xc4) & 0xffff) >= 0x101) return 1;
  }
  DAT_007dc750 = 0;
  return 1;
}
