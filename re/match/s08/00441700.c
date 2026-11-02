// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00441700: 100% (regs 100%), raw g2s 24.0%. By hand: the two locals are a float[2] passed to FUN_004c1c80 (cdecl, 2 args); 0.75 is the .rdata float DAT_005cc950. relocheck OK. Residual: none. */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
MGLOBAL(int, DAT_007f1a50, 0x007f1a50);
MGLOBAL(float, DAT_005cc950, 0x005cc950);
int FUN_004c1c80(int p0, float *p1);

// MATCH: 0x00441700 FUN_00441700
void FUN_00441700(int param_1)
{
  float vw[2];
  float h;

  vw[0] = ((*(float *)(param_1 + 0x6c) * *(float *)(param_1 + 0x58)) / *(float *)(param_1 + 0x24))
          * DAT_005cc950;
  h = *(float *)(param_1 + 0x70) * *(float *)(param_1 + 0x58);
  vw[1] = DAT_005cc950 * h;
  if (DAT_007f1a50 != 0) {
    vw[1] = h;
  }
  FUN_004c1c80(*(int *)(param_1 + 0x84), vw);
}
