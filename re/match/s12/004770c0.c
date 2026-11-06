// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x004770c0 (801 B): 100.0% (regs 100.0%), hand pass from the raw converter (61.6%). What moved it: the 12 globals at 0x00692558..0x00692590 typed float and stored as 1.0f / 0.0f (int stores made the compiler hold 0x3f800000 and 0 in registers), the DAT_00692564 |= 0x20003 written after the float stores (its load is scheduled early, the store late), |= straight on the +0x40 field instead of through a pointer temp, and the (char)flags < 0 test written as flags & 0x80. */
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
typedef unsigned int uint;
typedef unsigned int undefined4;
MGLOBAL(uint, DAT_00692564, 0x00692564);
MGLOBAL(float, DAT_00692580, 0x00692580);
MGLOBAL(float, DAT_0069256c, 0x0069256c);
MGLOBAL(float, DAT_00692558, 0x00692558);
MGLOBAL(float, DAT_00692568, 0x00692568);
MGLOBAL(float, DAT_00692560, 0x00692560);
MGLOBAL(float, DAT_0069255c, 0x0069255c);
MGLOBAL(float, DAT_0069257c, 0x0069257c);
MGLOBAL(float, DAT_00692578, 0x00692578);
MGLOBAL(float, DAT_00692570, 0x00692570);
MGLOBAL(float, DAT_00692590, 0x00692590);
MGLOBAL(float, DAT_0069258c, 0x0069258c);
MGLOBAL(float, DAT_00692588, 0x00692588);
MGLOBAL(int, DAT_007dc57c, 0x007dc57c);
MGLOBAL(undefined4, DAT_006132c4, 0x006132c4);
MGLOBAL(undefined4, DAT_006132c8, 0x006132c8);
uint FUN_00474db0(uint p0, int p1);
void FUN_00476ae0(uint *p0, int p1);
void FUN_00476b30(uint *p0, int p1);
void FUN_00476b90(uint *p0, int p1);
void FUN_00476be0(uint *p0, int p1);
void FUN_00476c10(uint *p0, int p1);
void FUN_00476c60(uint *p0, int p1);
void FUN_004b6520(void *p0, int p1);
int FUN_004c0b30(void);
int FUN_004e7e30(int p0, int p1);
int *FUN_004e8090(int p0, undefined4 p1);
uint FUN_00534b60(uint p0, uint p1, int p2);
// MATCH: 0x004770c0 FUN_004770c0
void FUN_004770c0(uint *param_1, uint param_2, uint param_3, undefined4 param_4)
{
  uint f;
  int g;

  f = 0x10000000;
  if (param_2 == 0) {
    param_2 = 0x807;
  }
  if (param_2 & 1) f = 0x10000001;
  if (param_2 & 2) f |= 4;
  if (param_2 & 4) f |= 2;
  if (param_2 & 8) f |= 8;
  if (param_2 & 0x10) f |= 0x20;
  if (param_2 & 0x20) f |= 0x80;
  if (param_2 & 0x40) f |= 0x100;
  if (param_2 & 0x200) f |= 0x8000;
  if (param_2 & 0x400) f |= 0x20000;
  if (param_2 & 0x800) f |= 0x80000;
  if (param_2 & 0x1000) f |= 0x100000;
  FUN_004b6520(param_1, 0x40);
  param_1[1] = FUN_00534b60(param_3, f, 0);
  DAT_00692580 = 1.0f;
  DAT_0069256c = 1.0f;
  DAT_00692558 = 1.0f;
  DAT_00692568 = 0.0f;
  DAT_00692560 = 0.0f;
  DAT_0069255c = 0.0f;
  DAT_0069257c = 0.0f;
  DAT_00692578 = 0.0f;
  DAT_00692570 = 0.0f;
  DAT_00692590 = 0.0f;
  DAT_0069258c = 0.0f;
  DAT_00692588 = 0.0f;
  DAT_00692564 |= 0x20003;
  *param_1 = param_2;
  if (param_2 & 0x80) FUN_00476ae0(param_1, 0);
  if (param_2 & 0x100) FUN_00476b30(param_1, 0);
  if (param_2 & 0x200) FUN_00476b90(param_1, 0);
  if (param_2 & 0x400) FUN_00476be0(param_1, 0);
  if (param_2 & 0x800) FUN_00476c10(param_1, 0);
  if (param_2 & 0x1000) FUN_00476c60(param_1, 0);
  FUN_004e8090(**(int **)(*(int *)(param_1[1] + 0x18) + 0x20), param_4);
  *(int *)(*(int *)(param_1[1] + DAT_007dc57c) + 0xac) = 1;
  *(undefined4 *)(*(int *)(param_1[1] + DAT_007dc57c) + 0xa4) = DAT_006132c4;
  *(undefined4 *)(*(int *)(param_1[1] + DAT_007dc57c) + 0xa8) = DAT_006132c8;
  *(uint *)(*(int *)(param_1[1] + DAT_007dc57c) + 0x40) |= 0x10000000;
  *(uint *)(*(int *)(param_1[1] + DAT_007dc57c) + 0x40) |= 0x800000;
  *(int *)(*(int *)(param_1[1] + DAT_007dc57c) + 4) = 0;
  g = FUN_004c0b30();
  FUN_004e7e30(param_1[1], g);
  param_1[2] = param_3;
  if (param_2 & 1) param_1[4] = FUN_00474db0(param_3, 0xc);
  if (param_2 & 2) param_1[5] = FUN_00474db0(param_3, 8);
  if (param_2 & 4) param_1[6] = FUN_00474db0(param_3, 4);
  if (param_2 & 8) param_1[7] = FUN_00474db0(param_3, 0x40);
  if (param_2 & 0x10) param_1[8] = FUN_00474db0(param_3, 4);
  if (param_2 & 0x20) param_1[9] = FUN_00474db0(param_3 * 2, 8);
  if (param_2 & 0x40) param_1[10] = FUN_00474db0(param_3 * 4, 8);
  param_1[0xb] = 0;
  param_1[0xc] = 0;
}
