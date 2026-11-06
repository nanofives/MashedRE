// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045cbe0 (h2): 100% (byte-identical whole symbol, 105 B; Ghidra's 102 B stops before the
   pop/pop/ret, so the instruction score reads 92.5%). Raw 43.5%. The file also holds its callee 0x0045c4e0 (100%),
   non-static and defined before the caller: with its body in the TU the compiler keeps DAT_0088fbc4 (ESI) and
   *DAT_0088fbc4 (EDI) across the call, as the original does. Explicit guard + do-while with a separate loop
   counter gives the jge-before-mov order. */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
typedef struct { int f[10]; } Rec28;
MGLOBAL(int *, DAT_0088fbc4, 0x0088fbc4);
MGLOBAL(Rec28 *, DAT_0088fbc8, 0x0088fbc8);
void FUN_0045c640(int param_1);
// MATCH: 0x0045c4e0 FUN_0045c4e0
int FUN_0045c4e0(int param_1, Rec28 *param_2, int param_3)
{
  int i;

  for (i = 0; i < param_3; i++) {
    if (param_2[i].f[0] == param_1)
      return i;
  }
  return -1;
}

// MATCH: 0x0045cbe0 FUN_0045cbe0
void FUN_0045cbe0(int param_1, int param_2)
{
  int idx;
  int i;

  FUN_0045c640(param_2);
  idx = FUN_0045c4e0(param_1, DAT_0088fbc8, *DAT_0088fbc4);
  if (idx != -1) {
    if (idx < *DAT_0088fbc4 - 1) {
      i = idx;
      do {
        DAT_0088fbc8[i] = DAT_0088fbc8[i + 1];
        i++;
      } while (i < *DAT_0088fbc4 - 1);
    }
    (*DAT_0088fbc4)--;
  }
}
