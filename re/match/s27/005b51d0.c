// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 h1: 50.3% (regs 55.4%, permuter best (240 s). Not classified beyond structure/register differences (large function, budget spent). Class: b. */
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
#define CONCAT11(a,b) ((ushort)(((uint)(uchar)(a)<<8)|(uchar)(b)))
MGLOBAL(undefined1, DAT_005e6cdc, 0x005e6cdc);
MGLOBAL(undefined1, DAT_005e6414, 0x005e6414);
MGLOBAL(undefined1, DAT_005e6424, 0x005e6424);
MGLOBAL(undefined4, DAT_00634438, 0x00634438);
MGLOBAL(undefined4, DAT_0063443c, 0x0063443c);
void FUN_005ac740(int p0);
int FUN_005adf30(byte * p0, byte * p1);
#ifdef __cplusplus
undefined4 FUN_005aea00(...);
#else
undefined4 FUN_005aea00(void);
#endif
MGLOBAL(int, _DAT_005e6cdc, 0x005e6cdc);
#ifdef __cplusplus
static int ROUND(double x) { return (int)(x + (x < 0 ? -0.5 : 0.5)); }
static int ABS(int x) { return x < 0 ? -x : x; }
#endif
// MATCH: 0x005b51d0 FUN_005b51d0
/* [C2 2026-06-01] EA-XA ADPCM decode-header initializer: allocates and fills a 0x60-byte (96)
   decoder-state block with big-endian fields? */

undefined4 FUN_005b51d0(undefined2 *param_1,uint param_2,int param_3,int param_4)

{
  uint *puVar1;
  int iVar2;
  uint uVar3;
  uint *puVar4;
  uint uVar5;
  
  if (*(uint *)(param_4 + 0x14) < 0x60) {
    FUN_005ac740(param_4);
  }
  if (*(int *)(param_4 + 0x10) == 0) {
    iVar2 = FUN_005aea00(0x60,0x30806);
    *(int *)(param_4 + 0x10) = iVar2;
    if (iVar2 == 0) {
      return 0;
    }
    *(undefined4 *)(param_4 + 0x14) = 0x60;
  }
  puVar4 = puVar1;
  puVar1 = *(uint **)(param_4 + 0x10);
  for (iVar2 = 0x18; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar4 = 0;
    puVar4 = puVar4 + 1;
  }
  iVar2 = FUN_005adf30((byte *)*(undefined4 *)(param_3 + 4),&DAT_005e6414);
  if (iVar2 == 0) {
    uVar3 = param_2 * 7 >> 2;
  }
  else {
    iVar2 = FUN_005adf30((byte *)*(undefined4 *)(param_3 + 4),&DAT_005e6424);
    uVar3 = 0;
    if (iVar2 == 0) {
      if (*(char *)(param_3 + 0xc) == '\x10') {
        uVar3 = param_2 >> 1;
      }
      else if (*(char *)(param_3 + 0xc) == '\b') {
        uVar3 = param_2;
      }
    }
  }
  *puVar1 = (uVar3 & 0xff0000 | uVar3 >> 0x10) >> 8 | (uVar3 & 0xff00 | uVar3 << 0x10) << 8;
  puVar4 = puVar1 + 7;
  uVar3 = (uVar3 / 0xe) * 0x10 + 1 + uVar3 % 0xe;
  uVar5 = 0;
  puVar1[1] = (uVar3 & 0xff0000 | uVar3 >> 0x10) >> 8 |
              (uVar3 & 0xff00 | (uVar3 & 0xfffffffe) << 0x10) << 8;
  do {
    uVar3 = (uint)ROUND(*(float *)((int)&DAT_00634438 + uVar5) * _DAT_005e6cdc);
    puVar4 = puVar4 + 1;
    *(ushort *)puVar4 = (ushort)(uVar3 >> 8) & 0xff ^ (ushort)((uVar3 & 0xff) << 8);
    uVar3 = (uint)ROUND(*(float *)((int)&DAT_0063443c + uVar5) * _DAT_005e6cdc);
    uVar5 = uVar5 + 8;
    *(ushort *)((int)puVar4 + 2) = (ushort)(uVar3 >> 8) & 0xff ^ (ushort)((uVar3 & 0xff) << 8);
  } while (uVar5 < 0x40);
  *(ushort *)((int)puVar1 + 0x42) = (ushort)param_1[1] >> 8 ^ param_1[1] << 8;
  *(ushort *)(puVar1 + 0x10) = CONCAT11((char)*param_1,(char)((ushort)*param_1 >> 8));
  return 0x60;
}
