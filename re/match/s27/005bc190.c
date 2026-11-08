// FLAGS: /O2 /Ob1 /G5
// MATCH-S27 0x005bc190: 21.3% (regs 33.7%), raw Ghidra shape, 291 vs 199 ins. Not iterated. Class: b.
/* MATCH-S27 0x005bc190: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs27_20261107/results.tsv. */
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
typedef unsigned char undefined1;
typedef unsigned int uint;
MGLOBAL(undefined1, DAT_005ce268, 0x005ce268);
MGLOBAL(undefined1, DAT_005ccd6c, 0x005ccd6c);
MGLOBAL(undefined1, DAT_005ccd14, 0x005ccd14);
MGLOBAL(undefined1, DAT_005cc348, 0x005cc348);
MGLOBAL(undefined1, DAT_005e712c, 0x005e712c);
MGLOBAL(undefined1, DAT_005e7128, 0x005e7128);
MGLOBAL(undefined1, DAT_005cc99c, 0x005cc99c);
MGLOBAL(undefined1, DAT_005e7124, 0x005e7124);
MGLOBAL(undefined1, DAT_005ccac0, 0x005ccac0);
MGLOBAL(undefined1, DAT_005e7120, 0x005e7120);
MGLOBAL(undefined1, DAT_005cc318, 0x005cc318);
MGLOBAL(undefined1, DAT_005e711c, 0x005e711c);
MGLOBAL(undefined1, DAT_005cd22c, 0x005cd22c);
MGLOBAL(undefined1, DAT_005e7118, 0x005e7118);
MGLOBAL(undefined1, DAT_005e7114, 0x005e7114);
MGLOBAL(undefined1, DAT_005e7110, 0x005e7110);
double _pow(double p0, double p1);
MGLOBAL(int, _DAT_005ce268, 0x005ce268);
MGLOBAL(int, _DAT_005ccd6c, 0x005ccd6c);
MGLOBAL(int, _DAT_005ccd14, 0x005ccd14);
MGLOBAL(int, _DAT_005cc348, 0x005cc348);
MGLOBAL(int, _DAT_005e712c, 0x005e712c);
MGLOBAL(int, _DAT_005e7128, 0x005e7128);
MGLOBAL(int, _DAT_005cc99c, 0x005cc99c);
MGLOBAL(int, _DAT_005e7124, 0x005e7124);
MGLOBAL(int, _DAT_005ccac0, 0x005ccac0);
MGLOBAL(int, _DAT_005e7120, 0x005e7120);
MGLOBAL(int, _DAT_005cc318, 0x005cc318);
MGLOBAL(int, _DAT_005e711c, 0x005e711c);
MGLOBAL(int, _DAT_005cd22c, 0x005cd22c);
MGLOBAL(int, _DAT_005e7118, 0x005e7118);
MGLOBAL(int, _DAT_005e7114, 0x005e7114);
MGLOBAL(int, _DAT_005e7110, 0x005e7110);
// MATCH: 0x005bc190 FUN_005bc190
/* [C2 2026-06-02] **Silence short-circuit**: if `*param_1 < -9999` (dB?100 floor): write `param_2
   = {0, 0, 0x3dcccccd (0.1f), 0}` and? */

void FUN_005bc190(int *param_1,int *param_2)

{
  int iVar1;
  double dVar2;
  
  if (*param_1 < -9999) {
    *param_2 = 0;
    param_2[1] = 0;
    param_2[2] = 0x3dcccccd;
    param_2[3] = 0;
    return;
  }
  dVar2 = _pow(10.0,(double)((float)*param_1 * _DAT_005ce268));
  param_2[1] = (int)(float)dVar2;
  param_2[3] = (int)(((float)param_1[0xb] - _DAT_005ccd6c) * _DAT_005ccd14);
  param_2[2] = param_1[3];
  if (-0x65 < param_1[1]) {
    if ((float)param_1[3] < _DAT_005cc348 != ((float)param_1[3] == _DAT_005cc348)) {
      *param_2 = 0;
      return;
    }
    if ((float)param_1[3] < _DAT_005e712c) {
      *param_2 = 0x14;
      return;
    }
    *param_2 = 8;
    return;
  }
  if (param_1[1] < -0xce3) {
    if ((float)param_1[3] < _DAT_005e7128 != ((float)param_1[3] == _DAT_005e7128)) {
      *param_2 = 1;
      return;
    }
    if ((float)param_1[3] < _DAT_005cc99c != ((float)param_1[3] == _DAT_005cc99c)) {
      *param_2 = 0xb;
      return;
    }
    if ((float)param_1[3] < _DAT_005e7124) {
      *param_2 = 4;
      return;
    }
    *param_2 = ((-0xa00 < param_1[5]) - 1 & 0xfffffff9) + 0x16;
    return;
  }
  iVar1 = param_1[5];
  if (-700 < iVar1) {
    if (0 < iVar1) {
      *param_2 = 0x15;
      return;
    }
    if ((float)param_1[3] < _DAT_005cc348) {
      *param_2 = 3;
      return;
    }
    *param_2 = 10;
    return;
  }
  if ((float)param_1[3] < _DAT_005ccac0 != ((float)param_1[3] == _DAT_005ccac0)) {
    *param_2 = 2;
    return;
  }
  if ((float)param_1[3] < _DAT_005e7124) {
    *param_2 = 4;
    return;
  }
  if ((float)param_1[3] < _DAT_005cc348) {
    if ((float)param_1[4] < _DAT_005e7120) {
      *param_2 = 0x11;
      return;
    }
    if ((float)param_1[4] < _DAT_005cc318) {
      *param_2 = ((-0x9a2 < iVar1) - 1 & 7) + 0xc;
      return;
    }
    if ((float)param_1[4] < _DAT_005e711c) {
      *param_2 = 0x10;
      return;
    }
    *param_2 = (uint)(iVar1 < -9999) * 4 + 0xe;
    return;
  }
  if ((float)param_1[3] < _DAT_005cd22c) {
    *param_2 = 5;
    return;
  }
  if ((float)param_1[3] < _DAT_005e7118) {
    *param_2 = 0xd;
    return;
  }
  if ((float)param_1[3] < _DAT_005e7114) {
    *param_2 = 7;
    return;
  }
  if ((float)param_1[3] < _DAT_005e7110) {
    *param_2 = 6;
    return;
  }
  *param_2 = 9;
  return;
}
