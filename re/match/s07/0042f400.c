// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042f400: 100% by hand (raw 17.4%). /GS frame: char buf[8] + cookie. FUN_004a2b60 is a cdecl
   sprintf-shaped varargs call with the "%d" string at 0x005cd794, strlen is the intrinsic (#pragma intrinsic).
   The loop is for (i < 3) switch (i) with one short v kept across iterations (cases that do not set it store
   the previous value, which is the stale edx Ghidra shows as uVar5 = local_c). C++: warning C4162 on the strlen
   pragma only, no error. */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
unsigned int strlen(const char *s);
#pragma intrinsic(strlen)
typedef struct { short f0; short d0; short d1; short d2; } Rec0042f400;
MGLOBALA(char, DAT_005cd794, 3, 0x005cd794);
Rec0042f400 *FUN_00427780(int p0);
int FUN_004a2b60(char *p0, const char *p1, ...);
// MATCH: 0x0042f400 FUN_0042f400
void FUN_0042f400(int param_1, int param_2)
{
  Rec0042f400 *r;
  char buf[8];
  int len;
  int i;
  short v;
  r = FUN_00427780(param_1);
  FUN_004a2b60(buf, DAT_005cd794, param_2);
  len = strlen(buf);
  for (i = 0; i < 3; i++) {
    switch (i) {
    case 0:
      if (len == 3) {
        v = buf[0];
      } else {
        v = '0';
      }
      r->d0 = v;
      break;
    case 1:
      if (len == 3) {
        v = buf[1];
      } else if (len == 2) {
        v = buf[0];
      } else if (len == 1) {
        v = '0';
      }
      r->d1 = v;
      break;
    case 2:
      if (len == 3) {
        v = buf[2];
      } else if (len == 2) {
        v = buf[1];
      } else if (len == 1) {
        v = buf[0];
      }
      r->d2 = v;
      break;
    }
  }
}
