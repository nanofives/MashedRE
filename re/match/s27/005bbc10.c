// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005bbc10: 44.4% (regs 53.0%). Hand: 6 params (Ghidra missed the out pointer), WAVEFORMATEX-style 0x12-byte blocks as Blk12{uint d[4]; ushort e} with W16/W32 offset macros, explicit zero stores placed after the obj check / before the +0x14 call, stdcall vtable calls via VT macro. Residual (b): original zero-stores interleave with the field stores and the block lands in the pushes of the first call; bits loop flag reuse (add word,-8 / je) and the 3 Release epilogues' layout not reproduced. */
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
typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef int (__stdcall *vf1)(void *);
typedef int (__stdcall *vf2)(void *, void *);
typedef int (__stdcall *vf4)(void *, void *, int, int);
#define VT(o, off) (*(int *)(*(int *)(o) + (off)))
/* 0x12-byte WAVEFORMATEX-style blocks: +0 tag, +2 channels, +4 rate, +8 avg, +0xc align, +0xe bits, +0x10 cb */
typedef struct Blk12 { uint d[4]; ushort e; } Blk12;
#define W16(b, o) (*(ushort *)((byte *)&(b) + (o)))
#define W32(b, o) (*(uint *)((byte *)&(b) + (o)))
int *FUN_005bbd50(int p0, int p1);
// MATCH: 0x005bbc10 FUN_005bbc10
int FUN_005bbc10(int param_1, byte *param_2, int *param_3, byte *param_4, int param_5, int **param_6)
{
  int *piVar2;
  int iVar3;
  Blk12 f;
  Blk12 g;

  piVar2 = FUN_005bbd50(param_1, param_5);
  if (piVar2 == 0) {
    return 0;
  }
  f.d[0] = 0; f.d[1] = 0; f.d[2] = 0; f.d[3] = 0; f.e = 0;
  W16(f, 2) = *param_2;
  W32(f, 4) = *param_3;
  W16(f, 0xe) = *param_4;
  W16(f, 0xc) = (ushort)((*param_4 >> 3) * W16(f, 2));
  W16(f, 0) = 1;
  W16(f, 0x10) = 0x12;
  W32(f, 8) = W16(f, 0xc) * W32(f, 4);
  do {
    iVar3 = ((vf2)VT(piVar2, 0x38))(piVar2, &f);
    W16(f, 0xe) = W16(f, 0xe) - 8;
    if (W16(f, 0xe) == 0) {
      if (iVar3 < 0) {
        ((vf1)VT(piVar2, 8))(piVar2);
        return 0;
      }
      break;
    }
  } while (iVar3 < 0);
  g.d[0] = 0; g.d[1] = 0; g.d[2] = 0; g.d[3] = 0; g.e = 0;
  iVar3 = ((vf4)VT(piVar2, 0x14))(piVar2, &g, 0x12, 0);
  if (iVar3 < 0) {
    ((vf1)VT(piVar2, 8))(piVar2);
    return 0;
  }
  *param_3 = W32(g, 4);
  *param_4 = (byte)W16(g, 0xe);
  *param_2 = (byte)W16(g, 2);
  if (param_6 != 0) {
    *param_6 = piVar2;
    return 1;
  }
  ((vf1)VT(piVar2, 8))(piVar2);
  return 1;
}
