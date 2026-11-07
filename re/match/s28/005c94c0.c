// FLAGS: /O1
/* MATCH-S28 h1 0x005c94c0 : 17.5% (regs 35, ops 53) at O1 (O2 /Ob1 /G5 12.9, O2 /GS /Ob1 15.0). Raw decompiler structure compiled by hand with the callee 0x005c95b0 given 4 cdecl args; bitmap run-finder (bit loop over a byte, run length vs param_2, max-run local kept on the stack in the original although it is dead). Not restructured further: the original keeps the dead max-run in memory and reloads param_2 inside the bit loop, which our optimizer removes; goto/loop forms not tried beyond the raw shape. Residual class (b), ran out of budget. */

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

void FUN_005c95b0(int *p0, unsigned int p1, int p2, int p3);
// MATCH: 0x005c94c0 FUN_005c94c0
int FUN_005c94c0(int *param_1, unsigned int param_2, unsigned int *param_3)
{
  unsigned char bVar1;
  unsigned int uVar2;
  unsigned int uVar3;
  unsigned int uVar4;
  unsigned int uVar5;
  unsigned int local_8;

  uVar2 = 0;
  uVar5 = 0;
  uVar4 = 0;
  local_8 = 0;
  if (param_1[1] != 0) {
    do {
      bVar1 = *(unsigned char *)(*param_1 + uVar2);
      if (bVar1 == 0) {
        if (uVar4 == 0) {
          uVar5 = uVar2 * 8;
        }
        uVar4 = uVar4 + 8;
LAB_005c9503:
        if (local_8 < uVar4) {
          local_8 = uVar4;
        }
      }
      else {
        if (bVar1 != 0xff) {
          uVar3 = 0;
          do {
            if (((unsigned int)bVar1 & 1 << ((unsigned char)uVar3 & 0x1f)) == 0) {
              if (uVar4 == 0) {
                uVar5 = uVar3 + uVar2 * 8;
              }
              uVar4 = uVar4 + 1;
            }
            else {
              uVar5 = uVar3 + uVar2 * 8;
              uVar4 = 0;
            }
          } while ((uVar4 != param_2) && (uVar3 = uVar3 + 1, uVar3 < 8));
          goto LAB_005c9503;
        }
        uVar5 = uVar2 * 8;
        uVar4 = 0;
      }
      if (param_2 <= uVar4) goto LAB_005c955c;
      uVar2 = uVar2 + 1;
    } while (uVar2 < (unsigned int)param_1[1]);
  }
  if (uVar4 < param_2) {
    param_2 = uVar4;
    *param_3 = 0;
    return 0;
  }
  param_2 = uVar4;
LAB_005c955c:
  FUN_005c95b0(param_1, uVar5, param_2, 1);
  *param_3 = param_2 << (*(unsigned char *)(param_1 + 2) & 0x1f) | uVar5;
  return 1;
}
