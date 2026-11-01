// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 h11 0x0042ad90 (54 B): 100.0% exact (regs 100, ops 100), byte-identical.
   Raw converter 33.3% (unaff_EBX/EDI). Static function: list in EDX, key in EBX, nth in EDI, the convention cl
   gives it here (pilot finding 1); caller_0042ad90 is a stand-in for the real callers at 0x0043d4b5.., which
   pass EBX = 0xff000000, EDI = 0 (not matched). A while loop over a moving pointer strength-reduced and spilled
   key to the stack (33%); an indexed for loop over list[i] matched. Residual: none. */
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
// MATCH: 0x0042ad90 FUN_0042ad90
static int FUN_0042ad90(int *list, int key, int nth)
{
  int cnt;
  int i;
  if (list == 0)
    return -1;
  cnt = 0;
  for (i = 0; list[i] != (int)0xff070000; i++) { if (list[i] == key) { if (nth == cnt) return list[i + 1]; cnt++; } }
  return -1;
}

/* stand-in caller (not matched): the real callers sit at 0x0043d4b5 and later (EBX = 0xff000000, EDI = 0) */
int caller_0042ad90(int *list, int *list2)
{
  int a = FUN_0042ad90(list, 0xff000000, 0);
  return a + FUN_0042ad90(list2, 0xff000000, 0);
}
