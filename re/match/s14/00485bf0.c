// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 h3 0x00485bf0: 100.0% (regs 100%). Raw g2s 29.2%. Stack struct {count,max,items}; the
   callee 0x004a332b (CRT exit-style: push 0,0,code; call 0x004a3258) declared __declspec(noreturn), which drops the
   add esp after it. relocheck OK. */
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
typedef unsigned int undefined4;
typedef struct Stack {
  int count;
  int max;
  undefined4 *items;
} Stack;

__declspec(noreturn) void FUN_004a332b(int p0);
// MATCH: 0x00485bf0 FUN_00485bf0
undefined4 FUN_00485bf0(Stack *s,undefined4 v)
{
  if (s->count >= s->max) {
    return 0;
  }
  if (s->count < 0) {
    FUN_004a332b(-0x2a);
  }
  s->items[s->count] = v;
  s->count++;
  return 1;
}
