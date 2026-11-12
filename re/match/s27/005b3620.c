// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005b3620: 82.1% (regs 89.7%). Hand: intrusive list node (LN next at +4 of head, entry = node-0x20 bytes), loop in call-first form; FUN_005adf30 2-arg cdecl. Residual (a): original loads param_1 into eax first then lea ebx,[eax+4] and push order; typed struct, do/for/while forms, permuter 100 compiles did not change it. */
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
int FUN_005adf30(int p0, int p1);
typedef struct LN { struct LN *next; } LN;
typedef struct H { int f0; LN link; } H;
// MATCH: 0x005b3620 FUN_005b3620
int *FUN_005b3620(H *s, int key)
{
  LN *head = &s->link;
  LN *p = head->next;
  int *q = (int *)p - 8;
  while (FUN_005adf30(*q, key) != 0) {
    p = p->next;
    if (p == head) return (int *)0;
    q = (int *)p - 8;
  }
  return q;
}
