// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0051c220: libpng 1.0.12 pngrtran.c png_build_gamma_table (gamma / screen_gamma are float fields at
   png_struct+0x15c / +0x160). Raw g2s.py output (body unchanged; globals moved to MGLOBAL), not hand-matched.
   25.1% (regs 35.0, ops 31.5). 1990 B, above the 1000 B budget line, and the original's 7 float-to-int
   conversions are the inline fnstcw / fldcw / fistp sequence that 9176 never emits (class (a), as 0x0051d240), so
   it was left at the raw converter's form. Class (a) + (b) out of budget. C++ check FAILS (raw Ghidra types: void* to
   undefined4, ROUND), not fixed since the file is not a match. */
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
typedef unsigned char byte;
typedef long double float10;
typedef unsigned int size_t;
#define CONCAT22(a,b) ((uint)(((uint)(ushort)(a)<<16)|(ushort)(b)))
MGLOBAL(undefined1, DAT_005d1008, 0x005d1008);
MGLOBAL(undefined1, DAT_005de6a0, 0x005de6a0);
MGLOBAL(undefined1, DAT_005cd0c8, 0x005cd0c8);
MGLOBAL(undefined1, DAT_005e3390, 0x005e3390);
MGLOBAL(undefined1, DAT_005e3388, 0x005e3388);
MGLOBAL(undefined1, DAT_005ce1d8, 0x005ce1d8);
MGLOBAL(undefined1, DAT_005e33a8, 0x005e33a8);
MGLOBAL(undefined1, DAT_005e33a0, 0x005e33a0);
MGLOBAL(undefined1, DAT_005e3398, 0x005e3398);
void * FUN_005208c0(int p0, size_t p1);
double _pow(double p0, double p1);
MGLOBAL(int, _DAT_005d1008, 0x005d1008);
MGLOBAL(int, _DAT_005de6a0, 0x005de6a0);
MGLOBAL(int, _DAT_005cd0c8, 0x005cd0c8);
MGLOBAL(int, _DAT_005e3390, 0x005e3390);
MGLOBAL(int, _DAT_005e3388, 0x005e3388);
MGLOBAL(int, _DAT_005ce1d8, 0x005ce1d8);
MGLOBAL(int, _DAT_005e33a0, 0x005e33a0);
MGLOBAL(int, _DAT_005e3398, 0x005e3398);
MGLOBAL(int, _DAT_005e33a8, 0x005e33a8);
// MATCH: 0x0051c220 FUN_0051c220
void FUN_0051c220(int param_1)

