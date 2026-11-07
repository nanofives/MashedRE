// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d7480 (h4): 96.2% (regs 97.8%, ops 99.6%). The file also holds FUN_004d5480 (100%, non-static, defined first): with it in the same TU, 9176 keeps value in edx across the call in case 0x1d as the original does (Ghidra's extraout_EDX). As a static it took register args. Moved from the raw 52.7% by: case bodies in the original's memory order (0xe,0xf,0x10,0x11,2,3,4,9,1,8,6,0xa,0xb,7,0xc,0xd,5,0x14..0x1e, from the jump table at 0x004d7a48), SetSamplerState (+0x114) typed __stdcall, the value != 0 branch first in cases 8 and 0xe, case 0x10 as one if (value != current) block with a shared return 1, case 0x11 reusing the param slot for the float (param_2 = *(int *)param_2), store order in case 2. Residual (b): case 0x10 loads the 0x5d8c54 table entry after the dirty-flag load in the original (ours before, and edx/eax swapped in the else arm), and case 0x11 stores DAT_007d5928 before the 0x7d592c test via eax=edx (ours after, ecx/eax). Tried 4 case-0x11 forms (91-92%), permuter 245 compiles 0 gains. Table and queue lengths in MGLOBALA are placeholders [UNCERTAIN]. Class (b). */
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
typedef long (__stdcall *SetSamplerFn)(int *dev, unsigned int sampler, unsigned int type, int value);
MGLOBAL(int, DAT_007d6bf8, 0x007d6bf8);
MGLOBAL(int, DAT_007d58d0, 0x007d58d0);
MGLOBAL(int, DAT_007d6bfc, 0x007d6bfc);
MGLOBAL(int, DAT_007d58c0, 0x007d58c0);
MGLOBAL(int, DAT_007d58c4, 0x007d58c4);
MGLOBAL(int, DAT_007d6c14, 0x007d6c14);
MGLOBAL(float, DAT_007d6b24, 0x007d6b24);
MGLOBAL(float, DAT_007d5928, 0x007d5928);
MGLOBAL(int, DAT_007d592c, 0x007d592c);
MGLOBAL(int, DAT_007d6b20, 0x007d6b20);
MGLOBAL(int, DAT_00911fc4, 0x00911fc4);
MGLOBAL(int, DAT_007d5914, 0x007d5914);
MGLOBAL(int, DAT_007d5910, 0x007d5910);
MGLOBAL(int, DAT_007d5c58, 0x007d5c58);
MGLOBAL(int, DAT_007d5c5c, 0x007d5c5c);
MGLOBAL(int, DAT_007d6b28, 0x007d6b28);
MGLOBAL(int, DAT_007d590c, 0x007d590c);
MGLOBAL(int, DAT_007d5908, 0x007d5908);
MGLOBAL(int, DAT_007d6b1c, 0x007d6b1c);
MGLOBAL(int, DAT_007d58dc, 0x007d58dc);
MGLOBAL(int, DAT_007d58d8, 0x007d58d8);
MGLOBAL(int, DAT_007d6b40, 0x007d6b40);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBAL(int, DAT_007d4f70, 0x007d4f70);
MGLOBAL(int, DAT_007d6ae8, 0x007d6ae8);
MGLOBAL(int, DAT_007d6aec, 0x007d6aec);
MGLOBAL(int, DAT_007d5834, 0x007d5834);
MGLOBAL(int, DAT_007d5830, 0x007d5830);
MGLOBAL(int, DAT_007d586c, 0x007d586c);
MGLOBAL(int, DAT_007d5868, 0x007d5868);
MGLOBAL(int, DAT_007d6b38, 0x007d6b38);
MGLOBAL(int, DAT_007d4f68, 0x007d4f68);
MGLOBAL(int, DAT_007d6b34, 0x007d6b34);
MGLOBAL(int, DAT_007d4f64, 0x007d4f64);
MGLOBALA(int, DAT_005d8c54, 16, 0x005d8c54);
MGLOBALA(int, DAT_005d8c94, 16, 0x005d8c94);
MGLOBALA(int, DAT_005d8d4c, 16, 0x005d8d4c);
MGLOBALA(int, DAT_007d5168, 64, 0x007d5168);
int FUN_004d6b90(int value);
int FUN_004d6c40(int value);
int FUN_004d6c90(int value);
int FUN_004d7200(int value);
int FUN_004d7250(int value);
int FUN_004d72a0(int value);
int FUN_004d72f0(int value);
int FUN_004d7340(int value);
int FUN_004d7390(int value);
int FUN_004d73e0(int value);
int FUN_004d7430(int value);
int FUN_004d7bc0(int value);
int FUN_004d7c10(int value);
int FUN_004d7100(int value);
int FUN_004d7ac0(int value);
int FUN_004d6ce0(int tex, int stage);
MGLOBALA(int, DAT_007d57f8, 64, 0x007d57f8);
// MATCH: 0x004d5480 FUN_004d5480
void FUN_004d5480(int param_1,int param_2)
{
  if ((DAT_007d57f8[param_1 * 2] != param_2) &&
     (DAT_007d57f8[param_1 * 2] = param_2, DAT_007d57f8[param_1 * 2 + 1] == 0)) {
    DAT_007d57f8[param_1 * 2 + 1] = 1;
    DAT_007d5168[DAT_007d6c14] = param_1;
    DAT_007d6c14 = DAT_007d6c14 + 1;
  }
  return;
}
// MATCH: 0x004d7480 FUN_004d7480
int FUN_004d7480(int param_1,int param_2)

