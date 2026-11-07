// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 h1 0x005c6cc0 : 40.6% (regs 42, ops 49), 92 vs 170 instructions. By hand from the asm: the repeated 'x = (short)(1 << FUN_005aee20(0x28)) ? (1 << FUN_005aee20(0x28)) & 0xffff : 1' is an inline/macro helper (mk()) expanded 7 times; a leading-bit table lookup at 0x00617ff8 (signed bytes) picks r, ORed as r<<0x1c into [p+0x40]; installs FUN_005c6f50/FUN_005c7070/FUN_005c70d0 at +0x28/+0x34/+0x2c. The original keeps each expansion's constant-1 path as live code (mov edx,1; sar edx,n; movsx) and spills the second-level value, ours folds and merges them (about half the instructions). static __inline helper, test spellings, separate +7/+0x10 adds tried. Residual class (b), expansion shape (likely a macro with the conditional inside the index expression). */

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

MGLOBALA(signed char, DAT_00617ff8, 256, 0x00617ff8);
MGLOBAL(unsigned char, DAT_005e7a4c, 0x005e7a4c);
int FUN_005a9e10(int p0, unsigned char *p1, int p2);
int FUN_005aee20(int p0);
void FUN_005c6f50(void);
void FUN_005c70d0(void);
void FUN_005c7070(void);
static __inline unsigned int mk(void)
{
  if ((short)(1 << FUN_005aee20(0x28)) != 0) {
    return (1 << FUN_005aee20(0x28)) & 0xffff;
  }
  return 1;
}
// MATCH: 0x005c6cc0 FUN_005c6cc0
void FUN_005c6cc0(int param_1)
{
  unsigned int v;
  unsigned int w;
  unsigned int z;
  int r;

  FUN_005a9e10(param_1, &DAT_005e7a4c, 0);
  *(short *)(param_1 + 0x46) = 8;
  *(unsigned int *)(param_1 + 0x40) = 0xb0;
  v = mk();
  if ((v & 0xffff0000) != 0) {
    w = mk();
    if ((w >> 24) != 0) {
      z = mk();
      r = DAT_00617ff8[(int)z >> 24] + 7;
      r += 0x10;
    } else {
      z = mk();
      r = DAT_00617ff8[(int)z >> 16] - 1;
      r += 0x10;
    }
  } else {
    w = mk();
    if ((w & 0xff00) != 0) {
      z = mk();
      r = DAT_00617ff8[(int)z >> 8] + 7;
    } else {
      z = mk();
      r = DAT_00617ff8[z] - 1;
    }
  }
  *(int *)(param_1 + 0x28) = (int)FUN_005c6f50;
  *(int *)(param_1 + 0x34) = (int)FUN_005c7070;
  *(unsigned int *)(param_1 + 0x40) = *(unsigned int *)(param_1 + 0x40) | (unsigned int)(r << 0x1c);
  *(int *)(param_1 + 0x2c) = (int)FUN_005c70d0;
  *(short *)(param_1 + 0x44) = 0;
}




