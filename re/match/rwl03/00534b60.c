// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x00534b60: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned char byte;
MGLOBAL(undefined1, DAT_00623c78, 0x00623c78);
int FUN_00534d00(int p0, uint p1, int p2, void * p3);
// MATCH: 0x00534b60 FUN_00534b60
void FUN_00534b60(undefined4 param_1,uint param_2,undefined4 param_3)
{
  int f1, f4, f8, f20, f40, f80, f100, f20000, f40000, f80000, f100000;
  uint v, t;

  v = param_2;
  f1 = (param_2 & 1) == 1;
  f4 = (param_2 & 4) == 4;
  f8 = (param_2 & 8) == 8;
  f20 = (param_2 & 0x20) == 0x20;
  f40 = (param_2 & 0x40) == 0x40;
  f80 = (param_2 & 0x80) == 0x80;
  f100 = (param_2 & 0x100) == 0x100;
  f20000 = (param_2 & 0x20000) == 0x20000;
  f40000 = (param_2 & 0x40000) == 0x40000;
  f80000 = (param_2 & 0x80000) == 0x80000;
  f100000 = (param_2 & 0x100000) == 0x100000;
  if (f8 == 1 && (t = param_2 & 0x8000) == 0x8000) {
    v = t;
  }
  if (f20 == 1 && f20000 == 1) {
    v = v & 0x20000;
  }
  if (f40 == 1 && f40000 == 1) {
    v = v & 0x40000;
  }
  if (f80 == 1 && f80000 == 1) {
    v = v & 0x80000;
  }
  if (f100 == 1 && f100000 == 1) {
    v = v & 0x100000;
  }
  if (f8 == 1) {
    if (f1 == 1) {
      v = v & 0xfffffffe;
    }
    if (f4 == 1) {
      v = v & 0xfffffffb;
    }
    if (f20 == 1) {
      v = v & 0xffffffdf;
    }
  }
  if (f8 == 0 && f1 == 0) {
    v = v | 1;
  }
  if ((v & 0x30000000) == 0) {
    v = v | 0x10000000;
  }
  FUN_00534d00(param_1,v,param_3,&DAT_00623c78);
}
