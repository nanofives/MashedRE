// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x0052ff00: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
#ifdef __cplusplus
typedef int (__cdecl code)(...);
#else
typedef int code();
#endif
MGLOBAL(undefined4, DAT_007dc518, 0x007dc518);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
undefined4 FUN_004cc5e0(undefined4 p0, int p1, void * p2, void * p3);
undefined4 FUN_004cc790(undefined4 p0, undefined4 p1, undefined4 p2);
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 * FUN_004d8480(undefined4 * p0);
int FUN_0052eeb0(int p0, int p1);
// MATCH: 0x0052ff00 FUN_0052ff00
uint FUN_0052ff00(uint param_1,undefined4 param_2)

{
  uint uVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  int local_10;
  undefined1 local_c [4];
  struct { undefined4 a; undefined4 b; } st;
  
  uVar1 = param_1;
  iVar2 = FUN_004cc5e0(param_1,1,local_c,&param_1);
  if (iVar2 != 0) {
    if ((param_1 < 0x35000) || (0x37002 < param_1)) {
      st.a = 0x116;
      st.b = FUN_004d7ff0(0x80000004);
      FUN_004d8480(&st.a);
    }
    else {
      iVar2 = FUN_004cc790(uVar1,(undefined4)&local_10,4);
      if ((iVar2 != 0) && (local_10 == 9)) {
        puVar3 = (undefined4 *)(**(code **)(DAT_007d3ff8 + 0x118))(DAT_007dc518,0x30116);
        puVar4 = puVar3;
        for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
          *puVar4 = 0;
          puVar4 = puVar4 + 1;
        }
        iVar2 = FUN_004cc790(uVar1,(undefined4)puVar3,4);
        if (iVar2 != 0) {
          FUN_0052eeb0(param_2,(int)puVar3);
          return uVar1;
        }
      }
    }
  }
  return 0;
}
