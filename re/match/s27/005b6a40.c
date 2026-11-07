// FLAGS: /O2 /Ob1 /G5
// MATCH-S27 0x005b6a40: 40.8% (regs 85.9%, ops 98.6%). File written from the raw g2s body with MGLOBALA tables. Gain 11.8 -> 40.8% from storing the nibble through the dead param slot (*(byte *)&param_1) as the original does (mov byte [esp+0x10], al). Residual: eax/edx role swap of diff and nibble, permuter (77 variants) no gain. Class: b.
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
typedef unsigned int uint;
typedef unsigned char byte;
MGLOBALA(short, DAT_00634498, 89, 0x00634498);
MGLOBALA(short, DAT_00634478, 16, 0x00634478);
// MATCH: 0x005b6a40 FUN_005b6a40
void FUN_005b6a40(short param_1,int *param_2,int *param_3)
{
  int iVar2;
  int iVar3;
  int iVar4;
  byte d;
  byte *pd;

  iVar2 = (int)DAT_00634498[*param_3];
  iVar4 = (int)param_1;
  pd = (byte *)&param_1;
  iVar3 = iVar4 - *param_2;
  d = 0;
  *pd = d;
  if (iVar3 < 0) {
    d = 8;
    *pd = d;
    iVar3 = -iVar3;
  }
  if (iVar3 >= iVar2) {
    d |= 4;
    iVar3 -= iVar2;
    *pd = d;
  }
  iVar2 >>= 1;
  if (iVar3 >= iVar2) {
    d |= 2;
    iVar3 -= iVar2;
    *pd = d;
  }
  iVar2 >>= 1;
  if (iVar3 >= iVar2) {
    d |= 1;
    iVar3 -= iVar2;
    *pd = d;
  }
  iVar2 >>= 1;
  if ((d & 8) != 0) {
    *param_2 = (iVar4 - iVar2) + iVar3;
  } else {
    *param_2 = (iVar2 - iVar3) + iVar4;
  }
  if (*param_2 > 0x7fff) {
    *param_2 = 0x7fff;
  } else if (*param_2 < -0x8000) {
    *param_2 = -0x8000;
  }
  iVar2 = *param_3 + (int)DAT_00634478[*pd];
  *param_3 = iVar2;
  if (iVar2 < 0) {
    *param_3 = 0;
    return;
  }
  if (iVar2 > 0x58) {
    *param_3 = 0x58;
  }
}
