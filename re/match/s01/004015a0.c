// FLAGS: /O2 /GS /Ob1
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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
/* a1 0x004015a0 (126 B Ghidra, 129 B real). Best: BYTES-OK (whole symbol, relocations masked); score.py shows
   94.0% only because Ghidra size 126 stops before the last 3 epilogue instructions (pop ebp/pop ebx/ret).
   Start (g2s) 47.9%. Fix: 13-entry struct array at 0x00636578, stride 0x68, ends at 0x00636ac0 (= &DAT_00636ac0),
   walked with an INDEX loop (n<13): 9176 strength-reduces it to the pointer + `cmp esi,0x636ac0 / jl` form.
   A pointer loop gave a down-counter. Residual: none (class c, Ghidra boundary). */
typedef struct Slot {
  int clump;        /* +0x00 */
  int f04, f08, f0c;
  int pad10[9];     /* +0x10 */
  float scale;      /* +0x34 */
  int f38;          /* +0x38 */
  int pad3c;
  int a[4];         /* +0x40 */
  int b[6];         /* +0x50 */
} Slot;             /* 0x68 */
MGLOBALA(Slot, DAT_00636578, 13, 0x00636578);
MGLOBAL(Slot *, DAT_00636ac0, 0x00636ac0);
int RpClumpDestroy(int c);
// MATCH: 0x004015a0 FUN_004015a0
void FUN_004015a0(void)
{
  int n, i;
  DAT_00636ac0 = DAT_00636578;
  for (n = 0; n < 13; n++) {
    for (i = 0; i < 4; i++) RpClumpDestroy(DAT_00636578[n].a[i]);
    for (i = 0; i < 6; i++) RpClumpDestroy(DAT_00636578[n].b[i]);
    if (DAT_00636578[n].clump != 0) {
      RpClumpDestroy(DAT_00636578[n].clump);
      DAT_00636578[n].clump = 0;
    }
    DAT_00636578[n].clump = 0;
    DAT_00636578[n].f04 = 0;
    DAT_00636578[n].f08 = 0;
    DAT_00636578[n].f0c = 0;
    DAT_00636578[n].scale = 1.0f;
    DAT_00636578[n].f38 = 0;
  }
}
