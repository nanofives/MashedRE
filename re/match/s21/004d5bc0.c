// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d5bc0 (h2): 88.6% (regs 89.1%, ops 99.8%). The file also holds 0x004d5480 (100%, the
   render-state setter, non-static: defined before its caller so the compiler knows it leaves edx alone; the
   original keeps 0 and 5 in edx across its calls, Ghidra's extraout_EDX). Written from odis.py + the raw form.
   D3D9 render-state reset: fills the default table 0x007d54b0 (0xd2 ints) with -1 except states 0x22/0x8b/
   0x3a/0x3b, copies it into the {value, changed} cache at 0x007d57f8, clears the texture-stage cache (8 x 0x21
   {value, dirty} at 0x007d4720), the sampler cache 0x007d4f60 (8 x 14) and the other tables, then sets the
   default render states (inline RS_SET macro for 0x1c..0x39, FUN_004d5480 for the rest), per stage i < 8:
   device->SetTexture(i, 0) (vtbl +0x104), sampler state 6 (inline cache test + vtbl +0x114 when caps
   0x00911fe0 & 0x200) and FUN_004d5570 / FUN_004d54f0 calls, then tail-calls FUN_004d53b0.
   What moved it: FUN_004d5480 defined in the file (raw 56.8% -> 74.3% static (register args, wrong) ->
   87.9% non-static), an unsigned copy loop, a pointer walk over the stage cache (88.1%), and the permuter
   (1162 compiles: value store before changed in the copy loop, 88.6%). relocheck BAD = the residual below.
   Residual: (1) both cache-init loops base their pointer on the second struct field in the original
   (`[ecx]` = changed, `[ecx-4]` = value; `[eax-4]`/`[eax]` for the stage cache) and on the first in ours;
   (2) the five zero stores around the 0x007d5120 memset use eax (the memset value) in the original, edx in ours.
   Class (b).
   RWC-DRIVER (2026-11-11): 100% (relocheck OK). Moved by: DAT_007d6c24/6c28/6c00/6c04/6c08 typed float (the
   original's eax zero stores), the stage-cache fill as counted do-whiles over an int* at .dirty (89.8%), then the
   permuter's swap: the DAT_007d6c24 store after the 0x007d5120 memset in source (100%). */
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
typedef struct RSCache { int value; int changed; } RSCache;
typedef struct StageInfo { int texture; int a; int b; int c; int d; int e; } StageInfo;
typedef struct IDirect3DDevice9 { void **lpVtbl; } IDirect3DDevice9;
typedef int (__stdcall *D3DSetTextureFn)(IDirect3DDevice9 *, unsigned int, void *);
typedef int (__stdcall *D3DSetSamplerStateFn)(IDirect3DDevice9 *, unsigned int, int, unsigned int);
MGLOBALA(int, DAT_007d54b0, 0xd2, 0x007d54b0);
MGLOBALA(RSCache, DAT_007d57f8, 0xd2, 0x007d57f8);
MGLOBALA(int, DAT_007d5168, 0x58, 0x007d5168);
MGLOBAL(int, DAT_007d6c14, 0x007d6c14);
MGLOBALA(int, DAT_007d5e88, 0x108, 0x007d5e88);
typedef struct StateEnt { int value; int dirty; } StateEnt;
typedef StateEnt StateRow[33];
MGLOBALA(StateRow, DAT_007d4720, 26, 0x007d4720);
MGLOBALA(int, DAT_007d4f60, 0x70, 0x007d4f60);
MGLOBALA(int, DAT_007d5120, 0x11, 0x007d5120);
MGLOBALA(StageInfo, DAT_007d6b30, 8, 0x007d6b30);
MGLOBAL(IDirect3DDevice9 *, DAT_007d4110, 0x007d4110);
MGLOBAL(unsigned int, DAT_00911fc4, 0x00911fc4);
MGLOBAL(unsigned int, DAT_00911fe0, 0x00911fe0);
MGLOBAL(float, DAT_007d6c24, 0x007d6c24);
MGLOBAL(float, DAT_007d6c28, 0x007d6c28);
MGLOBAL(float, DAT_007d6c00, 0x007d6c00);
MGLOBAL(float, DAT_007d6c04, 0x007d6c04);
MGLOBAL(float, DAT_007d6c08, 0x007d6c08);
MGLOBAL(int, DAT_007d6c18, 0x007d6c18);
MGLOBAL(int, DAT_007d6c1c, 0x007d6c1c);
MGLOBAL(int, DAT_007d6c20, 0x007d6c20);
MGLOBAL(int, DAT_007d6b1c, 0x007d6b1c);
MGLOBAL(int, DAT_007d6b20, 0x007d6b20);
MGLOBAL(float, DAT_007d6b24, 0x007d6b24);
MGLOBAL(int, DAT_007d6b28, 0x007d6b28);
MGLOBAL(int, DAT_007d6b2c, 0x007d6b2c);
MGLOBAL(int, DAT_007d6ae8, 0x007d6ae8);
MGLOBAL(int, DAT_007d6aec, 0x007d6aec);
MGLOBAL(int, DAT_007d6af0, 0x007d6af0);
MGLOBAL(int, DAT_007d6af4, 0x007d6af4);
MGLOBAL(int, DAT_007d6af8, 0x007d6af8);
MGLOBAL(int, DAT_007d6afc, 0x007d6afc);
MGLOBAL(int, DAT_007d6b00, 0x007d6b00);
MGLOBAL(int, DAT_007d6b04, 0x007d6b04);
MGLOBAL(int, DAT_007d6b08, 0x007d6b08);
MGLOBAL(int, DAT_007d6b0c, 0x007d6b0c);
MGLOBAL(int, DAT_007d6b10, 0x007d6b10);
MGLOBAL(int, DAT_007d6b14, 0x007d6b14);
MGLOBAL(int, DAT_007d6b18, 0x007d6b18);
MGLOBAL(int, DAT_007d6bf0, 0x007d6bf0);
MGLOBAL(int, DAT_007d6bf4, 0x007d6bf4);
MGLOBAL(int, DAT_007d6bf8, 0x007d6bf8);
MGLOBAL(int, DAT_007d6bfc, 0x007d6bfc);
#ifdef __cplusplus
extern "C"
#endif
void *memset(void *dst, int c, unsigned int n);
#pragma intrinsic(memset)
void FUN_004d53b0(void);
void FUN_004d54f0(int stage, int type, int value);
void FUN_004d5570(int stage, int type, int value);
#define RS_SET(s, v) \
  if (DAT_007d57f8[s].value != (v)) { \
    DAT_007d57f8[s].value = (v); \
    if (!DAT_007d57f8[s].changed) { \
      DAT_007d57f8[s].changed = 1; \
      DAT_007d5168[DAT_007d6c14] = (s); \
      DAT_007d6c14++; \
    } \
  }
// MATCH: 0x004d5480 FUN_004d5480
void FUN_004d5480(int state, int value)
{
  RS_SET(state, value);
}
// MATCH: 0x004d5bc0 FUN_004d5bc0
void FUN_004d5bc0(void)
{
  int i, j;
  unsigned int u;
  memset(DAT_007d54b0, 0xff, sizeof(DAT_007d54b0));
  DAT_007d54b0[0x22] = 0;
  DAT_007d54b0[0x8b] = 0;
  DAT_007d54b0[0x3a] = 0;
  DAT_007d54b0[0x3b] = 0;
  for (u = 0; u < 0xd2; u++) {
    DAT_007d57f8[u].value = DAT_007d54b0[u];
    DAT_007d57f8[u].changed = 0;
  }
  memset(DAT_007d5e88, 0xff, sizeof(DAT_007d5e88));
  DAT_007d6c14 = 0;
  {
    int *p = &DAT_007d4720[0][0].dirty;
    i = 8;
    do {
      j = 0x21;
      do {
        p[-1] = -1;
        *p = 0;
        p += 2;
      } while (--j);
    } while (--i);
  }
  memset(DAT_007d4f60, 0xff, sizeof(DAT_007d4f60));
  memset(DAT_007d5120, 0, sizeof(DAT_007d5120));
  DAT_007d6c24 = 0;
  DAT_007d6c28 = 0;
  DAT_007d6c00 = 0;
  DAT_007d6c04 = 0;
  DAT_007d6c08 = 0;
  DAT_007d6c18 = 0;
  DAT_007d6c1c = 0;
  DAT_007d6c20 = 0;
  DAT_007d6b1c = 0;
  DAT_007d6b20 = 1;
  DAT_007d6b24 = 1.0f;
  DAT_007d6b28 = 0;
  RS_SET(0x1c, 0);
  if ((DAT_00911fc4 & 0x100) && (DAT_00911fc4 & 0x100000)) {
    RS_SET(0x23, 3);
    RS_SET(0x8c, 0);
  } else {
    RS_SET(0x23, 0);
    RS_SET(0x8c, 3);
  }
  RS_SET(0x22, 0);
  RS_SET(9, 2);
  DAT_007d6b2c = 2;
  RS_SET(0x17, 4);
  RS_SET(0xe, 1);
  RS_SET(7, 1);
  DAT_007d6ae8 = 1;
  DAT_007d6aec = 1;
  RS_SET(0x34, 0);
  RS_SET(0x35, 1);
  RS_SET(0x36, 1);
  RS_SET(0x37, 1);
  RS_SET(0x38, 8);
  RS_SET(0x39, 0);
  FUN_004d5480(0x3a, -1);
  FUN_004d5480(0x3b, -1);
  DAT_007d6b00 = 8;
  DAT_007d6b08 = -1;
  DAT_007d6b0c = -1;
  DAT_007d6af0 = 0;
  DAT_007d6af4 = 1;
  DAT_007d6af8 = 1;
  DAT_007d6afc = 1;
  DAT_007d6b04 = 0;
  for (i = 0; i < 8; i++) {
    ((D3DSetTextureFn)DAT_007d4110->lpVtbl[0x104 / 4])(DAT_007d4110, i, 0);
    DAT_007d6b30[i].texture = 0;
    if (DAT_00911fe0 & 0x200) {
      if (DAT_007d4f60[i * 0xe + 6] != 2) {
        DAT_007d4f60[i * 0xe + 6] = 2;
        ((D3DSetSamplerStateFn)DAT_007d4110->lpVtbl[0x114 / 4])(DAT_007d4110, i, 6, 2);
      }
      FUN_004d5570(i, 5, 2);
      DAT_007d6b30[i].c = 2;
    } else {
      FUN_004d5570(i, 6, 1);
      FUN_004d5570(i, 5, 1);
      DAT_007d6b30[i].c = 1;
    }
    FUN_004d5570(i, 7, 0);
    FUN_004d5570(i, 1, 1);
    FUN_004d5570(i, 2, 1);
    DAT_007d6b30[i].a = 1;
    DAT_007d6b30[i].b = 1;
    FUN_004d5570(i, 4, 0xff000000);
    DAT_007d6b30[i].d = 0xff000000;
    DAT_007d6b30[i].e = 1;
    FUN_004d5570(i, 10, 1);
    if (i != 0) {
      FUN_004d54f0(i, 1, 1);
      FUN_004d54f0(i, 4, 1);
    }
  }
  FUN_004d5480(0x13, 5);
  FUN_004d5480(0x14, 6);
  DAT_007d6bf0 = 5;
  DAT_007d6bf4 = 6;
  FUN_004d5480(0x19, 5);
  DAT_007d6bf8 = 5;
  DAT_007d6bfc = 1;
  FUN_004d5480(0x18, 0);
  FUN_004d5480(0x1b, 0);
  FUN_004d5480(0xf, 0);
  DAT_007d6b10 = 0;
  DAT_007d6b14 = 0;
  FUN_004d5480(0x89, 0);
  FUN_004d5480(0x16, 2);
  DAT_007d6b18 = 2;
  FUN_004d54f0(0, 1, 3);
  FUN_004d54f0(0, 3, 0);
  FUN_004d54f0(0, 4, 3);
  FUN_004d54f0(0, 6, 0);
  FUN_004d5480(0x91, 0);
  FUN_004d5480(0x92, 0);
  FUN_004d5480(0x93, 0);
  FUN_004d5480(0x1a, DAT_00911fc4 & 1);
  FUN_004d5480(0x1d, 0);
  FUN_004d5480(0x8e, 0);
  FUN_004d5480(0x8b, -1);
  FUN_004d5480(0x8f, 0);
  FUN_004d53b0();
}
