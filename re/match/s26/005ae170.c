// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 h1 0x005ae170 (81 B). Best 61.9% (regs 76.2%), residual class (b). ops 97.6%: case-fold with Ghidra's odd range tests (a>='a'||a>='A')&&(a<='z'||a<='Z'), 2nd operand (b>='a'||b<='A'); only reg alloc differs (orig a in al then mov cl,al); for/do/while + permuter 200s. */
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
// MATCH: 0x005ae170 FUN_005ae170
int FUN_005ae170(char *param_1,char *param_2,int param_3)
{
  char a, b;
  do {
    a = *param_1;
    if ((a >= 'a' || a >= 'A') && (a <= 'z' || a <= 'Z')) a &= 0xdf;
    b = *param_2;
    if ((b >= 'a' || b <= 'A') && (b <= 'z' || b <= 'Z')) b &= 0xdf;
    if (a != b) return a - b;
    param_1++; param_2++;
  } while (--param_3);
  return 0;
}
