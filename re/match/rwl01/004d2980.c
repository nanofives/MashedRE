// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004d2980: best 53.3% (regs ~56%, ops 64%), class (b). Texture creation (IDirect3DDevice9 slots 0x5c/0x64 via cdecl-typed Ghidra pointers, still to be typed per call) with the flag byte read as byte<<8 (xor eax,eax; mov ah) and the format tests on that word; strings as MGLOBALA char arrays (26 and 72 bytes). Remaining: the original hoists (flags & 0xf00) into eax before the 0x6000 test, the cube-texture branch is placed out of line, and the call-cleanup (stdcall vs cdecl per slot) is not yet matched. */
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
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
#define CONCAT11(a,b) ((ushort)(((uint)(uchar)(a)<<8)|(uchar)(b)))
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBAL(uint, DAT_007d46d0, 0x007d46d0);
MGLOBAL(uint, DAT_007d46cc, 0x007d46cc);
MGLOBAL(int, DAT_007d46d8, 0x007d46d8);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d46dc, 0x007d46dc);
MGLOBAL(undefined4, DAT_007d46e4, 0x007d46e4);
MGLOBAL(int *, DAT_007d46e0, 0x007d46e0);
MGLOBAL(uint, DAT_007d46d4, 0x007d46d4);
MGLOBAL(undefined1, DAT_005d8bf0, 0x005d8bf0);
MGLOBAL(undefined1, DAT_005d8bf5, 0x005d8bf5);
undefined4 FUN_004cc7f0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
undefined4 FUN_004d7ff0(undefined4 p0, ...);
undefined4 * FUN_004d8480(undefined4 * p0);
void FUN_004ddcb0(int * p0, int * p1, uint p2);
MGLOBALA(char, s_Failed_to_create_texture__0061832c, 26, 0x0061832c);
MGLOBALA(char, s_Palletized_texture_can_only_be_r_00618348, 72, 0x00618348);
// MATCH: 0x004d2980 FUN_004d2980
undefined4 FUN_004d2980(int param_1,int param_2)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  undefined4 uVar4;
  uint uVar6;
  uint uVar7;
  int iVar8;
  int *piVar9;
  char *pcVar10;
  undefined4 loc[2];
  
  uVar6 = (uint)*(byte *)(param_1 + 0x23) << 8;
  if ((uVar6 & 0x6000) == 0) {
    *(undefined4 *)(param_2 + 0x18) = *(undefined4 *)(&DAT_005d8bf0 + (uVar6 >> 8 & 0xf) * 8);
  }
  else {
    if (((uVar6 & 0xf00) != 0x600) && ((uVar6 & 0xf00) != 0x500)) {
      pcVar10 = s_Palletized_texture_can_only_be_r_00618348;
      goto LAB_004d2c0a;
    }
    if (DAT_007d46dc == 0) {
      DAT_007d46dc = FUN_004cc7f0(0x404,0x40,4,0x30411);
    }
    iVar8 = (**(code **)(DAT_007d3ff8 + 0x118))(DAT_007d46dc,0x30411);
    *(int *)(param_2 + 4) = iVar8;
    if (iVar8 != 0) {
      if (DAT_007d46cc == 0) {
        uVar7 = DAT_007d46d4;
        DAT_007d46d4 = DAT_007d46d4 + 1;
      }
      else {
        DAT_007d46cc = DAT_007d46cc - 1;
        uVar7 = (uint)*(ushort *)(DAT_007d46d8 + DAT_007d46cc * 2);
      }
      *(uint *)(*(int *)(param_2 + 4) + 0x400) = uVar7;
    }
    *(undefined4 *)(param_2 + 0x18) = 0x29;
  }
  puVar1 = (undefined4 *)(param_1 + 0x10);
  puVar2 = (undefined4 *)(param_1 + 0xc);
  *(undefined *)(param_2 + 8) = (&DAT_005d8bf5)[(uVar6 >> 8 & 0xf) * 8];
  FUN_004ddcb0((int *)puVar2,(int *)puVar1,(uVar6 | *(byte *)(param_1 + 0x20)));
  if ((*(byte *)(param_2 + 9) & 0xf) == 0) {
    iVar8 = *(int *)(param_2 + 0x18);
    if ((iVar8 < 0x46) || (0x50 < iVar8)) {
      iVar8 = (**(code **)(*DAT_007d4110 + 0x5c))
                        (DAT_007d4110,*puVar2,*puVar1,~uVar6 >> 0xf & 1,
                         -(uint)((*(byte *)(param_2 + 10) & 0xf) != 0) & 0x400,iVar8,1,param_2,0);
      goto LAB_004d2b53;
    }
    iVar8 = (**(code **)(*DAT_007d4110 + 0x5c))(DAT_007d4110,*puVar2,*puVar1,1,2,iVar8,0,param_2,0);
    if (-1 < iVar8) {
      piVar9 = (int *)(**(code **)(DAT_007d3ff8 + 0x118))(DAT_007d46e4,0x30411);
      *piVar9 = param_1;
      piVar9[1] = (int)DAT_007d46e0;
      DAT_007d46e0 = piVar9;
      goto LAB_004d2b53;
    }
  }
  else {
    iVar8 = (**(code **)(*DAT_007d4110 + 100))
                      (DAT_007d4110,*puVar2,~uVar6 >> 0xf & 1,
                       -(uint)((*(byte *)(param_2 + 10) & 0xf) != 0) & 0x400,
                       *(undefined4 *)(param_2 + 0x18),1,param_2,0);
LAB_004d2b53:
    if (-1 < iVar8) {
      if (*(int *)(param_2 + 4) != 0) {
        piVar9 = (int *)(**(code **)(DAT_007d3ff8 + 0x118))(DAT_007d46e4,0x30411);
        *piVar9 = param_1;
        piVar9[1] = (int)DAT_007d46e0;
        DAT_007d46e0 = piVar9;
      }
      return 1;
    }
  }
  if ((*(int *)(param_2 + 0x18) == 0x29) && (*(int *)(param_2 + 4) != 0)) {
    uVar4 = *(undefined4 *)(*(int *)(param_2 + 4) + 0x400);
    if (DAT_007d46d0 <= DAT_007d46cc) {
      DAT_007d46d0 = DAT_007d46d0 + 0x200;
      if (DAT_007d46d8 == 0) {
        DAT_007d46d8 = (**(code **)(DAT_007d3ff8 + 0x108))(DAT_007d46d0 * 2);
      }
      else {
        DAT_007d46d8 = (**(code **)(DAT_007d3ff8 + 0x110))(DAT_007d46d8,DAT_007d46d0 * 2,0x1030411);
      }
    }
    *(short *)(DAT_007d46d8 + DAT_007d46cc * 2) = (short)uVar4;
    DAT_007d46cc = DAT_007d46cc + 1;
    (**(code **)(DAT_007d3ff8 + 0x11c))(DAT_007d46dc,*(undefined4 *)(param_2 + 4));
    *(undefined4 *)(param_2 + 4) = 0;
  }
  pcVar10 = s_Failed_to_create_texture__0061832c;
LAB_004d2c0a:
  loc[0] = 1;
  loc[1] = FUN_004d7ff0(2,pcVar10);
  FUN_004d8480(loc);
  return 0;
}