{
  float fVar1;
  float fVar2;
  byte bVar3;
  undefined4 uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  byte bVar8;
  uint uVar9;
  uint uVar10;
  int iVar11;
  uint uVar12;
  int iVar13;
  int iVar14;
  double dVar15;
  uint local_44;
  uint local_40;
  undefined1 local_30;
  undefined2 local_24;
  ushort local_20;
  undefined2 uStack_1e;
  double local_18;
  
  if (*(float *)(param_1 + 0x15c) != (float)_DAT_005d1008) {
    if (*(byte *)(param_1 + 0x127) < 9) {
      if (*(float *)(param_1 + 0x160) <= (float)_DAT_005de6a0) {
        local_18 = 1.0;
      }
      else {
        local_18 = (double)((float)_DAT_005cd0c8 /
                           (*(float *)(param_1 + 0x160) * *(float *)(param_1 + 0x15c)));
      }
      uVar4 = FUN_005208c0(param_1,0x100);
      *(undefined4 *)(param_1 + 0x164) = uVar4;
      local_40 = 0;
      do {
        dVar15 = _pow((double)(int)local_40 * _DAT_005e3390,local_18);
        local_30 = (undefined1)
                   (int)ROUND((float)((float10)dVar15 * (float10)_DAT_005e3388 +
                                     (float10)_DAT_005ce1d8));
        *(undefined1 *)(local_40 + *(int *)(param_1 + 0x164)) = local_30;
        local_40 = local_40 + 1;
      } while ((int)local_40 < 0x100);
      if ((*(uint *)(param_1 + 0x70) & 0x600080) != 0) {
        fVar1 = *(float *)(param_1 + 0x15c);
        fVar2 = (float)_DAT_005cd0c8;
        uVar4 = FUN_005208c0(param_1,0x100);
        *(undefined4 *)(param_1 + 0x16c) = uVar4;
        local_40 = 0;
        do {
          dVar15 = _pow((double)(int)local_40 * _DAT_005e3390,(double)(fVar2 / fVar1));
          local_30 = (undefined1)
                     (int)ROUND((float)((float10)dVar15 * (float10)_DAT_005e3388 +
                                       (float10)_DAT_005ce1d8));
          *(undefined1 *)(local_40 + *(int *)(param_1 + 0x16c)) = local_30;
          local_40 = local_40 + 1;
        } while ((int)local_40 < 0x100);
        uVar4 = FUN_005208c0(param_1,0x100);
        fVar1 = (float)_DAT_005de6a0;
        *(undefined4 *)(param_1 + 0x168) = uVar4;
        if (*(float *)(param_1 + 0x160) <= fVar1) {
          fVar1 = *(float *)(param_1 + 0x15c);
        }
        else {
          fVar1 = (float)_DAT_005cd0c8 / *(float *)(param_1 + 0x160);
        }
        local_40 = 0;
        do {
          dVar15 = _pow((double)(int)local_40 * _DAT_005e3390,(double)fVar1);
          local_30 = (undefined1)
                     (int)ROUND((float)((float10)dVar15 * (float10)_DAT_005e3388 +
                                       (float10)_DAT_005ce1d8));
          *(undefined1 *)(local_40 + *(int *)(param_1 + 0x168)) = local_30;
          local_40 = local_40 + 1;
        } while ((int)local_40 < 0x100);
        return;
      }
    }
    else {
      if ((*(byte *)(param_1 + 0x126) & 2) == 0) {
        local_40 = (uint)*(byte *)(param_1 + 0x17f);
      }
      else {
        local_40 = (uint)*(byte *)(param_1 + 0x17c);
        if ((uint)*(byte *)(param_1 + 0x17c) < (uint)*(byte *)(param_1 + 0x17d)) {
          local_40 = (uint)*(byte *)(param_1 + 0x17d);
        }
        if (local_40 < *(byte *)(param_1 + 0x17e)) {
          local_40 = (uint)*(byte *)(param_1 + 0x17e);
        }
      }
      if (local_40 == 0) {
        local_40 = 0;
      }
      else {
        local_40 = 0x10 - local_40;
      }
      if (((*(uint *)(param_1 + 0x70) & 0x400) == 0) || (4 < (int)local_40)) {
        if ((int)local_40 < 9) {
          if ((int)local_40 < 0) {
            local_40 = 0;
          }
        }
        else {
          local_40 = 8;
        }
      }
      else {
        local_40 = 5;
      }
      fVar1 = (float)_DAT_005de6a0;
      bVar3 = (byte)local_40;
      bVar8 = 8 - bVar3;
      *(uint *)(param_1 + 0x158) = local_40 & 0xff;
      iVar11 = 1 << (bVar8 & 0x1f);
      if (*(float *)(param_1 + 0x160) <= fVar1) {
        local_18 = 1.0;
      }
      else {
        local_18 = (double)((float)_DAT_005cd0c8 /
                           (*(float *)(param_1 + 0x160) * *(float *)(param_1 + 0x15c)));
      }
      uVar4 = FUN_005208c0(param_1,iVar11 * 4);
      *(undefined4 *)(param_1 + 0x170) = uVar4;
      local_44 = 0;
      if ((*(uint *)(param_1 + 0x70) & 0x480) == 0) {
        if (0 < iVar11) {
          do {
            uVar4 = FUN_005208c0(param_1,0x200);
            *(undefined4 *)(*(int *)(param_1 + 0x170) + local_44 * 4) = uVar4;
            uVar7 = *(int *)(local_40 * 4 + 0x6215dc) * local_44 >> 4;
            iVar14 = 0;
            do {
              dVar15 = _pow((double)uVar7 * _DAT_005e33a0,local_18);
              iVar13 = iVar14 + 2;
              uVar7 = uVar7 + 0x100;
              local_24 = (undefined2)
                         (int)ROUND((float)((float10)dVar15 * (float10)_DAT_005e3398 +
                                           (float10)_DAT_005ce1d8));
              *(undefined2 *)(iVar14 + *(int *)(*(int *)(param_1 + 0x170) + local_44 * 4)) =
                   local_24;
              iVar14 = iVar13;
            } while (iVar13 < 0x200);
            local_44 = local_44 + 1;
          } while ((int)local_44 < iVar11);
        }
      }
      else {
        if (0 < iVar11) {
          do {
            uVar4 = FUN_005208c0(param_1,0x200);
            *(undefined4 *)(*(int *)(param_1 + 0x170) + local_44 * 4) = uVar4;
            local_44 = local_44 + 1;
          } while ((int)local_44 < iVar11);
        }
        local_18 = _DAT_005cd0c8 / local_18;
        uVar12 = 0;
        uVar5 = iVar11 << 8;
        local_20 = (ushort)uVar5;
        uStack_1e = (undefined2)((uint)iVar11 >> 8);
        local_44 = 0;
        uVar7 = CONCAT22(uStack_1e,local_20);
        do {
          dVar15 = _pow(((double)(int)local_44 + _DAT_005ce1d8) * _DAT_005e33a8,local_18);
          uVar10 = (uint)ROUND((float)((float10)dVar15 * (float10)uVar7));
          if (uVar12 <= uVar10) {
            local_20 = (ushort)((local_44 & 0xff) << 8) | (ushort)local_44;
            do {
              uVar6 = 0xff >> (bVar3 & 0x1f) & uVar12;
              uVar9 = uVar12 >> (bVar8 & 0x1f);
              uVar12 = uVar12 + 1;
              *(ushort *)(*(int *)(*(int *)(param_1 + 0x170) + uVar6 * 4) + uVar9 * 2) = local_20;
            } while (uVar12 <= uVar10);
          }
          local_44 = local_44 + 1;
        } while ((int)local_44 < 0x100);
        if (uVar12 < uVar5) {
          do {
            uVar7 = 0xff >> (bVar3 & 0x1f) & uVar12;
            uVar10 = uVar12 >> (bVar8 & 0x1f);
            uVar12 = uVar12 + 1;
            *(undefined2 *)(*(int *)(*(int *)(param_1 + 0x170) + uVar7 * 4) + uVar10 * 2) = 0xffff;
          } while (uVar12 < uVar5);
        }
      }
      if ((*(uint *)(param_1 + 0x70) & 0x600080) != 0) {
        fVar1 = *(float *)(param_1 + 0x15c);
        fVar2 = (float)_DAT_005cd0c8;
        uVar4 = FUN_005208c0(param_1,iVar11 * 4);
        *(undefined4 *)(param_1 + 0x178) = uVar4;
        local_44 = 0;
        if (0 < iVar11) {
          do {
            uVar4 = FUN_005208c0(param_1,0x200);
            *(undefined4 *)(*(int *)(param_1 + 0x178) + local_44 * 4) = uVar4;
            uVar7 = *(int *)(local_40 * 4 + 0x6215dc) * local_44 >> 4;
            iVar14 = 0;
            do {
              dVar15 = _pow((double)uVar7 * _DAT_005e33a0,(double)(fVar2 / fVar1));
              iVar13 = iVar14 + 2;
              uVar7 = uVar7 + 0x100;
              local_24 = (undefined2)
                         (int)ROUND((float)((float10)dVar15 * (float10)_DAT_005e3398 +
                                           (float10)_DAT_005ce1d8));
              *(undefined2 *)(iVar14 + *(int *)(*(int *)(param_1 + 0x178) + local_44 * 4)) =
                   local_24;
              iVar14 = iVar13;
            } while (iVar13 < 0x200);
            local_44 = local_44 + 1;
          } while ((int)local_44 < iVar11);
        }
        if (*(float *)(param_1 + 0x160) <= (float)_DAT_005de6a0) {
          fVar1 = *(float *)(param_1 + 0x15c);
        }
        else {
          fVar1 = (float)_DAT_005cd0c8 / *(float *)(param_1 + 0x160);
        }
        uVar4 = FUN_005208c0(param_1,iVar11 * 4);
        *(undefined4 *)(param_1 + 0x174) = uVar4;
        local_44 = 0;
        if (0 < iVar11) {
          do {
            uVar4 = FUN_005208c0(param_1,0x200);
            *(undefined4 *)(*(int *)(param_1 + 0x174) + local_44 * 4) = uVar4;
            uVar7 = *(int *)(local_40 * 4 + 0x6215dc) * local_44 >> 4;
            iVar14 = 0;
            do {
              dVar15 = _pow((double)uVar7 * _DAT_005e33a0,(double)fVar1);
              iVar13 = iVar14 + 2;
              uVar7 = uVar7 + 0x100;
              local_24 = (undefined2)
                         (int)ROUND((float)((float10)dVar15 * (float10)_DAT_005e3398 +
                                           (float10)_DAT_005ce1d8));
              *(undefined2 *)(iVar14 + *(int *)(*(int *)(param_1 + 0x174) + local_44 * 4)) =
                   local_24;
              iVar14 = iVar13;
            } while (iVar13 < 0x200);
            local_44 = local_44 + 1;
          } while ((int)local_44 < iVar11);
        }
      }
    }
  }
  return;
}
