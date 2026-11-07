// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005a8890 (h2): best 88.2% (regs 97.1%), class b. param_2/puVar2 register split differs (edx vs eax) at the two struct copies. relocheck BAD only because the code differs from the original. */
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
typedef struct { int a[7]; } S28_005a8890;
typedef struct { int a[9]; } S36_005a8890;
__declspec(dllimport) unsigned long __stdcall WaitForSingleObject(void * p0, unsigned long p1);
__declspec(dllimport) int __stdcall ReleaseSemaphore(void * p0, long p1, long * p2);
MGLOBAL(void *, DAT_007dcae0, 0x007dcae0);
int FUN_005aad40(void);
void FUN_005b0c70(int p0, int * p1);
// MATCH: 0x005a8890 FUN_005a8890
void FUN_005a8890(int param_1,int *param_2)

{
  int *piVar1;
  int *puVar2;
  int iVar3;

  WaitForSingleObject(DAT_007dcae0,0xffffffff);
  puVar2 = param_2;
  if (param_2 != 0) {
    if (param_2[2] != 0) {
      *(S28_005a8890 *)(param_1 + 0xc0) = *(S28_005a8890 *)param_2[2];
    }
    puVar2 = (int *)(param_1 + 0xd78);
    *(S36_005a8890 *)puVar2 = *(S36_005a8890 *)param_2;
    puVar2[2] = param_1 + 0xc0;
  }
  FUN_005b0c70(param_1 + 0x154,puVar2);
  piVar1 = (int *)(param_1 + 0x14);
  **(int **)(param_1 + 0x18) = *(int *)(param_1 + 0x14);
  *(int *)(*piVar1 + 4) = *(int *)(param_1 + 0x18);
  *(uint *)(param_1 + 0xd9c) = *(uint *)(param_1 + 0xd9c) | 1;
  ReleaseSemaphore(DAT_007dcae0,1,0);
  if (*piVar1 != 0) {
    **(int **)(param_1 + 0x18) = *piVar1;
    *(int *)(*piVar1 + 4) = *(int *)(param_1 + 0x18);
  }
  iVar3 = FUN_005aad40();
  *piVar1 = *(int *)(iVar3 + 0xc);
  iVar3 = FUN_005aad40();
  *(int *)(param_1 + 0x18) = iVar3 + 0xc;
  iVar3 = FUN_005aad40();
  *(int **)(*(int *)(iVar3 + 0xc) + 4) = piVar1;
  iVar3 = FUN_005aad40();
  *(int **)(iVar3 + 0xc) = piVar1;
}
