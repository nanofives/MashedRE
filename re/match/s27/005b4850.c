// FLAGS: /O2 /Ob1 /G5
// MATCH-S27 0x005b4850: 15.2% (regs 34.3%), raw Ghidra shape (C++ fixes: ROUND macro for C++ only). Same inline float-truncation note as 0x005b41f0. Not iterated. Class: b.
/* MATCH-S27 0x005b4850: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs27_20261107/results.tsv. */
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;
typedef unsigned char byte;
#ifdef __cplusplus
#define ROUND(x) ((int)(x))
#endif
MGLOBAL(undefined1, DAT_005e6cd8, 0x005e6cd8);
MGLOBAL(undefined1, DAT_005cc9d0, 0x005cc9d0);
MGLOBAL(undefined1, DAT_005e6cd4, 0x005e6cd4);
MGLOBAL(undefined1, DAT_005e6cd0, 0x005e6cd0);
MGLOBAL(undefined1, DAT_005ce9fc, 0x005ce9fc);
MGLOBAL(undefined1, DAT_005e6ccc, 0x005e6ccc);
uint FUN_005b4150(int p0, int p1, int p2);
MGLOBAL(int, _DAT_005cc9d0, 0x005cc9d0);
MGLOBAL(int, _DAT_005e6cd8, 0x005e6cd8);
MGLOBAL(int, _DAT_005e6cd4, 0x005e6cd4);
MGLOBAL(int, _DAT_005ce9fc, 0x005ce9fc);
MGLOBAL(int, _DAT_005e6ccc, 0x005e6ccc);
MGLOBAL(int, _DAT_005e6cd0, 0x005e6cd0);
// MATCH: 0x005b4850 FUN_005b4850
/* [C2 2026-06-01] Bit-depth and signedness PCM converter (8?16 bit, signed?unsigned). A
   converter-node transform ([[U-6909]], reached via? */

uint FUN_005b4850(int param_1,int param_2,int param_3,int param_4,undefined4 *param_5,uint *param_6)

