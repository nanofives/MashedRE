// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR01 2026-11-08: 100% (regs 100%), relocheck OK, was 98.2%. The bitmap sum reads through a byte pointer m = (byte *)(piVar4 + 2) set before the loop (m[uVar5]); that gives the [ecx+eax+8] index/base order. Supersedes the header below. */
/* MATCH-RWL01 0x004ccc50: best 98.2% (regs 100%, ops 100%), class (a). Rewritten as for-loops (list unlink, byte-sum loop, relink); only the base/index order of [ecx+eax+8] differs (9 index spellings, permuter 284 compiles). */
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
typedef unsigned int uint;
typedef unsigned char byte;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
#ifndef __cplusplus
#define true 1
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
// MATCH: 0x004ccc50 FUN_004ccc50
int FUN_004ccc50(int *param_1)
{
  int *piVar1;
  int *piVar3;
  int *piVar4;
  uint uVar2;
  uint uVar5;
  int iVar6;
  int local_4;
  local_4 = 0;
  uVar2 = param_1[2];
  piVar1 = param_1 + 4;
  for (piVar4 = (int *)param_1[4]; piVar4 != piVar1; piVar4 = piVar3) {
    *(int *)piVar4[1] = *piVar4;
    *(int *)(*piVar4 + 4) = piVar4[1];
    piVar3 = (int *)*piVar4;
    iVar6 = 0;
    { byte *m = (byte *)(piVar4 + 2); for (uVar5 = 0; uVar5 < uVar2; uVar5++) {
      iVar6 = iVar6 + m[uVar5];
    } }
    if (iVar6 == 0) {
      (**(code **)(DAT_007d3ff8 + 0x10c))(piVar4);
      local_4 = local_4 + 1;
    }
    else {
      iVar6 = *piVar1;
      *piVar4 = iVar6;
      piVar4[1] = (int)piVar1;
      *(int *)(*piVar1 + 4) = (int)piVar4;
      *piVar1 = (int)piVar4;
    }
  }
  return *param_1 * local_4;
}
