// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d4f00 (h1, by hand): best 98.5% exact, regs 98.5% (raw 74.2%, esi/edi swapped). Find by name in a
   40-byte element array (count +4, elems +8), optionally starting after a given element; strcmp through
   RwGlobals+0xe8; *index = count - remaining or -1. Moves: reading elems next to count, before the count test
   (74.2 -> 98.5: fixes the esi/edi assignment). Residual: one load. The original loads elems after the param_3
   load inside the count != 0 branch; with that placement 9178 swaps esi/edi (74.2). Tried: declaration order,
   register on either variable, for vs while, int/long/uint/short counter, uint count, condition merges, permuter
   (241 compiles over both forms, no gain). Class (b): register allocation that moves with the source. C++ ok. */
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
typedef struct { char *name; } Named_;
typedef struct { Named_ *obj; int rest[9]; } Elem_;
typedef struct { int pad; int count; Elem_ *elems; } List_;
typedef int (*FUN_004d4f00_strcmp)(const char *a, const char *b);
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
// MATCH: 0x004d4f00 FUN_004d4f00
Elem_ *FUN_004d4f00(List_ *param_1,const char *param_2,Elem_ *param_3,int *param_4)

{
  Elem_ *e;
  int n;

  if (param_1 != 0 && param_2 != 0) {
    n = param_1->count;
    e = param_1->elems;
    if (n != 0) {
      if (param_3 != 0) {
        while (e != param_3 && n > 0) {
          e++;
          n--;
        }
        e++;
        n--;
      }
      while (n > 0) {
        if (e->obj != 0 &&
            (*(FUN_004d4f00_strcmp *)(DAT_007d3ff8 + 0xe8))(e->obj->name, param_2) == 0) {
          if (param_4 != 0) {
            *param_4 = param_1->count - n;
          }
          return e;
        }
        e++;
        n--;
      }
    }
  }
  if (param_4 != 0) {
    *param_4 = -1;
  }
  return 0;
}
