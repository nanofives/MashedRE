// FLAGS: /O2 /Ob1 /G5 /Oy-
/* MATCH-S22 0x004fb050 (h7): 100.0% (regs 100.0%) under /O2 /Ob1 /G5 /Oy- (ebp frame, same as 0x004fb0e0; plain
   /G5 raw converter 86.1%). 0x006199b0 is written as one 16-dword array (the 3x4 copy skips every 4th dword). */
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
MGLOBAL(int *, DAT_007d9900, 0x007d9900);
MGLOBALA(int, DAT_006199b0, 16, 0x006199b0);
MGLOBAL(int, DAT_007d9904, 0x007d9904);
MGLOBAL(int, DAT_007d9908, 0x007d9908);
MGLOBAL(int, DAT_007d990c, 0x007d990c);
// MATCH: 0x004fb050 FUN_004fb050
void FUN_004fb050(int *param_1)
{
  DAT_007d9900 = param_1;
  if (param_1 != 0) {
    DAT_006199b0[0] = param_1[0];
    DAT_006199b0[1] = param_1[1];
    DAT_006199b0[2] = param_1[2];
    DAT_006199b0[4] = param_1[4];
    DAT_006199b0[5] = param_1[5];
    DAT_006199b0[6] = param_1[6];
    DAT_006199b0[8] = param_1[8];
    DAT_006199b0[9] = param_1[9];
    DAT_006199b0[10] = param_1[10];
    DAT_006199b0[12] = param_1[12];
    DAT_006199b0[13] = param_1[13];
    DAT_006199b0[14] = param_1[14];
  }
  DAT_007d9904 = 0;
  DAT_007d9908 = 0;
  DAT_007d990c = 0;
}
