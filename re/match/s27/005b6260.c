// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005b6260: 17.3% (regs 17.3%), raw converter body kept (best equivalent form). Hand rewrites (short* out, k/sh/c1/c2 locals, no shift mask) scored 8-14%; the permuter reached 42% only by changing the semantics (rejected). Residual (b): original keeps loop state in a memory-heavy frame (counters [esp+0x14]/[esp+0x20], prev sample [esp+0x10]/[esp+0x1c]) and schedules both ebp+=2; register allocation not found. */
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
typedef unsigned int uint;
typedef unsigned char byte;
MGLOBAL(undefined1, DAT_00634410, 0x00634410);
MGLOBAL(undefined1, DAT_00634414, 0x00634414);
// MATCH: 0x005b6260 FUN_005b6260
void FUN_005b6260(byte *param_1,undefined2 *param_2,int param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  byte bVar4;
  int iVar5;
  undefined2 *puVar6;
  undefined2 *puVar7;
  uint uVar8;
  int local_8;
  int local_4;
  
  iVar5 = 0;
  local_8 = 0;
  if (param_3 != 0) {
    local_4 = param_3;
    puVar6 = param_2;
    do {
      param_2 = (undefined2 *)0xe;
      uVar8 = (uint)(*param_1 >> 4);
      bVar4 = 0x1c - (*param_1 & 0xf);
      param_1 = param_1 + 2;
      iVar1 = *(int *)(&DAT_00634410 + uVar8 * 8);
      iVar2 = *(int *)(&DAT_00634414 + uVar8 * 8);
      puVar7 = puVar6;
      do {
        puVar6 = puVar7 + 2;
        iVar3 = (((int)((uint)*param_1 << 0x1c) >> 0x1c) << (bVar4 & 0x1f)) + local_8 * iVar2 +
                iVar5 * iVar1;
        local_8 = iVar3 >> 0x10;
        *puVar7 = (short)((uint)iVar3 >> 0x10);
        iVar3 = (((int)((uint)(*param_1 >> 4) << 0x1c) >> 0x1c) << (bVar4 & 0x1f)) + local_8 * iVar1
                + iVar5 * iVar2;
        iVar5 = iVar3 >> 0x10;
        puVar7[1] = (short)((uint)iVar3 >> 0x10);
        param_1 = param_1 + 1;
        param_2 = (undefined2 *)((int)param_2 + -1);
        puVar7 = puVar6;
      } while (param_2 != (undefined2 *)0x0);
      local_4 = local_4 + -1;
    } while (local_4 != 0);
  }
  return;
}
