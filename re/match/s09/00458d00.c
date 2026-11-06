// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00458d00: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs09_20261102/results.tsv. */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
undefined4 FUN_004576b0(void);
int FUN_00472690(int p0, int p1);
// MATCH: 0x00458d00 FUN_00458d00
undefined4 FUN_00458d00(void)

{
  int iVar1;
  int iVar2;
  undefined4 local_24 [9];
  
  local_24[0] = 0xc;
  local_24[1] = 0x13;
  local_24[2] = 0x12;
  local_24[3] = 0x11;
  local_24[4] = 0x10;
  local_24[5] = 0xb;
  local_24[6] = 10;
  local_24[7] = 9;
  local_24[8] = 7;
  iVar1 = FUN_00472690(0,8);
  iVar2 = FUN_004576b0();
  if (iVar2 != 0) {
    local_24[0] = 0xc;
    local_24[1] = 0x12;
    local_24[2] = 0x11;
    local_24[3] = 0x10;
    local_24[4] = 0xb;
    local_24[5] = 10;
    local_24[6] = 9;
    local_24[7] = 7;
    iVar1 = FUN_00472690(0,7);
    return local_24[iVar1];
  }
  return local_24[iVar1];
}
