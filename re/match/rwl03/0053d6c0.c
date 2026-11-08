// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x0053d6c0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
int FUN_004cbe80(undefined4 p0, void * p1, uint p2);
undefined4 FUN_004cc580(undefined4 p0, undefined4 p1, undefined4 p2, uint p3, uint p4);
undefined4 FUN_004cc770(undefined4 p0, undefined4 p1, undefined4 p2);
typedef struct { uint x, y, z; } V3;
typedef struct { uint a0; V3 u; V3 w; uint c; uint d; } HdrA;
typedef struct { uint v[4]; } HdrB;
// MATCH: 0x0053d6c0 FUN_0053d6c0
byte * FUN_0053d6c0(byte *param_1,undefined4 param_2)
{
  int iVar1;
  undefined4 *puVar2;
  uint uVar3;
  int iVar4;
  ushort n;
  HdrB B;
  HdrA A;

  iVar4 = (*(ushort *)(param_1 + 0x1e) + 3) * 0x10;
  if ((*param_1 & 1) != 0) {
    iVar4 = iVar4 + (uint)*(ushort *)(param_1 + 0x1c) * 2;
  }
  iVar1 = FUN_004cc580(param_2,0x2c,iVar4,0x37002,10);
  if (iVar1 == 0) {
    return (byte *)0x0;
  }
  iVar4 = FUN_004cc580(param_2,1,iVar4 + -0xc,0x37002,10);
  if (iVar4 == 0) {
    return (byte *)0x0;
  }
  A.u = *(V3 *)(param_1 + 0x10);
  A.w = *(V3 *)(param_1 + 4);
  A.a0 = *(undefined4 *)param_1;
  A.c = (uint)*(ushort *)(param_1 + 0x1c);
  A.d = (uint)*(ushort *)(param_1 + 0x1e);
  iVar4 = FUN_004cbe80(param_2,&A,0x24);
  if (iVar4 != 0) {
    n = *(ushort *)(param_1 + 0x1e);
    if ((n > 0) && (uVar3 = 0, n > 0)) {
      iVar4 = 0;
      do {
        puVar2 = (undefined4 *)(*(int *)(param_1 + 0x20) + iVar4);
        B.v[0] = puVar2[0];
        B.v[1] = puVar2[1];
        B.v[2] = puVar2[2];
        B.v[3] = puVar2[3];
        iVar1 = FUN_004cbe80(param_2,&B,0x10);
        if (iVar1 == 0) {
          return (byte *)0x0;
        }
        uVar3 = uVar3 + 1;
        iVar4 = iVar4 + 0x10;
      } while (uVar3 < n);
    }
    if (((*param_1 & 1) != 0) &&
       (iVar4 = FUN_004cc770(param_2,*(undefined4 *)(param_1 + 0x24),
                             (uint)*(ushort *)(param_1 + 0x1c) * 2), iVar4 == 0)) {
      return (byte *)0x0;
    }
    return param_1;
  }
  return (byte *)0x0;
}
