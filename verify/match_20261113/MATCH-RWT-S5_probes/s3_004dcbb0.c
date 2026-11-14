/* MATCH-RWT-S3 (Opus high) 2026-11-13: SEMANTIC FIX, score 77.5 -> 62.0% (ddk = rw9044). The found branch wrote
   `DAT_007d70c8 = idx; idx = uVar4;`, storing the OLD index; c2 drops that store as redundant (idx was loaded
   from DAT_007d70c8). The original stores the NEW index: 0x004dcc27 mov ebp,ecx / 0x004dcc2c mov [0x7d70c8],ebp.
   Now `idx = uVar4; DAT_007d70c8 = idx;`. The tail stores are back in the original order (DAT_007d70cc store at
   0x004dcce9 before the div at 0x004dccf2). The 77.5% listed in RANKED_MATCH-RWT-REFRESH.tsv and the
   permuter's 77.5% on this row both measured the store-dropping source and are void. Open residual: the
   original never gives param_1 a register (ecx copy at 0x004dcbb0, reload mov ebx,[esp+0x14] per loop pass at
   0x004dcc00, div dword ptr [esp+0x14] at 0x004dccf2) and keeps the end offset uVar5 in ebx; ours enregisters
   param_1 and spills uVar5 to param_1's home slot. */
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004dcbb0: 77.5% regs 80%.  class (b) Real prototype is cdecl 5 args (align,size,out_obj**,p4,out_idx*), not Ghidra's unaff_* form; slot index kept in a local mirrored to DAT_007d70c8; vtable call is __stdcall. Permuter swapped two independent stores. Remaining: original keeps param_1 in ecx early and uVar5/uVar6 in ebx/esi (register assignment).. */
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
typedef int (__stdcall *vfn_t)(int *, uint, uint, undefined4, uint);
MGLOBAL(uint, DAT_00911fbc, 0x00911fbc);
MGLOBAL(uint, DAT_007d70c8, 0x007d70c8);
MGLOBAL(int *, DAT_007d70c0, 0x007d70c0);
MGLOBALA(uint, DAT_007d70cc, 4, 0x007d70cc);
MGLOBALA(uint, DAT_007d70dc, 4, 0x007d70dc);
MGLOBALA(int *, DAT_007d70ec, 4, 0x007d70ec);
undefined4 FUN_004dcaa0(uint p0, int ** p1);
// MATCH: 0x004dcbb0 FUN_004dcbb0
int FUN_004dcbb0(uint param_1,int param_2,int **param_3,undefined4 param_4,uint *param_5)
{
  int *piVar2;
  int iVar3;
  int *pn;
  uint idx;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;

  idx = DAT_007d70c8;
  uVar7 = param_1 * param_2;
  if ((DAT_00911fbc & 0x10000) != 0) {
    uVar6 = ((DAT_007d70cc[idx] + param_1 - 1) / param_1) * param_1;
    uVar5 = uVar6 + uVar7;
    if (uVar5 <= DAT_007d70dc[idx]) goto LAB_004dcc4c;
    uVar4 = 0;
    do {
      if (uVar4 != idx) {
        uVar6 = ((DAT_007d70cc[uVar4] + param_1 - 1) / param_1) * param_1;
        uVar5 = uVar6 + uVar7;
        if (uVar5 <= DAT_007d70dc[uVar4]) {
          idx = uVar4;
          DAT_007d70c8 = idx;
          if (uVar4 < 4) goto LAB_004dcc4c;
          break;
        }
      }
      uVar4 = uVar4 + 1;
    } while (uVar4 < 4);
    idx = idx + 1;
    DAT_007d70c8 = idx;
    if (idx >= 4) {
      idx = 0;
      DAT_007d70c8 = idx;
    }
  }
  uVar6 = 0;
  uVar5 = uVar7;
LAB_004dcc4c:
  if (uVar7 > DAT_007d70dc[idx]) {
    for (pn = DAT_007d70c0; pn != 0; pn = (int *)pn[4]) {
      if ((int *)pn[2] == DAT_007d70ec[idx]) {
        pn[1] = 0;
        pn[3] = 0;
        idx = DAT_007d70c8;
        break;
      }
    }
    DAT_007d70dc[idx] = uVar7;
    FUN_004dcaa0(uVar7,&DAT_007d70ec[idx]);
    idx = DAT_007d70c8;
  }
  piVar2 = DAT_007d70ec[idx];
  if (uVar6 == 0) {
    iVar3 = (*(vfn_t *)(*piVar2 + 0x2c))(piVar2,0,uVar7,param_4,0x2800);
    DAT_007d70cc[DAT_007d70c8] = uVar7;
    *param_5 = 0;
  }
  else {
    iVar3 = (*(vfn_t *)(*piVar2 + 0x2c))(piVar2,uVar6,uVar7,param_4,0x1800);
    DAT_007d70cc[DAT_007d70c8] = uVar5;
    *param_5 = uVar6 / param_1;
  }
  *param_3 = DAT_007d70ec[DAT_007d70c8];
  return iVar3 >= 0;
}
