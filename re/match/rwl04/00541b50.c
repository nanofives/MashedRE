// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00541b50: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char byte;
MGLOBAL(int *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined1, DAT_005e4848, 0x005e4848);
int FUN_004c0ed0(int p0);
undefined4 * FUN_004c4600(void *p0, void *p1, void *p2);
float * FUN_004c4dc0(void *p0, undefined4 p1);
int FUN_004cb330(int p0, void *p1);
int FUN_004cff00(int p0);
void FUN_004d54f0(int p0, int p1, int p2);
undefined4 FUN_004d6e70(undefined4 *p0, int p1);
// MATCH: 0x00541b50 FUN_00541b50

undefined4 FUN_00541b50(undefined4 *param_1,int param_2,int param_3)
{
  int iVar1;
  float *pfVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  float m[16];
  undefined1 local_80 [64];
  undefined1 local_40 [64];
  FUN_004d6e70(param_1,param_2);
  iVar1 = FUN_004cff00(*param_1);
  if (iVar1 != 0) {
    FUN_004d54f0(param_2,0x18,3);
    FUN_004d54f0(param_2,0xb,0x30000);
    pfVar2 = (float *)FUN_004c0ed0(*(undefined4 *)(*DAT_007d3ff8 + 4));
    m[0] = -*pfVar2;
    m[1] = -pfVar2[1];
    m[2] = -pfVar2[2];
    m[4] = pfVar2[4];
    m[5] = pfVar2[5];
    m[6] = pfVar2[6];
    m[8] = pfVar2[8];
    m[9] = pfVar2[9];
    m[10] = pfVar2[10];
    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = 0.0f;
    m[3] = 0.0f;
    m[7] = 0.0f;
    m[11] = 0.0f;
    m[15] = 1.0f;
    FUN_004cb330(param_2 + 0x10,m);
    return 1;
  }
  FUN_004d54f0(param_2,0x18,2);
  FUN_004d54f0(param_2,0xb,0x10000);
  if (param_3 != 0) {
    uVar3 = FUN_004c0ed0(*(undefined4 *)(*DAT_007d3ff8 + 4));
    uVar4 = FUN_004c0ed0(param_3);
    FUN_004c4dc0(local_80,uVar4);
    FUN_004c4600(m,(void *)uVar3,local_80);
    m[0] = -m[0];
    m[1] = -m[1];
    m[2] = -m[2];
    m[3] = 0.0f;
    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = 0.0f;
    FUN_004c4600(local_40,m,&DAT_005e4848);
    FUN_004cb330(param_2 + 0x10,local_40);
    return 1;
  }
  FUN_004cb330(param_2 + 0x10,&DAT_005e4848);
  return 1;
}
