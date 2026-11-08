// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x0054fd60 h1: 36.5% (regs 68.3%, ops 89.2%); structure only (967 B). Raw converter body kept, C++ fixed with typed callee prototypes and a RwGlob vtable struct. Frame 0x60 vs orig 0x68 (orig pushes ebx). Class (b). */
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
typedef int (*cbv)(int, int);
typedef int (*cbv1)(int);
typedef struct { char pad[0x108]; cbv fn108; cbv1 fn10c; } RwGlob;
MGLOBAL(RwGlob *, DAT_007d3ff8, 0x007d3ff8);
int FUN_004c5a00(int p0);
int FUN_004c5ae0(int p0, void *p1);
int FUN_004c5b50(int p0, void *p1);
void FUN_004c5bc0(int p0, int p1);
int FUN_004c7600(int p0);
int FUN_004c7650(int p0);
int FUN_004c76f0(int p0);
int FUN_004c77c0(int p0, int p1, int p2, int p3);
unsigned int FUN_004c7860(int p0, unsigned int p1, int p2);
int FUN_004cbd30(int p0, void *p1, unsigned int p2);
int FUN_004cc5e0(int p0, int p1, int p2, int p3);
int FUN_004cdd00(int p0);
int FUN_004cee90(int p0);
int FUN_004cefd0(int p0);
int FUN_004d5310(int p0, int p1);
int FUN_004d5340(int p0, int p1, int *p2, int *p3, int *p4, unsigned int *p5);
int FUN_004d7ff0(int p0, int p1);
int FUN_004d8480(int *p0);
int FUN_004db2e0(int p0, int p1, int p2);
// MATCH: 0x0054fd60 FUN_0054fd60
int FUN_0054fd60(int param_1,int *param_2)

