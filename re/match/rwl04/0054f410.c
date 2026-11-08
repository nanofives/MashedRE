// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x0054f410 h1: best 75.8% (regs 75.8%, ops 89.7%). Globals typed float (was int, gave fimul). Residue: orig copies the float lanes with fld/fstp, ours with integer mov edx; some fadd operand order. Flags /Op /G6 no change, /Oa worse. Class (a). */
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
MGLOBAL(float, DAT_005ccac8, 0x005ccac8);
MGLOBAL(float, DAT_005cc31c, 0x005cc31c);
MGLOBAL(float, DAT_005cd0a0, 0x005cd0a0);
// MATCH: 0x0054f410 FUN_0054f410
/* [C1 2026-05-19] 2-param `(out_dst*, src_4x4_control_points*)`. Control-net builder. */

void FUN_0054f410(float *param_1,float *param_2)

{
  *param_1 = *param_2;
  param_1[1] = param_2[1];
  param_1[2] = param_2[2];
  param_1[4] = param_2[4];
  param_1[5] = param_2[5];
  param_1[6] = param_2[6];
  param_1[8] = param_2[8];
  param_1[9] = param_2[9];
  param_1[10] = param_2[10];
  param_1[0xc] = param_2[0xc];
  param_1[0xd] = param_2[0xd];
  param_1[0xe] = param_2[0xe];
  param_1[0x10] = param_2[0x10];
  param_1[0x11] = param_2[0x11];
  param_1[0x12] = param_2[0x12];
  param_1[0x14] =
       (((param_2[0x14] + param_2[0x14] + param_2[0x10]) - *param_2) + param_2[4]) * DAT_005ccac8;
  param_1[0x15] =
       (((param_2[0x15] + param_2[0x15] + param_2[0x11]) - param_2[1]) + param_2[5]) * DAT_005ccac8
  ;
  param_1[0x16] =
       (((param_2[0x16] + param_2[0x16] + param_2[0x12]) - param_2[2]) + param_2[6]) * DAT_005ccac8
  ;
  param_1[0x18] =
       ((param_2[0x14] + param_2[8] + param_2[0x14] + param_2[8] + param_2[0x18]) -
       (param_2[4] + param_2[4])) * DAT_005ccac8;
  param_1[0x19] =
       ((param_2[0x15] + param_2[9] + param_2[0x15] + param_2[9] + param_2[0x19]) -
       (param_2[5] + param_2[5])) * DAT_005ccac8;
  param_1[0x1a] =
       ((param_2[0x16] + param_2[10] + param_2[0x16] + param_2[10] + param_2[0x1a]) -
       (param_2[6] + param_2[6])) * DAT_005ccac8;
  param_1[0x1c] = (param_2[0x18] - param_2[8]) + param_2[0xc];
  param_1[0x1d] = (param_2[0x19] - param_2[9]) + param_2[0xd];
  param_1[0x1e] = (param_2[0x1a] - param_2[10]) + param_2[0xe];
  param_1[0x20] = param_2[0x20];
  param_1[0x21] = param_2[0x21];
  param_1[0x22] = param_2[0x22];
  param_1[0x24] =
       ((param_2[0x20] + param_2[0x14] + param_2[0x20] + param_2[0x14] + param_2[0x24]) -
       (param_2[0x10] + param_2[0x10])) * DAT_005ccac8;
  param_1[0x25] =
       ((param_2[0x21] + param_2[0x15] + param_2[0x21] + param_2[0x15] + param_2[0x25]) -
       (param_2[0x11] + param_2[0x11])) * DAT_005ccac8;
  param_1[0x26] =
       ((param_2[0x16] + param_2[0x22] + param_2[0x16] + param_2[0x22] + param_2[0x26]) -
       (param_2[0x12] + param_2[0x12])) * DAT_005ccac8;
  param_1[0x28] =
       ((((param_2[0x24] + param_2[0x18] + param_2[0x24] + param_2[0x18] + param_2[0x20]) -
         (param_2[0x10] + param_2[0x10])) + param_2[8] + *param_2) - (param_2[4] + param_2[4])) *
       DAT_005ccac8;
  param_1[0x29] =
       ((((param_2[0x25] + param_2[0x19] + param_2[0x25] + param_2[0x19] + param_2[0x21]) -
         (param_2[0x11] + param_2[0x11])) + param_2[1] + param_2[9]) - (param_2[5] + param_2[5])) *
       DAT_005ccac8;
  param_1[0x2a] =
       ((((param_2[0x26] + param_2[0x1a] + param_2[0x26] + param_2[0x1a] + param_2[0x22]) -
         (param_2[0x12] + param_2[0x12])) + param_2[2] + param_2[10]) - (param_2[6] + param_2[6])) *
       DAT_005ccac8;
  param_1[0x2c] =
       ((param_2[0x18] + param_2[0x18] + (param_2[4] - (param_2[0x14] + param_2[0x14])) +
        param_2[0x24]) - (param_2[8] + param_2[8])) + param_2[0xc];
  param_1[0x2d] =
       ((param_2[0x19] + param_2[0x19] + (param_2[5] - (param_2[0x15] + param_2[0x15])) +
        param_2[0x25]) - (param_2[9] + param_2[9])) + param_2[0xd];
  param_1[0x2e] =
       ((param_2[0x1a] + param_2[0x1a] + (param_2[6] - (param_2[0x16] + param_2[0x16])) +
        param_2[0x26]) - (param_2[10] + param_2[10])) + param_2[0xe];
  param_1[0x30] = param_2[0x30];
  param_1[0x31] = param_2[0x31];
  param_1[0x32] = param_2[0x32];
  param_1[0x34] = (param_2[0x30] - param_2[0x20]) + param_2[0x24];
  param_1[0x35] = (param_2[0x31] - param_2[0x21]) + param_2[0x25];
  param_1[0x36] = (param_2[0x32] - param_2[0x22]) + param_2[0x26];
  param_1[0x38] =
       ((param_2[0x24] + param_2[0x24] + (param_2[0x30] - (param_2[0x20] + param_2[0x20])) +
        param_2[0x18]) - (param_2[0x14] + param_2[0x14])) + param_2[0x10];
  param_1[0x39] =
       ((param_2[0x25] + param_2[0x25] + (param_2[0x31] - (param_2[0x21] + param_2[0x21])) +
        param_2[0x19]) - (param_2[0x15] + param_2[0x15])) + param_2[0x11];
  param_1[0x3a] =
       ((param_2[0x26] + param_2[0x26] + (param_2[0x32] - (param_2[0x22] + param_2[0x22])) +
        param_2[0x1a]) - (param_2[0x16] + param_2[0x16])) + param_2[0x12];
  param_1[0x3c] =
       (DAT_005cc31c * param_2[4] +
       ((DAT_005cc31c * param_2[0x10] +
         ((DAT_005cc31c * param_2[0x18] +
          ((DAT_005cc31c * param_2[0x24] + param_2[0x30]) - DAT_005cc31c * param_2[0x20])) -
         DAT_005cd0a0 * param_2[0x14]) + param_2[0xc]) - DAT_005cc31c * param_2[8])) - *param_2;
  param_1[0x3d] =
       (DAT_005cc31c * param_2[5] +
       ((DAT_005cc31c * param_2[0x11] +
         ((DAT_005cc31c * param_2[0x19] +
          ((DAT_005cc31c * param_2[0x25] + param_2[0x31]) - DAT_005cc31c * param_2[0x21])) -
         DAT_005cd0a0 * param_2[0x15]) + param_2[0xd]) - DAT_005cc31c * param_2[9])) - param_2[1];
  param_1[0x3e] =
       (DAT_005cc31c * param_2[6] +
       ((DAT_005cc31c * param_2[0x12] +
         ((DAT_005cc31c * param_2[0x1a] +
          ((DAT_005cc31c * param_2[0x26] + param_2[0x32]) - DAT_005cc31c * param_2[0x22])) -
         DAT_005cd0a0 * param_2[0x16]) + param_2[0xe]) - DAT_005cc31c * param_2[10])) - param_2[2]
  ;
  return;
}
