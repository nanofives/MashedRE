// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR06 2026-11-08: written from the original disassembly (Ghidra-missed, no pc_funcs.csv row) */
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
/* Ghidra-missed (no pc_funcs.csv row). Original body 0x00536780..0x0053693b (443 B, ret then 90 padding).
   Called at 0x00536337. q = *(param_1 + DAT_007dc57c) (0x00536784..0x0053678a). Writes callback addresses into
   q+0x24 (0x0053678d), q+0x28 (0x005367b0..0x0053685e), q+0x2c (0x00536878..0x005368c8), q+0x30
   (0x005368df..0x00536910), q+0x38 (0x0053691d), q+0x34 (0x0053692b / 0x00536933), selected by bits of the dword
   at q+0xa0. */
MGLOBAL(int, DAT_007dc57c, 0x007dc57c);
extern void FUN_00537fb0(void);
extern void FUN_005379c0(void);
extern void FUN_00537890(void);
extern void FUN_00537610(void);
extern void FUN_00536fe0(void);
extern void FUN_005371f0(void);
extern void FUN_00536c90(void);
extern void FUN_00537460(void);
extern void FUN_00536e90(void);
extern void FUN_00536ad0(void);
extern void FUN_00536940(void);
extern void FUN_00537d70(void);
extern void FUN_00537de0(void);
extern void FUN_00537e60(void);
extern void FUN_00537ed0(void);
extern void FUN_00537b20(void);
extern void FUN_00537b80(void);
extern void FUN_00537c40(void);
extern void FUN_00537d20(void);
extern void FUN_00538300(void);
extern void FUN_00537f40(void);
typedef void (*PrtCB)(void);
typedef struct {
  char pad0[0x24];
  PrtCB cb24;
  PrtCB cb28;
  PrtCB cb2c;
  PrtCB cb30;
  PrtCB cb34;
  PrtCB cb38;
  char pad3c[0xa0 - 0x3c];
  unsigned int flags;
} PrtRender;
// MATCH: 0x00536780 FUN_00536780
void FUN_00536780(char *param_1)
{
  PrtRender *q;

  q = *(PrtRender **)(param_1 + DAT_007dc57c);
  q->cb24 = FUN_00537fb0;
  if ((q->flags & 8) == 8) {
    if ((q->flags & 0x1000000) == 0x1000000)
      q->cb28 = FUN_005379c0;
    else
      q->cb28 = FUN_00537890;
  } else if ((q->flags & 0x8000) == 0x8000) {
    if ((q->flags & 0x1000000) == 0x1000000)
      q->cb28 = FUN_00536ad0;
    else
      q->cb28 = FUN_00536940;
  } else if ((q->flags & 0x20) == 0x20) {
    if ((q->flags & 4) == 4) {
      if ((q->flags & 0x1000000) == 0x1000000)
        q->cb28 = FUN_00537610;
      else
        q->cb28 = FUN_00536fe0;
    } else {
      if ((q->flags & 0x1000000) == 0x1000000)
        q->cb28 = FUN_005371f0;
      else
        q->cb28 = FUN_00536c90;
    }
  } else if ((q->flags & 4) == 4) {
    if ((q->flags & 0x1000000) == 0x1000000)
      q->cb28 = FUN_00537460;
    else
      q->cb28 = FUN_00536e90;
  } else {
    if ((q->flags & 0x1000000) == 0x1000000)
      q->cb28 = FUN_00536ad0;
    else
      q->cb28 = FUN_00536940;
  }

  if ((q->flags & 0x80) == 0x80)
    q->cb2c = FUN_00537d70;
  else if ((q->flags & 0x100) == 0x100)
    q->cb2c = FUN_00537de0;
  else if ((q->flags & 0x80000) == 0x80000)
    q->cb2c = FUN_00537e60;
  else if ((q->flags & 0x100000) == 0x100000)
    q->cb2c = FUN_00537ed0;
  else
    q->cb2c = 0;

  if ((q->flags & 2) == 2)
    q->cb30 = FUN_00537b20;
  else if ((q->flags & 0x40) == 0x40)
    q->cb30 = FUN_00537b80;
  else if ((q->flags & 0x40000) == 0x40000)
    q->cb30 = FUN_00537c40;
  else
    q->cb30 = FUN_00537d20;

  q->cb38 = FUN_00538300;
  if ((q->flags & 0x10) == 0x10)
    q->cb34 = FUN_00537f40;
  else
    q->cb34 = 0;
}
