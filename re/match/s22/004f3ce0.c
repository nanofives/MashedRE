// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f3ce0 (h3, by hand): 100% (regs 100%) under /O2 /Ob1 /G5. Raw converter 46.5% (goto form).
   Same {int *data; int count;} list as 0x004f3cb0: size = count*4 + 0x10, plus 0xc + FUN_004e8400(e) per entry
   with no earlier duplicate. Duplicate search written as `j = i; while (j--) if (...) break; if (j < 0)`. */
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
int FUN_004e8400(int p0);
// MATCH: 0x004f3ce0 FUN_004f3ce0
int FUN_004f3ce0(S22IntList *l)
{
  int size = l->count * 4 + 0x10;
  int i, j;
  for (i = 0; i < l->count; i++) {
    j = i;
    while (j--) {
      if (l->data[j] == l->data[i]) break;
    }
    if (j < 0) {
      size += 0xc + FUN_004e8400(l->data[i]);
    }
  }
  return size;
}
