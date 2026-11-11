// FLAGS: /O2 /GS /Ob1
/* TL-MATCH-REST 0x00409680 (20 B): fills the 0x480 dwords at 0x0063a5f0 with 0xbf800000 (-1.0f). */
/* ---- MATCH-TLR preamble (same text in every re/match/tlr source; not part of any function body) ----
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
MGLOBALA(unsigned int, DAT_0063a5f0, 0x480, 0x0063a5f0);
// MATCH: 0x00409680 FUN_00409680
void FUN_00409680(void)
{
  int i;
  for (i = 0; i < 0x480; i++) {
    DAT_0063a5f0[i] = 0xbf800000;
  }
}
