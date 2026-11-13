// FLAGS: /O2 /Ob1 /G5
// MATCH-RWC-NEAR-B (2026-11-12): 100% BYTE-IDENTICAL under ddk and rw9044. Was 81.8% (esi/edi swap). Lever: one
// nested success branch (`if (o) { ... return o; }` then the alloc-failure error at the end) with the size written
// `num * 20` at both uses (no size local) and o->data tested through the struct field.
/* MATCH-RWL03 0x004f0ae0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef int (*Cb004f0ae0)(int, int);
typedef int *(*CbP004f0ae0)(int, int);
MGLOBAL(int, DAT_007d7290, 0x007d7290);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int FUN_004d7ff0(int p0, int p1);
void FUN_004d8480(int *p0);
typedef struct { int pluginID; int errorCode; } Err004f0ae0;
typedef struct { int n; int x; void *data; } Obj004f0ae0;
// MATCH: 0x004f0ae0 FUN_004f0ae0
Obj004f0ae0 * FUN_004f0ae0(int num)
{
  Obj004f0ae0 *o;
  o = (Obj004f0ae0 *)(*(CbP004f0ae0 *)(DAT_007d3ff8 + 0x118))(DAT_007d7290,0x30502);
  if (o) {
    o->x = 0;
    if (num) {
      o->data = (void *)(*(Cb004f0ae0 *)(DAT_007d3ff8 + 0x108))(num * 20,0x1030502);
      if (!o->data) {
        (*(Cb004f0ae0 *)(DAT_007d3ff8 + 0x11c))(DAT_007d7290,(int)o);
        {
          Err004f0ae0 e;
          e.pluginID = 2;
          e.errorCode = FUN_004d7ff0(0x80000013,num * 20);
          FUN_004d8480((int *)&e);
        }
        return 0;
      }
      o->n = num;
    } else {
      o->data = 0;
      o->n = 0;
    }
    return o;
  }
  {
    Err004f0ae0 e;
    e.pluginID = 2;
    e.errorCode = FUN_004d7ff0(0x80000013,0xc);
    FUN_004d8480((int *)&e);
  }
  return 0;
}
