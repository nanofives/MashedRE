// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x0054b740: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl04_20261107/results.tsv. */
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
undefined4 FUN_004cc5e0(undefined4 p0, int p1, undefined4 * p2, uint * p3);
undefined4 FUN_004cc790(undefined4 p0, undefined4 p1, undefined4 p2);
int * FUN_004f3e90(undefined4 p0, void * p1);
undefined4 * FUN_005493e0(void * p0);
// MATCH: 0x0054b740 FUN_0054b740
undefined4 * FUN_0054b740(undefined4 param_1)

{
  uint uVar1;
  uint iVar2;
  int iVar3;
  undefined4 *puVar4;
  uint buf[5];
  
  iVar3 = FUN_004cc790(param_1,(undefined4)buf,0x14);
  if (iVar3 == 0) {
    return (undefined4 *)iVar3;
  }
  puVar4 = (undefined4 *)FUN_005493e0(buf);
  uVar1 = puVar4[0xb];
  iVar2 = puVar4[0xf];
  if ((uVar1 & 1) != 0) {
    iVar3 = FUN_004cc790(iVar3,*puVar4,buf[1] * 0xc);
    if (iVar3 == 0) {
      return (undefined4 *)iVar3;
    }
  }
  if ((uVar1 & 2) != 0) {
    iVar3 = FUN_004cc790(iVar3,puVar4[1],buf[1] * 0xc);
    if (iVar3 == 0) {
      return (undefined4 *)iVar3;
    }
  }
  if ((uVar1 & 4) != 0) {
    iVar3 = FUN_004cc790(iVar3,puVar4[2],buf[1] * 4);
    if (iVar3 == 0) {
      return (undefined4 *)iVar3;
    }
  }
  if (iVar2 > 0) {
    iVar3 = FUN_004cc790(iVar3,puVar4[3],buf[1] * iVar2 * 8);
    if (iVar3 == 0) {
      return (undefined4 *)iVar3;
    }
  }
  if (buf[3] > 0) {
    iVar3 = FUN_004cc790(iVar3,puVar4[0x16],buf[3] * 0x44);
    if (iVar3 == 0) {
      return (undefined4 *)iVar3;
    }
  }
  if (buf[2] > 0) {
    iVar3 = FUN_004cc790(iVar3,puVar4[0x17],buf[2] * 0x2c);
    if (iVar3 == 0) {
      return (undefined4 *)iVar3;
    }
  }
  FUN_004cc5e0(iVar3,8,0,0);
  if (iVar3 == 0) {
    return (undefined4 *)iVar3;
  }
  FUN_004f3e90(iVar3,puVar4 + 0x12);
  return puVar4;
}
