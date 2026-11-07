// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d55b0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs21_20261107/results.tsv. */
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
typedef unsigned char byte;
typedef int code();
#ifndef __cplusplus
#define true 1
#endif
#define CONCAT11(a,b) ((ushort)(((uint)(uchar)(a)<<8)|(uchar)(b)))
#define CONCAT21(a,b) ((uint)(((uint)(ushort)(a)<<8)|(uchar)(b)))
#define CONCAT31(a,b) ((uint)(((uint)(a)<<8)|(uchar)(b)))
MGLOBAL(float, DAT_007d6c24, 0x007d6c24);
MGLOBAL(float, DAT_007d6c28, 0x007d6c28);
MGLOBAL(int, DAT_007d6c20, 0x007d6c20);
MGLOBAL(uint, DAT_007d6c1c, 0x007d6c1c);
MGLOBAL(float, DAT_007d6c00, 0x007d6c00);
MGLOBAL(float, DAT_006181a0, 0x006181a0);
MGLOBAL(float, DAT_007d6c04, 0x007d6c04);
MGLOBAL(float, DAT_006181a4, 0x006181a4);
MGLOBAL(float, DAT_007d6c08, 0x007d6c08);
MGLOBAL(float, DAT_006181a8, 0x006181a8);
MGLOBAL(undefined1, DAT_005ceb90, 0x005ceb90);
MGLOBAL(undefined4, DAT_007d5120, 0x007d5120);
MGLOBAL(float, DAT_007d5124, 0x007d5124);
MGLOBAL(undefined1, DAT_007d5128, 0x007d5128);
MGLOBAL(undefined1, DAT_007d512c, 0x007d512c);
MGLOBAL(int, DAT_007d5c50, 0x007d5c50);
MGLOBAL(int, DAT_007d5c54, 0x007d5c54);
MGLOBAL(int, DAT_007d6c14, 0x007d6c14);
MGLOBAL(int, DAT_007d5c60, 0x007d5c60);
MGLOBAL(int, DAT_007d5c64, 0x007d5c64);
MGLOBAL(int, DAT_007d5c90, 0x007d5c90);
MGLOBAL(int, DAT_007d5c94, 0x007d5c94);
MGLOBAL(int, DAT_007d5c98, 0x007d5c98);
MGLOBAL(int, DAT_007d5c9c, 0x007d5c9c);
MGLOBAL(undefined1, DAT_007d5130, 0x007d5130);
MGLOBAL(undefined1, DAT_007d5134, 0x007d5134);
MGLOBAL(undefined1, DAT_007d5138, 0x007d5138);
MGLOBAL(undefined1, DAT_007d513c, 0x007d513c);
MGLOBAL(undefined1, DAT_007d5150, 0x007d5150);
MGLOBAL(undefined1, DAT_007d5154, 0x007d5154);
MGLOBAL(undefined1, DAT_007d515c, 0x007d515c);
MGLOBAL(undefined1, DAT_007d5158, 0x007d5158);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBAL(undefined1, DAT_007d5140, 0x007d5140);
MGLOBAL(undefined1, DAT_007d5144, 0x007d5144);
MGLOBAL(undefined1, DAT_007d5148, 0x007d5148);
MGLOBAL(undefined1, DAT_007d514c, 0x007d514c);
MGLOBAL(undefined1, DAT_007d5160, 0x007d5160);
MGLOBAL(undefined4, DAT_007d5168, 0x007d5168);
MGLOBAL(int, _DAT_007d512c, 0x007d512c);
MGLOBAL(int, _DAT_007d5128, 0x007d5128);
MGLOBAL(int, _DAT_007d5130, 0x007d5130);
MGLOBAL(int, _DAT_007d5134, 0x007d5134);
MGLOBAL(int, _DAT_007d5138, 0x007d5138);
MGLOBAL(int, _DAT_005ceb90, 0x005ceb90);
MGLOBAL(int, _DAT_007d5158, 0x007d5158);
MGLOBAL(int, _DAT_007d5150, 0x007d5150);
MGLOBAL(int, _DAT_007d5154, 0x007d5154);
MGLOBAL(int, _DAT_007d515c, 0x007d515c);
MGLOBAL(int, _DAT_007d513c, 0x007d513c);
MGLOBAL(int, _DAT_007d5140, 0x007d5140);
MGLOBAL(int, _DAT_007d5144, 0x007d5144);
MGLOBAL(int, _DAT_007d5148, 0x007d5148);
MGLOBAL(int, _DAT_007d514c, 0x007d514c);
MGLOBAL(int, _DAT_007d5160, 0x007d5160);
// MATCH: 0x004d55b0 FUN_004d55b0
/* [C1 2026-06-01] **Cache check** (0x004d55b0): compares DAT_007d6c24/28/20/1c/00/04/08 against
   incoming params; if all
 */

