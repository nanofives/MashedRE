// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00461e90 (h5): best 95.8% (regs 95.8%, ops 98.0%), from 13.3% raw. Nested switch: outer on param_1
   (jump table + byte index), inner on the colour param_2, every leaf `*p3 = a; *p4 = b; return r;`. Outer case
   order = code order (0x1e,3,0x27,2,0x26,0xb,0x19,0,0x24,0x22,0x25,0x21,0x1a), inner cases ascending.
   Moves: case 0x24 needs a 4th inner case whose body equals the default (0,2,0): the compiler drops its compare
   but the 4-value count gives the original's binary tree, and the extra block shifts which identical return
   blocks get tail-merged (57.8% -> 95.8%). The value of that case is not recoverable (any value between
   0xff646464 and 0xffb48080 gives the same bytes; 0xff808080 is a placeholder).
   Residual: case 0x21 and 0x1a 0xff968080 leaves get their own (0,1,1) block instead of jumping to case 0xb's.
   Tried: hidden default-equal cases in 0x19/0x26/0/0x1e/3 and hidden outer cases (all worse), reversed inner
   order, single-exit locals form (23.8%). Class (b): tail-merge choice follows block numbering, the source has
   some other hidden shape not found. */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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

// MATCH: 0x00461e90 FUN_00461e90
int FUN_00461e90(int param_1, int param_2, int *param_3, int *param_4)
{
  switch (param_1) {
  case 0x1e:
    switch (param_2) {
    case 0xffb48080:
      *param_3 = 2;
      *param_4 = 1;
      return 1;
    case 0xffc88080:
      *param_3 = 0;
      *param_4 = 1;
      return 1;
    default:
      *param_3 = 3;
      *param_4 = 3;
      return 0;
    }
  case 0x3:
    switch (param_2) {
    case 0xff804000:
      *param_3 = 1;
      *param_4 = 0;
      return 0;
    case 0xffdc8080:
      *param_3 = 2;
      *param_4 = 2;
      return 1;
    default:
      *param_3 = 3;
      *param_4 = 3;
      return 0;
    }
  case 0x27:
    switch (param_2) {
    case 0xff961e5a:
      *param_3 = 0;
      *param_4 = 0;
      return 1;
    case 0xffb4b4b4:
      *param_3 = 3;
      *param_4 = 2;
      return 1;
    case 0xffffffff:
      *param_3 = 3;
      *param_4 = 3;
      return 1;
    default:
      *param_3 = 3;
      *param_4 = 3;
      return 0;
    }
  case 0x2:
    switch (param_2) {
    case 0xffb48080:
      *param_3 = 2;
      *param_4 = 1;
      return 1;
    case 0xffc88080:
      *param_3 = 0;
      *param_4 = 1;
      return 1;
    case 0xffdc8080:
      *param_3 = 2;
      *param_4 = 2;
      return 1;
    default:
      *param_3 = 2;
      *param_4 = 2;
      return 0;
    }
  case 0x26:
    switch (param_2) {
    case 0xff968080:
      *param_3 = 0;
      *param_4 = 1;
      return 1;
    default:
      *param_3 = 3;
      *param_4 = 3;
      return 0;
    }
  case 0xb:
    switch (param_2) {
    case 0xff0064b4:
      *param_3 = 3;
      *param_4 = 3;
      return 1;
    case 0xff80c800:
      *param_3 = 2;
      *param_4 = 2;
      return 1;
    case 0xff968080:
      *param_3 = 0;
      *param_4 = 1;
      return 1;
    default:
      *param_3 = 0;
      *param_4 = 1;
      return 0;
    }
  case 0x19:
    switch (param_2) {
    case 0xff1e80b4:
      *param_3 = 1;
      *param_4 = 0;
      return 1;
    case 0xff961e5a:
      *param_3 = 0;
      *param_4 = 0;
      return 1;
    default:
      *param_3 = 0;
      *param_4 = 0;
      return 0;
    }
  case 0x0:
    switch (param_2) {
    case 0xff646464:
      *param_3 = 0;
      *param_4 = 1;
      return 1;
    case 0xff968080:
      *param_3 = 0;
      *param_4 = 1;
      return 1;
    default:
      *param_3 = 0;
      *param_4 = 1;
      return 0;
    }
  case 0x24:
    switch (param_2) {
    case 0xff646464:
      *param_3 = 0;
      *param_4 = 2;
      return 1;
    case 0xff808080: /* placeholder value, see header */
      *param_3 = 0;
      *param_4 = 2;
      return 0;
    case 0xffb48080:
      *param_3 = 1;
      *param_4 = 2;
      return 1;
    case 0xffdc8080:
      *param_3 = 1;
      *param_4 = 1;
      return 0;
    default:
      *param_3 = 0;
      *param_4 = 2;
      return 0;
    }
  case 0x22:
    switch (param_2) {
    case 0xff005ab4:
      *param_3 = 3;
      *param_4 = 2;
      return 1;
    case 0xff0064b4:
      *param_3 = 3;
      *param_4 = 3;
      return 1;
    case 0xff968080:
      *param_3 = 0;
      *param_4 = 1;
      return 1;
    default:
      *param_3 = 2;
      *param_4 = 1;
      return 0;
    }
  case 0x25:
    switch (param_2) {
    case 0xff008000:
      *param_3 = 1;
      *param_4 = 0;
      return 1;
    case 0xff14508c:
      *param_3 = 1;
      *param_4 = 2;
      return 1;
    case 0xff80c800:
      *param_3 = 1;
      *param_4 = 1;
      return 1;
    case 0xff968080:
      *param_3 = 0;
      *param_4 = 1;
      return 1;
    default:
      *param_3 = 0;
      *param_4 = 0;
      return 0;
    }
  case 0x21:
    switch (param_2) {
    case 0xff008000:
      *param_3 = 1;
      *param_4 = 0;
      return 1;
    case 0xff0080c8:
      *param_3 = 0;
      *param_4 = 3;
      return 1;
    case 0xff804040:
      *param_3 = 0;
      *param_4 = 1;
      return 1;
    case 0xff968080:
      *param_3 = 0;
      *param_4 = 1;
      return 1;
    default:
      *param_3 = 0;
      *param_4 = 1;
      return 0;
    }
  case 0x1a:
    switch (param_2) {
    case 0xff80c800:
      *param_3 = 1;
      *param_4 = 1;
      return 1;
    case 0xff968080:
      *param_3 = 0;
      *param_4 = 1;
      return 1;
    case 0xffc88080:
      *param_3 = 2;
      *param_4 = 0;
      return 1;
    default:
      *param_3 = 0;
      *param_4 = 0;
      return 0;
    }
  default:
    *param_3 = 3;
    *param_4 = 3;
    return 0;
  }
}
