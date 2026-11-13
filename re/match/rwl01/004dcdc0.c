// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004dcdc0: 35.7% regs 81.1%.  class (b) Allocator teardown; ops 97.2% but layout differs: original keeps list head in ecx (loop var in eax, do-while with explicit entry test) while ours swaps the two registers. Tried head hoist, do-while form, declaration order, literal compares; permuter 53% variant moves a read after the free (semantic change, rejected).. */
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
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
#ifndef __cplusplus
#define true 1
#endif
/* RWC-CORE-RUN run 38: the two `call [reg+8]` at 0x004dce28 / 0x004dcedd are COM Release, __stdcall (the callee pops
   the argument; no `add esp, 4` follows either call in the original). Typed through `code` (cdecl) both callers also
   popped it, and 0x004dcdc0 returned through the wrong stack slot on the RW core path. */
typedef unsigned long __stdcall fpRelease(void *self);
MGLOBAL(int, DAT_007d70c0, 0x007d70c0);
MGLOBAL(undefined4, DAT_007d70c8, 0x007d70c8);
MGLOBAL(int, DAT_007d70c4, 0x007d70c4);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d70b0, 0x007d70b0);
MGLOBAL(int, DAT_007d70b8, 0x007d70b8);
MGLOBAL(int, DAT_007d70bc, 0x007d70bc);
MGLOBAL(int, DAT_007d70b4, 0x007d70b4);
MGLOBAL(undefined4, DAT_007d70cc, 0x007d70cc);
MGLOBAL(undefined4, DAT_007d70dc, 0x007d70dc);
MGLOBAL(undefined4, DAT_007d70ec, 0x007d70ec);
undefined4 FUN_004cc9f0(int p0);
// MATCH: 0x004dcdc0 FUN_004dcdc0

void FUN_004dcdc0(void)

{
  int *piVar1;
  int iVar4;
  undefined4 uVar3;
  int iVar2;
  uint uVar5;
  
  iVar4 = DAT_007d70c0;
  DAT_007d70c8 = 0;
  uVar5 = 0;
  do {
    *(undefined4 *)((int)&DAT_007d70cc + uVar5) = 0;
    *(undefined4 *)((int)&DAT_007d70dc + uVar5) = 0;
    if (*(int *)((int)&DAT_007d70ec + uVar5) != 0) {
      iVar2 = iVar4;
      if (iVar2 != 0) {
        do {
          if (*(int *)(iVar2 + 8) == *(int *)((int)&DAT_007d70ec + uVar5)) {
            *(undefined4 *)(iVar2 + 4) = 0;
            *(undefined4 *)(iVar2 + 0xc) = 0;
            iVar4 = DAT_007d70c0;
            break;
          }
          iVar2 = *(int *)(iVar2 + 0x10);
        } while (iVar2 != 0);
      }
      *(undefined4 *)((int)&DAT_007d70ec + uVar5) = 0;
    }
    uVar5 = uVar5 + 4;
    if (uVar5 >= 0x10) {
      while (iVar4 != 0) {
        piVar1 = *(int **)(iVar4 + 8);
        iVar2 = *(int *)(iVar4 + 0x10);
        if (piVar1 != (int *)0x0) {
          (**(fpRelease **)(*piVar1 + 8))(piVar1);
          *(undefined4 *)(DAT_007d70c0 + 8) = 0;
          iVar4 = DAT_007d70c0;
          if (*(undefined4 **)(DAT_007d70c0 + 0xc) != (undefined4 *)0x0) {
            **(undefined4 **)(DAT_007d70c0 + 0xc) = 0;
            iVar4 = DAT_007d70c0;
          }
        }
        (**(code **)(DAT_007d3ff8 + 0x11c))(DAT_007d70c4,iVar4);
        iVar4 = iVar2;
        DAT_007d70c0 = iVar2;
      }
      uVar3 = 0;
      if (DAT_007d70c4 != 0) {
        FUN_004cc9f0(DAT_007d70c4);
        DAT_007d70c4 = 0;
        uVar3 = DAT_007d70c4;
      }
      while (DAT_007d70c4 = uVar3, DAT_007d70b0 != 0) {
        iVar2 = *(int *)(DAT_007d70b0 + 0xc);
        iVar4 = *(int *)(DAT_007d70b0 + 4);
        while (iVar4 != 0) {
          uVar3 = *(undefined4 *)(iVar4 + 0xc);
          (**(code **)(DAT_007d3ff8 + 0x11c))(DAT_007d70b8,iVar4);
          *(undefined4 *)(DAT_007d70b0 + 4) = uVar3;
          iVar4 = *(int *)(DAT_007d70b0 + 4);
        }
        piVar1 = *(int **)(DAT_007d70b0 + 8);
        while (piVar1 != (int *)0x0) {
          iVar4 = piVar1[1];
          piVar1 = (int *)*piVar1;
          if (piVar1 != (int *)0x0) {
            (**(fpRelease **)(*piVar1 + 8))(piVar1);
          }
          (**(code **)(DAT_007d3ff8 + 0x11c))(DAT_007d70bc,*(undefined4 *)(DAT_007d70b0 + 8));
          *(int *)(DAT_007d70b0 + 8) = iVar4;
          piVar1 = *(int **)(DAT_007d70b0 + 8);
        }
        (**(code **)(DAT_007d3ff8 + 0x11c))(DAT_007d70b4,DAT_007d70b0);
        DAT_007d70b0 = iVar2;
        uVar3 = DAT_007d70c4;
      }
      if (DAT_007d70bc != 0) {
        FUN_004cc9f0(DAT_007d70bc);
        DAT_007d70bc = 0;
      }
      if (DAT_007d70b8 != 0) {
        FUN_004cc9f0(DAT_007d70b8);
        DAT_007d70b8 = 0;
      }
      if (DAT_007d70b4 != 0) {
        FUN_004cc9f0(DAT_007d70b4);
        DAT_007d70b4 = 0;
      }
      return;
    }
  } while( true );
}
