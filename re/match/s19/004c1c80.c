// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c1c80 (h2 hand form, h6 flags): 100.0%, regs 100.0%, relocheck OK (DAT_005cc320 renamed from _DAT_ so relocheck resolves it), cxx ok. V2d struct copy, 1.0f in a local. The 86.4% residual (x load and fld 1.0 after push esi) was the flag set: /G5 fixes it with the source unchanged. */
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
MGLOBAL(float, DAT_005cc320, 0x005cc320);
int FUN_004c0e50(int p0);
typedef struct { float x; float y; } RwV2d_t;
// MATCH: 0x004c1c80 FUN_004c1c80
int FUN_004c1c80(int param_1, RwV2d_t *param_2)
{
  float one = DAT_005cc320;
  *(RwV2d_t *)(param_1 + 0x68) = *param_2;
  *(float *)(param_1 + 0x70) = one / *(float *)(param_1 + 0x68);
  *(float *)(param_1 + 0x74) = DAT_005cc320 / *(float *)(param_1 + 0x6c);
  if (*(int *)(param_1 + 4) != 0) {
    FUN_004c0e50(*(int *)(param_1 + 4));
  }
  return param_1;
}
