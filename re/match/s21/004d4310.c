// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d4310 (h4): 95.7% (regs 95.7%). Rewritten from odis with structs (entries stride 0x28 holding Sub*, Sub count +0x20 / items +0x24 stride 0xc), unsigned for-loops, and last = min at the outer loop head (raw 42.6%). Residual: one instruction moves. The original reloads the spilled count into eax before 'cmp edi,-1', so both the found-none exit and the inc share it. Ours reloads after the je. Tried break / return-in-loop / else-return / count++ in the for header, permuter 112 compiles 0 gains. Class (b). */
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
typedef unsigned int uint;
typedef struct Item { uint key; int pad[2]; } Item;
typedef struct Sub { char pad[0x20]; uint count; Item *items; } Sub;
typedef struct Entry { Sub *sub; char pad[0x24]; } Entry;
typedef struct Obj { int pad; uint count; Entry *entries; } Obj;
// MATCH: 0x004d4310 FUN_004d4310
int FUN_004d4310(Obj *obj)
{
  int count = 0;
  uint n = obj->count;
  uint min = 0;
  for (;;) {
    uint last = min;
    uint i;
    min = 0xffffffff;
    for (i = 0; i < n; i++) {
      Sub *sub = obj->entries[i].sub;
      uint j;
      for (j = 0; j < sub->count; j++) {
        uint v = sub->items[j].key;
        if (v > last && v < min) min = v;
      }
    }
    if (min == 0xffffffff) break;
    count++;
  }
  return count;
}
