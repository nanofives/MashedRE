// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004ddcb0: best 46.2% (regs ~55%), class (b). Two duplicated pow2-round bodies (flag test 0x2 / 0x100 in the else arm). With one shared counter the compiler tail-merges them (original does not); with separate counters it allocates edi for the shift result where the original uses eax/ecx. Tried: byte vs int counters, shared counter, test-ah forms. */
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
typedef unsigned int uint;
typedef unsigned char byte;
MGLOBAL(int, DAT_00911ff8, 0x00911ff8);
MGLOBAL(int, DAT_00911ffc, 0x00911ffc);
MGLOBAL(undefined1, DAT_00911fdc, 0x00911fdc);
MGLOBAL(int, _DAT_00911fdc, 0x00911fdc);
// MATCH: 0x004ddcb0 FUN_004ddcb0
void FUN_004ddcb0(int *param_1,int *param_2,uint param_3)
{
  int iVar1;
  int bVar2;
  int bVar3;
  if (*param_1 > DAT_00911ff8) {
    *param_1 = DAT_00911ff8;
  }
  if (*param_2 > DAT_00911ffc) {
    *param_2 = DAT_00911ffc;
  }
  if (((param_3 & 7) != 0) && ((param_3 & 0x8000) != 0)) {
    if ((_DAT_00911fdc & 2) == 0) goto LAB_004ddd47;
    bVar2 = -1;
    for (iVar1 = *param_1; iVar1 != 0; iVar1 = iVar1 >> 1) {
      bVar2 = bVar2 + 1;
    }
    bVar3 = -1;
    *param_1 = 1 << bVar2;
    for (iVar1 = *param_2; iVar1 != 0; iVar1 = iVar1 >> 1) {
      bVar3 = bVar3 + 1;
    }
  }
  else {
    if (((_DAT_00911fdc & 2) == 0) || ((_DAT_00911fdc & 0x100) != 0)) goto LAB_004ddd47;
    bVar2 = -1;
    for (iVar1 = *param_1; iVar1 != 0; iVar1 = iVar1 >> 1) {
      bVar2 = bVar2 + 1;
    }
    bVar3 = -1;
    *param_1 = 1 << bVar2;
    for (iVar1 = *param_2; iVar1 != 0; iVar1 = iVar1 >> 1) {
      bVar3 = bVar3 + 1;
    }
  }
  *param_2 = 1 << bVar3;
LAB_004ddd47:
  if ((_DAT_00911fdc & 0x20) != 0) {
    if (*param_1 < *param_2) {
      *param_2 = *param_1;
      return;
    }
    *param_1 = *param_2;
  }
  return;
}
