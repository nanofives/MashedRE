// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f3d40 (h3, by hand): 76.7% (regs 78.1%) under /O2 /Ob1 /G5. Raw converter 26.0%.
   Stream write of the {int *data; int count;} list (size loop = 0x004f3ce0's body, two chunk headers via
   FUN_004cc580, count, per-entry first-index via FUN_004cc770(&j), then FUN_004e8440 per unique entry).
   What moved it: list param copied to a local (the address-taken j reuses param 1's slot) and a separate
   non-address-taken index for the size loop (56 -> 76.7%).
   Residual: the original duplicates the epilogue after the two FUN_004cc580 failures (returning the zero eax,
   no xor) and a third time with xor after FUN_004cc770; ours merges all failure returns into one epilogue.
   Tried: pointer/int callee returns, returning the call result, nested-if form, permuter (114 compiles). */
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
typedef struct { int *data; int count; } S22IntList;
int FUN_004cc580(void *stream, int type, int size, unsigned int version, unsigned int build);
void *FUN_004cc770(void *stream, const void *data, unsigned int size);
int FUN_004e8400(int p0);
void *FUN_004e8440(int p0, void *stream);
// MATCH: 0x004f3d40 FUN_004f3d40
S22IntList *FUN_004f3d40(S22IntList *l, void *stream)
{
  S22IntList *list = l;
  int size;
  int i, j, k;

  size = list->count * 4 + 0x10;
  for (i = 0; i < list->count; i++) {
    k = i;
    while (k--) {
      if (list->data[k] == list->data[i]) break;
    }
    if (k < 0) {
      size += 0xc + FUN_004e8400(list->data[i]);
    }
  }
  if (!FUN_004cc580(stream, 8, size, 0x37002, 10)) return 0;
  if (!FUN_004cc580(stream, 1, list->count * 4 + 4, 0x37002, 10)) return 0;
  if (!FUN_004cc770(stream, &list->count, 4)) return 0;
  for (i = 0; i < list->count; i++) {
    j = i;
    while (j--) {
      if (list->data[j] == list->data[i]) break;
    }
    if (!FUN_004cc770(stream, &j, 4)) return 0;
  }
  for (i = 0; i < list->count; i++) {
    j = i;
    while (j--) {
      if (list->data[j] == list->data[i]) break;
    }
    if (j < 0) {
      if (!FUN_004e8440(list->data[i], stream)) return 0;
    }
  }
  return list;
}
