// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005b04b0: 30.6% (regs 52.4%, ops 70.4%), 192 vs 206 insns, FLAGS /O2 /Ob1 /G5 (the raw /GS line scored 29.6%). Typed rewrite (Lk circular-list node, Hd list head, uint index, int flags) replacing Ghidra's int ****** chains; relink loop, goto LAB_005b06aa, list scans and all 7 callee arities kept. Residual: this typed form makes the DDK build an ebp frame with and esp,-8 and spill locals differently (the raw converter form has no frame but 22-25%); bisected struct vs array locals, byte locals, removed relink loop: frame stays. Original has no frame (sub esp,0x44, esp-relative). Class (b). cxx checked below. */
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
typedef unsigned char byte;
typedef unsigned int uint;
typedef struct Lk { struct Lk *next; struct Lk *prev; } Lk;
typedef struct Hd { Lk lk; int w; } Hd;
int FUN_005a9ff0(int p0, int p1, Hd * p2);
void FUN_005ac980(int p0, uint * p1, int p2);
void FUN_005ade90(Hd * p0);
int FUN_005b0700(int p0, uint p1);
int FUN_005b0740(uint * p0, uint * p1, uint p2, int p3, Hd * p4, Lk * p5, int p6, int p7);
int FUN_005b0a90(Lk * p0, int p1, int p2, uint * p3, int p4, uint p5, int p6);
void FUN_005b0b40(Lk * p0);
// MATCH: 0x005b04b0 FUN_005b04b0
int FUN_005b04b0(int param_1,int param_2,int param_3,int param_4,Lk *param_5,int param_6)
{
  Lk *pk;
  uint *puVar2;
  int iVar3;
  Lk *pn;
  int bVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  Lk *pk9;
  uint uVar10;
  uint local_44;
  int local_40;
  Lk local_30;
  Hd local_28;
  uint local_1c;
  uint local_18;
  uint local_14;
  byte local_10;
  byte local_f;
  uint local_c;
  uint local_8;
  byte local_4;
  byte local_3;

  bVar5 = 0;
  local_44 = 0;
  if (*(uint *)(*(int *)(param_1 + 0xc) + 0x14) == 0) {
    return 1;
  }
  local_40 = 0;
  do {
    pk = param_5->next;
    while (1) {
      if (pk == param_5) {
        if (param_3 == 0) {
          return 0;
        }
        if (!bVar5) {
          local_28.lk.next = &local_28.lk;
          local_28.lk.prev = &local_28.lk;
          local_28.w = 0;
          iVar6 = FUN_005a9ff0(0,param_3,&local_28);
          if (iVar6 == 0) {
            return 0;
          }
          bVar5 = 1;
        }
        local_30.next = &local_30;
        local_30.prev = &local_30;
        iVar6 = 0;
        puVar2 = *(uint **)(*(int *)(*(int *)(param_1 + 0xc) + 0x18) + 0x14 + local_40);
        do {
          do {
            iVar7 = FUN_005b0700(param_6,iVar6);
            if (iVar7 == 0) {
              FUN_005ade90(&local_28);
              return 0;
            }
            iVar3 = *(int *)(iVar7 + 0x54);
            uVar10 = 0;
            iVar6 = *(int *)(iVar3 + 0x10) + 1;
          } while (*(int *)(iVar3 + 8) == 0);
          do {
            local_18 = 0;
            local_14 = 0;
            local_10 = 0;
            local_f = 0;
            local_4 = 0;
            local_3 = 0;
            local_c = 0;
            local_8 = 0;
            local_1c = *puVar2;
            FUN_005ac980(*(int *)(*(int *)(iVar3 + 4) + uVar10 * 4),&local_1c,0);
            iVar8 = FUN_005b0740(puVar2,&local_1c,local_44,param_3,&local_28,&local_30,param_1,0);
            if (iVar8 != 0) {
              ((int *)local_30.next)[-2] = param_4;
              iVar6 = FUN_005b0a90(&local_30,iVar7,param_2,&local_1c,0,local_44,(int)local_30.prev - 0x4c);
              if (iVar6 == 0) {
                FUN_005ade90(&local_28);
                FUN_005b0b40(&local_30);
                return 0;
              }
              pk9 = local_30.next;
              if (pk9 != &local_30) {
                do {
                  pn = pk9->next;
                  pk9->prev->next = pn;
                  pk9->next->prev = pk9->prev;
                  pk9->prev = 0;
                  pk9->next = 0;
                  pk9->next = param_5->next;
                  pk9->prev = param_5;
                  param_5->next->prev = pk9;
                  param_5->next = pk9;
                  pk9 = pn;
                } while (pn != &local_30);
              }
              goto LAB_005b06aa;
            }
            uVar10 = uVar10 + 1;
          } while (uVar10 < *(uint *)(iVar3 + 8));
        } while (1);
      }
      if (((uint *)pk)[-1] == local_44) break;
      pk = pk->next;
    }
LAB_005b06aa:
    local_40 = local_40 + 0x28;
    local_44 = local_44 + 1;
    if (*(uint *)(*(int *)(param_1 + 0xc) + 0x14) <= local_44) {
      return 1;
    }
  } while (1);
}
