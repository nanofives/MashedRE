// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d6910 (h2): 89.1% (regs 92.5%, ops 99.3%). Render-state getter: switch (state) over 1..0x1e
   (jump table at 0x004d6b18, `dec eax; cmp eax,0x1d`) writing one cached global to *value and returning 1;
   state 2 returns the 0x007d6b34 value only when it equals 0x007d6b38, else writes 0 and returns 0; state 0x11
   is a float (fld/fstp); unknown states return 0.
   What moved it: case bodies in the original's emitted order (0xe, 0x10, 0xf, 0x11, 2, 3, 4, 9, 1, 8, 6, 0xa, 0xb,
   7, 0xc, 0xd, 0x14..0x1d, 0x1e; checked with constants in place of the globals: 9178 emits bodies in source
   order) and the float case (raw 88.4%).
   Residual: every case body is the same 4-5 instructions, but the scratch-register pair rotates one step out of
   phase (original case 0xe uses ecx/edx, ours eax/ecx, and so on for all 27 bodies; the float case and the state
   2 tail follow the same shift). Tried unsigned state, int * value, a result variable with break (33%), and the
   permuter (41 compiles, nothing). Class (b): the source detail that sets the rotation phase was not found. */
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
MGLOBAL(int, DAT_007d58b8, 0x007d58b8);
MGLOBAL(int, DAT_007d6ae8, 0x007d6ae8);
MGLOBAL(int, DAT_007d6aec, 0x007d6aec);
MGLOBAL(int, DAT_007d6af0, 0x007d6af0);
MGLOBAL(int, DAT_007d6af4, 0x007d6af4);
MGLOBAL(int, DAT_007d6af8, 0x007d6af8);
MGLOBAL(int, DAT_007d6afc, 0x007d6afc);
MGLOBAL(int, DAT_007d6b00, 0x007d6b00);
MGLOBAL(int, DAT_007d6b04, 0x007d6b04);
MGLOBAL(int, DAT_007d6b08, 0x007d6b08);
MGLOBAL(int, DAT_007d6b0c, 0x007d6b0c);
MGLOBAL(int, DAT_007d6b10, 0x007d6b10);
MGLOBAL(int, DAT_007d6b18, 0x007d6b18);
MGLOBAL(int, DAT_007d6b1c, 0x007d6b1c);
MGLOBAL(int, DAT_007d6b20, 0x007d6b20);
MGLOBAL(float, DAT_007d6b24, 0x007d6b24);
MGLOBAL(int, DAT_007d6b28, 0x007d6b28);
MGLOBAL(int, DAT_007d6b2c, 0x007d6b2c);
MGLOBAL(int, DAT_007d6b30, 0x007d6b30);
MGLOBAL(int, DAT_007d6b34, 0x007d6b34);
MGLOBAL(int, DAT_007d6b38, 0x007d6b38);
MGLOBAL(int, DAT_007d6b3c, 0x007d6b3c);
MGLOBAL(int, DAT_007d6b40, 0x007d6b40);
MGLOBAL(int, DAT_007d6bf0, 0x007d6bf0);
MGLOBAL(int, DAT_007d6bf4, 0x007d6bf4);
MGLOBAL(int, DAT_007d6bf8, 0x007d6bf8);
// MATCH: 0x004d6910 FUN_004d6910
int FUN_004d6910(int state, void *value)
{
  switch (state) {
  case 0xe:
    *(int *)value = DAT_007d6b1c;
    return 1;
  case 0x10:
    *(int *)value = DAT_007d6b20;
    return 1;
  case 0xf:
    *(int *)value = DAT_007d6b28;
    return 1;
  case 0x11:
    *(float *)value = DAT_007d6b24;
    return 1;
  case 0x2:
    if (DAT_007d6b34 == DAT_007d6b38) {
      *(int *)value = DAT_007d6b34;
      return 1;
    }
    *(int *)value = 0;
    return 0;
  case 0x3:
    *(int *)value = DAT_007d6b34;
    return 1;
  case 0x4:
    *(int *)value = DAT_007d6b38;
    return 1;
  case 0x9:
    *(int *)value = DAT_007d6b3c;
    return 1;
  case 0x1:
    *(int *)value = DAT_007d6b30;
    return 1;
  case 0x8:
    *(int *)value = DAT_007d6ae8;
    return 1;
  case 0x6:
    *(int *)value = DAT_007d6aec;
    return 1;
  case 0xa:
    *(int *)value = DAT_007d6bf0;
    return 1;
  case 0xb:
    *(int *)value = DAT_007d6bf4;
    return 1;
  case 0x7:
    *(int *)value = DAT_007d6b2c;
    return 1;
  case 0xc:
    *(int *)value = DAT_007d6b10;
    return 1;
  case 0xd:
    *(int *)value = DAT_007d6b40;
    return 1;
  case 0x14:
    *(int *)value = DAT_007d6b18;
    return 1;
  case 0x15:
    *(int *)value = DAT_007d6af0;
    return 1;
  case 0x16:
    *(int *)value = DAT_007d6af4;
    return 1;
  case 0x17:
    *(int *)value = DAT_007d6af8;
    return 1;
  case 0x18:
    *(int *)value = DAT_007d6afc;
    return 1;
  case 0x19:
    *(int *)value = DAT_007d6b00;
    return 1;
  case 0x1a:
    *(int *)value = DAT_007d6b04;
    return 1;
  case 0x1b:
    *(int *)value = DAT_007d6b08;
    return 1;
  case 0x1c:
    *(int *)value = DAT_007d6b0c;
    return 1;
  case 0x1d:
    *(int *)value = DAT_007d6bf8;
    return 1;
  case 0x1e:
    *(int *)value = DAT_007d58b8;
    return 1;
  }
  return 0;
}
