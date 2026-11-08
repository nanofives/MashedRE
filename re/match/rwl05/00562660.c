// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x00562660: hand-matched (h1). 38.2% (regs 45.1%, ops 51.0%), 79/102 ins. Bounding-sphere getter by type 0..4 (0/1 share a block through FUN_00565cd0). Original has a different branch layout (shared 'src == NULL' tail, frame sub esp 0x20 at entry) and int-typed copies; tried raw Ghidra chain, 0||1 grouping, float vs int copies (best seen 44.1% with float copies). Class (b). */
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
void FUN_004e5fc0(int p0);
void FUN_00565cd0(unsigned int * p0, unsigned int * p1);
// MATCH: 0x00562660 FUN_00562660
unsigned int * FUN_00562660(int param_1,unsigned int *param_2)
{
  int iVar1;
  unsigned int *puVar2;
  unsigned int local_20, local_1c, local_18;
  unsigned int local_10, local_c, local_8;

  iVar1 = *(int *)(param_1 + 0x44);
  if (iVar1 == 0 || iVar1 == 1) {
    if (iVar1 == 1) {
      puVar2 = (unsigned int *)(*(int *)(param_1 + 0x40) + 0x48);
    } else {
      puVar2 = (unsigned int *)(*(int *)(param_1 + 0x40) + 0x50);
    }
    if (puVar2 == 0) {
      return 0;
    }
    local_10 = puVar2[3];
    local_c = puVar2[4];
    local_8 = puVar2[5];
    local_20 = *puVar2;
    local_1c = puVar2[1];
    local_18 = puVar2[2];
    FUN_00565cd0(param_2,&local_20);
    param_2[3] = *(unsigned int *)(param_1 + 0x4c) + param_2[3];
    return param_2;
  }
  if (iVar1 == 2) {
    if ((*(unsigned char *)(*(int *)(param_1 + 0x40) + 0x4c) & 2) != 0) {
      FUN_004e5fc0(*(int *)(param_1 + 0x40));
    }
    puVar2 = (unsigned int *)(*(int *)(param_1 + 0x40) + 0x1c);
  }
  else if (iVar1 == 3) {
    puVar2 = (unsigned int *)(*(int *)(*(int *)(param_1 + 0x40) + 0x5c) + 4);
  }
  else {
    if (iVar1 != 4) {
      return 0;
    }
    param_2[2] = 0;
    param_2[1] = 0;
    *param_2 = 0;
    param_2[3] = 0x7f7fffff;
    return param_2;
  }
  if (puVar2 == 0) {
    return 0;
  }
  *param_2 = *puVar2;
  param_2[1] = puVar2[1];
  param_2[2] = puVar2[2];
  param_2[3] = puVar2[3];
  param_2[3] = *(unsigned int *)(param_1 + 0x4c) + param_2[3];
  return param_2;
}
