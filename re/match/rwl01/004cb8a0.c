// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004cb8a0: best 43.3% (regs 50%), class (b). Vertex-declaration cache lookup: typed cdecl/stdcall COM-slot prototypes (the original mixes both: found-path calls pop their args, the CreateDecl call does not), memcpy intrinsic for the copy. The original keeps param_1 in ebp and re-reads the cache count into ebx after the compare loop; ours keeps param_1 in esi. A CE-struct rewrite scored lower (25%); permuter variants moved the count increment before the memcpy (wrong), discarded. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef int code();
MGLOBAL(uint, DAT_007d4144, 0x007d4144);
MGLOBAL(int, DAT_007d414c, 0x007d414c);
MGLOBAL(uint, DAT_007d4148, 0x007d4148);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)
typedef int (__cdecl *PFN_CDECL3)(int *, int, int);
typedef int (__cdecl *PFN_CDECL1)(int *);
typedef int (__stdcall *PFN_CREATEDECL)(int *, int *, int *);
typedef int (__cdecl *PFN_ALLOC1)(uint);
typedef int (__cdecl *PFN_ALLOC2)(uint, uint);
typedef int (__cdecl *PFN_REALLOC)(int, uint, uint);
// MATCH: 0x004cb8a0 FUN_004cb8a0
bool FUN_004cb8a0(int *param_1,int *param_2)
{
  char cVar1;
  int *piVar2;
  int iVar3;
  int uVar4;
  uint uVar5;
  uint uVar6;
  int *piVar7;
  int iVar8;
  iVar8 = 0;
  piVar7 = param_1 + 1;
  cVar1 = (char)param_1[1];
  while (cVar1 != '') {
    piVar2 = piVar7 + 2;
    piVar7 = piVar7 + 2;
    iVar8 = iVar8 + 1;
    cVar1 = (char)*piVar2;
  }
  iVar8 = iVar8 + 1;
  uVar6 = 0;
  if (0 < DAT_007d4144) {
    piVar7 = (int *)(DAT_007d414c + 8);
    do {
      if (piVar7[-1] == iVar8) {
        uVar5 = (uint)(iVar8 * 8) >> 2;
        if (uVar5 == 0) {
LAB_004cb904:
          if (uVar6 < DAT_007d4144) {
            iVar8 = uVar6 * 0xc;
            piVar7 = *(int **)(iVar8 + DAT_007d414c);
            if (piVar7 == (int *)0x0) {
              (*(PFN_CDECL3 *)(*DAT_007d4110 + 0x158))
                        (DAT_007d4110,*(int *)(iVar8 + DAT_007d414c + 8),iVar8 + DAT_007d414c
                        );
            }
            else {
              (*(PFN_CDECL1 *)(*piVar7 + 4))(piVar7);
            }
            iVar8 = *(int *)(iVar8 + DAT_007d414c);
            *param_1 = iVar8;
            return iVar8 != 0;
          }
          break;
        }
        piVar2 = param_1;
        while (*(int *)((*piVar7 - (int)param_1) + (int)piVar2) == *piVar2) {
          piVar2 = piVar2 + 1;
          uVar5 = uVar5 - 1;
          if (uVar5 == 0) goto LAB_004cb904;
        }
      }
      uVar6 = uVar6 + 1;
      piVar7 = piVar7 + 3;
    } while (uVar6 < DAT_007d4144);
  }
  if (DAT_007d4144 >= DAT_007d4148) {
    DAT_007d4148 = DAT_007d4148 + 0x10;
    if (DAT_007d414c == 0) {
      DAT_007d414c = (*(PFN_ALLOC1)*(int *)(DAT_007d3ff8 + 0x108))(DAT_007d4148 * 0xc);
    }
    else {
      DAT_007d414c = (*(PFN_REALLOC)*(int *)(DAT_007d3ff8 + 0x110))(DAT_007d414c,DAT_007d4148 * 0xc,0x1040411);
    }
  }
  iVar3 = (*(PFN_CREATEDECL *)(*DAT_007d4110 + 0x158))(DAT_007d4110,param_1,param_2);
  if (-1 < iVar3) {
    *(int *)(DAT_007d414c + DAT_007d4144 * 0xc) = *param_2;
    *(int *)(DAT_007d414c + 4 + DAT_007d4144 * 0xc) = iVar8;
    uVar4 = (*(PFN_ALLOC2)*(int *)(DAT_007d3ff8 + 0x108))(iVar8 * 8,0x40411);
    *(int *)(DAT_007d414c + 8 + DAT_007d4144 * 0xc) = uVar4;
    memcpy(*(void **)(DAT_007d414c + 8 + DAT_007d4144 * 0xc),param_1,iVar8 * 8);
    DAT_007d4144 = DAT_007d4144 + 1;
    return *param_2 != 0;
  }
  *param_2 = 0;
  return *param_2 != 0;
}
