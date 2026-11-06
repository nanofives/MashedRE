// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x00487d70: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs14_20261102/results.tsv. */
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
typedef int code();
MGLOBAL(int, DAT_00703058, 0x00703058);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined1, DAT_00703068, 0x00703068);
void FUN_004770a0(int p0);
undefined4 FUN_005c9d00(void);
// MATCH: 0x00487d70 FUN_00487d70
void FUN_00487d70(void)

{
  undefined4 local_4;
  
  if (DAT_00703058 != 0) {
    (**(code **)(DAT_007d3ff8 + 0x24))(0x14,&local_4);
    (**(code **)(DAT_007d3ff8 + 0x20))(0x14,1);
    (**(code **)(DAT_007d3ff8 + 0x20))(8,0);
    FUN_005c9d00(0x3e99999a);
    FUN_004770a0(&DAT_00703068);
    (**(code **)(DAT_007d3ff8 + 0x20))(8,1);
    (**(code **)(DAT_007d3ff8 + 0x20))(0x14,local_4);
    FUN_005c9d00(0);
  }
  return;
}
