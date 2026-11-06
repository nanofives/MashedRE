// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00463150 (h6): 100.0% (regs 100%). Raw 46.7%. FUN_00472650 is a cdecl float(float, float)
   (Ghidra had a fastcall ECX arg), globals typed float/float/double, voice table read at every use.
   Residual: none. */
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
MGLOBAL(int, DAT_00605bfc, 0x00605bfc);
MGLOBAL(float, DAT_005ce6b8, 0x005ce6b8);
MGLOBAL(float, DAT_006036c4, 0x006036c4);
MGLOBAL(double, DAT_005ce988, 0x005ce988);
MGLOBALA(S10Voice, DAT_0068f640, 1, 0x0068f640);
void FUN_0045e0f0(int p0, float p1);
float FUN_00472650(float p0, float p1);
void FUN_005a66d0(int p0, int p1);
void FUN_005a6dc0(int p0, int p1, int p2, float p3);
// MATCH: 0x00463150 FUN_00463150
void FUN_00463150(void)
{
  int ch;

  ch = DAT_00605bfc;
  if ((*(unsigned char *)(DAT_0068f640[ch].h + 0x34) & 2) == 0) {
    FUN_005a66d0(DAT_0068f640[ch].h, 1);
    *(float *)(DAT_0068f640[ch].h + 0x3c) = DAT_005ce6b8 - FUN_00472650(0.0f, 5000.0f);
    if (*(int *)(DAT_0068f640[ch].h + 0x30) != 0) {
      FUN_005a6dc0(*(int *)(DAT_0068f640[ch].h + 0x30), 4, 1, DAT_005ce6b8 - FUN_00472650(0.0f, 5000.0f));
    }
    FUN_0045e0f0(ch, (float)(DAT_006036c4 * DAT_005ce988));
  }
}
