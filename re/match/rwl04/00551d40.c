// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00551d40 h1: best 41.5% (regs 42.0%) from permuter, vs 27.9% hand form (regs 57.4%, ops 90.7%); structure only (649 B). Locals as struct {v[3]; m[16]} (frame 0x68). Register allocation differs (loop counter / g_ptr+offset registers). Class (b). */
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
typedef int (*code2)(int *, int *);
typedef struct { int v[3]; int m[16]; } Loc;
MGLOBAL(int, DAT_009124f8, 0x009124f8);
MGLOBAL(float, DAT_005cd100, 0x005cd100);
MGLOBAL(int, DAT_005e488c, 0x005e488c);
MGLOBAL(int, DAT_005e4898, 0x005e4898);
int FUN_00552050(int *a, int *b);
int FUN_005524c0(int *a, int *b);
void FUN_004c45f0(int *p0);
int FUN_004c4d20(int *p0, int *p1, float p2, int p3);
void FUN_004c51a0(int *p0, int *p1, int p2);
void FUN_004c52f0(int p0, int *p1, int p2);
void FUN_005402d0(int p0, int p1, int p2);
// MATCH: 0x00551d40 FUN_00551d40
int FUN_00551d40(int param_1)
{
  int *piVar3;
  unsigned int *puVar2;
  code2 pcVar5;
  int iVar6;
  unsigned int *puVar1;
  int iVar4;
  int iVar7;
  int *local_64;
  unsigned int uVar8;
  int *piVar9;
  int *local_68;
  int local_5c;
  int local_58;
  Loc L;
  
  puVar1 = (unsigned int *)(DAT_009124f8 + param_1);
  uVar8 = 0;
  do {
    puVar2 = (unsigned int *)puVar1[uVar8];
    if (puVar2 != 0) {
      puVar2[10] = 0x3f800000;
      puVar2[2] = 0;
      puVar2[5] = 0x3f800000;
      *puVar2 = 0x3f800000;
      puVar2[9] = 0;
      puVar2[8] = 0;
      puVar2[4] = 0;
      puVar2[0xe] = 0;
      puVar2[6] = 0;
      puVar2[1] = 0;
      puVar2[0xc] = 0;
      puVar2[0xd] = 0;
      puVar2[3] = puVar2[3] | 0x20003;
    }
    uVar8 = uVar8 + 1;
  } while (uVar8 < 2);
  local_64 = (int *)(puVar1 + 2);
  local_58 = 8;
  do {
    piVar3 = (int *)*local_64;
    if (piVar3 != 0) {
      pcVar5 = (code2)piVar3[0xf];
      iVar4 = piVar3[9];
      local_68 = piVar3 + 0x13;
      iVar6 = *(int *)(*piVar3 + 0x14);
      local_5c = 0;
      if (0 < piVar3[0xb]) {
        piVar9 = piVar3 + 0x18;
        do {
          uVar8 = *(unsigned int *)(iVar6 + 0x20 + local_5c * 4);
          if ((uVar8 < 2) && (iVar7 = puVar1[uVar8], iVar7 != 0)) {
            if (pcVar5 == FUN_005524c0) {
              L.m[0] = piVar9[-3];
              L.m[1] = piVar9[-2];
              L.m[4] = piVar9[-1];
              L.m[5] = *piVar9;
              L.m[12] = piVar9[1];
              L.m[2] = 0;
              L.m[6] = 0;
              L.m[8] = 0;
              L.m[13] = piVar9[2];
              L.m[9] = 0;
              L.m[10] = 0;
              FUN_004c45f0(L.m);
              L.m[14] = 0;
            }
            else if (pcVar5 == FUN_00552050) {
              L.m[0] = piVar9[-2];
              L.m[5] = piVar9[-1];
              L.m[1] = *piVar9;
              L.m[12] = piVar9[1];
              L.v[0] = 0;
              L.v[1] = 0;
              L.m[13] = piVar9[2];
              L.v[2] = 0x3f800000;
              L.m[2] = 0;
              L.m[4] = 0;
              L.m[6] = 0;
              L.m[8] = 0;
              L.m[10] = 0;
              FUN_004c45f0(L.m);
              L.m[9] = 0;
              FUN_004c51a0(L.m,&DAT_005e488c,2);
              FUN_004c4d20(L.m,L.v,(float)piVar9[-3] * DAT_005cd100,2);
              L.m[14] = 0;
              FUN_004c51a0(L.m,&DAT_005e4898,2);
            }
            else {
              (*pcVar5)(L.m,local_68);
            }
            FUN_004c52f0(iVar7,L.m,1);
            piVar9 = (int *)((int)piVar9 + iVar4);
            local_68 = (int *)((int)local_68 + iVar4);
          }
          local_5c = local_5c + 1;
        } while (local_5c < piVar3[0xb]);
      }
    }
    local_64 = 1 + local_64;
    local_58 = local_58 + -1;
  } while (local_58 != 0);
  FUN_005402d0(param_1,*puVar1,puVar1[1]);
  return param_1;
}
