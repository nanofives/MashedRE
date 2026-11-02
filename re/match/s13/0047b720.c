// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047b720 (h1) KTC_NewCopter: 100% (regs 100%). Lua C function: gettop, tostring(1) strcpy'd into a
   64-byte name, the remaining args read back to front through tonumber+_ftol into int[5], FUN_00405780, pushnumber.
   Raw 47.1%. What moved it: strcpy intrinsic, and an up-counting loop indexed args[n-1-i]/top-i (the compiler
   turns it into the dec/jne counter and keeps L on the stack inside the loop). Residual: none. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
MGLOBAL(uint, DAT_00616038, 0x00616038);
int FUN_00405780(char *name, int *args, int n);
int FUN_004b6fc0(void *L);
double FUN_004b7090(void *L, int idx);
char *FUN_004b70d0(void *L, int idx);
void FUN_004b7140(void *L, double v);

char *strcpy(char *d, const char *s);
#pragma intrinsic(strcpy)

// MATCH: 0x0047b720 FUN_0047b720
int FUN_0047b720(void *L)
{
  int top;
  int n;
  int i;
  int args[5];
  char name[64];

  top = FUN_004b6fc0(L);
  n = top - 1;
  strcpy(name, FUN_004b70d0(L, top - n));
  for (i = 0; i < n; i++) {
    args[n - 1 - i] = (int)FUN_004b7090(L, top - i);
  }
  FUN_004b7140(L, (double)FUN_00405780(name, args, n));
  return 1;
}
