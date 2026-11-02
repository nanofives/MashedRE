// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042f020 (h7): 100% (byte-identical incl. the jump table past Ghidra size 99). Raw g2s 58.5% (in_EAX).
   Register args (pilot finding 1): `static` callee, row in EAX and id in ECX from the (row, id) parameter order.
   No caller is in this slice, so the file holds a small stand-in caller h7_standin_0042f020 (not a game
   function) to make the compiler pick the register convention: the target bytes are identical, the file is not
   the real TU. Source: switch on id, 2/3/4/5/6/10 clear byte 0/1/2/5/3/11 of the 12-byte row at 0x007f0e50.
   Residual: none. */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
#ifdef __cplusplus
#define MATCH_STATIC
#else
#define MATCH_STATIC static
#endif
MGLOBALA(char, DAT_007f0e50, 0x9c, 0x007f0e50);
// MATCH: 0x0042f020 FUN_0042f020
MATCH_STATIC int FUN_0042f020(int row, int id)
{
  switch (id) {
  case 2:
    DAT_007f0e50[row * 0xc] = 0;
    return 0;
  case 3:
    DAT_007f0e50[row * 0xc + 1] = 0;
    return 0;
  case 4:
    DAT_007f0e50[row * 0xc + 2] = 0;
    return 0;
  case 5:
    DAT_007f0e50[row * 0xc + 5] = 0;
    return 0;
  case 6:
    DAT_007f0e50[row * 0xc + 3] = 0;
    return 0;
  case 10:
    DAT_007f0e50[row * 0xc + 0xb] = 0;
    return 0;
  }
  return 0;
}

#ifndef __cplusplus
/* stand-in caller (not a game function): makes the compiler give the static callee its register convention */
int h7_standin_0042f020(int row, int id)
{
  return FUN_0042f020(row, id) + FUN_0042f020(id, row);
}
#endif
