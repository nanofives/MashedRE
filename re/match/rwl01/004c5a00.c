// FLAGS: /O2 /Ob1 /G5
/* RWC-MATCH-S01 (2026-11-12): 100%, byte-identical under ddk and rw9044, relocheck OK. Store order f0, f4, f10, f30, f54, then +0x50 cleared as a dword, then two read-modify-write mask updates of that dword (& ~0xff00 | 0x1100, then & ~0xff | 1). Permuter first took it 89.7 -> 93.1. */
/* MATCH-RWL01 0x004c5a00: best 89.7% (regs 89.7%, ops 100%), class (b). Struct view of the object; the remaining diff is only the order of the dword [+0x50] store and byte [+0x51]/[+0x30] stores (permuter, 254 compiles; manual store orders gave 72-86%). */
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d4054, 0x007d4054);
MGLOBAL(undefined4, DAT_00618138, 0x00618138);
typedef struct S5A00 {
  undefined4 f0;
  undefined4 f4;
  undefined1 pad8[8];
  undefined1 f10;
  undefined1 pad11[0x1f];
  undefined1 f30;
  undefined1 pad31[0x1f];
  undefined1 f50;
  undefined1 f51;
  undefined1 pad52[2];
  undefined4 f54;
} S5A00;
int FUN_004d8000(int p0, undefined4 p1);
// MATCH: 0x004c5a00 FUN_004c5a00
undefined4 * FUN_004c5a00(undefined4 param_1)
{
  S5A00 *p;
  p = (S5A00 *)(**(code **)(DAT_007d3ff8 + 0x118))(*(undefined4 *)(DAT_007d4054 + 8 + DAT_007d3ff8),0x30006);
  if (p != (S5A00 *)0x0) {
    p->f0 = param_1;
    p->f4 = 0;
    p->f10 = 0;
    p->f30 = 0;
    p->f54 = 1;
    *(uint *)&p->f50 = 0;
    *(uint *)&p->f50 = (*(uint *)&p->f50 & ~0xff00u) | 0x1100;
    *(uint *)&p->f50 = (*(uint *)&p->f50 & ~0xffu) | 1;
    FUN_004d8000((int)&DAT_00618138,(undefined4)p);
  }
  return (undefined4 *)p;
}
