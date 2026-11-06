// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 h6 0x004922e0: 100% (regs 100%). Hand: 0x4c-stride struct array at 0x007f1058, int array at 0x007f1a14 indexed [i*4], callees return int. Residual: none. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
typedef struct {
  int f0;
  int f4;
  int f8;
  int fc;
  int f10;
  int f14;
  int pad[13];
} S7f1058;
MGLOBALA(S7f1058, DAT_007f1058, 1, 0x007f1058);
MGLOBALA(int, DAT_007f1a14, 4, 0x007f1a14);
int FUN_0040e350(void);
int FUN_0040e470(int p0);
// MATCH: 0x004922e0 FUN_004922e0
void FUN_004922e0(int param_1,int param_2,int param_3,int param_4)
{
  S7f1058 *p;
  if (FUN_0040e470(param_1) == 1) {
    if (FUN_0040e350() == 6) {
      p = &DAT_007f1058[DAT_007f1a14[param_1 * 4]];
      if (p->f4 != 0) {
        p->f10 = param_2;
        p->f0 = 1;
        p->fc = param_3;
        p->f14 = param_4;
      }
    }
  }
}
