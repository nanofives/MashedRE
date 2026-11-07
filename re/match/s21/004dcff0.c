// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004dcff0 (h4): 7.0% (regs 11.6%), raw converter body kept. The original is an inline-asm block: pushf/popf EFLAGS.ID toggle test, then cpuid with eax=1, edx stored to [ebp-4], and 'and 0x6000000; neg; sbb; neg', inside a push ebp/mov ebp,esp frame with ebx saved twice. The required source form forbids __asm and there is no compiler intrinsic for pushf/popf or cpuid in 13.00, so no C form can give these bytes. Class (c). */
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
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef unsigned long DWORD;
#ifndef __cplusplus
#define true 1
#endif
#ifndef __cplusplus
#define false 0
#endif
#define CONCAT22(a,b) ((uint)(((uint)(ushort)(a)<<16)|(ushort)(b)))
/* Ghidra pseudo-op for the cpuid instruction (no C equivalent; see header) */
int cpuid_Version_info(int leaf);
// MATCH: 0x004dcff0 FUN_004dcff0
/* [C1 2026-05-02] Same CPUID-support check as `FUN_004dcf90`. Returns true if **either** bit 25 or
   bit 26 of the DWORD
 */

bool FUN_004dcff0(void)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  undefined2 unaff_BX;
  byte in_AF;
  byte in_TF;
  byte in_IF;
  byte in_NT;
  undefined2 uStack_10;
  
  uVar2 = CONCAT22(unaff_BX,(ushort)(in_NT & 1) * 0x4000 | (ushort)(in_IF & 1) * 0x200 |
                            (ushort)(in_TF & 1) * 0x100 | 0x40 | (ushort)(in_AF & 1) * 0x10) | 4;
  uVar3 = uVar2 ^ 0x200000;
  uStack_10 = (undefined2)(uVar3 >> 0x10);
  if (CONCAT22(uStack_10,
               (ushort)((uVar3 & 0x4000) != 0) * 0x4000 | (ushort)((uVar3 & 0x200) != 0) * 0x200 |
               (ushort)((uVar3 & 0x100) != 0) * 0x100 | (ushort)((uVar3 & 0x40) != 0) * 0x40 |
               (ushort)((uVar3 & 0x10) != 0) * 0x10 | (ushort)((uVar3 & 4) != 0) * 4) == uVar2) {
    return false;
  }
  iVar1 = cpuid_Version_info(1);
  return (*(uint *)(iVar1 + 8) & 0x6000000) != 0;
}
