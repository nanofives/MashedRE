// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004de1b0: 56.8% regs 58.6%.  class (b) Raster-to-8bit conversion. Fixed callee arity (FUN_004d7ff0 takes 2 args) and replaced the dword/byte copy loops by memcpy intrinsic (rep movsd/movsb) 11.5 -> 56.8. Strings via MGLOBALA. Remaining: original frame has two zeroed int locals plus the 8-byte error struct laid out differently, and keeps ebp/esi/edi/ebx assigned differently.. */
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
#ifndef __cplusplus
#define true 1
#endif
#ifndef __cplusplus
#define false 0
#endif
MGLOBAL(int, DAT_00911ae4, 0x00911ae4);
MGLOBALA(char, s_Conversion_from_rasters_to_4bit_i_00618458, 0x20, 0x00618458);
MGLOBALA(char, s_Conversion_from_non_palettised_r_00618490, 0x48, 0x00618490);
MGLOBALA(char, s_Conversion_from_compressed_raste_006184d8, 0x28, 0x006184d8);
undefined4 FUN_004c7600(undefined4 p0);
int FUN_004c7620(int p0);
uint FUN_004c76c0(undefined4 p0, uint p1);
uint FUN_004c7860(undefined4 p0, uint p1, int p2);
undefined4 FUN_004d7ff0(undefined4 p0, char * p1);
undefined4 * FUN_004d8480(undefined4 * p0);
void FUN_004de390(int p0, int * p1);
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)
// MATCH: 0x004de1b0 FUN_004de1b0
undefined4 FUN_004de1b0(int param_1,int param_2)
{
  int bVar1;
  int bVar8;
  int iVar5;
  int local_10;
  undefined4 local_c;
  undefined4 local_8;
  undefined4 local_4;

  local_10 = 0;
  bVar1 = 0;
  if ((*(byte *)(DAT_00911ae4 + 10 + param_2) & 0xf0) != 0) {
    local_10 = 1;
    local_c = FUN_004d7ff0(2,(char *)s_Conversion_from_compressed_raste_006184d8);
    FUN_004d8480((undefined4 *)&local_10);
    return 0;
  }
  bVar8 = (*(byte *)(param_2 + 0x22) & 2) == 0;
  if (bVar8) {
    FUN_004c7860(param_2,0,2);
  }
  local_10 = bVar8;
  if (((*(byte *)(param_2 + 0x23) & 0x60) != 0) && ((*(byte *)(param_2 + 0x22) & 8) == 0)) {
    FUN_004c76c0(param_2,2);
    bVar1 = 1;
  }
  if (*(int *)(param_1 + 0xc) == 0x20) {
    FUN_004de390(param_1,(int *)param_2);
  }
  else {
    if (*(int *)(param_1 + 0xc) == 8) {
      if ((*(int *)(param_2 + 0x14) == 8) && ((*(byte *)(param_2 + 0x23) & 0xf) != 0)) {
        if ((*(int *)(param_1 + 0x10) == *(int *)(param_2 + 0x18)) &&
           (*(int *)(param_2 + 0xc) == *(int *)(param_2 + 0x18))) {
          memcpy(*(void **)(param_1 + 0x14),*(void **)(param_2 + 4),*(int *)(param_2 + 0x10) * *(int *)(param_2 + 0xc));
        }
        else {
          iVar5 = 0;
          if (0 < *(int *)(param_2 + 0x10)) {
            do {
              memcpy((void *)(*(int *)(param_1 + 0x10) * iVar5 + *(int *)(param_1 + 0x14)),
                     (void *)(*(int *)(param_2 + 0x18) * iVar5 + *(int *)(param_2 + 4)),
                     *(unsigned int *)(param_2 + 0xc));
              iVar5 = iVar5 + 1;
            } while (iVar5 < *(int *)(param_2 + 0x10));
          }
        }
        if (*(undefined4 **)(param_2 + 8) == (undefined4 *)0) {
          iVar5 = 0;
          do {
            *(undefined1 *)(*(int *)(param_1 + 0x18) + iVar5 * 4) = (undefined1)iVar5;
            *(undefined1 *)(*(int *)(param_1 + 0x18) + 1 + iVar5 * 4) = (undefined1)iVar5;
            *(undefined1 *)(*(int *)(param_1 + 0x18) + 2 + iVar5 * 4) = (undefined1)iVar5;
            *(undefined1 *)(*(int *)(param_1 + 0x18) + 3 + iVar5 * 4) = 0xff;
            iVar5 = iVar5 + 1;
          } while (iVar5 < 0x100);
        }
        else {
          memcpy(*(void **)(param_1 + 0x18),*(void **)(param_2 + 8),0x400);
        }
        goto LAB_004de35a;
      }
      local_8 = 1;
      local_4 = FUN_004d7ff0(2,(char *)s_Conversion_from_non_palettised_r_00618490);
    }
    else {
      local_8 = 1;
      local_4 = FUN_004d7ff0(2,(char *)s_Conversion_from_rasters_to_4bit_i_00618458);
    }
    FUN_004d8480(&local_8);
  }
LAB_004de35a:
  if (bVar1) {
    FUN_004c7620(param_2);
  }
  if (local_10 == 1) {
    FUN_004c7600(param_2);
  }
  return 1;
}