{
  int uVar2;
  
  uVar2 = 0;
  switch(param_1) {
  case 0xe:
    if (param_2 != 0) {
      if ((DAT_007d6b1c == 0) && ((DAT_00911fc4 & 0x180) != 0)) {
        DAT_007d58d8 = 1;
      if (DAT_007d58dc == 0) {
        DAT_007d58dc = 1;
        DAT_007d5168[DAT_007d6c14] = 0x1c;
        DAT_007d6c14 = DAT_007d6c14 + 1;
      }
        DAT_007d6b1c = 1;
        return 1;
      }
    }
    else if (DAT_007d6b1c != 0) {
      DAT_007d58d8 = 0;
      if (DAT_007d58dc == 0) {
        DAT_007d58dc = 1;
        DAT_007d5168[DAT_007d6c14] = 0x1c;
        DAT_007d6c14 = DAT_007d6c14 + 1;
      }
      DAT_007d6b1c = 0;
    }
    return 1;
  case 0xf:
    if (param_2 != DAT_007d6b28) {
      DAT_007d5908 = param_2;
      if (DAT_007d590c == 0) {
        DAT_007d590c = 1;
        DAT_007d5168[DAT_007d6c14] = 0x22;
        DAT_007d6c14 = DAT_007d6c14 + 1;
      }
      DAT_007d6b28 = param_2;
    }
    return 1;
  case 0x10:
    if (param_2 != DAT_007d6b20) {
      if (((DAT_00911fc4 & 0x100) != 0) && ((DAT_00911fc4 & 0x100000) != 0)) {
        DAT_007d5910 = DAT_005d8c54[param_2];
        if (DAT_007d5914 == 0) {
          DAT_007d5914 = 1;
          DAT_007d5168[DAT_007d6c14] = 0x23;
          DAT_007d6c14 = DAT_007d6c14 + 1;
        }
        if (DAT_007d5c58 != 0) {
          DAT_007d5c58 = 0;
          if (DAT_007d5c5c == 0) {
            DAT_007d5c5c = 1;
            DAT_007d5168[DAT_007d6c14] = 0x8c;
            DAT_007d6c14 = DAT_007d6c14 + 1;
          }
        }
      }
      else {
        if ((DAT_007d5910 != 0) && (DAT_007d5910 = 0, DAT_007d5914 == 0)) {
          DAT_007d5914 = 1;
          DAT_007d5168[DAT_007d6c14] = 0x23;
          DAT_007d6c14 = DAT_007d6c14 + 1;
        }
        DAT_007d5c58 = DAT_005d8c54[param_2];
        if (DAT_007d5c5c == 0) {
          DAT_007d5c5c = 1;
          DAT_007d5168[DAT_007d6c14] = 0x8c;
          DAT_007d6c14 = DAT_007d6c14 + 1;
        }
      }
      DAT_007d6b20 = param_2;
    }
    return 1;
  case 0x11:
    param_2 = *(int *)param_2;
    if (*(float *)&param_2 != DAT_007d6b24) {
      DAT_007d5928 = *(float *)&param_2;
      if (DAT_007d592c == 0) {
        DAT_007d592c = 1;
        DAT_007d5168[DAT_007d6c14] = 0x26;
        DAT_007d6c14 = DAT_007d6c14 + 1;
      }
      DAT_007d6b24 = *(float *)&param_2;
    }
    return 1;
  case 2:
    if (param_2 != DAT_007d6b34) {
      DAT_007d6b34 = param_2;
      DAT_007d4f64 = DAT_005d8c94[param_2];
      (*(SetSamplerFn *)(*DAT_007d4110 + 0x114))(DAT_007d4110,0,1,DAT_007d4f64);
    }
    if (param_2 != DAT_007d6b38) {
      DAT_007d6b38 = param_2;
      DAT_007d4f68 = DAT_005d8c94[param_2];
      (*(SetSamplerFn *)(*DAT_007d4110 + 0x114))(DAT_007d4110,0,2,DAT_007d4f68);
    }
    return 1;
  case 3:
    if (param_2 != DAT_007d6b34) {
      DAT_007d6b34 = param_2;
      DAT_007d4f64 = DAT_005d8c94[param_2];
      (*(SetSamplerFn *)(*DAT_007d4110 + 0x114))(DAT_007d4110,0,1,DAT_007d4f64);
    }
    return 1;
  case 4:
    if (param_2 != DAT_007d6b38) {
      DAT_007d6b38 = param_2;
      DAT_007d4f68 = DAT_005d8c94[param_2];
      (*(SetSamplerFn *)(*DAT_007d4110 + 0x114))(DAT_007d4110,0,2,DAT_007d4f68);
      return 1;
    }
    break;
  case 9:
    uVar2 = FUN_004d6b90(param_2);
    return uVar2;
  case 1:
    uVar2 = FUN_004d6ce0(param_2,0);
    return uVar2;
  case 8:
    if (param_2 != 0) {
      if (DAT_007d6ae8 == 0) {
      if ((DAT_007d6aec == 0) && (DAT_007d5830 = 1, DAT_007d5834 == 0)) {
        DAT_007d5834 = 1;
        DAT_007d5168[DAT_007d6c14] = 7;
        DAT_007d6c14 = DAT_007d6c14 + 1;
      }
      DAT_007d5868 = 1;
      if (DAT_007d586c == 0) {
        DAT_007d586c = 1;
        DAT_007d5168[DAT_007d6c14] = 0xe;
        DAT_007d6c14 = DAT_007d6c14 + 1;
      }
        DAT_007d6ae8 = 1;
        return 1;
      }
    }
    else if (DAT_007d6ae8 != 0) {
      if ((DAT_007d6aec == 0) && (DAT_007d5830 = 0, DAT_007d5834 == 0)) {
        DAT_007d5834 = 1;
        DAT_007d5168[DAT_007d6c14] = 7;
        DAT_007d6c14 = DAT_007d6c14 + 1;
      }
      DAT_007d5868 = 0;
      if (DAT_007d586c == 0) {
        DAT_007d586c = 1;
        DAT_007d5168[DAT_007d6c14] = 0xe;
        DAT_007d6c14 = DAT_007d6c14 + 1;
      }
      DAT_007d6ae8 = 0;
    }
    return 1;
  case 6:
    uVar2 = FUN_004d7ac0(param_2);
    return uVar2;
  case 10:
    uVar2 = FUN_004d6c40(param_2);
    return uVar2;
  case 0xb:
    uVar2 = FUN_004d6c90(param_2);
    return uVar2;
  case 7:
    uVar2 = FUN_004d7bc0(param_2);
    return uVar2;
  case 0xc:
    uVar2 = FUN_004d7100(param_2);
    return uVar2;
  case 0xd:
    if (param_2 != DAT_007d6b40) {
      DAT_007d6b40 = param_2;
      DAT_007d4f70 = param_2;
      (*(SetSamplerFn *)(*DAT_007d4110 + 0x114))(DAT_007d4110,0,4,param_2);
      return 1;
    }
    break;
  case 5:
    return (param_2 != 0);
  case 0x14:
    uVar2 = FUN_004d7c10(param_2);
    return uVar2;
  case 0x15:
    uVar2 = FUN_004d7200(param_2);
    return uVar2;
  case 0x16:
    uVar2 = FUN_004d7250(param_2);
    return uVar2;
  case 0x17:
    uVar2 = FUN_004d72a0(param_2);
    return uVar2;
  case 0x18:
    uVar2 = FUN_004d72f0(param_2);
    return uVar2;
  case 0x19:
    uVar2 = FUN_004d7340(param_2);
    return uVar2;
  case 0x1a:
    uVar2 = FUN_004d7390(param_2);
    return uVar2;
  case 0x1b:
    uVar2 = FUN_004d73e0(param_2);
    return uVar2;
  case 0x1c:
    uVar2 = FUN_004d7430(param_2);
    return uVar2;
  case 0x1d:
    if (param_2 != DAT_007d6bf8) {
      DAT_007d6bf8 = param_2;
      DAT_007d6bfc = (param_2 != 8);
      if (DAT_007d58d0 != 0) {
        FUN_004d5480(0xf,DAT_007d6bfc);
      }
      DAT_007d58c0 = DAT_005d8d4c[param_2];
      if (DAT_007d58c4 == 0) {
        DAT_007d58c4 = 1;
        DAT_007d5168[DAT_007d6c14] = 0x19;
        DAT_007d6c14 = DAT_007d6c14 + 1;
      }
    }
    return 1;
  case 0x1e:
    FUN_004d5480(0x18,param_2);
    break;
  default:
    goto switchD_004d7494_caseD_12;
  }
  uVar2 = 1;
switchD_004d7494_caseD_12:
  return uVar2;
}
