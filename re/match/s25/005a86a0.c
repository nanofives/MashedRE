// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005a86a0 (h2): best 98.5% (regs 98.5%), class a. one store ([esi]=ecx) sits between the 3rd and 4th struct dwords in the original. relocheck BAD only because the code differs from the original. */
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
typedef struct { int a; int b; int c; int d; } V4_005a86a0;
__declspec(dllimport) unsigned long __stdcall WaitForSingleObject(void * p0, unsigned long p1);
__declspec(dllimport) int __stdcall ReleaseSemaphore(void * p0, long p1, long * p2);
MGLOBAL(void *, DAT_007dcae0, 0x007dcae0);
void FUN_005b0ca0(int p0, int * p1);
char * FUN_004a34b0(char * p0, char * p1, unsigned int p2);
// MATCH: 0x005a86a0 FUN_005a86a0
void FUN_005a86a0(int param_1,int *param_2)

{
  int *piVar1;
  V4_005a86a0 *src;

  piVar1 = (int *)(param_1 + 0x28);
  piVar1[0] = param_2[0];
  piVar1[1] = param_2[1];
  *(V4_005a86a0 *)(param_1 + 0x30) = *(V4_005a86a0 *)*param_2;
  piVar1[0] = param_1 + 0x30;
  param_2 = (int *)*param_2;
  if (param_2[1] == 1) {
    FUN_004a34b0((char *)(param_1 + 0x40),(char *)*param_2,0x80);
  }
  else if (param_2[1] == 2) {
    *(V4_005a86a0 *)(param_1 + 0xdc) = *(V4_005a86a0 *)*param_2;
  }
  WaitForSingleObject(DAT_007dcae0,0xffffffff);
  FUN_005b0ca0(param_1 + 0x154,piVar1);
  ReleaseSemaphore(DAT_007dcae0,1,0);
}
