// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR06 2026-11-08: 97.6% (was 48.2), ops 100%. Typed struct + indexed loop; aligned stored before raw. Residual: the compiler-made p3-p2 delta is homed in param_1's slot ([esp+0x14]); the original homes it in param_5's ([esp+0x24]). Supersedes the header below. */
/* MATCH-RWL03 0x00533d00: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
typedef unsigned int uint;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(undefined4, DAT_0091246c, 0x0091246c);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
undefined4 FUN_0052ddf0(int p0, int p1);
// MATCH: 0x00533d00 FUN_00533d00
typedef struct { uint a; int b; uint c; uint d; } E533d;
typedef struct { uint flags; int num; uint aligned; uint raw; E533d *arr; uint f14; void *self; uint f1c; uint f20; } S533d;
S533d *FUN_00533d00(int param_1,uint *param_2,uint *param_3,uint param_4,undefined4 param_5)
{
  S533d *s;
  uint raw;
  int i;

  s = (S533d *)(**(code **)(DAT_007d3ff8 + 0x118))(DAT_0091246c,0x3011e);
  s->f20 = FUN_0052ddf0(param_1,param_5);
  s->flags = param_4;
  s->num = param_1;
  s->f14 = 0;
  if ((param_4 & 2) == 0) {
    raw = (**(code **)(DAT_007d3ff8 + 0x108))(param_1 * 0x40 + 0xf,0x3011e);
    s->aligned = (raw + 0xf) & 0xfffffff0;
    s->raw = raw;
  }
  else {
    s->aligned = 0;
    s->raw = 0;
  }
  s->arr = (E533d *)(**(code **)(DAT_007d3ff8 + 0x108))(param_1 << 4,0x3011e);
  for (i = 0; i < param_1; i++) {
    s->arr[i].d = 0;
    if (param_3 != 0) {
      s->arr[i].a = param_3[i];
    }
    s->arr[i].b = i;
    if (param_2 != 0) {
      s->arr[i].c = param_2[i];
    }
  }
  s->self = s;
  return s;
}
