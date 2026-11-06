// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045db50 (h3): 100% (regs 100%), whole symbol incl. jump table byte-identical. Raw converter
   did not compile (goto into the switch). Natural switch with case 2 = store 0.0f + return (the compiler tail-merges
   it into the final clamp store). Float literals = 0.2f @0x005cc9c0, 0.005f @0x005cd0ec, 1.0f @0x005cc320,
   0.0f @0x005d757c. Residual: none. */
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
MGLOBAL(float, DAT_0068fc8c, 0x0068fc8c);
// MATCH: 0x0045db50 FUN_0045db50
void FUN_0045db50(int param_1)
{
  switch (param_1) {
  case 0:
    DAT_0068fc8c = DAT_0068fc8c - 0.2f;
    break;
  case 1:
    DAT_0068fc8c = DAT_0068fc8c + 0.2f;
    break;
  case 2:
    DAT_0068fc8c = 0.0f;
    return;
  case 3:
    DAT_0068fc8c = DAT_0068fc8c - 0.005f;
    break;
  }
  if (DAT_0068fc8c > 1.0f) {
    DAT_0068fc8c = 1.0f;
    return;
  }
  if (DAT_0068fc8c < 0.0f) {
    DAT_0068fc8c = 0.0f;
  }
}
