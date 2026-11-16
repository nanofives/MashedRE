// FLAGS: /O2 /Ob1 /G5
/* 0x005bad30: 94.4% (regs 94.4%, 142/142 ins), source from the listing (PORT-TOP-AUDIO-5AEF30-KNOB, 2026-11-16). Ghidra's shape (33.8%,
   unaff_EBP / stack0xffffff70) mis-tracked the frame: the locals are one 0x8c frame, [esp] = the pointer 0x005c1920 writes, +4 a
   3-dword block, +0x10 a 0x1c-byte block, +0x2c the 0x60-byte block the vtable +0x10 call fills. Residual: the two 16-bit clamps
   (0x005badd4 / 0x005bae02) keep the 0x80 block as the fall-through (jbe to the and-block), this compile inverts it (ja, ja). Previous
   source: D:/MashedWT/PORT-TOP-AUDIO-5AEF30-KNOB_data/005bad30_before.c. 0x005c1920 is jmp [0x5cc01c], the DSOUND.dll ordinal 11 import. */
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
typedef void *HWND;
typedef int (__stdcall *PFN1_005bad30)(int *);
typedef int (__stdcall *PFN2_005bad30)(int *, uint *);
typedef int (__stdcall *PFN3_005bad30)(int *, HWND, int);
void *memset(void *p0, int p1, unsigned int p2);
#pragma intrinsic(memset)
MGLOBAL(undefined1, DAT_005e6424, 0x005e6424);
int __stdcall FUN_005c1920(int p0, int **p1, int p2);
int FUN_005bac00(uint *p0);
int FUN_005bbdb0(int *p0, uint *p1, int p2, int p3, uint p4, uint *p5);
void FUN_005bbf30(uint *p0);
__declspec(dllimport) HWND __stdcall GetForegroundWindow(void);
struct L_005bad30 { int *ds; uint buf[3]; uint s[7]; uint caps[0x18]; };
// MATCH: 0x005bad30 FUN_005bad30
undefined4 FUN_005bad30(undefined4 param_1, uint *param_2)
{
  struct L_005bad30 l;
  uint v;
  uint u;
  uint w;

  param_2[0] = 0;
  param_2[1] = 0;
  param_2[5] = 48000;
  *(ushort *)(param_2 + 6) = 0x10;
  param_2[2] = 0;
  param_2[3] = 1;
  param_2[4] = 0;
  param_2[8] = 0;
  param_2[9] = 1;
  param_2[7] = 0xa0;
  if (FUN_005c1920(0, &l.ds, 0) >= 0) {
  memset(l.caps, 0, sizeof(l.caps));
  l.caps[0] = 0x60;
  if ((*(PFN2_005bad30 *)(*l.ds + 0x10))(l.ds, l.caps) >= 0) {
    if (l.caps[8] != 0) {
      u = param_2[5];
      if (u < (v = l.caps[2]) || (v = l.caps[3], u > v)) {
        param_2[5] = v;
      }
    }
    v = l.caps[0xf];
    if (v != 0) {
      param_2[4] = 0;
      if (v > 0xffff || (ushort)v > 0x80) {
        w = 0x80;
      } else {
        w = v & 0xffff;
      }
      *(ushort *)((char *)param_2 + 0x1a) = (ushort)w;
    } else {
      v = l.caps[9];
      if (v != 0) {
        if (v > 0xffff || (ushort)v > 0x80) {
          w = 0x80;
        } else {
          w = v & 0xffff;
        }
        *(ushort *)((char *)param_2 + 0x1a) = (ushort)w;
      } else {
        param_2[2] = 1;
        *(ushort *)((char *)param_2 + 0x1a) = 0x20;
      }
      param_2[4] = 1;
    }
    (*(PFN3_005bad30 *)(*l.ds + 0x18))(l.ds, GetForegroundWindow(), 2);
    ((uchar *)l.s)[0xc] = *(uchar *)(param_2 + 6);
    l.s[0] = param_2[5];
    ((uchar *)l.s)[0x18] = 0;
    ((uchar *)l.s)[0x19] = 0;
    l.s[4] = 0;
    l.s[5] = 0;
    l.s[1] = (uint)&DAT_005e6424;
    l.s[2] = 4;
    ((uchar *)l.s)[0xd] = 1;
    l.buf[0] = 0;
    l.buf[1] = 0;
    l.buf[2] = 0;
    if (FUN_005bbdb0(l.ds, l.s, 0, 1, 0x200b2, l.buf) != 0) {
      if (FUN_005bac00(l.buf) != 0) {
        param_2[3] = 1;
      }
      FUN_005bbf30(l.buf);
    }
  }
  (*(PFN1_005bad30 *)(*l.ds + 8))(l.ds);
  return param_1;
  }
  *(ushort *)((char *)param_2 + 0x1a) = 0;
  return param_1;
}
