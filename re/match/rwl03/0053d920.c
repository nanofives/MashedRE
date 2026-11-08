// FLAGS: /O1
/* MATCH-RWL03 0x0053d920: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
#ifndef __cplusplus
#define true 1
#endif
#define CONCAT11(a,b) ((ushort)(((uint)(uchar)(a)<<8)|(uchar)(b)))
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int FUN_004cc050(undefined4 p0, int p1);
undefined4 FUN_004cc790(undefined4 p0, void *p1, undefined4 p2);
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 * FUN_004d8480(undefined4 * p0);
uint * FUN_0053d5a0(ushort p0, ushort p1, undefined4 p2, uint p3);
// MATCH: 0x0053d920 FUN_0053d920
byte * FUN_0053d920(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                   undefined4 *param_5)

{
  byte bVar1;
  byte *pbVar2;
  int iVar3;
  int iVar4;
  byte *pbVar5;
  undefined4 *puVar6;
  uint uVar7;
  undefined4 local_90;
  undefined4 uStack_8c;
  uint local_88 [34];
  
  *param_5 = 0;
  pbVar2 = (byte *)FUN_0053d5a0(param_2,param_3,param_4,1);
  if (pbVar2 == (byte *)0x0) {
    return (byte *)0x0;
  }
  if (*(short *)(pbVar2 + 0x1e) == 0) {
    iVar3 = FUN_004cc050(param_1,4);
    if (iVar3 == 0) {
      (**(code **)(DAT_007d3ff8 + 0x10c))(pbVar2);
      return (byte *)0x0;
    }
  }
  else {
    pbVar5 = *(byte **)(pbVar2 + 0x20);
    uVar7 = 0;
    if (*(short *)(pbVar2 + 0x1e) != 0) {
      do {
        iVar3 = FUN_004cc790(param_1,local_88,4);
        if (iVar3 == 0) goto LAB_0053db75;
        iVar3 = FUN_004cc790(param_1,&local_90,4);
        if (iVar3 == 0) goto LAB_0053db75;
        iVar3 = FUN_004cc790(param_1,pbVar5 + 4,4);
        if (iVar3 == 0) goto LAB_0053db75;
        iVar3 = FUN_004cc790(param_1,pbVar5 + 0xc,4);
        if (iVar3 == 0) goto LAB_0053db75;
        bVar1 = (byte)(local_88[0] >> 0x10);
        pbVar5[8] = bVar1;
        *pbVar5 = bVar1 | 1;
        *(short *)(pbVar5 + 10) = (short)local_90;
        *(short *)(pbVar5 + 2) = (short)(local_90 >> 0x10);
        if ((local_88[0] & 0xff00) == 0x200) {
          pbVar5[1] = 0xff;
        }
        else {
          pbVar5[1] = 0;
        }
        if ((char)local_88[0] == '\x02') {
          pbVar5[9] = 0xff;
        }
        else {
          pbVar5[9] = 0;
        }
        uVar7 = uVar7 + 1;
        pbVar5 = pbVar5 + 0x10;
      } while (uVar7 < *(ushort *)(pbVar2 + 0x1e));
    }
    (*(ushort *)((char *)&local_90 + 0)) = CONCAT11(0xff,(undefined1)local_90);
    local_90 = (uint)(ushort)local_90;
    puVar6 = &local_90;
    iVar3 = 1;
    do {
      if (*(char *)((int)puVar6 + 1) == -1) {
        iVar4 = iVar3 + 1;
        puVar6 = (undefined4 *)((uint)*(ushort *)((int)puVar6 + 2) * 0x10 + *(int *)(pbVar2 + 0x20))
        ;
        local_88[iVar4] = (uint)(puVar6 + 2);
      }
      else {
        iVar4 = FUN_004cc790(param_1,local_88,4);
        if (iVar4 == 0) goto LAB_0053db75;
        if (0xef < (ushort)(local_88[0] >> 0x10)) {
          *param_5 = 1;
          (**(code **)(DAT_007d3ff8 + 0x10c))(pbVar2);
          local_90 = 0x11d;
          uStack_8c = FUN_004d7ff0(1);
          FUN_004d8480(&local_90);
          return (byte *)0x0;
        }
        *(char *)((int)puVar6 + 1) = (char)(local_88[0] >> 0x10);
        iVar4 = iVar3 + -1;
        *(undefined2 *)((int)puVar6 + 2) = (undefined2)local_88[0];
        puVar6 = (undefined4 *)local_88[iVar3];
      }
      iVar3 = iVar4;
    } while (iVar4 != 0);
  }
  if (((*pbVar2 & 1) == 0) || (uVar7 = 0, *(short *)(pbVar2 + 0x1c) == 0)) {
    return pbVar2;
  }
  while( true ) {
    iVar3 = FUN_004cc790(param_1,&local_90,4);
    if (iVar3 == 0) break;
    *(ushort *)(*(int *)(pbVar2 + 0x24) + uVar7 * 2) = (ushort)local_90;
    uVar7 = uVar7 + 1;
    if (*(ushort *)(pbVar2 + 0x1c) <= uVar7) {
      return pbVar2;
    }
  }
LAB_0053db75:
  (**(code **)(DAT_007d3ff8 + 0x10c))(pbVar2);
  return (byte *)0x0;
}
