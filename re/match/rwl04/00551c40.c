// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00551c40 h1: best 80.5% (regs 80.5%, ops 85.4%). Orig has no shrink-wrap (push edi at entry, 3 duplicated epilogues returning eax); ours shrink-wraps edi and emits xor eax,eax on 2nd fail. Tried nested/early-return/uint callee return/struct vtable. Class (a)/(b). */
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
typedef int (*code)(int,int);
typedef struct { char pad[0x118]; code fn118; } RwGlob;
MGLOBAL(int, DAT_007dc770, 0x007dc770);
MGLOBAL(RwGlob *, DAT_007d3ff8, 0x007d3ff8);
int FUN_004cbd30(int p0, int p1, int p2);
int FUN_004cc790(int p0, int p1, int p2);
// MATCH: 0x00551c40 FUN_00551c40
int FUN_00551c40(int param_1)
{
  int iVar1;
  int iVar2;
  
  iVar1 = DAT_007d3ff8->fn118(DAT_007dc770,0x30135);
  if (iVar1 == 0) {
    return iVar1;
  }
  iVar2 = FUN_004cbd30(param_1,iVar1,0x20);
  if (iVar2 == 0) {
    return iVar2;
  }
  iVar2 = FUN_004cc790(param_1,iVar1 + 0x20,0x20);
  if (iVar2 == 0) {
    return iVar2;
  }
  *(int *)(iVar1 + 0x40) = 1;
  return iVar1;
}
