// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f0810 (h1): 100% under /O2 /Ob1 /G5. Plugin open on RwGlobals (DAT_007d3ff8) + DAT_00911ae0:
   clears the 9 dwords at +0x3c..+0x5c, then calls 0x004d8560, 0x004f43f0, 0x004f4340 in turn; on any 0 it calls
   0x004f43b0, 0x004f4470, 0x0045b350 and returns 0. The raw nested ifs scored 34% (0 in eax, test eax,eax). The key
   was one status variable chained through the three calls (`r = f(); if (r) r = g(); ...`): 9176 then keeps 0 in esi
   for the stores and `cmp eax, esi`. Permuter (140 compiles) and the flag sweep did not find it. */
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
typedef struct { char pad[0x3c]; void *a[9]; } Glob_004f0810;
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00911ae0, 0x00911ae0);
void FUN_0045b350(void);
int FUN_004d8560(void);
int FUN_004f4340(void);
void FUN_004f43b0(void);
int FUN_004f43f0(void);
void FUN_004f4470(void);
// MATCH: 0x004f0810 FUN_004f0810
int FUN_004f0810(void)
{
  int r;

  ((Glob_004f0810 *)(DAT_007d3ff8 + DAT_00911ae0))->a[0] = 0;
  ((Glob_004f0810 *)(DAT_007d3ff8 + DAT_00911ae0))->a[1] = 0;
  ((Glob_004f0810 *)(DAT_007d3ff8 + DAT_00911ae0))->a[2] = 0;
  ((Glob_004f0810 *)(DAT_007d3ff8 + DAT_00911ae0))->a[3] = 0;
  ((Glob_004f0810 *)(DAT_007d3ff8 + DAT_00911ae0))->a[4] = 0;
  ((Glob_004f0810 *)(DAT_007d3ff8 + DAT_00911ae0))->a[5] = 0;
  ((Glob_004f0810 *)(DAT_007d3ff8 + DAT_00911ae0))->a[6] = 0;
  ((Glob_004f0810 *)(DAT_007d3ff8 + DAT_00911ae0))->a[7] = 0;
  ((Glob_004f0810 *)(DAT_007d3ff8 + DAT_00911ae0))->a[8] = 0;
  r = FUN_004d8560();
  if (r) {
    r = FUN_004f43f0();
  }
  if (r) {
    r = FUN_004f4340();
  }
  if (r) {
    return 1;
  }
  FUN_004f43b0();
  FUN_004f4470();
  FUN_0045b350();
  return 0;
}
