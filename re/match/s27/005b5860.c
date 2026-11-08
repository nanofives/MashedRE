// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005b5860: 25.3% (regs 41.6%), raw converter body kept. ADPCM-style block decoder (magic-number divide by 0xe, byte swaps, 0x8000 clamp, 0x10-short table). Residual (b): 178 vs 160 instr; no structural pass made (budget).  C++ fix: param_1 < (uint)param_2. */
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
typedef unsigned short undefined2;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
#define CONCAT11(a,b) ((ushort)(((uint)(uchar)(a)<<8)|(uchar)(b)))
// MATCH: 0x005b5860 FUN_005b5860
int FUN_005b5860(uint param_1,byte *param_2,undefined2 *param_3)

{
  short sVar1;
  short sVar2;
  uint *puVar3;
  int iVar4;
  byte bVar5;
  uint *puVar6;
  uint uVar7;
  uint uVar8;
  byte *pbVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  short asStack_20 [16];
  
  puVar3 = *(uint **)(param_1 + 0x10);
  uVar10 = *puVar3;
  puVar6 = puVar3 + 7;
  uVar10 = (uVar10 & 0xff0000 | uVar10 >> 0x10) >> 8 | (uVar10 & 0xff00 | uVar10 << 0x10) << 8;
  uVar11 = uVar10 - 2;
  uVar12 = 0;
  do {
    uVar7 = *puVar6;
    puVar6 = (uint *)((int)puVar6 + 2);
    asStack_20[uVar12] = CONCAT11((char)(short)uVar7,(char)((ushort)(short)uVar7 >> 8));
    uVar12 = uVar12 + 1;
  } while (uVar12 < 0x10);
  uVar12 = (uint)(byte)((ushort)(short)puVar3[0x10] >> 8) ^ (int)(short)((short)puVar3[0x10] << 8);
  uVar7 = (uint)(byte)((ushort)*(short *)((int)puVar3 + 0x42) >> 8) ^
          (int)(short)(*(short *)((int)puVar3 + 0x42) << 8);
  uVar10 = uVar10 + 0xb;
  iVar4 = uVar10 * 0x24924925;
  *param_3 = (short)uVar12;
  param_3[1] = (short)uVar7;
  param_3 = param_3 + 2;
  pbVar9 = param_2;
  for (uVar10 = uVar10 / 0xe; uVar10 != 0; uVar10 = uVar10 - 1) {
    bVar5 = *pbVar9 & 0xf;
    uVar8 = (uint)(*pbVar9 >> 4);
    sVar1 = asStack_20[uVar8 * 2];
    sVar2 = asStack_20[uVar8 * 2 + 1];
    pbVar9 = pbVar9 + 1;
    param_2 = (byte *)0xe;
    if (uVar11 < 0xe) {
      param_2 = (byte *)uVar11;
    }
    uVar11 = uVar11 - (int)param_2;
    param_1 = 0;
    if (param_2 != (byte *)0x0) {
      uVar8 = uVar12;
      do {
        uVar12 = uVar7;
        if ((param_1 & 1) == 0) {
          uVar7 = (((int)(short)((ushort)*pbVar9 << 8) >> 0xc) << bVar5) +
                  ((int)((int)sVar2 * uVar8) >> 0xb) + ((int)((int)sVar1 * uVar12) >> 0xb);
          if ((int)uVar7 < 0x8000) {
            if ((int)uVar7 < -0x8000) {
              uVar7 = 0xffff8000;
            }
            *param_3 = (short)uVar7;
          }
          else {
            uVar7 = 0x7fff;
            *param_3 = 0x7fff;
          }
        }
        else {
          uVar7 = (((int)(short)((ushort)*pbVar9 << 0xc) >> 0xc) << bVar5) +
                  ((int)((int)sVar2 * uVar8) >> 0xb) + ((int)((int)sVar1 * uVar12) >> 0xb);
          if ((int)uVar7 < 0x8000) {
            if ((int)uVar7 < -0x8000) {
              uVar7 = 0xffff8000;
            }
          }
          else {
            uVar7 = 0x7fff;
          }
          *param_3 = (short)uVar7;
          pbVar9 = pbVar9 + 1;
        }
        param_3 = param_3 + 1;
        param_1 = param_1 + 1;
        uVar8 = uVar12;
      } while (param_1 < (uint)param_2);
    }
    iVar4 = 0;
  }
  return iVar4;
}
