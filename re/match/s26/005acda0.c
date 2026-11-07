// FLAGS: /O2 /Op /Ob1 /G5
/* MATCH-S26 0x005acda0: 44.3% (regs 50.0%, ops 67.1%), 55 vs 70 insns, FLAGS /O2 /Op. Raw gotos kept, callees typed, float global as MGLOBAL(float), (uint)(float expr) cast. Residual: the original converts the float to uint with an INLINE fnstcw/or ah,0xc/fldcw/fistp/fldcw sequence (and does fstp dword/fld around it), where the DDK calls _ftol; also the uVar2/1 division is kept literal (mov ecx,1; div ecx) with a duplicated epilogue. Tried (int) cast, /Op /QIfist /Oa /Oi /G6 /G7 /Ox. Class (a) for the inline float->uint truncation. cxx pending. */
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
typedef unsigned int uint;
typedef unsigned char byte;
MGLOBALA(byte, DAT_005e6454, 1, 0x005e6454);
MGLOBALA(byte, DAT_005e64b4, 1, 0x005e64b4);
MGLOBAL(float, DAT_005e64c4, 0x005e64c4);
uint FUN_005ac650(int p0);
uint FUN_005ace70(int p0);
int FUN_005adf30(byte * p0, byte * p1);
// MATCH: 0x005acda0 FUN_005acda0
uint FUN_005acda0(int param_1,uint param_2)
{
  int iVar1;
  uint uVar2;
  int iVar3;

  iVar1 = FUN_005ace70(param_1);
  uVar2 = FUN_005ac650(param_1);
  iVar3 = FUN_005adf30(*(byte **)(param_1 + 4),DAT_005e6454);
  if (iVar3 == 0) {
LAB_005acde4:
    if (*(char *)(param_1 + 0xd) == '\x02') {
      uVar2 = (uint)(((float)param_2 / (float)(int)uVar2) * DAT_005e64c4);
      goto LAB_005ace44;
    }
  }
  else {
    iVar3 = FUN_005adf30(*(byte **)(param_1 + 4),DAT_005e64b4);
    if (iVar3 == 0) goto LAB_005acde4;
  }
  uVar2 = (param_2 / uVar2) * iVar1;
LAB_005ace44:
  if ((*(byte *)(param_1 + 0x18) & 4) != 0) {
    return uVar2 / 1;
  }
  return uVar2 / *(byte *)(param_1 + 0xd);
}
