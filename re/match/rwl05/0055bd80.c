// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x0055bd80: hand-matched (h3). 100%. if/else chain instead of the raw conditional-assign form. */
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
typedef int (*VCb)(int, int, int, int);
int FUN_004c4600(void *p0, int p1, int p2);
// MATCH: 0x0055bd80 FUN_0055bd80
void FUN_0055bd80(int param_1, int param_2, int param_3, int param_4)
{
  int iVar1;
  char local_40[64];

  if ((*(unsigned char *)(*(int *)(param_1 + 0x5c) + 0x40) & 2) == 0 &&
      (*(unsigned int *)(param_1 + 0xc) & 0x20000) == 0) {
    if (param_2 != 0) {
      iVar1 = FUN_004c4600(local_40, param_1, param_2);
    } else {
      iVar1 = param_1;
    }
  } else {
    iVar1 = param_2;
  }
  (*(VCb *)(*(int *)(param_1 + 0x5c) + 0x10))(param_1, iVar1, param_3, param_4);
}
