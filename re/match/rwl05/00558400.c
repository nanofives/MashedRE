// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x00558400: hand-matched (h1). 100%. Inline memset via #pragma intrinsic, row pointer split from the advanced pointer, count reloaded into param_2. */
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
void * __cdecl memset(void *, int, unsigned int);
// MATCH: 0x00558400 FUN_00558400
unsigned char * FUN_00558400(unsigned char *param_1,int param_2,unsigned char *param_3,int param_4)
{
  unsigned int uVar2;
  unsigned char bVar3;
  unsigned char *p;

  int cnt = *(int *)(param_2 + 0xc);
  uVar2 = *(unsigned int *)(param_2 + 8);
  if (0 < cnt) {
    param_2 = cnt;
    do {
      p = param_3;
      param_3 = param_3 + param_4;
      memset(p, 0, uVar2);
      for (bVar3 = *param_1; bVar3 != 0; bVar3 = bVar3 >> 1) {
        *p = bVar3 & 1;
        p = p + 1;
      }
      param_1 = param_1 + 1;
      param_2 = param_2 - 1;
    } while (param_2 != 0);
  }
  return param_1;
}
