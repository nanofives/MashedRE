// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00452f30: 100% by hand (Ghidra size 50 stops before the last jl/pop/ret: match.py --bytes reports
   the whole 53 B obj symbol BYTE-IDENTICAL; the instruction score reads 73.3% only because of that cut). This file holds the two real static callees 0x00452ec0 and 0x00452f00
   (pilot finding 1: their record pointer arrives in ESI because they are static, defined before the caller,
   address not taken). */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
int FUN_004e4800(int param_1);
int FUN_004e45b0(int param_1, int param_2);
MGLOBALA(undefined4, DAT_00684e38, 28, 0x00684e38);
MGLOBALA(undefined, DAT_00684ea8, 0x2200, 0x00684ea8);
// MATCH: 0x00452ec0 FUN_00452ec0
static void FUN_00452ec0(int p)
{
  int iVar1;
  *(undefined4 *)(p + 8) = 0;
  *(undefined4 *)(p + 0x10) = 0;
  *(undefined4 *)(p + 4) = 0xffffffff;
  iVar1 = FUN_004e4800(*(int *)(p + 0x14));
  if (iVar1 != 0) {
    FUN_004e45b0(iVar1, *(int *)(p + 0x14));
  }
}
// MATCH: 0x00452f00 FUN_00452f00
static void FUN_00452f00(int p)
{
  int iVar1;
  *(undefined4 *)(p + 4) = 0;
  *(undefined4 *)(p + 0x4c) = 0;
  *(undefined4 *)(p + 0x44) = 0;
  *(undefined4 *)(p + 0x54) = 0;
  iVar1 = FUN_004e4800(*(int *)(p + 0x48));
  if (iVar1 != 0) {
    FUN_004e45b0(iVar1, *(int *)(p + 0x48));
  }
}
// MATCH: 0x00452f30 FUN_00452f30
void FUN_00452f30(void)
{
  undefined4 *puVar1;
  undefined *puVar2;
  puVar1 = DAT_00684e38;
  do {
    FUN_00452ec0((int)puVar1);
    puVar1 = puVar1 + 7;
  } while ((int)puVar1 < 0x684ea8);
  puVar2 = DAT_00684ea8;
  do {
    FUN_00452f00((int)puVar2);
    puVar2 = puVar2 + 0x110;
  } while ((int)puVar2 < 0x6870a8);
}
