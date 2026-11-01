// FLAGS: /O2 /GS /Ob1
/* 0x0041f710 FUN_0041f710 (88 B): 100.0% exact, regs 100.0%, whole symbol BYTE-IDENTICAL.
   Raw g2s was 13.6% (in_EAX). The record pointer arrives in EAX: the only caller FUN_00420870 does mov eax,esi
   at 0x004209ed before call 0x0041f710. Static-function convention (pilot finding 1): FUN_0041f710 is static,
   defined before a caller. caller_00420870 is a stand-in for that caller (not matched).
   The 0x8c-byte local is a struct (12-byte vector copied as a struct from +0x258, then +0x264, +0x290 + 0xff00,
   three zeros, 0x6c bytes unwritten), passed by address to FUN_00484cf0. An int pad keeps /GS from adding a cookie.
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
#ifdef __cplusplus
#define MATCH_STATIC
#else
#define MATCH_STATIC static
#endif
typedef struct { int x, y, z; } V3i;
typedef struct {
  V3i v;
  int a;
  int b;
  int c, d, e;
  int pad[0x1b];
} Msg8c;

void FUN_00484cf0(Msg8c *m);

// MATCH: 0x0041f710 FUN_0041f710
MATCH_STATIC void FUN_0041f710(char *p)
{
  Msg8c m;
  m.v = *(V3i *)(p + 0x258);
  m.a = *(int *)(p + 0x264);
  m.b = *(int *)(p + 0x290) + 0xff00;
  m.c = 0;
  m.d = 0;
  m.e = 0;
  FUN_00484cf0(&m);
}

#ifndef __cplusplus
void caller_00420870(char *p)
{
  FUN_0041f710(p);
}
#endif
