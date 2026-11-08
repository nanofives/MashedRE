// FLAGS: /O2 /Ob1 /G5
// h2 hand-matched. 56.9% (regs 72.7%, ops 65.4%). Chunk stream writer. Fixed FUN_004cc580 to its real 5-arg form (stream,id,size,0x37002,10) and grouped the 12 stack dwords in one Frame struct so the slots do not overlap (frame 0x30 like the original). Residual: the original emits a separate epilogue for every failure return (xor eax,eax / pops / ret), ours cross-jumps them into one tail (240 vs 260 ins), same class as 0x004e5d30 and 0x004e7e50. Class (a) for the tails, (b) for the rest. permuter produced 0 variants.
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
#define CONCAT22(a,b) ((uint)(((uint)(ushort)(a)<<16)|(ushort)(b)))
MGLOBAL(undefined4, DAT_006186d0, 0x006186d0);
int FUN_004cbe80(uint p0, void *p1, uint p2);
int FUN_004cc580(int p0, int p1, int p2, int p3, int p4);
int FUN_004cc6e0(uint p0, int p1, uint p2);
int FUN_004e1ce0(void *p0, uint p1);
int FUN_004e1d20(void *p0, uint p1, uint p2);
int FUN_004e9010(uint p0);
int FUN_004f3ce0(uint p0);
int FUN_004f3d40(uint p0, uint p1);
// MATCH: 0x004e9080 FUN_004e9080
typedef struct { uint p[2]; uint h[4]; uint e[6]; } Frame;
uint FUN_004e9080(uint param_1,uint param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 *puVar5;
  undefined2 *puVar6;
  Frame L;
  
  iVar1 = FUN_004e9010(param_1);
  iVar2 = FUN_004f3ce0(param_1 + 0x20);
  iVar3 = FUN_004e1ce0(&DAT_006186d0,param_1);
  iVar1 = FUN_004cc580(param_2,0xf,iVar1 + iVar2 + 0x24 + iVar3,0x37002,10);
  if (iVar1 == 0) {
    return 0;
  }
  iVar1 = FUN_004cc580(param_2,1,FUN_004e9010(param_1),0x37002,10);
  if (iVar1 == 0) {
    return 0;
  }
  L.h[2] = *(undefined4 *)(param_1 + 0x14);
  L.h[0] = (*(uint *)(param_1 + 0x1c) & 0xff) << 0x10 | *(uint *)(param_1 + 8);
  L.h[1] = *(undefined4 *)(param_1 + 0x10);
  L.h[3] = *(undefined4 *)(param_1 + 0x18);
  iVar1 = FUN_004cbe80(param_2,&L.h[0],0x10);
  if (iVar1 == 0) {
    return 0;
  }
  if (((*(uint *)(param_1 + 8) & 0x1000000) == 0) && (*(int *)(param_1 + 0x14) != 0)) {
    if (((*(uint *)(param_1 + 8) & 8) != 0) &&
       (iVar1 = FUN_004cbe80(param_2,(void *)*(undefined4 *)(param_1 + 0x30),*(int *)(param_1 + 0x14) * 4),
       iVar1 == 0)) {
      return 0;
    }
    if (0 < *(int *)(param_1 + 0x1c)) {
      iVar1 = *(int *)(param_1 + 0x14);
      iVar2 = 0;
      if (0 < *(int *)(param_1 + 0x1c)) {
        puVar5 = (undefined4 *)(param_1 + 0x34);
        do {
          iVar3 = FUN_004cc6e0(param_2,*puVar5,iVar1 << 3);
          if (iVar3 == 0) {
            return 0;
          }
          iVar2 = iVar2 + 1;
          puVar5 = puVar5 + 1;
        } while (iVar2 < *(int *)(param_1 + 0x1c));
      }
    }
    iVar1 = *(int *)(param_1 + 0x10);
    if (iVar1 != 0) {
      puVar6 = *(undefined2 **)(param_1 + 0x2c);
      do {
        L.p[0] = CONCAT22(*puVar6,puVar6[1]);
        L.p[1] = CONCAT22(puVar6[2],puVar6[3]);
        iVar1 = iVar1 + -1;
        puVar6 = puVar6 + 4;
        iVar2 = FUN_004cbe80(param_2,&L.p[0],8);
        if (iVar2 == 0) {
          return 0;
        }
      } while (iVar1 != 0);
    }
  }
  iVar1 = 0;
  if (0 < *(int *)(param_1 + 0x18)) {
    iVar2 = 0;
    do {
      iVar3 = iVar2 + *(int *)(param_1 + 0x5c);
      L.e[0] = *(undefined4 *)(iVar3 + 4);
      L.e[1] = *(undefined4 *)(iVar3 + 8);
      L.e[2] = *(undefined4 *)(iVar3 + 0xc);
      L.e[3] = *(undefined4 *)(iVar3 + 0x10);
      if ((*(uint *)(param_1 + 8) & 0x1000000) == 0) {
        L.e[4] = (uint)(*(int *)(iVar3 + 0x14) != 0);
        L.e[5] = (uint)(*(int *)(iVar3 + 0x18) != 0);
      }
      else {
        L.e[4] = 0;
        L.e[5] = 0;
      }
      iVar3 = FUN_004cbe80(param_2,&L.e[0],0x18);
      if (iVar3 == 0) {
        return 0;
      }
      if ((L.e[4] != 0) &&
         (iVar3 = FUN_004cc6e0(param_2,*(undefined4 *)(iVar2 + 0x14 + *(int *)(param_1 + 0x5c)),
                               *(int *)(param_1 + 0x14) * 0xc), iVar3 == 0)) {
        return 0;
      }
      if ((L.e[5] != 0) &&
         (iVar3 = FUN_004cc6e0(param_2,*(undefined4 *)(iVar2 + 0x18 + *(int *)(param_1 + 0x5c)),
                               *(int *)(param_1 + 0x14) * 0xc), iVar3 == 0)) {
        return 0;
      }
      iVar1 = iVar1 + 1;
      iVar2 = iVar2 + 0x1c;
    } while (iVar1 < *(int *)(param_1 + 0x18));
  }
  iVar1 = FUN_004f3d40(param_1 + 0x20,param_2);
  if (iVar1 == 0) {
    return 0;
  }
  iVar1 = FUN_004e1d20(&DAT_006186d0,param_2,param_1);
  return -(uint)(iVar1 != 0) & param_1;
}
