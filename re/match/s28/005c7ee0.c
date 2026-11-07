// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 h1 0x005c7ee0 : 100.0%. By hand (raw did not compile): cdecl 2 args, the object is the SECOND arg (mov ebx,[esp+0xc] right after push ebx); memset intrinsic (rep stosd/stosb) twice, Wait/ReleaseSemaphore imports at 0x5cc090/0x5cc094, intrusive list walk from param_1+0x1c with node = link-0xd0. Residual: none. */


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

#pragma intrinsic(memset)
void *memset(void *d, int c, unsigned int n);
__declspec(dllimport) unsigned long __stdcall WaitForSingleObject(void *h, unsigned long ms);
__declspec(dllimport) int __stdcall ReleaseSemaphore(void *h, long n, long *prev);
int FUN_005ab040(void);
void FUN_005c9d10(int p0);
void FUN_005c98d0(int p0);
void FUN_005c9740(int p0);
// MATCH: 0x005c7ee0 FUN_005c7ee0
int FUN_005c7ee0(int param_0, int param_1)
{
  int *pNode;
  int t;
  int node;

  memset(*(void **)(param_1 + 0x38), 0, *(unsigned int *)(param_1 + 0x34));
  memset(*(void **)(param_1 + 0x3c), 0, *(unsigned int *)(param_1 + 0x34));
  WaitForSingleObject((void *)*(int *)(param_1 + 4), 0xffffffff);
  t = FUN_005ab040();
  pNode = *(int **)(param_1 + 0x1c);
  while (pNode != (int *)(param_1 + 0x1c)) {
    node = (int)pNode - 0xd0;
    *(int *)((int)pNode - 0x30) = t;
    *(int *)((int)pNode - 0x40) = *(int *)((int)pNode - 0x48);
    *(int *)((int)pNode - 0x3c) = *(int *)((int)pNode - 0x44);
    FUN_005c9d10(node);
    FUN_005c98d0(node);
    pNode = (int *)*pNode;
  }
  ReleaseSemaphore((void *)*(int *)(param_1 + 4), 1, (long *)0);
  FUN_005c9740(param_1);
  return param_1 + 0x40;
}

