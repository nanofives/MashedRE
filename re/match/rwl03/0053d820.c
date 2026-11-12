// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x0053d820: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
#ifdef __cplusplus
typedef int (__cdecl code)(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
uint FUN_004cbd30(undefined4 p0, undefined4 p1, uint p2);
undefined4 FUN_004cc5e0(undefined4 p0, int p1, void * p2, void * p3);
undefined4 FUN_004cc790(undefined4 p0, undefined4 p1, undefined4 p2);
uint * FUN_0053d5a0(ushort p0, ushort p1, uint * p2, uint p3);
typedef struct { uint a; uint b[6]; uint c; uint d; } HdrA;
typedef struct { uint v[6]; } HdrB;
// MATCH: 0x0053d820 FUN_0053d820
byte * FUN_0053d820(undefined4 param_1)
{
  undefined4 uVar1;
  int iVar2;
  byte *pbVar3;
  int iVar4;
  HdrB B;
  HdrA A;

  uVar1 = param_1;
  iVar2 = FUN_004cc5e0(param_1,1,0,&param_1);
  if (iVar2 == 0) {
    return (byte *)0x0;
  }
  iVar2 = FUN_004cbd30(uVar1,(undefined4)&A,0x24);
  if (iVar2 != 0x24) {
    return (byte *)0x0;
  }
  B.v[3] = A.b[0];
  B.v[4] = A.b[1];
  B.v[5] = A.b[2];
  B.v[0] = A.b[3];
  B.v[1] = A.b[4];
  B.v[2] = A.b[5];
  pbVar3 = (byte *)FUN_0053d5a0((ushort)A.c,(ushort)A.d,(uint *)&B,A.a);
  if (pbVar3 == (byte *)0x0) {
    return (byte *)0x0;
  }
  if ((*(ushort *)(pbVar3 + 0x1e) == 0) ||
     (iVar2 = (uint)*(ushort *)(pbVar3 + 0x1e) * 0x10,
     iVar4 = FUN_004cbd30(uVar1,*(undefined4 *)(pbVar3 + 0x20),iVar2), iVar4 == iVar2)) {
    if ((*pbVar3 & 1) == 0 || FUN_004cc790(uVar1,*(undefined4 *)(pbVar3 + 0x24),(uint)*(ushort *)(pbVar3 + 0x1c) * 2) != 0) {
      return pbVar3;
    }
  }
  (**(code **)(DAT_007d3ff8 + 0x10c))(pbVar3);
  return (byte *)0x0;
}
