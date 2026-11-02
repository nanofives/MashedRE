// FLAGS: /O2 /GS /Ob1
/* 0x0041f880 FUN_0041f880 (112 B): 100.0% exact, regs 100.0%.
   Raw g2s was 56.1% (unaff_EDI). The record arrives in EDI: the caller FUN_00420870 sets mov edi,esi at
   0x004209e1 and EDI still holds the record at the call 0x00420a03. Static-function convention (pilot finding 1):
   FUN_0041f880 is static, defined before a caller; Caller_00420870 is a stand-in (not matched), same shape as the
   merged S04 0x0041f4f0 file. Body is an if / else-if chain on the flags word +0x294 (0x400, 0x10 && 0x20, 0x10,
   0x20) choosing (k, id), id stored to +0x28c, then FUN_00422aa0(+0x290, k) when DAT_00636acc == 0.
   Residual: none. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
typedef struct {
  char u000[0x28c];
  int x28c;                      /* +0x28c */
  unsigned int x290;             /* +0x290 */
  unsigned int x294;             /* +0x294 */
} CarRec;
MGLOBAL(int, DAT_00636acc, 0x00636acc);
void FUN_00422aa0(unsigned int a, int b);

// MATCH: 0x0041f880 FUN_0041f880
static void FUN_0041f880(CarRec *r)
{
  unsigned int f = r->x294;
  int k = 0;
  int id = 0x3b;
  if (f & 0x400)
    id = 0x40;
  else if ((f & 0x10) && (f & 0x20)) {
    k = 3;
    id = 0x3e;
  } else if (f & 0x10) {
    k = 1;
    id = 0x3c;
  } else if (f & 0x20) {
    k = 2;
    id = 0x3d;
  }
  r->x28c = id;
  if (DAT_00636acc == 0)
    FUN_00422aa0(r->x290, k);
}

/* stand-in caller shaped like the call site in 0x00420870 (not matched here) */
void Caller_00420870(CarRec *r, int a)
{
  FUN_0041f880(r);
  FUN_0041f880(r);
}
