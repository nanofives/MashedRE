// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x004623e0 (h6): 100.0% (regs 100%). Raw 42.2%. Typed the globals as float, DAT_0068f640 as an
   8-byte {handle,pad} array, 5a6dc0 takes 4 args (add esp,0x10). Moved to 100% by reading the table entry
   at every use (no handle local) and writing the tail as one clamp if/else-if with a single call (the compiler
   tail-duplicates the call). Residual: none. */
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
typedef struct { int h; int pad; } S10Voice;
MGLOBAL(float, DAT_006900d4, 0x006900d4);
MGLOBAL(float, DAT_0068fc8c, 0x0068fc8c);
MGLOBAL(int, DAT_00605db8, 0x00605db8);
MGLOBAL(float, DAT_0068fc94, 0x0068fc94);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBALA(S10Voice, DAT_0068f640, 1, 0x0068f640);
void FUN_0045e0f0(int p0, float p1);
void FUN_005a66d0(int p0, int p1);
void FUN_005a6dc0(int p0, int p1, int p2, float p3);
// MATCH: 0x004623e0 FUN_004623e0
void FUN_004623e0(void)
{
  int ch;

  DAT_0068fc94 = DAT_0068fc8c * DAT_006900d4;
  ch = DAT_00605db8;
  if (DAT_0068fc94 <= DAT_005d757c) {
    FUN_005a66d0(DAT_0068f640[ch].h, 0);
    return;
  }
  if ((*(byte *)(DAT_0068f640[ch].h + 0x34) & 2) == 0) {
    FUN_005a66d0(DAT_0068f640[ch].h, 1);
  }
  *(float *)(DAT_0068f640[ch].h + 0x3c) = 22050.0f;
  if (*(int *)(DAT_0068f640[ch].h + 0x30) != 0) {
    FUN_005a6dc0(*(int *)(DAT_0068f640[ch].h + 0x30), 4, 1, 22050.0f);
  }
  if (DAT_0068fc94 > DAT_005cc320) {
    DAT_0068fc94 = 1.0f;
  }
  else if (DAT_0068fc94 < DAT_005d757c) {
    DAT_0068fc94 = 0.0f;
  }
  FUN_0045e0f0(ch, DAT_0068fc94);
}
