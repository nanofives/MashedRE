// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00543bb0 h1: best 72.3% (regs 72.3%, ops 90.6%). Residue: orig dec/dec/dec switch chain and unmerged epilogues (pop ecx..ret copies) and push ecx local slot; ours cross-jumps failure exits. Case-per-label switch, ents!=0 branch first, &fn108 member pointer. Class (a)/(b). */
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
typedef int (*code2)(int, int);
typedef int (*code1)(int);
typedef struct { char pad0[0xcc]; code2 fnCC; char pad1[0xf4 - 0xd0]; code1 fnF4; char pad2[0x108 - 0xf8]; code2 fn108; code1 fn10c; } RwGlob;
typedef struct Ent { char *name; int type; int count; void *data; } Ent;
typedef struct Arr { int n; Ent *ents; } Arr;
MGLOBAL(RwGlob *, DAT_007d3ff8, 0x007d3ff8);
void *memcpy(void *, const void *, unsigned int);
void *memset(void *, int, unsigned int);
#pragma intrinsic(memcpy, memset)
// MATCH: 0x00543bb0 FUN_00543bb0
int FUN_00543bb0(Arr *param_1,char *param_2,int param_3,int param_4)
{
  Ent *puVar1;
  int iVar2;
  int iVar4;
  Ent *piVar5;
  int local_4;
  Ent *e;
  
  local_4 = -1;
  iVar4 = param_1->n;
  iVar2 = 0;
  if (0 < iVar4) {
    e = param_1->ents;
    do {
      if (e->data == 0) {
        local_4 = iVar2;
      }
      iVar2 = iVar2 + 1;
      e = e + 1;
    } while (iVar2 < iVar4);
    if (local_4 != -1) goto LAB_00543c49;
  }
  if (param_1->ents != 0) {
    puVar1 = (Ent *)DAT_007d3ff8->fn108((iVar4 + 1) * 0x10,0x3011f);
    if (puVar1 == 0) {
      return -1;
    }
    memcpy(puVar1, param_1->ents, param_1->n << 4);
    DAT_007d3ff8->fn10c((int)param_1->ents);
    param_1->ents = puVar1;
  }
  else {
    param_1->ents = (Ent *)DAT_007d3ff8->fn108((iVar4 + 1) * 0x10,0x3011f);
    if (param_1->ents == 0) {
      return -1;
    }
  }
  local_4 = param_1->n;
  param_1->n = local_4 + 1;
LAB_00543c49:
  piVar5 = param_1->ents + local_4;
  switch (param_3) {
  case 1:
    iVar4 = 4;
    break;
  case 2:
    iVar4 = 4;
    break;
  case 3:
    iVar4 = 4;
    break;
  default:
    iVar4 = 0;
    break;
  }
  piVar5->data = (void *)DAT_007d3ff8->fn108(iVar4 * param_4,0x3011f);
  if (piVar5->data == 0) {
    return -1;
  }
  switch (param_3) {
  case 1:
    iVar4 = 4;
    break;
  case 2:
    iVar4 = 4;
    break;
  case 3:
    iVar4 = 4;
    break;
  default:
    iVar4 = 0;
    break;
  }
  memset(piVar5->data, 0, iVar4 * param_4);
  piVar5->name = 0;
  if (param_2 != 0) {
    code2 *pf = &DAT_007d3ff8->fn108;
    piVar5->name = (char *)(*pf)(DAT_007d3ff8->fnF4((int)param_2) + 1, 0x30002);
    if (piVar5->name != 0) {
      DAT_007d3ff8->fnCC((int)piVar5->name,(int)param_2);
    }
  }
  piVar5->type = param_3;
  piVar5->count = param_4;
  return local_4;
}
