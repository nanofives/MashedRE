// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d8680: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs21_20261107/results.tsv. */
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
// MATCH: 0x004d8680 FUN_004d8680
int FUN_004d8680(char *param_1,char *param_2)

{
  char cVar1;
  char cVar2;
  
  if ((param_1 != (char *)0x0) && (param_2 != (char *)0x0)) {
    do {
      cVar1 = *param_1;
      cVar2 = *param_2;
      if (('@' < cVar1) && (cVar1 < '[')) {
        cVar1 = cVar1 + ' ';
      }
      if (('@' < cVar2) && (cVar2 < '[')) {
        cVar2 = cVar2 + ' ';
      }
      if (cVar1 != cVar2) {
        return (int)cVar1 - (int)cVar2;
      }
      param_1 = param_1 + 1;
      param_2 = param_2 + 1;
    } while ((cVar1 != '\0') && (cVar2 != '\0'));
  }
  return 0;
}
