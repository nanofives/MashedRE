// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x004625b0 (h6): 100% (whole obj symbol byte-identical, 420 B; Ghidra's 386 B stops before the
   loop tail, so the per-instruction score shows 95.0% for the 7 tail instructions past its end).
   Raw did not compile. Written as FUN_004a2b60(buf, "") then strcat intrinsics on two [6][0x80] char
   tables, switch cases in source order 0,1,4,2,3,default (the original's layout order).
   Residual: none. */
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
char *strcat(char *, const char *);
#pragma intrinsic(strcat)
MGLOBALA(char, DAT_00690158, 0x300, 0x00690158);
MGLOBALA(char, DAT_006041f0, 0x300, 0x006041f0);
int FUN_004a2b60(char *p0, const char *p1);
// MATCH: 0x004625b0 FUN_004625b0
void FUN_004625b0(int param_1)
{
  int i;

  for (i = 0; i < 0x300; i += 0x80) {
    FUN_004a2b60(&DAT_00690158[i], "");
    strcat(&DAT_00690158[i], "toastaudio\\pc\\audio\\pcdics\\");
    switch (param_1) {
    case 0:
      strcat(&DAT_00690158[i], "english\\");
      break;
    case 1:
      strcat(&DAT_00690158[i], "french\\");
      break;
    case 4:
      strcat(&DAT_00690158[i], "italian\\");
      break;
    case 2:
      strcat(&DAT_00690158[i], "german\\");
      break;
    case 3:
      strcat(&DAT_00690158[i], "spanish\\");
      break;
    default:
      strcat(&DAT_00690158[i], "english\\");
      break;
    }
    strcat(&DAT_00690158[i], &DAT_006041f0[i]);
    strcat(&DAT_00690158[i], ".rws");
  }
}
