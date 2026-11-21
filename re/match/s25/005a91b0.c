// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005a91b0: PORT-TOP-AUDIO-5BA1D0-STOPS, see below. */
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
/* PORT-TOP-AUDIO-5BA1D0-STOPS (2026-11-21): new source from the listing of MASHED.exe.unpatched 0x005a91b0..0x005a9272 (no
   Ghidra source existed; the function has no caller, 0x005a8f30 stores its address at +0x28, 0x005a9160). The factor is read
   at its original address 0x005d757c (file image 0x00000000; a runtime writer is not checked here). */
MGLOBAL(float, DAT_005d757c, 0x005d757c);
typedef struct { float x; float y; float z; } V3_005a91b0;
// MATCH: 0x005a91b0 FUN_005a91b0
void FUN_005a91b0(int param_1)

{
  V3_005a91b0 v;
  float t;

  v.x = 0.0f;
  v.y = 0.0f;
  v.z = 0.0f;
  *(V3_005a91b0 *)(param_1 + 0x28) = v;          /* 0x005a91e2 / 0x005a91f2 / 0x005a91f5 */
  *(float *)(param_1 + 0x64) = 10.0f;            /* 0x005a91e4: 0x41200000 */
  *(int *)(param_1 + 0x70) = 0x7b;               /* 0x005a91eb */
  *(V3_005a91b0 *)(param_1 + 0x34) = v;          /* 0x005a91fb / 0x005a9206 / 0x005a9214 */
  *(int *)(param_1 + 0x4c) = 0;
  *(int *)(param_1 + 0x50) = 0;
  *(float *)(param_1 + 0x54) = 1.0f;             /* 0x005a9222 */
  v.x = 0.0f;
  v.y = 1.0f;
  v.z = 0.0f;
  *(V3_005a91b0 *)(param_1 + 0x40) = v;          /* 0x005a9229 / 0x005a9233 / 0x005a923a */
  *(float *)(param_1 + 0x68) = 1.0f;
  *(float *)(param_1 + 0x6c) = 1.0f;
  t = DAT_005d757c * *(float *)(param_1 + 0x48);                              /* 0x005a923e / 0x005a9241 */
  *(float *)(param_1 + 0x58) = *(float *)(param_1 + 0x44) - t;               /* 0x005a924a / 0x005a924d */
  *(float *)(param_1 + 0x5c) = t - *(float *)(param_1 + 0x40);               /* 0x005a9252 / 0x005a9254 */
  *(float *)(param_1 + 0x60) = DAT_005d757c * *(float *)(param_1 + 0x40) -
                               DAT_005d757c * *(float *)(param_1 + 0x44);    /* 0x005a9259 .. 0x005a926c */
  return;
}
