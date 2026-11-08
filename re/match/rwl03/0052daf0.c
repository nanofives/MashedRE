// FLAGS: /O2 /Ob1 /G5
// h3: 42.5% regs 34.0%. Linear goto form for the two table lookups, 12-dword table struct, error record struct. Original saves all four callee-saved at entry and has 0x20 of locals; class (b).
/* MATCH-RWL03 0x0052daf0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
typedef int (*Cb108_52daf0)(int, int);
typedef int (*CbE_52daf0)(int, int *);
typedef struct { int id; int p4; int p8; int pc; int pad[4]; CbE_52daf0 fn; int pad2[2]; int sz; } T52daf0;
MGLOBAL(int, DAT_007dbcd8, 0x007dbcd8);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(T52daf0, DAT_00912160, 1, 0x00912160);
int FUN_004cc790(int p0, int *p1, int p2);
int FUN_004d7ff0(int p0);
void FUN_004d8480(int *p0);
// MATCH: 0x0052daf0 FUN_0052daf0
int * FUN_0052daf0(int param_1)
{
  int iVar1;
  int *puVar2;
  T52daf0 *piVar3;
  int local_20;
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  int local_c;
  T52daf0 *entry;
  T52daf0 *e;
  struct { int a; int b; } st;

  iVar1 = FUN_004cc790(param_1,&local_1c,4);
  if (iVar1 == 0) {
    return 0;
  }
  if (local_1c != 0x100) {
    return 0;
  }
  iVar1 = FUN_004cc790(param_1,&local_20,4);
  if (iVar1 == 0) {
    return 0;
  }
  iVar1 = 0;
  if (0 < DAT_007dbcd8) {
    piVar3 = DAT_00912160;
    do {
      if (piVar3->id == local_20) goto found1;
      iVar1 = iVar1 + 1;
      piVar3 = piVar3 + 1;
    } while (iVar1 < DAT_007dbcd8);
  }
  st.a = 0x1b7;
  st.b = FUN_004d7ff0(2);
  FUN_004d8480(&st.a);
  return 0;
found1:
  entry = &DAT_00912160[iVar1];
  if (entry == 0) {
    return 0;
  }
  iVar1 = FUN_004cc790(param_1,&local_10,4);
  if (iVar1 == 0) {
    return 0;
  }
  iVar1 = FUN_004cc790(param_1,&local_14,4);
  if (iVar1 == 0) {
    return 0;
  }
  iVar1 = FUN_004cc790(param_1,&local_18,4);
  if (iVar1 == 0) {
    return 0;
  }
  local_c = local_18;
  iVar1 = 0;
  if (0 < DAT_007dbcd8) {
    piVar3 = DAT_00912160;
    do {
      if (piVar3->id == local_20) goto found2;
      iVar1 = iVar1 + 1;
      piVar3 = piVar3 + 1;
    } while (iVar1 < DAT_007dbcd8);
  }
  st.a = 0x1b7;
  st.b = FUN_004d7ff0(2);
  FUN_004d8480(&st.a);
  return 0;
found2:
  e = &DAT_00912160[iVar1];
  if (e == 0) {
    return 0;
  }
  puVar2 = (int *)(*(Cb108_52daf0 *)(DAT_007d3ff8 + 0x108))(e->p8 * local_10 + 0x18 + e->sz,0x3001b);
  puVar2[3] = local_c;
  puVar2[1] = local_10;
  puVar2[2] = local_14;
  *puVar2 = (int)e;
  puVar2[4] = (int)(puVar2 + 6);
  if (e->sz < 1) {
    puVar2[5] = 0;
  }
  else {
    puVar2[5] = e->p8 * local_10 + (int)(puVar2 + 6);
  }
  (*entry->fn)(param_1,puVar2);
  return puVar2;
}