{
  int iVar1;
  int *piVar2;
  int *pi;
  int iVar3;
  unsigned int *puVar4;
  int iVar5;
  int iVar6;
  unsigned int uVar7;
  unsigned int uVar8;
  int iStack_68;
  int iStack_64;
  int uStack_60;
  int uStack_5c;
  unsigned int local_58;
  unsigned int uStack_54;
  int iStack_50;
  int uStack_4c;
  unsigned char local_48 [32];
  unsigned char auStack_28 [32];
  int local_8;
  unsigned int uStack_4;
  
  local_58 = 0;
  iVar1 = FUN_004cbd30(param_1,local_48,0x48);
  if (iVar1 != 0x48) {
    return 0;
  }
  piVar2 = (int *)DAT_007d3ff8->fn108(local_8 * 4,0x101b3);
  if (piVar2 == (int *)0x0) {
    iStack_50 = 0x1b3;
    uStack_4c = FUN_004d7ff0(0x80000013,local_8 * 4);
    FUN_004d8480(&iStack_50);
    return 0;
  }
  iVar1 = 0;
  if (0 < local_8) {
    do {
      iVar3 = FUN_004cc5e0(param_1,0x18,0,0);
      if (iVar3 == 0) {
        return 0;
      }
      puVar4 = (unsigned int *)FUN_004cee90(param_1);
      piVar2[iVar1] = (int)puVar4;
      if (puVar4 == (unsigned int *)0x0) {
        return 0;
      }
      if ((*puVar4 & 2) == 0) {
        FUN_004cefd0((int)puVar4);
      }
      iVar1 = iVar1 + 1;
    } while (iVar1 < local_8);
  }
  uVar8 = 4;
  uStack_5c = *(int *)(*piVar2 + 4);
  uStack_60 = *(int *)(*piVar2 + 8);
  iStack_64 = *(int *)(*piVar2 + 0xc);
  if (local_8 < 2) {
    if (((uStack_4 & 0xff) != 1) && ((uStack_4 & 0xff) != 2)) {
      uVar8 = 0x9004;
    }
  }
  else {
    uVar8 = 0x8004;
  }
  FUN_004d5340(*piVar2,uVar8,&uStack_5c,&uStack_60,&iStack_64,&uStack_54);
  iVar1 = FUN_004c77c0(uStack_5c,uStack_60,iStack_64,uStack_54 | uVar8);
  if (iVar1 == 0) {
    iVar1 = 0;
    if (0 < local_8) {
      do {
        FUN_004cdd00(piVar2[iVar1]);
        iVar1 = iVar1 + 1;
      } while (iVar1 < local_8);
    }
    DAT_007d3ff8->fn10c((int)piVar2);
    return 0;
  }
  param_1 = *(int *)(iVar1 + 0xc);
  if ((*(int *)(*piVar2 + 4) != param_1) || (*(int *)(*piVar2 + 8) != *(int *)(iVar1 + 0x10))) {
    iStack_68 = *(int *)(iVar1 + 0x10);
    if (iStack_64 == 8) {
      FUN_004c7650(iVar1);
      iVar1 = FUN_004c77c0(param_1,iStack_68,0x20,uVar8 | 0x500);
      if (iVar1 == 0) {
        iVar1 = FUN_004c77c0(param_1,iStack_68,0x10,uVar8 | 0x100);
      }
    }
    iStack_50 = FUN_004c76f0(iVar1);
    iVar3 = 0;
    if (0 < iStack_50) {
      do {
        iVar5 = FUN_004db2e0(piVar2[iVar3],param_1,iStack_68);
        if (iVar5 == 0) {
          return 0;
        }
        FUN_004cdd00(piVar2[iVar3]);
        piVar2[iVar3] = iVar5;
        if (1 < param_1) {
          param_1 = param_1 >> 1;
        }
        if (1 < iStack_68) {
          iStack_68 = iStack_68 >> 1;
        }
        iVar3 = iVar3 + 1;
      } while (iVar3 < iStack_50);
    }
  }
  if (1 < local_8) {
    local_58 = *(unsigned char *)(iVar1 + 0x23) & 0x10;
    *(unsigned char *)(iVar1 + 0x23) = ~(unsigned char)local_58 & *(unsigned char *)(iVar1 + 0x23);
  }
  iVar3 = FUN_004c76f0(iVar1);
  iVar5 = 0;
  if (0 < iVar3) {
    do {
      if (iVar5 == local_8) break;
      iVar6 = FUN_004c7860(iVar1,iVar5,1);
      if (iVar6 == 0) {
        return 0;
      }
      iVar6 = FUN_004d5310(iVar1,piVar2[iVar5]);
      if (iVar6 == 0) {
        return 0;
      }
      iVar6 = FUN_004c7600(iVar1);
      if (iVar6 == 0) {
        return 0;
      }
      FUN_004cdd00(piVar2[iVar5]);
      iVar5 = iVar5 + 1;
    } while (iVar5 < iVar3);
  }
  if (1 < local_8) {
    *(unsigned char *)(iVar1 + 0x23) = *(unsigned char *)(iVar1 + 0x23) | (unsigned char)local_58;
  }
  if (iVar5 < local_8) {
    do {
      FUN_004cdd00(piVar2[iVar5]);
      iVar5 = iVar5 + 1;
    } while (iVar5 < local_8);
  }
  DAT_007d3ff8->fn10c((int)piVar2);
  iVar1 = FUN_004c5a00(iVar1);
  if (iVar1 == 0) {
    return 0;
  }
  FUN_004c5ae0(iVar1,local_48);
  FUN_004c5b50(iVar1,auStack_28);
  *(unsigned char *)(iVar1 + 0x50) = (unsigned char)uStack_4;
  uVar8 = uStack_4 >> 8 & 0xf;
  uVar7 = uStack_4 >> 0xc & 0xf;
  if (uVar8 == 0) {
    uVar8 = uVar7;
    if (uVar7 == 0) {
      uVar8 = 1;
      uVar7 = 1;
    }
  }
  else if (uVar7 == 0) {
    uVar7 = uVar8;
  }
  uVar8 = (uVar8 << 8 ^ *(unsigned int *)(iVar1 + 0x50)) & 0xf00 ^ *(unsigned int *)(iVar1 + 0x50);
  *(unsigned int *)(iVar1 + 0x50) = (uVar7 << 0xc ^ uVar8) & 0xf000 ^ uVar8;
  FUN_004c5bc0(*param_2,iVar1);
  return *param_2;
}
