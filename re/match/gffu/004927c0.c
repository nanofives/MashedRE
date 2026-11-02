// FLAGS: /O2 /GS /Ob1
/* MATCH-GFFU 0x004927c0 (500 B): 100%. Raw Ghidra form (decomp_pc.py, pool0) scored 26.3%. Source form: a 12-entry
   local table (0x004927cd..0x0049280d), the step word 0x00771980 clamped to 0..11 (0x00492811..0x00492818), eight
   (byte 0x007f10xx != 0 && byte 0x007f15xx == 0) checks that set r = 2, a switch on table[step] (jump table
   0x004929b4: case 0 -> 0x004928dc, 1 -> 0x00492903, 2 -> 0x00492951, 3 -> 0x0049292a) where the slot's own check
   sets r = 1 and the step check jumps to the increment (0x00492971), then r == 2 -> step 0 and return (0x004929a7),
   and at step 12 a 0x9c-dword fill of 0x007f0a40 with 2 and 0x007f0f2c = 1 (0x00492980..0x00492991). What moved the
   last 7%: case 3 written before case 2 (block order follows the source). Residual: none. */
/* ---- MATCH-GFFU preamble (same text in every re/match/gffu source; not part of any function body) ----
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

MGLOBAL(int, DAT_00771980, 0x00771980);
MGLOBAL(char, DAT_007f1044, 0x007f1044);
MGLOBAL(char, DAT_007f1045, 0x007f1045);
MGLOBAL(char, DAT_007f1046, 0x007f1046);
MGLOBAL(char, DAT_007f1047, 0x007f1047);
MGLOBAL(char, DAT_007f10dc, 0x007f10dc);
MGLOBAL(char, DAT_007f10dd, 0x007f10dd);
MGLOBAL(char, DAT_007f10de, 0x007f10de);
MGLOBAL(char, DAT_007f10df, 0x007f10df);
MGLOBAL(char, DAT_007f1504, 0x007f1504);
MGLOBAL(char, DAT_007f1505, 0x007f1505);
MGLOBAL(char, DAT_007f1506, 0x007f1506);
MGLOBAL(char, DAT_007f1507, 0x007f1507);
MGLOBAL(char, DAT_007f159c, 0x007f159c);
MGLOBAL(char, DAT_007f159d, 0x007f159d);
MGLOBAL(char, DAT_007f159e, 0x007f159e);
MGLOBAL(char, DAT_007f159f, 0x007f159f);
MGLOBALA(int, DAT_007f0a40, 0x9c, 0x007f0a40);
MGLOBAL(int, DAT_007f0f2c, 0x007f0f2c);
#define ON(a, b) ((a) != 0 && (b) == 0)
// MATCH: 0x004927c0 FUN_004927c0
void FUN_004927c0(void)
{
  int seq[12];
  int r;
  int i;
  int *p;

  seq[2] = 1;
  seq[6] = 1;
  seq[9] = 1;
  seq[11] = 1;
  seq[0] = 0;
  seq[1] = 2;
  seq[3] = 3;
  seq[4] = 0;
  seq[5] = 3;
  seq[7] = 2;
  seq[8] = 0;
  seq[10] = 0;
  if (DAT_00771980 < 0 || DAT_00771980 > 11) {
    DAT_00771980 = 0;
  }
  r = 0;
  if (ON(DAT_007f1046, DAT_007f1506)) r = 2;
  if (ON(DAT_007f10de, DAT_007f159e)) r = 2;
  if (ON(DAT_007f1047, DAT_007f1507)) r = 2;
  if (ON(DAT_007f10df, DAT_007f159f)) r = 2;
  if (ON(DAT_007f1044, DAT_007f1504)) r = 2;
  if (ON(DAT_007f10dc, DAT_007f159c)) r = 2;
  if (ON(DAT_007f1045, DAT_007f1505)) r = 2;
  if (ON(DAT_007f10dd, DAT_007f159d)) r = 2;
  switch (seq[DAT_00771980]) {
  case 0:
    if (ON(DAT_007f1046, DAT_007f1506)) r = 1;
    if (ON(DAT_007f10de, DAT_007f159e)) goto step;
    break;
  case 1:
    if (ON(DAT_007f1047, DAT_007f1507)) r = 1;
    if (ON(DAT_007f10df, DAT_007f159f)) goto step;
    break;
  case 3:
    if (ON(DAT_007f1044, DAT_007f1504)) r = 1;
    if (ON(DAT_007f10dc, DAT_007f159c)) goto step;
    break;
  case 2:
    if (ON(DAT_007f1045, DAT_007f1505)) r = 1;
    if (ON(DAT_007f10dd, DAT_007f159d)) goto step;
    break;
  }
  if (r == 1) {
step:
    DAT_00771980++;
  } else if (r == 2) {
    DAT_00771980 = 0;
    return;
  }
  if (DAT_00771980 == 12) {
    p = DAT_007f0a40;
    for (i = 0x9c; i != 0; i--) {
      *p++ = 2;
    }
    DAT_007f0f2c = 1;
  }
}
