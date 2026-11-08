// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x00564040: hand-matched (h1). 19.4% (regs 59.7%, ops 77.6%), 63/67 ins. Collect list + bubble sort; original reloads param_2 into ebp/ecx with and 0xffff and keeps the count in a stack slot. Raw form, 'n > 1', unsigned-int param + casts (13%) tried. Class (b). */
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
// MATCH: 0x00564040 FUN_00564040
void FUN_00564040(int param_1,unsigned short param_2,int param_3,int param_4,int param_5)
{
  unsigned short uVar1;
  unsigned short uVar2;
  int iVar3;
  unsigned int uVar4;
  unsigned int local_4;

  iVar3 = *(int *)(param_1 + 8);
  local_4 = 0;
  for (uVar1 = *(unsigned short *)(iVar3 + 2 + (unsigned int)param_2 * 8); uVar1 != param_2;
      uVar1 = *(unsigned short *)((unsigned int)uVar1 * 8 + 2 + iVar3)) {
    uVar2 = *(unsigned short *)((unsigned int)uVar1 * 8 + 4 + iVar3);
    if ((*(unsigned int *)(param_4 + (unsigned int)(uVar2 >> 5) * 4) & 1 << ((unsigned char)uVar2 & 0x1f)) == 0) {
      *(unsigned short *)(param_3 + local_4 * 2) = uVar2;
      local_4 = local_4 + 1;
    }
    iVar3 = *(int *)(param_1 + 8);
  }
  if (param_5 != 0) {
    while (local_4 > 1) {
      local_4 = local_4 - 1;
      uVar4 = 0;
      if (local_4 != 0) {
        do {
          uVar1 = *(unsigned short *)(param_3 + uVar4 * 2);
          uVar2 = *(unsigned short *)(param_3 + 2 + uVar4 * 2);
          if (uVar2 < uVar1) {
            *(unsigned short *)(param_3 + uVar4 * 2) = uVar2;
            *(unsigned short *)(param_3 + 2 + uVar4 * 2) = uVar1;
          }
          uVar4 = uVar4 + 1;
        } while (uVar4 < local_4);
      }
    }
  }
}
