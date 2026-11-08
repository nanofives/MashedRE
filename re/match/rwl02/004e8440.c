// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h3 0x004e8440: hand-written (local[7] request struct) + permuter. 70.8%. Residual (a): original does not tail-merge the four failure returns (pop edi/pop esi/add esp/ret inline after each jne, eax already 0), ours jumps to one shared epilogue with xor eax,eax; same effect as in 0x004e98d0. Tried return 0 / return iVarN, goto, nested ifs, flags /O1 /GS /Ob0 /Ox /Os. */
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
MGLOBAL(unsigned int, DAT_006186a4, 0x006186a4);
int FUN_004cbe80(int p0, void * p1, int p2);
int FUN_004cc580(int p0, int p1, int p2, int p3, int p4);
int FUN_004cf460(int p0);
int FUN_004cf4a0(int p0, int p1);
int FUN_004e1ce0(void * p0, int * p1);
int FUN_004e1d20(void * p0, int p1, int * p2);
// MATCH: 0x004e8440 FUN_004e8440
unsigned int FUN_004e8440(int *param_1, int param_2)
{
  int iVar1;
  int iVar2;
  int local[7];

  iVar2 = 0x28;
  if (*param_1 != 0) {
    iVar2 = FUN_004cf460(*param_1);
    iVar2 = iVar2 + 0x34;
  }
  iVar1 = FUN_004e1ce0(&DAT_006186a4, param_1);
  iVar2 = FUN_004cc580(param_2, 7, iVar2 + iVar1 + 0xc, 0x37002, 10);
  if (iVar2 == 0) {
    return iVar2;
  }
  iVar2 = FUN_004cc580(param_2, 1, 0x1c, 0x37002, 10);
  if (iVar2 == 0) {
    return iVar2;
  }
  local[4] = param_1[3];
  local[5] = param_1[4];
  local[3] = (unsigned int)(*param_1 != 0);
  local[0] = 0;
  local[6] = param_1[5];
  local[1] = param_1[1];
  iVar2 = FUN_004cbe80(param_2, local, 0x1c);
  if (iVar2 == 0) {
    return iVar2;
  }
  if (*param_1 != 0) {
    iVar2 = FUN_004cf4a0(*param_1, param_2);
    if (iVar2 == 0) {
      return iVar2;
    }
  }
  iVar2 = FUN_004e1d20(&DAT_006186a4, param_2, param_1);
  return iVar2 != 0 ? (unsigned int)param_1 : 0;
}


