// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 0x005c47e0: 80.0% (regs 80.3%). Compile-fail raw rewritten: float10 returns as double (x87 st0), FUN_004d7ff0 is variadic (1 and 2 arg calls), err_t {code,val} frame struct, 14-dword read buffer, node copy via puVar1[0,1,2,4,5,6,8,9,10,12,13,14]; error branch moved to else; permuter reordered independent stores and used for-loops (+17.6). Residual: x87 compare shapes and schedule. Class: b. */
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
typedef struct { int code; int val; } err_t;
typedef int (__cdecl *alloc_fn)(int, int);
typedef void (__cdecl *free_fn)(int);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00635cf0, 0x00635cf0);
MGLOBAL(float, DAT_005cc328, 0x005cc328);
MGLOBAL(float, DAT_005cc9b4, 0x005cc9b4);
MGLOBAL(undefined4, DAT_00617f78, 0x00617f78);
int FUN_004c0b30(void);
unsigned int FUN_004c0de0(int p0);
void FUN_004c0e50(int p0);
unsigned int FUN_004c0ef0(int p0);
int FUN_004c0f00(int p0, int p1);
int FUN_004c1040(void);
double FUN_004c4220(float *p0);
double FUN_004c4270(float *p0);
double FUN_004c42d0(float *p0);
unsigned int FUN_004cbd30(int p0, void *p1, unsigned int p2);
unsigned int FUN_004cc5e0(int p0, int p1, void *p2, unsigned int *p3);
int FUN_004d7ff0(int p0, ...);
void FUN_004d8480(err_t *p0);
int FUN_004e1b60(void *p0, int p1, int p2);
// MATCH: 0x005c47e0 FUN_005c47e0
int *FUN_005c47e0(int param_1, int *param_2)
{
  float *puVar1;
  int local_4c;
  int uVar5;
  int iVar4;
  int iVar2;
  double fVar6;
  char local_44[4];
  int buf[14];
  unsigned int local_48;
  int iVar3;
  err_t e;

  iVar2 = FUN_004cc5e0(param_1, 1, local_44, &local_48);
  if (iVar2 != 0) {
    if (!(local_48 >= 0x35000 && local_48 <= 0x37002)) {
      e.code = 1;
      e.val = FUN_004d7ff0(0x80000004);
      FUN_004d8480(&e);
    } else {
      iVar2 = FUN_004cbd30(param_1, &local_4c, 4);
      if (iVar2 == 4) {
        param_2[1] = local_4c;
        iVar2 = ((alloc_fn)*(int *)(DAT_007d3ff8 + 0x108))(local_4c << 2, 0x3000e);
        *param_2 = iVar2;
        if (iVar2 == 0) {
          e.code = 1;
          e.val = FUN_004d7ff0(0x80000013, local_4c * 4);
          FUN_004d8480(&e);
          return (int *)0;
        }
        for (iVar2 = 0; iVar2 < local_4c; iVar2++) {
          iVar4 = FUN_004cbd30(param_1, buf, 0x38);
          if (iVar4 != 0x38) {
            ((free_fn)*(int *)(DAT_007d3ff8 + 0x10c))(*param_2);
            return (int *)0;
          }
          iVar4 = FUN_004c0b30();
          if (iVar4 == 0) {
            ((free_fn)*(int *)(DAT_007d3ff8 + 0x10c))(*param_2);
            return (int *)0;
          }
          puVar1 = (float *)(iVar4 + 0x10);
          ((int *)puVar1)[1] = buf[1];
          ((int *)puVar1)[2] = buf[2];
          ((int *)puVar1)[0] = buf[0];
          ((int *)puVar1)[4] = buf[3];
          ((int *)puVar1)[5] = buf[4];
          ((int *)puVar1)[8] = buf[6];
          ((int *)puVar1)[9] = buf[7];
          ((int *)puVar1)[6] = buf[5];
          ((int *)puVar1)[10] = buf[8];
          ((int *)puVar1)[14] = buf[11];
          ((int *)puVar1)[12] = buf[9];
          ((int *)puVar1)[13] = buf[10];
          fVar6 = FUN_004c42d0(puVar1);
          if (((fVar6 > DAT_005cc328) ||
              (fVar6 = FUN_004c4270(puVar1), fVar6 > DAT_005cc328)) ||
             (fVar6 = FUN_004c4220(puVar1), fVar6 < DAT_005cc9b4)) {
            uVar5 = *(unsigned int *)(iVar4 + 0x1c) & 0xfffdfffc;
          }
          else {
            uVar5 = *(unsigned int *)(iVar4 + 0x1c) & 0xfffdffff;
          }
          *(unsigned int *)(iVar4 + 0x1c) = uVar5;
          *(int *)(*param_2 + iVar2 * 4) = iVar4;
          if (buf[12] >= 0) {
            if (DAT_00635cf0 == 0) {
              FUN_004c0f00(*(int *)(*param_2 + 4 * buf[12]), iVar4);
            }
            else {
              FUN_004c1040();
            }
          }
        }
        iVar2 = 0;
        if (0 < local_4c) {
          do {
            iVar4 = *(int *)(*param_2 + iVar2 * 4);
            iVar3 = FUN_004e1b60(&DAT_00617f78, param_1, iVar4);
            if (iVar3 == 0) {
              FUN_004c0de0(*(int *)*param_2);
              ((free_fn)*(int *)(DAT_007d3ff8 + 0x10c))(*param_2);
              return (int *)0;
            }
            iVar3 = FUN_004c0ef0(iVar4);
            if ((iVar4 == iVar3) && (DAT_00635cf0 == 1)) {
              FUN_004c0e50(iVar4);
            }
            iVar2 = iVar2 + 1;
          } while (iVar2 < local_4c);
        }
        return param_2;
      }
    }
  }
  return (int *)0;
}


