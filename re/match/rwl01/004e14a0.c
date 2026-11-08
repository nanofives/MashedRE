// FLAGS: /O2 /Ob1 /G5
/* h3 hand-match: best 38.9% (regs 59.3%), class b. Heap-grow (size round 0x1f&~0x1f, scan 8-byte entries backwards, alloc 0x8b extra, link block, sprintf of resize, FUN_004e1670). String global as MGLOBALA, 2-dword stack record as an array so both stores survive, tail 'esi=*p+0x20; if(esi)' instead of Ghidra's 0xffffffe0 test. Residual: block layout (original puts the post-link tail after the fail epilogues via jmp, loads param_2 into eax before the pushes, and-with-edx not byte and). Permuter variants swap stores past calls: rejected. */
/* MATCH-RWL01 0x004e14a0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl01_20261107/results.tsv. */
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
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
undefined4 FUN_004d7ff0(undefined4 p0, undefined4 p1);
undefined4 * FUN_004d8480(undefined4 * p0);
undefined4 FUN_004e15f0(uint * p0, uint * p1, uint * p2);
int FUN_004e1670(uint * p0, uint p1, uint p2);
MGLOBALA(char, s_Heap_resized_006185f8, 40, 0x006185f8);
// MATCH: 0x004e14a0 FUN_004e14a0
undefined4 FUN_004e14a0(uint *param_1,int param_2)

{
  uint *puVar1;
  uint *puVar2;
  int iVar3;
  undefined4 uVar4;
  uint uVar5;
  uint *puVar6;
  uint uVar7;
  undefined4 uStack_108[2];
  undefined1 auStack_100 [256];
  
  uVar7 = param_2 + 0x1fU & 0xffffffe0;
  uVar5 = param_1[5];
  puVar2 = (uint *)((param_1[3] - 8) + uVar5 * 8);
  do {
    if (*puVar2 >= uVar7) {
      param_1[6] = 1;
      uVar4 = FUN_004e1670(param_1,(uint)puVar2,uVar7);
      return uVar4;
    }
    puVar2 = puVar2 + -2;
    uVar5 = uVar5 - 1;
  } while (uVar5 != 0);
  uVar5 = uVar7 + 0x60;
  if (uVar7 + 0x60 < *param_1) {
    uVar5 = *param_1;
  }
  puVar2 = (uint *)(**(code **)(DAT_007d3ff8 + 0x108))(uVar5 + 0x8b,0x1040409);
  if (puVar2 != (uint *)0x0) {
    puVar2[1] = uVar5;
    puVar2[2] = 0;
    *puVar2 = (int)puVar2 + 0x8bU & 0xffffff80;
    iVar3 = FUN_004e15f0(puVar2,(uint *)param_1[1],param_1);
    if (iVar3 == 0) {
      (**(code **)(DAT_007d3ff8 + 0x10c))(puVar2);
    }
    else {
      iVar3 = 0;
      puVar2[2] = param_1[1];
      param_1[1] = (uint)puVar2;
      puVar6 = puVar2;
      do {
        puVar1 = puVar6 + 1;
        puVar6 = (uint *)puVar6[2];
        iVar3 = iVar3 + *puVar1;
      } while (puVar6 != (uint *)0x0);
      (**(code **)(DAT_007d3ff8 + 0xc4))
                (auStack_100,s_Heap_resized_006185f8,iVar3 - uVar5,iVar3);
      puVar2 = (uint *)(*puVar2 + 0x20);
      if (puVar2 != (uint *)0x0) {
        param_1[6] = 1;
        uVar4 = FUN_004e1670(param_1,puVar2[3],uVar7);
        return uVar4;
      }
    }
  }
  uStack_108[0] = 1;
  uStack_108[1] = FUN_004d7ff0(0x80000013,uVar7);
  FUN_004d8480(uStack_108);
  return 0;
}
