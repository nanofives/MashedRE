// FLAGS: /O2 /Ob1 /G5
/* 0x005bbdb0: 88.2% (regs 91.8%, ops 100.0%, 85/85 ins), source from the listing (PORT-TOP-AUDIO-5AEF30-KNOB, 2026-11-16). The frame is
   two blocks: 0x12 bytes at [esp] (zeroed as 4 dwords + 1 word, 0x005bbdb9..0x005bbde7), 0x24 bytes at +0x14 (first dword 0x24 at
   0x005bbe4a), passed to the vtable +0xc call (0x005bbe9a). Residual: eax / edx swap between param_5 and the block address
   (0x005bbe2b / 0x005bbe42). Previous source (18.8%): D:/MashedWT/PORT-TOP-AUDIO-5AEF30-KNOB_data/005bbdb0_before.c. */
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
void *memset(void *p0, int p1, unsigned int p2);
#pragma intrinsic(memset)
MGLOBALA(uint, DAT_005d099c, 4, 0x005d099c);
MGLOBALA(uint, DAT_005d1144, 4, 0x005d1144);
int FUN_005bbed0(int **p0, int p1, int p2);
typedef int (__stdcall *PFN1_005bbdb0)(int *);
typedef int (__stdcall *PFN4_005bbdb0)(int *, void *, int **, int);
/* the two stack blocks of 0x005bbdb0 (offsets from the listing; field names are offsets) */
struct A_005bbdb0 { ushort h0; ushort h2; uint d4; uint d8; ushort hc; ushort he; ushort h10; };
struct B_005bbdb0 { uint d0; uint d4; uint d8; uint dc; struct A_005bbdb0 *p10; uint g[4]; };
struct L_005bbdb0 { struct A_005bbdb0 a; struct B_005bbdb0 b; };
// MATCH: 0x005bbdb0 FUN_005bbdb0
int FUN_005bbdb0(int *param_1, uchar *param_2, int param_3, int param_4, uint param_5, int **param_6)
{
  struct L_005bbdb0 l;
  uint *g;

  param_6[0] = 0;
  param_6[1] = 0;
  param_6[2] = 0;
  memset(&l.a, 0, 0x12);
  l.a.h2 = param_2[0xd];
  l.a.he = param_2[0xc];
  l.a.d4 = *(uint *)param_2;
  l.a.hc = (ushort)(l.a.he >> 3) * l.a.h2;
  l.a.h0 = 1;
  l.a.d8 = l.a.hc * l.a.d4;
  l.a.h10 = 0x12;
  if (param_3 != 0) {
    param_5 |= 0x20010;
  }
  l.b.d0 = 0x24;
  l.b.d4 = param_5;
  l.b.d8 = *(uint *)(param_2 + 8);
  l.b.dc = 0;
  l.b.p10 = &l.a;
  g = DAT_005d099c;
  if ((param_5 & 1) == 0) {
    g = DAT_005d1144;
  }
  l.b.g[0] = g[0];
  l.b.g[1] = g[1];
  l.b.g[2] = g[2];
  l.b.g[3] = g[3];
  if ((*(PFN4_005bbdb0 *)(*param_1 + 0xc))(param_1, &l.b, param_6, 0) >= 0) {
    if (FUN_005bbed0(param_6, param_3, param_4) != 0) {
      return 1;
    }
    (*(PFN1_005bbdb0 *)(**param_6 + 8))(*param_6);
  }
  return 0;
}
