// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x00563b00: hand-matched (h3). 27.5% (nosize). x87 float10 callee return type fixed so it compiles. class (b), not worked. */
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
typedef unsigned int uint;
typedef unsigned short ushort;
typedef long double float10;
MGLOBAL(undefined1, DAT_005cc33c, 0x005cc33c);
float * FUN_00546d10(float * p0, int p1, int p2, float * p3);
float10 FUN_00563c80(int p0, float * p1, ushort p2, float p3, ushort * p4);
undefined4 FUN_00565ab0(undefined4 * p0, undefined4 * p1);
undefined4 FUN_00573890(undefined4 p0, float * p1, float * p2, float * p3);
MGLOBAL(int, _DAT_005cc33c, 0x005cc33c);
// MATCH: 0x00563b00 FUN_00563b00
/* [C1 2026-05-19] Computes 2 support-extrema along an axis defined by a quaternion-transformed
   direction. */

void FUN_00563b00(int param_1)

{
  float fVar1;
  float fVar2;
  float fVar3;
  int iVar4;
  uint uVar5;
  float10 fVar6;
  float10 fVar7;
  float local_3c;
  float local_38;
  float local_34;
  float local_30;
  float local_2c;
  float local_28;
  undefined1 local_24 [36];
  
  iVar4 = param_1;
  FUN_00573890(param_1,(float *)(param_1 + 0x5c),(float *)local_24,(float *)(param_1 + 0x54));
  FUN_00565ab0((undefined4 *)(iVar4 + 0x68),(undefined4 *)local_24);
  fVar1 = *(float *)(iVar4 + 0x68) * _DAT_005cc33c;
  fVar2 = *(float *)(iVar4 + 0x6c) * _DAT_005cc33c;
  fVar3 = *(float *)(iVar4 + 0x70) * _DAT_005cc33c;
  uVar5 = 0x21312300 >> ((((fVar2 < fVar1) * '\x02' | fVar3 < fVar1) * '\x02' | fVar3 < fVar2) << 2)
          & 3;
  local_3c = (float)(uVar5 == 0);
  local_38 = (float)(uVar5 == 1);
  local_34 = (float)(uVar5 == 2);
  FUN_00546d10(&local_30,(int)&local_3c,1,(float *)(iVar4 + 0x74));
  fVar6 = (float10)FUN_00563c80(iVar4,&local_30,0,0x7f7fffff,(ushort *)&param_1);
  local_30 = local_3c * _DAT_005cc33c;
  local_2c = local_38 * _DAT_005cc33c;
  local_28 = local_34 * _DAT_005cc33c;
  fVar7 = (float10)FUN_00563c80(iVar4,&local_30,0,0x7f7fffff,(ushort *)&param_1);
  *(uint *)(iVar4 + 0x88) = *(uint *)(iVar4 + 0x88) | 2;
  *(float *)(iVar4 + 0x58) = (float)(fVar7 + (float10)(float)fVar6);
  return;
}
