// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR04 2026-11-08: 79.8%, was 78.0%. Products written out (no in[3] = in[0] copies). Remaining: the original keeps w*s and h*s as stack temps (h*s reuses the scale slot). Class (b). */
/* MATCH-RWL04 0x00552920 (h2): 78.0% (regs 78.0, ops 90.2). Typed: in[12]/out[12] float arrays for the two 4x3 vertex blocks, scale in a 2-float array sa[2] (forces the original frame 0x70 with s at +0), global reads moved after the if. Residue: stack slot order of s/fVar1/fVar2 and CSE of fVar*s (orig computes once and spills, ours re-multiplies). class (b). */
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
MGLOBAL(int, DAT_00912c0c, 0x00912c0c);
MGLOBAL(float, DAT_00912b08, 0x00912b08);
MGLOBAL(float, DAT_00912b0c, 0x00912b0c);
MGLOBAL(float, DAT_005cd50c, 0x005cd50c);
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);
MGLOBAL(float, DAT_00912be0, 0x00912be0);
MGLOBAL(undefined1, DAT_00912b58, 0x00912b58);
void FUN_004c3d90(float *p0, float *p1, int p2, undefined1 *p3);
void FUN_00552e40(void);
void FUN_00552fa0(int p0);
int FUN_00553030(int p0, float p1, float p2);
int FUN_005530f0(int p0, float p1, float p2);
int FUN_005533d0(int p0);
// MATCH: 0x00552920 FUN_00552920
int FUN_00552920(int param_1)
{
  float sa[2];
  float fVar1;
  float fVar2;
  float x, y;
  float in[12];
  float out[12];

  if (*(int *)(DAT_00912c0c + 0x14) == 2) {
    sa[0] = 1.0f;
  }
  else {
    sa[0] = DAT_00912be0;
  }
  fVar1 = DAT_00912b08;
  fVar2 = DAT_00912b0c;
  FUN_00552e40();
  fVar1 = fVar1 * sa[0];
  fVar2 = fVar2 * sa[0];
  in[0] = fVar1 * DAT_005cd50c;
  in[1] = fVar2 * DAT_005cd50c;
  in[2] = 0;
  in[3] = fVar1 * DAT_005cd50c;
  in[4] = fVar2 * DAT_005cc32c;
  in[5] = 0;
  in[6] = fVar1 * DAT_005cc32c;
  in[7] = fVar2 * DAT_005cc32c;
  in[8] = 0;
  in[9] = fVar1 * DAT_005cc32c;
  in[10] = fVar2 * DAT_005cd50c;
  in[11] = 0;
  FUN_004c3d90(out,in,4,&DAT_00912b58);
  FUN_00552fa0(param_1);
  FUN_00553030(param_1,out[0],out[1]);
  FUN_005530f0(param_1,out[3],out[4]);
  FUN_005530f0(param_1,out[6],out[7]);
  FUN_005530f0(param_1,out[9],out[10]);
  FUN_005533d0(param_1);
  return 1;
}
