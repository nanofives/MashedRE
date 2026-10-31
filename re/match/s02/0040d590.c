// FLAGS: /O2 /GS /Ob1
/* 0x0040d590 FUN_0040d590 (850 B): best 100.0% exact, regs 100.0%, whole symbol BYTE-IDENTICAL (match.py --bytes).
   The six repeated float blocks are one helper inlined five times: Ahead(a, b) reads FUN_00408ad0 for both,
   wraps 80/20/100 (floats at 0x005cc730 = 80.0f, 0x005ccd6c = 20.0f, 0x005cc568 = 100.0f) and returns fa > fb.
   Raw Ghidra 31.1%. A plain static or static __inline helper is NOT inlined under /Ob1 here (37.4%: the
   compiler emits it as a register-passed static callee); static __forceinline gives 100%. Ahead is not a
   separate function in the original, so it has no MATCH line. Branch nesting written in the original's
   block order (fallthrough = "first argument ahead"). No residual. */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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

float FUN_00408ad0(int param_1);
int FUN_00422fd0(int param_1);
void FUN_0040b290(int param_1, int param_2);

static __forceinline int Ahead(int a, int b)
{
  float fa = FUN_00408ad0(a);
  float fb = FUN_00408ad0(b);

  if (fa > 80.0f && fb < 20.0f) {
    fa -= 100.0f;
  } else if (fb > 80.0f && fa < 20.0f) {
    fb -= 100.0f;
  }
  return fa > fb;
}

// MATCH: 0x0040d590 FUN_0040d590
void FUN_0040d590(int param_1, int param_2, int param_3, int param_4)
{
  if (Ahead(param_1, param_2)) {
    if (Ahead(param_3, param_2)) {
      FUN_00422fd0(param_2);
      FUN_0040b290(param_2, -param_4);
      if (Ahead(param_1, param_3)) {
        FUN_00422fd0(param_3);
        FUN_0040b290(param_3, param_4);
        FUN_0040b290(param_1, param_4 * 2);
      } else {
        FUN_00422fd0(param_1);
        FUN_0040b290(param_1, param_4);
        FUN_0040b290(param_3, param_4 * 2);
      }
    } else {
      FUN_00422fd0(param_3);
      FUN_0040b290(param_3, -param_4);
      FUN_00422fd0(param_2);
      FUN_0040b290(param_2, param_4);
      FUN_0040b290(param_1, param_4 * 2);
    }
  } else {
    if (Ahead(param_3, param_1)) {
      FUN_00422fd0(param_1);
      FUN_0040b290(param_1, -param_4);
      if (Ahead(param_2, param_3)) {
        FUN_00422fd0(param_3);
        FUN_0040b290(param_3, param_4);
        FUN_0040b290(param_2, param_4 * 2);
      } else {
        FUN_00422fd0(param_2);
        FUN_0040b290(param_2, param_4);
        FUN_0040b290(param_3, param_4 * 2);
      }
    } else {
      FUN_00422fd0(param_3);
      FUN_0040b290(param_3, -param_4);
      FUN_00422fd0(param_1);
      FUN_0040b290(param_1, param_4);
      FUN_0040b290(param_2, param_4 * 2);
    }
  }
}
