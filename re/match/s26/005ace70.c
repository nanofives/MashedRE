// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005ace70: 89.7% (regs 82.4%, ops 96.3%), 132 vs 136 insns. Hand rewrite: Fmt struct (7 dwords) copied by value (rep movsd), byte +0xd set to 1, uint division (not 64-bit), uVar3 = FUN_005ac650(&local)/uVar2 evaluated before FUN_005ace70, (iVar1 != 0 ? 0x180 : 0) gives neg/sbb/and. Residual: the DDK cross-jumps the first and third 'return 1' blocks into one, the original keeps 4 separate mov eax,imm/pop/add esp/ret exits; and original loads cl then 'mov al,1; cmp cl,al'. Relocheck BAD only from shifted offsets. Class (a). cxx pending. */
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
typedef unsigned char byte;
typedef struct Fmt { uint w[7]; } Fmt;
MGLOBALA(byte, DAT_005e6414, 1, 0x005e6414);
MGLOBALA(byte, DAT_005e6424, 1, 0x005e6424);
MGLOBALA(byte, DAT_005e6434, 1, 0x005e6434);
MGLOBALA(byte, DAT_005e6444, 1, 0x005e6444);
MGLOBALA(byte, DAT_005e6454, 1, 0x005e6454);
MGLOBALA(byte, DAT_005e6474, 1, 0x005e6474);
MGLOBALA(byte, DAT_005e6484, 1, 0x005e6484);
MGLOBALA(byte, DAT_005e6494, 1, 0x005e6494);
MGLOBALA(byte, DAT_005e64a4, 1, 0x005e64a4);
MGLOBALA(byte, DAT_005e64b4, 1, 0x005e64b4);
uint FUN_005ac650(Fmt * p0);
int FUN_005adf30(byte * p0, byte * p1);
// MATCH: 0x005ace70 FUN_005ace70
uint FUN_005ace70(Fmt *param_1)
{
  int iVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  Fmt local_1c;

  if (FUN_005adf30((byte *)param_1->w[1],DAT_005e6424) == 0) {
    return 1;
  }
  if (FUN_005adf30((byte *)param_1->w[1],DAT_005e6414) == 0) {
    return 0x1c;
  }
  if (FUN_005adf30((byte *)param_1->w[1],DAT_005e6434) == 0) {
    return 1;
  }
  if (FUN_005adf30((byte *)param_1->w[1],DAT_005e64a4) == 0) {
    return 0x600;
  }
  if (FUN_005adf30((byte *)param_1->w[1],DAT_005e6444) == 0) {
    return 0xe;
  }
  iVar1 = FUN_005adf30((byte *)param_1->w[1],DAT_005e6454);
  if (iVar1 != 0) {
    iVar1 = FUN_005adf30((byte *)param_1->w[1],DAT_005e64b4);
    if (iVar1 != 0) {
      iVar1 = FUN_005adf30((byte *)param_1->w[1],DAT_005e6474);
      if (iVar1 != 0) {
        iVar1 = FUN_005adf30((byte *)param_1->w[1],DAT_005e6484);
        if (iVar1 != 0) {
          iVar1 = FUN_005adf30((byte *)param_1->w[1],DAT_005e6494);
          return iVar1 != 0 ? 0x180 : 0;
        }
      }
      return 0x480;
    }
  }
  if (*((byte *)param_1 + 0xd) == 1) {
    return 0x40;
  }
  local_1c = *param_1;
  *((byte *)&local_1c + 0xd) = 1;
  uVar2 = FUN_005ac650(param_1);
  uVar3 = FUN_005ac650(&local_1c) / uVar2;
  uVar4 = FUN_005ace70(&local_1c);
  return uVar4 / uVar3;
}