bool FUN_004d55b0(float *param_1,byte *param_2,uint param_3)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = param_3 & 0x48;
  if (((((DAT_007d6c24 == param_1[2]) && (DAT_007d6c28 == *param_1)) &&
       (DAT_007d6c20 == *(int *)param_2)) &&
      ((DAT_007d6c1c == uVar2 && (DAT_007d6c00 == DAT_006181a0)))) &&
     ((DAT_007d6c04 == DAT_006181a4 && (DAT_007d6c08 == DAT_006181a8)))) {
    return true;
  }
  DAT_007d6c24 = param_1[2];
  DAT_007d6c28 = *param_1;
  DAT_007d6c20 = *(int *)param_2;
  DAT_007d6c00 = DAT_006181a0;
  DAT_007d6c04 = DAT_006181a4;
  DAT_007d6c08 = DAT_006181a8;
  DAT_007d6c1c = uVar2;
  if (((param_3 & 0x40) == 0) || (*(int *)param_2 == -1)) {
    DAT_007d5120 = param_1[2];
    _DAT_007d512c = 1.0;
    DAT_007d5124 = DAT_007d5120;
    _DAT_007d5128 = DAT_007d5120;
    if ((DAT_007d5c50 != -1) && (DAT_007d5c50 = -1, DAT_007d5c54 == 0)) {
      DAT_007d5c54 = 1;
      (&DAT_007d5168)[DAT_007d6c14] = 0x8b;
      DAT_007d6c14 = DAT_007d6c14 + 1;
    }
    if ((DAT_007d5c90 != 0) && (DAT_007d5c90 = 0, DAT_007d5c94 == 0)) {
      (&DAT_007d5168)[DAT_007d6c14] = 0x93;
      DAT_007d6c14 = DAT_007d6c14 + 1;
      DAT_007d5c94 = 1;
    }
    if ((param_3 & 8) == 0) {
      if ((DAT_007d5c60 != 0) && (DAT_007d5c60 = 0, DAT_007d5c64 == 0)) {
        (&DAT_007d5168)[DAT_007d6c14] = 0x8d;
        DAT_007d6c14 = DAT_007d6c14 + 1;
        DAT_007d5c64 = 1;
      }
      if (DAT_007d5c98 != 0) {
        DAT_007d5c98 = 0;
        goto LAB_004d5ac7;
      }
    }
    else {
      if ((DAT_007d5c60 != 1) && (DAT_007d5c60 = 1, DAT_007d5c64 == 0)) {
        (&DAT_007d5168)[DAT_007d6c14] = 0x8d;
        DAT_007d6c14 = DAT_007d6c14 + 1;
        DAT_007d5c64 = 1;
      }
      if (DAT_007d5c98 != 1) {
        DAT_007d5c98 = 1;
LAB_004d5ac7:
        if (DAT_007d5c9c == 0) {
          (&DAT_007d5168)[DAT_007d6c14] = 0x94;
          DAT_007d6c14 = DAT_007d6c14 + 1;
          DAT_007d5c9c = 1;
        }
      }
    }
    if (*param_1 == 1.0) {
      _DAT_007d5130 = DAT_006181a0;
      _DAT_007d5134 = DAT_006181a4;
      _DAT_007d5138 = DAT_006181a8;
    }
    else {
      _DAT_007d5130 = DAT_006181a0 * *param_1;
      _DAT_007d5134 = DAT_006181a4 * *param_1;
      _DAT_007d5138 = DAT_006181a8 * *param_1;
    }
  }
  else {
    _DAT_007d5128 = param_1[2] * _DAT_005ceb90;
    DAT_007d5120 = (float)*param_2 * _DAT_007d5128;
    DAT_007d5124 = (float)param_2[1] * _DAT_007d5128;
    _DAT_007d5128 = (float)param_2[2] * _DAT_007d5128;
    _DAT_007d512c = (float)param_2[3] * _DAT_005ceb90;
    _DAT_007d5158 = *param_1 * _DAT_005ceb90;
    if ((param_3 & 8) != 0) {
      iVar1 = CONCAT31(CONCAT21(CONCAT11(param_2[3],*param_2),param_2[1]),param_2[2]);
      if ((DAT_007d5c50 != iVar1) && (DAT_007d5c50 = iVar1, DAT_007d5c54 == 0)) {
        DAT_007d5c54 = 1;
        (&DAT_007d5168)[DAT_007d6c14] = 0x8b;
        DAT_007d6c14 = DAT_007d6c14 + 1;
      }
      if ((DAT_007d5c60 != 1) && (DAT_007d5c60 = 1, DAT_007d5c64 == 0)) {
        (&DAT_007d5168)[DAT_007d6c14] = 0x8d;
        DAT_007d6c14 = DAT_007d6c14 + 1;
        DAT_007d5c64 = 1;
      }
      if ((DAT_007d5c90 != 1) && (DAT_007d5c90 = 1, DAT_007d5c94 == 0)) {
        (&DAT_007d5168)[DAT_007d6c14] = 0x93;
        DAT_007d6c14 = DAT_007d6c14 + 1;
        DAT_007d5c94 = 1;
      }
      if ((DAT_007d5c98 != 0) && (DAT_007d5c98 = 0, DAT_007d5c9c == 0)) {
        (&DAT_007d5168)[DAT_007d6c14] = 0x94;
        DAT_007d6c14 = DAT_007d6c14 + 1;
        DAT_007d5c9c = 1;
      }
      _DAT_007d5130 = 0.0;
      _DAT_007d5134 = 0.0;
      _DAT_007d5138 = 0.0;
      _DAT_007d5150 = (float)*param_2 * DAT_006181a0 * _DAT_007d5158;
      _DAT_007d5154 = (float)param_2[1] * DAT_006181a4 * _DAT_007d5158;
      _DAT_007d5158 = (float)param_2[2] * DAT_006181a8 * _DAT_007d5158;
      goto LAB_004d5b6e;
    }
    if ((DAT_007d5c50 != -1) && (DAT_007d5c50 = -1, DAT_007d5c54 == 0)) {
      DAT_007d5c54 = 1;
      (&DAT_007d5168)[DAT_007d6c14] = 0x8b;
      DAT_007d6c14 = DAT_007d6c14 + 1;
    }
    if ((DAT_007d5c60 != 0) && (DAT_007d5c60 = 0, DAT_007d5c64 == 0)) {
      (&DAT_007d5168)[DAT_007d6c14] = 0x8d;
      DAT_007d6c14 = DAT_007d6c14 + 1;
      DAT_007d5c64 = 1;
    }
    if ((DAT_007d5c90 != 0) && (DAT_007d5c90 = 0, DAT_007d5c94 == 0)) {
      (&DAT_007d5168)[DAT_007d6c14] = 0x93;
      DAT_007d6c14 = DAT_007d6c14 + 1;
      DAT_007d5c94 = 1;
    }
    if ((DAT_007d5c98 != 0) && (DAT_007d5c98 = 0, DAT_007d5c9c == 0)) {
      (&DAT_007d5168)[DAT_007d6c14] = 0x94;
      DAT_007d6c14 = DAT_007d6c14 + 1;
      DAT_007d5c9c = 1;
    }
    _DAT_007d5130 = (float)*param_2 * DAT_006181a0 * _DAT_007d5158;
    _DAT_007d5134 = (float)param_2[1] * DAT_006181a4 * _DAT_007d5158;
    _DAT_007d5138 = (float)param_2[2] * DAT_006181a8 * _DAT_007d5158;
  }
  _DAT_007d5158 = 0.0;
  _DAT_007d5154 = 0.0;
  _DAT_007d5150 = 0.0;
LAB_004d5b6e:
  _DAT_007d515c = 0x3f800000;
  _DAT_007d513c = 0x3f800000;
  _DAT_007d5140 = 0;
  _DAT_007d5144 = 0;
  _DAT_007d5148 = 0;
  _DAT_007d514c = 0x3f800000;
  _DAT_007d5160 = 0;
  iVar1 = (**(code **)(*DAT_007d4110 + 0xc4))(DAT_007d4110,&DAT_007d5120);
  return -1 < iVar1;
}
