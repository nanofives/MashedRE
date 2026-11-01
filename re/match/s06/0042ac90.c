// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 h11 0x0042ac90 (127 B): 100.0% exact (regs 100, ops 100), byte-identical.
   Raw converter 25.0%. Skip loop as for (j = -1; j != n; j++) (gives the n != -1 guard and n + 1 down-count;
   j < n + 1 gave 51%). i = 0 written before the two table loads moved the register choice from 87.2% to 100%.
   Residual: none. */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
MGLOBAL(int, DAT_0067e9f8, 0x0067e9f8);
MGLOBAL(int *, DAT_0067ed38, 0x0067ed38);
MGLOBAL(int, DAT_0067ed40, 0x0067ed40);
// MATCH: 0x0042ac90 FUN_0042ac90
int FUN_0042ac90(void)
{
  int *list;
  int i;
  int j;
  int n;
  i = 0;
  list = (&DAT_0067ed38)[DAT_0067e9f8 * 0x10];
  n = (&DAT_0067ed40)[DAT_0067e9f8 * 0x10];
  for (j = -1; j != n; j++) { while (list[i] != (int)0xff040000) i++; i++; }
  while (list[i] != (int)0xff050000 && list[i] != (int)0xff140000 && list[i] != (int)0xff060000)
    i++;
  if (list[i] == (int)0xff060000)
    return -1;
  return list[i + 1];
}
