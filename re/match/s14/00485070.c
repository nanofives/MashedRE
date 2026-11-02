// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x00485070 (h2): world-object pair pass dispatch. 100%. Raw 90.9%: the branch order was
   inverted; `if (DAT_006e70c8 != 0) FUN_00484de0(); else FUN_00484f70();` matches. */
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
typedef unsigned int undefined4;
MGLOBAL(int, DAT_006e70c8, 0x006e70c8);
MGLOBAL(undefined4, DAT_006e70cc, 0x006e70cc);
MGLOBAL(undefined4, DAT_006e70d8, 0x006e70d8);
MGLOBAL(undefined4, DAT_006e70d4, 0x006e70d4);
void FUN_00484de0(void);
void FUN_00484f70(void);
// MATCH: 0x00485070 FUN_00485070
void FUN_00485070(void)

{
  if (DAT_006e70c8 != 0) {
    FUN_00484de0();
  }
  else {
    FUN_00484f70();
  }
  DAT_006e70d8 = DAT_006e70cc;
  DAT_006e70cc = 0;
  DAT_006e70d4 = 0;
  return;
}
