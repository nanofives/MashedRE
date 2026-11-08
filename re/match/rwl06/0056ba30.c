// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 h1 0x0056ba30: 100.0% (regs 100.0%), class -. early return false, unsigned >0, inc after store. Tried: direct. cxx:ok. */
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
uint FUN_00568990(int p0, int p1, uint p2, int p3, uint p4);
int FUN_0056b7a0(undefined4 * p0, int p1, uint p2, int p3);
// MATCH: 0x0056ba30 FUN_0056ba30
int FUN_0056ba30(int param_1,int param_2)
{
  undefined4 *puVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;

  iVar2 = *(int *)(param_1 + 4);
  *(undefined4 *)(iVar2 + 0x84) = 0;
  if (!(*(uint *)(iVar2 + 0x90) < *(uint *)(iVar2 + 0x94))) {
    return 0;
  }
  puVar1 = (undefined4 *)(*(int *)(iVar2 + 0xd4) + param_2 * 0x28);
  FUN_0056b7a0((undefined4 *)param_1,*puVar1,puVar1[3],
               *(int *)(*(int *)**(undefined4 **)(iVar2 + 0x70) + 0xc018));
  uVar3 = FUN_00568990(iVar2,*(int *)(iVar2 + 0x80),*(uint *)(iVar2 + 0x84),
                       *(int *)(iVar2 + 0x90) * 0xe0 + *(int *)(iVar2 + 0x8c),
                       *(int *)(iVar2 + 0x94) - *(int *)(iVar2 + 0x90));
  if (uVar3 != 0) {
    uVar5 = 0;
    if (0 < uVar3) {
      do {
        *(undefined4 *)((*(int *)(iVar2 + 0x90) + uVar5) * 0xe0 + 0xdc + *(int *)(iVar2 + 0x8c)) =
             puVar1[1];
        iVar4 = *(int *)(iVar2 + 0x90) + uVar5;
        puVar1[1] = iVar4 * 0xe0 + *(int *)(iVar2 + 0x8c);
        uVar5 = uVar5 + 1;
      } while (uVar5 < uVar3);
    }
    *(uint *)(iVar2 + 0x90) += uVar3;
  }
  return 0 < uVar3;
}
