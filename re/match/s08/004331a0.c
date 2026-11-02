// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 h3 0x004331a0 (152 B): 100% (regs 100%, ops 100%), byte-identical. Raw g2s 76.5%.
   What moved it: 0x0067ebb0..0x0067ec23 is ONE global struct (float[4] at +0, ints at +0x18/+0x1c, int[20] at
   +0x20 zeroed by rep stosd, float at +0x70). Separate globals left the rep stosd setup scheduled early (94.1%,
   12 statement positions / memset / pointer loop all tried); the struct put it in place. 0x0067ec24 stays a
   separate global (as a +0x74 member its store moves up, 97.1%). The 4 floats are a loop over the float[4]
   (separate stores do not share ECX). Field names are offsets only. Residual class: none. */
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
typedef struct { float f0[4]; int u10; int u14; int u18; int u1c; int u20[20]; float f70; } S67ebb0;
MGLOBAL(float, DAT_0067ec24, 0x0067ec24);
MGLOBAL(S67ebb0, DAT_0067ebb0, 0x0067ebb0);
MGLOBAL(int, DAT_0067eca4, 0x0067eca4);
MGLOBAL(int, DAT_0067ecac, 0x0067ecac);
MGLOBAL(int, DAT_0067e844, 0x0067e844);
MGLOBAL(int, DAT_0067e9f8, 0x0067e9f8);
MGLOBAL(int, DAT_0067e914, 0x0067e914);
MGLOBAL(int, DAT_0067ea6c, 0x0067ea6c);
MGLOBAL(int, DAT_0067ea08, 0x0067ea08);
void FUN_004248b0(void);
void FUN_00424920(void);
void FUN_0042d3a0(void);
// MATCH: 0x004331a0 FUN_004331a0
void FUN_004331a0(int param_1)
{
  int i;

  if (DAT_0067eca4 == 0) {
    DAT_0067eca4 = 1;
    FUN_0042d3a0();
    DAT_0067ecac = param_1;
    for (i = 0; i < 4; i++) {
      DAT_0067ebb0.f0[i] = 210.0f;
    }
    DAT_0067ebb0.u18 = 0;
    DAT_0067ebb0.f70 = 0.0f;
    DAT_0067ebb0.u1c = 0;
    DAT_0067e844 = 0;
    DAT_0067e9f8 = 0;
    DAT_0067e914 = 0;
    DAT_0067ec24 = 0.0f;
    DAT_0067ea6c = 0;
    for (i = 0; i < 20; i++) {
      DAT_0067ebb0.u20[i] = 0;
    }
    DAT_0067ea08 = 0;
    FUN_004248b0();
    FUN_00424920();
  }
}
