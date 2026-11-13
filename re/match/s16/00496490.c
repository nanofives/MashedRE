// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00496490: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs16_20261102/results.tsv. */
/* ---- MATCH-S16 preamble (same text in every re/match/s16 source; not part of any function body) ----
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
/* BIND-CRT-STDIO-CALLERS 2026-11-12: callee prototypes typed so the text also compiles as C++ (the knob path's
   Match/PortTop_00496490.cpp): sprintf varargs, the 16-byte _ftime64 struct by address, char buffers, const strings.
   match.py stays 100% (verify/portorigtop_20261112/RESULT_BIND-CRT-STDIO-CALLERS.md). */
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBAL(uint, DAT_00616038, 0x00616038);
MGLOBAL(void *, DAT_00772fbc, 0x00772fbc);
MGLOBAL(char, DAT_005d0004, 0x005d0004);
void FUN_004963e0(char * p0);
int FUN_004a2b60(char * p0, const char * p1, ...);
char * FUN_004a43b9(void * p0);
void FUN_004a43d2(void * p0);
void * FUN_004a4541(const char * p0, const char * p1);
// MATCH: 0x00496490 FUN_00496490
/* [C1 2026-05-16] DAT_00772fbc = FUN_004a4541("mashed.log", &DAT_005d0004) - opens log file.
   WM_CREATE handler. */

undefined4 FUN_00496490(void)

{
  char * uVar1;
  uint unaff_retaddr;
  unsigned char local_114 [16];
  char local_104 [256];
  uint local_4;
  
  local_4 = DAT_00616038 ^ unaff_retaddr;
  DAT_00772fbc = FUN_004a4541("mashed.log",&DAT_005d0004);
  if (DAT_00772fbc == 0) {
    return 0;
  }
  FUN_004a43d2(local_114);
  uVar1 = FUN_004a43b9(local_114);
  FUN_004a2b60(local_104,"%.19s\n\n",uVar1);
  FUN_004963e0(local_104);
  return 1;
}
