// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005acaa0 (h3): 60.4% exact, regs 84.0%. Residual class (b) memcpy intrinsic replaces the rep-movs loops and the branch order is flipped (if (param_3 != 0) first): 26.2 -> 60.4%. The permuter reached 70.2% by reordering the side-effecting FUN_005aeca0 calls, so it was rejected. */
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
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned char byte;
undefined4 FUN_005acd60(undefined4 *p0);
undefined4 * FUN_005ae0c0(undefined4 * p0, undefined4 * p1, int p2);
void FUN_005aeca0(byte ** p0, byte * p1, int p2);
#ifdef __cplusplus
extern "C" void *memcpy(void *, const void *, unsigned int);
#else
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)
#endif
// MATCH: 0x005acaa0 FUN_005acaa0
undefined4 * FUN_005acaa0(undefined4 *param_1,undefined4 *param_2,int param_3,int param_4)
{
  undefined4 *puVar1;
  int iVar2;
  undefined4 *puVar6;
  undefined4 *puVar7;

  iVar2 = param_4;
  puVar6 = param_2;
  puVar1 = param_1;
  if (param_3 != 0) {
    if (param_4 == 0) {
      memcpy(param_2, param_1, 0x1c);
      param_2 = param_2 + 7;
    }
    else {
      param_1 = (undefined4 *)(uint)(param_1[1] != 0);
      FUN_005aeca0((byte **)&param_2,(byte *)puVar1,4);
      FUN_005aeca0((byte **)&param_2,(byte *)&param_1,4);
      FUN_005aeca0((byte **)&param_2,(byte *)(puVar1 + 2),4);
      FUN_005aeca0((byte **)&param_2,(byte *)(puVar1 + 3),1);
      FUN_005aeca0((byte **)&param_2,(byte *)puVar1 + 0xd,1);
      *(undefined2 *)param_2 = 0;
      param_2 = (undefined4 *)((int)param_2 + 2);
      FUN_005aeca0((byte **)&param_2,(byte *)(puVar1 + 4),4);
      FUN_005aeca0((byte **)&param_2,(byte *)(puVar1 + 5),4);
      FUN_005aeca0((byte **)&param_2,(byte *)(puVar1 + 6),1);
      FUN_005aeca0((byte **)&param_2,(byte *)puVar1 + 0x19,1);
      *(undefined2 *)param_2 = 0;
      param_2 = (undefined4 *)((int)param_2 + 2);
    }
    if (puVar1[1] != 0) {
      FUN_005ae0c0((undefined4 *)puVar1[1],param_2,iVar2);
      param_2 = param_2 + 4;
    }
    if ((undefined4 *)puVar1[4] != 0) {
      memcpy(param_2, (void *)puVar1[4], puVar1[5]);
      *(byte *)(puVar1 + 6) = *(byte *)(puVar1 + 6) | 2;
      return puVar1;
    }
  }
  else {
    if (param_4 == 0) {
      memcpy(param_1, param_2, 0x1c);
    }
    else {
      FUN_005aeca0((byte **)&param_1,(byte *)param_2,4);
      FUN_005aeca0((byte **)&param_1,(byte *)(puVar6 + 1),4);
      FUN_005aeca0((byte **)&param_1,(byte *)(puVar6 + 2),4);
      FUN_005aeca0((byte **)&param_1,(byte *)(puVar6 + 3),1);
      FUN_005aeca0((byte **)&param_1,(byte *)puVar6 + 0xd,1);
      *(undefined2 *)param_1 = 0;
      param_1 = (undefined4 *)((int)param_1 + 2);
      FUN_005aeca0((byte **)&param_1,(byte *)(puVar6 + 4),4);
      FUN_005aeca0((byte **)&param_1,(byte *)(puVar6 + 5),4);
      FUN_005aeca0((byte **)&param_1,(byte *)(puVar6 + 6),1);
      FUN_005aeca0((byte **)&param_1,(byte *)puVar6 + 0x19,1);
      *(undefined2 *)param_1 = 0;
    }
    puVar7 = puVar6 + 7;
    if (puVar1[1] != 0) {
      FUN_005ae0c0(puVar7,puVar7,iVar2);
      iVar2 = (int)FUN_005acd60(puVar7);
      puVar1[1] = iVar2;
      if (iVar2 == 0) {
        return 0;
      }
      puVar7 = puVar6 + 0xb;
    }
    if (puVar1[4] != 0) {
      puVar1[4] = (undefined4)puVar7;
    }
  }
  return puVar1;
}