{
  char cVar1;
  uint uVar2;
  uint uVar3;
  byte bVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  uint *puVar8;
  undefined4 *puVar9;
  int iVar10;
  
  puVar8 = param_6;
  if (param_6 == (uint *)0x0) {
    puVar8 = (uint *)&param_6;
  }
  uVar2 = FUN_005b4150(param_1,param_3,param_4);
  *puVar8 = uVar2;
  puVar9 = (undefined4 *)(param_2 + param_3);
  if (param_5 != (undefined4 *)0x0) {
    param_5 = (undefined4 *)((int)param_5 + uVar2);
  }
  cVar1 = *(char *)(param_1 + 0xc);
  if ((cVar1 == '\b') && (*(char *)(param_4 + 0xc) == '\x10')) {
    iVar7 = *(int *)(param_1 + 8);
    uVar2 = iVar7 * 2;
    uVar3 = iVar7 - param_3;
    if ((*(byte *)(param_1 + 0x18) & 1) == 0) {
      if ((*(byte *)(param_4 + 0x18) & 1) != 0) {
        if ((param_5 != (undefined4 *)0x0) && (uVar5 = 0, iVar7 != param_3)) {
          do {
            (*(ushort *)((char *)&param_2 + 0)) =
                 (undefined2)
                 (int)ROUND(((float)(int)*(char *)(uVar5 + (int)puVar9) + _DAT_005cc9d0) *
                            _DAT_005e6cd8);
            *(undefined2 *)((int)param_5 + uVar5 * 2) = (undefined2)param_2;
            uVar5 = uVar5 + 1;
          } while (uVar5 < uVar3);
        }
        return uVar2;
      }
      if ((param_5 != (undefined4 *)0x0) && (uVar5 = 0, iVar7 != param_3)) {
        do {
          (*(ushort *)((char *)&param_2 + 0)) =
               (undefined2)(int)ROUND((float)(int)*(char *)(uVar5 + (int)puVar9) * _DAT_005e6cd4);
          *(undefined2 *)((int)param_5 + uVar5 * 2) = (undefined2)param_2;
          uVar5 = uVar5 + 1;
        } while (uVar5 < uVar3);
      }
      return uVar2;
    }
    if ((*(byte *)(param_4 + 0x18) & 1) != 0) {
      if ((param_5 != (undefined4 *)0x0) && (uVar5 = 0, iVar7 != param_3)) {
        do {
          (*(ushort *)((char *)&param_2 + 0)) =
               (undefined2)(int)ROUND((float)*(byte *)(uVar5 + (int)puVar9) * _DAT_005e6cd8);
          *(undefined2 *)((int)param_5 + uVar5 * 2) = (undefined2)param_2;
          uVar5 = uVar5 + 1;
        } while (uVar5 < uVar3);
      }
      return uVar2;
    }
    if ((param_5 != (undefined4 *)0x0) && (uVar5 = 0, iVar7 != param_3)) {
      do {
        (*(ushort *)((char *)&param_2 + 0)) =
             (undefined2)
             (int)ROUND(((float)*(byte *)(uVar5 + (int)puVar9) - _DAT_005cc9d0) * _DAT_005e6cd4);
        *(undefined2 *)((int)param_5 + uVar5 * 2) = (undefined2)param_2;
        uVar5 = uVar5 + 1;
      } while (uVar5 < uVar3);
    }
    return uVar2;
  }
  if ((cVar1 == '\x10') && (*(char *)(param_4 + 0xc) == '\b')) {
    uVar5 = *(uint *)(param_1 + 8) >> 1;
    uVar3 = uVar5 - uVar2;
    if ((*(byte *)(param_1 + 0x18) & 1) == 0) {
      if ((*(byte *)(param_4 + 0x18) & 1) != 0) {
        if ((param_5 != (undefined4 *)0x0) && (uVar6 = 0, uVar5 != uVar2)) {
          do {
            (*(uchar *)((char *)&param_1 + 0)) =
                 (undefined1)
                 (int)ROUND(((float)(int)*(short *)((int)puVar9 + uVar6 * 2) + _DAT_005ce9fc) *
                            _DAT_005e6ccc);
            *(undefined1 *)(uVar6 + (int)param_5) = (undefined1)param_1;
            uVar6 = uVar6 + 1;
          } while (uVar6 < uVar3);
        }
        return uVar5;
      }
      if ((param_5 != (undefined4 *)0x0) && (uVar6 = 0, uVar5 != uVar2)) {
        do {
          (*(uchar *)((char *)&param_1 + 0)) =
               (undefined1)
               (int)ROUND((float)(int)*(short *)((int)puVar9 + uVar6 * 2) * _DAT_005e6ccc);
          *(undefined1 *)(uVar6 + (int)param_5) = (undefined1)param_1;
          uVar6 = uVar6 + 1;
        } while (uVar6 < uVar3);
      }
      return uVar5;
    }
    if ((*(byte *)(param_4 + 0x18) & 1) != 0) {
      if ((param_5 != (undefined4 *)0x0) && (uVar6 = 0, uVar5 != uVar2)) {
        do {
          (*(uchar *)((char *)&param_1 + 0)) =
               (undefined1)(int)ROUND((float)*(ushort *)((int)puVar9 + uVar6 * 2) * _DAT_005e6cd0);
          *(undefined1 *)(uVar6 + (int)param_5) = (undefined1)param_1;
          uVar6 = uVar6 + 1;
        } while (uVar6 < uVar3);
      }
      return uVar5;
    }
    if ((param_5 != (undefined4 *)0x0) && (uVar6 = 0, uVar5 != uVar2)) {
      do {
        (*(uchar *)((char *)&param_1 + 0)) =
             (undefined1)
             (int)ROUND(((float)*(ushort *)((int)puVar9 + uVar6 * 2) - _DAT_005ce9fc) *
                        _DAT_005e6ccc);
        *(undefined1 *)(uVar6 + (int)param_5) = (undefined1)param_1;
        uVar6 = uVar6 + 1;
      } while (uVar6 < uVar3);
    }
    return uVar5;
  }
  bVar4 = *(byte *)(param_1 + 0x18) & 1;
  if ((bVar4 == 0) && ((*(byte *)(param_4 + 0x18) & 1) != 0)) {
    uVar3 = *(uint *)(param_1 + 8);
    if ((cVar1 == '\b') && (*(char *)(param_4 + 0xc) == '\b')) {
      iVar7 = uVar3 - uVar2;
      if ((param_5 != (undefined4 *)0x0) && (iVar7 != 0)) {
        iVar10 = (int)puVar9 - (int)param_5;
        do {
          (*(uchar *)((char *)&param_1 + 0)) =
               (undefined1)(int)ROUND((float)(int)*(char *)(iVar10 + (int)param_5) + _DAT_005cc9d0);
          *(undefined1 *)param_5 = (undefined1)param_1;
          param_5 = (undefined4 *)((int)param_5 + 1);
          iVar7 = iVar7 + -1;
        } while (iVar7 != 0);
      }
      return uVar3;
    }
    if ((cVar1 == '\x10') && (*(char *)(param_4 + 0xc) == '\x10')) {
      iVar7 = (uVar3 >> 1) - uVar2;
      if ((param_5 != (undefined4 *)0x0) && (iVar7 != 0)) {
        iVar10 = (int)puVar9 - (int)param_5;
        do {
          (*(ushort *)((char *)&param_1 + 0)) =
               (undefined2)(int)ROUND((float)(int)*(short *)((int)param_5 + iVar10) + _DAT_005ce9fc)
          ;
          *(undefined2 *)param_5 = (undefined2)param_1;
          param_5 = (undefined4 *)((int)param_5 + 2);
          iVar7 = iVar7 + -1;
        } while (iVar7 != 0);
      }
      return uVar3;
    }
  }
  else if ((bVar4 == 0) || ((*(byte *)(param_4 + 0x18) & 1) != 0)) {
    if (cVar1 == *(char *)(param_4 + 0xc)) {
      if (param_5 != (undefined4 *)0x0) {
        uVar2 = *(uint *)(param_1 + 8);
        for (uVar3 = uVar2 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
          *param_5 = *puVar9;
          puVar9 = puVar9 + 1;
          param_5 = param_5 + 1;
        }
        for (uVar2 = uVar2 & 3; uVar2 != 0; uVar2 = uVar2 - 1) {
          *(undefined1 *)param_5 = *(undefined1 *)puVar9;
          puVar9 = (undefined4 *)((int)puVar9 + 1);
          param_5 = (undefined4 *)((int)param_5 + 1);
        }
      }
      return *(uint *)(param_1 + 8);
    }
  }
  else {
    uVar3 = *(uint *)(param_1 + 8);
    if ((cVar1 == '\b') && (*(char *)(param_4 + 0xc) == '\b')) {
      iVar7 = uVar3 - uVar2;
      if ((param_5 != (undefined4 *)0x0) && (iVar7 != 0)) {
        iVar10 = (int)puVar9 - (int)param_5;
        do {
          (*(uchar *)((char *)&param_1 + 0)) =
               (undefined1)(int)ROUND((float)*(byte *)((int)param_5 + iVar10) - _DAT_005cc9d0);
          *(undefined1 *)param_5 = (undefined1)param_1;
          param_5 = (undefined4 *)((int)param_5 + 1);
          iVar7 = iVar7 + -1;
        } while (iVar7 != 0);
      }
      return uVar3;
    }
    if ((cVar1 == '\x10') && (*(char *)(param_4 + 0xc) == '\x10')) {
      iVar7 = (uVar3 >> 1) - uVar2;
      if ((param_5 != (undefined4 *)0x0) && (iVar7 != 0)) {
        iVar10 = (int)puVar9 - (int)param_5;
        do {
          (*(ushort *)((char *)&param_1 + 0)) =
               (undefined2)(int)ROUND((float)*(ushort *)((int)param_5 + iVar10) - _DAT_005ce9fc);
          *(undefined2 *)param_5 = (undefined2)param_1;
          param_5 = (undefined4 *)((int)param_5 + 2);
          iVar7 = iVar7 + -1;
        } while (iVar7 != 0);
      }
      return uVar3;
    }
  }
  return 0;
}
