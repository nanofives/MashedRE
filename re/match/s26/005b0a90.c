// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 h1 0x005b0a90 (115 B). Best 100.0% (regs 100.0%), exact. 28-byte struct assignments (S7) give rep movsd; ints elsewhere. */
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
typedef struct S7 { int d[7]; } S7;
typedef struct Node { int w0; int w1; int w2; S7 a; S7 b; int w11; int w12; int pad[3]; int link[2]; } Node;
typedef struct Hdr { int next; int prev; } Hdr;
void *FUN_005aea00(int size, int tag);
// MATCH: 0x005b0a90 FUN_005b0a90
void FUN_005b0a90(Hdr *param_1,int param_2,int param_3,S7 *param_4,S7 *param_5,int param_6,int param_7)
{
  int *n = (int *)FUN_005aea00(0x54, 0x3080e);
  if (n != 0) {
    int *l = n + 0x13;
    int t = param_1->next;
    n[0x14] = (int)param_1;
    *l = t;
    *(int **)(param_1->next + 4) = l;
    param_1->next = (int)l;
    if (param_4 != 0) {
      *(S7 *)(n + 3) = *param_4;
    }
    if (param_5 != 0) {
      *(S7 *)(n + 10) = *param_5;
    }
    n[1] = param_2;
    n[2] = param_3;
    n[0x12] = param_6;
    n[0x11] = param_7;
    n[0] = 0;
  }
}
