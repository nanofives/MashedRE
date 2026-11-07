// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005b6760: 100.0% (regs 100.0%). Hand: the original reuses the parameter slots as address-taken locals: param_1 is the predictor (&param_1 passed to FUN_005b6b00), param_2 the group counter (cast int), idx a real local; 'param_3--' before the load of *p++; the clamp is idx<0||idx>0x58 (signed). */
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
typedef unsigned int uint;
void FUN_005b6b00(uint p0, int *p1, int *p2);
// MATCH: 0x005b6760 FUN_005b6760
undefined4 FUN_005b6760(uint *param_1, short *param_2, int param_3)
{
  uint *p;
  short *o;
  uint w;
  uint v;
  int idx;
  int c;

  p = param_1;
  o = param_2;
  while (param_3 != 0) {
    param_3--;
    w = *p++;
    param_1 = (uint *)(int)(short)w;
    idx = (w >> 0x10) & 0x7f;
    if (idx < 0 || idx > 0x58) {
      return 0;
    }
    *o++ = (short)(int)param_1;
    for (param_2 = (short *)0; (int)param_2 < 8; param_2 = (short *)((int)param_2 + 1)) {
      v = *p++;
      for (c = (param_2 != (short *)7) + 7; c > 0; c--) {
        FUN_005b6b00(v & 0xf, (int *)&param_1, &idx);
        *o++ = (short)(int)param_1;
        v >>= 4;
      }
    }
  }
  return 1;
}
