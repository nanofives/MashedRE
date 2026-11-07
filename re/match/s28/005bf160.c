// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 h1 0x005bf160 : 47.7% (regs 82, ops 88). By hand from the asm: cdecl 3 args; vtbl[10] returns an enumerator through &param_1 (arg slot), AddRef vtbl[5], Next vtbl[3](q,1,&L,0), per item vtbl[9](L,&param_3) compared with param_2, returns the param_3-th match (still referenced), releases the enumerator. The original zero register is edi (L=0, Next's 4th arg) and its loop bottom/tail layout differs (do-while with the Next call duplicated before the loop and the tail inlined twice); for/do-while/break forms tried, allocation and block order not reached. Residual class (b). */

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

typedef int (__stdcall *F2)(int *, void *);
typedef int (__stdcall *F4)(int *, int, void *, int);
typedef int (__stdcall *RFN)(int *);
// MATCH: 0x005bf160 FUN_005bf160
int *FUN_005bf160(int *param_1, int param_2, int param_3)
{
  int *L;
  int hr;
  int idx;
  int want;
  int key;
  int t;

  L = (int *)0;
  hr = ((F2)(*(int **)param_1)[10])(param_1, &param_1);
  if (hr >= 0) {
    idx = 0;
    ((RFN)(*(int **)param_1)[5])(param_1);
    hr = ((F4)(*(int **)param_1)[3])(param_1, 1, &L, 0);
    if (hr == 0) {
      want = param_3;
      key = param_2;
      do {
        hr = ((F2)(*(int **)L)[9])(L, &param_3);
        if (hr < 0) {
          ((RFN)(*(int **)L)[2])(L);
          L = (int *)0;
          break;
        }
        if (param_3 == key) {
          t = idx;
          idx++;
          if (t == want) break;
        }
        ((RFN)(*(int **)L)[2])(L);
        L = (int *)0;
        hr = ((F4)(*(int **)param_1)[3])(param_1, 1, &L, 0);
      } while (hr == 0);
    }
    ((RFN)(*(int **)param_1)[2])(param_1);
  }
  return L;
}
